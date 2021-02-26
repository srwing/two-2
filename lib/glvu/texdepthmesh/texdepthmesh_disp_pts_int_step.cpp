//------------------------------------------------------------------------------
// File : texdepthmesh_disp_pts_int_step.cpp
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
// texdepthmesh_disp_int_step.cpp : 
//   POINT-BASED DISPLAY ROUTINES (UNIFORM STEPSIZING : INTEGER STEP)
//============================================================================

#include "texdepthmesh.hpp"

void TexturedDepthMesh::DisplayAsPts(int StepSize)
{
 Begin_DrawSetup();
  int iD=0;
  glTranslatef(0.5,0.5,0);
  glBegin(GL_POINTS);
    for (int y=0; y<nRows; y+=StepSize)
    {
      iD = nCols*y;
      for (int x=0; x<nCols; x+=StepSize, iD+=StepSize)
        glVertex3f(x,y,Depth[iD]); 
    }
  glEnd();
 End_DrawSetup();
}

void TexturedDepthMesh::DisplayAsPtsWithColor(int StepSize)
{
 Begin_DrawSetup();
  int iC=0, iD=0;
  glTranslatef(0.5,0.5,0);
  glBegin(GL_POINTS);
    for (int y=0; y<nRows; y+=StepSize)
    {
      iD = nCols*y;
      iC = iD*3;
      for (int x=0; x<nCols; x+=StepSize, iD+=StepSize, iC+=(StepSize*3))
      {
        glColor3ubv(&(Color[iC]));
        glVertex3f(x,y,Depth[iD]); 
      }
    }
  glEnd();
 End_DrawSetup();
}
