/*  libk2   <k2/type_manip.h>

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
#ifndef K2_TYPE_MANIP_H
#define K2_TYPE_MANIP_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_STDINT_H
#   include <k2/stdint.h>
#endif

namespace k2
{

    template <typename type_, bool>
    struct type_if
    {
    };
    template <typename type_>
    struct type_if<type_, true>
    {
        typedef type_   type;
    };

    template <typename type1_, typename type2_, bool>
    struct type_select
    {
        typedef type2_  type;
    };
    template <typename type1_, typename type2_>
    struct type_select<type1_, type2_, true>
    {
        typedef type1_  type;
    };

    template <typename lhs_t_, typename rhs_t_>
    struct type_equality
    {
        static const bool value = false;
    };
    template <typename type_t_>
    struct type_equality<type_t_, type_t_>
    {
        static const bool value = true;
    };

    template <typename type_>
    struct is_reference
    {
        static const bool   value = false;
    };
    template <typename type_>
    struct is_reference<type_&>
    {
        static const bool   value = true;
    };

    template <typename type_>
    struct is_const
    {
        static const bool   value = false;
    };
    template <typename type_>
    struct is_const<const type_>
    {
        static const bool   value = true;
    };

    template <typename type_>
    struct is_pointer
    {
        static const bool   value = false;
    };
    template <typename type_>
    struct is_pointer<type_*>
    {
        static const bool   value = true;
    };

    template <typename type_>
    struct is_array
    {
        static const bool   value = false;
    };
    template <typename type_, size_t size_>
    struct is_array<type_[size_]>
    {
        static const bool   value = true;
        static const size_t size = size_;
    };

    template <typename type_>
    struct strip_const
    {
        typedef type_   type;
    };
    template <typename type_>
    struct strip_const<const type_>
    {
        typedef type_   type;
    };

    template <typename type_>
    struct strip_reference
    {
        typedef type_   type;
    };
    template <typename type_>
    struct strip_reference<type_&>
    {
        typedef type_   type;
    };

    template <typename type_>
    struct strip_pointer
    {
        typedef type_   type;
    };
    template <typename type_>
    struct strip_pointer<type_*>
    {
        typedef type_   type;
    };

    template <typename type_>
    struct strip_array
    {
        typedef type_   type;
    };
    template <typename type_, size_t size_>
    struct strip_array<type_[size_]>
    {
        typedef type_   type;
    };



    template <typename type_>
    struct is_integer
    {
        static const bool   value = false;
    };
    template <>
    struct is_integer<bool>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<char>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<signed char>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<unsigned char>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<short>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<unsigned short>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<int>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<unsigned int>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<long>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<unsigned long>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<long long>
    {
        static const bool   value = true;
    };
    template <>
    struct is_integer<unsigned long long>
    {
        static const bool   value = true;
    };

    template <typename type_>
    struct is_float_point
    {
        static const bool   value = false;
    };
    template <>
    struct is_float_point<float>
    {
        static const bool   value = true;
    };
    template <>
    struct is_float_point<double>
    {
        static const bool   value = true;
    };
    template <>
    struct is_float_point<long double>
    {
        static const bool   value = true;
    };

    template <typename type_>
    struct is_primitive
    {
        static const bool   value =
            is_integer<type_>::value ? true :
            is_float_point<type_>::value ? true :
            is_pointer<type_>::value ? true :
            false;
    };

    template <typename type_>
    struct type_tag
    {
        typedef type_   type;
    };

    template <typename int_t_, int_t_ value_>
    struct integer_tag
    {
        typedef int_t_      type;
        static const int_t_ value = value_;
    };

    template <bool value>
    struct bool_tag : integer_tag<bool, value>
    {
    };

    typedef bool_tag<true>  true_tag;
    typedef bool_tag<false> false_tag;

}   //  namespace k2


#endif  //  !K2_TYPE_MANIP_H
