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
#ifndef K2_IPV4_TCP_H
#define K2_IPV4_TCP_H

#ifndef K2_IPV4_ADDR_H
#   include <k2/ipv4_addr.h>
#endif
#ifndef K2_SOCKET_H
#   include <k2/socket.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif

#ifndef K2_STD_H_MEMORY
#   include <memory>
#   define  K2_STD_H_MEMORY
#endif

namespace k2
{

    /** \defgroup   Networking
    */

    namespace ipv4ex
    {
        using ipv4::interface_addr;
        using ipv4::transport_addr;

        class tcp_local_binding
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            K2_DLSPEC explicit tcp_local_binding (const transport_addr&);
            K2_DLSPEC ~tcp_local_binding ();

        private:
            socket_desc m_desc;
        };
    }
    namespace ipv4
    {

        class tcp_local_binding
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            K2_DLSPEC explicit tcp_local_binding (const transport_addr&);
            K2_DLSPEC ~tcp_local_binding ();

        private:
            socket_desc m_desc;
        };

        class tcp_transport;

        /*  \ingroup    Networking
        */
        class tcp_listener
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            K2_DLSPEC explicit tcp_listener (const transport_addr& local_addr);
            K2_DLSPEC explicit tcp_listener (int desc, bool own);
            K2_DLSPEC ~tcp_listener ();

            K2_DLSPEC int   get_desc () const;
            const transport_addr local_addr () const;

            K2_DLSPEC std::auto_ptr<tcp_transport>
                accept ();
            K2_DLSPEC std::auto_ptr<tcp_transport>
                accept (const time_span& timeout);

        protected:
            friend class tcp_transport;
            K2_DLSPEC int accept_desc ();
            K2_DLSPEC int accept_desc (const time_span& timeout);

        private:
            socket_desc m_desc;
        };

        /*  \ingroup    Networking
        */
        class tcp_transport
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            K2_DLSPEC explicit tcp_transport (const transport_addr& remote_addr);
            K2_DLSPEC explicit tcp_transport (const transport_addr& remote_addr, const time_span& timeout);
            K2_DLSPEC tcp_transport (const transport_addr& local_addr, const transport_addr& remote_addr);
            K2_DLSPEC tcp_transport (const transport_addr& local_addr, const transport_addr& remote_addr, const time_span& timeout);
            K2_DLSPEC explicit tcp_transport (tcp_listener& listener);
            K2_DLSPEC tcp_transport (tcp_listener& listener, const time_span& timeout);
            K2_DLSPEC tcp_transport (int tcp_desc, bool own);
            K2_DLSPEC ~tcp_transport ();

            K2_DLSPEC int   get_desc () const;
            K2_DLSPEC const transport_addr local_addr () const;
            K2_DLSPEC const transport_addr remote_addr () const;

            K2_DLSPEC size_t write (const char* buf, size_t bytes);
            K2_DLSPEC size_t write_all (const char* buf, size_t bytes);
            K2_DLSPEC size_t write_all (const char* buf, size_t bytes, const time_span& timeout);

            K2_DLSPEC size_t read (char* buf, size_t bytes);
            K2_DLSPEC size_t read_all (char* buf, size_t bytes);
            K2_DLSPEC size_t read_all (char* buf, size_t bytes, const time_span& timeout);

        private:
            socket_desc m_desc;
        };

    }

}   //  namespace k2

#endif  //  !K2_IPV4_TCP_H
