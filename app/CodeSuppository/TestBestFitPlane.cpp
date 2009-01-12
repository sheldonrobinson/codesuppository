#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "TestBestFitPlane.h"
#include "RenderDebug/RenderDebug.h"
#include "common/snippets/FloatMath.h"
#include "common/snippets/rand.h"

#include "common/snippets/SendTextMessage.h"

void testBestFitPlane(void)
{
  gRenderDebug->Reset();
  SEND_TEXT_MESSAGE(0,"Demonstration of Best Fit Plane to a set of points, originally published on March 23, 2006\r\n");

  float quat[4];
  fm_eulerToQuat(ranf()*FM_PI,ranf()*FM_PI,ranf()*FM_PI,quat);
  float matrix[16];
  fm_quatToMatrix(quat,matrix);

  #define PCOUNT 60
  float points[PCOUNT*3];
  float *pos = points;

  for (int i=0; i<PCOUNT; i++)
  {

    float t[3] = { pos[0], pos[1], pos[2] };
    pos[0] = (float) ((rand()&63)-32)*0.15f;
    pos[1] = (float) ((rand()&63)-32)*0.0015f;
    pos[2] = (float) ((rand()%63)-32)*0.05f;
    fm_transform(matrix,pos,t);
    gRenderDebug->DebugPoint(t,0.1f,0xFFFFFF,15.0f);

    pos[0] = t[0];
    pos[1] = t[1];
    pos[2] = t[2];

    pos+=3;
  }

  {
    float plane[4];

    fm_computeBestFitPlane(PCOUNT,points,sizeof(float)*3,0,0,plane);

    gRenderDebug->DebugPlane(plane,4,4,0xFFFF00,15.0f);

  }
}
