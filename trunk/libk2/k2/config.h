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
#ifndef K2_CONFIG_H
#define K2_CONFIG_H

//  Configurations are still in evolution, don't define K2_NO_AUTOCONF or
//  K2_USERCONF, before the config system settles.
//  Allow libk2 to perform compile-time auto-configuration.
//  If your compiler is not supported, mail to kenho@bluebottle.com.
//  I will see what I can do.
#if !defined(K2_NO_AUTOCONF)
#   include <k2/nonpublic/autoconf.h>
#endif

#if defined(K2_USERCONF)
#   include K2_USERCONF
#endif

#if defined(K2_COMPILER_GCC)
#   if #cpu(i386)
#       define  K2_CPU_I386
#       define  K2_BYTEORDER_LITTLEENDIAN
#   elif #cpu(arm)
#       define  K2_CPU_ARM
#       define  K2_BYTEORDER_BIGENDIAN  //  Is this true?
#   else
#       define  K2_CPU_UNKNOWN
#       define  K2_BYTEORDER_UNKNOWN
#   endif
#	define  K2_DLSPEC
#   if  defined(K2_OPT_BRANCH)
#       define  K2_OPT_BRANCH_TRUE(exp)     (long(__builtin_expect(long(exp), long(1))))
#       define  K2_OPT_BRANCH_FALSE(exp)    (long(__builtin_expect(long(exp), long(0))))
#   else
#       define  K2_OPT_BRANCH_TRUE(exp)     (long(exp))
#       define  K2_OPT_BRANCH_FALSE(exp)    (long(exp))
#   endif
#   if defined(_RTTI)
#       define K2_RTTI
#   endif
#   define  K2_RTTI
#elif defined(K2_COMPILER_VC) || defined(K2_COMPILER_BCB)   //  I think this would work.
#   if defined (_M_IX86) && (_M_IX86 >= 300)
#       define  K2_CPU_I386
#       define  K2_BYTEORDER_LITTLEENDIAN
#   else
#       define  K2_CPU_UNKNOWN
#       define  K2_BYTEORDER_UNKNOWN
#   endif
#   ifdef K2_BUILD
#       define  K2_DLSPEC __declspec(dllexport)
#   else
#       define  K2_DLSPEC __declspec(dllimport)
#   endif
#   define  K2_OPT_BRANCH_TRUE(exp)     (long(exp))
#   define  K2_OPT_BRANCH_FALSE(exp)    (long(exp))
#   if !defined(_MT)
#       error   "Must link against Multi-threaded DLL"
#   endif
#else
#   error   "Unknown compiler!!!"
#endif


//
//  K2_HAS_XXXX_API macros are used to assert if target supports some certain APIs.
//  K2_RT_XXXX macros are used to assert if target has the required runtime library.
//
//  Note that in some scenaios, both K2_HAS_POSIX_API and K2_HAS_WIN32_API are defined.
//  In these cases, some WIN32 APIs are actually masked by POSIX APIs, including
//  headers and functions.
//
//  Try to avoid using macros for OSes, it offten implies greater impact to portability.
//  If you can, use macros for APIs and runtime libraries instead.
//
#if defined(K2_OS_LINUX)
#   define  K2_HAS_POSIX_API
#   define  K2_RT_POSIX
#elif defined(K2_OS_MINGW32)
#   define  K2_HAS_POSIX_API
#   define  K2_HAS_WIN32_API
#   define  K2_RT_MSVCRT
#elif defined(K2_OS_CYGWIN)
#   define  K2_HAS_POSIX_API
#   define  K2_HAS_WIN32_API
#   define  K2_RT_POSIX
#elif defined(K2_OS_UNIX)
#   define  K2_HAS_POSIX_API
#   define  K2_RT_POSIX
#elif defined(K2_OS_WIN32)
#   define  K2_HAS_WIN32_API
#   define  K2_RT_MSVCRT
#else
#   error "Unknown OS!!!"
#endif

#endif  //  !K2_CONFIG_H
