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
#ifndef K2_PROCESS_H
#define K2_PROCESS_H

#ifndef K2_BITS_DLSPEC_H
#   include <k2/bits/dlspec.h>
#endif
#ifndef K2_BITS_COPY_BOUNCER_H
#   include <k2/bits/copy_bouncer.h>
#endif
#ifndef K2_STD_H_STRING
#   include <string>
#   define  K2_STD_H_STRING
#endif

namespace k2
{

    namespace trail
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

            K2_DLSPEC static void create_detached (
                const std::string& filename, const std::string& cmdline = "");

            K2_DLSPEC explicit process (
                const std::string& filename, const std::string& cmdline = "");
            K2_DLSPEC ~process ();

        private:
            nonpublic::process_impl*    m_pimpl;
        };
    }   //  namespace trial

}   //  namespace k2

#endif  //  K2_PROCESS_H
