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
#   include <k2/ipv4_addr.h>
#endif
#ifndef K2_IPV6_ADDR_H
#   include <k2/ipv6_addr.h>
#endif

#ifndef K2_SOCKET_ERROR_H
#   include <k2/socket_error.h>
#endif
#ifndef K2_IPV4_TCP_H
#   include <k2/ipv4_tcp.h>
#endif
#ifndef K2_IPV4_UDP_H
#   include <k2/ipv4_udp.h>
#endif
#ifndef K2_ERRNO_H
#   include <k2/errno.h>
#endif
#ifndef K2_TIMING_H
#   include <k2/timing.h>
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

#ifdef K2_HAS_POSIX_API
#   include <sys/socket_desc.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/time.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <fcntl.h>
#elif defined(K2_HAS_WIN32_API)
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma warning (disable : 4800) //  what for???
    typedef int socklen_t;
#else
#   error   "libk2: How to include socket_desc header(s)?"
#endif

namespace   //  unnamed
{

    template <typename tp_addr_>
    struct addr_traits
    {
    };
    template <>
    struct addr_traits<k2::ipv4::transport_addr>
    {
        typedef sockaddr_in mapped_type;
        static const int    family = PF_INET;
    };
    template <>
    struct addr_traits<k2::ipv6::transport_addr>
    {
        typedef sockaddr_in6 mapped_type;
        static const int    family = PF_INET6;
    };
    template <>
    struct addr_traits<sockaddr_in>
    {
        typedef k2::ipv4::transport_addr mapped_type;
        static const int    family = PF_INET6;
    };
    template <>
    struct addr_traits<sockaddr_in6>
    {
        typedef k2::ipv6::transport_addr mapped_type;
        static const int    family = PF_INET6;
    };

    void socket_set_nb (int get, bool nb)
    {
#if defined(K2_HAS_POSIX_API)
        //  BSD Socket specific
        int flags = fcntl(get, F_GETFL, 0);
        if(flags != -1 && nb)
        {
            flags |= O_NONBLOCK;
            fcntl(get, F_SETFL, flags);
        }
        else
        {
            flags &= ~O_NONBLOCK;
            fcntl(get, F_SETFL, flags);
        }
#else
        //  Windows Socket specific
        u_long  on = (nb ? 1 : 0);
        ioctlsocket(get, FIONBIO, &on);
#endif
    }
    struct socket_nb
    {
        socket_nb (int get)
        :   get(get)
        {
            socket_set_nb(get, true);
        }
        ~socket_nb ()
        {
            socket_set_nb(get, false);
        }

        int get;
    };

    template <typename tp_addr_>
    int socket_open (bool udp)
    {
        int get = (int)socket(
            addr_traits<tp_addr_>::family,
            udp ? SOCK_DGRAM : SOCK_STREAM,
            0);

        if(get == -1)
            throw   k2::socket_open_error();
        return  get;
    }
    template <typename tp_addr_>
    int tcp_open ()
    {
        return  socket_open<tp_addr_>(false);
    }
    template <typename tp_addr_>
    int udp_open ()
    {
        return  socket_open<tp_addr_>(true);
    }

