/*  libk2   <k2/scope.h>

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
#ifndef K2_SCOPE_H
#define K2_SCOPE_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_EXCEPTION_H
#   include <k2/exception.h>
#endif
#ifndef K2_COPY_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif

namespace k2
{

    class timestamp;

    template <typename type_>
    class scoped_guard
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        scoped_guard (type_& obj)
        :   m_obj(obj)
        {;
            m_obj.acquire();
        }
        scoped_guard (type_& obj, const timestamp& timer)
        :   m_obj(obj)
        {
            if (m_obj.acquire(timer) == false)
                throw   timedout_error();
        }
        ~scoped_guard ()
        {
            m_obj.release();
        }


    private:
        type_&  m_obj;
    };


}

#endif  //  !K2_SCOPE_H
