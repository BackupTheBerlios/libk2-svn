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
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <k2/socket.h>

#if !defined(WIN32)
//  !kh! need to get rid of unused headers
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/time.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <fcntl.h>
#else
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma warning (disable : 4800) //  what for???
    typedef int socklen_t;
#endif

k2::socket_desc::~socket_desc ()
{
    if(m_own)
    {
#if !defined(WIN32)
        //  Generic
        close(m_desc);
#else
        //  Windows Socket specific
        closesocket(m_desc);
#endif
    }
}
int
k2::socket_desc::get () const
{
    return  m_desc;
}
k2::socket_desc::socket_desc (family_enum family, type_enum type)
:   m_desc(-1)
,   m_own(true)
{
    m_desc = (int)socket(
        (family == family_ipv4 ? AF_INET : AF_INET6),
        (type == type_stream ? SOCK_STREAM : SOCK_DGRAM),
        0);

    if(m_desc == -1)
        throw   k2::socket_open_error();
}
k2::socket_desc::socket_desc (int get, bool own)
:   m_desc(get)
,   m_own(own)
{
    if(m_desc == -1)
        throw   k2::socket_open_error();
}


#ifndef K2_ERRNO_H
#   include <k2/errno.h>
#endif

#if defined(WIN32)
#   include <winsock2.h>
#   pragma warning (disable : 4800) //  what is this for???
int
k2::socket_errno::get_last ()
{
    int err = WSAGetLastError();
    switch (err)
    {
        case WSAEACCES:
        case WSAEFAULT:
        case WSAEINTR:
        case WSAEINVAL:
        case WSAEMFILE:
            err -= 10000;
            break;
        case WSAEWOULDBLOCK:
            err = EAGAIN;   //  same thing as EWOULDBLOCK, see <k2/errno.h>
            break;
    }
    return  err;
}
#else
int
k2::socket_errno::get_last ()
{
    return  errno;
}
#endif
