//------------------------------------------------------------------------------
// File : texdepthmesh_disp_mesh_int_step.cpp
//------------------------------------------------------------------------------
// GLVU : Copyright 1997 - 2002 
//        The University of North Carolina at Chapel Hill
//------------------------------------------------------------------------------
// Permission to use, copy, modify, distribute and sell this software and its 
// documentation for any purpose is hereby granted without fee, provided that 
// the above copyright notice appear in all copies and that both that copyright 
// notice and this permission notice appear in supporting documentation. 
// Binaries may be compiled with this software without any royalties or 
// restrictions. 
//
// The University of North Carolina at Chapel Hill makes no representations 
// about the suitability of this software for any purpose. It is provided 
// "as is" without express or implied warranty.

//============================================================================
// texdepthmesh_disp_mesh_int_step.cpp : 
//   MESH-BASED DISPLAY ROUTINES (UNIFORM STEPSIZING : INTEGER STEP)
//============================================================================

#include "texdepthmesh.hpp"

void TexturedDepthMesh::DisplayAsMesh(int StepSize)
{
 Begin_DrawSetup();
  glTranslatef(0.5,0.5,0);

  int NextY, iD1, iD2;
  for (int y=0; y<(nRows-1); y+=StepSize)
  {
    NextY = y+StepSize;
    if (NextY >= nRows) NextY=nRows-1;

    iD1=nCols*y;
    iD2=nCols*NextY;

    glBegin(GL_TRIANGLE_STRIP);

     int x;
     for (x=0; x<nCols; x+=StepSize)
     {
       glVertex3f(x,NextY,Depth[iD2]); 
       glVertex3f(x,y,Depth[iD1]); 

       iD1+=StepSize;
       iD2+=StepSize;
     }

     int ExtraX = x - (nCols-1);
     if (ExtraX > 0)
     {
       glVertex3f(nCols-1,NextY,Depth[iD2-ExtraX]); 
       glVertex3f(nCols-1,y,Depth[iD1-ExtraX]); 
     }

    glEnd();
  }
 End_DrawSetup();
}

void TexturedDepthMesh::DisplayAsMeshWithColor(int StepSize)
{
 Begin_DrawSetup();
  glTranslatef(0.5,0.5,0);

  int NextY, iD1, iC1, iD2, iC2;
  for (int y=0; y<(nRows-1); y+=StepSize)
  {
    NextY = y+StepSize;
    if (NextY >= nRows) NextY=nRows-1;

    iD1=nCols*y;
    iC1=nCols*y*3;
    iD2=nCols*NextY;
    iC2=nCols*NextY*3;

    glBegin(GL_TRIANGLE_STRIP);

     int x;
     for (x=0; x<nCols; x+=StepSize)
     {
       glColor3ubv(&(Color[iC2]));
       glVertex3f(x,NextY,Depth[iD2]); 
       glColor3ubv(&(Color[iC1]));
       glVertex3f(x,y,Depth[iD1]); 

       iD1+=StepSize;
       iC1+=(StepSize*3);
       iD2+=StepSize;
       iC2+=(StepSize*3);
     }

     int ExtraX = x - (nCols-1);
     if (ExtraX > 0)
     {
       glColor3ubv(&(Color[iC2-(ExtraX*3)]));
       glVertex3f(nCols-1,NextY,Depth[iD2-ExtraX]); 
       glColor3ubv(&(Color[iC1-(ExtraX*3)]));
       glVertex3f(nCols-1,y,Depth[iD1-ExtraX]); 
     }

    glEnd();
  }
 End_DrawSetup();
}

//-----------------------------------------------------------------------------
// Another multi-resolution meshing routine that allows for arbitary numbers
// of mesh pts. This routine creates a uniform mesh unlike the uniform
// variable step-size versions that "clip" on the boundaries.
//-----------------------------------------------------------------------------
void TexturedDepthMesh::DisplayAsMesh(int NumX, int NumY)
{
 Begin_DrawSetup();
  float dx = (nCols-1) / (float)(NumX-1);
  float dy = (nRows-1) / (float)(NumY-1);
  float x,y,nexty;
  int iD1,iD2;

  y=0.5;
  for (int iy=0; iy<(NumY-1); iy++, y+=dy)
  {
    x=0.5;
    nexty=y+dy;
    iD1=(int)y*nCols;
    iD2=(int)(y+dy)*nCols;
    glBegin(GL_TRIANGLE_STRIP);
     for (int ix=0; ix<NumX; ix++, x+=dx)
     {
       glVertex3f(x, nexty, Depth[ iD2+(int)x ]);
       glVertex3f(x, y, Depth[ iD1+(int)x ]);
     }
    glEnd();
  }
 End_DrawSetup();
}
