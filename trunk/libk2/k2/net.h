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
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_NET_H
#define K2_NET_H

#ifndef K2_TYPE_MANIP_H
#   include <k2/type_manip.h>
#endif

#ifndef K2_BITS_ENDIAN_H
#   include <k2/bits/endian.h>
#endif
#if !defined(K2_BYTE_ORDER)
#   error "libk2 : No byte-order defined."
#endif

#ifndef K2_STD_H_ALGORITHM
#   define  K2_STD_H_ALGORITHM
#   include <algorithm>
#endif

namespace k2
{

#if !defined(DOXYGENBLIND)

    namespace nonpublic
    {
        template <typename Int, size_t Size>
        Int net_conv (Int val, const integer_tag<size_t, Size>);

        template <typename Int>
        Int net_conv (Int val, const integer_tag<size_t, 1>)
        {
            return  v
        }

#if (K2_BYTE_ORDER == K2_LITTLE_ENDIAN)

        template <typename Int>
        Int net_conv (Int val, const integer_tag<size_t, 2>)
        {
            Int  ret = val;
            const char* src = reinterpret_cast<const char*>(&val);
            char*       dst = reinterpret_cast<char*>(&ret);
            dst[0] = src[1];
            dst[1] = src[0];
            return  ret;
        }
        template <typename Int>
        Int net_conv (Int val, const integer_tag<size_t, 4>)
        {
            Int  ret = val;
            const char* src = reinterpret_cast<const char*>(&val);
            char*       dst = reinterpret_cast<char*>(&ret);
            dst[0] = src[3];
            dst[1] = src[2];
            dst[2] = src[1];
            dst[3] = src[0];
            return  ret;
        }

#elif (K2_BYTE_ORDER == K2_BIT_ENDIAN)

        template <typename Int>
        Int net_conv (Int val, const integer_tag<size_t, 2>)
        {
            return  val;
        }
        template <typename Int>
        Int net_conv (Int val, const integer_tag<size_t, 4>)
        {
            return  val;
        }

#else

#endif  //  K2_BYTEORDER == K2_LITTLE_ENDIAN
    }
#endif  //  !DOXYGENBLIND

    template <typename Int>
    Int net_conv (Int val)
    {
        return  nonpublic::net_conv(val, integer_tag<size_t, sizeof(Int)>());
    }

    typedef uint16_t    host16_t;
    typedef uint32_t    host32_t;

    template <typename OutIter, typename Int>
    OutIter net_encode (Int from, OutIter to)
    {
        Int net = net_conv(from);
        std::copy(
            reinterpret_cast<const char*>(&net),
            reinterpret_cast<const char*>(&net + 1),
            to);
       return   to + sizeof(from);
    }
    template <typename InIter, typename Int>
    InIter net_decode (InIter from, Int& to)
    {
        std::copy(
            from,
            from + sizeof(to),
            reinterpret_cast<char*>(&to));
        return  from + sizeof(to);
    }

}   //  namespace k2

#endif  //  !K2_NET_H
