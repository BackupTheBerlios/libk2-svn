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
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_IPV4_ADDR_H
#   include <k2/ipv4_addr.h>
#endif
#ifndef K2_IPV6_ADDR_H
#   include <k2/ipv6_addr.h>
#endif

#ifndef K2_SOCKET_H
#   include <k2/socket.h>
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
//#   include <k2/byte_manip.h>
#endif
#ifndef K2_ASSERT_H
#   include <k2/assert.h>
#endif

#ifndef K2_STD_H_IOSTREAM
#   include <iostream>
#endif

#if !defined(WIN32)
//  !kh! need to get rid of unused headers
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/time.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <fcntl.h>
#else
#   include <winsock2.h>
#   include <ws2tcpip.h>
#   pragma warning (disable : 4800) //  what for???
    typedef int socklen_t;
#endif

namespace
{
    const int family_inet = PF_INET;
#if defined(PF_INET6)
    const int family_inet6 = PF_INET6;
#endif
}

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
        static const int    family = family_inet;
    };
#if defined(PF_INET6)
    template <>
    struct addr_traits<k2::ipv6::transport_addr>
    {
        typedef sockaddr_in6 mapped_type;
        static const int    family = family_inet6;
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
#endif

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
#if !defined(__linux__)
        //  See
        //  man 2 select
        //  in Linux man page.
        //  For select() behaves different from other platforms.
        k2::timestamp   pre_wait;
#endif  //  !__linux__

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
#if defined(__linux__)
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
            throw   k2::socket_bind_error();
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
            throw   k2::socket_bind_error();
    }
    template <typename tp_addr_>
    void transport_connect (int get, const tp_addr_& remote_addr)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type bsd_tp_addr_t;
        const bsd_tp_addr_t sa = remote_addr;
        if(connect(get, (const sockaddr*)(&sa), sizeof(sa)) == -1)
            throw   k2::socket_connect_error();
    }
    template <typename tp_addr_>
    void transport_connect (
        int tcp_desc,
        const tp_addr_& remote_addr,
        const k2::time_span& timeout)
    {
        typedef typename addr_traits<tp_addr_>::mapped_type bsd_tp_addr_t;
        socket_nb       nb(tcp_desc);
        const bsd_tp_addr_t sa = remote_addr;
        if(connect(tcp_desc, (const sockaddr*)(&sa), sizeof(sa)) == -1)
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
                throw   k2::socket_connect_error();
            }
            else
            {
                if(socket_wait(tcp_desc, wait_write, timeout) == false)
                    throw   k2::socket_connect_error();

#if defined(K2_HAS_POSIX_API)
                //  BSD Socket specific
                int         so_error;
                socklen_t   len = sizeof(so_error);
                if(getsockopt(tcp_desc, SOL_SOCKET, SO_ERROR, &so_error, &len) != 0)
                    throw   k2::socket_connect_error();

                if(so_error != 0)
                    throw   k2::socket_connect_error();
#else
                 //  Windows Socket specific
                if(connect(tcp_desc, (const sockaddr*)(&sa), sizeof(sa)) != EISCONN)
                    throw   k2::socket_connect_error();
#endif
            }
        }
    }
    size_t tcp_write (int tcp_desc, const char* buf, size_t bytes)
    {
        //  Works with connected udp socket, too.
        //  But, we just don't use it that way in k2.
        int ret = send(tcp_desc, buf, (socklen_t)bytes, 0);
        if(ret == -1)
            throw   k2::socket_connection_error();

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
            throw   k2::socket_connection_error();

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


k2::ipv4::tcp_listener::tcp_listener (const transport_addr& local_addr)
:   m_desc(socket_desc::family_ipv4, socket_desc::type_stream)
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
:   m_desc(socket_desc::family_ipv4, socket_desc::type_stream)
{
    transport_connect(m_desc.get(), remote_addr);
}
k2::ipv4::tcp_transport::tcp_transport (
    const transport_addr& remote_addr,
    const time_span& timeout)
:   m_desc(socket_desc::family_ipv4, socket_desc::type_stream)
{
    transport_connect(m_desc.get(), remote_addr, timeout);
}

k2::ipv4::tcp_transport::tcp_transport (
    const transport_addr& local_addr,
    const transport_addr& remote_addr)
:   m_desc(socket_desc::family_ipv4, socket_desc::type_stream)
{
    transport_bind(m_desc.get(), local_addr);
    transport_connect(m_desc.get(), remote_addr);
}

k2::ipv4::tcp_transport::tcp_transport (
    const transport_addr&   local_addr,
    const transport_addr&   remote_addr,
    const time_span&        timeout)
:   m_desc(socket_desc::family_ipv4, socket_desc::type_stream)
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



k2::ipv4::udp_transport::udp_transport ()
:   m_desc(socket_desc::family_ipv4, socket_desc::type_dgram)
{
}
k2::ipv4::udp_transport::udp_transport (const transport_addr& local_addr)
:   m_desc(socket_desc::family_ipv4, socket_desc::type_dgram)
{
    transport_bind(m_desc.get(), local_addr);
}
k2::ipv4::udp_transport::udp_transport (int udp_desc, bool own)
:   m_desc(socket_desc::family_ipv4, socket_desc::type_dgram)
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
void
k2::ipv4::udp_transport::connect (const transport_addr& remote_addr)
{
    transport_connect(m_desc.get(), remote_addr);
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
