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
#ifndef K2_BITS_POOL_ALLOC_H
#define K2_BITS_POOL_ALLOC_H

#ifndef K2_BITS_FAST_LOCK_H
#   include <k2/bits/fast_lock.h>
#endif
#ifndef K2_BITS_OPT_H
#   include <k2/bits/opt.h>
#endif
#ifndef K2_BITS_COPY_BOUNCER_H
#   include <k2/bits/copy_bouncer.h>
#endif
#ifndef K2_BITS_DEFALLC_BASE_H
#   include <k2/bits/defalloc_base.h>
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
        struct mem_pool_base
        {
        protected:
            union chunk
            {
                chunk*  pnext;
            };
            chunk*  m_allocs;
            chunk*  m_frees;
        };
    }   //  namespace nonpublic

#endif  //  !DOXYGEN_BLIND

    template <
        size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe = true>
    class mem_pool
    :   nonpublic::mem_pool_base
    {
    private:
        K2_INJECT_COPY_BOUNCER();

        typedef typename fast_lock<ThreadSafe>::type    lock_type;
        typedef typename lock_type::scoped_guard        scoped_guard;

        lock_type   m_lock;
        chunk*      m_allocs;
        chunk*      m_frees;

        void deposit (void* p)
        {
            chunk* pchunk = reinterpret_cast<chunk*>(p);
            pchunk->pnext = m_frees;
            m_frees = pchunk;
        }
        void grow (size_t chunk_cnt)
        {
            char*   raw_mem = new char[chunk_cnt * alignment];
            chunk*  pchunk = reinterpret_cast<chunk*>(raw_mem);
            pchunk->pnext = m_allocs;
            m_allocs = pchunk;

            size_t idx = 0;
            for (;
                idx < chunk_cnt;
                idx++,raw_mem += alignment)
            {
                this->deposit(raw_mem);
            }
        }

        static const size_t max_aligment = 1024;

    public:
        static const size_t alignment = safe_alignof::constant<ChunkBytes>::value;

        mem_pool ()
        :   m_allocs(0)
        ,   m_frees(0)
        {
            K2_STATIC_ASSERT(alignment <= max_aligment, template_parameter_ChunkBytes_is_too_big);
            this->grow(ChunkCount);
        }
        ~mem_pool ()
        {
            chunk*  pchunk = m_allocs;
            while(pchunk)
            {
                chunk*  pnext = pchunk->pnext;
                delete [] reinterpret_cast<char*>(pchunk);
                pchunk = pnext;
            }
        }
        void* alloc ()
        {
            scoped_guard    guard(m_lock);

            if(K2_OPT_BRANCH_FALSE(m_frees == 0))
            {
                size_t  chunk_cnt = ChunkCount;
                if(alignment * ChunkCount > max_aligment * 4)
                {
                    chunk_cnt = max_aligment * 4 / alignment;
                }
                this->grow(chunk_cnt);
            }

            chunk*  pchunk = m_frees;
            m_frees = m_frees->pnext;
            return  pchunk;
        }
        void dealloc (void* p)
        {
            scoped_guard    guard(m_lock);
            this->deposit(p);
        }

    };

    template <
        typename    ValueT
    ,   size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe = true
    ,   typename    InstanceTagT = void>
    class shared_pool_allocator;


    template <
        size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe = true
    ,   typename    InstanceTagT = void>
    class shared_pool
    {
    public:
        typedef mem_pool<ChunkCount, ChunkBytes, ThreadSafe>
            pool_type;
    private:
        static pool_type    s_pool;
        typedef shared_pool<
            ChunkCount
        ,   ChunkBytes
        ,   ThreadSafe
        ,   InstanceTagT>   self_type;

    public:
        static pool_type&    instance ()
        {
            return  shared_pool::s_pool;
        }

        template <typename ValueT>
        struct allocator
        {
            typedef shared_pool_allocator<
                ValueT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>   type;
        };
    };

    //static
    template <
        size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe
    ,   typename    InstanceTagT>
    typename shared_pool<ChunkCount, ChunkBytes, ThreadSafe, InstanceTagT>::pool_type
        shared_pool<ChunkCount, ChunkBytes, ThreadSafe, InstanceTagT>::s_pool;

    template <
        typename    ValueT
    ,   size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe
    ,   typename    InstanceTagT>
    class shared_pool_allocator
    :   public defalloc_base<ValueT>
    {
    private:
        typedef shared_pool<
                ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>   shared;

    public:
        template <typename OtherT>
        struct rebind
        {
            typedef shared_pool_allocator<
                OtherT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>   other;
        };

        shared_pool_allocator ()
        {}
        template <typename OtherT>
        shared_pool_allocator (
            const shared_pool_allocator<
                OtherT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>&)
        {}
        template <typename OtherT>
        shared_pool_allocator& operator= (
            const shared_pool_allocator<
                OtherT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>&)
        {
            return  *this;
        }
        ValueT* allocate (size_type n, void* /*hint*/ = 0)
        {
            size_type   bytes = sizeof(ValueT) * n;

            if(K2_OPT_BRANCH_FALSE(bytes > ChunkBytes))
            {
                return  reinterpret_cast<ValueT*>(::operator new(bytes));
            }

            return  reinterpret_cast<ValueT*>(shared::instance().alloc());
        }
        void    deallocate (ValueT* p, size_type n)
        {
            size_type   bytes = sizeof(ValueT) * n;

            if(K2_OPT_BRANCH_FALSE(bytes > ChunkBytes))
            {
                ::operator delete(p);
            }

            shared::instance().dealloc(p);
        }
    };

    template <
        size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe
    ,   typename    InstanceTagT>
    class shared_pool_allocator<
        void
    ,   ChunkCount
    ,   ChunkBytes
    ,   ThreadSafe
    ,   InstanceTagT>
    :   public defalloc_base<void>
    {
    public:
        template <typename OtherT>
        struct rebind
        {
            typedef shared_pool_allocator<
                OtherT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>   other;
        };

        shared_pool_allocator ()
        {}
        template <typename OtherT>
        shared_pool_allocator (
            const shared_pool_allocator<
                OtherT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>&)
        {}
        template <typename OtherT>
        shared_pool_allocator& operator= (
            const shared_pool_allocator<
                OtherT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>&)
        {
            return  *this;
        }
    };

    template <
        typename    LhsT
    ,   typename    RhsT
    ,   size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe
    ,   typename    InstanceTagT>
    bool operator== (
        const shared_pool_allocator<
                LhsT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>&,
        const shared_pool_allocator<
                RhsT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>&)
    {
        return  true;
    }
    template <
        typename    LhsT
    ,   typename    RhsT
    ,   size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe
    ,   typename    InstanceTagT>
    bool operator!= (
        const shared_pool_allocator<
                LhsT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>&,
        const shared_pool_allocator<
                RhsT
            ,   ChunkCount
            ,   ChunkBytes
            ,   ThreadSafe
            ,   InstanceTagT>&)
    {
        return  false;
    }
