/*
 * Copyright (c) 2003, 2004, 2005,
 * Kenneth Chang-Hsing Ho <kenho@bluebottle.com> All rights reterved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of k2, libk2, copyright owners nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT OWNERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_BITS_AUTO_ALLOC_H
#define K2_BITS_AUTO_ALLOC_H

#ifndef K2_BITS_FAST_LOCK_H
#   include <k2/bits/fast_lock.h>
#endif
#ifndef K2_BITS_COPY_BOUNCER_H
#   include <k2/bits/copy_bouncer.h>
#endif
#ifndef K2_BITS_DEFALLC_BASE_H
#   include <k2/bits/defalloc_base.h>
#endif
#ifndef K2_CLASS_SINGLETON_H
#   include <k2/bits/class_singleton.h>
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

#ifndef DOXYGEN_BLIND
    namespace nonpublic
    {
        template <size_t ChunkBytes, bool ThreadSafe>
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
                    new char[count * safe_alignof::constant<ChunkBytes>::value];
                chunk*  pchunk = reinterpret_cast<chunk*>(raw_mem);
                pchunk->pnext = m_allocs;
                m_allocs = pchunk;

                size_t idx = 0;
                for (;
                    idx < count;
                    idx++,raw_mem += safe_alignof::constant<ChunkBytes>::value)
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

            typedef typename fast_lock<ThreadSafe>::type    lock_type;
            typedef typename lock_type::scoped_guard        scope_guard;
            lock_type   m_lock;
        };
    }   //  namespace nonpublic
#endif  //  !DOXYGEN_BLIND

    /**
    *   \brief A memory pool class.
    *
    *   An object instance of \b static_pool<> acquires a block of memory, then
    *   divids it into number of \b chunk_count_ of chunks of memory those are
    *   of size \b ChunkBytes bytes on construction.
    *   Throws when it ran out of free chunks for user allocation.
    *   Memory acquired by an object instances of static_pool<> is released on
    *   destruction.
    *   Object instances of \b static_pool<> don't have value sematic.
    */
    template <
        size_t  ChunkBytes,
        bool    ThreadSafe = true>
    class static_pool
    {
    private:
        nonpublic::pool_impl<ChunkBytes, ThreadSafe>    m_impl;

    public:
        K2_INJECT_COPY_BOUNCER();

        explicit static_pool (size_t init_chunk_count)
        {
            m_impl.grow(init_chunk_count);
        }

        void* allocate (size_t bytes)
        {
            if (bytes > ChunkBytes)
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
    *   of size \b ChunkBytes bytes when it ran out of free chunks
    *   for user allocation.
    *   Memory acquired by an object instance of dynamic_pool<> is released on
    *   destruction.
    *   Object instances of \b dynamic_pool<> don't have value sematic.
    */
    template <
        size_t  ChunkBytes,
        bool    ThreadSafe = true>
    class dynamic_pool
    {
    private:
        nonpublic::pool_impl<ChunkBytes, ThreadSafe>   m_impl;

    public:
        K2_INJECT_COPY_BOUNCER();

        explicit dynamic_pool (size_t grow_count)
        :   m_grow_count(grow_count)
        {
        }

        void* allocate (size_t bytes)
        {
            if (bytes > ChunkBytes)
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
            size_t  PoolInit,
            template <size_t, bool> class PoolTmpl,
            size_t  PoolChunkSize,
            bool    ThreadSafe>
        class pool_sharing_helper
        {
        private:
            struct init_helper
            {
                typedef PoolTmpl<
                    safe_alignof<PoolChunkSize>::value,
                    ThreadSafe>
                    pool_type;

                init_helper ()
                :   m_pool(PoolInit) {}

                pool_type   pool;
            };
            static init_helper m_init_helper;

        public:
            static init_helper::pool_type&  instance ()
            {
                typedef class_singleton<
                    pool_sharing_helper,
                    init_helper>
                    shared_mem_pool;
                return  shared_mem_pool::instance().m_pool;
            }
        };
        template <
            size_t  PoolInit,
            template <size_t, bool> class PoolTmpl,
            size_t  PoolChunkSize,
            bool    ThreadSafe>
        pool_sharing_helper::init_helper    m_init_helper;

    }   //  namespace nonpublic
#endif  //  !DOXYGEN_BLIND

    /**
    *   \brief A Standard Allocator-compliant class template.
    *
    *   Object instances try to share the same underlying singleton instance
    *   of PoolTmpl<PoolInit,
    *   safe_alignof<PoolChunkSize>::value, ThreadSafe>.
    *   They do so by not considering T and compile-time compute
    *   safe_alignof<PoolChunkSize>::value, which made sure where
    *   T s compute the same alignment could share the same pool
    *   object instance.
    *
    *   Its first parameter, a template parameter \b T has the same
    *   sematic meaning of the first template parameter of std::allocator
    *   class tmeplate.
    *
    *   Its second parameter, a template template parameter \b PoolTmpl
    *   specifies the underlying pool implementation, which determines its
    *   allocation behavior, i.e. a class instanciation of \b static_pool<> or
    *   \b dynamic_pool<>.
    *
    *   Its third to fifth template parameters are used to instantiate class
    *   instances of PoolTmpl<>.
    */
    template <
        typename T,
        size_t  PoolInit,
        template <size_t, bool> class PoolTmpl,
        size_t  PoolChunkSize = sizeof(value_type),
        bool    ThreadSafe = true>
    class pool_allocator
    :   public defalloc_base<T>
    {
    private:
        typedef pool_allocator<
            value_type,
            PoolInit,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>
            self_type;

        typedef nonpublic::pool_sharing_helper<
            PoolInit,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>
            shared_mem_pool;

    public:
        template <typename other_T>
        struct rebind
        {
            typedef pool_allocator<
                other_T,
                PoolInit,
                PoolTmpl,
                PoolChunkSize,
                ThreadSafe>
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
        size_t  PoolInit,
        template <size_t, size_t, bool> class PoolTmpl,
        size_t  PoolChunkSize,
        bool    ThreadSafe>
    class pool_allocator<
        void,
        PoolInit,
        PoolTmpl,
        PoolChunkSize,
        ThreadSafe>
    :   public defalloc_base<void>
    {
    public:
        template <typename other_T>
        struct rebind
        {
            typedef pool_allocator<
                other_T,
                PoolInit,
                PoolTmpl,
                PoolChunkSize,
                ThreadSafe>
                other;
        };
    };
    template <
        typename lhs_T,
        typename rhs_T,
        size_t  PoolInit,
        template <size_t, size_t, bool> class PoolTmpl,
        size_t  PoolChunkSize,
        bool    ThreadSafe>
    bool operator== (
        const pool_allocator<
            lhs_T,
            PoolInit,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>&,
        const pool_allocator<
            rhs_T,
            PoolInit,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>&)
    {
        return  true;
    }
    template <
        typename lhs_T,
        typename rhs_T,
        size_t  PoolInit,
        template <size_t, size_t, bool> class PoolTmpl,
        size_t  PoolChunkSize,
        bool    ThreadSafe>
    bool operator!= (
        const pool_allocator<
            lhs_T,
            PoolInit,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>&,
        const pool_allocator<
            rhs_T,
            PoolInit,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>&)
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
    *   same \b PoolTmpl are just wrapper of comparison of user specified pool
    *   instances.
    *
    *   Its first parameter, a template parameter \b T has the same
    *   sematic meaning of the first template parameter of std::allocator
    *   class tmeplate.
    *
    *   Its second parameter, a template template parameter \b PoolTmpl
    *   specifis the underlying pool implementation, which determines its
    *   allocation behavior, i.e. a class instanciation of \b static_pool<> or
    *   \b dynamic_pool<>.
    *
    *   Its third to fifth template parameters are used to instantiate class
    *   instances of PoolTmpl<>.
    */
    template <
        typename T,
        template <size_t, bool> class PoolTmpl,
        size_t  PoolChunkSize = sizeof(value_type),
        bool    ThreadSafe = true>
    class pool_allocator_adapter
    :   public defalloc_base<T>
    {
    private:
        typedef pool_allocator_adapter<
            value_type,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>
            self_type;
        typedef PoolTmpl<PoolChunkSize, ThreadSafe>
            pool_type;
        pool_type* m_pool;

    public:
        template <typename other_T>
        struct rebind
        {
            typedef pool_allocator_adapter<
                other_T,
                PoolTmpl,
                PoolChunkSize,
                ThreadSafe>
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
        template <size_t, size_t, bool> class PoolTmpl,
        size_t  PoolChunkSize,
        bool    ThreadSafe>
    class pool_allocator_adapter<
        void,
        PoolTmpl,
        PoolChunkSize,
        ThreadSafe>
    :   public defalloc_base<void>
    {
    public:
        template <typename other_T>
        struct rebind
        {
            typedef pool_allocator_adapter<
                other_T,
                PoolTmpl,
                PoolChunkSize,
                ThreadSafe>
                other;
        };
    };
    template <
        typename lhs_T,
        typename rhs_T,
        template <size_t, size_t, bool> class PoolTmpl,
        size_t  PoolChunkSize,
        bool    ThreadSafe>
    bool operator== (
        const pool_allocator_adapter<
            lhs_T,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>& lhs,
        const pool_allocator_adapter<
            rhs_T,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>& rhs)
    {
        return  &lhs.get_pool() == &rhs.get_pool();
    }
    template <
        typename lhs_T,
        typename rhs_T,
        template <size_t, size_t, bool> class PoolTmpl,
        size_t  PoolChunkSize,
        bool    ThreadSafe>
    bool operator!= (
        const pool_allocator_adapter<
            lhs_T,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>& lhs,
        const pool_allocator_adapter<
            rhs_T,
            PoolTmpl,
            PoolChunkSize,
            ThreadSafe>& rhs)
    {
        return  !(lhs == rhs);
    }

    template <
        typename T,
        size_t  PoolChunkSize,
        bool    ThreadSafe>
    pool_allocator_adapter<
        T,
        static_pool,
        PoolChunkSize,
        ThreadSafe>
    make_allocator (
        static_pool<
            PoolChunkSize,
            ThreadSafe>& pool)
    {
        return  pool_allocator_adapter<
            T,
            static_pool,
            PoolChunkSize,
            ThreadSafe>(pool);
    }
    template <
        typename T,
        size_t  PoolChunkSize,
        bool    ThreadSafe>
    pool_allocator_adapter<
        T,
        dynamic_pool,
        PoolChunkSize,
        ThreadSafe>
    make_allocator (
        dynamic_pool<
            PoolChunkSize,
            ThreadSafe>& pool)
    {
        return  pool_allocator_adapter<
            T,
            dynamic_pool,
            PoolChunkSize,
            ThreadSafe>(pool);
    }

#if(0)
    void foo ()
    {
        Window win1;
        Window win2;

        win1 = win2;    //

        typedef mem_pool<256, true, false, false>   my_pool;
        my_pool pool;

        static const size_t my_size = 256;
        mem_pool<my_size, true, false, false>   pool;
        mem_pool<256, true>   pool;
        mem_pool<>   pool;

        pool.allocate();
    }

    template <
        size_t  ChunkBytes =    256,
        bool    PreAlloc =      true,
        bool    GrowOnDemand =  false,
        bool    ThreadSafe =    false>
    class mem_pool;

    template <
        size_t  ChunkBytes,
        bool    PreAlloc =      false,
        bool    GrowOnDemand =  false,
        bool    ThreadSafe =    false>
    class mem_pool
    {
    public:
        static const size_t chunk_bytes = ChunkBytes;

        mem_pool (size_t n);
        ~mem_pool ();

        void*   allocate ();
        void    deallocate (void* p);

        template <typename T>
        class allocator
        :   public defalloc_base<T>
        {
        public:
        };

        struct shared
        {
            static mem_pool&    instance ();

            template <typename T>
            class allocator
            :   public defalloc_base<T>
            {
            public:
            };
        };
        template <typename T>
        class shared_allocator
        {
        };

    private:
    };

    class mem_pool
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        mem_pool (size_t chunk_count, size_t chunk_bytes);
        ~mem_pool ();

        void*   allocate ();
        void    deallocate (void* p);

    private:
    };

    template <
        size_t      ChunkCount,
        size_t      ChunkBytes,
        bool        ThreadSafe,
        typename    IntanceTag = void>
    class shared_pool
    {
    private:
        struct init_helper
        {
            init_helper ()
            :   pool(ChunkCount, ChunkBytes)
            {
            }
            mem_pool    pool;
        };
        static init_helper  helper;

    public:
        static mem_pool&    instance ()
        {
            return  helper.pool;
        }
    };

    template <
        typename    T,
        size_t      ChunkCount,
        size_t      ChunkBytes,
        bool        ThreadSafe = false,
        typename    InstanceTag = void>
    class pool_allocator
    :   public defalloc_base
    {
    public:
        template <typename U>
        struct rebind
        {
            typedef pool_allocator<
                U,
                ChunkCount,
                ChunkBytes,
                ThreadSafe,
                InstanceTag>    other;
        };

        pool_allocator ()
        :   m_pool(
                shared_pool<
                    ChunkCount,
                    ChunkBytes,
                    ThreadSafe,
                    InstanceTag>::instance())
        {}
        template <typename U>
        pool_allocator (const rebind<U>::other& other)
        :   m_pool(other.get_pool())
        { /*no-op*/ }
        template <typename U>
        pool_allocator<T>& operator= (const rebind<U>::other& other)
        { /*no-op*/ }

        size_type   max_size () const
        {
            return  ChunkBytes;
        }
        mem_pool&   get_pool () const;

    private:
        mem_pool&   m_pool;
    };

    template <
        size_t      ChunkCount,
        size_t      ChunkBytes,
        bool        ThreadSafe,
        typename    InstanceTag>
    class pool_allocator<
        void,
        ChunkCount,
        ChunkBytes,
        ThreadSafe,
        InstanceTag>
    :   public defalloc_base<void>
    {
    public:
        template <typename U>
        struct rebind
        {
            typedef pool_allocator<
                U,
                ChunkCount,
                ChunkBytes,
                ThreadSafe,
                InstanceTag>    other;
        };

        pool_allocator ()
        :   m_pool(
                shared_pool<
                    ChunkCount,
                    ChunkBytes,
                    ThreadSafe,
                    InstanceTag>::instance())
        {}
        template <typename U>
        pool_allocator (const rebind<U>::other& other)
        :   m_pool(other.get_pool())
        { /*no-op*/ }
        template <typename U>
        pool_allocator<T>& operator= (const rebind<U>::other& other)
        { /*no-op*/ }

    private:
        mem_pool&   m_pool;
    };



    template <
        typename    T,
        size_t      ChunkCount,
        size_t      ChunkBytes,
        bool        ThreadSafe = false,
        typename    InstanceTag = void>
    pool_allocator

    template <typename InstanceTag>
    class pool_instances
    {
    public:
        template <
            size_t  ChunkBytes,
            bool    PreAlloc,
            bool    GrowOnDemand,
            bool    ThreadSafe>
        class instance
        {
        public:
            static void*    allocate ();
            static void     deallocate ();
        };
    };
    template <
        size_t  ChunkBytes,
        bool    PreAlloc,
        bool    GrowOnDemand,
        bool    ThreadSafe,
        typename InstanceTag = void>
    class mem_pool
    {
    public:
        mem_pool (size_t n);
        ~mem_pool ();

        void*   allocate ();
        void    deallocate (void* p);

    private:
    };

    typedef make_vector<turn_binding, pool_allocator<> >::type bindings_type;
    bindings_type   old_bindings;
    bindings_type   new_bindings;

    template <
    >
    struct make_pool_allocator
    {
        template <>
        struct
        typedef pool_allocator<>
            type;
    };

    template <
        typename T,
        typename PoolType>
    class pool_allocator
    {
    public:
        shared_pool_allocator ();
    };

    template <typename PoolType>
    class pool_allocator_adapter
    {
    public:
        template <typename T>
        class shared_type
        {
        public:
        };
        template <typename T>
        class instance_type
        {
        public:
        };
    };

#endif

}   //  namespace k2

#endif  //  !K2_BITS_AUTO_ALLOC_H
