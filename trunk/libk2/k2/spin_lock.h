/*  libk2   <k2/spin_lock.h>

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
#ifndef K2_SPIN_LOCK_H
#define K2_SPIN_LOCK_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif
#ifndef K2_SCOPE_H
#   include <k2/scope.h>
#endif
#ifndef K2_ATOMIC_H
#   include <k2/atomic.h>
#endif

namespace k2
{

    class timestamp;

    /** \defgroup   Threading
    */

    /**
    *   \ingroup    Threading
    *   \brief      Threading synchronization primitive.
    *   \relates    scoped_guard<>
    *   \relates    timestamp
    */
    class spin_lock
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        typedef scoped_guard<spin_lock> scoped_guard;

        K2_DLSPEC   spin_lock ();

        K2_DLSPEC   void acquire ();
        K2_DLSPEC   bool acquire (const timestamp& timer);
        K2_DLSPEC   void release ();

    private:
        atomic_int_t    m_lock;
    };


}   //  namespace k2

#endif  //  !K2_SPIN_LOCK_H
