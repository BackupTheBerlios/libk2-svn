/*	libk2	<k2/assert.h>

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
#ifndef	K2_ASSERT_H
#define	K2_ASSERT_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_STD_H_EXCEPTION
#	include <exception>
#	define	K2_STD_H_EXCEPTION
#endif

#define	K2_STATIC_ASSERT(expr, msg_class)\
{\
	struct msg_class\
	{\
	};\
	msg_class msg;\
	(void)sizeof(k2::static_assert_failed<(expr)>(msg));\
}

namespace k2
{

	template <bool>
	struct static_assert_failed;

	template <>
	struct static_assert_failed<true>
	{
		static_assert_failed (...);
	};

	inline void runtime_assert (bool expr)
#if defined(K2_DEBUG)
	{
	    if (expr == false)
	        throw   std::bad_exception();
	}
#else
    {
    }
#endif

}	//	namespace k2


#endif	//	!K2_ASSERT_H
