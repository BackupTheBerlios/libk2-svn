/*	libk2	<test_tcp_client.cpp>

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
#include <k2/utility.h>
#include <k2/thread.h>
#include <k2/spin_lock.h>
#include <k2/assert.h>
#include <k2/timing.h>
#include <k2/ipv4_tcp.h>
#include <k2/ipv4_udp.h>
#include <k2/singleton.h>
#include <k2/allocator.h>

#include <iostream>
#include <cassert>

using namespace std;
using namespace k2;
using namespace k2::ipv4;

struct tcp_dumper
{
    transport_addr m_peer_addr;

    tcp_dumper (transport_addr peer_addr)
    :   m_peer_addr(peer_addr)
    {
    }
    void operator() () const
    {
        tcp_transport   tcp(m_peer_addr);
        const char      msg[] = "Hello";

        for (size_t cnt = 0; cnt < 1000; ++cnt)
        {
            tcp.write_all(msg, sizeof(msg));
            thread::sleep(2000);
        }

    }
};

int main ()
{
    using namespace ipv4;
    std::vector<transport_addr> tp_addrs = getaddrinfo(
        type_tag<tcp_transport>(), 
        false,
        local_string(""),
        local_string("2266"));
    try
    {
        tcp_dumper  tcp_dump(tp_addrs.front());
        tcp_dump();
    }
    catch (socket_errno&)
    {
        return  1;
    }
	return	0;
}
