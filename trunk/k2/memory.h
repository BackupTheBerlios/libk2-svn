/*	libk2	<k2/memory.h>

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
#ifndef	K2_MEMORY_H
#define	K2_MEMORY_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_STD_H_MEMORY
#	include <memory>
#	define	K2_STD_H_MEMORY
#endif

namespace k2
{

	template <typename from_iter_t_, typename to_iter_t_>
	void uninit_copy (from_iter_t_ first, from_iter_t_ last, to_iter_t_ to)
	{
		std::uninitialized_copy(first, last, to);
	}
	template <typename iter_t_, typename value_t_>
	void uninit_fill (iter_t_ first, iter_t_ last, const value_t_& v)
	{
		std::uninitialized_fill(first, last, v);
	}
	template <typename iter_t_, typename value_t_>
	void uninit_fill_n (iter_t_ first, size_t n, const value_t_& v)
	{
		std::uninitialized_fill_n(first, last, v);
	}

	template <typename type_>
	struct destroyer
	{
		explicit destroyer (type_* p)
		:	m_p(p) {}

		void operator() () const
		{
			delete	m_p;
		}
		type_*	m_p;
	};

}	//	namespace k2

#endif	//	!K2_MEMORY_H
