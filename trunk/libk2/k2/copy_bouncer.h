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
#ifndef	K2_COPY_BOUNCER_H
#define	K2_COPY_BOUNCER_H

#ifndef	K2_CONFIG_H
#	include <k2/config.h>
#endif

namespace k2
{

	struct default_copy_bouncer
	{
		default_copy_bouncer () {}

	private:
		//	Deliberately not implemented.
		default_copy_bouncer (
			const default_copy_bouncer&);
		default_copy_bouncer& operator= (const default_copy_bouncer&);
	};

}	//	namespace k2


/**	\defgroup	Utility
*/

/**
*	\ingroup	Utility
*	\brief		Macro that inserts a non-copyable member into a class.
*/
#define	K2_INJECT_COPY_BOUNCER()\
	k2::default_copy_bouncer copy_bouncer

#endif	//	!K2_COPY_BOUNCER_H
