/*
    Copyright (c) 2003, 2004, Kenneth Chang-Hsing Ho
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the k2 nor the names of its contributors may be used
      to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef K2_COND_VAR_H
#define K2_COND_VAR_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif

namespace k2
{

    class mutex;

    /** \defgroup   Threading
    */

    /**
    *   \ingroup    Threading
    *   \brief      CONDition VARiable.
    *   \relates    mutex
    *   \relates    timestamp
    */
    class cond_var
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        /**
        *   \brief      Allocates resource for *this.
        *
        *   Constructs and establish association with \a mtx.
        */
        K2_DLSPEC cond_var (mutex& mtx) ;
        /**
        *   \brief      Releases resource for *this.
        */
        K2_DLSPEC ~cond_var () ;

        /**
        *   \brief      Waits for *this to be signalled.
        *
        *   \pre        Calling thread has acquired ownership of associated
        *               mutex.
        *
        *   Releases ownership of associated mutex, blocks the calling thread
        *   until signalled. The owership of the mutex is resumed before return.
        */
        K2_DLSPEC void wait ();

        template <typename pred_t_>
        void wait (pred_t_ pred)
        {
            while (pred() == false)
            {
                this->wait(mxg);
            }
        }

        /**
        *   \brief      Waits for *this to be signalled.
        */
        K2_DLSPEC bool wait (const timestamp& timer);

        template <typename pred_t_>
        void wait (const timestamp& timer, pred_t_ pred)
        {
            while (pred() == false)
            {
                this->wait(mxg, timer);
            }
        }

        /**
        *   \brief      Signals to one of waiting threads.
        */
        K2_DLSPEC void signal () ;

        /**
        *   \brief      Signals to all waiting threads.
        */
        K2_DLSPEC void broadcast () ;

#if !defined(DOXYGEN_BLIND)
        struct handle
        {
            long    holder[16];
        };
#endif  //  !DOXYGEN_BLIND

    private:
        handle  m_handle;
        mutex&  m_mtx;
    };  //  class cond_var

}   //  namespace k2

#endif  //  !K2_COND_VAR_H