#if(0)
    template <
        size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe = true
    ,   typename    InstanceTagT = void>
    class shared_pool
    {
    public:
        typedef mem_pool<ChunkCount, ChunkBytes, ThreadSafe>
            pool_type;
    private:
        static pool_type    s_pool;
        typedef shared_pool<
            ChunkCount
        ,   ChunkBytes
        ,   ThreadSafe
        ,   InstanceTagT>   self_type;

    public:
        static pool_type&    instance ()
        {
            return  shared_pool::s_pool;
        }

        template <typename ValueT>
        class allocator
        :   public defalloc_base<ValueT>
        {
        public:
            template <typename OtherT>
            struct rebind
            {
                typedef allocator<OtherT>::other;
            };

            allocator ()
            {}
            template <typename OtherT>
            allocator (const allocator<OtherT>&)
            {}
            template <typename OtherT>
            allocator& operator= (const allocator<OtherT>&)
            {
                return  *this;
            }
            template <typename OtherT>
            bool operator== (const allocator<OtherT>&) const
            {
                return  true;
            }
            template <typename OtherT>
            bool operator!= (const allocator<OtherT>&) const
            {
                return  false;
            }

            ValueT* allocate (size_type n, void* /*hint*/ = 0)
            {
                size_type   bytes = sizeof(ValueT) * n;

                if(K2_OPT_BRANCH_FALSE(bytes > ChunkBytes))
                {
                    return  reinterpret_cast<ValueT*>(::operator new(bytes));
                }

                return  reinterpret_cast<ValueT*>(shared_pool::instance().alloc());
            }
            void    deallocate (ValueT* p, size_type n)
            {
                size_type   bytes = sizeof(ValueT) * n;

                if(K2_OPT_BRANCH_FALSE(bytes > ChunkBytes))
                {
                    ::operator delete(p);
                }

                shared_pool::instance().dealloc(p);
            }
        };

        template <>
        class allocator<void>
        :   public defalloc_base<void>
        {
            template <typename OtherT>
            struct rebind
            {
                typedef allocator<OtherT>::other;
            };
            allocator ()
            {}
            template <typename OtherT>
            allocator (const allocator<OtherT>&)
            {}
            template <typename OtherT>
            allocator& operator= (const allocator<OtherT>&)
            {
                return  *this;
            }
            template <typename OtherT>
            bool operator== (const allocator<OtherT>&) const
            {
                return  true;
            }
            template <typename OtherT>
            bool operator!= (const allocator<OtherT>&) const
            {
                return  false;
            }
        };
    };

    //static
    template <
        size_t      ChunkCount
    ,   size_t      ChunkBytes
    ,   bool        ThreadSafe
    ,   typename    InstanceTagT>
    typename shared_pool<ChunkCount, ChunkBytes, ThreadSafe, InstanceTagT>::pool_type
        shared_pool<ChunkCount, ChunkBytes, ThreadSafe, InstanceTagT>::s_pool;
