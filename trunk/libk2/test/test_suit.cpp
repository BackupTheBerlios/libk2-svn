/*  libk2   <test_suit.cpp>

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
#include <k2/socket_error.h>
#include <k2/singleton.h>
#include <k2/allocator.h>

#include <cstddef>
#include <iostream>
#include <cassert>

using namespace std;
using namespace k2;


namespace test_threading
{
    struct ticker_tracker
    {
        enum state_enum
        {
            not_ticked,
            ticker1_ticked_1st,
            ticker2_ticked_1st,
            ticker1_ticked_2nd,
            ticker2_ticked_2nd,
            tickers_interlaced
        };
        state_enum  m_state;
        std::size_t      m_ticker1_ticks;
        std::size_t      m_ticker2_ticks;
        spin_lock   m_lock;

        ticker_tracker ()
        :   m_state(not_ticked)
        ,   m_ticker1_ticks(0)
        ,   m_ticker2_ticks(0)
        {
        }

        void tick (int ticker_id)
        {
            spin_lock::scoped_guard guard(m_lock);
            thread::sleep(time_span(10));
            switch (ticker_id)
            {
            case 1:
                switch (m_state)
                {
                case not_ticked:
                    m_state = ticker1_ticked_1st;
                    break;
                case ticker2_ticked_1st:
                    m_state = ticker1_ticked_2nd;
                    break;
                case ticker2_ticked_2nd:
                    m_state = tickers_interlaced;
                    break;
                case ticker1_ticked_1st:
                case ticker1_ticked_2nd:
                default:
                    //  meaning nothing...
                    //  no-op
                    break;
                }
                ++m_ticker1_ticks;
                break;
            case 2:
                switch (m_state)
                {
                case not_ticked:
                    m_state = ticker2_ticked_1st;
                    break;
                case ticker1_ticked_1st:
                    m_state = ticker2_ticked_2nd;
                    break;
                case ticker1_ticked_2nd:
                    m_state = tickers_interlaced;
                    break;
                case ticker2_ticked_1st:
                case ticker2_ticked_2nd:
                default:
                    //  meaning nothing...
                    //  no-op
                    break;
                }
                ++m_ticker2_ticks;
                break;
            }
        }
    };

    struct ticker
    {
        int     m_id;
        static const size_t ticks = 50;
        ticker_tracker& m_tracker;

        ticker (int id, ticker_tracker& tracker)
        :   m_id(id)
        ,   m_tracker(tracker)
        {
        }

        void operator() () const
        {
            size_t cnt = 0;
            for (; cnt < ticks; ++cnt)
            {
                m_tracker.tick(m_id);
            }
        }
    };

    struct live_counter
    {
        size_t& m_sec_lived;

        live_counter (size_t& sec_lived)
        :   m_sec_lived(sec_lived)
        {
        }
        void operator() ()
        {
            size_t  cnt = 1;
            for (;;++cnt)
            {
                thread::test_cancel();
                thread::sleep(time_span(100));  //  Sleep for 1/10 of a second.
                if (cnt % 10 == 0 && cnt != 0)  //  Increment m_sec_lived every second
                    ++m_sec_lived;              //  except for the 1st time.
            }
        }
    };

    void test ()
    {
        {
            ticker_tracker  tracker;
            {
                ticker ticker1(1, tracker);
                ticker ticker2(2, tracker);
                thread  ticker1_th(ticker1);
                ticker2();
                //  ticker1_th is implicitly joined when destructor is invoked.
            }
            assert(tracker.m_state == ticker_tracker::tickers_interlaced);
            cout << "Test of concurrent (interlaced) execution passed." << endl;
            assert(tracker.m_ticker1_ticks == ticker::ticks);
            assert(tracker.m_ticker2_ticks == ticker::ticks);
            cout << "Test of completion of threads execution (" << ticker::ticks << " ticks per thread) passed." << endl;
            cout << "Test of implicit thread joining passed." << endl;
        }

        {
            size_t  sec_lived = 0;
            size_t  sec_lived_save = 0;
            {
                const size_t    sec_to_live = 1;
                live_counter    counter(sec_lived);
                thread          counter_th(counter);

                thread::sleep(time_span(sec_to_live * 1000));
                counter_th.cancel();
                counter_th.join();  //  explicit joining

                assert(sec_lived == sec_to_live);
                sec_lived_save = sec_lived;
                thread::sleep(time_span(sec_to_live * 1000));

                //  Making sure counter_th not still running after explicit joining
                assert(sec_lived == sec_lived_save);
            }
            cout << "Test of thread synchronous cancellation passed." << endl;
            cout << "Test of explicit thread joining passed." << endl;
        }
    }
}   //  namespace test_threading

namespace test_timing
{
    void test ()
    {
        {
            timestamp now;
            time_span ten_min(10 * 60 * 1000);
            timestamp ten_min_ago(now - ten_min);
            assert((now - ten_min_ago) == ten_min);
            time_span one_sec = ten_min / 60 / 10;
            assert(one_sec.in_sec() == 1);
            assert(one_sec.msec_of_sec() == 0);
            cout << "Test #1 of timestamp/time_span arithmetics passed." << endl;

            time_span five_sec(5 * 1000);
            timestamp pre_wait;
            thread::sleep(five_sec);
            now = timestamp();
            time_span waited(now - pre_wait);

            time_span four_point_5_sec(45 * 100);
            time_span five_point_5_sec(55 * 100);
            assert(waited > four_point_5_sec && waited < five_point_5_sec);
            cout << "Test #2 of timestamp/time_span arithmetics passed." << endl;
            cout << "Test of system sleeping test passed." << endl;
        }
    }
}   //  namespace test_timing


namespace test_udp
{

    void test ()
    {
        using namespace ipv4;
/*
        std::vector<transport_addr> addrs1 = getaddrinfo(
            type_tag<udp_transport>(),
            false,
            false,
            local_string(""),
            //local_string("localhost"),
            local_string("7777"));
        std::vector<transport_addr> addrs2 = getaddrinfo(
            type_tag<udp_transport>(),
            false,
            false,
            local_string(""),
            local_string("7788"));

        assert(addrs1.empty() == false);
        assert(addrs2.empty() == false);

        transport_addr  addr1 = addrs1.front();
        transport_addr  addr2 = addrs2.front();
*/

        transport_addr  addr1(interface_addr::loopback, 7777);
        transport_addr  addr2(interface_addr::loopback, 7788);
        udp_transport udp1(addr1);
        udp_transport udp2(addr2);

        const char msg[] = "a message string";
        char buf[sizeof(msg)] = "";

        udp1.write(msg, sizeof(msg), addr2);
        transport_addr from;
        udp2.read(buf, sizeof(buf), from);
        assert(from == addr1);
        assert(strcmp(buf, msg) == 0);
        cout << "Test of udp_transport read/write passed." << endl;

        bool timedout = false;
        try
        {
            udp2.read(buf, sizeof(buf), from, time_span(1000));
        }
        catch (socket_timedout_error&)
        {
            timedout = true;
        }
        assert(timedout == true);
        cout << "Test of udp_transport timed read passed." << endl;
    }

}   //  namespace test_udp

