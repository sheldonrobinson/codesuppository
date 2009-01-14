#include "common/snippets/UserMemAlloc.h"
#include "common/snippets/JobSwarm.h"
#include "comlayer.h"

/*!

**

** Copyright (c) 2007 by John W. Ratcliff mailto:jratcliff@infiniplex.net

**

** Portions of this source has been released with the PhysXViewer application, as well as

** Rocket, CreateDynamics, ODF, and as a number of sample code snippets.

**

** If you find this code useful or you are feeling particularily generous I would

** ask that you please go to http://www.amillionpixels.us and make a donation

** to Troy DeMolay.

**

** DeMolay is a youth group for young men between the ages of 12 and 21.

** It teaches strong moral principles, as well as leadership skills and

** public speaking.  The donations page uses the 'pay for pixels' paradigm

** where, in this case, a pixel is only a single penny.  Donations can be

** made for as small as $4 or as high as a $100 block.  Each person who donates

** will get a link to their own site as well as acknowledgement on the

** donations blog located here http://www.amillionpixels.blogspot.com/

**

** If you wish to contact me you can use the following methods:

**

** Skype Phone: 636-486-4040 (let it ring a long time while it goes through switches)

** Skype ID: jratcliff63367

** Yahoo: jratcliff63367

** AOL: jratcliff1961

** email: jratcliff@infiniplex.net

**

**

** The MIT license:

**

** Permission is hereby granted, MEMALLOC_FREE of charge, to any person obtaining a copy

** of this software and associated documentation files (the "Software"), to deal

** in the Software without restriction, including without limitation the rights

** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell

** copies of the Software, and to permit persons to whom the Software is furnished

** to do so, subject to the following conditions:

**

** The above copyright notice and this permission notice shall be included in all

** copies or substantial portions of the Software.


** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,

** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE

** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,

** WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN

** CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


*/




#include "../include/common/snippets/cparser.h"

#pragma warning(disable:4100)
#pragma warning(disable:4189)

SendTextMessage        *gSendTextMessage=0;
JOB_SWARM::JobSwarmContext *gJobSwarmContext=0;

bool gBadState=false;

const char *gBadFile=0;

HeU32 gBadLine=0;




enum ComLayerCommand

{

  CLC_MEMORY_REPORT  = 1000,

};


ComLayer::ComLayer(void)

{

  mClient = 0;

  mProcessed = false;

  if ( gTheCommandParser == 0 )

  {

    gTheCommandParser = MEMALLOC_NEW(TheCommandParser);

    AddToken("ClcMemoryReport", CLC_MEMORY_REPORT );
  }



}


ComLayer::~ComLayer(void)

{

  

//  delete gTheCommandParser;

//  gTheCommandParser = 0;

}


HeU32 getHeapSize(HeU32 &unused)

{

  return MEMALLOC_GET_HEAP_SIZE(unused);

}


HeI32 ComLayer::CommandCallback(HeI32 token,HeI32 /* count */,const char ** /* arglist */)

{

  HeI32 ret = 0;


  SendTextMessage *stm = gSendTextMessage;


  switch ( token )

  {

    case CLC_MEMORY_REPORT:

      MEMALLOC_REPORT(mDescription.Get(), stm, true);

      break;

  }


  return ret;

}




void ComLayer::frameBegin(SendTextMessage *stm)

{

  MEMALLOC_FRAME_BEGIN(stm);

}


void ComLayer::frameEnd(SendTextMessage *stm,const char *header)

{

  MEMALLOC_FRAME_END(stm,header);

}


