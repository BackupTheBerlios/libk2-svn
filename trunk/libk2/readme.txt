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

Directory structure:
    libk2/
      Root directory for libk2

    libk2/k2/
      Public header files

    libk2/k2/nonpublic/
      Non-public header files
      Do NOT include directly

    libk2/source/
      Source files

    libk2/test/
      Test suit application

    libk2/win32.vc.net/
      Root for VC.net build files, binaries and etc.

    libk2/win32.vc.net/bin.opt/
      Optimized (release) version of intermediate and output files

    libk2/win32.vc.net/bin.debug/
      Debug version of intermediate and output files

    libk2/win32.vc.net/build/
      VC.net solution, project files and Doxyfile

    libk2/win32.vc.net/contrib/include
      3rd party headers libk2 needs to build with VC.net

    libk2/win32.vc.net/contrib/lib
      3rd party libraries libk2 needs to build with VC.net

    libk2/win32.vc.net/html/
      Doxygen-ed documents of win32.vc.net builds
      Not included in snapshot

Build instructions on Windows with VC.net:
    1.  Presumably you already checked out lastest release of libk2 from either
        svnhosting.org or sourceforge.net.
    2.  If your snapshot doesn't include Pthreads for Win32, download it from
        ftp://sources.redhat.com/pub/pthreads-win32/dll-latest/ or
        visit http://sources.redhat.com/pthreads-win32/ for more information.
    3.  Place all header files .../dll-latest/include/ in
        libk2/win32.vc.net/depend/include.
    4.  Place .../dll-latest/lib/pthreadVC.lib in libk2/win32.vc.net/depend/lib/.
    5.  Place .../dll-latest/lib/pthreadVC.dll in libk2/win32.vc.net/working/.
    6.  Open libk2/win32.vc.net/build/k2.sln.
    7.  You should have no problem building. In case if you do get any problem
        regarding libk2, you could try reach me at kenho@bluebottle.com or
        kenho@user.sourceforge.net. When emailing, please put something
        meaningful and try to be specific on its subject, or I would probably
        treat it as another spam mail :-).