    void    socket_close (int get)
    {
#if defined(K2_HAS_POSIX_API)
        //  BSD Socket specific
        close(get);
#else
        //  Windows Socket specific
        closesocket(get);
#endif
    }
    enum wait_opt
    {
        wait_read,
        wait_write,
        wait_except
    };
    bool socket_wait (int get, wait_opt opt, const k2::time_span& timeout)
    {
        fd_set  fds;
        FD_ZERO(&fds);
        FD_SET(get, &fds);
        fd_set  *prd_fds = 0;
        fd_set  *pwr_fds = 0;
        fd_set  *pex_fds = 0;

        switch (opt)
        {
        case wait_read:
            prd_fds = &fds;
            break;
        case wait_write:
            pwr_fds = &fds;
            break;
        case wait_except:
            pex_fds = &fds;
            break;
        default:
            k2::runtime_assert(0);
        }

        timeval tv;
        tv.tv_sec   = timeout.in_sec();
        tv.tv_usec  = timeout.msec_of_sec() * 1000;
        if(::select(get + 1, prd_fds, pwr_fds, pex_fds, &tv) < 1)
            return  false;
        else
            return  true;
    }
    bool socket_wait2 (int get, wait_opt opt, k2::time_span& timeout)
    {
#if !defined(K2_OS_LINUX)
        //  See
        //  man 2 select
        //  in Linux man page.
        //  For select() behaves different from other platforms.
        k2::timestamp   pre_wait;
#endif  //  !GNU

        fd_set  fds;
        FD_ZERO(&fds);
        FD_SET(get, &fds);
        fd_set  *prd_fds = 0;
        fd_set  *pwr_fds = 0;
        fd_set  *pex_fds = 0;

        switch (opt)
        {
        case wait_read:
            prd_fds = &fds;
            break;
        case wait_write:
            pwr_fds = &fds;
            break;
        case wait_except:
            pex_fds = &fds;
            break;
        default:
            k2::runtime_assert(0);
        }

        timeval tv;
        tv.tv_sec   = timeout.in_sec();
        tv.tv_usec  = timeout.msec_of_sec() * 1000;
        if(::select(get + 1, prd_fds, pwr_fds, pex_fds, &tv) < 1)
        {
            timeout = k2::time_span(0);
            return  false;
        }
        else
        {
#if defined(K2_OS_LINUX)
            //  Linux select() specific
            timeout = time_span(tv.tv_sec * 1000 + tv.tv_usec / 1000);
#else
            //  General select()
            k2::timestamp now;
            k2::time_span waited = now - pre_wait;
            if(waited > timeout)
                timeout = k2::time_span(0);
            else
                timeout -= waited;
#endif
            return  true;
        }
    }
    void tcp_listen (int get, size_t back_log)
    {
        if(listen(get, (int)back_log) == -1)
        {
            socket_close(get);
            throw   k2::socket_bind_error();
        }
    }
    int tcp_accept_no_throw (int get)
    {
        return (int)accept(get, 0, 0);
    }
    int tcp_accept_no_throw (int get, const k2::time_span& timeout)
    {
        if(socket_wait(get, wait_read, timeout) == false)
            return  -1;

        return  tcp_accept_no_throw(get);
    }
    int tcp_accept (int get)
    {
        int tcp_desc = (int)accept(get, 0, 0);

        //  Not suppose to happend, take it out?
        if(tcp_desc == -1)
            throw   k2::socket_accept_error();

        return  tcp_desc;
    }
    int tcp_accept (int get, const k2::time_span& timeout)
    {
        if(socket_wait(get, wait_read, timeout) == false)
            throw   k2::socket_timedout_error();

        return  tcp_accept(get);
    }
    template <typename tp_addr_>
    void transport_bind (int get, const tp_addr_& local_addr)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type bsd_tp_addr_t;
        const bsd_tp_addr_t sa (local_addr);
        if(bind(get, (const sockaddr*)(&sa), sizeof(sa)) == -1)
        {
            socket_close(get);
            throw   k2::socket_bind_error();
        }
    }
    template <typename tp_addr_>
    void transport_connect (int get, const tp_addr_& remote_addr)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type bsd_tp_addr_t;
        const bsd_tp_addr_t sa = remote_addr;
        if(connect(get, (const sockaddr*)(&sa), sizeof(sa)) == -1)
        {
            socket_close(get);
            throw   k2::socket_connect_error();
        }
    }
    template <typename tp_addr_>
    void transport_connect (
        int get,
        const tp_addr_& remote_addr,
        const k2::time_span& timeout)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type bsd_tp_addr_t;
        socket_nb       nb(get);
        const bsd_tp_addr_t sa = remote_addr;
        if(connect(get, (const sockaddr*)(&sa), sizeof(sa)) == -1)
        {
#if defined(K2_HAS_POSIX_API)
            //  BSD Socket specific
            static const int connect_in_progress = EINPROGRESS;
#else
            //  Windows Socket specific
            static const int connect_in_progress = EWOULDBLOCK;
#endif
            if(k2::socket_errno::get_last() != connect_in_progress)
            {
                goto NONBLOCK_CONNECT_ERROR;
            }
            else
            {
                if(socket_wait(get, wait_write, timeout) == false)
                    goto NONBLOCK_CONNECT_ERROR;

#if defined(K2_HAS_POSIX_API)
                //  BSD Socket specific
                int         so_error;
                socklen_t   len = sizeof(so_error);
                if(getsockopt(get, SOL_SOCKET, SO_ERROR, &so_error, &len) != 0)
                    goto NONBLOCK_CONNECT_ERROR;

                if(so_error != 0)
                    goto NONBLOCK_CONNECT_ERROR;
#else
                 //  Windows Socket specific
                if(connect(get, (const sockaddr*)(&sa), sizeof(sa)) != EISCONN)
                    goto NONBLOCK_CONNECT_ERROR;
#endif
            }
        }

        //  Successful return
        return;

        //  Errorous return
        NONBLOCK_CONNECT_ERROR:
            socket_close(get);
            throw   k2::socket_connect_error();
    }
    size_t tcp_write (int tcp_desc, const char* buf, size_t bytes)
    {
        //  Works with connected udp socket, too.
        //  But, we just don't use it that way in k2.
        int ret = send(tcp_desc, buf, (socklen_t)bytes, 0);
        if(ret == -1)
            throw   k2::socket_tcp_io_error();

        return  ret;
    }
    size_t tcp_write_all (int tcp_desc, const char* buf, size_t bytes)
    {
        size_t bytes_done = 0;
        for(;;)
        {
            bytes_done += tcp_write(
                tcp_desc,
                buf + bytes_done,
                bytes - bytes_done);

            if(bytes_done == bytes)
                break;
        }
        return  bytes;
    }
    size_t tcp_write_all (
        int tcp_desc,
        const char* buf,
        size_t bytes,
        k2::time_span timeout)
    {
        size_t bytes_done = 0;

        for(;;)
        {
            if(socket_wait2(tcp_desc, wait_write, timeout) == false)
                throw   k2::socket_timedout_error();

            bytes_done += tcp_write(
                tcp_desc,
                buf + bytes_done,
                bytes - bytes_done);

            if(bytes_done == bytes)
                break;
        }
        return  bytes;
    }
    size_t tcp_read (int tcp_desc, char* buf, size_t bytes)
    {
        int ret = recv(tcp_desc, buf, (socklen_t)bytes, 0);
        if(ret == -1)
            throw   k2::socket_tcp_io_error();

        return  ret;
    }
    size_t tcp_read_all (int tcp_desc, char* buf, size_t bytes)
    {
        size_t bytes_done = 0;
        for(;;)
        {
            size_t res = tcp_read(
                tcp_desc,
                buf + bytes_done,
                bytes - bytes_done);

            if(res == 0)    //  connection closed by peer
                return  0;  //  previous read data is partial, discarded.

            bytes_done += res;
            if(bytes_done == bytes)
                break;
        }
        return  bytes;
    }
    size_t tcp_read_all (
        int tcp_desc,
        char* buf,
        size_t bytes,
        k2::time_span timeout)
    {
        size_t bytes_done = 0;

        for(;;)
        {
            if(socket_wait2(tcp_desc, wait_read, timeout) == false)
                throw   k2::socket_timedout_error();

            size_t res = tcp_read(
                tcp_desc,
                buf + bytes_done,
                bytes - bytes_done);

            if(res == 0)    //  connection closed by peer
                return  0;  //  previous read data is partial, discarded.

            bytes_done += res;

            if(bytes_done == bytes)
                break;
        }
        return  bytes;
    }

    template <typename tp_addr_>
    size_t udp_write (
        int udp_desc,
        const char* buf,
        size_t bytes,
        const tp_addr_& remote_addr)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type
            bsd_tp_addr_t;
        const bsd_tp_addr_t sa = remote_addr;
        int ret = sendto(
            udp_desc,
            buf,
            (socklen_t)bytes,
            0,
            (const sockaddr*)(&sa), sizeof(sa));

        return  ret;
    }
    template <typename tp_addr_>
    size_t udp_read (
        int udp_desc,
        char* buf,
        size_t bytes,
        tp_addr_& remote_addr)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type
            bsd_tp_addr_t;
        bsd_tp_addr_t   sa;
        socklen_t   len = sizeof(sa);

        int ret = recvfrom(
            udp_desc,
            buf,
            (socklen_t)bytes,
            0,
            (sockaddr*)(&sa),
            &len);

        if(ret != -1)
            remote_addr = sa;

        return  ret;
    }
    template <typename tp_addr_>
    size_t udp_read (
        int udp_desc,
        char* buf,
        size_t bytes,
        tp_addr_& remote_addr,
        const k2::time_span& timeout)
    {
        if(socket_wait(udp_desc, wait_read, timeout) == false)
            throw   k2::socket_timedout_error();

        return  udp_read(udp_desc, buf, bytes, remote_addr);
    }

    template <typename tp_addr_>
    const tp_addr_ local_tranport_addr (int get)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type
            bsd_tp_addr_t;
        bsd_tp_addr_t   sa;
        socklen_t   len = sizeof(sa);
        k2::runtime_assert(getsockname(get, (sockaddr*)(&sa), &len) != -1);
        return  tp_addr_(sa);
    }
    template <typename tp_addr_>
    const tp_addr_ remote_tranport_addr (int get)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type
            bsd_tp_addr_t;
        bsd_tp_addr_t   sa;
        socklen_t   len = sizeof(sa);
        k2::runtime_assert(getpeername(get, (sockaddr*)(&sa), &len) != -1);
        return  tp_addr_(sa);
    }

}   //  unnamed namespace

