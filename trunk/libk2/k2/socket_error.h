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
#ifndef K2_SOCKET_ERROR_H
#define K2_SOCKET_ERROR_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_EXCEPTION_H
#   include <k2/exception.h>
#endif

namespace k2
{

    /** \defgroup   Networking
    */

    /*  \ingroup    Networking
    */
    class socket_errno
    {
    public:
        socket_errno ()
        :   m_errno(get_last()) {}

        int get () const { return m_errno; }

        K2_DLSPEC static int get_last ();

    private:
        int m_errno;
    };

    /*  \ingroup    Networking
    */
    struct socket_accept_error : bad_resource_alloc, socket_errno
    {
        explicit socket_accept_error (const char* what = "k2::socket_accept_error")
            :   bad_resource_alloc(what) {};
    };
    /*  \ingroup    Networking
    */
    struct socket_open_error : bad_resource_alloc, socket_errno
    {
        explicit socket_open_error (const char* what = "k2::socket_open_error")
            :   bad_resource_alloc(what) {};
    };
    /*  \ingroup    Networking
    */
    struct socket_bind_error : runtime_error, socket_errno
    {
        explicit socket_bind_error (const char* what = "k2::socket_bind_error")
            :   runtime_error(what) {};
    };
    /*  \ingroup    Networking
    */
    struct socket_connect_error : runtime_error, socket_errno
    {
        explicit socket_connect_error (const char* what = "k2::socket_connect_error")
            :   runtime_error(what) {};
    };
    /*  \ingroup    Networking
    */
    struct socket_timedout_error : timedout_error, socket_errno
    {
        explicit socket_timedout_error (const char* what = "k2::socket_timedout_error")
            :   timedout_error(what) {};
    };
    /*  \ingroup    Networking
    */
    struct socket_tcp_io_error : runtime_error, socket_errno
    {
        explicit socket_tcp_io_error (const char* what = "k2::socket_tcp_io_error")
            :   runtime_error(what) {};
    };
    /*  \ingroup    Networking
    */
    struct socket_udp_io_error : runtime_error, socket_errno
    {
        explicit socket_udp_io_error (const char* what = "k2::socket_udp_io_error")
            :   runtime_error(what) {};
    };

}   //  namespace k2

#endif  //  !K2_SOCKET_ERROR_H
