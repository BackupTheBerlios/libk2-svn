/*	libk2	<k2/stdint.h>

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
#ifndef K2_STDINT_H
#define K2_STDINT_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif

namespace k2
{
	//	Integer types define, a minimal set.

    typedef signed char     	int8_t;
    typedef unsigned char   	uint8_t;

    typedef short           	int16_t;
    typedef unsigned short  	uint16_t;

	typedef long            	int32_t;
	typedef unsigned long   	uint32_t;

	typedef long long        	int64_t;
	typedef unsigned long long 	uint64_t;

	typedef	unsigned int		uint_t;

#if defined(K2_OS_CYGWIN)
	typedef	unsigned long       size_t;
#endif

}	//	namespace k2

#endif  //  !K2_STDINT_H
