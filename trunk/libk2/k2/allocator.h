/*  libk2   <k2/allocator.h>

    Copyright (C) 2003, 2004 Kenneth Chang-Hsing Ho.

    Written by Kenneth Chang-Hsing Ho <kenho@bluebottle.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef K2_ALLOCATOR_H
#define K2_ALLOCATOR_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_MEMORY_H
#   include <k2/memory.h>
#endif
#ifndef K2_SPIN_LOCK_H
#   include <k2/spin_lock.h>
#endif
#ifndef K2_DUMMY_LOCK_H
#   include <k2/dummy_lock.h>
#endif
#ifndef K2_SCOPE_H
#   include <k2/scope.h>
#endif

#if !defined(K2_STACK_ALLOCATOR_MAX_ALLOCATE)
#   define K2_STACK_ALLOCATOR_MAX_ALLOCATE  (32)
#endif

namespace k2
{

#ifndef DOXYGEN_BLIND
    namespace nonpublic
    {

        struct op_new_mem_mgr
        {
            static void* allocate (size_t bytes)
            {
                return ::operator new(bytes);
            }

            static void deallocate (void* p, size_t bytes)
            {
                ::operator delete(p, bytes);
            }
        };

        struct malloc_mem_mgr
        {
            static void* allocate (size_t bytes)
            {
                return  ::malloc_mem_mgr(bytes);
            }

            static void deallocate (void* p, size_t /*unsued_bytes*/)
            {
                ::free(p);
            }
        };

        struct stack_mem_mgr
        {
            static void* allocate (size_t bytes)
            {
                k2::alloc(bytes);
            }

            static void deallocate (void* /*unused_p*/, size_t /*unused_bytes*/)
            {
                //  Automatically deallocates when stepping up in stack frame.
            }
        };

#if defined(K2_DEFAULT_MALLOC_ALLOCATOR)
        typedef malloc_mem_mgr      default_mem_mgr;
#else
        typedef op_new_mem_mgr      default_mem_mgr;