k2::socket_desc::~socket_desc ()
{
    if(m_own)
    {
#if defined(K2_HAS_POSIX_API)
        //  BSD Socket specific
        close(m_desc);
#else
        //  Windows Socket specific
        closesocket(m_desc);
#endif
    }
}
int
k2::socket_desc::get () const
{
    return  m_desc;
}
k2::socket_desc::socket_desc (family_enum family, prot_enum prot)
:   m_desc(-1)
,   m_own(true)
{
    m_desc = (int)::socket(
        (family == family_ipv4 ? AF_INET : AF_INET6),
        (prot == prot_tcp ? SOCK_STREAM : SOCK_DGRAM),
        0);

    if(m_desc == -1)
        throw   k2::socket_open_error();
}
k2::socket_desc::socket_desc (int get, bool own)
:   m_desc(get)
,   m_own(own)
{
    if(m_desc == -1)
        throw   k2::socket_open_error();
}


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


k2::ipv4::tcp_listener::tcp_listener (const transport_addr& local_addr)
:   m_desc(socket_desc::family_ipv4, socket_desc::prot_tcp)
{
    transport_bind(m_desc.get(), local_addr);
    tcp_listen(m_desc.get(), 5);
}
k2::ipv4::tcp_listener::tcp_listener (int desc, bool own)
:   m_desc(desc, true)
{
}
k2::ipv4::tcp_listener::~tcp_listener ()
{
}
int
k2::ipv4::tcp_listener::get_desc () const
{
    return  m_desc.get();
}
const k2::ipv4::transport_addr
k2::ipv4::tcp_listener::local_addr () const
{
    return  local_tranport_addr<transport_addr>(m_desc.get());
}
std::auto_ptr<k2::ipv4::tcp_transport>
k2::ipv4::tcp_listener::accept ()
{
    int tcp = tcp_accept_no_throw(m_desc.get());
    if(tcp == -1)
        return  std::auto_ptr<tcp_transport>(0);
    else
        return  std::auto_ptr<tcp_transport>(new tcp_transport(tcp, true));
}
std::auto_ptr<k2::ipv4::tcp_transport>
k2::ipv4::tcp_listener::accept (const k2::time_span& timeout)
{
    int tcp = tcp_accept_no_throw(m_desc.get(), timeout);
    if(tcp == -1)
        return  std::auto_ptr<tcp_transport>(0);
    else
        return  std::auto_ptr<tcp_transport>(new tcp_transport(tcp, true));
}
int
k2::ipv4::tcp_listener::accept_desc ()
{
    return  tcp_accept(m_desc.get());
}
int
k2::ipv4::tcp_listener::accept_desc (const k2::time_span& timeout)
{
    return  tcp_accept(m_desc.get(), timeout);
}




