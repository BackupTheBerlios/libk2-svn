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
#ifndef K2_IPV4_UDP_H
#define K2_IPV4_UDP_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_IPV4_ADDR_H
#   include <k2/ipv4_addr.h>
#endif
#ifndef K2_SOCKET_H
#   include <k2/socket.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif

namespace k2
{

    /** \defgroup   Networking
    */
    namespace ipv4
    {

        /*  \ingroup    Networking
        */
        class udp_transport
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            static const size_t io_error = size_t(-1);

            K2_DLSPEC explicit udp_transport ();
            K2_DLSPEC explicit udp_transport (const transport_addr& local_addr);
            K2_DLSPEC explicit udp_transport (int udp_desc, bool own);
            K2_DLSPEC ~udp_transport ();

            K2_DLSPEC int       get_desc () const;
            K2_DLSPEC const transport_addr local_addr () const;

            K2_DLSPEC void      connect (const transport_addr& remote_addr);
            K2_DLSPEC size_t    write (const char* buf, size_t bytes, const transport_addr& remote_addr);

            K2_DLSPEC size_t    read (char* buf, size_t bytes, transport_addr& remote_addr);
            K2_DLSPEC size_t    read (char* buf, size_t bytes, transport_addr& remote_addr, const time_span& timeout);


        private:
            socket_desc m_desc;
        };

    }   //  namespace k2

}   //  namespace k2

#endif  //  !K2_IPV4_UDP_H
