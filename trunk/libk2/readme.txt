/*	libk2

	Copyright (C) 2004 Kenneth Chang-Hsing Ho.

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

    libk2/win32.vc.net/depend/include
      3rd party headers libk2 needs to build with VC.net

    libk2/win32.vc.net/depend/lib
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
