/*	libk2	<k2/copy_bouncer.h>

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
#ifndef	K2_COPY_BOUNCER_H
#define	K2_COPY_BOUNCER_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif

namespace k2
{

	struct default_copy_bouncer
	{
		default_copy_bouncer () {}

	private:
		//	Deliberately not implemented.
		default_copy_bouncer (
			const default_copy_bouncer&);
		default_copy_bouncer& operator= (const default_copy_bouncer&);
	};

}	//	namespace k2


/**	\defgroup	Utility
*/

/**
*	\ingroup	Utility
*	\brief		Macro that inserts a non-copyable member into a class.
*/
#define	K2_INJECT_COPY_BOUNCER()\
	k2::default_copy_bouncer copy_bouncer

#endif	//	!K2_COPY_BOUNCER_H
