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
#include <k2/timing.h>

#if !defined(WIN32)
#   include <sys/time.h>
#else
#   include <sys/timeb.h>
#endif

#include <source/pthread_util.inl>
#include <time.h>

namespace   //  unnamed
{
    static const k2::uint64_t   one_thousand    = 1000;

    inline k2::uint64_t os_timestamp ()
#if !defined(WIN32)
    {
        timeval tv;
        gettimeofday(&tv, NULL);

        return  (k2::uint64_t(tv.tv_sec) * one_thousand +
            tv.tv_usec / one_thousand);
    }
#else
    {
        _timeb  tb;
        _ftime( &tb );

        return  (k2::uint64_t(tb.time) * one_thousand + tb.millitm );
    }
#endif

}   //  unnamed namespace

//static
k2::timestamp::now_tag  k2::timestamp::now;

k2::timestamp::timestamp ()
:   m_msec(os_timestamp())
{
}
k2::timestamp::timestamp (const timestamp& rhs)
:   m_msec(rhs.m_msec)
{
}
k2::timestamp::timestamp (const time_span& span_to_increse_from_now)
:   m_msec(os_timestamp())
{
    *this += span_to_increse_from_now;
}
k2::timestamp::timestamp (uint64_t msec)
:   m_msec(msec)
{
}
k2::timestamp&
k2::timestamp::operator= (const timestamp& rhs)
{
    m_msec = rhs.m_msec;
    return  *this;
}
k2::uint64_t
k2::timestamp::in_msec () const
{
    return  m_msec;
}
k2::uint32_t
k2::timestamp::in_sec () const
{
    return  uint32_t(m_msec / one_thousand);
}
k2::uint16_t
k2::timestamp::msec_of_sec () const
{
    return  uint16_t(m_msec % one_thousand);
}
bool
k2::timestamp::expired () const
{
    return  *this <= timestamp::now;
}
k2::timestamp&
k2::timestamp::operator+= (const time_span& span_to_increase)
{
    m_msec += span_to_increase.in_msec();
    return  *this;
}
k2::timestamp&
k2::timestamp::operator-= (const time_span& span_to_decrease)
{
    m_msec -= span_to_decrease.in_msec();
    return  *this;
}
const k2::time_span
k2::timestamp::operator-= (const timestamp& rhs)
{
    if (m_msec > rhs.in_msec())
        return  time_span(m_msec - rhs.in_msec());
    else
        return  time_span(0);
}
const k2::time_span
k2::timestamp::operator-= (timestamp::now_tag)
{
    uint64_t    now = os_timestamp();
    if (m_msec > now)
        return  time_span(m_msec - now);
    else
        return  time_span(0);
}
k2::uint32_t
k2::timestamp::hash () const
{
    //  Ideally, magic_num should be a timestamp close to present time.
    //  Perhaps year 2000?
    static const uint64_t   magic_num = 0;
    uint64_t    to_hash = m_msec - magic_num;
    uint32_t*   pint32 = reinterpret_cast<uint32_t*>(&to_hash);
    return
#if defined(K2_BYTEORDER_BIGENDIAN)
    pint32[1];
#else
    pint32[0];
#endif
}
k2::timestamp
k2::operator+ (const timestamp& val, const time_span& span_to_increase)
{
    timestamp res(val);
    res += span_to_increase;
    return  res;
}
k2::timestamp
k2::operator- (const timestamp& val, const time_span& span_to_decrease)
{
    timestamp res(val);
    res -= span_to_decrease;
    return  res;
}
k2::timestamp
k2::operator+ (timestamp::now_tag, const time_span& span_to_increase)
{
    return  timestamp(os_timestamp() + span_to_increase.in_msec());
}
k2::timestamp
k2::operator- (timestamp::now_tag, const time_span& span_to_decrease)
{
    return  timestamp(os_timestamp() - span_to_decrease.in_msec());
}
k2::time_span
k2::operator- (const timestamp& lhs, const timestamp& rhs)
{
    timestamp   tmp(lhs);
    return  (tmp -= rhs);
}
k2::time_span
k2::operator- (timestamp::now_tag, const timestamp& rhs)
{
    return  time_span(os_timestamp() - rhs.in_msec());
}
k2::time_span
k2::operator- (const timestamp& lhs, timestamp::now_tag)
{
    timestamp   tmp(lhs);
    return  (tmp -= timestamp::now);
}
bool
k2::operator== (const timestamp& lhs, const timestamp& rhs)
{
    return  lhs.in_msec() == rhs.in_msec();
}
bool
k2::operator!= (const timestamp& lhs, const timestamp& rhs)
{
    return  lhs.in_msec() != rhs.in_msec();
}
bool
k2::operator< (const timestamp& lhs, const timestamp& rhs)
{
    return  lhs.in_msec() < rhs.in_msec();
}
bool
k2::operator<= (const timestamp& lhs, const timestamp& rhs)
{
    return  lhs.in_msec() <= rhs.in_msec();
}
bool
k2::operator> (const timestamp& lhs, const timestamp& rhs)
{
    return  lhs.in_msec() > rhs.in_msec();
}
bool
k2::operator>= (const timestamp& lhs, const timestamp& rhs)
{
    return  lhs.in_msec() >= rhs.in_msec();
}
bool
k2::operator== (const timestamp& lhs, timestamp::now_tag)
{
    return  lhs.in_msec() == os_timestamp();
}
bool
k2::operator!= (const timestamp& lhs, timestamp::now_tag)
{
    return  lhs.in_msec() != os_timestamp();
}
bool
k2::operator< (const timestamp& lhs, timestamp::now_tag)
{
    return  lhs.in_msec() < os_timestamp();
}
bool
k2::operator<= (const timestamp& lhs, timestamp::now_tag)
{
    return  lhs.in_msec() <= os_timestamp();
}
bool
k2::operator> (const timestamp& lhs, timestamp::now_tag)
{
    return  lhs.in_msec() > os_timestamp();
}
bool
k2::operator>= (const timestamp& lhs, timestamp::now_tag)
{
    return  lhs.in_msec() >= os_timestamp();
}
bool
k2::operator== (timestamp::now_tag, const timestamp& rhs)
{
    return  os_timestamp() == rhs.in_msec();
}
bool
k2::operator!= (timestamp::now_tag, const timestamp& rhs)
{
    return  os_timestamp() != rhs.in_msec();
}
bool
k2::operator< (timestamp::now_tag, const timestamp& rhs)
{
    return  os_timestamp() < rhs.in_msec();
}
bool
k2::operator<= (timestamp::now_tag, const timestamp& rhs)
{
    return  os_timestamp() <= rhs.in_msec();
}
bool
k2::operator> (timestamp::now_tag, const timestamp& rhs)
{
    return  os_timestamp() > rhs.in_msec();
}
bool
k2::operator>= (timestamp::now_tag, const timestamp& rhs)
{
    return  os_timestamp() >= rhs.in_msec();
}







