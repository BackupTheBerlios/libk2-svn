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
#ifndef K2_ATOMIC_H
#define K2_ATOMIC_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif

#if !defined(K2_ATOMIC_USE_GLIBC) && !defined(K2_ATOMIC_USE_WIN32_API) && !defined(K2_ATOMIC_USE_PTHREADS)
#   if defined(K2_OS_LINUX)
#       define  K2_ATOMIC_USE_GLIBC
#   elif defined(K2_HAS_WIN32_API)
#       define  K2_ATOMIC_USE_WIN32
#   elif defined(K2_HAS_PTHREADS)
#       define  K2_ATOMIC_USE_PTHREADS
#   else
#       error   "libk2: how to perform atomic operation???"
#   endif
#endif

namespace k2
{

    /**
    *   \brief  empty structure containing typedef  for platform-dependent atomic type.
    */
    typedef volatile int    atomic_int_t;

    /**
    *   \relates atomic_int_t
    *   \brief  Increments (increases by one) \a value and
    *           test if \a value is non-zero atomically.
    */
    K2_DLSPEC bool atomic_increase (atomic_int_t& value) throw ();
    /**
    *   \relates atomic_int_t
    *   \brief  Decrements (decreases by one) \a value and
    *           test if \a value is non-zero atomically.
    */
    K2_DLSPEC bool atomic_decrease (atomic_int_t& value) throw ();

}   //  namespace k2

#endif  //  !K2_ATOMIC_H
