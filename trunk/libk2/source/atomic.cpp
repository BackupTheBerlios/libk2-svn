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
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <k2/bits/atomic.h>

#if !defined(__GUNC__)

#   if !defined(WIN32)
#       include <pthread.h>
    namespace
    {
        pthread_mutex_t local_mtx = PTHREADS_MUTEX_INITIALIZER;
    }


    bool k2::atomic_increase (k2::atomic_int_t& value) throw ()
    {
        bool ret;
        pthread_mutex_lock(&local_mtx);
        ret = ++value;
        pthread_mutex_unlock(&local_mtx);
        return  ret;
    }

    bool k2::atomic_decrease (k2::atomic_int_t& value) throw ()
    {
        bool ret;
        pthread_mutex_lock(&local_mtx);
        ret = --value;
        pthread_mutex_unlock(&local_mtx);
        return  ret;
    }

#   else
#       include <windows.h>

    bool k2::atomic_increase (k2::atomic_int_t& value) throw ()
    {
        return  ::InterlockedIncrement(
            reinterpret_cast<volatile long*>(&value)) == 0 ? false : true;
    }
    bool k2::atomic_decrease (k2::atomic_int_t& value) throw ()
    {
        return  ::InterlockedDecrement(
            reinterpret_cast<volatile long*>(&value)) == 0 ? false : true;
    }
#   endif

#endif
