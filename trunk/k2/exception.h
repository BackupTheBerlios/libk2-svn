/*	libk2	<k2/exception.h>

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
#ifndef	K2_EXCEPTION_H
#define	K2_EXCEPTION_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif

#ifndef	K2_STD_H_EXCEPTION
#	include <exception>
#	define	K2_STD_H_EXCEPTION
#endif
#ifndef	K2_STD_H_CSTRING
#	include <cstring>
#	define	K2_STD_H_CSTRING
#endif

namespace k2
{

	/**	\defgroup	Exception
	*/

	/**
	*	\ingroup	Exception
	*	\brief	Base exception class.
	*
	*	Base exception class, that guarantee no-throw.
	*/
	struct exception : std::exception
	{
		static const size_t max_what = 255;

		explicit exception (const char* what = "exception") throw ()
		{
			std::strncpy(m_what, what, sizeof(what));
			m_what[sizeof(what)] = 0;
		}

		virtual const char* what () const throw ()
		{
			return	m_what;
		}

	private:
		char	m_what[max_what + 1];
	};

	/**
	*	\ingroup	Exception
	*	\brief Timed-out exception class.
	*/
	struct timedout_error : exception
	{
		explicit timedout_error (const char* what = "timedout_error") throw ()
			:	exception(what) {};
	};

	/**
	*	\ingroup	Exception
	*	\brief No resource exception class.
	*/
	struct bad_resource_alloc : exception
	{
		explicit bad_resource_alloc (const char* what = "bad_resource_alloc") throw ()
			:	exception(what) {};
	};

	/**
	*	\ingroup	Exception
	*	\brief No resource exception class.
	*/
	struct critical_error : exception
	{
		explicit critical_error (const char* what = "critical_error") throw ()
			:	exception(what) {};
	};


	/**	\defgroup	Threading
	*/

	/**
	*	\ingroup	Exception
	*	\ingroup	Threading
	*	\brief Exception class for threading classes.
	*/
	struct thread_error : exception
	{
		explicit thread_error (const char* what = "thread_error")
			:	exception(what) {};
	};

	/**
	*	\ingroup	Exception
	*	\brief General runtime error exception class.
	*/
	struct runtime_error : exception
	{
		explicit runtime_error (const char* what = "runtime_error") throw ()
			:	exception(what) {};
	};

}	//	namespace k2

#endif	//	!K2_EXCEPTION_H
