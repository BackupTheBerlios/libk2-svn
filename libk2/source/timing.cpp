/*	libk2	<timing.cpp>

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
#include <k2/timing.h>

#ifndef	K2_SPIN_LOCK_H
#	include <k2/spin_lock.h>
#endif
#ifndef	K2_SINGLETON_H
#	include <k2/singleton.h>
#endif

#if defined(K2_OS_WIN32)
#	include <sys/timeb.h>
#elif defined(K2_HAS_POSIX_API)
#   include <sys/time.h>
#endif

#include <time.h>

namespace k2
{

	namespace	//	unnamed
	{
		static const uint64_t	one_thousand	= 1000;

		inline uint64_t os_timestamp ()
#if defined(K2_OS_WIN32)
		{
			_timeb  tb;
			_ftime( &tb );

			return	(uint64_t(tb.time) * one_thousand + tb.millitm );
		}
#elif defined(K2_HAS_POSIX_API)
		{
			timeval tv;
			gettimeofday(&tv, NULL);

			return	(uint64_t(tv.tv_sec) * one_thousand +
				tv.tv_usec / one_thousand);
		}
#endif

	}	//	unnamed namespace

	//static
	timestamp::now_tag	timestamp::now;

	timestamp::timestamp ()
	:	m_msec(os_timestamp())
	{
	}
	timestamp::timestamp (const timestamp& rhs)
	:	m_msec(rhs.m_msec)
	{
	}
	timestamp::timestamp (const time_span& span_to_increse_from_now)
	:	m_msec(os_timestamp())
	{
		*this += span_to_increse_from_now;
	}
	timestamp::timestamp (uint64_t msec)
	:	m_msec(msec)
	{
	}
	timestamp& timestamp::operator= (const timestamp& rhs)
	{
		m_msec = rhs.m_msec;
		return	*this;
	}
	uint64_t timestamp::in_msec () const
	{
		return	m_msec;
	}
	uint32_t timestamp::in_sec () const
	{
		return	uint32_t(m_msec / one_thousand);
	}
	uint16_t timestamp::msec_of_sec () const
	{
		return	uint16_t(m_msec % one_thousand);
	}
	bool timestamp::expired () const
	{
		return	*this <= timestamp::now;
	}
	timestamp& timestamp::operator+= (const time_span& span_to_increase)
	{
		m_msec += span_to_increase.in_msec();
		return	*this;
	}
	timestamp& timestamp::operator-= (const time_span& span_to_decrease)
	{
		m_msec -= span_to_decrease.in_msec();
		return	*this;
	}
	const time_span timestamp::operator-= (const timestamp& rhs)
	{
	    if (m_msec > rhs.in_msec())
	        return  time_span(m_msec - rhs.in_msec());
	    else
	        return  time_span(0);
	}
	const time_span timestamp::operator-= (timestamp::now_tag)
	{
	    uint64_t    now = os_timestamp();
	    if (m_msec > now)
	        return  time_span(m_msec - now);
	    else
	        return  time_span(0);
	}
	uint32_t timestamp::hash () const
	{
		//	Ideally, magic_num should be a timestamp close to present time.
		//	Perhaps year 2000?
		static const uint64_t	magic_num = 0;
		uint64_t	to_hash = m_msec - magic_num;
		uint32_t*	pint32 = reinterpret_cast<uint32_t*>(&to_hash);
		return
#if defined(K2_BYTEORDER_BIGENDIAN)
		pint32[1];
#else
		pint32[0];
#endif
	}
	timestamp operator+ (const timestamp& val, const time_span& span_to_increase)
	{
		timestamp res(val);
		res += span_to_increase;
		return	res;
	}
	timestamp operator- (const timestamp& val, const time_span& span_to_decrease)
	{
		timestamp res(val);
		res -= span_to_decrease;
		return	res;
	}
	time_span operator- (const timestamp& lhs, const timestamp& rhs)
	{
		timestamp	tmp(lhs);
		return	(tmp -= rhs);
	}
	time_span operator- (const timestamp& lhs, timestamp::now_tag)
	{
		timestamp	tmp(lhs);
		return	(tmp -= timestamp::now);
	}
	bool operator== (const timestamp& lhs, const timestamp& rhs)
	{
		return	lhs.in_msec() == rhs.in_msec();
	}
	bool operator!= (const timestamp& lhs, const timestamp& rhs)
	{
		return	lhs.in_msec() != rhs.in_msec();
	}
	bool operator< (const timestamp& lhs, const timestamp& rhs)
	{
		return	lhs.in_msec() < rhs.in_msec();
	}
	bool operator<= (const timestamp& lhs, const timestamp& rhs)
	{
		return	lhs.in_msec() <= rhs.in_msec();
	}
	bool operator> (const timestamp& lhs, const timestamp& rhs)
	{
		return	lhs.in_msec() > rhs.in_msec();
	}
	bool operator>= (const timestamp& lhs, const timestamp& rhs)
	{
		return	lhs.in_msec() >= rhs.in_msec();
	}
	bool operator== (const timestamp& lhs, timestamp::now_tag)
	{
		return	lhs.in_msec() == os_timestamp();
	}
	bool operator!= (const timestamp& lhs, timestamp::now_tag)
	{
		return	lhs.in_msec() != os_timestamp();
	}
	bool operator< (const timestamp& lhs, timestamp::now_tag)
	{
		return	lhs.in_msec() < os_timestamp();
	}
	bool operator<= (const timestamp& lhs, timestamp::now_tag)
	{
		return	lhs.in_msec() <= os_timestamp();
	}
	bool operator> (const timestamp& lhs, timestamp::now_tag)
	{
		return	lhs.in_msec() > os_timestamp();
	}
	bool operator>= (const timestamp& lhs, timestamp::now_tag)
	{
		return	lhs.in_msec() >= os_timestamp();
	}







	time_span::time_span ()
	:	m_msec(0)
	{
	}
	time_span::time_span (const time_span& rhs)
	:	m_msec(rhs.m_msec)
	{
	}
	time_span::time_span (uint64_t msec)
	:	m_msec(msec)
	{
	}
	time_span& time_span::operator= (const time_span& rhs)
	{
		m_msec = rhs.m_msec;
		return	*this;
	}
	uint64_t time_span::in_msec () const
	{
		return	m_msec;
	}
	uint32_t time_span::in_sec () const
	{
		return	uint32_t(m_msec / one_thousand);
	}
	uint16_t time_span::msec_of_sec () const
	{
		return	uint16_t(m_msec % one_thousand);
	}
	time_span& time_span::operator+= (const time_span& span_to_increase)
	{
		m_msec += span_to_increase.m_msec;
		return	*this;
	}
	time_span& time_span::operator-= (const time_span& span_to_decrease)
	{
		m_msec -= span_to_decrease.m_msec;
		return	*this;
	}
	time_span& time_span::operator*= (uint_t mul)
	{
		m_msec *= mul;
		return	*this;
	}
	time_span& time_span::operator/= (uint_t div)
	{
		m_msec /= div;
		return	*this;
	}
	time_span operator+ (const time_span& val, const time_span& span_to_increase)
	{
		time_span res(val);
		res += span_to_increase;
		return	res;
	}
	time_span operator- (const time_span& val, const time_span& span_to_decrease)
	{
		time_span res(val);
		res -= span_to_decrease;
		return	res;
	}
	time_span operator* (const time_span& val, uint_t mul)
	{
		time_span res(val);
		res *= mul;
		return	res;
	}
	time_span operator/ (const time_span& val, uint_t div)
	{
		time_span res(val);
		res /= div;
		return	res;
	}
	bool operator== (const time_span& lhs, const time_span& rhs)
	{
		return	lhs.in_msec() == rhs.in_msec();
	}
	bool operator!= (const time_span& lhs, const time_span& rhs)
	{
		return	lhs.in_msec() != rhs.in_msec();
	}
	bool operator< (const time_span& lhs, const time_span& rhs)
	{
		return	lhs.in_msec() < rhs.in_msec();
	}
	bool operator<= (const time_span& lhs, const time_span& rhs)
	{
		return	lhs.in_msec() <= rhs.in_msec();
	}
	bool operator> (const time_span& lhs, const time_span& rhs)
	{
		return	lhs.in_msec() > rhs.in_msec();
	}
	bool operator>= (const time_span& lhs, const time_span& rhs)
	{
		return	lhs.in_msec() >= rhs.in_msec();
	}









	struct calendar_util
	{
		//	change to spin_lock
		typedef	spin_lock									lock_type;
		typedef	class_singleton<calendar_util, spin_lock>	the_lock;
		typedef	scoped_guard<spin_lock>						scoped_guard;
	};

	template </*time_zone_constant*/>
	void  calendar<time_zone::utc>::validate () const
	{
		calendar_util::lock_type&	the_lock = calendar_util::the_lock::instance();
		typedef	calendar_util::scoped_guard	scoped_guard;

		if (m_validated == false)
		{
			time_t	t = m_timestamp.in_sec();

			{
				scoped_guard	guard(the_lock);

				tm*	ptm = gmtime(&t);

				m_year			= ptm->tm_year + 1970;
				m_month_of_year	= ptm->tm_mon;
				m_day_of_month	= ptm->tm_mday - 1;
				m_hour_of_day	= ptm->tm_hour;
				m_min_of_hour	= ptm->tm_min;
				m_sec_of_min	= ptm->tm_sec;
				m_day_of_year	= ptm->tm_yday;
				m_day_of_week	= ptm->tm_wday;
			}
			m_validated		= true;
			m_msec_of_sec	= m_timestamp.msec_of_sec();
		}
	}

	template </*time_zone_constant*/>
	void calendar<time_zone::local>::validate () const
	{
		calendar_util::lock_type&	the_lock = calendar_util::the_lock::instance();
		typedef	calendar_util::scoped_guard	scoped_guard;

		if (m_validated == false)
		{
			time_t	t = m_timestamp.in_sec();

			{
				scoped_guard	guard(the_lock);

				tm*		ptm = localtime(&t);

				m_year			= ptm->tm_year + 1970;
				m_month_of_year	= ptm->tm_mon;
				m_day_of_month	= ptm->tm_mday - 1;
				m_hour_of_day	= ptm->tm_hour;
				m_min_of_hour	= ptm->tm_min;
				m_sec_of_min	= ptm->tm_sec;
				m_day_of_year	= ptm->tm_yday;
				m_day_of_week	= ptm->tm_wday;
			}
			m_validated		= true;
			m_msec_of_sec	= m_timestamp.msec_of_sec();
		}
	}

}	//	namespace k2