namespace test_tcp
{

    void test ()
    {
        using namespace ipv4;
/*
        std::vector<transport_addr> addrs = getaddrinfo(
            type_tag<tcp_listener>(),
            false,
            local_string(""),
            local_string("7777"));

        assert(addrs.empty() == false);
        transport_addr addr(addrs.front());
*/
        transport_addr addr(interface_addr::loopback, 7777);
        tcp_listener    listener(addr);

        tcp_transport   active_end(addr);   //  active open end
        cout << "Test of tcp_transport connetion establishment passed." << endl;
        const char msg[] = "a message string";
        char buf[sizeof(msg)] = "";
        {
            tcp_transport   passive_end(listener);  //  passive open end

            active_end.write_all(msg, sizeof(msg));
            passive_end.read_all(buf, sizeof(buf));
            assert(strcmp(msg, buf) == 0);
            cout << "Test of tcp_transport read/write passed." << endl;

            active_end.write_all(msg, sizeof(msg));
            bool timedout = false;
            try
            {
                passive_end.read_all(buf, sizeof(buf) + 10, time_span(1000));
            }
            catch (socket_timedout_error&)
            {
                timedout = true;
            }
            assert(timedout == true);

            cout << "Test of tcp_transport timed read passed." << endl;
            //  How to test timed write???

            //  passive_end closes here
        }

        size_t ret = active_end.read(buf, sizeof(buf));
        assert(ret == 0);
        cout << "Test of tcp_transport implicit close passed." << endl;
    }

}   //  namespace test_tcp

