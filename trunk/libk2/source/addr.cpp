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
#ifndef K2_IPV4_ADDR_H
#   include <k2/ipv4_addr.h>
#endif
#ifndef K2_IPV6_ADDR_H
#   include <k2/ipv6_addr.h>
#endif
#ifndef K2_BYTE_MANIP_H
#   include <k2/byte_manip.h>
#endif
#ifndef K2_ASSERT_H
#   include <k2/assert.h>
#endif

#ifndef K2_STD_H_IOSTREAM
#   include <iostream>
#endif

#if defined(__linux__)
//  !kh! need to get rid of unused headers
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/time.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <fcntl.h>
#elif defined(WIN32)
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma warning (disable : 4800) //  what for???
    typedef int socklen_t;
#else
#   error   "libk2: Manual attention required."
#endif

//static
const k2::ipv4::interface_addr::loopback_tag
    k2::ipv4::interface_addr::loopback;
//static
const k2::ipv4::interface_addr::any_tag
    k2::ipv4::interface_addr::any;

k2::ipv4::interface_addr::interface_addr ()
{
    std::fill(m_data, m_data + interface_addr::size, 0);
}
k2::ipv4::interface_addr::interface_addr (loopback_tag)
{
    static uint8_t loopback_data[] = {127, 0, 0, 1};
    std::copy(loopback_data, loopback_data + sizeof(loopback_data), m_data);
}
k2::ipv4::interface_addr::interface_addr (any_tag)
{
    std::fill(m_data, m_data + interface_addr::size, 0);
}
k2::ipv4::interface_addr::interface_addr (const in_addr& bsd_if_addr)
{
    const uint8_t*  src =
        reinterpret_cast<const uint8_t*>(&bsd_if_addr.s_addr);
    std::copy(src, src + interface_addr::size, m_data);
}
k2::ipv4::interface_addr::operator const in_addr () const
{
    in_addr ret;
    uint8_t*  dst =
        reinterpret_cast<uint8_t*>(&ret.s_addr);
    std::copy(m_data, m_data + interface_addr::size, dst);
    return  ret;
}
k2::ipv4::interface_addr::interface_addr (const uint8_t* data)
{
    std::copy(
        data,
        data + interface_addr::size,
        m_data);
}
const k2::uint8_t*
k2::ipv4::interface_addr::data () const
{
    return  m_data;
}
k2::uint32_t
k2::ipv4::interface_addr::hash () const
{
    return *reinterpret_cast<const uint32_t*>(m_data);
}
int
k2::ipv4::interface_addr::compare (const interface_addr& rhs) const
{
    return  std::lexicographical_compare(
        m_data,
        m_data + interface_addr::size,
        rhs.data(),
        rhs.data() + interface_addr::size);
}



k2::ipv4::transport_addr::transport_addr ()
:   m_if_addr()
,   m_port(0)
{
}
k2::ipv4::transport_addr::transport_addr (const interface_addr& if_addr, host16_t port)
:   m_if_addr(if_addr)
,   m_port(port)
{
}
k2::ipv4::transport_addr::transport_addr (const sockaddr_in& bsd_tp_addr)
:   m_if_addr(bsd_tp_addr.sin_addr),
    m_port(net2host(bsd_tp_addr.sin_port))
{
}
k2::ipv4::transport_addr::operator const sockaddr_in () const
{
    sockaddr_in ret;
    ret.sin_family      = AF_INET;
    ret.sin_addr        = in_addr(m_if_addr);
    ret.sin_port        = host2net(m_port);
    return  ret;
}
const k2::ipv4::interface_addr&
k2::ipv4::transport_addr::if_addr () const
{
    return  m_if_addr;
}
k2::host16_t
k2::ipv4::transport_addr::port () const
{
    return  m_port;
}
k2::ipv4::interface_addr&
k2::ipv4::transport_addr::if_addr ()
{
    return  m_if_addr;
}
k2::host16_t&
k2::ipv4::transport_addr::port ()
{
    return  m_port;
}
k2::uint32_t
k2::ipv4::transport_addr::hash () const
{
    uint8_t hash_buf[interface_addr::size];
    std::copy(
        m_if_addr.data(),
        m_if_addr.data() + interface_addr::size,
        hash_buf);
    hash_buf[0] ^= uint8_t(m_port >> 8);
    hash_buf[interface_addr::size - 1] ^= uint8_t((m_port << 8) >> 8);
    return  *reinterpret_cast<const uint32_t*>(hash_buf);
}
int
k2::ipv4::transport_addr::compare (const transport_addr& rhs) const
{
    if(m_if_addr.compare(rhs.if_addr()) > 0)
        return  1;
    if(m_if_addr.compare(rhs.if_addr()) < 0)
        return  -1;

    if(m_port > rhs.port())
        return  1;
    if(m_port < rhs.port())
        return  -1;

    return  0;
}

std::vector<k2::ipv4::transport_addr>
k2::ipv4::nonpublic::getaddrinfo_impl (
    k2::type_tag<k2::ipv4::transport_addr>,
    bool                is_udp,
    bool                is_passive,
    bool                is_numeric_host,
    const char*         host,
    const char*         port_num)
{
    ::addrinfo          hint;
    hint.ai_flags       = 0;
    hint.ai_flags       |= (is_passive ? AI_PASSIVE : 0);
    hint.ai_flags       |= (is_numeric_host ? AI_NUMERICHOST : 0);
    hint.ai_family      = PF_INET;
    hint.ai_socktype    = (is_udp ? SOCK_DGRAM : SOCK_STREAM);

    ::addrinfo* results;
    int ret = ::getaddrinfo(
        host,
        port_num,
        &hint,
        &results);

    try
    {
        if(ret != 0)
        {
            const char* err = 0;
            switch(ret)
            {
                case EAI_NONAME:
                case EAI_SERVICE:
                    break;
                default:
                    err = ::gai_strerror(ret);
            }
            if(err)
                throw   k2::runtime_error(err);
        }

        std::vector<k2::ipv4::transport_addr>   addrs;
        if(ret == 0)
        {
            addrs.reserve(8);
            for(;results != 0; results = results->ai_next);
            {
                assert(results->ai_family == hint.ai_family);
                assert(results->ai_family == hint.ai_socktype);
                addrs.push_back(transport_addr(*reinterpret_cast<const sockaddr_in*>(results->ai_addr)));
            }
        }
        return  addrs;
    }
    catch(std::exception&)
    {
        ::freeaddrinfo(results);
        throw;
    }

    ::freeaddrinfo(results);
}


