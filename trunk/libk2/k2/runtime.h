/*
    Copyright (c) 2003, 2004, Kenneth Chang-Hsing Ho
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    * Neither the name of the k2 nor the names of its contributors may be used
      to endorse or promote products derived from this software without
      specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef K2_RUNTIME_H
#define K2_RUNTIME_H

#ifndef K2_CONFIG_H
#   include <k2/config.h>
#endif
#ifndef K2_STD_H_CLIMITS
#   include <climits>
#   define  K2_STD_H_CLIMITS
#endif
#ifndef K2_STD_H_MEMROY
#   include <memory>
#   define  K2_STD_H_MEMORY
#endif

namespace k2
{

    class runtime
    {
    public:
        K2_DLSPEC static void init ();
        K2_DLSPEC static void fini ();

        K2_DLSPEC static bool is_dl ();

        typedef int prio_t;
        static const prio_t prio_highest      = INT_MAX;
        static const prio_t prio_lowest       = INT_MIN + 1;
        static const prio_t prio_default      = 0;

        template <typename functor0_>
        static void atexit (functor0_ func, prio_t pri)
        {
            functor0_* arg = new functor0_(func);
            void(*adapter)(void*) = runtime::adapter<functor0_>;
            runtime::atexit_impl(adapter, (void*)arg, pri);
        }

    private:
        K2_DLSPEC static void atexit_impl (void (*adapter)(void*), void* function0, prio_t pri);
        template <typename functor0_>
        static void adapter (void* function0)
        {
            std::auto_ptr<functor0_> functor0(reinterpret_cast<functor0_*>(function0));
            (*functor0)();
        }
    };

}   //  namespace k2

#endif  //  !K2_RUNTIME_H
