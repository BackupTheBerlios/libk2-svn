/*	libk2	<runtime.cpp>

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
#include <k2/runtime.h>

#ifndef	K2_SPIN_LOCK_H
#	include	<k2/spin_lock.h>
#endif
#ifndef	K2_STD_H_EXCEPTION
#	include <exception>
#	define	K2_STD_H_EXCEPTION
#endif

namespace
{
	bool k2_is_dl = false;

}
namespace k2
{

	bool runtime::is_dl ()
	{
		return	k2_is_dl;
	}

}	//	namespace k2

#if defined(K2_OS_WIN32)

#	include <winsock2.h>
#	include <windows.h>

	BOOL k2_winsock2_init ()
	{
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		wVersionRequested = MAKEWORD( 2, 2 );

		err = WSAStartup( wVersionRequested, &wsaData );
		if ( err != 0 ) {
			// Tell the user that we could not find a usable
			// WinSock DLL.
			return	FALSE;
		}
		if ( LOBYTE( wsaData.wVersion ) != 2 ||
				HIBYTE( wsaData.wVersion ) != 2 ) {
			// Tell the user that we could not find a usable
			// WinSock DLL.
			WSACleanup( );
			return	FALSE;
		}
		return	TRUE;
	}

	BOOL k2_win32_init ()
	{
		if (k2_winsock2_init() == TRUE)
		{
			try
			{
				k2::runtime::init();
			}
			catch (std::exception&)
			{
				return	FALSE;
			}
			return	TRUE;
		}
		return	FALSE;
	}
	void k2_win32_fini ()
	{
		k2::runtime::fini();
	}

	BOOL APIENTRY DllMain( HANDLE /*hModule*/,
						DWORD  ul_reason_for_call,
						LPVOID /*lpReserved*/
						)
	{
		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
			k2_is_dl = true;
			return	k2_win32_init();
		case DLL_PROCESS_DETACH:
			k2_win32_fini();
		}
		return TRUE;
	}

#endif	//	K2_OS_WIN32

#if defined(K2_OS_CYGWIN)

#	include <windows.h>

    extern "C" BOOL APIENTRY k2_cygwin_main ( HANDLE /*hModule*/,
        						DWORD  ul_reason_for_call,
        						LPVOID * d/*lpReserved*/
        						)
	{
		switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
			k2_is_dl = true;
			k2::runtime::init();
			return  true;
		case DLL_PROCESS_DETACH:
			k2::runtime::fini();
		}
		return TRUE;
	}

#endif	//	K2_OS_CYGWIN

#if defined(K2_OS_LINUX)

	void __attribute__ ((constructor)) my_init ()
	{
		k2::runtime::init();
	}
	void __attribute__ ((destructor)) my_fini ()
	{
		k2::runtime::fini();
	}

#endif	//	K2_OS_LINUX

#ifndef	K2_STD_H_CSTDLIB
#	include <cstdlib>
#	define	K2_STD_H_CSTDLIB
#endif

namespace k2
{

	struct atexit_task
	{
		void			(*routine)(void*);
		void*			arg;
		int				prio;
		atexit_task*	pnext;

		atexit_task (void (*routine)(void*), void* arg, int prio)
		:	routine(routine)
		,	arg(arg)
		,	prio(prio)
		,	pnext(0)
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
			atexit_task*	ptask = new atexit_task(routine, arg, prio);

			if (pstack == 0 || pstack && prio >= pstack->prio)
			{
				ptask->pnext = pstack;
				pstack = ptask;
				return;
			}

			if (pstack->pnext)
			{
				atexit_task*	p = pstack;
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
		static atexit_task*	pstack;
		static spin_lock	lock;
		static bool			exec_registered;
	};
	//static
	atexit_task*	atexit_stack::pstack = 0;
	//static
	spin_lock		atexit_stack::lock;
	//static
	bool			atexit_stack::exec_registered = false;

	//static
	void runtime::atexit_impl (void (*routine)(void*), void* arg, atexit_prio_t prio)
	{
		spin_lock::scoped_guard	guard(atexit_stack::lock);

		atexit_stack::push(routine, arg, prio);

		if (atexit_stack::exec_registered == false && runtime::is_dl() == false)
			std::atexit(atexit_stack::exec);
	}

	//static
	void runtime::init ()
	{
	}

	//static
	void runtime::fini ()
	{
		atexit_stack::exec();
	}


}	//	namespace k2

