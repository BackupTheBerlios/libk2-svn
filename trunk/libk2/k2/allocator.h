/*
 * Copyright (c) 2003, 2004, Kenneth Chang-Hsing Ho <kenho@bluebottle.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_ALLOCATOR_H
#define K2_ALLOCATOR_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_MEMORY_H
#   include <k2/memory.h>
#endif
#ifndef K2_FAST_LOCK_H
#   include <k2/fast_lock.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif
#ifndef K2_SINGLETON_H
#   include <k2/singleton.h>
#endif
#ifndef K2_BYTE_MANIP_H
#   include <k2/byte_manip.h>
#endif

#ifndef K2_STD_H_MEMORY
#   define  K2_STD_H_MEMORY
#   include <memory>
#endif

namespace k2
{

    /**
    *   \brief A class template to help implementing allocators.
    */
    template <typename T>
    class allocator_base
    {
    public:
        typedef T           value_type;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
        typedef const T*    const_pointer;
        typedef T*          pointer;
        typedef const T&    const_reference;
        typedef T&          reference;

        size_type   max_size () const
        {
            return  size_type(-1) / safe_alignof<sizeof(value_type)>::value;
        }
        void construct (pointer p, const_reference v)
        {
            new (p) value_type(v);
        }
        void destroy (pointer p)
        {
            p->~value_type();
        }
        const_pointer address (const_reference r)
        {
            return  &r;
        }
        pointer address (reference r)
        {
            return  &r;
        }
    };
    template <>
    class allocator_base<void>
    {
    public:
        typedef void        value_type;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
        typedef const void* const_pointer;
        typedef void*       pointer;
    };

    /**
    *   \brief A Standard Allocator-compliant class template.
    *
    *   Its first template parameter \b T has the same sematic
    *   meaning of the first template parameter of std::allocator class tmeplate.
    *
    *   Allocations made through object instances of local_allocator<> are on
    *   the stack-frame.
    *   This is probably close to the fastest allocation avaiable (if not the
    *   fastest). Only use it when you need extream performance and know what
    *   "allocation on the stack" means and its consequences.
    */
    template <typename T>
    class local_allocator
    :   public allocator_base<T>
    {
    public:
        template <typename other_T>
        struct rebind
        {
            typedef local_allocator<other_T>  other;
        };

        local_allocator () {}
        local_allocator (const local_allocator<value_type>&) {}
        template <typename other_T>
        local_allocator (const local_allocator<other_T>&) {}
        ~local_allocator () {}

        template <typename other_T>
        local_allocator& operator= (const local_allocator<other_T>&)
        {
            return  *this;
        }

        pointer allocate (size_type count, const void* hint = 0)
        {
            return  reinterpret_cast<pointer>(stack_push(sizeof(value_type) * count));
        }
        void deallocate (pointer p, size_type n)
        {
            //  no-op.
        }
    };
    template <>
    class local_allocator<void>
    :   public allocator_base<void>
    {
    public:
        template <typename other_T>
        struct rebind
        {
            typedef local_allocator<other_T>  other;
        };
    };
    template <typename lhs_T, typename rhs_T>
    bool operator== (const local_allocator<lhs_T>&, const local_allocator<rhs_T>&)
    {
        return  true;
    }
    template <typename lhs_T, typename rhs_T>
    bool operator!= (const local_allocator<lhs_T>&, const local_allocator<rhs_T>&)
    {
        return  false;
    }

