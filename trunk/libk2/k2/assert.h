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
#ifndef K2_ASSERT_H
#define K2_ASSERT_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_STD_H_EXCEPTION
#   include <exception>
#   define  K2_STD_H_EXCEPTION
#endif

#define K2_STATIC_ASSERT(expr, msg_class)\
{\
    struct msg_class\
    {\
    };\
    msg_class msg;\
    (void)sizeof(k2::static_assert_failed<(expr)>(msg));\
}

namespace k2
{

    template <bool>
    struct static_assert_failed;

    template <>
    struct static_assert_failed<true>
    {
        static_assert_failed (...);
    };

    inline void runtime_assert (bool expr)
#if defined(K2_DEBUG)
    {
        if (expr == false)
            throw   std::bad_exception();
    }
#else
    {
    }
#endif

}   //  namespace k2


#endif  //  !K2_ASSERT_H
