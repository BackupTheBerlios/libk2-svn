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
#ifndef K2_SPIN_LOCK_H
#define K2_SPIN_LOCK_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif
#ifndef K2_SCOPE_H
#   include <k2/scope.h>
#endif
#ifndef K2_ATOMIC_H
#   include <k2/atomic.h>
#endif

namespace k2
{

    class timestamp;

    /** \defgroup   Threading
    */

    /**
    *   \ingroup    Threading
    *   \brief      Threading synchronization primitive.
    *   \relates    scoped_guard<>
    *   \relates    timestamp
    */
    class spin_lock
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        typedef scoped_guard<spin_lock> scoped_guard;

        K2_DLSPEC   spin_lock ();

        K2_DLSPEC   void acquire ();
        K2_DLSPEC   bool acquire (const timestamp& timer);
        K2_DLSPEC   void release ();

    private:
        atomic_int_t    m_lock;
    };


}   //  namespace k2

#endif  //  !K2_SPIN_LOCK_H
