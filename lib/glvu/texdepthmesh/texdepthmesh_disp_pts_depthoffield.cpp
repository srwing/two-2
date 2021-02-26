//------------------------------------------------------------------------------
// File : texdepthmesh_disp_pts_depthoffield.cpp
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
// texdepthmesh_disp_pts_depthoffield : 
//============================================================================

#include "texdepthmesh.hpp"

//----------------------------------------------------------------------------
// Display with "depth-of-field". User provides a focus distance (usually
// somewhere between the camera's, or TDM's, near and far plane distances)
// and an aperture size (in same camera space as near and far distances).
//   NearPtSize = ApertureSize*0.5*(FocusDist-near) / FocusDist
//   Slope = -(ApertureSize*0.5) / FocusDist
//   NewPtSize = NearPtSize + Slope * (Far-Near)*depth
//----------------------------------------------------------------------------
void TexturedDepthMesh::DisplayPtsWithDepthOfField(float FocusDist, float ApertureSize, int StepSize)
{
  #define ABS(x) (((x)<0)?(-(x)):x)

  float NearPtSize = ApertureSize*0.5*(FocusDist-Near) / FocusDist;
  float Slope = -(ApertureSize*0.5) / FocusDist;
  float dFarNear = Far-Near;
  float NearFar = Near*Far;
  float LinDepth, NewPtSize;

  Begin_DisplayState();

  int iC=0, iD=0;
  glTranslatef(0.5,0.5,0);
    for (int y=0; y<nRows; y+=StepSize)
    {
      iD = nCols*y;
      iC = iD*3;
      for (int x=0; x<nCols; x+=StepSize, iD+=StepSize, iC+=(StepSize*3))
      {
        LinDepth = NearFar / (Far - Depth[iD]*dFarNear);
        NewPtSize = NearPtSize + Slope * LinDepth;
        glPointSize(ABS(NewPtSize));
        glColor3ubv(&(Color[iC]));
        glBegin(GL_POINTS);
          glVertex3f(x,y,Depth[iD]); 
        glEnd();
      }
    }
   
  End_DisplayState();
}