#ifndef DOXYGEN_BLIND
    namespace nonpublic
    {
        template <size_t chunk_bytes_, bool threading_>
        class pool_impl
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            pool_impl ()
            :   m_allocs(0)
            ,   m_frees(0)
            {
            }

            ~pool_impl ()
            {
                chunk*  pchunk = m_allocs;
                while (pchunk)
                {
                    chunk*  pnext = pchunk->pnext;
                    delete [] reinterpret_cast<char*>(pchunk);
                    pchunk = pnext;
                }
            }

            void grow (size_t count)
            {
                scoped_guard    guard(m_lock);

                char*   raw_mem =
                    new char[count * safe_alignof<chunk_bytes_>::value];
                chunk*  pchunk = reinterpret_cast<chunk*>(raw_mem);
                pchunk->pnext = m_allocs;
                m_allocs = pchunk;

                size_t idx = 0;
                for (;
                    idx < count;
                    idx++,raw_mem += safe_alignof<chunk_bytes_>::value)
                {
                    this->deposit(raw_mem);
                }
            }
            void* allocate ()
            {
                scoped_guard    guard(m_lock);

                if (m_frees == 0)
                    return  0;

                chunk*  pchunk = m_frees;
                m_frees = m_frees->pnext;
                return  pchunk;
            }
            void deallocate (void* p)
            {
                scoped_guard    guard(m_lock);
                this->deposit(p);
            }

        private:
            void deposit (void* p)
            {
                chunk* pchunk = reinterpret_cast<chunk*>(p);
                pchunk->pnext = m_frees;
                m_frees = pchunk;
            }
            union chunk
            {
                chunk*  pnext;
            };

            size_t  m_chunk_count;
            chunk*  m_allocs;
            chunk*  m_frees;

            typedef typename fast_lock<threading_>::scoped_guard
                scoped_guard;
            fast_lock<threading_>   m_lock;
        };
    }   //  namespace nonpublic
#endif  //  !DOXYGEN_BLIND

    /**
    *   \brief A memory pool class.
    *
    *   An object instance of \b static_pool<> acquires a block of memory, then
    *   divids it into number of \b chunk_count_ of chunks of memory those are
    *   of size \b chunk_bytes_ bytes on construction.
    *   Throws when it ran out of free chunks for user allocation.
    *   Memory acquired by an object instances of static_pool<> is released on
    *   destruction.
    *   Object instances of \b static_pool<> don't have value sematic.
    */
    template <
        size_t  chunk_bytes_,
        bool    threading_ = true>
    class static_pool
    {
    private:
        nonpublic::pool_impl<chunk_bytes_, threading_>   m_impl;

    public:
        K2_INJECT_COPY_BOUNCER();

        explicit static_pool (size_t init_chunk_count)
        {
            m_impl.grow(init_chunk_count);
        }

        void* allocate (size_t bytes)
        {
            if (bytes > chunk_bytes_)
                throw   std::bad_alloc();

            void*   ret = m_impl.allocate();
            if (ret == 0)
                throw   std::bad_alloc();

            return  ret;
        }
        void deallocate (void* p, size_t bytes)
        {
            m_impl.deallocate(p);
        }
    };

    /**
    *   \brief A memory pool class.
    *
    *   An object instance of \b dynamic_pool<> acquires a block of memory, then
    *   divids it into number of \b grow_count_ of chunks of memory those are
    *   of size \b chunk_bytes_ bytes when it ran out of free chunks
    *   for user allocation.
    *   Memory acquired by an object instance of dynamic_pool<> is released on
    *   destruction.
    *   Object instances of \b dynamic_pool<> don't have value sematic.
    */
    template <
        size_t  chunk_bytes_,
        bool    threading_ = true>
    class dynamic_pool
    {
    private:
        nonpublic::pool_impl<chunk_bytes_, threading_>   m_impl;

    public:
        K2_INJECT_COPY_BOUNCER();

        explicit dynamic_pool (size_t grow_count)
        :   m_grow_count(grow_count)
        {
        }

        void* allocate (size_t bytes)
        {
            if (bytes > chunk_bytes_)
                throw   std::bad_alloc();

            void*   ret = m_impl.allocate();
            if (ret == 0)
            {
                m_impl.grow(m_grow_count);
                return  m_impl.allocate();
            }

            return  ret;
        }
        void deallocate (void* p, size_t bytes)
        {
            m_impl.deallocate(p);
        }

    private:
        size_t  m_grow_count;
    };

#ifndef DOXYGEN_BLIND
    namespace nonpublic
    {
        /**
        *   \brief An helper class template.
        *
        *   Helper class used by \b pool_allocator<>
        */
        template <
            size_t  pool_init_arg_,
            template <size_t, bool> class pool_tmpl_,
            size_t  pool_chunk_size_,
            bool    threading_>
        class pool_sharing_helper
        {
        private:
            struct pool_init_helper
            {
                typedef pool_tmpl_<
                    safe_alignof<pool_chunk_size_>::value,
                    threading_>
                    pool_type;

                pool_init_helper ()
                :   m_pool(pool_init_arg_) {}

                pool_type   m_pool;
            };

        public:
            static pool_init_helper::pool_type&  instance ()
            {
                typedef class_singleton<
                    pool_sharing_helper,
                    pool_init_helper>
                    shared_mem_pool;
                return  shared_mem_pool::instance().m_pool;
            }
        };
    }   //  namespace nonpublic
