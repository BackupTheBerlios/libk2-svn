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
#ifndef	K2_BITS_COPY_BOUNCER_H
#define	K2_BITS_COPY_BOUNCER_H

namespace k2
{

	struct copy_bouncer
	{
		copy_bouncer () {}

	private:
		//	Deliberately not implemented.
		copy_bouncer (
			const copy_bouncer&);
		copy_bouncer& operator= (const copy_bouncer&);
	};

}	//	namespace k2

/**	\defgroup	Utility
*/

/**
*	\ingroup    Utility
*	\brief      Macro that inserts a non-copyable member into a class.
*/
//  !kh!
//  Although user may define K2_DISABLE_COPY_BOUNCER (save memory?),
//  it's highly recommended not to do so.
//  It's just not worth sacrificing the benifits to save sizeof(size_t)
//  bytes in non-value-sematic objects.
//  This option may be unavailable in future versions.
//  The rationale of using this macro instead of using boost alike
//  noncopyable is to avoid inheritance, this may not the best
//  technique in most senarios, but it's my preferred practice.
#if defined(K2_DISABLE_COPY_BOUNCER)
#   define  K2_INJECT_COPY_BOUNCER()
#else
#   define	K2_INJECT_COPY_BOUNCER()\
	    k2::copy_bouncer copy_bouncer
#endif

#endif	//	!K2_BITS_COPY_BOUNCER_H
