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
#include <k2/atomic.h>

#if defined(K2_ATOMIC_USE_GLIBC)
#   include <bits/atomicity.h>
#elif defined(K2_ATOMIC_USE_WIN32)
#   include <windows.h>
#elif defined(K2_ATOMIC_USE_PTHREADS)
#   include <pthread.h>
namespace
{
    pthread_mutex_t local_mtx = PTHREADS_MUTEX_INITIALIZER;
}
#endif


    bool k2::atomic_increase (k2::atomic_int_t& value) throw ()
#if defined(K2_ATOMIC_USE_GLIBC)
    {
        __exchange_and_add(&value, 1);
        return  __exchange_and_add(&value, 0);
    }
#elif defined(K2_ATOMIC_USE_WIN32)
    {
        return  ::InterlockedIncrement(
            reinterpret_cast<volatile long*>(&value)) == 0 ? false : true;
    }
#elif defined(K2_ATOMIC_USE_PTHREADS)
    {
        bool ret;
        pthread_mutex_lock(&local_mtx);
        ret = ++value;
        pthread_mutex_unlock(&local_mtx);
        return  ret;
    }
#endif

    bool k2::atomic_decrease (k2::atomic_int_t& value) throw ()
#if defined(K2_ATOMIC_USE_GLIBC)
    {
        __exchange_and_add(&value, -1);
        return  __exchange_and_add(&value, 0);
    }
#elif defined(K2_ATOMIC_USE_WIN32)
    {
        return  ::InterlockedDecrement(
            reinterpret_cast<volatile long*>(&value)) == 0 ? false : true;
    }
#elif defined(K2_ATOMIC_USE_PTHREADS)
    {
        bool ret;
        pthread_mutex_lock(&local_mtx);
        ret = --value;
        pthread_mutex_unlock(&local_mtx);
        return  ret;
    }
#endif