k2::time_span::time_span ()
:   m_msec(0)
{
}
k2::time_span::time_span (const time_span& rhs)
:   m_msec(rhs.m_msec)
{
}
k2::time_span::time_span (uint64_t msec)
:   m_msec(msec)
{
}
k2::time_span&
k2::time_span::operator= (const time_span& rhs)
{
    m_msec = rhs.m_msec;
    return  *this;
}
k2::uint64_t
k2::time_span::in_msec () const
{
    return  m_msec;
}
k2::uint32_t
k2::time_span::in_sec () const
{
    return  uint32_t(m_msec / one_thousand);
}
k2::uint16_t
k2::time_span::msec_of_sec () const
{
    return  uint16_t(m_msec % one_thousand);
}
k2::time_span&
k2::time_span::operator+= (const time_span& span_to_increase)
{
    m_msec += span_to_increase.m_msec;
    return  *this;
}
k2::time_span&
k2::time_span::operator-= (const time_span& span_to_decrease)
{
    m_msec -= span_to_decrease.m_msec;
    return  *this;
}
k2::time_span&
k2::time_span::operator*= (uint_t mul)
{
    m_msec *= mul;
    return  *this;
}
k2::time_span&
k2::time_span::operator/= (uint_t div)
{
    m_msec /= div;
    return  *this;
}
k2::time_span
k2::operator+ (const time_span& val, const time_span& span_to_increase)
{
    time_span res(val);
    res += span_to_increase;
    return  res;
}
k2::time_span
k2::operator- (const time_span& val, const time_span& span_to_decrease)
{
    time_span res(val);
    res -= span_to_decrease;
    return  res;
}
k2::time_span
k2::operator* (const time_span& val, uint_t mul)
{
    time_span res(val);
    res *= mul;
    return  res;
}
k2::time_span
k2::operator/ (const time_span& val, uint_t div)
{
    time_span res(val);
    res /= div;
    return  res;
}
bool
k2::operator== (const time_span& lhs, const time_span& rhs)
{
    return  lhs.in_msec() == rhs.in_msec();
}
bool
k2::operator!= (const time_span& lhs, const time_span& rhs)
{
    return  lhs.in_msec() != rhs.in_msec();
}
bool
k2::operator< (const time_span& lhs, const time_span& rhs)
{
    return  lhs.in_msec() < rhs.in_msec();
}
bool
k2::operator<= (const time_span& lhs, const time_span& rhs)
{
    return  lhs.in_msec() <= rhs.in_msec();
}
bool
k2::operator> (const time_span& lhs, const time_span& rhs)
{
    return  lhs.in_msec() > rhs.in_msec();
}
bool
k2::operator>= (const time_span& lhs, const time_span& rhs)
{
    return  lhs.in_msec() >= rhs.in_msec();
}






