/*  libk2   <k2/nonpublic/autoconf.h>

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
#ifndef K2_NONPUBLIC_AUTOCONF_H
#define K2_NONPUBLIC_AUTOCONF_H

#if defined(K2_COMPILER_VC)
#   undef   K2_COMPILER_VC
#endif
#if defined(K2_COMPILER_GCC)
#   undef   K2_COMPILER_GCC
#endif

//  Compiler
#if defined(_MSC_VER) && (_MSC_VER >= 1310)
#   define K2_COMPILER_VC
#   define K2_COMPILER_VER          (_MSC_VER / 100)
#   define K2_COMPILER_MINOR        (_MSC_VER % 100 / 10)
#elif defined(__GNUC__) && (__GNUC__ >= 3) && (__GNUC_MINOR__ >= 2)
#   define K2_COMPILER_GCC
#   define K2_COMPILER_VER          __GNUC__
#   define K2_COMPILER_MINOR        __GNUC_MINOR__
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
