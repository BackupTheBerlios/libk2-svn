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
#ifndef K2_BITS_DEFALLC_BASE_H
#define K2_BITS_DEFALLC_BASE_H

#include <cstddef>

namespace k2
{

    /**
    *   \brief A class template to help implementing allocators.
    */
    template <typename T>
    class defalloc_base
    {
    public:
        typedef T               value_type;
        typedef size_t          size_type;
        typedef std::ptrdiff_t  difference_type;
        typedef const T*        const_pointer;
        typedef T*              pointer;
        typedef const T&        const_reference;
        typedef T&              reference;

        size_type   max_size () const
        {
            return  size_type(-1) / safe_alignof<sizeof(value_type)>::value;
        }
        void construct (pointer p, const_reference v)
        {
            new (p) value_type(v);
        }
        void destroy (pointer p)
        {
            p->~value_type();
        }
        const_pointer address (const_reference r)
        {
            return  &r;
        }
        pointer address (reference r)
        {
            return  &r;
        }
    };
    template <>
    class defalloc_base<void>
    {
    public:
        typedef void        value_type;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
        typedef const void* const_pointer;
        typedef void*       pointer;
    };

}   //  namespace k2

#endif  //  !K2_BITS_DEFALLC_BASE_H
