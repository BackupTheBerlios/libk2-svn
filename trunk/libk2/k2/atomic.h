/*	libk2	<k2/atomic.h>

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
#ifndef	K2_ATOMIC_H
#define	K2_ATOMIC_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif

namespace k2
{

    /**
	*	\brief	empty structure containing typedef	for platform-dependent atomic type.
    */
#if defined(K2_OS_LINUX)
	typedef	volatile int	atomic_int_t;
#elif defined(K2_HAS_WIN32_API)
	typedef	long	        atomic_int_t;
#else
    error   "How to perform atomic operation?"
#endif

    /**
	*	\relates atomic_int_t
    *   \brief  Increments (increases by one) \a value and
    *           test if \a value is non-zero atomically.
    */
	K2_DLSPEC bool atomic_increase (atomic_int_t& value) throw ();
    /**
	*	\relates atomic_int_t
    *   \brief  Decrements (decreases by one) \a value and
    *           test if \a value is non-zero atomically.
    */
	K2_DLSPEC bool atomic_decrease (atomic_int_t& value) throw ();

}	//	namespace k2

#endif	//	!K2_ATOMIC_H
