/*	libk2	<k2/errno.h>

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
#ifndef	K2_ERRNO_H
#define	K2_ERRNO_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_STD_H_CEERNO
#	include <cerrno>
#	define	K2_STD_H_CEERNO
#endif

#if !defined(K2_HAS_POSIX_API)

//	Defines missing errno constants in Win32 <cerrno>.
//	Error number constants used are values from <winsock2.h>.
//	For those already defined in <cerrno>, the values in
//	cerrno are used, for example WSAEFAULT is 10014 in
//	<winsock2.h> but EFAULT is something else in <cerrno>,
//	in these cases, libk2 stick with the standard
//	<cerrno>.

/*	defined in <cerrno>
#define	EACCES			10013
*/
#define EADDRINUSE		10048
#define EADDRNOTAVAIL	10049
#define EAFNOSUPPORT	10047
#define EALREADY		10037
#define ECONNABORTED	10053
#define ECONNREFUSED	10061
#define ECONNRESET		10054
#define EDESTADDRREQ	10039
/*	defined in <cerrno>
#define	EFAULT			10014
*/
#define EHOSTDOWN		10064
#define EHOSTUNREACH	10065
#define	EINPROGRESS		10036
/*	defined in <cerrno>
#define	EINTR			10004
#define	EINVAL			10022
*/
#define EISCONN			10056
/*	defined in <cerrno>
#define	EMFILE			10024
*/
#define EMSGSIZE		10040
#define ENETDOWN		10050
#define ENETRESET		10052
#define ENETUNREACH		10051
#define ENOBUFS			10055
#define ENOPROTOOPT		10042
#define ENOTCONN		10057
#define ENOTSOCK		10038
#define EOPNOTSUPP		10045
#define EPFNOSUPPORT	10046
#define EPROCLIM		10067
#define EPROTONOSUPPORT	10043
#define EPROTOTYPE		10041
#define ESHUTDOWN		10058
#define ESOCKTNOSUPPORT	10044
#define	ETIMEDOUT		10060
#define EWOULDBLOCK		EAGAIN

#endif	//	K2_RT_MSVCRT

#endif	//	!K2_ERRNO_H
