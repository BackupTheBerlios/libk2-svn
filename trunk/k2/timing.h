/*	libk2	<k2/timing.h>

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
#ifndef K2_TIMING_H
#define K2_TIMING_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif
#ifndef	K2_STDINT_H
#	include <k2/stdint.h>
#endif
#ifndef	K2_TYPE_MANIP_H
#	include <k2/type_manip.h>
#endif

namespace k2
{

	class time_span;

	/**	\defgroup	Timing
	*/

	/**
	*	\ingroup	Timing
	*	\brief		Representation of point in time.
	*
	*	Class whose objects represent points in time.
	*	\relates	time_span
	*/
	class timestamp
	{
	public:
		typedef	integer_tag<int, 0>		now_tag;
		K2_DLSPEC static now_tag	now;

		K2_DLSPEC timestamp ();
		K2_DLSPEC timestamp (const timestamp&);
		K2_DLSPEC explicit timestamp (
			const time_span& span_to_increse_from_now);
		K2_DLSPEC explicit timestamp (uint64_t msec);
		K2_DLSPEC timestamp& operator= (const timestamp& rhs);

		K2_DLSPEC uint64_t		in_msec () const;
		K2_DLSPEC uint32_t		in_sec () const;
		K2_DLSPEC uint16_t		msec_of_sec () const;
		K2_DLSPEC bool 			expired () const;

		K2_DLSPEC timestamp& operator+= (const time_span& span_to_increase);
		K2_DLSPEC timestamp& operator-= (const time_span& span_to_decrease);
		K2_DLSPEC const time_span operator-= (const timestamp& rhs);
		K2_DLSPEC const time_span operator-= (now_tag);
		K2_DLSPEC uint32_t	hash () const;

	private:
		uint64_t	m_msec;
	};

	K2_DLSPEC timestamp operator+ (const timestamp& val, const time_span& span_to_increase);
	K2_DLSPEC timestamp operator- (const timestamp& val, const time_span& span_to_decrease);
	K2_DLSPEC time_span operator- (const timestamp& lhs, const timestamp& rhs);
	K2_DLSPEC time_span operator- (const timestamp& lhs, timestamp::now_tag);

	K2_DLSPEC bool operator== (const timestamp& lhs, const timestamp& rhs);
	K2_DLSPEC bool operator!= (const timestamp& lhs, const timestamp& rhs);
	K2_DLSPEC bool operator< (const timestamp& lhs, const timestamp& rhs);
	K2_DLSPEC bool operator<= (const timestamp& lhs, const timestamp& rhs);
	K2_DLSPEC bool operator> (const timestamp& lhs, const timestamp& rhs);
	K2_DLSPEC bool operator>= (const timestamp& lhs, const timestamp& rhs);

	K2_DLSPEC bool operator== (const timestamp& lhs, timestamp::now_tag);
	K2_DLSPEC bool operator!= (const timestamp& lhs, timestamp::now_tag);
	K2_DLSPEC bool operator< (const timestamp& lhs, timestamp::now_tag);
	K2_DLSPEC bool operator<= (const timestamp& lhs, timestamp::now_tag);
	K2_DLSPEC bool operator> (const timestamp& lhs, timestamp::now_tag);
	K2_DLSPEC bool operator>= (const timestamp& lhs, timestamp::now_tag);



	/**
	*	\ingroup	Timing
	*	\brief		Representation of span in time.
	*
	*	Class whose objects represent (positive) spans in time.
	*	\relates	timestamp
	*/
	class time_span
	{
	public:
		K2_DLSPEC time_span ();
		K2_DLSPEC time_span (const time_span& rhs);
		K2_DLSPEC explicit time_span (uint64_t msec);
		K2_DLSPEC time_span& operator= (const time_span& rhs);

		K2_DLSPEC uint64_t		in_msec () const;
		K2_DLSPEC uint32_t		in_sec () const;
		K2_DLSPEC uint16_t		msec_of_sec () const;

		K2_DLSPEC time_span& operator+= (const time_span& rhs);
		K2_DLSPEC time_span& operator-= (const time_span& rhs);
		K2_DLSPEC time_span& operator*= (uint_t mul);
		K2_DLSPEC time_span& operator/= (uint_t div);

	private:
		uint64_t	m_msec;
	};

	K2_DLSPEC time_span operator+ (const time_span& val, const time_span& diff);
	K2_DLSPEC time_span operator- (const time_span& val, const time_span& diff);
	K2_DLSPEC time_span operator* (const time_span& val, uint_t mul);
	K2_DLSPEC time_span operator/ (const time_span& val, uint_t div);

	K2_DLSPEC bool operator== (const time_span& lhs, const time_span& rhs);
	K2_DLSPEC bool operator!= (const time_span& lhs, const time_span& rhs);
	K2_DLSPEC bool operator< (const time_span& lhs, const time_span& rhs);
	K2_DLSPEC bool operator<= (const time_span& lhs, const time_span& rhs);
	K2_DLSPEC bool operator> (const time_span& lhs, const time_span& rhs);
	K2_DLSPEC bool operator>= (const time_span& lhs, const time_span& rhs);

	struct time_zone
	{
		typedef	int	value_type;
		static const value_type	local = 0;
		static const value_type	utc = 1;
	};

    /**
	*	\ingroup	Timing
	*	\brief		Broken-down representation of time.
	*/
	template <time_zone::value_type time_zone_>
	class calendar
	{
	public:
		/**
		*	\brief	Instantiates a calendar object to 00:00:00, Jan/1/1970 UTC.
		*/
		calendar () : m_validated(false) {
		}
		/**
		*	\brief	Instantiates a calendar object to \a timestamp.
		*
		*	\param	ts
		*/
		calendar (const timestamp& timestamp)
		:	m_timestamp(timestamp)
		,	m_validated(false) {
		}
		/**
		*	\brief	Converts *this to timestamp.
		*/
		operator const timestamp& () const {
			return	m_timestamp;
		}

		/**
		*	\brief		Converts *this to another time zone.
		*/
		template <time_zone::value_type other_time_zone >
		operator const calendar<other_time_zone> () const throw() {
		    return  calendar<other_time_zone>(m_timestamp);
		}

		/**
		*	\brief		The number of years, starting from 1970.
		*/
		uint16_t	year () const {
			this->validate();
			return	m_year;
		}
		/**
		*	\brief		The number of months since January, in the range 0 to 11.
		*/
		uint16_t	month_of_year () const {
			this->validate();
			return	m_month_of_year;
		}
		/**
		*	\brief		The day of the month_of_year, the first day of month_of_year is 0, in the range 0 to 30.
		*/
		uint16_t	day_of_month () const {
			this->validate();
			return	m_day_of_month;
		}
		/**
		*	\brief		The number of hours past midnight, in the range 0 to 23.
		*/
		uint16_t	hour_of_day () const {
			this->validate();
			return	m_hour_of_day;
		}
		/**
		*	\brief		The number of minutes after the hour, in the range 0 to 59.
		*/
		uint16_t	min_of_hour () const {
			this->validate();
			return	m_min_of_hour;
		}
		/**
		*	\brief		The number of seconds after the minute, in the range 0 to 59, but can be up to 61 to allow for leap seconds.
		*
		*	Search for \a leap \a seconds in Google to find out more.
		*/
		uint16_t	sec_of_min () const {
			this->validate();
			return	m_sec_of_min;
		}

		/**
		*	\brief		The number of milli-seconds after the second, in the range 0 to 999999.
		*/
		uint16_t	msec_of_sec () const {
			this->validate();
			return	m_msec_of_sec;
		}

		/**
		*	\brief		The day of the year, in the range 0 to 365.
		*/
		uint16_t	day_of_year () const {
			this->validate();
			return	m_day_of_year;
		}
		/**
		*	\brief		The day of the week, in the range 0 to 6.
		*/
		uint16_t	day_of_week () const {
			this->validate();
			return	m_day_of_week;
		}

	private:
		timestamp		m_timestamp;
		mutable	bool	m_validated;
		K2_DLSPEC void 	validate () const;

		mutable uint16_t	m_year;
		mutable uint16_t	m_month_of_year;
		mutable uint16_t	m_day_of_month;
		mutable uint16_t	m_hour_of_day;
		mutable uint16_t	m_min_of_hour;
		mutable uint16_t	m_sec_of_min;
		mutable uint32_t	m_msec_of_sec;
		mutable uint16_t	m_day_of_year;
		mutable uint16_t	m_day_of_week;
	};

	/**
	*	\ingroup	Timing
	*	\brief		Specialization of calendar class template for time_zone::utc.
	*/
	typedef	calendar<time_zone::utc>	utc_calendar;
	/**
	*	\ingroup	Timing
	*	\brief		Specialization of calendar class template for time_zone::local.
	*/
	typedef	calendar<time_zone::local>	local_calendar;

}   //  namespace k2

#endif  //  !K2_TIMING_H
