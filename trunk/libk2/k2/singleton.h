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

#ifndef K2_STD_H_LIMITS
#   define  K2_STD_H_LIMITS
#   include <climits>
#endif
#ifndef K2_RUNTIME_H
#   include <k2/runtime.h>
#endif
#ifndef K2_THREAD_ONCE_H
#   include <k2/thread_once.h>
#endif
#ifndef K2_EXCEPTION_H
#   include <k2/exception.h>
#endif

namespace k2
{

    /**
    *   \ingroup    Singleton
    *   \brief      Common typedefs and constants for all singleton<>
    *               template classes.
    */
    struct singleton_base
    {
        typedef int lifetime_t;
        static const lifetime_t lifetime_short    = INT_MIN;
        static const lifetime_t lifetime_long     = INT_MAX - 1;
        static const lifetime_t lifetime_default  = 0;
    };

    /**
    *   \ingroup    Singleton
    *   \brief      An improved singleton class template based on
    *               Modern C++ Design Pattern.
    *
    *               Key features:
    *
    *               1.  Lazy initialization. Controlled instance is initialized
    *                   on first reference. This is widely supported by many
    *                   singleton patterns.
    *
    *               2.  Fine-grained control over controlled instance lifetime,
    *                   or to be more specific, when to un-initialize. Adapted
    *                   from Modern C++ Design Pattern
    *
    *               3.  Preallocation. Memory foot print for controlled instance
    *                   is allocated when template class is instanciated, i.e.
    *                   not dynamic allocated.
    *
    *               4.  Genuine thread-safe instance construction. Utilize
    *                   atomic arithmetics to insure guard against race
    *                   condition on \b "first references". Other singleton
    *                   patterns I've seen either ignore this issue or use a
    *                   "static mutex" to insure thread-safty, which is a false
    *                   sense of security, one can not use a runtime initialized
    *                   static object to insure thread-safety of another runtime
    *                   initialized static object or a \b "singleton-ed"
    *                   instance.
    *
    *               5.  Permits multiple controlled instances of the same type
    *                   (and the same lifetime), via the second template
    *                   parameter InstanceTagT.
    *
    */
    template <
        typename    InstanceT
    ,   typename    InstanceTagT = void
    ,   singleton_base::lifetime_t    Lifetime = singleton_base::lifetime_default
    >
    class singleton
    :   public singleton_base
    {
    private:
        typedef singleton<InstanceT, InstanceTagT, Lifetime>    self_type;

    public:
        singleton ();
        ~singleton ();

        static InstanceT&   instance ()
        {
            self_type::s_once.run(self_type::instance_init);
            return  *reinterpret_cast<InstanceT*>(self_type::s_instance_holder);
        }

    private:
        static void instance_init ()
        {
            InstanceT*  instance_ptr(reinterpret_cast<InstanceT*>(self_type::s_instance_holder));
            //  invoke inplace constructor to initialize instance
            new (instance_ptr) InstanceT();

            try
            {
                runtime::atexit(self_type::instance_fini, -Lifetime);
            }
            catch(...)
            {
                //  if runtime::atexit() failed, kill it now!
                instance_ptr->~InstanceT();
                throw;
            }
        }
        static void instance_fini ()
        {
            InstanceT*  instance_ptr(reinterpret_cast<InstanceT*>(self_type::s_instance_holder));
            instance_ptr->~InstanceT();
        }

    private:
        static thread_once  s_once;
        static char         s_instance_holder[sizeof(InstanceT)];
    };

    //static
    template <
        typename    InstanceT
    ,   typename    InstanceTagT
    ,   singleton_base::lifetime_t    Lifetime
    >
    thread_once singleton<InstanceT, InstanceTagT, Lifetime>::s_once = K2_THREAD_ONCE_INIT;
    //static
    template <
        typename    InstanceT
    ,   typename    InstanceTagT
    ,   singleton_base::lifetime_t    Lifetime
    >
    char singleton<InstanceT, InstanceTagT, Lifetime>::s_instance_holder[sizeof(InstanceT)];

}   //  namespace k2

#endif  //  K2_SINGLETON_H