k2::ipv6::interface_addr::interface_addr ()
{
    std::fill(m_data, m_data + interface_addr::size, 0);
}
k2::ipv6::interface_addr::interface_addr (const in6_addr& bsd_if_addr)
{
    const uint8_t*  src =
#ifdef  H2_HAS_POSIX_API
    //  BSD Socket
        reinterpret_cast<const uint8_t*>(bsd_if_addr.s6_addr);
#else
    //  WinSocket
        reinterpret_cast<const uint8_t*>(bsd_if_addr.u.Byte);
#endif
    std::copy(src, src + interface_addr::size, m_data);
}
k2::ipv6::interface_addr::operator const in6_addr () const
{
    in6_addr    ret;
    uint8_t*    dst =
#ifdef  H2_HAS_POSIX_API
    //  BSD Socket
        reinterpret_cast<uint8_t*>(ret.s6_addr);
#else
    //  WinSocket
        reinterpret_cast<uint8_t*>(ret.u.Byte);
#endif
    std::copy(m_data, m_data + interface_addr::size, dst);
    return  ret;
}
k2::ipv6::interface_addr::interface_addr (const uint8_t* data)
{
    std::copy(data, data + interface_addr::size, m_data);
}
const k2::uint8_t*
k2::ipv6::interface_addr::data () const
{
    return  m_data;
}
k2::uint32_t
k2::ipv6::interface_addr::hash () const
{
    const uint32_t* p = reinterpret_cast<const uint32_t*>(m_data);
    return (p[0] ^ p[1]) + (p[2] ^ p[3]);
}
int
k2::ipv6::interface_addr::compare (const interface_addr& rhs) const
{
    return  std::lexicographical_compare(
        m_data,
        m_data + interface_addr::size,
        rhs.data(),
        rhs.data() + interface_addr::size);
}


k2::ipv6::transport_addr::transport_addr ()
:   m_port(0)
,   m_flowinfo(0)
,   m_scope_id(0)
{
}
k2::ipv6::transport_addr::transport_addr (
    const interface_addr&   if_addr,
    host16_t                port,
    host32_t                flowinfo,
    host32_t                scope_id)
:   m_if_addr(if_addr)
,   m_port(port)
,   m_flowinfo(flowinfo)
,   m_scope_id(scope_id)
{
}
k2::ipv6::transport_addr::transport_addr (const sockaddr_in6& bsd_tp_addr)
:   m_if_addr(bsd_tp_addr.sin6_addr)
,   m_port(net2host(bsd_tp_addr.sin6_port))
,   m_flowinfo(net2host(bsd_tp_addr.sin6_flowinfo))
,   m_scope_id(net2host(bsd_tp_addr.sin6_scope_id))
{
}
k2::ipv6::transport_addr::operator const sockaddr_in6 () const
{
    sockaddr_in6    ret;
    ret.sin6_family   = AF_INET6;
    ret.sin6_addr     = m_if_addr;
    ret.sin6_port     = host2net(m_port);
    ret.sin6_flowinfo = host2net(m_flowinfo & 0x000fffff);
    ret.sin6_scope_id = host2net(m_scope_id);

    return  ret;
}
const k2::ipv6::interface_addr&
k2::ipv6::transport_addr::if_addr () const
{
    return  m_if_addr;
}
k2::host16_t
k2::ipv6::transport_addr::port () const
{
    return  m_port;
}
k2::host32_t
k2::ipv6::transport_addr::flowinfo () const
{
    return  m_flowinfo & 0x000fffff;
}
k2::host32_t
k2::ipv6::transport_addr::scope_id () const
{
    return  m_scope_id;
}

k2::ipv6::interface_addr&
k2::ipv6::transport_addr::if_addr ()
{
    return  m_if_addr;
}
k2::host16_t&
k2::ipv6::transport_addr::port ()
{
    return  m_port;
}
k2::host32_t&
k2::ipv6::transport_addr::flowinfo ()
{
    return  (m_flowinfo &= 0x000fffff);
}
k2::host32_t&
k2::ipv6::transport_addr::scope_id ()
{
    return  m_scope_id;
}
k2::uint32_t
k2::ipv6::transport_addr::hash () const
{
    return  m_if_addr.hash() ^ (((m_flowinfo & 0x000fffff) << 6) ^ m_scope_id);
}
int
k2::ipv6::transport_addr::compare (const transport_addr& rhs) const
{
    if(m_if_addr.compare(rhs.if_addr()) > 0)
        return  1;
  if(m_if_addr.compare(rhs.if_addr()) < 0)
        return  -1;

    if(m_port > rhs.port())
        return  1;
    if(m_port < rhs.port())
        return  -1;

    if((m_flowinfo & 0x000fffff) > rhs.flowinfo())
        return  1;
    if(m_flowinfo < rhs.flowinfo())
        return  -1;

    if(m_scope_id > rhs.scope_id())
        return  1;
    if(m_scope_id < rhs.scope_id())
        return  -1;

    return  0;
}
