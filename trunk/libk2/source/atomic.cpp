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

#if defined(K2_ATOMIC_USE_GLIBC)
#   include <bits/atomicity.h>
#elif defined(K2_ATOMIC_USE_WIN32_API)
#   include <windows.h>
#elif defined(K2_ATOMIC_USE_PTHREADS)
#   include <pthread.h>
namespace
{
    pthread_mutex_t local_mtx = PTHREADS_MUTEX_INITIALIZER;
}
#endif


namespace k2
{

    bool atomic_increase (atomic_int_t& value) throw ()
#if defined(K2_ATOMIC_USE_GLIBC)
    {
        __exchange_and_add(&value, 1);
        return  __exchange_and_add(&value, 0);
    }
#elif defined(K2_ATOMIC_USE_WIN32_API)
    {
        return  ::InterlockedIncrement(&value) == 0 ? false : true;
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

    bool atomic_decrease (atomic_int_t& value) throw ()
#if defined(K2_ATOMIC_USE_GLIBC)
    {
        __exchange_and_add(&value, -1);
        return  __exchange_and_add(&value, 0);
    }
#elif defined(K2_ATOMIC_USE_WIN32_API)
    {
        return  ::InterlockedDecrement(&value) == 0 ? false : true;
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

}   //  namespace k2
