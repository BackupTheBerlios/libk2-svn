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
#ifndef K2_IPV4_ADDR_H
#define K2_IPV4_ADDR_H

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
#ifndef K2_UTILITY_H
#   include <k2/utility.h>
#endif

#ifndef K2_STD_H_VECTOR
#   include <vector>
#   define  K2_STD_H_VECTOR
#endif
#ifndef K2_STD_H_STRING
#   include <string>
#   define  K2_STD_H_STRING
#endif
#ifndef K2_STD_H_CASSERT
#   include <cassert>
#   define  K2_STD_H_CASSERT
#endif
#ifndef K2_STD_H_FUNCTIONAL
#   include <functional>
#   define  K2_STD_H_FUNCTIONAL
#endif

struct in_addr;
struct sockaddr_in;

namespace k2
{

    namespace ipv4
    {

        /** \defgroup   Networking
        */

        /**
        *   \ingroup    Networking
        *
        *   \brief      IP ver.4 interface (Layer 3) address
        *
        *   Also see ipv4::transport_addr.
        */
        class interface_addr
        {
        public:
            static const size_t size = 32 / 8;
            struct loopback_tag
            {
            };
            struct any_tag
            {
            };
            K2_DLSPEC static const loopback_tag   loopback;
            K2_DLSPEC static const any_tag        any;

            K2_DLSPEC interface_addr ();
            K2_DLSPEC interface_addr (loopback_tag);
            K2_DLSPEC interface_addr (any_tag);
            K2_DLSPEC interface_addr (const in_addr& bsd_if_addr);
            K2_DLSPEC explicit interface_addr (const uint8_t* data);

            K2_DLSPEC operator const in_addr () const;
            K2_DLSPEC const uint8_t* data () const;

            K2_DLSPEC uint32_t hash () const;
            K2_DLSPEC int compare (const interface_addr& rhs) const;

        private:
            uint8_t m_data[size];
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
        *   \brief      IP ver.4 transport (Layer 4) protocol (e.g. TCP and UDP) address
        *
        *   Also see ipv4::interface_addr.
        */
        class transport_addr
        {
        public:
            K2_DLSPEC transport_addr ();
            K2_DLSPEC transport_addr (const interface_addr& if_addr, host16_t port);
            K2_DLSPEC transport_addr (const sockaddr_in& bsd_tp_addr);
            K2_DLSPEC operator const sockaddr_in () const;

            K2_DLSPEC const interface_addr& if_addr () const;
            K2_DLSPEC host16_t              port () const;

            K2_DLSPEC interface_addr&       if_addr ();
            K2_DLSPEC host16_t&             port ();

            K2_DLSPEC uint32_t hash () const;
            K2_DLSPEC int compare (const transport_addr& rhs) const;

        private:
            interface_addr  m_if_addr;
            host16_t        m_port;
        };
        inline bool operator == (const transport_addr& lhs, const transport_addr& rhs)
        {
            return  (lhs.if_addr() == rhs.if_addr()) && (lhs.port() == rhs.port());
        }
        inline bool operator != (const transport_addr& lhs, const transport_addr& rhs)
        {
            return  !(lhs == rhs);
        }
        inline bool operator < ( const transport_addr& lhs, const transport_addr& rhs)
        {
            return (lhs.compare(rhs) < 0);
        }


#ifndef DOXYGEN_BLIND

        class udp_transport;
        class tcp_transport;
        class tcp_listener;

        namespace nonpublic
        {

            K2_DLSPEC
            std::vector<transport_addr>
            getaddrinfo_impl (
                type_tag<transport_addr>,
                bool                is_udp,
                bool                is_passive,
                bool                is_numeric_host,
                const char*         host,
                const char*         port_num);


        }   //  namespace nonpublic

#endif  //  !DOXYGEN_BLIND

        template <
            template <typename> class alloc_>
        const std::vector<transport_addr>
        getaddrinfo (
            type_tag<udp_transport>        udp_transport_tag,
            bool                        is_passive,
            bool                        is_numeric_host,
            const std::basic_string<
                char,
                std::char_traits<char>,
                alloc_<char> >&         host,
            const std::basic_string<
                char,
                std::char_traits<char>,
                alloc_<char> >&         port_num)
        {
            return  nonpublic::getaddrinfo_impl(
                type_tag<transport_addr>(),
                true,
                is_passive,
                is_numeric_host,
                host.c_str(),
                port_num.c_str());
        }

        template <
            template <typename> class alloc_>
        const std::vector<transport_addr>
        getaddrinfo (
            type_tag<tcp_transport>        tcp_transport_tag,
            bool                        is_numeric_host,
            const std::basic_string<
                char,
                std::char_traits<char>,
                alloc_<char> >&         host,
            const std::basic_string<
                char,
                std::char_traits<char>,
                alloc_<char> >&         port_num)
        {
            return  nonpublic::getaddrinfo_impl(
                type_tag<transport_addr>(),
                false,
                false,
                is_numeric_host,
                host.c_str(),
                port_num.c_str());
        }

        template <
            template <typename> class alloc_>
        const std::vector<transport_addr>
        getaddrinfo (
            type_tag<tcp_listener>      tcp_listener_tag,
            bool                        is_numeric_host,
            const std::basic_string<
                char,
                std::char_traits<char>,
                alloc_<char> >&         host,
            const std::basic_string<
                char,
                std::char_traits<char>,
                alloc_<char> >&         port_num)
        {
            return  nonpublic::getaddrinfo_impl(
                type_tag<transport_addr>(),
                false,
                true,
                is_numeric_host,
                host.c_str(),
                port_num.c_str());
        }

    }   //  namespace ipv4

}   //  namespace k2

#endif  //  !K2_IPV4_ADDR_H
