/*  libk2   <k2/byte_manip.h>

    Copyright (C) 2003, 2004 Kenneth Chang-Hsing Ho.

    Written by Kenneth Chang-Hsing Ho <kenho@bluebottle.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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

    void reverse_byteorder (void* p, size_t n)
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

}   //  namespace k2

#endif  //  !K2_BYTEMANIP_H
