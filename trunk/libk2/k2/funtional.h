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
    public:
        typedef type_select<
            is_reference<type_>::value,
            strip_const<type_>::type,
            strip_const<type>::type&>::type
                                        reference;
        typedef const reference         const_reference;
        typedef strip_reference<reference>::type
                                        value_type;
        typedef type_select<
            is_primitive<type_type>::value,
            value_type,
            const_reference::type>::type
                                        input_type;
        typedef reference               output_type;
    }

    //  Specialization for std::auto_ptr<>, diables types where
    //  std::auto_ptr<> has no semantics.
    template <typename type_>
    struct functional_traits<std::auto_ptr<type_> >
    {
        typedef std::auto_ptr<type_>    input_type;
        typedef std::auto_ptr<type_>    return_type;
    }

}   //  namespace k2

#endif  //  !K2_FUNCTIONAL_H
