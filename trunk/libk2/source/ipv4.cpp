/*  libk2   <ipv4.cpp>

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
#include <k2/ipv4.h>

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
#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <sys/types.h>
#   include <sys/time.h>
#   include <unistd.h>
#   include <netdb.h>
#   include <fcntl.h>
#elif defined(K2_HAS_WIN32_API)
#   include <winsock2.h>
#   pragma warning (disable : 4800) //  what for???
    typedef int socklen_t;
#else
#   error   "libk2: How to include socket header(s)?"
#endif

namespace k2
{

    namespace nonpublic
    {

        void socket_set_nb (int desc, bool nb)
#if defined(K2_HAS_POSIX_API)
        {
            int flags = fcntl(desc, F_GETFL, 0);
            if (flags != -1 && nb)
            {
                flags |= O_NONBLOCK;
                fcntl(desc, F_SETFL, flags);
            }
            else
            {
                flags &= ~O_NONBLOCK;
                fcntl(desc, F_SETFL, flags);
            }
        }
#else
        {
            u_long  on = (nb ? 1 : 0);
            ioctlsocket(desc, FIONBIO, &on);
        }
#endif
        struct socket_nb
        {
            socket_nb (int desc)
            :   desc(desc)
            {
                socket_set_nb(desc, true);
            }
            ~socket_nb ()
            {
                socket_set_nb(desc, false);
            }

            int desc;
        };

        int socket_open (int family, int type, int prot)
        {
            int desc = (int)socket(family, type, prot);
            if (desc == -1)
                throw   socket_open_error();
            return  desc;
        }
        void    socket_close (int desc)
#if defined(K2_HAS_POSIX_API)
        {
            close(desc);
        }
#else
        {
            closesocket(desc);
        }
#endif
        enum wait_opt
        {
            wait_read,
            wait_write,
            wait_except
        };
        bool socket_wait (int desc, wait_opt opt, const time_span& timeout)
        {
            fd_set  fds;
            FD_ZERO(&fds);
            FD_SET(desc, &fds);
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
                runtime_assert(0);
            }

            timeval tv;
            tv.tv_sec   = timeout.in_sec();
            tv.tv_usec  = timeout.msec_of_sec() * 1000;
            if (::select(desc + 1, prd_fds, pwr_fds, pex_fds, &tv) < 1)
                return  false;
            else
                return  true;
        }
        bool socket_wait2 (int desc, wait_opt opt, time_span& timeout)
        {
#if !defined(K2_OS_LINUX)
            //  See
            //  man 2 select
            //  in Linux man page.
            //  For select() behaves different from other platforms.
            timestamp   pre_wait;
#endif  //  !GNU

            fd_set  fds;
            FD_ZERO(&fds);
            FD_SET(desc, &fds);
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
                runtime_assert(0);
            }

            timeval tv;
            tv.tv_sec   = timeout.in_sec();
            tv.tv_usec  = timeout.msec_of_sec() * 1000;
            if (::select(desc + 1, prd_fds, pwr_fds, pex_fds, &tv) < 1)
            {
                timeout = time_span(0);
                return  false;
            }
            else
            {
#if defined(K2_OS_LINUX)
                timeout = time_span(tv.tv_sec * 1000 + tv.tv_usec / 1000);
#else
                timestamp now;
                time_span waited = now - pre_wait;
                if (waited > timeout)
                    timeout = time_span(0);
                else
                    timeout -= waited;
#endif
                return  true;
            }
        }
        void tcp_listen (int desc, size_t back_log)
        {
            if (listen(desc, (int)back_log) == -1)
            {
                socket_close(desc);
                throw   socket_bind_error();
            }
        }
        int tcp_accept_no_throw (int desc)
        {
            return (int)accept(desc, 0, 0);
        }
        int tcp_accept_no_throw (int desc, const time_span& timeout)
        {
            if (socket_wait(desc, wait_read, timeout) == false)
                return  -1;

            return  tcp_accept_no_throw(desc);
        }
        int tcp_accept (int desc)
        {
            int tcp_desc = (int)accept(desc, 0, 0);

            //  Not suppose to happend, take it out?
            if (tcp_desc == -1)
                throw   socket_accept_error();

            return  tcp_desc;
        }
        int tcp_accept (int desc, const time_span& timeout)
        {
            if (socket_wait(desc, wait_read, timeout) == false)
                throw   socket_timedout_error();

            return  tcp_accept(desc);
        }
        void layer4_bind (int desc, const ipv4::layer4_addr& local_addr)
        {
            const sockaddr_in   sa (local_addr);
            if (bind(desc, (const sockaddr*)(&sa), sizeof(sa)) == -1)
            {
                socket_close(desc);
                throw   socket_bind_error();
            }
        }
        void tcp_connect (int desc, const ipv4::layer4_addr& remote_addr)
        {
            //  Works with udp socket, too.
            //  But, we just don't use it that way in k2.
            const sockaddr_in   sa = remote_addr;
            if (connect(desc, (const sockaddr*)(&sa), sizeof(sa)) == -1)
            {
                socket_close(desc);
                throw   socket_connect_error();
            }
        }
        void tcp_connect (int desc, const ipv4::layer4_addr& remote_addr, const time_span& timeout)
        {
            socket_nb   nb(desc);
            const sockaddr_in   sa = remote_addr;
            if (connect(desc, (const sockaddr*)(&sa), sizeof(sa)) == -1)
            {
#if defined(K2_HAS_POSIX_API)
                static const int connect_in_progress = EINPROGRESS;
#endif

#if defined(K2_HAS_WIN32_API)
                static const int connect_in_progress = EWOULDBLOCK;
#endif
                if (socket_errno::get_last() != connect_in_progress)
                {
                    goto NONBLOCK_CONNECT_ERROR;
                }
                else
                {
                    if (socket_wait(desc, wait_write, timeout) == false)
                        goto NONBLOCK_CONNECT_ERROR;

#if defined(K2_HAS_POSIX_API)
                    int         so_error;
                    socklen_t   len = sizeof(so_error);
                    if (getsockopt(desc, SOL_SOCKET, SO_ERROR, &so_error, &len) != 0)
                        goto NONBLOCK_CONNECT_ERROR;

                    if (so_error != 0)
                        goto NONBLOCK_CONNECT_ERROR;
#endif

#if defined(K2_HAS_WIN32_API)
                    if (connect(desc, (const sockaddr*)(&sa), sizeof(sa)) != EISCONN)
                        goto NONBLOCK_CONNECT_ERROR;
#endif
                }
            }

            //  Successful return
            return;

            //  Errorous return
            NONBLOCK_CONNECT_ERROR:
                socket_close(desc);
                throw   socket_connect_error();
        }
        size_t tcp_write (int tcp_desc, const char* buf, size_t bytes)
        {
            //  Works with connected udp socket, too.
            //  But, we just don't use it that way in k2.
            int ret = send(tcp_desc, buf, (socklen_t)bytes, 0);
            if (ret == -1)
                throw   socket_tcp_io_error();

            return  ret;
        }
        size_t tcp_write_all (int tcp_desc, const char* buf, size_t bytes)
        {
            size_t bytes_done = 0;
            for (;;)
            {
                bytes_done += tcp_write(tcp_desc, buf + bytes_done, bytes - bytes_done);

                if (bytes_done == bytes)
                    break;
            }
            return  bytes;
        }
        size_t tcp_write_all (int tcp_desc, const char* buf, size_t bytes, const time_span& timeout_)
        {
            size_t bytes_done = 0;
            time_span timeout = timeout_;

            for (;;)
            {
                if (socket_wait2(tcp_desc, wait_write, timeout) == false)
                    throw   socket_timedout_error();

                bytes_done += tcp_write(tcp_desc, buf + bytes_done, bytes - bytes_done);

                if (bytes_done == bytes)
                    break;
            }
            return  bytes;
        }
        size_t tcp_read (int tcp_desc, char* buf, size_t bytes)
        {
            int ret = recv(tcp_desc, buf, (socklen_t)bytes, 0);
            if (ret == -1)
                throw   socket_tcp_io_error();

            return  ret;
        }
        size_t tcp_read_all (int tcp_desc, char* buf, size_t bytes)
        {
            size_t bytes_done = 0;
            for (;;)
            {
                size_t res = tcp_read(tcp_desc, buf + bytes_done, bytes - bytes_done);
                if (res == 0)       //  connection closed by peer
                    return  0;      //  previous read data is partial, not useful.

                bytes_done += res;
                if (bytes_done == bytes)
                    break;
            }
            return  bytes;
        }
        size_t tcp_read_all (int tcp_desc, char* buf, size_t bytes, const time_span& timeout_)
        {
            size_t bytes_done = 0;
            time_span   timeout(timeout_);

            for (;;)
            {
                if (socket_wait2(tcp_desc, wait_read, timeout) == false)
                    throw   socket_timedout_error();

                size_t res = tcp_read(tcp_desc, buf + bytes_done, bytes - bytes_done);
                if (res == 0)       //  connection closed by peer
                    return  0;      //  previous read data is partial, not useful.

                bytes_done += res;

                if (bytes_done == bytes)
                    break;
            }
            return  bytes;
        }

        size_t udp_write (int udp_desc, const char* buf, size_t bytes, const ipv4::layer4_addr& remote_addr)
        {
            const sockaddr_in   sa = remote_addr;
            int ret = sendto(udp_desc, buf, (socklen_t)bytes, 0, (const sockaddr*)(&sa), sizeof(sa));
            if (ret == -1)
                throw   socket_udp_io_error();

            return  ret;
        }
        size_t udp_read (int udp_desc, char* buf, size_t bytes, ipv4::layer4_addr& remote_addr)
        {
            sockaddr_in sa;
            socklen_t   len = sizeof(sa);

            int ret = recvfrom(udp_desc, buf, (socklen_t)bytes, 0, (sockaddr*)(&sa), &len);
            if (ret == -1)
                throw   socket_udp_io_error();

            remote_addr = sa;

            return  ret;
        }
        size_t udp_read (int udp_desc, char* buf, size_t bytes, ipv4::layer4_addr& remote_addr, const time_span& timeout)
        {
            if (socket_wait(udp_desc, wait_read, timeout) == false)
                throw   socket_timedout_error();

            return  udp_read(udp_desc, buf, bytes, remote_addr);
        }

        ipv4::layer4_addr   layer4_local_addr (int desc)
        {
            sockaddr_in sa;
            socklen_t   len = sizeof(sa);
            runtime_assert(getsockname(desc, (sockaddr*)(&sa), &len) != -1);
            return  ipv4::layer4_addr(sa);
        }
        ipv4::layer4_addr   layer4_remote_addr (int desc)
        {
            sockaddr_in sa;
            socklen_t   len = sizeof(sa);
            runtime_assert(getpeername(desc, (sockaddr*)(&sa), &len) != -1);
            return  ipv4::layer4_addr(sa);
        }

    }







    ipv4_addr::ipv4_addr (uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3)
    :   m_host32(0) //  to suppress warning from gcc 3.2
    {
        this->byte_at(0) = byte0;
        this->byte_at(1) = byte1;
        this->byte_at(2) = byte2;
        this->byte_at(3) = byte3;
    }
    ipv4_addr::ipv4_addr (const std::string& string)
    :   m_host32(net2host(inet_addr(string.c_str())))
    {
    }
    ipv4_addr::ipv4_addr (const in_addr& other)
    :   m_host32(net2host(other.s_addr))
    {
    }

    ipv4_addr::operator const in_addr () const
    {
        in_addr temp;
        temp.s_addr = host2net(m_host32);
        return  temp;
    }
    uint8_t& ipv4_addr::byte_at (int idx)
    {
        uint8_t*    first = (uint8_t*)(&m_host32);
    #if defined(K2_BYTEORDER_LITTLEENDIAN)
        return  first[sizeof(m_host32) - idx - 1];
    #else
        return  first[idx];
    #endif  //  K2_BYTEORDER_LITTLEENDIAN
    }
    uint8_t ipv4_addr::byte_at (int idx) const
    {
        const uint8_t*  first = (const uint8_t*)(&m_host32);
#if defined(K2_BYTEORDER_LITTLEENDIAN)
        return  first[sizeof(m_host32) - idx - 1];
#else
        return  first[idx];
#endif  //  K2_BYTEORDER_LITTLEENDIAN
    }

    std::ostream& operator << (std::ostream& out, const ipv4_addr& ipaddr)
    {
        out
            <<  uint32_t(ipaddr.byte_at(0)) << '.'
            <<  uint32_t(ipaddr.byte_at(1)) << '.'
            <<  uint32_t(ipaddr.byte_at(2)) << '.'
            <<  uint32_t(ipaddr.byte_at(3));

        return  out;
    }
    std::istream& operator >> (std::istream& in, ipv4_addr& ipaddr)
    {
        int    byte0, byte1, byte2, byte3;
        char   dot;

        in >> byte0;
        if (byte0 > 255 || byte0 < 0)
        {
            //  error
            in.setstate(std::ios::failbit);
            return  in;
        }
        in >> dot;
        if (dot != '.')
        {
            //  error
            in.setstate(std::ios::failbit);
            return  in;
        }

        in >> byte1;
        if (byte1 > 255 || byte0 < 0)
        {
            //  error
            in.setstate(std::ios::failbit);
            return  in;
        }
        in >> dot;
        if (dot != '.')
        {
            //  error
            in.setstate(std::ios::failbit);
            return  in;
        }

        in >> byte2;
        if (byte2 > 255 || byte0 < 0)
        {
            //  error
            in.setstate(std::ios::failbit);
            return  in;
        }
        in >> dot;
        if (dot != '.')
        {
            //  error
            in.setstate(std::ios::failbit);
            return  in;
        }

        in >> byte3;
        if (byte3 > 255 || byte0 < 0)
        {
            //  error
            in.setstate(std::ios::failbit);
            return  in;
        }

        ipaddr = ipv4_addr(uint8_t(byte0), uint8_t(byte1), uint8_t(byte2), uint8_t(byte3));

        return  in;
    }

    namespace ipv4
    {

        bool dns_util::lookup (const std::string& fqdn, std::vector<ipv4_addr>& ips)
        {
            scoped_guard    guard(class_lock::instance());

            hostent* hostent_ptr = gethostbyname(fqdn.c_str());
            if (hostent_ptr == 0)
                return  false;

            ips.clear();
            size_t  idx = 0;
            for (; hostent_ptr->h_addr_list[idx] != 0; ++idx)
                ips.push_back(ipv4_addr(net2host(*reinterpret_cast<const uint32_t*>(hostent_ptr->h_addr_list[0]))));

            return  true;
        }
        bool dns_util::lookup (const std::string& fqdn, ipv4_addr& ip)
        {
            scoped_guard    guard(class_lock::instance());

            hostent* hostent_ptr = gethostbyname(fqdn.c_str());
            if (hostent_ptr == 0)
                return  false;

            if (hostent_ptr->h_addr_list[0])
            {
                ip = ipv4_addr(net2host(*reinterpret_cast<const uint32_t*>(hostent_ptr->h_addr_list[0])));
            }
            return  true;
        }



        layer4_addr::layer4_addr (const sockaddr_in& other)
        :   ip(net2host(other.sin_addr.s_addr)),
            port(net2host(other.sin_port))
        {
        }
        layer4_addr::operator const sockaddr_in () const
        {
            sockaddr_in ret;
            ret.sin_family      = AF_INET;
            ret.sin_addr        = in_addr(this->ip);
            ret.sin_port        = host2net(this->port);
            return  ret;
        }





        tcp_listener::tcp_listener (const layer4_addr& local_addr)
        :   m_desc(nonpublic::socket_open(AF_INET, SOCK_STREAM, 0))
        ,   m_own(true)
        {
            nonpublic::layer4_bind(m_desc, local_addr);
            nonpublic::tcp_listen(m_desc, 5);
        }
        tcp_listener::tcp_listener (int desc, bool own)
        :   m_desc(desc)
        ,   m_own(own)
        {
        }
        tcp_listener::~tcp_listener ()
        {
            if (m_own)
                nonpublic::socket_close(m_desc);
        }
        const layer4_addr tcp_listener::local_addr () const
        {
            return  nonpublic::layer4_local_addr(m_desc);
        }
        std::auto_ptr<tcp_transport> tcp_listener::accept ()
        {
            int tcp = nonpublic::tcp_accept_no_throw(m_desc);
            if (tcp == -1)
                return  std::auto_ptr<tcp_transport>(0);
            else
                return  std::auto_ptr<tcp_transport>(new tcp_transport(tcp, true));
        }
        std::auto_ptr<tcp_transport> tcp_listener::accept (const time_span& timeout)
        {
            int tcp = nonpublic::tcp_accept_no_throw(m_desc, timeout);
            if (tcp == -1)
                return  std::auto_ptr<tcp_transport>(0);
            else
                return  std::auto_ptr<tcp_transport>(new tcp_transport(tcp, true));
        }
        int tcp_listener::accept_desc ()
        {
            return  nonpublic::tcp_accept(m_desc);
        }
        int tcp_listener::accept_desc (const time_span& timeout)
        {
            return  nonpublic::tcp_accept(m_desc, timeout);
        }




        tcp_transport::tcp_transport (const layer4_addr& remote_addr)
        :   m_desc(nonpublic::socket_open(AF_INET, SOCK_STREAM, 0))
        ,   m_own(true)
        {
            nonpublic::tcp_connect(m_desc, remote_addr);
        }
        tcp_transport::tcp_transport (const layer4_addr& remote_addr, const time_span& timeout)
        :   m_desc(nonpublic::socket_open(AF_INET, SOCK_STREAM, 0))
        ,   m_own(true)
        {
            nonpublic::tcp_connect(m_desc, remote_addr, timeout);
        }

        tcp_transport::tcp_transport (const layer4_addr& local_addr, const layer4_addr& remote_addr)
        :   m_desc(nonpublic::socket_open(AF_INET, SOCK_STREAM, 0))
        ,   m_own(true)
        {
            nonpublic::layer4_bind(m_desc, local_addr);
            nonpublic::tcp_connect(m_desc, remote_addr);
        }

        tcp_transport::tcp_transport (const layer4_addr& local_addr, const layer4_addr& remote_addr, const time_span& timeout)
        :   m_desc(nonpublic::socket_open(AF_INET, SOCK_STREAM, 0))
        ,   m_own(true)
        {
            nonpublic::layer4_bind(m_desc, local_addr);
            nonpublic::tcp_connect(m_desc, remote_addr, timeout);
        }

        tcp_transport::tcp_transport (tcp_listener& listener)
        :   m_desc(listener.accept_desc())
        ,   m_own(true)
        {
        }
        tcp_transport::tcp_transport (tcp_listener& listener, const time_span& timeout)
        :   m_desc(listener.accept_desc(timeout))
        ,   m_own(true)
        {
        }
        tcp_transport::tcp_transport (int tcp_desc, bool own)
        :   m_desc(tcp_desc)
        ,   m_own(own)
        {
        }
        tcp_transport::~tcp_transport ()
        {
            if (m_own)
                nonpublic::socket_close(m_desc);
        }
        const layer4_addr tcp_transport::local_addr () const
        {
            return  nonpublic::layer4_local_addr(m_desc);
        }
        const layer4_addr tcp_transport::remote_addr () const
        {
            return  nonpublic::layer4_remote_addr(m_desc);
        }
        size_t tcp_transport::write (const char* buf, size_t bytes)
        {
            return  nonpublic::tcp_write(m_desc, buf, bytes);
        }
        size_t tcp_transport::write_all (const char* buf, size_t bytes)
        {
            return  nonpublic::tcp_write_all(m_desc, buf, bytes);
        }
        size_t tcp_transport::write_all (const char* buf, size_t bytes, const time_span& timeout)
        {
            return  nonpublic::tcp_write_all(m_desc, buf, bytes, timeout);
        }
        size_t tcp_transport::read (char* buf, size_t bytes)
        {
            return  nonpublic::tcp_read(m_desc, buf, bytes);
        }
        size_t tcp_transport::read_all (char* buf, size_t bytes)
        {
            return  nonpublic::tcp_read_all(m_desc, buf, bytes);
        }
        size_t tcp_transport::read_all (char* buf, size_t bytes, const time_span& timeout)
        {
            return  nonpublic::tcp_read_all(m_desc, buf, bytes, timeout);
        }



        udp_transport::udp_transport (const layer4_addr& local_addr)
        :   m_desc(nonpublic::socket_open(AF_INET, SOCK_DGRAM, 0))
        ,   m_own(true)
        {
            nonpublic::layer4_bind(m_desc, local_addr);
        }
        udp_transport::udp_transport (int udp_desc, bool own)
        :   m_desc(udp_desc)
        ,   m_own(own)
        {
        }
        udp_transport::~udp_transport ()
        {
            if (m_own)
                nonpublic::socket_close(m_desc);
        }
        const layer4_addr udp_transport::local_addr () const
        {
            return  nonpublic::layer4_local_addr(m_desc);
        }
        size_t udp_transport::write (const char* buf, size_t bytes, const layer4_addr& remote_addr)
        {
            return  nonpublic::udp_write(m_desc, buf, bytes, remote_addr);
        }
        size_t udp_transport::read (char* buf, size_t bytes, layer4_addr& remote_addr)
        {
            return  nonpublic::udp_read(m_desc, buf, bytes, remote_addr);
        }
        size_t udp_transport::read (char* buf, size_t bytes, layer4_addr& remote_addr, const time_span& timeout)
        {
            return  nonpublic::udp_read(m_desc, buf, bytes, remote_addr, timeout);
        }


    }   //  namespace ipv4






}   //  namepace k2

