/*
    Copyright (c) 2003, 2004, Kenneth Chang-Hsing Ho
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the k2 nor the names of its contributors may be used
      to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef K2_FAST_LOCK_H
#define K2_FAST_LOCK_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_MUTEX_H
#   include <k2/mutex.h>
#endif
#ifndef K2_SPIN_LOCK_H
#   include <k2/spin_lock.h>
#endif
#ifndef K2_DUMMY_LOCK_H
#   include <k2/dummy_lock.h>
#endif
#ifndef K2_SCOPE_H
#   include <k2/scope.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif

namespace k2
{

    class timestamp;

    template <bool threading_>
    struct fast_lock;

    template <>
    struct fast_lock<true>
    {
    private:
        typedef spin_lock   impl_type;
        impl_type           m_impl;

    public:
        K2_INJECT_COPY_BOUNCER();

        typedef scoped_guard<fast_lock<true> >
            scoped_guard;

        void acquire ()
        {
            m_impl.acquire();
        }
        bool acquire (const timestamp& timer)
        {
            m_impl.acquire(timer);
        }
        void release ()
        {
            m_impl.release();
        }
    };
    template <>
    struct fast_lock<false>
    {
    private:
        typedef dummy_lock  impl_type;
        impl_type           m_impl;

    public:
        typedef scoped_guard<fast_lock<false> >
            scoped_guard;

        void acquire ()
        {
            m_impl.acquire();
        }
        bool acquire (const timestamp& timer)
        {
            m_impl.acquire(timer);
        }
        void release ()
        {
            m_impl.release();
        }
    };

}   //  namespace k2

#endif  //  !K2_FAST_LOCK_H
