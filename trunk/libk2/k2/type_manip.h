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
#ifndef K2_TYPE_MANIP_H
#define K2_TYPE_MANIP_H

#ifndef K2_STDINT_H
#   include <k2/stdint.h>
#endif

namespace k2
{

    template <typename Type, bool>
    struct type_if
    {
    };
    template <typename Type>
    struct type_if<Type, true>
    {
        typedef Type   type;
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

    template <typename Type>
    struct is_reference
    {
        static const bool   value = false;
    };
    template <typename Type>
    struct is_reference<Type&>
    {
        static const bool   value = true;
    };

    template <typename Type>
    struct is_const
    {
        static const bool   value = false;
    };
    template <typename Type>
    struct is_const<const Type>
    {
        static const bool   value = true;
    };

    template <typename Type>
    struct is_pointer
    {
        static const bool   value = false;
    };
    template <typename Type>
    struct is_pointer<Type*>
    {
        static const bool   value = true;
    };

    template <typename Type>
    struct is_array
    {
        static const bool   value = false;
    };
    template <typename Type, size_t Size>
    struct is_array<Type[Size]>
    {
        static const bool   value = true;
        static const size_t size = Size;
    };

    template <typename Type>
    struct strip_const
    {
        typedef Type   type;
    };
    template <typename Type>
    struct strip_const<const Type>
    {
        typedef Type   type;
    };

    template <typename Type>
    struct strip_reference
    {
        typedef Type   type;
    };
    template <typename Type>
    struct strip_reference<Type&>
    {
        typedef Type   type;
    };

    template <typename Type>
    struct strip_pointer
    {
        typedef Type   type;
    };
    template <typename Type>
    struct strip_pointer<Type*>
    {
        typedef Type   type;
    };

    template <typename Type>
    struct strip_array
    {
        typedef Type   type;
    };
    template <typename Type, size_t Size>
    struct strip_array<Type[Size]>
    {
        typedef Type   type;
    };



    template <typename Type>
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

    template <typename Type>
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

    template <typename Type>
    struct is_primitive
    {
        static const bool   value =
            is_integer<Type>::value ? true :
            is_float_point<Type>::value ? true :
            is_pointer<Type>::value ? true :
            false;
    };

    template <typename Type>
    struct type_tag
    {
        typedef Type   type;
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
