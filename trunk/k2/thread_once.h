/*	libk2	<k2/thread_once.h>

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
#ifndef K2_THREAD_ONCE_H
#define K2_THREAD_ONCE_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_ATOMIC_H
#	include <k2/atomic.h>
#endif

namespace k2
{

	/**	\defgroup	Threading
	*/

	/**
	*	\ingroup	Threading
	*	\brief		Once-only initialization.
	*
	*	The purpose of \a thread_once is to ensure that a piece of
	*	initialization code is executed exactly once.
	*
	*	A \a thread_once object must be statically initialized to
	*	K2_THREAD_ONCE_INIT.
	*
	*	\deprecated	Use process_singleton, class_singleton or
	*	thread_specific_singleton instead.
	*/
	class thread_once
	{
	public:
		/**
		*	\brief	Invokes \a once_functor once.
		*	\param	once_functor	Funtor to invoke.
		*/
		template <typename unary_functor_t_>
		void run (unary_functor_t_ once_functor)
		{
			thread_once::run_impl(
				once_functor_wrapper<unary_functor_t_>,
				&once_functor);
		}

#ifndef	DOXYGEN_BLIND
		atomic_int_t	started;
		bool			done;
#endif	//	DOXYGEN_BLIND

	private:
		template <typename unary_functor_t_>
		static void once_functor_wrapper (void* p)
		{
			unary_functor_t_*	pf = reinterpret_cast<unary_functor_t_*>(p);
			(*pf)();
		}
		K2_DLSPEC void run_impl (void (*once_functor)(void*), void* arg) ;
	};
	/**
	*	\ingroup	Threading
	*	\brief		thread_once initializer
	*
	*	thread_once static initializer macro.
	*	\relates	thread_once
	*/
#	define K2_THREAD_ONCE_INIT	{-1, false}

}   //  namespace k2

#endif  //  !K2_THREAD_ONCE_H
