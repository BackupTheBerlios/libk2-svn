/*
 * Copyright (c) 2003, 2004, Kenneth Chang-Hsing Ho <kenho@bluebottle.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_UTILITY_H
#define K2_UTILITY_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif

#include <cwchar>

namespace std
{
    template <typename char_, typename char_traits_, typename alloc_>
    class basic_string;
    template <typename char_>
    struct char_traits;
    template <typename value_, typename alloc_>
    class vector;
    template <typename value_, typename alloc_>
    class deque;
    template <typename value_, typename alloc_>
    class list;
}

namespace k2
{
    template <typename value_>
    class local_allocator;

    template <typename char_, template <typename> class alloc_>
    struct make_string
    {
        typedef std::basic_string<
            char_,
            std::char_traits<char_>,
            alloc_<char_> >
            type;
    };

    typedef make_string<char, local_allocator>::type
        local_string;
    typedef make_string<wchar_t, local_allocator>::type
        local_wstring;

    template <typename value_>
    struct make_local_vector
    {
        typedef std::vector<
            value_,
            local_allocator<value_> >
            type;
    };

    template <typename value_>
    struct make_local_deque
    {
        typedef std::deque<
            value_,
            local_allocator<value_> >
            type;
    };

}   //  namespace k2

#endif  //  !K2_UTILITY_H
