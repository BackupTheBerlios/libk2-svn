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
#ifndef K2_BITS_SCOPE_GUARD_H
#define K2_BITS_SCOPE_GUARD_H

#ifndef K2_EXCEPTION_H
#   include <k2/exception.h>
#endif
#ifndef K2_BITS_COPY_BOUNCER_H
#   include <k2/bits/copy_bouncer.h>
#endif

namespace k2
{

    class timestamp;

    template <typename GuardedType>
    class scoped_guard
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        scoped_guard (GuardedType& obj)
        :   m_obj(obj)
        {;
            m_obj.acquire();
        }
        scoped_guard (GuardedType& obj, const timestamp& timer)
        :   m_obj(obj)
        {
            if (m_obj.acquire(timer) == false)
                throw   timedout_error();
        }
        ~scoped_guard ()
        {
            m_obj.release();
        }

    private:
        GuardedType&  m_obj;
    };

}

#endif  //  !K2_BITS_SCOPE_GUARD_H
