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
#ifndef K2_NONPUBLIC_AUTOCONF_H
#define K2_NONPUBLIC_AUTOCONF_H

#if defined(K2_COMPILER_VC)
#   undef   K2_COMPILER_VC
#endif
#if defined(K2_COMPILER_GCC)
#   undef   K2_COMPILER_GCC
#endif

//  Compiler
#if defined(_MSC_VER) && (_MSC_VER >= 1310) //  VC.NET 2003
#   define K2_COMPILER_VC
#   define K2_COMPILER_VER      (_MSC_VER / 100)
#   define K2_COMPILER_MINOR    (_MSC_VER % 100 / 10)
#elif defined(__GNUC__) && (__GNUC__ >= 3) && (__GNUC_MINOR__ >= 2)
#   define K2_COMPILER_GCC
#   define K2_COMPILER_VER      __GNUC__
#   define K2_COMPILER_MINOR    __GNUC_MINOR__
#elif defined(__BCPLUSPLUS__) && (__BCPLUSPLUS__ >= 0x0560) // BCB6
#   define K2_COMPILER_BCB
#   define K2_COMPILER_VER      (__BCPLUSPLUS__ / 100)
#   define K2_COMPILER_MINOR    (__BCPLUSPLUS__ % 100 / 10)
#   define _WIN32
#else
#   error "Unknown compiler!!!"
#endif

//  OS
#if defined(K2_OS_LINUX)
#   undef   K2_OS_LINUX
#endif
#if defined(K2_HAS_POSIX_API)
#   undef   K2_HAS_POSIX_API
#endif
#if defined(K2_OS_MINGW32)
#   undef   K2_OS_MINGW32
#endif
#if defined(K2_OS_CYGWIN)
#   undef   K2_OS_CYGWIN
#endif
#if defined(K2_OS_UNIX)
#   undef   K2_OS_UNIX
#endif
#if defined(K2_HAS_WIN32_API)
#   undef   K2_HAS_WIN32_API
#endif

#if defined(__linux__)
#   define  K2_OS_LINUX
#   define  K2_HAS_POSIX_API
#elif defined(__MINGW32__)
#   define  K2_OS_MINGW32
#   define  K2_HAS_POSIX_API
#   define  K2_HAS_WIN32_API
#elif defined(__CYGWIN__)
#   define  K2_OS_CYGWIN
#   define  K2_HAS_POSIX_API
#   define  K2_HAS_WIN32_API
#elif defined(__unix__)
#   define  K2_OS_UNIX
#   define  K2_HAS_POSIX_API
#elif defined(_WIN32)
#   define  K2_OS_WIN32
#   define  K2_HAS_WIN32_API
#else
#   error "Unknown OS!!!"
#endif

#endif  //  !K2_NONPUBLIC_AUTOCONF_H
