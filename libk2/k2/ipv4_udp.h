/*	libk2	<k2/ipv4_udp.h>

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
#ifndef	K2_IPV4_UDP_H
#define	K2_IPV4_UDP_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_IPV4_H
#	include <k2/ipv4.h>
#endif
#ifndef	K2_BOUNCER_H
#	include <k2/bouncer.h>
#endif

namespace k2
{

	/**	\defgroup	Networking
	*/
	namespace ipv4
	{

		/*	\ingroup	Networking
		*/
		class udp_transport
		{
		public:
			K2_BOUNCE_DEFAULT_COPY();

			K2_DLSPEC explicit udp_transport (const layer4_addr& local_addr);
			K2_DLSPEC explicit udp_transport (int udp_desc, bool own);
			K2_DLSPEC ~udp_transport ();

			K2_DLSPEC const layer4_addr	local_addr () const;

			K2_DLSPEC size_t write (const char* buf, size_t bytes, const layer4_addr& remote_addr);

			K2_DLSPEC size_t read (char* buf, size_t bytes, layer4_addr& remote_addr);
			K2_DLSPEC size_t read (char* buf, size_t bytes, layer4_addr& remote_addr, const time_span& timeout);

		private:
			int		m_desc;
			bool	m_own;	//	close() on destroy?
		};

	}	//	namespace k2

}	//	namespace k2

#endif	//	!K2_IPV4_UDP_H
