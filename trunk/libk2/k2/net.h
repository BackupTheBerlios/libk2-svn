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
#ifndef K2_BYTE_MANIP_H
#   include <k2/byte_manip.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif

namespace k2
{

    template <typename Int>
    Int netconv (Int value)
#if defined(K2_BYTEORDER_BIGENDIAN)
    {
        //  Big-endian need no net-to-host/host-to-net conversion.
        K2_STATIC_ASSERT(is_integer<Int>::value, value_is_not_integer);
        return  value;
    }
#else
    {
        K2_STATIC_ASSERT(is_integer<Int>::value, value_is_not_integer);
        return  nonpublic::netconv(value, integer_tag<size_t, sizeof(Int)>());
    }
#endif

    template <typename Int>
    Int net2host (Int value)
    {
        return  netconv(value);
    }
    template <typename Int>
    Int host2net (Int value)
    {
        return  netconv(value);
    }

    typedef uint16_t    host16_t;
    typedef uint32_t    host32_t;

    template <bool Aligned = false>
    struct net_codec
    {
        template <typename Int>
        static Int fetch (const void* from)
        {
            Int   tmp;
            std::copy(
                reinterpret_cast<char*>(from),
                reinterpret_cast<char*>(from) + sizeof(Int),
                reinterpret_cast<char*>(&tmp));

            return  net2host(tmp);
        }
        template <typename Int>
        static void dump (void* to, Int val)
        {
            val = host2net(val);

            std::copy(
                reinterpret_cast<const char*>(&val),
                reinterpret_cast<const char*>(&val + sizeof(Int)),
                reinterpret_cast<char*>(to));
        }
    };
    template <>
    struct net_codec</*Aligned =*/ true>
    {
        template <typename Int>
        static Int fetch (const void* from)
        {
            return  net2host(*reinterpret_cast<const Int*>(from));
        }
        template <typename Int>
        static void dump (void* to, Int val)
        {
            *reinterpret_cast<Int*>(to) = host2net(val);
        }
    };

    template <bool Aligned = false>
    struct host_codec
    {
        template <typename Int>
        static Int fetch (const void* from)
        {
            Int   tmp;
            std::copy(
                reinterpret_cast<char*>(from),
                reinterpret_cast<char*>(from) + sizeof(Int),
                reinterpret_cast<char*>(&tmp));

            return  tmp;
        }
        template <typename Int>
        static void dump (void* to, Int val)
        {
            std::copy(
                reinterpret_cast<const char*>(&val),
                reinterpret_cast<const char*>(&val + sizeof(Int)),
                reinterpret_cast<char*>(to));
        }
    };
    template <>
    struct host_codec</*Aligned =*/ true>
    {
        template <typename Int>
        static Int fetch (const void* from)
        {
            return  *reinterpret_cast<const Int*>(from);
        }
        template <typename Int>
        static void dump (void* to, Int val)
        {
            *reinterpret_cast<Int*>(to) = val;
        }
    };

    template <typename Codec, typename Int>
    Int unserialize (const void*& from)
    {
        Int ret = Codec::fetch(from);
        std::advance(reinterpret_cast<const char*&>(from), sizeof(Int));
        return  ret;
    }
    template <typename Codec, typename Int>
    void serialize (void*& to, Int val)
    {
        Codec::dump(to, val);
        std::advance(reinterpret_cast<char*&>(from), sizeof(Int))
    }

}   //  namespace k2

#endif  //  !K2_NET_H
