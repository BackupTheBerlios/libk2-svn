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
#ifndef K2_NET_H
#define K2_NET_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_TYPE_MANIP_H
#   include <k2/byte_manip.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif

namespace k2
{

    template <typename int_t_>
    int_t_ netconv (int_t_ value)
#if defined(K2_BYTEORDER_BIGENDIAN)
    {
        //  Big-endian need no net-to-host/host-to-net conversion.
        K2_STATIC_ASSERT(is_integer<int_t_>::value, value_is_not_integer);
        return  value;
    }
#else
    {
        K2_STATIC_ASSERT(is_integer<int_t_>::value, value_is_not_integer);
        return  nonpublic::netconv(value, integer_tag<size_t, sizeof(int_t_)>());
    }
#endif

    template <typename int_t_>
    int_t_ net2host (int_t_ value)
    {
        return  netconv(value);
    }
    template <typename int_t_>
    int_t_ host2net (int_t_ value)
    {
        return  netconv(value);
    }

    typedef uint16_t    host16_t;
    typedef uint32_t    host32_t;


    template <typename int_t_>
    void net_encode (void*& buf, int_t_ val)
    {
        K2_STATIC_ASSERT(is_integer<int_t_>::value, value_is_not_integer);
        uint8_t*        dst = reinterpret_cast<uint8_t*>(buf);
        const uint8_t*  src = reinterpret_cast<const uint8_t*>(&val);

#if defined(K2_BYTEORDER_LITTLEENDIAN)
        reverse_byteorder(val);
#endif

        std::copy(src, src + sizeof(val), dst);
        buf = dst + sizeof(val);
    }

    template <typename int_t_>
    int_t_ net_decode (const void*& buf)
    {
        K2_STATIC_ASSERT(is_integer<int_t_>::value, value_is_not_integer);

        const uint8_t*  src = reinterpret_cast<const uint8_t*>(buf);
        int_t_          val;
        uint8_t*        dst = reinterpret_cast<uint8_t*>(&val);

        std::copy(src, src + sizeof(val), dst);
#if defined(K2_BYTEORDER_LITTLEENDIAN)
        reverse_byteorder(val);
#endif
        buf = src + sizeof(val);
        return  val;
    }

}   //  namespace k2

#endif  //  !K2_NET_H
