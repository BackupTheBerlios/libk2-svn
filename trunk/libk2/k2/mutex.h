/*  libk2   <k2/mutex.h>

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
#ifndef K2_MUTEX_H
#define K2_MUTEX_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif

#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif
#ifndef K2_SCOPE_H
#   include <k2/scope.h>
#endif

namespace k2
{

    class timestamp;
    class time_span;

    /** \defgroup   Threading
    */

    /**
    *   \ingroup    Threading
    *   \brief      MUTual EXclusion.
    *   \relates    scoped_guard<>
    *   \relates    cond_var
    *   \relates    timestamp
    */
    class mutex
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        typedef scoped_guard<mutex> scoped_guard;

        /**
        *   \brief      Allocates resource for *this.
        */
        K2_DLSPEC mutex ();
        /**
        *   \brief      Releases resource for *this.
        */
        K2_DLSPEC ~mutex () ;

        /**
        *   \brief      Acquires ownership of *this, blocks calling thread
        *               until successful.
        *   \throw      thread_error
        */
        K2_DLSPEC void  acquire () ;
        /**
        *   \brief      Acquires ownership of *this, blocks calling thread
        *               until successful, or timed-out.
        *   \return     true, if successfully acquired ownership.
        *               false, if timed-out.
        *   \throw      thread_error
        */
        K2_DLSPEC bool  acquire (const timestamp& timer) ;

        /**
        *   \brief      Releases ownership of *this.
        *   \throw      thread_error
        */
        K2_DLSPEC void  release () ;

#if !defined(DOXYGEN_BLIND)
        struct handle
        {
            long    holder[8];
        };
#endif  //  !DOXYGEN_BLIND

    private:
        handle  m_handle;
        friend class cond_var;
    };  //  class mutex

}   //  namespace k2

#endif  //  !K2_MUTEX_H
