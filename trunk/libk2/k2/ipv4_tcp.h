/*  libk2   <k2/ipv4_tcp.h>

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
#ifndef K2_IPV4_TCP_H
#define K2_IPV4_TCP_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_IPV4_H
#   include <k2/ipv4.h>
#endif
#ifndef K2_BOUNCER_H
#   include <k2/copy_bouncer.h>
#endif

#ifndef K2_STD_H_MEMORY
#   include <memory>
#   define  K2_STD_H_MEMORY
#endif

namespace k2
{

    /** \defgroup   Networking
    */
    namespace ipv4
    {

        class tcp_transport;

        /*  \ingroup    Networking
        */
        class tcp_listener
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            K2_DLSPEC explicit tcp_listener (const layer4_addr& local_addr);
            K2_DLSPEC explicit tcp_listener (int desc, bool own);
            K2_DLSPEC ~tcp_listener ();

            const layer4_addr local_addr () const;
            int get_desc () const { return  m_desc; }

            K2_DLSPEC std::auto_ptr<tcp_transport>
                accept ();
            K2_DLSPEC std::auto_ptr<tcp_transport>
                accept (const time_span& timeout);

        protected:
            friend class tcp_transport;
            K2_DLSPEC int accept_desc ();
            K2_DLSPEC int accept_desc (const time_span& timeout);

        private:
            int     m_desc;
            bool    m_own;  //  close() on destroy?
        };

        /*  \ingroup    Networking
        */
        class tcp_transport
        {
        public:
            K2_INJECT_COPY_BOUNCER();

            K2_DLSPEC explicit tcp_transport (const layer4_addr& remote_addr);
            K2_DLSPEC explicit tcp_transport (const layer4_addr& remote_addr, const time_span& timeout);
            K2_DLSPEC tcp_transport (const layer4_addr& local_addr, const layer4_addr& remote_addr);
            K2_DLSPEC tcp_transport (const layer4_addr& local_addr, const layer4_addr& remote_addr, const time_span& timeout);
            K2_DLSPEC explicit tcp_transport (tcp_listener& listener);
            K2_DLSPEC tcp_transport (tcp_listener& listener, const time_span& timeout);
            K2_DLSPEC tcp_transport (int tcp_desc, bool own);
            K2_DLSPEC ~tcp_transport ();

            K2_DLSPEC const layer4_addr local_addr () const;
            K2_DLSPEC const layer4_addr remote_addr () const;
            int get_desc () const { return  m_desc; }

            K2_DLSPEC size_t write (const char* buf, size_t bytes);
            K2_DLSPEC size_t write_all (const char* buf, size_t bytes);
            K2_DLSPEC size_t write_all (const char* buf, size_t bytes, const time_span& timeout);

            K2_DLSPEC size_t read (char* buf, size_t bytes);
            K2_DLSPEC size_t read_all (char* buf, size_t bytes);
            K2_DLSPEC size_t read_all (char* buf, size_t bytes, const time_span& timeout);

        private:
            int     m_desc;
            bool    m_own;  //  close() on destroy?
        };

    }

}   //  namespace k2

#endif  //  !K2_IPV4_TCP_H
