/*	libk2	<k2/config.h>

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
#ifndef K2_CONFIG_H
#define K2_CONFIG_H

//  Configurations are still in evolution, don't define K2_CONF_FILE, yet!!!
//  Allow libk2 to perform compile-time auto-configuration.
#if !defined(K2_CONF_FILE)
#   define  K2_CONF_FILE    <k2/nonpublic/autoconf.h>
#endif

#include K2_CONF_FILE

#if defined(K2_COMPILER_GCC)
#	if #cpu(i386)
#       define  K2_CPU_I386
#       define  K2_BYTEORDER_LITTLEENDIAN
#   elif #cpu(arm)
#       define  K2_CPU_ARM
#       define  K2_BYTEORDER_BIGENDIAN  //  Is this true?
#   else
#       define  K2_CPU_UNKNOWN
#       define  K2_BYTEORDER_UNKNOWN
#   endif
#define  K2_DLSPEC
#   if  defined(K2_OPT_BRANCH)
#       define  K2_OPT_BRANCH_TRUE(exp)		(long(__builtin_expect(long(exp), long(1))))
#       define  K2_OPT_BRANCH_FALSE(exp)	(long(__builtin_expect(long(exp), long(0))))
#   else
#       define  K2_OPT_BRANCH_TRUE(exp)		(long(exp))
#       define  K2_OPT_BRANCH_FALSE(exp)	(long(exp))
#   endif
#   if defined(_RTTI)
#	    define K2_RTTI
#   endif
#	define  K2_RTTI
#elif defined(K2_COMPILER_VC)
#	if defined (_M_IX86) && (_M_IX86 >= 300)
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
#   define  K2_OPT_BRANCH_TRUE(exp)		(long(exp))
#   define  K2_OPT_BRANCH_FALSE(exp)	(long(exp))
#   if !defined(_MT)
#       error   "Must link against Multi-threaded DLL"
#   endif
#else
#   error   "Unknown compiler!!!"
#endif

#if defined(K2_OS_LINUX)
#   define  K2_HAS_POSIX_API
#   define  K2_RT_GLIBC
#elif defined(K2_OS_MINGW32)
#   define  K2_HAS_POSIX_API
#   define  K2_HAS_WIN32_API
#   define  K2_RT_MSVCRT
#elif defined(K2_OS_CYGWIN)
#   define  K2_HAS_POSIX_API
#   define  K2_HAS_WIN32_API
#   define  K2_RT_GLIBC
#elif defined(K2_OS_UNIX)
#   define  K2_HAS_POSIX_API
#   define  K2_RT_GLIBC
#elif defined(K2_OS_WIN32)
#   define  K2_HAS_WIN32_API
#   define  K2_RT_MSVCRT
#else
#	error "Unknown OS!!!"
#endif

#endif  //  !K2_CONFIG_H
