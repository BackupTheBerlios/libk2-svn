/*
    Copyright (c) 2003, 2004, Kenneth Chang-Hsing Ho
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the k2 nor the names of its contributors may be used
      to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#include <k2/socket_error.h>

#if !defined(K2_HAS_POSIX_API)
#   if defined(K2_HAS_WIN32_API)
#       include <winsock2.h>
#       pragma warning (disable : 4800) //  what is this for???
#   else
#       error   "libk2: How to map error codes?"
#   endif
#endif

#ifndef K2_ERRNO_H
#   include <k2/errno.h>
#endif

#if defined(K2_RT_MSVCRT)

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

