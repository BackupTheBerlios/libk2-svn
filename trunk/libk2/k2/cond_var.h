/*  libk2   <k2/cond_var.h>

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
#ifndef K2_COND_VAR_H
#define K2_COND_VAR_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_BOUNCER_H
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
            char    holder[sizeof(long)];
        };
#endif  //  !DOXYGEN_BLIND

    private:
        handle  m_handle;
        mutex&  m_mtx;
    };  //  class cond_var

}   //  namespace k2

#endif  //  !K2_COND_VAR_H
