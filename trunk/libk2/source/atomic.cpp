/*  libk2   <atomic.cpp>

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
#include <k2/atomic.h>

#if defined(K2_OS_LINUX)
#   include <linux/config.h>
#   if defined(CONFIG_SMP)
#       define LOCK "acquire ; "
#   else
#       define LOCK ""
#   endif
#elif defined(K2_HAS_WIN32_API)
#   include <windows.h>
#else
#   error   "How to perform atomic operation???"
#endif  //  K2_HAS_WIN32_API

namespace k2
{

    bool atomic_increase (atomic_int_t& value) throw ()
#   if defined(K2_OS_LINUX)
    {
    /**
    * atomic_inc_and_test - increment and test
    * @v: pointer of type atomic_t
    *
    * Atomically increments @v by 1
    * and returns true if the result is zero, or false for all
    * other cases.  Note that the guaranteed
    * useful range of an atomic_t is only 24 bits.
    */

        unsigned char c;

        __asm__ __volatile__(
            LOCK "incl %0; sete %1"
            :"=m" (value), "=qm" (c)
            :"m" (value) : "memory");
        return c != 0;
    }
#   else
    {
        return  ::InterlockedIncrement(&value) == 0 ? false : true;
    }
#   endif

    bool atomic_decrease (atomic_int_t& value) throw ()
#   if defined(K2_OS_LINUX)
    {
    /**
    * atomic_dec_and_test - decrement and test
    * @v: pointer of type atomic_t
    *
    * Atomically decrements @v by 1 and
    * returns true if the result is 0, or false for all other
    * cases.  Note that the guaranteed
    * useful range of an atomic_t is only 24 bits.
    */

        unsigned char c;

        __asm__ __volatile__(
            LOCK "decl %0; sete %1"
            :"=m" (value), "=qm" (c)
            :"m" (value) : "memory");
        return c != 0;
    }
#   else
    {
        return  ::InterlockedDecrement(&value) == 0 ? false : true;
    }
#   endif

}   //  namespace k2
