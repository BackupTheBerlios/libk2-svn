/*	libk2	<test_tcp_server.cpp>

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
#include <k2/thread.h>
#include <k2/spin_lock.h>
#include <k2/assert.h>
#include <k2/timing.h>
#include <k2/ipv4_tcp.h>
#include <k2/ipv4_udp.h>
#include <k2/socket_error.h>
#include <k2/singleton.h>
#include <k2/mutex.h>
#include <k2/cond_var.h>
#include <k2/timing.h>

#include <iostream>
#include <cassert>
#include <queue>

using namespace std;
using namespace k2;
using namespace k2::ipv4;

queue<tcp_transport*>   tcp_queue;
mutex                   tq_mtx;
cond_var                tq_cv(tq_mtx);
time_span               timeout(1000);
mutex                   cout_mtx;
int	objs = 0;
int ctor = 0;
int dtor = 0;

struct tcp_sink_service
{
    size_t m_id;
    tcp_sink_service (size_t id)
    :   m_id(id)
    {
        mutex::scoped_guard guard(cout_mtx);
		++objs;
		++ctor;
    }
    tcp_sink_service (const tcp_sink_service& rhs)
    :   m_id(rhs.m_id)
    {
        mutex::scoped_guard guard(cout_mtx);
		++objs;
		++ctor;
    }
    ~tcp_sink_service ()
    {
        mutex::scoped_guard guard(cout_mtx);
		--objs;
		++dtor;
    }
    void operator() () const
    {
        {
            mutex::scoped_guard guard(cout_mtx);
            cout << "Service " << (int)m_id << ": started." << endl;
        }
        while (1)
        {
            {
                auto_ptr<tcp_transport> ptcp;
                {
                    mutex::scoped_guard guard(tq_mtx);

                    while (tcp_queue.empty())
                    {
                        timestamp   timer(timeout);
                        if (tq_cv.wait(timer) == false)
                        {
                            mutex::scoped_guard guard(cout_mtx);
                            cout << "Service " << (int)m_id << ": pending for connection." << endl;
                        }

                        thread::test_cancel();
                    }

                    ptcp.reset(tcp_queue.front());
                    tcp_queue.pop();
                }
                {
                    char      buf[128] = "";

                    try
                    {
                        for (size_t cnt = 0; cnt < 1000; ++cnt)
                        {
                            if (ptcp->read(buf, sizeof(buf)) == 0)
                            {
                                mutex::scoped_guard guard(cout_mtx);
                                cout << "Service " << (int)m_id << ": connection closed by " << ptcp->get_desc() << "!!!" << endl;
                                break;
                            }
                            mutex::scoped_guard guard(cout_mtx);
                            cout << "Service " << (int)m_id << ": message came from " << ptcp->get_desc() << "!!!" << endl;
                            thread::test_cancel();
                        }
                        if (cnt == 1000)
                        {
                            mutex::scoped_guard guard(cout_mtx);
                            cout << "Service " << (int)m_id << ": closing connection to " << ptcp->get_desc() << "!!!" << endl;
                        }
                    }
                    catch (socket_errno&)
                    {
                        mutex::scoped_guard guard(cout_mtx);
                        cout << "Service " << (int)m_id << ": connection to " << ptcp->get_desc() << " is lost!!!" << endl;
                    }
                }
            }
        }
    }
};

struct on_mgr_cancelled
{
    auto_ptr<thread>* m_service_pool;
    size_t m_service_cnt;

    on_mgr_cancelled (auto_ptr<thread>* service_pool, size_t service_cnt)
    :   m_service_pool(service_pool)
    ,   m_service_cnt(service_cnt)
    {
    }

    void operator() () const
    {
        {
            mutex::scoped_guard guard(cout_mtx);
            cout << "Mgr : I am cancelled, cancelling services." << endl;
        }
        for (size_t idx = 0; idx < m_service_cnt; ++idx)
            m_service_pool[idx]->cancel();
    }
};

struct service_mgr
{
    size_t  m_service_cnt;

    service_mgr (size_t service_cnt)
    :   m_service_cnt(service_cnt)
    {
    }
    void operator() () const
    {
        ipv4_addr       local_ip(127, 0, 0, 1);
        layer4_addr     local_addr(local_ip, 2266);
        tcp_listener    listener(local_addr);
        {
            auto_ptr<thread>    service_pool[128];
            size_t idx = 0;
            for (; idx < m_service_cnt; ++idx)
            {
                service_pool[idx].reset(new thread(tcp_sink_service(idx)));
            }

            {
               timestamp       timer(time_span(150000));
               on_mgr_cancelled on_cancel(service_pool, m_service_cnt);

                while (1)
                {
                    thread::test_cancel(on_cancel);
                    auto_ptr<tcp_transport>  ptcp = listener.accept(timeout);

                    if (ptcp.get())
                    {
                        mutex::scoped_guard guard(cout_mtx);
                        cout << "Mgr : new client " << ptcp->get_desc() << " accepted!!!" << endl;
                        tcp_queue.push(ptcp.release());
                    }
                    else
                    {
                        mutex::scoped_guard guard(cout_mtx);
                        cout << "Mgr : pending." << endl;
                    }

                    if (timer.expired())
                    {
                        mutex::scoped_guard guard(cout_mtx);
                        cout << "Mgr : timedout." << endl;
                        break;
                    }
                    else
                    {
                        mutex::scoped_guard guard(cout_mtx);
                        time_span remain = timer - timestamp::now;
                        cout << "Mgr : timer remains " << remain.in_sec() << " seconds." << endl;
                    }
                }
            }
        }
        mutex::scoped_guard guard(cout_mtx);
        cout << "Mgr : service threads are cancelled." << endl;
        cout << "Mgr : bye." << endl;
    }
};

int main ()
{

	{
		service_mgr mgr(10);    //  telling mgr to create a crew of 10 working threads.
		thread      mgr_th(mgr);
		char        wait_for_enter;

		cin >> wait_for_enter;

		mgr_th.cancel();
	}
	return	0;
}

