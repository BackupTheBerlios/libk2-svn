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
#ifndef K2_EXCEPTION_H
#define K2_EXCEPTION_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif

#ifndef K2_STD_H_EXCEPTION
#   include <exception>
#   define  K2_STD_H_EXCEPTION
#endif
#ifndef K2_STD_H_CSTRING
#   include <cstring>
#   define  K2_STD_H_CSTRING
#endif

namespace k2
{

    /** \defgroup   Exception
    */

    /**
    *   \ingroup    Exception
    *   \brief  Base exception class.
    *
    *   Base exception class, that guarantee no-throw.
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
            return  m_what;
        }

    private:
        char    m_what[max_what + 1];
    };

    /**
    *   \ingroup    Exception
    *   \brief Timed-out exception class.
    */
    struct timedout_error : exception
    {
        explicit timedout_error (const char* what = "timedout_error") throw ()
            :   exception(what) {};
    };

    /**
    *   \ingroup    Exception
    *   \brief No resource exception class.
    */
    struct bad_resource_alloc : exception
    {
        explicit bad_resource_alloc (const char* what = "bad_resource_alloc") throw ()
            :   exception(what) {};
    };

    /**
    *   \ingroup    Exception
    *   \brief No resource exception class.
    */
    struct critical_error : exception
    {
        explicit critical_error (const char* what = "critical_error") throw ()
            :   exception(what) {};
    };


    /** \defgroup   Threading
    */

    /**
    *   \ingroup    Exception
    *   \ingroup    Threading
    *   \brief Exception class for threading classes.
    */
    struct thread_error : exception
    {
        explicit thread_error (const char* what = "thread_error")
            :   exception(what) {};
    };

    /**
    *   \ingroup    Exception
    *   \brief General runtime error exception class.
    */
    struct runtime_error : exception
    {
        explicit runtime_error (const char* what = "runtime_error") throw ()
            :   exception(what) {};
    };

}   //  namespace k2

#endif  //  !K2_EXCEPTION_H
