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
#include <k2/runtime.h>

#include <k2/spin_lock.h>
#include <exception>

namespace
{
    bool k2_is_dl = false;
}

bool k2::runtime::is_dl ()
{
    return  k2_is_dl;
}

#if defined(WIN32)

#   include <winsock2.h>
#   include <windows.h>

    void winsock2_init ()
    {
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD( 2, 2 );

        err = WSAStartup( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            throw   std::exception("k2 fails to load a usable WinSock DLL");
        }
        if ( LOBYTE( wsaData.wVersion ) != 2 ||
                HIBYTE( wsaData.wVersion ) != 2 ) {
            WSACleanup( );
            throw   std::exception("k2 fails to load WinSock 2.2 DLL");
        }
    }

    BOOL APIENTRY DllMain( HANDLE /*hModule*/,
        DWORD  ul_reason_for_call,
        LPVOID /*lpReserved*/)
    {
        try
        {
            switch (ul_reason_for_call)
            {
            case DLL_PROCESS_ATTACH:
                k2_is_dl = true;
                winsock2_init();
                k2::runtime::init();
                break;
            case DLL_PROCESS_DETACH:
                k2::runtime::fini();
                break;
            }
        }
        catch(...)
        {
            return  FALSE;
        }
        return TRUE;
    }

#endif  //  WIN32

#if(0)
//  !kh!
//  cygwin port has not been maintained for a while,
//  might be broken
#if defined(K2_OS_CYGWIN)

#   include <windows.h>

    extern "C" BOOL APIENTRY k2_cygwin_main ( HANDLE /*hModule*/,
                                DWORD  ul_reason_for_call,
                                LPVOID * d/*lpReserved*/
                                )
    {
        switch (ul_reason_for_call)
        {
        case DLL_PROCESS_ATTACH:
            k2_is_dl = TRUE;
            k2::runtime::init();
            break;
        case DLL_PROCESS_DETACH:
            k2::runtime::fini();
            break;
        }
        return TRUE;
    }

#endif  //  K2_OS_CYGWIN
#endif

#if defined(__linux__)

    void __attribute__ ((constructor)) my_init ()
    {
        k2::runtime::init();
    }
    void __attribute__ ((destructor)) my_fini ()
    {
        k2::runtime::fini();
    }

#endif  //  __linux__

#ifndef K2_STD_H_CSTDLIB
#   include <cstdlib>
#   define  K2_STD_H_CSTDLIB
#endif

namespace
{

    struct atexit_task
    {
        void            (*routine)(void*);
        void*           arg;
        int             prio;
        atexit_task*    pnext;

        atexit_task (void (*routine)(void*), void* arg, int prio)
        :   routine(routine)
        ,   arg(arg)
        ,   prio(prio)
        ,   pnext(0)
        {
        }

        static void exec ()
        {
        }

    };

    struct atexit_stack
    {
        static void push (void (*routine)(void*), void* arg, int prio)
        {
            atexit_task*    ptask = new atexit_task(routine, arg, prio);

            if (pstack == 0 || pstack && prio >= pstack->prio)
            {
                ptask->pnext = pstack;
                pstack = ptask;
                return;
            }

            if (pstack->pnext)
            {
                atexit_task*    p = pstack;
                for (; p->pnext; p = p->pnext)
                {
                    if (prio >= p->pnext->prio)
                        break;
                }
                ptask->pnext = p->pnext;
                p->pnext = ptask;
            }
            else
        {
                pstack->pnext = ptask;
            }
        }
        static void exec ()
        {
            while (pstack)
            {
                atexit_task* pnext = pstack->pnext;
                pstack->routine(pstack->arg);
                delete pstack;
                pstack = pnext;
            }
        }
        static atexit_task*     pstack;
        static k2::spin_lock    lock;
        static bool             exec_registered;
    };

    //  static
    atexit_task*    atexit_stack::pstack = 0;
    //  static
    k2::spin_lock   atexit_stack::lock;
    //  static
    bool            atexit_stack::exec_registered = false;

}   //  namespace

//static
void
k2::runtime::atexit_impl (void (*routine)(void*), void* arg, prio_t prio)
{
    spin_lock::scoped_guard guard(atexit_stack::lock);

    atexit_stack::push(routine, arg, prio);

    if (atexit_stack::exec_registered == false && runtime::is_dl() == false)
        std::atexit(atexit_stack::exec);
}

//static
void
k2::runtime::init ()
{
}

//static
void
k2::runtime::fini ()
{
    atexit_stack::exec();
}
