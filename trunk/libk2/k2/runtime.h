/*	libk2	<k2/runtime.h>

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
#ifndef	K2_RUNTIME_H
#define	K2_RUNTIME_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef K2_STD_H_CLIMITS
#	include <climits>
#	define	K2_STD_H_CLIMITS
#endif
#ifndef K2_STD_H_MEMROY
#	include <memory>
#	define	K2_STD_H_MEMORY
#endif

namespace k2
{

	class runtime
	{
	public:
		K2_DLSPEC static void init ();
		K2_DLSPEC static void fini ();

		K2_DLSPEC static bool is_dl ();

		typedef	int atexit_prio_t;
		static const atexit_prio_t atexit_prio_highest		= INT_MAX;
		static const atexit_prio_t atexit_prio_lowest		= INT_MIN + 1;
		static const atexit_prio_t atexit_prio_default		= 0;

		template <typename atexit_func_>
		static void atexit (atexit_func_ func, atexit_prio_t pri)
		{
			atexit_func_* arg = new atexit_func_(func);
			void(*routine)(void*) = runtime::atexit_adapter<atexit_func_>;
			runtime::atexit_impl(routine, (void*)arg, pri);
		}

	private:
		K2_DLSPEC static void atexit_impl (void (*routine)(void*), void* arg, int pri);
		template <typename atexit_func_>
		static void atexit_adapter (void* arg)
		{
			std::auto_ptr<atexit_func_> pcleanup(reinterpret_cast<atexit_func_*>(arg));
			(*pcleanup)();
		}
	};

}	//	namespace k2

#endif	//	!K2_RUNTIME_H
