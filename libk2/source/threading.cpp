/*	libk2	<threading.cpp>

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
#include <k2/thread.h>

#ifndef	K2_SPIN_LOCK_H
#	include <k2/spin_lock.h>
#endif
#ifndef	K2_THREAD_ONCE_H
#	include <k2/thread_once.h>
#endif
#ifndef	K2_MUTEX_H
#	include <k2/mutex.h>
#endif
#ifndef	K2_COND_VAR_H
#	include <k2/cond_var.h>
#endif
#ifndef	K2_TLS_H
#	include <k2/tls.h>
#endif
#ifndef	K2_SINGLETON_H
#	include <k2/singleton.h>
#endif
#ifndef	K2_TIMING_H
#	include <k2/timing.h>
#endif
#ifndef	K2_ASSERT_H
#	include <k2/assert.h>
#endif
#ifndef	K2_ERRNO_H
#	include <k2/errno.h>
#endif


#if defined(K2_OS_UNIX)
#	include <unistd.h>	//	usleep
	namespace
	{
		inline void os_msleep (size_t msec)
		{
			::usleep(msec * 1000);
		}
	}
#else
#	include <windows.h>
	namespace
	{
		inline void os_msleep (size_t msec)
		{
			::Sleep(DWORD(msec));
		}
	}
#endif	//	K2_OS_UNIX

#include <pthread.h>
#include <sched.h>
namespace
{
	inline void os_sched_yield ()
	{
		sched_yield();
	}

	inline pthread_mutex_t&		get_impl (k2::mutex::handle& handle)
	{
		K2_STATIC_ASSERT(
			sizeof(handle) >= sizeof(pthread_mutex_t), handle_size_too_small);
		return	reinterpret_cast<pthread_mutex_t&>(handle.holder);
	}

	inline pthread_cond_t&		get_impl (k2::cond_var::handle& handle)
	{
		K2_STATIC_ASSERT(
			sizeof(handle) >= sizeof(pthread_cond_t), handle_size_too_small);
		return	reinterpret_cast<pthread_cond_t&>(handle.holder);
	}

	inline pthread_key_t& 		get_impl (
		k2::nonpublic::tls_ptr_impl::handle& handle)
	{
		K2_STATIC_ASSERT(
			sizeof(handle) >= sizeof(pthread_key_t), handle_size_too_small);
		return	reinterpret_cast<pthread_key_t&>(handle.holder);
	}
	inline const pthread_key_t&	get_impl (
		const k2::nonpublic::tls_ptr_impl::handle& handle)
	{
		K2_STATIC_ASSERT(
			sizeof(handle) >= sizeof(pthread_key_t), handle_size_too_small);
		return	reinterpret_cast<const pthread_key_t&>(handle.holder);
	}
}

namespace k2
{

	spin_lock::spin_lock ()
	:	m_lock(-1)
	{
	}
	void spin_lock::acquire ()
	{
		while (atomic_increase(m_lock) != 0)
			atomic_decrease(m_lock);
	}
	bool spin_lock::acquire (const timestamp& timer)
	{
		while (atomic_increase(m_lock) != 0)
		{
			atomic_decrease(m_lock);

			if (timer.expired() == false)
				continue;
			else
				return	false;
		}

		return	true;
	}
	void spin_lock::release ()
	{
		atomic_decrease(m_lock);
	}


	void thread_once::run_impl (void (*once_routine)(void* arg), void* arg)
	{
		runtime_assert(once_routine != 0);

		if (this->done == false)
		{
			if (atomic_increase(this->started) == false)
			{
				once_routine(arg);
				this->done = true;
			}
			else
			{
				while (this->done == false)
				{
					os_sched_yield();
				}
			}
		}
	}

	mutex::mutex ()
	{
		//	Never return error.
		pthread_mutex_init(&get_impl(m_handle), 0);
	}
	mutex::~mutex ()
	{
		pthread_mutex_destroy(&get_impl(m_handle));
	}

	void mutex::acquire ()
	{
		if(pthread_mutex_lock(&get_impl(m_handle)) != 0)
			throw	thread_error();
	}

	bool mutex::acquire (const timestamp& timer)
	{
		timespec	ts;
		ts.tv_sec = timer.in_sec();
		ts.tv_nsec = timer.msec_of_sec() * 1000;
		while (1)
		{
			int	res = pthread_mutex_trylock(&get_impl(m_handle));

			switch (res)
			{
				case 0:
					return	true;
				case EBUSY:
					if (timer.expired() == false)
						os_sched_yield();
					else
						return	false;
					break;
				default:
					throw	thread_error();
			}
		}
	}
	void mutex::release ()
	{
		if (pthread_mutex_unlock(&get_impl(m_handle)) != 0)
			throw	thread_error();
	}

	cond_var::cond_var (mutex& mtx)
	:	m_mtx(mtx)
	{
		//	Never return error.
		pthread_cond_init(&get_impl(m_handle), 0);
	}
	cond_var::~cond_var ()
	{
		pthread_cond_destroy(&get_impl(m_handle));
	}
	void cond_var::wait ()
	{
		//	Never return error.
		pthread_cond_wait(
			&get_impl(m_handle), &get_impl(m_mtx.m_handle)
		);
	}
	bool cond_var::wait (const timestamp& timer)
	{
		timespec ts;
		ts.tv_sec = timer.in_sec();
		ts.tv_nsec = timer.msec_of_sec() * 1000 * 1000;

		while (1)
		{
			int	res = pthread_cond_timedwait(
				&get_impl(m_handle),
				&get_impl(m_mtx.m_handle),
				&ts);

			switch (res)
			{
				case 0:
					return	true;
				case ETIMEDOUT:
					return	false;
			}
		}
	}
	void cond_var::signal ()
	{
		pthread_cond_signal(&get_impl(m_handle));
	}

	void cond_var::broadcast ()
	{
		pthread_cond_broadcast(&get_impl(m_handle));
	}

	namespace nonpublic
	{

		tls_ptr_impl::tls_ptr_impl (
			void (*destroy_routine)(void*))
		:	m_destroy_routine(destroy_routine)
		{
			if (pthread_key_create(&get_impl(m_handle), destroy_routine) != 0)
				throw	bad_resource_alloc();
		}
		tls_ptr_impl::~tls_ptr_impl ()
		{
		}
		void* tls_ptr_impl::get () const
		{
			return	pthread_getspecific(get_impl(m_handle));
		}
		void* tls_ptr_impl::release ()
		{
			void* old_p = pthread_getspecific(get_impl(m_handle));
			if (pthread_setspecific(get_impl(m_handle), 0) != 0)
				throw	thread_error();

			return	old_p;
		}
		void tls_ptr_impl::reset (void* p)
		{
			void* old_p = pthread_getspecific(get_impl(m_handle));
			if (old_p && old_p != p)
				m_destroy_routine(old_p);

			if (pthread_setspecific(get_impl(m_handle), p) != 0)
				throw	thread_error();
		}
	}

	namespace nonpublic
	{
		struct thread_cntx
		{
			enum exec_state
			{
				initializing,
				running,
				cancel_pending,
				exited
			};
			enum exit_cause_enum
			{
				na,
				completed,
				cancelled,
				uncaught_exception
			};

			typedef	pthread_t	implement_t;

			//	Members don't need synchronizations.
			implement_t			m_handle;
			void				(*m_thread_entry)(void*);
			void*				m_arg;
			const bool			m_detached;

			//	Members need synchronizations.
			mutex				m_mtx;
			cond_var			m_exec_state_change_cv;
			exec_state			m_exec_state;
			exit_cause_enum		m_exit_cause;
			bool				m_cancel_enabled;

			thread_cntx (void (*thread_entry)(void*), void* arg, bool detached)
			:	m_thread_entry(thread_entry)
			,	m_arg(arg)
			,	m_detached(detached)
			,	m_exec_state_change_cv(m_mtx)
			,	m_exec_state(initializing)
			,	m_exit_cause(na)
			,	m_cancel_enabled(true)
			{
			}
			~thread_cntx ()
			{
			}
		};
	}

	using namespace nonpublic;

	inline thread_cntx*&	thread_specific_cntx_ptr ()
	{
		return	thread_local_singleton<
			thread_cntx*, singleton::destroy_prio_lowest - 1>::instance();
	}

	thread_cntx* thread::spawn_impl (
		void (*thread_entry)(void*), void* arg, bool detached)
	{
		std::auto_ptr<thread_cntx>	pcntx;
		try
		{
			pcntx.reset(new thread_cntx(thread_entry, arg, detached));
		}
		catch (std::bad_alloc& x)
		{
			throw	bad_resource_alloc(x.what());
		}

		if (pthread_create(
			&pcntx->m_handle,
			0,
			pthread_entry_wrapper,
			static_cast<void*>(pcntx.get())) != 0)
		{
			throw	bad_resource_alloc();
		}

		{
			mutex::scoped_guard	guard(pcntx->m_mtx);
			while (pcntx->m_exec_state == thread_cntx::initializing)
			{
				pcntx->m_exec_state_change_cv.wait();
			}
		}

		//	If the spawn thread is in detached state.
		if (detached == true)
		{
			//	Release the ownership of the controlled pointer.
			//	Spawned thread will do the cleanup at the end
			//	of ThreadEntryWrapper().
			pcntx.release();
		}

		return	pcntx.release();
	}
	//	static
	void* thread::pthread_entry_wrapper (void* arg)
	{
		std::auto_ptr<thread_cntx>	pcntx(static_cast<thread_cntx*>(arg));
		bool	has_current_thread = false;

		try
		{
			{
				mutex::scoped_guard	guard(pcntx->m_mtx);

				pcntx->m_exec_state	= thread_cntx::running;
				pcntx->m_exec_state_change_cv.signal();

				thread_specific_cntx_ptr() = pcntx.get();
				has_current_thread = true;
			}

			pcntx->m_thread_entry(pcntx->m_arg);
			pcntx->m_exit_cause = thread_cntx::completed;
		}
		catch (thread::cancel_signal&)
		{
			pcntx->m_exit_cause = thread_cntx::cancelled;
			//	Absorb cancel signal exception.
		}
		catch (...)
		{
			pcntx->m_exit_cause = thread_cntx::uncaught_exception;
			//	Absorb all exceptions, clean up then exits.
		}

		{
			mutex::scoped_guard	guard(pcntx->m_mtx);
			pcntx->m_exec_state = thread_cntx::exited;

			//	If this dieing thread is in detached state.
			if (pcntx->m_detached == true)
			{
				pthread_detach(pcntx->m_handle);
				pcntx.reset();
			}
			else
			{
				if (has_current_thread)
					thread_specific_cntx_ptr() = 0;

				pcntx->m_exec_state_change_cv.signal();

				//	Release the ownership of the controlled pointer.
				//	Leave cleanup to thread object's destroctor.
				pcntx.release();
			}
		}
		return	0;
	}


	thread::~thread ()
	{
		this->join();
		pthread_join(m_pcntx->m_handle, 0);
		delete	m_pcntx;
	}
	void thread::cancel ()
	{
		mutex::scoped_guard	guard(m_pcntx->m_mtx);

		if (m_pcntx->m_exec_state != thread_cntx::exited)
			m_pcntx->m_exec_state = thread_cntx::cancel_pending;
	}
	void thread::join ()
	{
		mutex::scoped_guard	guard(m_pcntx->m_mtx);
		while (m_pcntx->m_exec_state != thread_cntx::exited)
		{
			m_pcntx->m_exec_state_change_cv.wait();
		}
	}
	bool thread::join (const timestamp& timer)
	{
		mutex::scoped_guard	guard(m_pcntx->m_mtx);
		if (m_pcntx->m_exec_state != thread_cntx::exited)
		{
			if (m_pcntx->m_exec_state_change_cv.wait(timer) == true)
				return	true;
			else
				return	false;
		}
		return	true;
	}
	//	static
	bool thread::cancel_enabled (bool yesno)
	{
		thread_cntx*	pcntx = thread_specific_cntx_ptr();
		if (pcntx == 0)
			return	false;

		mutex::scoped_guard	guard(pcntx->m_mtx);
		bool last = pcntx->m_cancel_enabled;
		pcntx->m_cancel_enabled = yesno;
		return	last;
	}
	//	static
	void thread::test_cancel ()
	{
		thread_cntx*	pcntx = thread_specific_cntx_ptr();
		if (pcntx == 0)
			return;

		mutex::scoped_guard	guard(pcntx->m_mtx);
		if (pcntx->m_exec_state == thread_cntx::cancel_pending)
			throw	thread::cancel_signal();
	}

	//	static
	void thread::sched_yield ()
	{
		os_sched_yield();
	}
	//	static
	void thread::sleep (const time_span& span)
	{
		os_msleep(uint32_t(span.in_msec()));
	}
	//	static
	void thread::sleep (size_t msec)
	{
		os_msleep(msec);
	}


}   //  namespace k2
