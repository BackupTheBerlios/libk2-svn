/*
 * Copyright (c) 2003, 2004, Kenneth Chang-Hsing Ho <kenho@bluebottle.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_SOCKET_H
#define K2_SOCKET_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_NET_H
#   include <k2/net.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif
#ifndef K2_EXCEPTION_H
#   include <k2/exception.h>
#endif

namespace k2
{

    class socket_desc
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        enum family_enum
        {
            family_ipv4,
            family_ipv6
        };
        enum type_enum
        {
            type_dgram,
            type_stream
        };

        K2_DLSPEC socket_desc (family_enum, type_enum);
        K2_DLSPEC socket_desc (int get, bool own);
        K2_DLSPEC virtual ~socket_desc ();

        K2_DLSPEC int     get () const;

    private:
        int         m_desc;
        bool        m_own;
    };

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
    struct socket_connection_error : runtime_error, socket_errno
    {
        explicit socket_connection_error (const char* what = "k2::socket_connection_error")
            :   runtime_error(what) {};
    };
    /*  \ingroup    Networking
    */

}   //  namespace k2

#endif  //  !K2_SOCKET_H
