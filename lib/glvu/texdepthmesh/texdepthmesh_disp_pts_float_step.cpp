//------------------------------------------------------------------------------
// File : texdepthmesh_disp_pts_float_step.cpp
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
// texdepthmesh_disp_float_step.cpp : 
//   POINT-BASED DISPLAY ROUTINES (NONUNIFORM STEPSIZING : FLOAT STEP)
//   For all pts to be displayed, a stepsize of 1 is chosen. Valid
//   values: 0<Xstep<nCols; 0<Ystep<nRows
//============================================================================

#include "texdepthmesh.hpp"

void TexturedDepthMesh::DisplayAsPts(float Xstep, float Ystep)
{
 if (Xstep>nCols || Ystep>nRows) return;
 Begin_DrawSetup();
  int iD;
  glTranslatef(0.5,0.5,0);
  glBegin(GL_POINTS);
    for (float y=0; y<nRows; y+=Ystep)
    {
      iD = (int)y*nCols;
      for (float x=0; x<nCols; x+=Xstep)
        glVertex3f(x,y,Depth[iD+(int)x]);
    }
  glEnd();
 End_DrawSetup();
}

void TexturedDepthMesh::DisplayAsPtsWithColor(float Xstep, float Ystep)
{
 if (Xstep>nCols || Ystep>nRows) return;
 Begin_DrawSetup();
  int iC, iD;
  glTranslatef(0.5,0.5,0);
  glBegin(GL_POINTS);
    for (float y=0; y<nRows; y+=Ystep)
    {
      iD = (int)y*nCols;
      iC = iD*3;
      for (float x=0; x<nCols; x+=Xstep)
      {
        glColor3ubv(&(Color[iC+((int)x)*3]));
        glVertex3f(x,y,Depth[iD+(int)x]); 
      }
    }
  glEnd();
 End_DrawSetup();
}
