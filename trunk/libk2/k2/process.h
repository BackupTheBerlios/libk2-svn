/*  libk2   <k2/process.h>

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
#ifndef K2_PROCESS_H
#define K2_PROCESS_H

#ifndef K2_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif
#ifndef K2_STD_H_STRING
#   include <string>
#   define  K2_STD_H_STRING
#endif

namespace k2
{

#ifndef DOXYGEN_BLIND
    namespace nonpublic
    {
        class process_impl;
    }   //  nonpublic
#endif  //  !DOXYGEN_BLIND

    class process
    {
    public:
        K2_INJECT_COPY_BOUNCER();

        K2_DLSPEC static
            void create_detached (const std::string& filename, const std::string& cmdline = "");

        K2_DLSPEC explicit process (const std::string& filename, const std::string& cmdline = "");
        K2_DLSPEC ~process ();


    private:
        nonpublic::process_impl*    m_pimpl;
    };

}   //  namespace k2

#endif  //  K2_PROCESS_H
