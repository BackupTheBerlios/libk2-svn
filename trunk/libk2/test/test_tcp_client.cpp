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
