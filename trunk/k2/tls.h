/*	libk2	<k2/tls.h>

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
#ifndef K2_TLS_H
#define K2_TLS_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_BOUNCER_H
#	include <k2/bouncer.h>
#endif

namespace k2
{

#if !defined(DOXYGEN_BLIND)
	namespace nonpublic
	{
		class tls_ptr_impl
		{
		public:
			K2_BOUNCE_DEFAULT_COPY();

			K2_DLSPEC tls_ptr_impl (void (*destroy_routine)(void*));
			K2_DLSPEC ~tls_ptr_impl () ;

			K2_DLSPEC void*	get () const ;
			K2_DLSPEC void*	release () ;
			K2_DLSPEC void	reset(void* p=0) ;

			struct handle
			{
				char	holder[sizeof(long)];
			};

		private:
			void (*m_destroy_routine)(void*);
			handle	m_handle;
		};
	}
#endif  //	!DOXYGEN_BLIND

	/**	\defgroup	Threading
	*/

	/**
	*	\ingroup	Threading
	*	\brief	Management of thread local stroage (a.k.a. thread-specific data).
	*
	*	Stores a pointer to an object obtained via new on a thread-by-thread basis and
	*	calls delete on the contained pointer when the thread terminates.
	*
	*	Each thread initially stores the null pointer in a tls_ptr<> object.
	*
	*	Note: Do not destroy any data that may be stored in any thread's thread-specific data.
	*	For this reason you should not destroy a tls_ptr<> object until you are certain
	*	there are no threads running that have made use of its thread-specific data.
	*/
	template <typename type_>
	class tls_ptr
	{
	public:
		K2_BOUNCE_DEFAULT_COPY();

		typedef	type_	value_type;
		/**
		*	Allocates resource for *this.
		*
		*	/throw		bad_resource_alloc
		*/
		explicit tls_ptr ()
		:	m_impl(cleanup)
		{
		}
		/**
		*	Retrieves the stored pointer.
		*/
		type_*		get () const
		{
			return	reinterpret_cast<type_*>(m_impl.get());
		}
		/**
		*	Returns this->get().
		*/
		type_*		operator-> () const
		{
			return	reinterpret_cast<type_*>(m_impl.get());
		}
		/**
		*	Returns *(this->get()).
		*/
		type_&		operator* () const
		{
			return	this->get();
		}
		/**
		*	Releases and returns ownership of stored pointer.
		*/
		type_*		release ()
		{
			return	reinterpret_cast<type_*>(this->release());
		}
		/**
		*	Deletes stored pointer then take ownership of p.
		*/
		void	reset (type_* p = 0)
		{
			return	m_impl.reset(p);
		}

	private:
		static void cleanup (void* p)
		{
			type_*	pt = reinterpret_cast<type_*>(p);
			delete	pt;
		}
		nonpublic::tls_ptr_impl	m_impl;
	};	//	class tls

}   //  namespace k2

#endif  //  !K2_TLS_H