#endif

        template <int instance_id_, bool threading_>
        struct fast_mem_mgr
        {
        private:
            typedef type_select<threading_, spin_lock, dummy_lock>::type    lock_type;
            typedef typename lock_type::scoped_guard    scoped_guard;
            static const size_t max_chunk_bytes = 128;
            static const size_t new_req_bytes = 512;
            static const size_t alignment = 8;
            static const size_t free_list_cnt = max_chunk_bytes / alignment;

            union chunk
            {
                char    ref[1];
                chunk*  pnext;
            };

            static lock_type    m_lock;
            static chunk*       m_free_chunk_map[free_list_cnt];
            static chunk*       m_mem_blks;
            static char*        m_heap_begin;
            static char*        m_heap_end;

        public:

            static void* allocate (size_t bytes)
            {
                //  Allocate from C++ runtime's memory manager if bytes is too big.
                if (K2_OPT_BRANCH_FALSE(bytes > max_chunk_bytes))
                    return  op_new_mem_mgr::allocate(bytes);

                size_t  rounded_up_bytes = round_up(bytes);
                size_t  key = free_chunk_key(bytes);

                scoped_guard    guard(m_lock);

                chunk*  pchunk = m_free_chunk_map[key];

                if (pchunk == 0)
                {
                    pchunk = alloc_from_heap(rounded_up_bytes);
                    if (pchunk == 0)
                    {
                        pchunk = alloc_from_other_chunks(rounded_up_bytes);
                        if (pchunk == 0)
                        {
                            pchunk = alloc_from_op_new(rounded_up_bytes);
                        }
                    }
                    return  pchunk;
                }

                m_free_chunk_map[key] = pchunk->pnext;
                return pchunk;
            }
            static void deallocate (void* p, size_t bytes)
            {
                //  Returns back to C++ runtime's memory manager if bytes is too big.
                if (K2_OPT_BRANCH_FALSE(bytes > max_chunk_bytes))
                {
                    op_new_mem_mgr::deallocate(bytes);
                    return;
                }

                size_t  rounded_up_bytes = round_up(bytes);
                size_t  key = free_chunk_key(bytes);

                scoped_guard    guard(m_lock);

                //  Push back to our free chunk map.
                chunk*& plist = m_free_chunk_map[key];
                chunk*  pchunk = reinterpret_cast<chunk*>(p);

                pchunk->pnext = plist;
                plist = pchunk;
            }

        private:
            static size_t   round_up (size_t bytes)
            {
                return  (bytes + (size_t)alignment-1) &
                    ~((size_t)alignment-1);
            }
            static size_t   free_chunk_key (size_t bytes)
            {
                return  (bytes + (size_t)alignment-1) /
                    ((size_t)alignment-1);
            }
            static chunk* alloc_from_heap (size_t rounded_up_bytes)
            {
                size_t  heap_bytes = m_heap_end - m_heap_begin;

                //  Try to fullfill the request from our heap.
                if (K2_OPT_BRANCH_TRUE(heap_bytes > rounded_up_bytes))
                {
                    chunk*  pret = reinterpret_cast<chunk*>(m_heap_begin);
                    m_heap_begin += rounded_up_bytes;
                    return  pret;
                }
                return  0;
            }
            static chunk* alloc_from_other_chunks (size_t rounded_up_bytes)
            {
                size_t bytes_cnt = rounded_up_bytes;

                //  Try to find the next free (and bigger) chunk
                for (;bytes_cnt <= max_chunk_bytes; bytes_cnt += alignment)
                {
                    size_t key = free_chunk_key(bytes_cnt);
                    chunk*  pchunk = m_free_chunk_map[key];

                    //  If found.
                    if (K2_OPT_BRANCH_FALSE(pchunk != 0))
                    {
                        //  aux_bytes is always rounded up.
                        size_t aux_bytes = bytes_cnt - rounded_up_bytes;
                        K2_DEBUG_ASSERT(aux_bytes == round_up(aux_bytes));

                        //  Push aux part of mem to free chunk map,
                        //  then return requested size.
                        key = free_chunk_key(aux_bytes);
                        chunk* paux =
                            reinterpret_cast<chunk*>(&pchunk->ref[bytes_cnt]);
                        paux->pnext = m_free_chunk_map[key];
                        m_free_chunk_map[key] = paux;

                        return  pchunk;
                    }
                }
                return  0;
            }
            static chunk* alloc_from_op_new (size_t rounded_up_bytes)
            {
                //  Allocate a block of raw memory from operator new.
                char* pnew = reinterpret_cast<char*>(
                    op_new_mem_mgr::allocate(new_req_bytes));

                size_t  heap_bytes = m_heap_end - m_heap_begin;
                //  Under legi usage, this should never happend.
                K2_DEBUG_ASSERT(heap_bytes > max_chunk_bytes);

                //  Push remaining heap to free chunk map.
                chunk*  pchunk = reinterpret_cast<chunk*>(m_heap_bytes);
                size_t  key = free_chunk_key(rounded_up_bytes);

                pchunk->pnext = m_free_chunk_map[key];
                m_free_chunk_map[key] = pchunk;

                //  Set heap to tailing aux part.
                m_heap_begin = pnew + rouned_up_bytes;
                m_heap_end = new_req_bytes - rouned_up_bytes

                //  Return requested bytes of memory.
                pchunk = reinterpret_cast<chunk*>(pnew);
                return  pchunk;
            }
        };


        template <int instancc_id_, bool threading_>
        class pool_mem_mgr
        {
        public:
            static void* allocate (size_t bytes)
            {
            }
            static void deallocate (void* p, size_t bytes)
            {
            }
        private:
        };

    }
