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
#ifndef K2_BYTEMANIP_H
#define K2_BYTEMANIP_H

#ifndef K2_ASSERT_H
#   include <k2/assert.h>
#endif

#ifndef K2_STD_H_ALGORITHM
#   include <algorithm>
#   define  K2_STD_H_ALGORITHM
#endif


namespace k2
{

    inline void reverse_byteorder (void* p, size_t n)
    {

#if defined(K2_DEBUG)
        if (n < 2)
            return;
#endif  //  K2_DEBUG

        char* head = reinterpret_cast<char*>(p);
        char* tail = reinterpret_cast<char*>(p) + n - 1;

        for (; head < tail; ++head, --tail)
            std::swap(*head, *tail);
    }

    struct safe_alignof
    {
        static const size_t default_aligment = 8;

        template <size_t Size, size_t Alignment = default_aligment>
        struct constant
        {
            static const size_t  value =
                ((Size + Alignment - 1) & ~(Alignment - 1 ));
        };

        size_t operator() (size_t size, size_t alignment = default_aligment)
        {
            return  ((size + alignment-1) & ~(alignment - 1 ));
        }
    };

#if(0)
    template <size_t Bytes = 0, size_t Alignment = 8>
    struct safe_alignof
    {
        static const size_t  value =
            ((Bytes + Alignment - 1) & ~(Alignment - 1 ));
    };

    template <size_t Alignment>
    struct safe_alignof<0, Alignment>
    {
        static const size_t default_aligment = 8;

        size_t  operator() (size_t bytes)
        {
            return  ((size + Alignment - 1) & ~(Alignment - 1 ));
        }
    };
#endif

}   //  namespace k2

#endif  //  !K2_BYTEMANIP_H
