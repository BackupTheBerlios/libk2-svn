/*
 * Copyright (c) 2003, 2004, 2005,
 * Kenneth Chang-Hsing Ho <kenho@bluebottle.com> All rights reterved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of k2, libk2, copyright owners nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT OWNERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef K2_SINGLETON_H
#define K2_SINGLETON_H

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
        typedef int lifetime_t;

        static const lifetime_t lifetime_short = INT_MIN;
        static const lifetime_t lifetime_long  = INT_MAX - 1;
        static const lifetime_t lifetime_normal= 0;

        static runtime::prio_t lifetime_to_atexit_prio(lifetime_t lifetime)
        {
            return  -lifetime;
        }
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
            static singleton::lifetime_t
                s_lifetime;
        };
        //  static
        template <typename owner_t_, typename instance_t_>
        thread_once singleton_holder<owner_t_, instance_t_>::s_once = K2_THREAD_ONCE_INIT;
        //  static
        template <typename owner_t_, typename instance_t_>
        instance_t_* singleton_holder<owner_t_, instance_t_>::s_instance_ptr = 0;
        //  static
        template <typename owner_t_, typename instance_t_>
        singleton::lifetime_t   singleton_holder<owner_t_, instance_t_>::s_lifetime = 0;


        template <typename owner_t_, typename instance_t_, singleton::lifetime_t lifetime_>
        class singleton_impl : singleton_holder<owner_t_, instance_t_>
        {
        protected:
            static instance_t_& instance ();
            static void instance_init ();
            static void instance_fini ();

            typedef singleton_holder<owner_t_, instance_t_>
                singleton_holder_type;
            typedef singleton_impl<owner_t_, instance_t_, lifetime_>
                self_type;
        };
        //  static
        template <typename owner_t_, typename instance_t_, singleton::lifetime_t lifetime_>
        void singleton_impl<owner_t_, instance_t_, lifetime_>::instance_init ()
        {
            std::auto_ptr<instance_t_>  sptr(new instance_t_());
            destroyer<instance_t_>      dtor(sptr.get());

            runtime::atexit(dtor, singleton::lifetime_to_atexit_prio(lifetime_));

            singleton_holder_type::s_instance_ptr = sptr.release();
            singleton_holder_type::s_lifetime = lifetime_;
        }
        //  static
        template <typename owner_t_, typename instance_t_, singleton::lifetime_t lifetime_>
        void singleton_impl<owner_t_, instance_t_, lifetime_>::instance_fini ()
        {
            delete  singleton_holder_type::s_instance_ptr;
        }
        //  static
        template <typename owner_t_, typename instance_t_, singleton::lifetime_t lifetime_>
        instance_t_& singleton_impl<owner_t_, instance_t_, lifetime_>::instance ()
        {
            singleton_holder_type::s_once.run(self_type::instance_init);
            if (lifetime_ != singleton_holder_type::s_lifetime)
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
    template <typename instance_t_, singleton::lifetime_t lifetime_ = singleton::lifetime_normal>
    class process_singleton
    :   nonpublic::singleton_impl<process_singleton<instance_t_, lifetime_>, instance_t_, lifetime_>
    {
    private:
        typedef process_singleton<instance_t_, lifetime_>
            self_type;
        typedef nonpublic::singleton_impl<self_type, instance_t_, lifetime_>
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
    template <typename owner_t_, typename instance_t_, singleton::lifetime_t lifetime_ = singleton::lifetime_normal >
    class class_singleton
    :   nonpublic::singleton_impl<owner_t_, instance_t_, lifetime_>
    {
    private:
        typedef nonpublic::singleton_impl<owner_t_, instance_t_, lifetime_>
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
    template <typename instance_t_, singleton::lifetime_t lifetime_ = singleton::lifetime_normal >
    class thread_local_singleton
    :   nonpublic::singleton_impl<thread_local_singleton<instance_t_, lifetime_>, tls_ptr<instance_t_>, lifetime_>
    {
    private:
        typedef thread_local_singleton<instance_t_, lifetime_>
            self_type;
        typedef nonpublic::singleton_impl<self_type, tls_ptr<instance_t_>, lifetime_>
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

#if(0)


    template <init_type_>
    struct local_static_traits
    {
        static const
    };

   template <typename method_traits_>
    class singleton
    {
    public:
        template <typename value_type_>
        static value_type_& instance ()
        {
            static instance_type static_local_instance;
            return  static_local_instance;
        }
    };

    class local_static_tag
    {
    };

    template <typename method_tag_, typename init_>
    class singleton
    {
    public:
        explicit singleton (init_ v)
        :   m_init(v)
        ,   m_instance(0)
        {
        }



    private:

    };
    template <>
    class singleton<local_static_tag>
    {
    public:
        singleton (
        template <typename value_type_>
        static value_type_& instance ()
        {
            static instance_type static_local_instance;
            return  static_local_instance;
        }
    };

    class singleton_using_local_static
    {
    public:
        template <typename value_type_>
        static value_type_& instance ()
        {
            static instance_type static_local_instance;
            return  static_local_instance;
        }
    };






    class local_static_set
    {
    public:
        template <typename value_type_>
        static value_type_& instance ()
        {
            static instance_type static_local_instance;
            return  static_local_instance;
        }
    };

#if !defined(DOXYGEN_BLIND)
    namespace nonpublic
    {
        template <typename instance_tag_, typename value_type_>
        class class_static_instanciator
        {
        public:
            static  value_type_ instance;
        };
        template <typename instance_tag_, typename value_type_>
        class_static_instanciator<instance_tag_, value_type_>::value_type_
        class_static_instanciator<instance_tag_, value_type_>::instance;
    }   //  namespace nonpublic
#endif  //  !DOXYGEN_BLIND

    template <typename instance_tag_ = global_tag>
    class class_static_set
    {
    public:
        template <typename value_type_>
        static value_type_& instance ()
        {
            return  nonpublic::class_static_instanciator<instance_tag_, value_type_>::instance;
        }
    };

    template <typename instance_t_, singleton::lifetime_t lifetime_ = singleton::lifetime_normal>
    class process_singleton

    bar& foo ()
    {
        local_static_set<>::instance<bar> instance;
        return  instance();

        static  class_instance
    }

#endif

}   //  namespace k2

#endif  //  K2_SINGLETON_H