#endif  //  !DOXYGEN_BLIND

    /**
    *   \brief A Standard Allocator-compliant class template.
    *
    *   Object instances try to share the same underlying singleton instance
    *   of pool_tmpl_<pool_init_arg_,
    *   safe_alignof<pool_chunk_size_>::value, threading_>.
    *   They do so by not considering T and compile-time compute
    *   safe_alignof<pool_chunk_size_>::value, which made sure where
    *   T s compute the same alignment could share the same pool
    *   object instance.
    *
    *   Its first parameter, a template parameter \b T has the same
    *   sematic meaning of the first template parameter of std::allocator
    *   class tmeplate.
    *
    *   Its second parameter, a template template parameter \b pool_type_
    *   specifis the underlying pool implementation, which determines its
    *   allocation behavior, i.e. a class instanciation of \b static_pool<> or
    *   \b dynamic_pool<>.
    *
    *   Its third to fifth template parameters are used to instantiate class
    *   instances of pool_tmpl_<>.
    */
    template <
        typename T,
        size_t  pool_init_arg_,
        template <size_t, bool> class pool_tmpl_,
        size_t  pool_chunk_size_ = sizeof(value_type),
        bool    threading_ = true>
    class pool_allocator
    :   public allocator_base<T>
    {
    private:
        typedef pool_allocator<
            value_type,
            pool_init_arg_,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>
            self_type;

        typedef nonpublic::pool_sharing_helper<
            pool_init_arg_,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>
            shared_mem_pool;

    public:
        template <typename other_T>
        struct rebind
        {
            typedef pool_allocator<
                other_T,
                pool_init_arg_,
                pool_tmpl_,
                pool_chunk_size_,
                threading_>
                other;
        };

        pool_allocator () {}
        pool_allocator (const self_type&) {}
        template <typename other_T>
        pool_allocator (const typename rebind<other_T>::other&) {}
        ~pool_allocator () {}

        template <typename other_T>
        pool_allocator& operator= (const typename rebind<other_T>::other&)
        {
        }

        pointer allocate (size_type count, const void* hint = 0)
        {
            return  reinterpret_cast<pointer>(shared_mem_pool::instance().allocate(count * sizeof(value_type)));
        }
        void deallocate (pointer p, size_type count)
        {
            shared_mem_pool::instance().deallocate(p, count * sizeof(value_type));
        }
    };
    template <
        size_t  pool_init_arg_,
        template <size_t, size_t, bool> class pool_tmpl_,
        size_t  pool_chunk_size_,
        bool    threading_>
    class pool_allocator<
        void,
        pool_init_arg_,
        pool_tmpl_,
        pool_chunk_size_,
        threading_>
    :   public allocator_base<void>
    {
    public:
        template <typename other_T>
        struct rebind
        {
            typedef pool_allocator<
                other_T,
                pool_init_arg_,
                pool_tmpl_,
                pool_chunk_size_,
                threading_>
                other;
        };
    };
    template <
        typename lhs_T,
        typename rhs_T,
        size_t  pool_init_arg_,
        template <size_t, size_t, bool> class pool_tmpl_,
        size_t  pool_chunk_size_,
        bool    threading_>
    bool operator== (
        const pool_allocator<
            lhs_T,
            pool_init_arg_,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>&,
        const pool_allocator<
            rhs_T,
            pool_init_arg_,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>&)
    {
        return  true;
    }
    template <
        typename lhs_T,
        typename rhs_T,
        size_t  pool_init_arg_,
        template <size_t, size_t, bool> class pool_tmpl_,
        size_t  pool_chunk_size_,
        bool    threading_>
    bool operator!= (
        const pool_allocator<
            lhs_T,
            pool_init_arg_,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>&,
        const pool_allocator<
            rhs_T,
            pool_init_arg_,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>&)
    {
        return  false;
    }

    /**
    *   \brief A Standard Allocator-compliant class template.
    *
    *   Object instances allocate memory through the pool instance user
    *   specified on construction. Supports ALL semantics supported by
    *   std::allocator.
    *   Note that comparison of object instances of class instances with the
    *   same \b pool_type_ are just wrapper of comparison of user specified pool
    *   instances.
    *
    *   Its first parameter, a template parameter \b T has the same
    *   sematic meaning of the first template parameter of std::allocator
    *   class tmeplate.
    *
    *   Its second parameter, a template template parameter \b pool_type_
    *   specifis the underlying pool implementation, which determines its
    *   allocation behavior, i.e. a class instanciation of \b static_pool<> or
    *   \b dynamic_pool<>.
    *
    *   Its third to fifth template parameters are used to instantiate class
    *   instances of pool_tmpl_<>.
    */
    template <
        typename T,
        template <size_t, bool> class pool_tmpl_,
        size_t  pool_chunk_size_ = sizeof(value_type),
        bool    threading_ = true>
    class pool_allocator_adapter
    :   public allocator_base<T>
    {
    private:
        typedef pool_allocator_adapter<
            value_type,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>
            self_type;
        typedef pool_tmpl_<pool_chunk_size_, threading_>
            pool_type;
        pool_type* m_pool;

    public:
        template <typename other_T>
        struct rebind
        {
            typedef pool_allocator_adapter<
                other_T,
                pool_tmpl_,
                pool_chunk_size_,
                threading_>
                other;
        };

        pool_allocator_adapter (pool_type& pool)
        :   m_pool(&pool) {}
        pool_allocator_adapter (const self_type& rhs)
        :   m_pool(&rhs.get_pool()) {}
        template <typename other_T>
        pool_allocator_adapter (const typename rebind<other_T>::other&)
        :   m_pool(&rhs.get_pool()) {}

        template <typename other_T>
        pool_allocator_adapter& operator= (const typename rebind<other_T>::other& rhs)
        {
            m_pool = &rhs.get_pool();
        }

        pointer allocate (size_type count, const void* hint = 0)
        {
            return  reinterpret_cast<pointer>(m_pool->allocate(count * sizeof(value_type)));
        }
        void deallocate (pointer p, size_type count)
        {
            m_pool->deallocate(p, count * sizeof(value_type));
        }

        const pool_type& get_pool () const
        {
            return  *m_pool;
        }
        pool_type& get_pool ()
        {
            return  *m_pool;
        }
    };
    template <
        template <size_t, size_t, bool> class pool_tmpl_,
        size_t  pool_chunk_size_,
        bool    threading_>
    class pool_allocator_adapter<
        void,
        pool_tmpl_,
        pool_chunk_size_,
        threading_>
    :   public allocator_base<void>
    {
    public:
        template <typename other_T>
        struct rebind
        {
            typedef pool_allocator_adapter<
                other_T,
                pool_tmpl_,
                pool_chunk_size_,
                threading_>
                other;
        };
    };
    template <
        typename lhs_T,
        typename rhs_T,
        template <size_t, size_t, bool> class pool_tmpl_,
        size_t  pool_chunk_size_,
        bool    threading_>
    bool operator== (
        const pool_allocator_adapter<
            lhs_T,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>& lhs,
        const pool_allocator_adapter<
            rhs_T,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>& rhs)
    {
        return  &lhs.get_pool() == &rhs.get_pool();
    }
    template <
        typename lhs_T,
        typename rhs_T,
        template <size_t, size_t, bool> class pool_tmpl_,
        size_t  pool_chunk_size_,
        bool    threading_>
    bool operator!= (
        const pool_allocator_adapter<
            lhs_T,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>& lhs,
        const pool_allocator_adapter<
            rhs_T,
            pool_tmpl_,
            pool_chunk_size_,
            threading_>& rhs)
    {
        return  !(lhs == rhs);
    }

    template <
        typename T,
        size_t  pool_chunk_size_,
        bool    threading_>
    pool_allocator_adapter<
        T,
        static_pool,
        pool_chunk_size_,
        threading_>
    make_allocator (
        static_pool<
            pool_chunk_size_,
            threading_>& pool)
    {
        return  pool_allocator_adapter<
            T,
            static_pool,
            pool_chunk_size_,
            threading_>(pool);
    }
    template <
        typename T,
        size_t  pool_chunk_size_,
        bool    threading_>
    pool_allocator_adapter<
        T,
        dynamic_pool,
        pool_chunk_size_,
        threading_>
    make_allocator (
        dynamic_pool<
            pool_chunk_size_,
            threading_>& pool)
    {
        return  pool_allocator_adapter<
            T,
            dynamic_pool,
            pool_chunk_size_,
            threading_>(pool);
    }

