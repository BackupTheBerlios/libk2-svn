/*
 * Copyright (c) 2003, 2004, 2005,
 * Kenneth Chang-Hsing Ho <kenho@bluebottle.com> All rights reterved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of k2, libk2, copyright owners nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT OWNERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_ERRNO_H
#define K2_ERRNO_H

#ifndef K2_STD_H_CEERNO
#   define  K2_STD_H_CEERNO
#   include <cerrno>
#endif

#if defined(WIN32)

//  Defines missing errno constants in Win32 <cerrno>.
//  Error number constants used are values from <winsock2.h>.
//  For those already defined in <cerrno>, the values in
//  cerrno are used, for example WSAEFAULT is 10014 in
//  <winsock2.h> but EFAULT is something else in <cerrno>,
//  in these cases, libk2 sticks with the standard
//  <cerrno>. These values are used to evaluate against
//  return value of socket_error::gat_last(), to avoid
//  using ugly WSAEXXXXXX macros and avoid clients' need
//  to include <winsock2.h>

/*  defined in <cerrno>
#define EACCES          10013
*/
#define EADDRINUSE      10048
#define EADDRNOTAVAIL   10049
#define EAFNOSUPPORT    10047
#define EALREADY        10037
#define ECONNABORTED    10053
#define ECONNREFUSED    10061
#define ECONNRESET      10054
#define EDESTADDRREQ    10039
/*  defined in <cerrno>
#define EFAULT          10014
*/
#define EHOSTDOWN       10064
#define EHOSTUNREACH    10065
#define EINPROGRESS     10036
/*  defined in <cerrno>
#define EINTR           10004
#define EINVAL          10022
*/
#define EISCONN         10056
/*  defined in <cerrno>
#define EMFILE          10024
*/
#define EMSGSIZE        10040
#define ENETDOWN        10050
#define ENETRESET       10052
#define ENETUNREACH     10051
#define ENOBUFS         10055
#define ENOPROTOOPT     10042
#define ENOTCONN        10057
#define ENOTSOCK        10038
#define EOPNOTSUPP      10045
#define EPFNOSUPPORT    10046
#define EPROCLIM        10067
#define EPROTONOSUPPORT 10043
#define EPROTOTYPE      10041
#define ESHUTDOWN       10058
#define ESOCKTNOSUPPORT 10044
#define ETIMEDOUT       10060
#define EWOULDBLOCK     EAGAIN

#endif  //  K2_RT_MSVCRT

#endif  //  !K2_ERRNO_H
