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
#ifndef K2_THREAD_H
#define K2_THREAD_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif
#ifndef K2_MEMORY_H
#   include <k2/memory.h>
#endif
#ifndef K2_STDINT_H
#   include <k2/stdint.h>
#endif

namespace k2
{

    class timestamp;
    class time_span;

    /** \defgroup   Threading
    */

#if !defined(DOXYGEN_BLIND)
    namespace nonpublic
    {
        struct thread_cntx;
    }
#endif  //  !DOXYGEN_BLIND

    /**
    *   \ingroup    Threading
    *   \brief      Concurrent excecution construct.
    *
    *   The thread class provides the functionality to manipulate concurrent
    *   executions.
    */
    class thread
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        /**
        *   \brief      See thread::test_cancel ().
        */
        struct cancel_signal {};

        /**
        *   Allocates resource for *this and invokes thread_entry in parallel.
        *
        *   \param  thread_entry    A copy-constructable generator functor.
        *   \throw  bad_resource_alloc
        */
        template <typename generator_t_>
        thread (generator_t_ thread_entry)
        :   m_pcntx(thread::spawn(thread_entry, false))
        {
        }

        /**
        *   Creates a thread object that reclaims its resource when thread_entry
        *   finishes.
        *
        *   \param  thread_entry    A copy-constructable generator functor.
        *   \throw  bad_resource_alloc
        */
        template <typename generator_t_>
        static void spawn_detached (generator_t_ thread_entry)
        {
            thread::spawn(thread_entry, true);
        }

        /**
        *   Blocks calling thread and wait for thread_entry to finish execution
        *   then reclaims its resource.
        */
        K2_DLSPEC ~thread ();

        /**
        *   Sets the cancelled flag in *this.
        */
        K2_DLSPEC void      cancel () ;

        /**
        *   Waits *this to stop execution .
        */
        K2_DLSPEC void      join ();
        K2_DLSPEC bool      join (const timestamp& timer);

        /**
        *   Sets the cancel-enabled flag of now thread.
        *
        *   \return The original value.
        */
        K2_DLSPEC static bool   cancel_enabled (bool value) ;

        /**
        *   Tests if *this's cancelled flag has been set, if true, terminates
        *   calling thread by throwing thread::cancel_signal.
        *   Does nothing if now thread's cancelled flag has not been set.
        *   Does nothing if now thread's cancel-enabled flag is not set.
        *   Do NOT catch thread::cancel_signal.
        *
        *   \throw      thread::cancel_signal
        *
        *   See thread::cancel()
        */
        K2_DLSPEC static void   test_cancel ();

        /**
        *   Tests if *this's cancelled flag has been set, if true, invokes
        *   do_on_cancel, then terminates calling thread by throwing
        *   thread::cancel_signal.
        *   Does nothing if now thread's cancelled flag has not been set.
        *   Does nothing if now thread's cancel-enabled flag is not set.
        *   Do NOT catch thread::cancel_signal.
        *   do_on_cancel() should not throw.
        *
        *   \throw      thread::cancel_signal
        *
        *   See thread::cancel()
        */
        template <typename generator_t_>
        static void test_cancel (const generator_t_& do_on_cancel)
        {
            generator_t_    local(do_on_cancel);
            try
            {
                thread::test_cancel();
            }
            catch (cancel_signal&)
            {
                try
                {
                    local();
                }
                catch (...)
                {
                }
                throw;
            }
        }

        /**
        *   Hits scheduler it's ok to to schedule out calling thread.
        */
        K2_DLSPEC static void sched_yield () ;

        /**
        *   Puts now thread to sleep.
        */
        K2_DLSPEC static void sleep (const time_span& span);
        K2_DLSPEC static void sleep (size_t msec);

    private:
        template <typename generator_t_>
        static void thread_entry_wrapper (void* arg)
        {
            std::auto_ptr<generator_t_>
                pthread_entry(reinterpret_cast<generator_t_*>(arg));

            //  If you get a compile error here, note that thread_entry
            //  has to be a functor taking no argument (a.k.a generator).
            (*pthread_entry)();
        }
        template <typename generator_t_>
        static nonpublic::thread_cntx* spawn (const
            generator_t_& thread_entry, bool detached)
        {
            //  If you get a compile error here, note that thread_entry
            //  has to be copy constructable.
            generator_t_* pthread_entry = new generator_t_(thread_entry);
            return  spawn_impl(thread_entry_wrapper< generator_t_ >,
                reinterpret_cast<void*>(pthread_entry), detached);
        }
        K2_DLSPEC static nonpublic::thread_cntx* spawn_impl (
            void (*thread_entry)(void*), void* arg, bool detached);
        K2_DLSPEC static void*  pthread_entry_wrapper (void*);

        nonpublic::thread_cntx*     m_pcntx;
    };  //  class thread

}   //  namespace k2

#endif  //  !K2_THREAD_H
