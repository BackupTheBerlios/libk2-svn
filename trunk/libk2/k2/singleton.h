/*  libk2   <k2/singleton.h>

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
#ifndef K2_SINGLETON_H
#define K2_SINGLETON_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_RUNTIME_H
#   include <k2/runtime.h>
#endif
#ifndef K2_THREAD_ONCE_H
#   include <k2/thread_once.h>
#endif
#ifndef K2_TLS_H
#   include <k2/tls.h>
#endif
#ifndef K2_EXCEPTION_H
#   include <k2/exception.h>
#endif
#ifndef K2_ASSERT_H
#   include <k2/assert.h>
#endif
#ifndef K2_MEMORY_H
#   include <k2/memory.h>   //  for template <> class destroyer
#endif

namespace k2
{

    /** \defgroup   Singleton
    */

    /**
    *   \ingroup    Singleton
    *   \brief      Utility class for \ref singleton module.
    *
    *   Also see process_singleton class template, class_singleton class template and thread_local_singleton class template.
    */
    struct singleton
    {
        typedef runtime::atexit_prio_t  destroy_prio_t;

        static const destroy_prio_t destroy_prio_highest    =   runtime::atexit_prio_highest;
        static const destroy_prio_t destroy_prio_lowest     =   runtime::atexit_prio_lowest;
        static const destroy_prio_t destroy_prio_default    =   runtime::atexit_prio_default;
    };


#ifndef DOXYGEN_BLIND
    namespace nonpublic
    {

        template <typename owner_t_, typename instance_t_>
        class singleton_holder
        {
        protected:
            static thread_once
                s_once;
            static instance_t_*
                s_instance_ptr;
            static singleton::destroy_prio_t
                s_destroy_prio;
        };
        //  static
        template <typename owner_t_, typename instance_t_>
        thread_once singleton_holder<owner_t_, instance_t_>::s_once = K2_THREAD_ONCE_INIT;
        //  static
        template <typename owner_t_, typename instance_t_>
        instance_t_* singleton_holder<owner_t_, instance_t_>::s_instance_ptr = 0;
        //  static
        template <typename owner_t_, typename instance_t_>
        singleton::destroy_prio_t   singleton_holder<owner_t_, instance_t_>::s_destroy_prio = 0;


        template <typename owner_t_, typename instance_t_, singleton::destroy_prio_t destroy_prio_>
        class singleton_impl : singleton_holder<owner_t_, instance_t_>
        {
        protected:
            static instance_t_& instance ();
            static void instance_init ();
            static void instance_fini ();

            typedef singleton_holder<owner_t_, instance_t_>
                singleton_holder_type;
            typedef singleton_impl<owner_t_, instance_t_, destroy_prio_>
                self_type;
        };
        //  static
        template <typename owner_t_, typename instance_t_, singleton::destroy_prio_t destroy_prio_>
        void singleton_impl<owner_t_, instance_t_, destroy_prio_>::instance_init ()
        {
            std::auto_ptr<instance_t_>  sptr(new instance_t_());
            destroyer<instance_t_>      dtor(sptr.get());

            runtime::atexit(dtor, destroy_prio_);

            singleton_holder_type::s_instance_ptr = sptr.release();
            singleton_holder_type::s_destroy_prio = destroy_prio_;
        }
        //  static
        template <typename owner_t_, typename instance_t_, singleton::destroy_prio_t destroy_prio_>
        void singleton_impl<owner_t_, instance_t_, destroy_prio_>::instance_fini ()
        {
            delete  singleton_holder_type::s_instance_ptr;
        }
        //  static
        template <typename owner_t_, typename instance_t_, singleton::destroy_prio_t destroy_prio_>
        instance_t_& singleton_impl<owner_t_, instance_t_, destroy_prio_>::instance ()
        {
            singleton_holder_type::s_once.run(self_type::instance_init);
            if (destroy_prio_ != singleton_holder_type::s_destroy_prio)
                throw   critical_error("singleton<>::instance() is referenced with differnt destroy-priorities!!!");

            return  *singleton_holder_type::s_instance_ptr;
        }

    }
#endif  //  DOXYGEN_BLIND

    /**
    *   \ingroup    Singleton
    *   \brief      runtime wide singleton class template.
    *
    *   Use in place of global(static) objects, featuring:
    *   \li Guaranteed initialized on first reference.
    *   \li Managed destroy-priority.
    *
    *   Also see class_singleton class template and thread_local_singleton class template .
    */
    template <typename instance_t_, singleton::destroy_prio_t destroy_prio_ = singleton::destroy_prio_default>
    class process_singleton
    :   nonpublic::singleton_impl<process_singleton<instance_t_, destroy_prio_>, instance_t_, destroy_prio_>
    {
    private:
        typedef process_singleton<instance_t_, destroy_prio_>
            self_type;
        typedef nonpublic::singleton_impl<self_type, instance_t_, destroy_prio_>
            impl_type;

    public:
        /**
        *   \return     Reference to the contained object.
        *
        *   \b Example:
        *   \includelineno  singleton_example1.cpp
        */
        static instance_t_& instance ()
        {
            return  impl_type::instance();
        }
    };



    /**
    *   \ingroup    Singleton
    *   \brief      Class private singleton class template.
    *
    *   Use in place of class static objects.
    *   \li Guaranteed initialized on first reference.
    *   \li Managed destroy-priority.
    *   \li Prohibits referencing outside owner_t_.
    *
    *   Also see process_singleton class template and thread_local_singleton class template.
    */
    template <typename owner_t_, typename instance_t_, singleton::destroy_prio_t destroy_prio_ = singleton::destroy_prio_default >
    class class_singleton
    :   nonpublic::singleton_impl<owner_t_, instance_t_, destroy_prio_>
    {
    private:
        typedef nonpublic::singleton_impl<owner_t_, instance_t_, destroy_prio_>
            impl_type;
    public:
        /**
        *   \return     Reference to the contained object.
        *
        *   \b Example:
        *   \includelineno  singleton_example2.cpp
        */
        static instance_t_& instance ()
        {
            return  impl_type::instance();
        }
    };

    /**
    *   \ingroup    Singleton
    *   \brief      Thread local singleton class template.
    *
    *   Use in place of thread local data.
    *   \li Guaranteed initialized on first reference on a thread-by-thread basis.
    *   \li Deletes the contained object when the thread terminates
    *   \li Managed destroy-priority.
    *
    *   Also see process_singleton class template and class_singleton class template.
    */
    template <typename instance_t_, singleton::destroy_prio_t destroy_prio_ = singleton::destroy_prio_default >
    class thread_local_singleton
    :   nonpublic::singleton_impl<thread_local_singleton<instance_t_, destroy_prio_>, tls_ptr<instance_t_>, destroy_prio_>
    {
    private:
        typedef thread_local_singleton<instance_t_, destroy_prio_>
            self_type;
        typedef nonpublic::singleton_impl<self_type, tls_ptr<instance_t_>, destroy_prio_>
            impl_type;

    public:
        /**
        *   \return     Reference to the contained object for now thread.
        *
        *   \b Example:
        *   \includelineno  singleton_example3.cpp
        */
        static instance_t_& instance ()
        {
            instance_t_*    p = impl_type::instance().get();
            if (p == 0)
            {
                std::auto_ptr<instance_t_>  sptr(new instance_t_());
                impl_type::instance().reset(sptr.get());
                p = sptr.release();
            }
            return  *p;
        }
    };

}   //  namespace k2

#endif  //  K2_SINGLETON_H
