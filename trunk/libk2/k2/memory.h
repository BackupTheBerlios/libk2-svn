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
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_MEMORY_H
#define K2_MEMORY_H

#ifndef K2_BITS_AUTO_ALLOC_H
#   include <k2/bits/auto_alloc.h>
#endif

namespace k2
{

    /**
    *   \brief A Standard Allocator-compliant class template.
    *
    *   Its first template parameter \b ValueT has the same sematic
    *   meaning of the first template parameter of std::allocator class tmeplate.
    *
    *   Allocations made through object instances of auto_allocator<> are on
    *   the stack-frame.
    *   This is probably close to the fastest allocation avaiable (if not the
    *   fastest). Only use it when you need extream performance and know what
    *   "allocation on the stack" means and its consequences.
    */
    template <typename ValueT>
    class auto_allocator
    :   public defalloc_base<ValueT>
    {
    public:
        template <typename OtherT>
        struct rebind
        {
            typedef auto_allocator<OtherT>  other;
        };

        auto_allocator () {}
        auto_allocator (const auto_allocator<ValueT>&) {}
        template <typename OtherT>
        auto_allocator (const auto_allocator<OtherT>&) {}
        ~auto_allocator () {}

        template <typename OtherT>
        auto_allocator& operator= (const auto_allocator<OtherT>&)
        {
            return  *this;
        }

        pointer allocate (size_type count, const void* hint = 0)
        {
            return  reinterpret_cast<pointer>(bits::auto_alloc(sizeof(ValueT) * count));
        }
        void deallocate (pointer p, size_type n)
        {
            //  no-op.
        }
    };
    template <>
    class auto_allocator<void>
    :   public defalloc_base<void>
    {
    public:
        template <typename OtherT>
        struct rebind
        {
            typedef auto_allocator<OtherT>  other;
        };
    };
    template <typename LhsT, typename RhsT>
    bool operator== (const auto_allocator<LhsT>&, const auto_allocator<RhsT>&)
    {
        return  true;
    }
    template <typename LhsT, typename RhsT>
    bool operator!= (const auto_allocator<LhsT>&, const auto_allocator<RhsT>&)
    {
        return  false;
    }



#if(0)
    template <typename type_>
    struct destroyer
    {
        explicit destroyer (type_* p)
        :   m_p(p) {}

        void operator() () const
        {
            delete  m_p;
        }
        type_*  m_p;
    };
    #endif

}   //  namespace k2

#endif  //  !K2_MEMORY_H