k2::ipv4::tcp_transport::tcp_transport (const transport_addr& remote_addr)
:   m_desc(socket_desc::family_ipv4, socket_desc::prot_tcp)
{
    transport_connect(m_desc.get(), remote_addr);
}
k2::ipv4::tcp_transport::tcp_transport (
    const transport_addr& remote_addr,
    const time_span& timeout)
:   m_desc(socket_desc::family_ipv4, socket_desc::prot_tcp)
{
    transport_connect(m_desc.get(), remote_addr, timeout);
}

k2::ipv4::tcp_transport::tcp_transport (
    const transport_addr& local_addr,
    const transport_addr& remote_addr)
:   m_desc(socket_desc::family_ipv4, socket_desc::prot_tcp)
{
    transport_bind(m_desc.get(), local_addr);
    transport_connect(m_desc.get(), remote_addr);
}

k2::ipv4::tcp_transport::tcp_transport (
    const transport_addr&   local_addr,
    const transport_addr&   remote_addr,
    const time_span&        timeout)
:   m_desc(socket_desc::family_ipv4, socket_desc::prot_tcp)
{
    transport_bind(m_desc.get(), local_addr);
    transport_connect(m_desc.get(), remote_addr, timeout);
}

k2::ipv4::tcp_transport::tcp_transport (tcp_listener& listener)
:   m_desc(listener.accept_desc(), true)
{
}
k2::ipv4::tcp_transport::tcp_transport (
    tcp_listener&    listener,
    const time_span& timeout)
