//------------------------------------------------------------------------------
// File : envmapcube.cpp
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
// envmapcube.cpp : environment-mapped cube (with depth)
//============================================================================

#include "envmapcube.hpp"
#include <GL/glut.h>
#include <minmaxbox.hpp>
#include <camutils.hpp>
#include <texdepthmesh.hpp>
#include <iostream.h>

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EnvMapCube::EnvMapCube(Object *Obj, Vec3f cntr, float halfdim, int n)
{
  Cntr = cntr;
  HalfDim = halfdim;
  N=n;
  VisibleFaceMask = ALLFACES;

  CreateFaceTDMs(Obj);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EnvMapCube::EnvMapCube(char *FileName)       // READ MAP FROM A FILE
{
  VisibleFaceMask = ALLFACES;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
EnvMapCube::~EnvMapCube()
{
  for (int i=0; i<6; i++)
    if (TDMs[i] != NULL)
      delete TDMs[i];
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EnvMapCube::CreateFaceTDMs(Object *Obj)
{
  // EnvMapCube AXES AND MIN/MAX
  Vec3f X(1,0,0), Y(0,1,0), Z(0,0,1);
  Vec3f m(Cntr.x-HalfDim,Cntr.y-HalfDim,Cntr.z-HalfDim),
        M(Cntr.x+HalfDim,Cntr.y+HalfDim,Cntr.z+HalfDim);

  Camera Cam;

  // POS/NEG X FACES (USES Y-UP)
  if (Obj->Max.x > M.x)
  { 
    Cam.Perspective(90,1,HalfDim,Obj->Max.x-Cntr.x);
    Cam.LookAt(Cntr,Cntr+X,Y); 
    AdjustCameraForBoundaryStitching(&Cam); 
    TDMs[0] = new TexturedDepthMesh(Obj,&Cam,N,N);
  }
  else // these elses added MJH 3/23/99
    TDMs[0] = NULL;

  if (Obj->Min.x < m.x)
  { 
    Cam.Perspective(90,1,HalfDim,Obj->Min.x-Cntr.x);
    Cam.LookAt(Cntr,Cntr-X,Y);
    AdjustCameraForBoundaryStitching(&Cam); 
    TDMs[1] = new TexturedDepthMesh(Obj,&Cam,N,N);
  }
  else
    TDMs[1] = NULL;

  // POS/NEG Y FACES (USES Z-UP)
  if (Obj->Max.y > M.y)
  { 
    Cam.Perspective(90,1,HalfDim,Obj->Max.y-Cntr.y);
    Cam.LookAt(Cntr,Cntr+Y,Z);
    AdjustCameraForBoundaryStitching(&Cam); 
    TDMs[2] = new TexturedDepthMesh(Obj,&Cam,N,N);
  }
  else
    TDMs[2] = NULL;
    
  if (Obj->Min.y < m.y)
  {
    Cam.Perspective(90,1,HalfDim,Obj->Min.y-Cntr.y);
    Cam.LookAt(Cntr,Cntr-Y,Z);
    AdjustCameraForBoundaryStitching(&Cam); 
    TDMs[3] = new TexturedDepthMesh(Obj,&Cam,N,N);
  }
  else
    TDMs[3] = NULL;

  // POS/NEG Z FACES (USES Y-UP)
  if (Obj->Max.z > M.z)
  { 
    Cam.Perspective(90,1,HalfDim,Obj->Max.z-Cntr.z);
    Cam.LookAt(Cntr,Cntr+Z,Y);
    AdjustCameraForBoundaryStitching(&Cam); 
    TDMs[4] = new TexturedDepthMesh(Obj,&Cam,N,N);
  }
  else
    TDMs[4] = NULL;
  
  if (Obj->Min.z < m.z)
  {
    Cam.Perspective(90,1,HalfDim,Obj->Min.z-Cntr.z);
    Cam.LookAt(Cntr,Cntr-Z,Y);
    AdjustCameraForBoundaryStitching(&Cam); 
    TDMs[5] = new TexturedDepthMesh(Obj,&Cam,N,N);
  }
  else
    TDMs[5] = NULL;

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EnvMapCube::Write(char *FileName)
{
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EnvMapCube::Display(int UseMesh, int StepSize, int UseProjTex,
			 int UseDisplayLists)
{
  for (int i=0; i<6; i++) {
    if (TDMs[i] && (VisibleFaceMask & (1<<i))) {
      TDMs[i]->UseProjTex = UseProjTex;
      TDMs[i]->UseDisplayLists = UseDisplayLists;
      if (UseMesh) {
        if (TDMs[i]->UseProjTex) {
          if (StepSize==1) TDMs[i]->DisplayAsMesh();
	  else TDMs[i]->DisplayAsMesh(StepSize);
	}
        else {
          if (StepSize==1) TDMs[i]->DisplayAsMeshWithColor();
	  else TDMs[i]->DisplayAsMeshWithColor(StepSize);
	}
      }
      else {
        if (TDMs[i]->UseProjTex) {
          if (StepSize==1) TDMs[i]->DisplayAsPts();
	  else TDMs[i]->DisplayAsPts(StepSize);
	}
        else {
          if (StepSize==1) TDMs[i]->DisplayAsPtsWithColor();
	  else TDMs[i]->DisplayAsPtsWithColor(StepSize);
	}
      }
    }
  }
}

//-----------------------------------------------------------------------------
// Connecting the mesh between adjacent cameras results in a missing "half-
// pixel" from each mesh. This fix widens the field of view by a half-pixel
// on each side (top, bottom, left, and right) so that the adjacent vertices
// are "shared". NOTE: only necessary for mesh display, pt sets do not need this.
//-----------------------------------------------------------------------------
void EnvMapCube::AdjustCameraForBoundaryStitching(Camera *Cam)
{
  float HalfPixel = (Cam->wT - Cam->wB) / (float)(2*N);
  Cam->wT += HalfPixel;
  Cam->wB -= HalfPixel;
  Cam->wR += HalfPixel;
  Cam->wL -= HalfPixel;
}

//-----------------------------------------------------------------------------
// Given an axis-aligned bounding box (min,max) and a camera that is inside
// the box "looking out", determines the visible faces by setting
// the corresponding bits in the VisibleFaceMask (OR with bitmasks to find).
//-----------------------------------------------------------------------------
void EnvMapCube::FindVisibleCubeFaces(Camera *Cam)
{
  float CamFarDist = Cam->Far * (float)0.577350269;  // SCALE BY FAR^(1/3)
  float m[3] = {Cntr.x-CamFarDist,Cntr.y-CamFarDist,Cntr.z-CamFarDist};
  float M[3] = {Cntr.x+CamFarDist,Cntr.y+CamFarDist,Cntr.z+CamFarDist};

  float V[8][3];
  GetMinMaxBoxVerts(m,M,V);
  
  VisibleFaceMask = 0;
  
  if ( CamQuadOverlap(Cam,V[0],V[3],V[7],V[4]) ) VisibleFaceMask |= PXFACE;
  if ( CamQuadOverlap(Cam,V[1],V[5],V[6],V[2]) ) VisibleFaceMask |= NXFACE;
  if ( CamQuadOverlap(Cam,V[4],V[5],V[1],V[0]) ) VisibleFaceMask |= PYFACE;
  if ( CamQuadOverlap(Cam,V[3],V[2],V[6],V[7]) ) VisibleFaceMask |= NYFACE;
  if ( CamQuadOverlap(Cam,V[0],V[1],V[2],V[3]) ) VisibleFaceMask |= PZFACE;
  if ( CamQuadOverlap(Cam,V[5],V[4],V[7],V[6]) ) VisibleFaceMask |= NZFACE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EnvMapCube::SetAllFacesVisible()
{
  VisibleFaceMask = ALLFACES;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EnvMapCube::DisplayCube(float Color[3])
{
  float m[3]={Cntr.x-HalfDim,Cntr.y-HalfDim,Cntr.z-HalfDim};
  float M[3]={Cntr.x+HalfDim,Cntr.y+HalfDim,Cntr.z+HalfDim};
  float V[8][3];
  GetMinMaxBoxVerts(m,M,V);

  glPushAttrib(GL_LIGHTING_BIT);
  glDisable(GL_LIGHTING);
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(1.0);
  glColor3fv(Color);

  glBegin(GL_LINE_LOOP);  // TOP FACE
    glVertex3fv(V[4]); glVertex3fv(V[5]); glVertex3fv(V[1]); glVertex3fv(V[0]);
  glEnd();
  glBegin(GL_LINE_LOOP);  // BOTTOM FACE
    glVertex3fv(V[3]); glVertex3fv(V[2]); glVertex3fv(V[6]); glVertex3fv(V[7]); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // LEFT FACE
    glVertex3fv(V[1]); glVertex3fv(V[5]); glVertex3fv(V[6]); glVertex3fv(V[2]); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // RIGHT FACE
    glVertex3fv(V[0]); glVertex3fv(V[3]); glVertex3fv(V[7]); glVertex3fv(V[4]); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // NEAR FACE
    glVertex3fv(V[1]); glVertex3fv(V[2]); glVertex3fv(V[3]); glVertex3fv(V[0]); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // FAR FACE
    glVertex3fv(V[4]); glVertex3fv(V[7]); glVertex3fv(V[6]); glVertex3fv(V[5]); 
  glEnd();

  glPopAttrib();
  glPopAttrib();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void EnvMapCube::PrintBits(unsigned int x)
{
  for (unsigned int i=0; i<32; i++)
    if ((x<<i) & 2147483648) putchar('1'); else putchar('0');
  putchar('\n');
}
