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
#ifndef K2_IPV6_ADDR_H
#define K2_IPV6_ADDR_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_STDINT_H
#   include <k2/stdint.h>
#endif
#ifndef K2_NET_H
#   include <k2/net.h>
#endif
#ifndef K2_MUTEX_H
#   include <k2/mutex.h>
#endif
#ifndef K2_SINGLETON_H
#   include <k2/singleton.h>
#endif

#ifndef K2_STD_H_VECTOR
#   include <vector>
#   define  K2_STD_H_VECTOR
#endif
#ifndef K2_STD_H_STRING
#   include <string>
#   define  K2_STD_H_STRING
#endif
#ifndef K2_STD_H_FUNCTIONAL
#   include <functional>
#   define  K2_STD_H_FUNCTIONAL
#endif

struct in6_addr;
struct sockaddr_in6;

namespace k2
{

    namespace ipv6
    {

        /** \defgroup   Networking
        */

        /**
        *   \ingroup    Networking
        *
        *   \brief      IP ver.6 interface (Layer 3) address
        *
        *   Also see ipv6::transport_addr.
        */
        class interface_addr
        {
        public:
            static const size_t size = 128 / 8;

            K2_DLSPEC interface_addr ();
            K2_DLSPEC interface_addr (const in6_addr& bsd_if_addr);
            K2_DLSPEC operator const in6_addr () const;

            K2_DLSPEC explicit interface_addr (const uint8_t* data);
            K2_DLSPEC const uint8_t* data () const;

            K2_DLSPEC uint32_t hash () const;
            K2_DLSPEC int compare (const interface_addr& rhs) const;

        private:
            uint8_t     m_data[size];
        };
        inline bool operator== (const interface_addr& lhs, const interface_addr& rhs)
        {
            return  lhs.compare(rhs) == 0;
        }
        inline bool operator!= (const interface_addr& lhs, const interface_addr& rhs)
        {
            return  !(lhs == rhs);
        }
        inline bool operator< (const interface_addr& lhs, const interface_addr& rhs)
        {
            return  lhs.compare(rhs) < 0;
        }


        /**
        *   \ingroup    Networking
        *   \brief      IP ver.4 transport (Layer 4) protocol (i.e. TCP and UDP) address
        *
        *   Also see interface_addr.
        */
        class transport_addr
        {
        public:
            K2_DLSPEC transport_addr ();
            K2_DLSPEC transport_addr (
                const interface_addr& if_addr,
                host16_t port,
                host32_t flowinfo = 0,
                host32_t scode_id = 0);
            K2_DLSPEC transport_addr (const sockaddr_in6& bsd_tp_addr);
            K2_DLSPEC operator const sockaddr_in6 () const;

            K2_DLSPEC const interface_addr& if_addr () const;
            K2_DLSPEC host16_t              port () const;
            K2_DLSPEC host32_t              flowinfo () const;
            K2_DLSPEC host32_t              scope_id () const;

            K2_DLSPEC interface_addr&       if_addr ();
            K2_DLSPEC host16_t&             port ();
            K2_DLSPEC host32_t&             flowinfo ();
            K2_DLSPEC host32_t&             scope_id ();

            K2_DLSPEC uint32_t hash () const;
            K2_DLSPEC int compare (const transport_addr& rhs) const;

        private:
            interface_addr  m_if_addr;
            host16_t        m_port;
            host32_t        m_flowinfo; //  20 bits
            host32_t        m_scope_id;
        };

        inline bool operator == (const transport_addr& lhs, const transport_addr& rhs)
        {
            return  (lhs.compare(rhs) == 0);
        }
        inline bool operator != (const transport_addr& lhs, const transport_addr& rhs)
        {
            return  !(lhs == rhs);
        }
        inline bool operator < ( const transport_addr& lhs, const transport_addr& rhs)
        {
            return  (lhs.compare(rhs) < 0);
        }

    }   //  namespace ipv6

}   //  namespace k2

#endif  //  !K2_IPV6_ADDR_H
