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
#ifndef K2_BYTEMANIP_H
#define K2_BYTEMANIP_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_TYPE_MANIP_H
#   include <k2/type_manip.h>
#endif
#ifndef K2_ASSERT_H
#   include <k2/assert.h>
#endif

#ifndef K2_STD_H_ALGORITHM
#   include <algorithm>
#   define  K2_STD_H_ALGORITHM
#endif

#if !defined(K2_BYTEORDER_BIGENDIAN) && !defined(K2_BYTEORDER_LITTLEENDIAN)
#   error Requires K2_BYTEORDER_BIGENDIAN or K2_BYTEORDER_LITTLEENDIAN defined!!!
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


#if !defined(DOXYGEN_BLIND)

    namespace nonpublic
    {
#if defined(K2_BYTEORDER_LITTLEENDIAN)
        template <typename int_t_, size_t n_>
        int_t_ netconv (int_t_ value, const integer_tag<size_t, n_> /*size_tag*/);

        template <typename int_t_>
        int_t_ netconv (int_t_ value, const integer_tag<size_t, 1> /*size_tag*/)
        {
            return  value;
        }
        template <typename int_t_>
        int_t_ netconv (int_t_ value, const integer_tag<size_t, 2> /*size_tag*/)
        {
            int_t_  res = value;
            const char* src = reinterpret_cast<const char*>(&value);
            char*       dst = reinterpret_cast<char*>(&res);
            dst[0] = src[1];
            dst[1] = src[0];
            return  res;
        }
        template <typename int_t_>
        int_t_ netconv (int_t_ value, const integer_tag<size_t, 4> /*size_tag*/)
        {
            int_t_  res = value;
            const char* src = reinterpret_cast<const char*>(&value);
            char*       dst = reinterpret_cast<char*>(&res);
            dst[0] = src[3];
            dst[1] = src[2];
            dst[2] = src[1];
            dst[3] = src[0];
            return  res;
        }
        template <typename int_t_>
        int_t_ netconv (int_t_ value, const integer_tag<size_t, 8> /*size_tag*/)
        {
            int_t_  res = value;
            const char* src = reinterpret_cast<const char*>(&value);
            char*       dst = reinterpret_cast<char*>(&res);
            dst[0] = src[7];
            dst[1] = src[6];
            dst[2] = src[5];
            dst[3] = src[4];
            dst[4] = src[3];
            dst[5] = src[2];
            dst[6] = src[1];
            dst[7] = src[0];
            return  res;
        }
#endif  //  K2_BYTEORDER_LITTLEENDIAN
    }
#endif  //  !DOXYGEN_BLIND

    template <size_t size_>
    struct safe_alignof
    {
    private:
        static const size_t  alignment = 8;
    public:
        static const size_t  value =
            ((size_ + alignment-1) & ~(alignment - 1 ));
    };

}   //  namespace k2

#endif  //  !K2_BYTEMANIP_H
