/*	libk2	<k2/dummy_lock.h>

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
#ifndef K2_DUMMY_LOCK_H
#define K2_DUMMY_LOCK_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_BOUNCER_H
#	include <k2/bouncer.h>
#endif
#ifndef	K2_SCOPE_H
#	include <k2/scope.h>
#endif

namespace k2
{

	class timestamp;

	class dummy_lock
	{
	public:
		K2_BOUNCE_DEFAULT_COPY();

		typedef	scoped_guard<dummy_lock>	scoped_guard;

		void acquire () {}
		bool acquire (const timestamp& timer) {return	true;}
		void release () {}
	};

}   //  namespace k2

#endif  //  !K2_DUMMY_LOCK_H
