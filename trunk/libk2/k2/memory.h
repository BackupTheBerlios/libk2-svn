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
#ifndef K2_MEMORY_H
#define K2_MEMORY_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif

//  Try to avoid including OS specific header,
//  unless user option forcing.
//  It's recommended to compile without K2_INLINE_STACK_PUSH first,
//  to make sure user code does NOT depend on OS specific includes.
//  After doing so, user could turn this option on if he/she needs it.
#ifdef  K2_INLINE_STACK_PUSH
#   if      defined(K2_RT_MSVCRT)
#       include <malloc.h>
#   elif    defined(K2_RT_POSIX)
#       include <alloca.h>
#   endif
#endif

namespace k2
{

#if !defined(K2_INLINE_STACK_PUSH)
    //  Default
    K2_DLSPEC void* stack_push (size_t bytes);
#else
    //  User optional
    inline void* stack_push (size_t bytes)
    {
#   if      defined(K2_RT_MSVCRT)
        return  _alloca(bytes);
#   elif    defined(K2_RT_POSIX)
        return  alloca(bytes);
#   endif
    }
#endif

    template <typename type_>
    struct destroyer
    {
        explicit destroyer (type_* p)
        :   m_p(p) {}

        void operator() () const
        {
            delete  m_p;
        }
        type_*  m_p;
    };

}   //  namespace k2

#endif  //  !K2_MEMORY_H