#endif

#if(0)

    class task_queue
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        task_queue (size_t thread_cnt);
        ~task_queue ();

        bool    empty () const;
        size_t  size () const;

        void    cancel ();

        template <typename TaskT>
        void enqueue (const TaskT& task)
        {
            std::auto_ptr<TaskT>    copy(new TaskT(task));
            m_tasks.push_back(task_cntx(task_adapter<TaskT>, task_cleanup<TaskT>, copy.get()));
            copy.release();
        }

    private:
        struct task_cntx
        {
            task_cntx (void (*adapter)(void*), void (*cleanup)(void*), void* arg)
            :   adapter(adapter)
            ,   cleanup(cleanup)
            ,   arg(arg) {}

            void operator() ()
            {
                try
                {
                    (*task)(arg);
                }
                catch(std::exception&)
                {
                    //  absorb, perhaps log
                }
                (*cleanup)(arg);
            }

            void    (*task)(void*);
            void    (*cleanup)(void*);
            void*   arg;
        };
        template <typename TaskT>
        void task_adapter (void* arg)
        {
            Task* task(reinterpret_cast<TaskT*>(arg);
            (*task)();
        }
        template <typename TaskT>
        void task_cleanup (void* arg)
        {
            delete  reinterpret_cast<TaskT*>(arg);
        }

        std::deque<task_cntx>   m_tasks;

        struct consumer
        {
            consumer (task_queue&);

            void operator() ()
            {
                while(1)
                {
                    scoped_guard    guard(m_queue.m_mtx);
                    while(m_queue.m_tasks.empty() == true)
                    {
                        m_queue.m_cv.wait();
                        thread::test_cancel();
                    }

                    task_cntx   cntx(m_queue.m_tasks.front());
                    m_queue.m_tasks.pop_front();
                    cntx();
                    thread::test_cancel();
                }
            }
        }

    };

#endif

}   //  namespace k2

#endif  //  !K2_BITS_POOL_ALLOC_H