#endif  //  !DOXYGEN_BLIND

    template <typename value_t_>
    struct allocator_base
    {
        typedef size_t          size_type;
        typedef ptrdiff_t       difference_type;
        typedef value_t_*       pointer;
        typedef const value_t_* const_pointer;
        typedef value_t_&       reference;
        typedef const value_t_& const_reference;
        typedef value_t_        value_type;

        pointer address (reference r) const
        {
            return  &r;
        }
        const_pointer address (const_reference r) const
        {
            return  &r;
        }
        void    construct (pointer p, const_reference v)
        {
            k2::construct(p, v);
        }
        void    destroy (pointer p)
        {
            k2::destroy(p);
        }

        const static size_type  value_size = sizeof(value_type);
    };

    template <>
    struct allocator_base<void>
    {
        typedef size_t          size_type;
        typedef ptrdiff_t       difference_type;
        typedef value_t_*       pointer;
        typedef const value_t_* const_pointer;
    };

    template <typename type_, typename mem_mgr_>
    class allocator_tmpl : public nonpublic::allocator_base<type_>
    {
    private:
        typedef mem_mgr_    mem_mgr_t;
        mem_mgr_t           m_mem_mgr;

    public:
        template <typename other_t_>
        struct rebind
        {
            typedef allocator_tmpl<other_t_>    other;
        };

        allocator_tmpl () {}
        allocator_tmpl (const allocator_tmpl& /*unused_rhs*/) {}
        template <typename other_t_>
        stack_allocator (const allocator_tmpl<other_t_>& /*unused_rhs*/) {}

        pointer allocate (size_type n, const void* /*unsed_hint*/= 0)
        {
            return m_mem_mgr.allocate(n);
        }
        void    deallocate (pointer p, size_t n)
        {
            return m_mem_mgr.deallocate(p, n);
        }
        size_type   max_size () const
        {
            return  m_mem_mgr.max_size ();
        }
    };

    template <typename type_>
    bool operator== (
        const allocator_tmpl<type_>& /*unused_lhs*/,
        const allocator_tmpl<type_>& /*unused_rhs*/)
    {
        return  true;
    }
    template <typename type_>
    bool operator!= (
        const allocator_tmpl<type_>& /*unused_lhs*/,
        const allocator_tmpl<type_>& /*unused_rhs*/)
    {
        return  false;
    }

    template <typename type_>
    struct default_allocator
    {
        typedef allocator_tmpl<type_, nonpublic::default_mem_mgr>   type;
    };

    template <typename type_>
    struct new_allocator
    {
        typedef allocator_temp<type_, nonpublic::op_new_mem_mgr>    type;
    }

    template <typename type_>
    struct malloc_allocator
    {
        typedef allocator_temp<type_, nonpublic::malloc_mem_mgr>    type;
    }

    template <typename type_>
    struct stack_allocator
    {
        typedef allocator_temp<type_, nonpublic::stack_mem_mgr> type;
    }

    template <typename type_, bool share>
    class recyclable_allocator
    {
    public:

    private:
    };


    template <typename type_>
    struct fast_allocator
    {
        class type : public allocator_base<type_>
        {
        private:
            typedef nonpublic::default_mem_mgr  mem_mgr_t;


        public:
            template <typename other_t_>
            struct rebind
            {
                typedef allocator_tmpl<other_t_>    other;
            };

            allocator_tmpl () {}
            allocator_tmpl (const allocator_tmpl& /*unused_rhs*/) {}
            template <typename other_t_>
            stack_allocator (const allocator_tmpl<other_t_>& /*unused_rhs*/) {}

            pointer allocate (size_type n, const void* /*unsed_hint*/= 0)
            {
                return mem_mgr_t::allocate(n);
            }
            void    deallocate (pointer p, size_t n)
            {
                return mem_mgr_t::deallocate(p, n);
            }
            size_type   max_size () const
            {
                return  size_type(-1) / value_size;
            }

        private:
        };
    };



    template <bool reentrant_>
    class mem_pool
    {
    private:
        size_t      m_pool_size;
        size_t      m_req_blk_bytes;
        size_t      m_next_blk_bytes;
        char*       m_pblks;
        char*       m_phead;
        lock_type   m_lock;

        //  Disable copy.
        mem_pool (const mem_pool&);
        //  Disable assignment.
        mem_pool& operator= (const mem_pool&);

        static char*&   next (char* p)
        {
            return  *(char**(p));
        }

    public:
        typedef lock_t_                 lock_type;
        typedef scoped_guard<lock_type> guard_type;

        mem_pool ()
        :   m_pool_size(0)
        ,   m_req_blk_bytes(0)
        ,   m_next_blk_bytes(0)
        ,   m_pblks(0)
        ,   m_phead(0)
        ,   m_used_size(0)
        {}
        mem_pool (size_type pool_size, size_t req_blk_bytes)
        :   m_pool_size(pool_size)
        ,   m_req_blk_bytes(req_blk_bytes),
        ,   m_next_blk_bytes(((req_blk_bytes - 1/4) + 1) * 4)
        ,   m_pblks(reinterpret_cast<char*>(::operator new(m_pool_size * m_next_blk_bytes)))
        ,   m_phead(m_pblks)
        ,   m_used_size(0)
        {
            size_type   pool_size_minus_1 = pool_size - 1;
            char*       p = m_pblks;
            for( size_type idx = 0; idx < pool_size_minus_1; ++idx)
            {
                next(p) = p + m_next_blk_bytes;
            }
            next(p) = 0;
        }
        ~mem_pool ()
        {
            ::operator delete(m_pblks, m_pool_size * m_next_blk_bytes);
        }

        void*   allocate (size_type n)
        {
            guard_type  g(m_lock);

            if (m_phead == 0)
                throw   std::bad_alloc("Pool is empty");
            if (n > m_req_blk_bytes)
                throw   std::bad_alloc("Memory blocks in pool are too small");

            char*   p = m_phead;
            m_phead = next(m_phead);

            ++m_used_size;
            return  p;
        }
        void    deallocate (void* ptr)
        {
            char*   p = reinterpret_cast<char*>(ptr);

            K2_DEBUG_ASSERT(p >= m_pblks);
            K2_DEBUG_ASSERT(p <=    m_pblks + (m_pool_size - 1) * m_next_blk_bytes);
            K2_DEBUG_ASSERT((p - m_pblks) % m_next_blk_bytes == 0);

            guard_type  g(m_lock);
            *(char**(p)) = m_phead;
            m_phead = p;
            --m_used_size;
        }

        size_t  used_size () const
        {
            return  m_used_size;
        }
        size_t  avail_size () const
        {
            return  m_pool_size - m_used_size;
        }
    };


    template <typename value_t_, lock_t_>
    class pool_allocator
    :   public alloc_impl_tracker<value_t_>
    {
    public:
        typedef lock_t_             lock_type;
        typedef mem_pool<lock_t_>   pool_type;

        pool_allocator (pool_type& pool)
        :   m_ppool(&pool) {}
        template <typename other_t_>
        pool_allocator (const pool_allocator<other_t_, lock_type>& rhs)
        :   m_ppool(&rhs.pool()) {}

        template <typename other_t_>
        rebind
        {
            typedef pool_allocator<other_t_, lock_t_>   other;
        };

        pointer allocate (size_type n, pointer /*hint_not_used*/ = 0)
        {
            return  m_ppool->allocate(n);
        }
        void    deallocate (pointer p, size_type n)
        {
            m_ppool->deallocate(p);
        }
        size_type   max_size () const
        {
            return  m_ppool->avail_size();
        }

        pool_type&  pool ()
        {
            return  *m_ppool;
        }
        const pool_type&    pool () const
        {
            return  *m_ppool;
        }

    private:
        pool_type*  m_ppool;
    };

    template <typename lhs_value_t_, typename lhs_lock_t_, typename rhs_value_t_, typename rhs_lock_t_>
    bool    operator== (const pool_allocator<lhs_value_t_, lhs_lock_t_>& lhs, const pool_allocator<rhs_value_t_, rhs_lock_t_>& rhs)
    {
        return  &lhs.pool() == &rhs.pool();
    }
    template <typename lhs_value_t_, typename lhs_lock_t_, typename rhs_value_t_, typename rhs_lock_t_>
    bool    operator!= (const pool_allocator<lhs_value_t_, lhs_lock_t_>& lhs, const pool_allocator<rhs_value_t_, rhs_lock_t_>& rhs)
    {
        return  !(lhs == rhs);
    }

}   //  namespace k2

#if !defined(DOXYGEN_BLIND)
#   undef   nonpublic
#endif  //  !DOXYGEN_BLIND

#endif  //  !K2_ALLOCATOR_H





#if(0)

class base;
class derived : public base;


template <typename alloc_>
class string;


void foo()
{
    derived     tmp;

    allocator<derived>  alloc;
    derived*    pd = alloc.allocate(2);

    alloc.construct(pd, tmp);
    new (pd) derived;
    new (pd + 1) derived();

    pd->~derived();

    alloc.deallocate(pd, 2);

    string<std_allocator>*  pstr1 = new string<std_allocator>;

    string<stack_allocator> str2;

    class data;

    data d(new char[4], new char[9]);

}

#endif
