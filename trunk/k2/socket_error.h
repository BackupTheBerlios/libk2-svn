/*	libk2	<k2/socket_error.h>

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
#ifndef K2_SOCKET_ERROR_H
#define K2_SOCKET_ERROR_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_EXCEPTION_H
#	include <k2/exception.h>
#endif

namespace k2
{

	/**	\defgroup	Networking
	*/

	/*	\ingroup	Networking
	*/
	class socket_errno
	{
	public:
		socket_errno ()
		:	m_errno(get_last()) {}

		int	get () const { return m_errno; }

		K2_DLSPEC static int get_last ();

	private:
		int	m_errno;
	};

	/*	\ingroup	Networking
	*/
	struct socket_accept_error : bad_resource_alloc, socket_errno
	{
		explicit socket_accept_error (const char* what = "k2::socket_accept_error")
			:	bad_resource_alloc(what) {};
	};
	/*	\ingroup	Networking
	*/
	struct socket_open_error : bad_resource_alloc, socket_errno
	{
		explicit socket_open_error (const char* what = "k2::socket_open_error")
			:	bad_resource_alloc(what) {};
	};
	/*	\ingroup	Networking
	*/
	struct socket_bind_error : runtime_error, socket_errno
	{
		explicit socket_bind_error (const char* what = "k2::socket_bind_error")
			:	runtime_error(what) {};
	};
	/*	\ingroup	Networking
	*/
	struct socket_connect_error : runtime_error, socket_errno
	{
		explicit socket_connect_error (const char* what = "k2::socket_connect_error")
			:	runtime_error(what) {};
	};
	/*	\ingroup	Networking
	*/
	struct socket_timedout_error : timedout_error, socket_errno
	{
		explicit socket_timedout_error (const char* what = "k2::socket_timedout_error")
			:	timedout_error(what) {};
	};
	/*	\ingroup	Networking
	*/
	struct socket_tcp_io_error : runtime_error, socket_errno
	{
		explicit socket_tcp_io_error (const char* what = "k2::socket_tcp_io_error")
			:	runtime_error(what) {};
	};
	/*	\ingroup	Networking
	*/
	struct socket_udp_io_error : runtime_error, socket_errno
	{
		explicit socket_udp_io_error (const char* what = "k2::socket_udp_io_error")
			:	runtime_error(what) {};
	};

}   //  namespace k2

#endif  //  !K2_SOCKET_ERROR_H
