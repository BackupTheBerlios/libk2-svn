/*  libk2   <socket_error.cpp>

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
#include <k2/socket_error.h>

#if defined(K2_RT_MSVCRT)
#   include <winsock2.h>
#   pragma warning (disable : 4800) //  what is this for???
#endif

#ifndef K2_ERRNO_H
#   include <k2/errno.h>
#endif

namespace k2
{

    //static
    int socket_errno::get_last ()
#if defined(K2_RT_MSVCRT)
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
                err = EWOULDBLOCK;
                break;
        }
        return  err;
    }
#else
    {
        return  errno;
    }
#endif

}   //  namespace k2
