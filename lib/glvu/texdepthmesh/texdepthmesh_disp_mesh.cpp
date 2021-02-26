//------------------------------------------------------------------------------
// File : texdepthmesh_disp_mesh.cpp
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
// texdepthmesh_disp_mesh.cpp : MESH-BASED DISPLAY ROUTINES (ALL POINTS)
//============================================================================

#include "texdepthmesh.hpp"

void TexturedDepthMesh::DisplayAsMesh()
{
 Begin_DrawSetup();
  glTranslatef(0.5,0.5,0);
  int iD=0;
  int iDstep=nCols;
  for (int y=0; y<(nRows-1); y++)
  {
    glBegin(GL_TRIANGLE_STRIP);
    for (int x=0; x<nCols; x++, iD++)
    {
      glVertex3f(x,y+1,Depth[iD+iDstep]);
      glVertex3f(x,y,Depth[iD]);
    }
    glEnd();
  }
 End_DrawSetup();
}

void TexturedDepthMesh::DisplayAsMeshWithColor()
{
 Begin_DrawSetup();
  glTranslatef(0.5,0.5,0);
  int iC=0, iD=0;
  int iCstep=nCols*3, iDstep=nCols;
  for (int y=0; y<(nRows-1); y++)
  {
    glBegin(GL_TRIANGLE_STRIP);
    for (int x=0; x<nCols; x++, iD++, iC+=3)
    {
      glColor3ubv(&(Color[iC+iCstep]));
      glVertex3f(x,y+1,Depth[iD+iDstep]);
      glColor3ubv(&(Color[iC]));
      glVertex3f(x,y,Depth[iD]);
    }
    glEnd();
  }
 End_DrawSetup();
}
