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
#ifndef K2_BITS_ATOMIC_H
#define K2_BITS_ATOMIC_H

#ifndef K2_BITS_DLSPEC_H
#   include <k2/bits/dlspec.h>
#endif

namespace k2
{

//  !kh!
//  There are only two functions and a typedef in <bits/atomicity.h>.
//  They are not typically used in user code. It's not likely to break anything.
#if defined(__GNUC__)
    namespace nonpublic
    {
#       include <bits/atomicity.h>
    }
#endif

    /**
    *   \brief  Typedef for atomic arithmetic type.
    */
    typedef volatile int    atomic_int_t;

    /**
    *   \relates atomic_int_t
    *   \brief  Increments (increases by one) \a value and
    *           test if \a value is non-zero atomically.
    */
#if defined(__GNUC__)
    inline bool atomic_increase (atomic_int_t& value) throw ()
    {
        nonpublic::__exchange_and_add(&value, 1);
        return  nonpublic::__exchange_and_add(&value, 0);
    }
#else
    K2_DLSPEC bool atomic_increase (atomic_int_t& value) throw ();
#endif


    /**
    *   \relates atomic_int_t
    *   \brief  Decrements (decreases by one) \a value and
    *           test if \a value is non-zero atomically.
    */
#if defined(__GNUC__)
    inline bool atomic_decrease (atomic_int_t& value) throw ()
    {
        nonpublic::__exchange_and_add(&value, -1);
        return  i386_glibcxx::__exchange_and_add(&value, 0);
    }
#else
    K2_DLSPEC bool atomic_decrease (atomic_int_t& value) throw ();
#endif
}   //  namespace k2

#endif  //  !K2_BITS_ATOMIC_H