namespace test_process_singleton
{

    struct lifetime_tracker_t
    {
        bool    m_passed;
        int     m_ctor_order[4];
        size_t  m_ctor_idx;
        int     m_dtor_order[4];
        size_t  m_dtor_idx;
        size_t  m_ctor_cnt[4];
        size_t  m_dtor_cnt[4];

        lifetime_tracker_t ()
        {
            m_passed = true;
            m_ctor_idx = 0;
            m_dtor_idx = 0;

            m_ctor_order[0] = 0;
            m_ctor_order[1] = 1;
            m_ctor_order[2] = 2;
            m_ctor_order[3] = 3;
            m_dtor_order[0] = 0;
            m_dtor_order[1] = 2;
            m_dtor_order[2] = 1;
            m_dtor_order[3] = 3;

            m_ctor_cnt[0] = 0;
            m_ctor_cnt[1] = 0;
            m_ctor_cnt[2] = 0;
            m_ctor_cnt[3] = 0;
            m_dtor_cnt[0] = 0;
            m_dtor_cnt[1] = 0;
            m_dtor_cnt[2] = 0;
            m_dtor_cnt[3] = 0;
        }
        ~lifetime_tracker_t ()
        {
            assert(m_passed == true);
            cout << "Test of process_singleton<> destruction order passed." << endl;
            cout << "Test of process_singleton<> destruction once passed." << endl;
        }
        void on_boj_construct(int id)
        {
            if (m_passed == true && m_ctor_order[m_ctor_idx] == id && m_ctor_cnt[m_ctor_idx] == 0)
                ++m_ctor_idx;
            else
                m_passed = false;
        }
        void on_boj_destruct(int id)
        {
            if (m_passed == true && m_dtor_order[m_dtor_idx] == id && m_dtor_cnt[m_dtor_idx] == 0)
                ++m_dtor_idx;
            else
                m_passed = false;
        }
    };

    lifetime_tracker_t  lifetime_tracker;

    template <int id>
    struct obj_tmpl
    {
        obj_tmpl ()
        {
            thread::sleep(time_span(100));
            lifetime_tracker.on_boj_construct(id);
            constructed = true;
        }
        ~obj_tmpl ()
        {
            lifetime_tracker.on_boj_destruct(id);
        }
        static  bool constructed;
    };
    template <int id>
    bool obj_tmpl<id>::constructed = false;

    struct singleton_ref
    {
        singleton_ref () {}
        void operator() () const
        {
            for (size_t cnt = 0; cnt < 1000; ++cnt)
            {
                process_singleton<obj_tmpl<0>, singleton::lifetime_long>::instance();
                assert(obj_tmpl<0>::constructed == true);
                process_singleton<obj_tmpl<1> >::instance();
                assert(obj_tmpl<1>::constructed == true);
                process_singleton<obj_tmpl<2> >::instance();
                assert(obj_tmpl<2>::constructed == true);
                process_singleton<obj_tmpl<3>, singleton::lifetime_short>::instance();
                assert(obj_tmpl<3>::constructed == true);
            }
        }
    };
    void test ()
    {
        {
            singleton_ref ref;
            thread  th(ref);
            ref();
        }
        assert(lifetime_tracker.m_passed = true);
        cout << "Test of process_singleton<> construction order passed." << endl;
        cout << "Test of process_singleton<> fully constructed on first reference passed." << endl;
        cout << "Test of process_singleton<> fully constructed once passed." << endl;
        cout << "Test of process_singleton<> follow-up references wait for full construction passed." << endl;
    }

}   //  test_process_singleton


