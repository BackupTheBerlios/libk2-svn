/*  libk2   <process.cpp>

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
#include <k2/process.h>

#ifndef K2_SPIN_LOCK_H
#   include <k2/spin_lock.h>
#endif
#ifndef K2_STD_H_EXCEPTION
#   include <exception>
#   define  K2_STD_H_EXCEPTION
#endif
#ifndef K2_STD_H_MEMORY
#   include <memory>
#   define  K2_STD_H_MEMORY
#endif

#if defined(K2_RT_POSIX)
//  These headers don't need to be guarded,
//  because they are platform dependent,
//  therefore would not be included by any k2 headers.
#   include <sys/types.h>
#   include <unistd.h>
#   include <sys/wait.h>
#   include <ctype.h>
#endif  //  K2_RT_POSIX

#if defined(K2_RT_MSVCRT)
//  Same here, platform dependent.
#   include <windows.h>
#endif  //  K2_RT_MSVCRT

namespace k2
{

    using std::string;
    using std::auto_ptr;

    namespace nonpublic
    {
        class process_impl
        {
        private:
#if defined(K2_RT_POSIX)
            typedef ::pid_t     native_id_t;
            typedef native_id_t native_handle_t;
#endif  //  K2_RT_POSIX
#if defined(K2_RT_MSVCRT)
            typedef ::DWORD     native_id_t;
            typedef ::HANDLE    native_handle_t;
#endif  //  K2_RT_MSVCRT

            static auto_ptr<process_impl>
                            create (const string& executable, const string& param, bool detached_state);

        public:
            static void     create_detached (const string& executable, const string& param);
            static auto_ptr<process_impl>
                            create_attached (const string& executable, const string& param);
            static void     join_and_destroy (auto_ptr<process_impl>);

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
        auto_ptr<process_impl> process_impl::create (const string& filename, const string& cmdline, bool detached_state)
        {
#if defined(K2_RT_POSIX)
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
                    //first = find_if(first, end, notspace);
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
#endif  //  K2_RT_POSIX

#if defined(K2_RT_MSVCRT)
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
                return  auto_ptr<process_impl>(new process_impl(pinf.dwProcessId, pinf.hProcess));
#endif  //  K2_RT_MSVCRT

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
#if defined(K2_RT_POSIX)
            ::waitpid(impl_ap->m_id, 0, 0);
#endif  //  K2_RT_POSIX

#if defined(K2_RT_MSVCRT)
            ::WaitForSingleObject(impl_ap->m_handle, INFINITE);
            ::CloseHandle(impl_ap->m_handle);
#endif  //  K2_RT_MSVCRT
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

}   //  namespace k2
