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
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * APARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <k2/process.h>

#ifndef K2_EXCEPTION_H
#   include <k2/exception.h>
#endif
#ifndef K2_STD_H_MEMORY
#   include <memory>
#   define  K2_STD_H_MEMORY
#endif

#ifndef K2_BITS_OS_H
#   include <k2/bits/os.h>
#endif

#if defined(K2_OS_UNIX)
#   include <sys/types.h>
#   include <unistd.h>
#   include <sys/wait.h>
#   include <ctype.h>
#elif defined(K2_OS_WIN32)
#   include <windows.h>
#else
#   error   "libk2: Manual attention required."
#endif

namespace k2
{

    namespace trail
    {

        using std::string;
        using std::auto_ptr;

        namespace nonpublic
        {
            class process_impl
            {
            private:
#if defined(K2_OS_UNIX)
                typedef ::pid_t     native_id_t;
                typedef native_id_t native_handle_t;
#elif defined(K2_OS_WIN32)
                typedef ::DWORD     native_id_t;
                typedef ::HANDLE    native_handle_t;
#endif

                static auto_ptr<process_impl>
                                create (
                                    const string& executable,
    				const string& param,
    				bool detached_state);

            public:
                static void create_detached (
                    const string& executable, const string& param);
                static auto_ptr<process_impl> create_attached (
                    const string& executable, const string& param);
                static void join_and_destroy (auto_ptr<process_impl>);

            private:
                explicit process_impl (native_id_t id, native_handle_t handle);
                native_id_t     m_id;
                native_handle_t m_handle;
            };

            process_impl::process_impl (native_id_t id, native_handle_t handle)
            :   m_id(id)
            ,   m_handle(handle)
            {
            }

            bool notspace (int ch)
            {
                return  !isspace(ch);
            }

            //  static
            auto_ptr<process_impl> process_impl::create (
    	    const string& filename, const string& cmdline, bool detached_state)
            {
#if defined(K2_OS_UNIX)
                native_id_t pid = ::fork();

                if (pid == 0)
                {
                    //  child process
                    const size_t MAX_ARGC = 64;
                    char*   argv[MAX_ARGC];
                    argv[0] = (char*)calloc(filename.length() + 1, 0);
                    copy(filename.begin(), filename.end(), argv[0]);
                    string::const_iterator  first= filename.begin();
                    string::const_iterator  last;
                    string::const_iterator  end = filename.end();

                    size_t idx = 1;
                    while (first != end)
                    {
                        if (first == end)
                            break;
                        last = find_if(first, end, isspace);
                        argv[idx] = (char*)calloc(last - first + 1, 0);
                        copy(first, last, argv[idx]);
                        argv[idx][last - first] = 0;
                        ++idx;
                    }
                    argv[idx] = 0;

                    if (::execvp(filename.c_str(), argv) == -1)
                        throw   runtime_error("Unable to exec().");
                }
                else
                {
                    //  parent process
                    if (detached_state == true)
                        return  auto_ptr<process_impl>(0);
                    else
                        return  auto_ptr<process_impl>(new process_impl(pid, pid));
                }
#elif defined(K2_OS_WIN32)
                STARTUPINFO         sinf;
                memset(&sinf, 0, sizeof(sinf));
                sinf.cb = sizeof(STARTUPINFO);

                PROCESS_INFORMATION pinf;
                memset(&pinf, 0, sizeof(pinf));

                BOOL ret =  ::CreateProcess(filename.c_str(),
                                            const_cast<char*>(cmdline.c_str()),
                                            0,          //  process security attributes
                                            0,          //  thread security attributes
                                            FALSE,      //  inherit handles
                                            0,          //  creation flags
                                            0,          //  environment varables
                                            0,          //  child process's cwd
                                            &sinf,
                                            &pinf);

                if (ret == FALSE)
                    throw   runtime_error("Failed to create process.");

                //  Don't need main thread's handle from the child process,
                //  hence close it.
                //  Note:   This is not terminating nor cancelling the thread.
                //          Just returning the handle to WIN32.
                ::CloseHandle(pinf.hThread);

                if (detached_state == true)
                {
                    //  Don't need from the child process's handle,
                    //  hence close it.
                    //  Note:   This is not terminating the process.
                    //          Just returning the handle to WIN32.
                    ::CloseHandle(pinf.hProcess);
                    return  auto_ptr<process_impl>(0);
                }
                else
                {
                    return  auto_ptr<process_impl>(new process_impl(pinf.dwProcessId, pinf.hProcess));
                }
#endif

                return  auto_ptr<process_impl>(0);
            }
            //  static
            void process_impl::create_detached (const string& filename, const string& cmdline)
            {
                process_impl::create (filename, cmdline, true);
            }
            //  static
            auto_ptr<process_impl> process_impl::create_attached (const string& filename, const string& cmdline)
            {
                return  process_impl::create (filename, cmdline, false);
            }
            //  static
            void process_impl::join_and_destroy (auto_ptr<process_impl> impl_ap)
            {
#if defined(K2_OS_UNIX)
                ::waitpid(impl_ap->m_id, 0, 0);
#elif defined(K2_OS_WIN32)
                ::WaitForSingleObject(impl_ap->m_handle, INFINITE);
                ::CloseHandle(impl_ap->m_handle);
#endif
            }

        }

        using namespace nonpublic;

        //  static
        void process::create_detached (const string& filename, const string& cmdline)
        {
            process_impl::create_detached(filename, cmdline);
        }
        process::process (const string& filename, const string& cmdline)
        :   m_pimpl(0)
        {
            std::auto_ptr<process_impl> ap = process_impl::create_attached(filename, cmdline);
            m_pimpl = ap.release();
        }
        process::~process ()
        {
            std::auto_ptr<process_impl> ap(m_pimpl);
            m_pimpl = 0;
            process_impl::join_and_destroy(ap);
        }

    }   //  namespace trial

}   //  namespace k2