namespace
{

    pthread_mutex_t gmtime_lock = PTHREAD_MUTEX_INITIALIZER;

}   //  namespace

template </*k2::time_zone_constant*/>
void
k2::calendar<k2::time_zone::utc>::validate () const
{
    if (m_validated == false)
    {
        time_t  t = m_timestamp.in_sec();

        {
            ptmtx_guard guard(gmtime_lock);

            tm* ptm = gmtime(&t);

            m_year          = ptm->tm_year + 1970;
            m_month_of_year = ptm->tm_mon;
            m_day_of_month  = ptm->tm_mday - 1;
            m_hour_of_day   = ptm->tm_hour;
            m_min_of_hour   = ptm->tm_min;
            m_sec_of_min    = ptm->tm_sec;
            m_day_of_year   = ptm->tm_yday;
            m_day_of_week   = ptm->tm_wday;
        }
        m_validated     = true;
        m_msec_of_sec   = m_timestamp.msec_of_sec();
    }
}

template </*k2::time_zone_constant*/>
void
k2::calendar<k2::time_zone::local>::validate () const
{
    if (m_validated == false)
    {
        time_t  t = m_timestamp.in_sec();

        {
            ptmtx_guard guard(gmtime_lock);

            tm*     ptm = localtime(&t);

            m_year          = ptm->tm_year + 1970;
            m_month_of_year = ptm->tm_mon;
            m_day_of_month  = ptm->tm_mday - 1;
            m_hour_of_day   = ptm->tm_hour;
            m_min_of_hour   = ptm->tm_min;
            m_sec_of_min    = ptm->tm_sec;
            m_day_of_year   = ptm->tm_yday;
            m_day_of_week   = ptm->tm_wday;
        }
        m_validated     = true;
        m_msec_of_sec   = m_timestamp.msec_of_sec();
    }
}
