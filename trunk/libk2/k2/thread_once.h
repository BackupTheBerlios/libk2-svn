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
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_THREAD_ONCE_H
#define K2_THREAD_ONCE_H

#ifndef K2_DLSPEC_H
#   include <k2/dlspec.h>
#endif
#ifndef K2_ATOMIC_H
#   include <k2/atomic.h>
#endif

namespace k2
{

    /** \defgroup   Threading
    */

    /**
    *   \ingroup    Threading
    *   \brief      Once-only initialization.
    *
    *   The purpose of \a thread_once is to ensure that a piece of
    *   initialization code is executed exactly once.
    *
    *   A \a thread_once object must be statically initialized to
    *   K2_THREAD_ONCE_INIT.
    *
    *   \deprecated Use process_singleton, class_singleton or
    *   thread_specific_singleton instead.
    */
    class thread_once
    {
    public:
        /**
        *   \brief  Invokes \a once_functor once.
        *   \param  once_functor    Funtor to invoke.
        */
        template <typename unary_functor_t_>
        void run (unary_functor_t_ once_functor)
        {
            thread_once::run_impl(
                once_functor_wrapper<unary_functor_t_>,
                &once_functor);
        }

#ifndef DOXYGEN_BLIND
        atomic_int_t    started;
        bool            done;
#endif  //  DOXYGEN_BLIND

    private:
        template <typename unary_functor_t_>
        static void once_functor_wrapper (void* p)
        {
            unary_functor_t_*   pf = reinterpret_cast<unary_functor_t_*>(p);
            (*pf)();
        }
        K2_DLSPEC void run_impl (void (*once_functor)(void*), void* arg) ;
    };
    /**
    *   \ingroup    Threading
    *   \brief      thread_once initializer
    *
    *   thread_once static initializer macro.
    *   \relates    thread_once
    */
#   define K2_THREAD_ONCE_INIT  {-1, false}

}   //  namespace k2

#endif  //  !K2_THREAD_ONCE_H
