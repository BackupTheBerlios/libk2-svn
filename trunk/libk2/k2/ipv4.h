/*  libk2   <k2/ipv4.h>

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
#ifndef K2_IPV4_H
#define K2_IPV4_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_STDINT_H
#   include <k2/stdint.h>
#endif
#ifndef K2_BYTE_MANIP_H
#   include <k2/byte_manip.h>
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

struct in_addr;
struct sockaddr_in;

namespace k2
{

    class time_span;

    namespace ipv4
    {
        class dns_util;
    };

    /** \defgroup   Networking
    */

    /**
    *   \ingroup    Networking
    *
    *   \brief      IP ver.4 address
    *
    *   Also see ipv4::layer4_addr.
    */
    class ipv4_addr
    {
    public:
        static const uint32_t  unspecified = 0;

        ipv4_addr () : m_host32(unspecified) {}
        ipv4_addr (uint32_t host32) : m_host32(host32) {}
        K2_DLSPEC ipv4_addr (
            uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3);
        /**
        *   \bug    No error checking if \a ip_string is not a valid aaa.bbb.ccc.ddd format string.
        */
        K2_DLSPEC explicit ipv4_addr (const std::string& string);
        K2_DLSPEC ipv4_addr (const in_addr& other);
        K2_DLSPEC operator const in_addr () const;

        uint8_t&    byte0 () { return this->byte_at(0); }
        uint8_t&    byte1 () { return this->byte_at(1); }
        uint8_t&    byte2 () { return this->byte_at(2); }
        uint8_t&    byte3 () { return this->byte_at(3); }

        uint8_t     byte0 () const { return this->byte_at(0); }
        uint8_t     byte1 () const { return this->byte_at(1); }
        uint8_t     byte2 () const { return this->byte_at(2); }
        uint8_t     byte3 () const { return this->byte_at(3); }

        uint32_t host32 () const { return m_host32; }
        uint32_t net32 () const  { return host2net(m_host32); }

        friend bool operator== (const ipv4_addr& lhs, const ipv4_addr& rhs);
        friend bool operator!= (const ipv4_addr& lhs, const ipv4_addr& rhs);
        friend bool operator< ( const ipv4_addr& lhs, const ipv4_addr& rhs);
        friend bool operator> ( const ipv4_addr& lhs, const ipv4_addr& rhs);
        friend bool operator<= (const ipv4_addr& lhs, const ipv4_addr& rhs);
        friend bool operator>= (const ipv4_addr& lhs, const ipv4_addr& rhs);
        K2_DLSPEC friend std::ostream& operator << (std::ostream& out, const ipv4_addr& ipaddr);
        K2_DLSPEC friend std::istream& operator >> (std::istream& in, ipv4_addr& ipaddr);

        friend class ipv4::dns_util;

    protected:
        uint8_t&    byte_at (int idx);
        uint8_t     byte_at (int idx) const;

    private:
        uint32_t    m_host32;
    };
    inline bool operator == (const ipv4_addr& lhs, const ipv4_addr& rhs)
    {
        return  lhs.m_host32 == rhs.m_host32;
    }
    inline bool operator != (const ipv4_addr& lhs, const ipv4_addr& rhs)
    {
        return  !(lhs == rhs);
    }
    inline bool operator < ( const ipv4_addr& lhs, const ipv4_addr& rhs)
    {
        return  lhs.m_host32 < rhs.m_host32;
    }
    inline bool operator > ( const ipv4_addr& lhs, const ipv4_addr& rhs)
    {
        return  lhs.m_host32 > rhs.m_host32;
    }
    inline bool operator <= (const ipv4_addr& lhs, const ipv4_addr& rhs)
    {
        return  !(lhs > rhs);
    }
    inline bool operator >= (const ipv4_addr& lhs, const ipv4_addr& rhs)
    {
        return  !(lhs < rhs);
    }
    K2_DLSPEC std::ostream& operator << (std::ostream& out, const ipv4_addr& ipaddr);
    K2_DLSPEC std::istream& operator >> (std::istream& in, ipv4_addr& ipaddr);

    namespace ipv4
    {

        typedef ipv4_addr   ip_addr;

        /*
        *   \ingroup    Networking
        *   \bug        dns_util functions are not thread-safe, yet!!!
        */
        class dns_util
        {
        public:
            K2_DLSPEC static bool lookup (const std::string& fqdn, std::vector<ip_addr>& ips);
            K2_DLSPEC static bool lookup (const std::string& fqdn, ip_addr& ip);

        private:
            typedef mutex                               lock_type;
            typedef lock_type::scoped_guard             scoped_guard;
            typedef class_singleton<dns_util, mutex>    class_lock;
        };

        /**
        *   \ingroup    Networking
        *   \brief      IP ver.4 layer 4 (transport layer) protocol (i.e. TCP and UDP) address
        *
        *   Also see ipv4_addr.
        */
        class layer4_addr
        {
        public:
            layer4_addr () : ip(), port(0) {}
            layer4_addr (const ip_addr& rhs_ip, uint16_t rhs_port)
            :   ip(rhs_ip), port(rhs_port) {}
            K2_DLSPEC layer4_addr (const sockaddr_in& rhs);
            K2_DLSPEC operator const sockaddr_in () const;

            ip_addr     ip;
            uint16_t    port;
        };

        inline bool operator == (const layer4_addr& lhs, const layer4_addr& rhs)
        {
            return  ((lhs.ip == rhs.ip) && (lhs.port == rhs.port));
        }
        inline bool operator != (const layer4_addr& lhs, const layer4_addr& rhs)
        {
            return  !(lhs == rhs);
        }
        inline bool operator < ( const layer4_addr& lhs, const layer4_addr& rhs)
        {
            return  ((lhs.ip < rhs.ip) || ((lhs.ip == rhs.ip) && (lhs.port < rhs.port)));
        }
        inline bool operator > ( const layer4_addr& lhs, const layer4_addr& rhs)
        {
            return  ((lhs.ip > rhs.ip) || ((lhs.ip == rhs.ip) && (lhs.port > rhs.port)));
        }
        inline bool operator <= (const layer4_addr& lhs, const layer4_addr& rhs)
        {
            return  !(lhs > rhs);
        }
        inline bool operator >= (const layer4_addr& lhs, const layer4_addr& rhs)
        {
            return  !(lhs < rhs);
        }

    }   //  namespace ipv4

}   //  namespace k2

#endif  //  !K2_IPV4_H
