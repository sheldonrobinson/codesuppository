/*----------------------------------------------------------------------
		Copyright (c) 2004 Open Dynamics Framework Group
					www.physicstools.org
		All rights reserved.

		Redistribution and use in source and binary forms, with or without modification, are permitted provided
		that the following conditions are met:

		Redistributions of source code must retain the above copyright notice, this list of conditions
		and the following disclaimer.

		Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

		Neither the name of the Open Dynamics Framework Group nor the names of its contributors may
		be used to endorse or promote products derived from this software without specific prior written permission.

		THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES,
		INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
		DISCLAIMED. IN NO EVENT SHALL THE INTEL OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
		EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
		LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
		IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
		THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

#include "UserMemAlloc.h"

#if defined(WIN32)
#define NOMINMAX
#include <windows.h>
#include <mmsystem.h>

#pragma comment( lib, "winmm.lib" )
#pragma warning( disable: 4996)
#else
#include "linux_compat.h"
#endif

#if defined(_XBOX)
#include"NxXBOX.h"
#endif


#include "clock.h"
#include "cycle.h"

namespace CLOCK
{


#if defined(_XBOX)
//-- modified from xbox sample code
struct TimeInfo
{
	NxU64 qwTime;
	NxU64 qwAppTime;
	NxF32 fAppTime;
	NxF32 fElapsedTime;
	NxF32 fSecsPerTick;
}gTimeInfo;
#endif





void getDateTime(char *da,char *tim)
{
	time_t ltime;
	struct tm *today;

	_tzset();
	time( &ltime );
	today = localtime( &ltime );

	strftime( da, 128,"%A, %B %d, %Y", today );
	strftime( tim, 128,"%I:%M %p",     today );
}

const char * getProcessorAscii(void) // get the name of the processor as ascii
{
	static char buf[256];

	strcpy(buf,"unknown");

#if WIN32
	__asm
	{
		// CPUID functions 80000002h, 80000003h, 80000004h return the brand string. -ENS
		lea edi, [buf];

		mov eax, 80000002h;
		cpuid;

		mov [edi],		eax;
		mov [edi+4],	ebx;
		mov [edi+8],	ecx;
		mov [edi+12], edx;

		add edi, 16;

		mov eax, 80000003h;
		cpuid;

		mov [edi],		eax;
		mov [edi+4],	ebx;
		mov [edi+8],	ecx;
		mov [edi+12], edx;

		add edi, 16;

		mov eax, 80000004h;
		cpuid;

		mov [edi],		eax;
		mov [edi+4],	ebx;
		mov [edi+8],	ecx;
		mov [edi+12], edx;

		mov byte ptr[edi+16], 0;
	}
#endif


	return buf;

}


NxU64 getCPUFrequency(NxU32 /*uiMeasureMSecs*/)
{
  NxU64 freq=0;
#ifdef WIN32
	QueryPerformanceFrequency((LARGE_INTEGER *) &freq);
#endif
	return freq;
}

NxU64       getClockSpeed(void)     // report the clock speed of the processor.
{
	return getCPUFrequency(10);
}

static NxU64 gProcessorSpeed=0;       // speed of the processor
static NxU64 gLastTimeRTDSC=0;        // the last value of our call to RTDSC
static NxF32  gProcessorRecip=0;
static NxF32  gCurrentTime=0;
static NxF32 gDtime;

void init(void)
{
  gProcessorSpeed = getClockSpeed();
  gProcessorRecip = 1.0f / (NxF32) gProcessorSpeed;
  gCurrentTime    = 0;
  QueryPerformanceCounter((LARGE_INTEGER *) &gLastTimeRTDSC);
  //gLastTimeRTDSC  = Cycle::getRTDSC();
}

NxF32       doClockFrame(void)   // indicate a new frame, compute and return the delta time value.
{
  NxF32 ret = 0;

  if ( gProcessorSpeed == 0 )
    init();

  NxU64 cycle;
  NxU64 cdiff;
	QueryPerformanceCounter((LARGE_INTEGER *) &cycle);
  if ( cycle >= gLastTimeRTDSC )
  {
    cdiff = cycle - gLastTimeRTDSC;
  }
  else // the CPU counter has rolled over since the last time.
  {
    NxU64 maxv = (NxU64)0xFFFFFFFFFFFFFFFFLL;
    cdiff = (maxv-gLastTimeRTDSC)+cycle;
  }
  ret = gDtime = (NxF32)cdiff*gProcessorRecip;
  gCurrentTime+=gDtime; // advance our total time counter.
  gLastTimeRTDSC        = cycle;

  return ret;
}


NxF32 getSystemTime(void)
{
  DWORD t = timeGetTime();
  NxF32 ret = (NxF32)t*(1.0f/1000.0f);
  return ret;
}

NxU32 getSystemTimeMS(void)
{
  DWORD t = timeGetTime();
  return t;
}


NxF32        getCurrentTime(void)
{
  return gCurrentTime;
}

NxF32        getDeltaTime(void)   // get the delta frame time.
{
  return gDtime;
}

NxU64       getCounter(void) // get the current performance counter.
{
  NxU64 ret = 0;

	QueryPerformanceCounter((LARGE_INTEGER *) &ret);

  return ret;
}

NxF32        getCounterDifference(NxU64 stime) // compute the difference and return it as a floating point number.
{

  NxU64 ctime;

	QueryPerformanceCounter((LARGE_INTEGER *) &ctime);

  NxU64 cdiff = (ctime-stime);

  NxF32 ret = (NxF32)cdiff*gProcessorRecip;

  return ret;

}

}; // end of namespace