:   m_desc(listener.accept_desc(timeout), true)
{
}
k2::ipv4::tcp_transport::tcp_transport (int tcp_desc, bool own)
:   m_desc(tcp_desc, own)
{
}
k2::ipv4::tcp_transport::~tcp_transport ()
{
}
int
k2::ipv4::tcp_transport::get_desc () const
{
    return  m_desc.get();
}
const k2::ipv4::transport_addr
k2::ipv4::tcp_transport::local_addr () const
{
    return  local_tranport_addr<transport_addr>(m_desc.get());
}
const k2::ipv4::transport_addr
k2::ipv4::tcp_transport::remote_addr () const
{
    return  remote_tranport_addr<transport_addr>(m_desc.get());
}
size_t
k2::ipv4::tcp_transport::write (const char* buf, size_t bytes)
{
    return  tcp_write(m_desc.get(), buf, bytes);
}
size_t
k2::ipv4::tcp_transport::write_all (const char* buf, size_t bytes)
{
    return  tcp_write_all(m_desc.get(), buf, bytes);
}
size_t
k2::ipv4::tcp_transport::write_all (const char* buf, size_t bytes, const time_span& timeout)
{
    return  tcp_write_all(m_desc.get(), buf, bytes, timeout);
}
size_t
k2::ipv4::tcp_transport::read (char* buf, size_t bytes)
{
    return  tcp_read(m_desc.get(), buf, bytes);
}
size_t
k2::ipv4::tcp_transport::read_all (char* buf, size_t bytes)
{
    return  tcp_read_all(m_desc.get(), buf, bytes);
}
size_t
k2::ipv4::tcp_transport::read_all (char* buf, size_t bytes, const time_span& timeout)
{
    return  tcp_read_all(m_desc.get(), buf, bytes, timeout);
}



k2::ipv4::udp_transport::udp_transport (const transport_addr& local_addr)
:   m_desc(socket_desc::family_ipv4, socket_desc::prot_udp)
{
    transport_bind(m_desc.get(), local_addr);
}
k2::ipv4::udp_transport::udp_transport (const transport_addr& local_addr, const transport_addr& remote_addr)
:   m_desc(socket_desc::family_ipv4, socket_desc::prot_udp)
{
    transport_bind(m_desc.get(), local_addr);
    transport_connect(m_desc.get(), remote_addr);
}
k2::ipv4::udp_transport::udp_transport (int udp_desc, bool own)
:   m_desc(socket_desc::family_ipv4, socket_desc::prot_udp)
{
}
k2::ipv4::udp_transport::~udp_transport ()
{
}
int
k2::ipv4::udp_transport::get_desc () const
{
    return  m_desc.get();
}
const k2::ipv4::transport_addr
k2::ipv4::udp_transport::local_addr () const
{
    return  local_tranport_addr<transport_addr>(m_desc.get());
}
size_t
k2::ipv4::udp_transport::write (
    const char*             buf,
    size_t                  bytes,
    const transport_addr&   remote_addr)
{
    return  udp_write(m_desc.get(), buf, bytes, remote_addr);
}
size_t
k2::ipv4::udp_transport::read (
    char*           buf,
    size_t          bytes,
    transport_addr& remote_addr)
{
    return  udp_read(m_desc.get(), buf, bytes, remote_addr);
}
size_t
k2::ipv4::udp_transport::read (
char*           buf,
size_t          bytes,
transport_addr& remote_addr,
const time_span& timeout)
{
    return  udp_read(m_desc.get(), buf, bytes, remote_addr, timeout);
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