#if(0)

    template <typename char_type_>
    class static_char_traits;

    template <>
    class static_char_traits<char>
    {
    public:
        typedef char_type_  char_type;
        typedef
    };

    class static_string_a


    template <bool thread_safe_>
    class mem_pool
    {
    public:
        template <typename T>
        class allocator_adapter
        :   public allocator_base<T>
        {
        public:
            template <typename other_>
            struct rebind
            {
                typedef allocator_adapter<other_>   other;
            };
            allocator_adapter ()
            {
            }
            explicit allocator_adapter (mem_pool& pool)
            :   m_pool_ptr(&pool)
            {
            }
            allocator_adapter (const allocator_adapter& alloc)
            :   m_pool_ptr(&m_alloc.pool())
            {
            }
            pointer allocate (size_type n, void* /*hint*/)
            {
                if(m_pool_ptr == 0)
                    throw   std::bad_alloc("No memory pool");
                if(m_pool_ptr == 0 || m_pool_ptr->chunk_bytes() < n * sizeof(value_type))
                    throw   std::bad_alloc("Allocation too large for memory pool");
                pointer p = reinterpret_cast<pointer>(m_pool_ptr.allocate();
                if(p == 0)
                    throw   std::bad_alloc("Memory pool exhausted");
                return  p;
            }
            template <typename other_>
            allocator_adapter&  operator = (const allocator_adapter<other_>& alloc)
            {
                m_pool_ptr = &alloc.pool();
                return  *this;
            }
            void    deallocate (pointer p, size_type /*n*/)
            {
                assert(m_pool_ptr);
                m_pool_ptr->deallocate(p);
            }
            size_type   max_size () const
            {
                return  (m_pool_ptr ? m_pool_ptr->chunk_bytes() / sizeof(value_type) : 0);
            }

        protected:
            mem_pool&   pool () const
            {
                return  *m_pool;
            }
        private:
            mem_pool<thread_safe_>* m_pool_ptr;
        };

        template <typename T>
        class allocator
        :   public allocator_base<T>
        {
        public:
            template <size_t ChunkCount>
            struct instance;
            struct adapter;
        };
        template <size_t Chunks>
        struct transform
        {
            template <typename T>
            class allocator
            :   public allocator_base<T>
            {
            public:
                template <typename other_>
                struct rebind
                {
                    typedef allocator<other_>   other;
                };
                allocator ()
                :   m_pool_ptr(0)
                {
                }
                allocator (const allocator& alloc)
                :   m_pool_ptr(&m_alloc.pool())
                {
                }
                explicit allocator (mem_pool& pool)
                :   m_pool_ptr(&pool)
                {
                }
                pointer allocate (size_type n, void* /*hint*/)
                {
                    if(m_pool_ptr == 0)
                        throw   std::bad_alloc("No memory pool");
                    if(m_pool_ptr == 0 || m_pool_ptr->chunk_bytes() < n * sizeof(value_type))
                        throw   std::bad_alloc("Allocation too large for memory pool");
                    pointer p = reinterpret_cast<pointer>(m_pool_ptr.allocate();
                    if(p == 0)
                        throw   std::bad_alloc("Memory pool exhausted");
                    return  p;
                }
                template <typename other_>
                allocator&  operator = (const allocator<other_>& alloc)
                {
                    m_pool_ptr = &alloc.pool();
                    return  *this;
                }
                void    deallocate (pointer p, size_type /*n*/)
                {
                    assert(m_pool_ptr);
                    m_pool_ptr->deallocate(p);
                }
                size_type   max_size () const
                {
                    return  (m_pool_ptr ? m_pool_ptr->chunk_bytes() / sizeof(value_type) : 0);
                }

            protected:
                mem_pool&   pool () const
                {
                    return  m_pool;
                }
            private:
                mem_pool<thread_safe_>  m_pool;
            };
        };
        template <>
        class allocator<void>
        :   public allocator_base<void>
        {
        public:
            template <typename other_>
            struct rebind
            {
                typedef allocator<other_>   other;
            };
        };


        K2_INJECT_COPY_BOUNCER();

        mem_pool (size_t chunk_count, size_t chunk_bytes);
        :   m_mem(0)
        ,   m_frees(0)
        ,   m_chunk_bytes(chunk_bytes)
        {
            size_t  safe_align = get_safe_align(chunk_bytes);
            m_mem = new char[chunk_count * safe_align];

            size_t idx = 0;
            for (; idx < chunk_count; idx++)
                this->deposit(m_mem + idx * safe_align);
        }

        ~mem_pool ()
        {
            delete  [] m_mem;
        }

        void* allocate ()
        {
            scoped_guard    guard(m_lock);

            if (m_frees == 0)
                return  0;

            chunk*  pchunk = m_frees;
            m_frees = m_frees->pnext;
            return  pchunk;
        }
        void deallocate (void* p)
        {
            scoped_guard    guard(m_lock);
            this->deposit(p);
        }
        size_t  chunk_bytes () const
        {
            return  m_chunk_bytes;
        }

    private:
        void deposit (void* p)
        {
            chunk* pchunk = reinterpret_cast<chunk*>(p);
            pchunk->pnext = m_frees;
            m_frees = pchunk;
        }
        union chunk
        {
            chunk*  pnext;
        };

        typedef fast_lock<thread_safe_>     lock;
        typedef typename lock::scoped_guard scoped_guard;
        lock    m_lock;
        size_t  m_chunk_count;
        char*   m_mem;
        chunk*  m_frees;

    };


    //template <size_t chunk_count_, size_t chunk_size_, typename instance_tag_ = global_tag>
    template <size_t chunk_count_, size_t chunk_bytes_, typename instance_tag_ = global_tag>
    class shared_mem_pool
    {
    private:
        struct pool
        {
            pool ()
            :   shared(chunk_count_, chunk_bytes_)
            {
            }
            mem_pool    shared;
        };
        shared
        process_singleton<shared>    process_shared;
    public:

        template <template <> class singleton_tmpl_>
        class shared
        {
        public:
            static const pool&  instance () const;
            static pool&        instance ();

            template <typename T>
            class allocator
            {
            public:
                typedef T value_type;

                pointer allocate (size_type n, void* /*hint*/ = 0)
                {
                    return  reinterpret_cast<pointer>(instance().allocate(n * sizeof(value_type)));
                }
                void    deallocate (pointer p, size_type n)
                {
                    instance().deallocate(p, n * sizeof(value_type));
                }
        };

        template <typename T>
        class allocator
        {
        public:
            typedef T value_type;

            pointer allocate (size_type n, void* /*hint*/ = 0)
            {
                return  reinterpret_cast<pointer>(shared_instance().allocate(n * sizeof(value_type)));
            }
            void    deallocate (pointer p, size_type n)
            {
                shared_instance().deallocate(p, n * sizeof(value_type));
            }
        };
    };

    template <typename base_, typename alloc_>
    //template <typename base_, template <> alloc_tmpl_>
    //template <typename base_>
    struct object_factory
    {
    public:
        typedef base_   base_type;
        typedef base_*  base_pointer;
        typedef alloc_  allocator_type;

        object_factory ();
        ~object_factory ();

        template <typename derived_>
        base_pointer    create ()
        {
            alloc_tmpl_<derived_>
            return  new derived_;
        }
        template <typename derived_, typename arg_>
        base_pointer    create (arg_ v)
        {
            return  new derived_(v);
        }
        void            destroy (base_pointer p)
        {
            delete  p;
        }

    private:
        allocator_type  m_alloc;
    };

    template <typename value_>
    struct noop_destroy_policy
    {
        void opeartor (value_)
        {
        }
    };
    template <typename value_>
    struct delete_destroy_policy
    {
        void opeartor (value_ v)
        {
            delete v;
        }
    };

#endif

}   //  namespace k2

#endif  //  !K2_ALLOCATOR_H