namespace test_thread_local_singleton
{

    struct obj
    {
        obj ()
        {
            printf("obj::obj()\n");
            printf("%08X\n", (int)this);
            //  See comments in destructor first.
            //  These are actually ok, just try to be consistant
            //  with destructor.
            //cout << "obj::obj()" << endl;
            //cout << this << endl;
        }
        ~obj ()
        {
            printf("obj::~obj()\n");
            printf("%08X\n", (int)this);
            //  Main thread's object apears to be deleted
            //  after C++ stdlib gets uninitialized.
            //  cout don't work here...
            //cout << "obj::~obj()" << endl;
            //cout << this << endl;
        }
    };
    struct singleton_ref
    {
        singleton_ref () {}
        void operator() () const
        {
            for (size_t cnt = 0; cnt < 1000; ++cnt)
            {
                thread_local_singleton<obj>::instance();
            }
        }
    };

    void test ()
    {
        //  I don't know how to write a test for thread_local_singleton<>
        //  You can inspect the outputs, it's working correctly.
        singleton_ref   ref;
        {
            thread  th(ref);
            ref();
        }
    }

}   //  namespace test_thread_local_singleton

#include <k2/memory.h>
#include <k2/allocator.h>

namespace test_allocator
{
    void test ()
    {
        static const size_t loop = 1000;
        static const size_t cnt = 1000;
        int*                array[cnt];
        size_t idx = 0;
        size_t i = 0;
        {
           static_pool<sizeof(int), false>    pool(cnt);
            i = 0;

            timestamp   start;
            for (; i < loop; ++i)
            {
                idx = 0;
                for(; idx < cnt; ++idx)
                {
                    array[idx] = reinterpret_cast<int*>(pool.allocate(sizeof(int)));
                }
                idx = 0;
                for(; idx < cnt; ++idx)
                {
                    pool.deallocate(array[idx], sizeof(int));
                }
            }
            cout << "Pool:" << endl;
            cout << (timestamp::now - start).in_msec() << endl;
        }
        {

            pool_allocator<int, cnt, static_pool, sizeof(int)>  alloc;
            //loose_sharing_pool_allocator<int, static_pool<cnt, sizeof(int)> >   alloc;
            i = 0;

            timestamp   start;
            for (; i < loop; ++i)
            {
                idx = 0;
                for(; idx < cnt; ++idx)
                {
                    array[idx] = alloc.allocate(1);
                }
                idx = 0;
                for(; idx < cnt; ++idx)
                {
                    alloc.deallocate(array[idx], 1);
                }
            }
            cout << "Pool Allocator:" << endl;
            cout << (timestamp::now - start).in_msec() << endl;
        }
        {
            std::allocator<int> alloc;

            i = 0;

            timestamp   start;
            for (; i < loop; ++i)
            {
                idx = 0;
                for(; idx < cnt; ++idx)
                {
                    array[idx] = alloc.allocate(1);
                }
                idx = 0;
                for(; idx < cnt; ++idx)
                {
                    alloc.deallocate(array[idx], 1);
                }
            }
            cout << "Standard Allocator:" << endl;
            cout << (timestamp::now - start).in_msec() << endl;
        }
        idx = 0;
        {
            i = 0;

            timestamp   start;
            for (; i < loop; ++i)
            {
                idx = 0;
                for(; idx < cnt; ++idx)
                {
                    array[idx] = new int;
                }
                idx = 0;
                for(; idx < cnt; ++idx)
                {
                    delete array[idx];
                }
            }
            cout << "new/delete" << endl;
            cout << (timestamp::now - start).in_msec() << endl;
        }
    }
}

int main ()
{
    //for (size_t cnt = 0; ; ++cnt)
    {

        //cout << "No: " << (int)cnt << endl;
        //test_thread_local_singleton::test();
        //test_allocator::test();
        //return 0;
        test_udp::test();
        test_timing::test();
        test_process_singleton::test();
        test_tcp::test();
        test_threading::test();
    }

    return  0;
}
