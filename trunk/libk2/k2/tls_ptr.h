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
#ifndef K2_TLS_H
#define K2_TLS_H

#ifndef K2_BITS_DLSPEC_H
#   include <k2/bits/dlspec.h>
#endif
#ifndef K2_BITS_COPY_BOUNCER_H
#   include <k2/bits/copy_bouncer.h>
#endif

namespace k2
{

#if !defined(DOXYGEN_BLIND)
    namespace nonpublic
    {
        class tls_ptr_impl
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            K2_DLSPEC tls_ptr_impl (void (*destroy_routine)(void*));
            K2_DLSPEC ~tls_ptr_impl () ;

            K2_DLSPEC void* get () const ;
            K2_DLSPEC void* release () ;
            K2_DLSPEC void  reset(void* p=0) ;

            struct handle
            {
                long    holder[1];
            };

        private:
            void (*m_destroy_routine)(void*);
            handle  m_handle;
        };
    }
#endif  //  !DOXYGEN_BLIND

    /** \defgroup   Threading
    */

    /**
    *   \ingroup    Threading
    *   \brief  Management of thread local stroage (a.k.a. thread-specific data).
    *
    *   Stores a pointer to an object obtained via new on a thread-by-thread basis and
    *   calls delete on the contained pointer when the thread terminates.
    *
    *   Each thread initially stores the null pointer in a tls_ptr<> object.
    *
    *   Note: Do not destroy any data that may be stored in any thread's thread-specific data.
    *   For this reason you should not destroy a tls_ptr<> object until you are certain
    *   there are no threads running that have made use of its thread-specific data.
    */
    template <typename InstanceT>
    class tls_ptr
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        typedef InstanceT   value_type;
        /**
        *   Allocates resource for *this.
        *
        *   /throw      bad_resource_alloc
        */
        explicit tls_ptr ()
        :   m_impl(cleanup)
        {
        }
        /**
        *   Retrieves the stored pointer.
        */
        InstanceT*      get () const
        {
            return  reinterpret_cast<InstanceT*>(m_impl.get());
        }
        /**
        *   Returns this->get().
        */
        InstanceT*      operator-> () const
        {
            return  reinterpret_cast<InstanceT*>(m_impl.get());
        }
        /**
        *   Returns *(this->get()).
        */
        InstanceT&      operator* () const
        {
            return  this->get();
        }
        /**
        *   Releases and returns ownership of stored pointer.
        */
        InstanceT*      release ()
        {
            return  reinterpret_cast<InstanceT*>(m_impl.release());
        }
        /**
        *   Deletes stored pointer then take ownership of p.
        */
        void    reset (InstanceT* p = 0)
        {
            return  m_impl.reset(p);
        }

    private:
        static void cleanup (void* p)
        {
            InstanceT*  pt = reinterpret_cast<InstanceT*>(p);
            delete  pt;
        }
        nonpublic::tls_ptr_impl m_impl;
    };  //  class tls

}   //  namespace k2

#endif  //  !K2_TLS_H
