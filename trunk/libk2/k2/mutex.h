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
#ifndef K2_MUTEX_H
#define K2_MUTEX_H

#ifndef K2_BITS_DLSPEC_H
#   include <k2/bits/dlspec.h>
#endif
#ifndef K2_BITS_COPY_BOUNCER_H
#   include <k2/bits/copy_bouncer.h>
#endif
#ifndef K2_BITS_SCOPE_GUARD_H
#   include <k2/bits/scoped_guard.h>
#endif

namespace k2
{

    class timestamp;
    class time_span;

    /** \defgroup   Threading
    */

    /**
    *   \ingroup    Threading
    *   \brief      MUTual EXclusion.
    *   \relates    scoped_guard<>
    *   \relates    cond_var
    *   \relates    timestamp
    */
    class mutex
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        typedef scoped_guard<mutex> scoped_guard;

        /**
        *   \brief      Allocates resource for *this.
        */
        K2_DLSPEC mutex ();
        /**
        *   \brief      Releases resource for *this.
        */
        K2_DLSPEC ~mutex () ;

        /**
        *   \brief      Acquires ownership of *this, blocks calling thread
        *               until successful.
        *   \throw      thread_error
        */
        K2_DLSPEC void  acquire () ;
        /**
        *   \brief      Acquires ownership of *this, blocks calling thread
        *               until successful, or timed-out.
        *   \return     true, if successfully acquired ownership.
        *               false, if timed-out.
        *   \throw      thread_error
        */
        K2_DLSPEC bool  acquire (const timestamp& timer) ;

        /**
        *   \brief      Releases ownership of *this.
        *   \throw      thread_error
        */
        K2_DLSPEC void  release () ;

#if !defined(DOXYGEN_BLIND)
        struct handle
        {
            long    holder[8];
        };
#endif  //  !DOXYGEN_BLIND

    private:
        handle  m_handle;
        friend class cond_var;
    };  //  class mutex

}   //  namespace k2

#endif  //  !K2_MUTEX_H
