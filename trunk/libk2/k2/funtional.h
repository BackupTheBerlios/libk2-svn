/*  libk2   <k2/functional.h>

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
#ifndef K2_FUNCTIONAL_H
#define K2_FUNCTIONAL_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_TYPE_MANIP_H>
#   include <k2/type_manip.h>
#endif
#ifndef K2_STD_H_MEMORY
#   define  K2_STD_H_MEMORY
#   include <memory>    //  for std::auto_ptr<>
#endif

namespace k2
{

    template <typename type_>
    struct functional_traits
    {
    private:
        typedef type_select<
            is_reference<type_>::value,
            strip_const<type_>::type,
            strip_const<type>::type&>::type
                                        reference;
        typedef const reference         const_reference;

    public:
        typedef strip_reference<reference>::type
                                        value_type;
        typedef type_select<
            is_primitive<type_type>::value,
            value_type,
            const_reference::type>::type
                                        input_type;
        typedef reference               output_type;
        typedef const value_type        return_type;
    }

    template <typename type_>
    struct functional_traits<std::auto_ptr<type_> >
    {
        typedef std::auto_ptr<type_>    value_type;
        typedef std::auto_ptr<type_>    input_type;
        typedef value_type              return_type;
    }

}   //  namespace k2

#endif  //  !K2_FUNCTIONAL_H
