

        struct process_wide;
        struct thread_local;
        struct stack_local;

        template <typename memory_trait>
        struct mem_management
        {
            void* allocate (size_t bytes);
            void deallocate (void* p, size_t bytes);
        };
        template <>
        struct mem_management<process_witde>
        {
            void* allocate (size_t bytes);
            void deallocate (void* p, size_t bytes);
        };
        template <>
        struct mem_management<thread_local>
        {
            void* allocate (size_t bytes);
            void deallocate (void* p, size_t bytes);
        };
        template <>
        struct mem_management<stack_local>
        {
            void* allocate (size_t bytes);
            void deallocate (void* p, size_t bytes);
        };

		template <bool threading_>
		class mem_pool
		{
		private:
            typedef type_select<threading_, spin_lock, dummy_lock>::type
                lock_type;
            typedef typename lock_type::scoped_guard	scoped_guard;

            static const size_t	alignment = 8;

			union chunk
			{
			    char    addr[1];
			    chunk*  pnext;
			};

			lock_type	m_lock;
			//  Client asked for size in bytes of a chunk.
		    size_t      m_client_bytes;
		    //  Actual size in bytes of a chunk (rounded-up for alignment).
		    size_t      m_chunk_bytes;
		    //  Pointer to memory block allocated.
		    char*       m_mem_blk;
		    //  Linked list of free chunks.
			chunk*		m_free_chunks;

			//  Too much overhead... decided to drop this feature.
			//bool        m_wait_for_free;    //  Wait for free if no free chunks?
			                                //  Otherwise, throw std::bad_alloc.

		public:
		    mem_pool (size_t client_bytes /* of a chunk */, size_t chunk_cnt)
		    :   m_client_bytes(client_bytes)
		    ,   m_chunk_bytes(0)
		    ,   m_mem_blk(0)
		    ,   m_free_chunks(0)
		    {
		        m_chunks_bytes = (client_bytes + (size_t)alignment-1) &
		                ~((size_t)alignment-1);

		        m_mem_blk = new char[m_chunk_bytes * chunk_cnt];

		        if (chunk_cnt)
		        {
		            //  Build m_free_list form m_mem_blk.
		            size_t  idx = 0;
		            chunk*  list = m_free_chunks = reinterpret_cast<chunk*>(m_mem_blk);
		            for (;;)
		            {
    		            if (chunk_cnt > idx + 1)
    		            {
    		                ++idx;
    		                list->pnext = reinterpret_cast<chunk*>(&m_mem_blk[idx]);
    		            }
    		            else
    		            {
    		                list->pnext = 0;
    		                break;
    		            }
    		        }
		        }
		    }
		    ~mem_pool ()
		    {
		        delete [] m_mem_blk;
		    }

		    void* allocate (size_t req_bytes)
			{
			    scoped_guard    guard(m_lock);
			    if (m_free_chunks && req_bytes <= m_client_bytes)
			    {
    			    chunk*  p = m_free_chunks;
    			    m_free_chunks = m_free_chunks->pnext;
    			    return  p;
			    }

		        throw   std::bad_alloc();
            }

			void deallocate (void* p, size_t req_bytes)
			{
                //  Only simple error detections are implemented.
                //  Raionale 1. Result similar behaviors in different builds.
                //           2. Preserve memory otherwise required for
                //              more sophisticated error detection.
                //           3. Install more when required later.
			    assert(req_bytes <= m_client_bytes);
			    assert(p >= (void*)&m_chunks[0]);
			    assert((((void*)&m_chunks[0] - p) % m_chunk_bytes) == 0);

			    chunk* pchunk = reinterpret_cast<chunk*>(p);

			    scoped_guard    guard(m_lock);
			    if (m_free_list)
			    {
    			    pchunk->pnext = m_free_list;
                    m_free_list = pchunk;
                    return;
			    }

		        m_free_list = pchunk;
			}
		};

        template <typename type_, size_t capacity_, bool threading>
        class pool_allocator : public allocator_base<type_>
        {
        public:
            pool_allocator ()
            :   m_pool(sizeof(type_), capacity_) {}

            pool_allocator () {}
            pool_allocator (const pool_allocator<type_>& rhs) {}
            template <typename rhs_type_>
            pool_allocator (const pool_allocator<rhs_type_>& rhs) {}
            template <typename rhs_type_>
            pool_allocator& operator= (const pool_allocator<rhs_type_>& rhs) {}

        private:
            mem_pool   m_pool;
        };

        template <typename type_>
        class pool_adapter_allocator : public allocator_base<type_>
        {
        public:
            pool_adapter_allocator ()
            :   m_ref_pool(0) {}

            pool_adapter_allocator (mem_pool& pool)
            :   m_ref_pool(&pool) {}
            pool_adapter_allocator (const pool_adapter_allocator<type_, true>& rhs)
            :   m_ref_pool(rhs.get_pimpl()) {}
            pool_adapter_allocator& operator= (const pool_adapter_allocator<type_>& rhs)
            :   m_ref_pool(rhs.get_pimpl()) {}

            pointer   allocate (size_type n, void* /*phint*/ = 0)
            {
                if (m_ref_pool)
                    return  reinterpret_cast<value_type*>(m_ref_pool->allocate(sizeof(type_) * n));
                else
                    throw   std::bad_alloc();
            }
            void    deallocate (pointer p, size_type n)
            {
                if (m_ref_pool)
                {
                    m_ref_pool->deallocate(p, sizeof(type_) * n);
                    return;
                }
                assert(0);
            }
            mem_pool*  get_pimpl () const
            {
                return  m_ref_pool;
            }

        private:
            mem_pool*          m_ref_pool;
        };

