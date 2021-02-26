//------------------------------------------------------------------------------
// File : texdepthmesh_disp_pts_adaptive.cpp
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
// texdepthmesh_disp_pts_adaptive : 
//   This routine performs adaptive stepsizing for proper minification and
//   magnification reconstruction using points. Only the density of the points
//   is altered, NOT the point size. Using the given new desired camera view,
//   an appropriate stepsize is chosen, in X and Y independently.
// Computes a representative reference point in the center of the camera view
// at the average depth. This point is used to determine the projected
// area and "importance" of the TexturedDepthMesh into the new camera view.
// The projected area estimate is calculated for angle offsets in X and Y
// independently. The sqrt of the area estimate is used as the stepsizes.
//============================================================================

#include "texdepthmesh.hpp"

Vec3f TexturedDepthMesh::CalcRefPt()  // CENTER POINT AT AVERAGE DEPTH
{
  if (AvDepth<=0) CalcMinMaxAvDepths();
  float x=(wL+wR)*0.5, y=(wT+wB)*0.5;  // MIDPOINT ON VIEWPLANE WINDOW
  float Scale = AvDepth/Near;          // CALC POINT PROJECTED ONTO THE AVDEPTH PLANE
  Vec3f RefPt(x*Scale,y*Scale,-AvDepth);
  return(RefPt);
}

void TexturedDepthMesh::CalcStepSizes(Vec3f NewCOP, float *Xstep, float *Ystep)
{
  // 1) CALCULATE THE REFERENCE POINT
  Vec3f RefPt = CalcRefPt();

  // 2) XFORM NEW CAMERA VIEW CENTER-OF-PROJECTION INTO OLD VIEW
  NewCOP = WorldToCam(NewCOP);

  // 3) CREATE VECTORS: OLD COP TO REF PT, NEW COP TO REF PT
  Vec3f Old2R = RefPt;
  Vec3f New2R = RefPt - NewCOP;

  // 4) CALCULATE NEW AND OLD DISTANCES FROM REF PT
  float OldDist = Old2R.Length();
  float NewDist = New2R.Length();

float MinDist = AvDepth-MinDepth; // CLAMP NEW DIST TO A MIN DIST
float t = MaxDepth-AvDepth;
if (t<MinDist) MinDist=t;
if (NewDist<MinDist) NewDist=MinDist;

  // 5) CALCULATE DISTANCE AREA-SCALING
  float sqrtDistAreaScale = OldDist/NewDist;  // SQRT OF DIST AREA SCALE (AVOIDS SQRT)

float XangScale, YangScale;
if (NewDist>MinDist)
{
  // 6) PROJECT VECTORS ONTO XZ AND YZ PLANES AND NORMALIZE
  float l;
  float xzO2R[] = { Old2R.x, Old2R.z };
  float xzN2R[] = { New2R.x, New2R.z };
  l = (float)sqrt(xzO2R[0]*xzO2R[0] + xzO2R[1]*xzO2R[1]);
  xzO2R[0]/=l; xzO2R[1]/=l;
  l = (float)sqrt(xzN2R[0]*xzN2R[0] + xzN2R[1]*xzN2R[1]);
  xzN2R[0]/=l; xzN2R[1]/=l;
  float yzO2R[] = { Old2R.y, Old2R.z };
  float yzN2R[] = { New2R.y, New2R.z };
  l = (float)sqrt(yzO2R[0]*yzO2R[0] + yzO2R[1]*yzO2R[1]);
  yzO2R[0]/=l; yzO2R[1]/=l;
  l = (float)sqrt(yzN2R[0]*yzN2R[0] + yzN2R[1]*yzN2R[1]);
  yzN2R[0]/=l; yzN2R[1]/=l;

  // 7) CALCULATE INDEPENDENT XY ANGULAR AREA-SCALING; CLAMP TO [0,1]
  XangScale = xzO2R[0]*xzN2R[0] + xzO2R[1]*xzN2R[1];  // COS ANGLE BETWEEN Old2R AND New2R IN XZ-PLANE
  YangScale = yzO2R[0]*yzN2R[0] + yzO2R[1]*yzN2R[1];  // COS ANGLE BETWEEN Old2R AND New2R IN YZ-PLANE
  float XangRes = 1 / (float)nCols; 
  float YangRes = 1 / (float)nRows; 
  if (XangScale<XangRes) XangScale=XangRes;
  if (YangScale<YangRes) YangScale=YangRes;
}
else
{
  XangScale=YangScale=1;
}

  // 8) CALCULATE STEPSIZE SCALE BASED ON WINDOW RESOLUTION (TDM AND WINDOW RES DIFFER)
  float WW = glutGet((GLenum)GLUT_WINDOW_WIDTH);
  float WH = glutGet((GLenum)GLUT_WINDOW_HEIGHT);
  float XwinScale = nRows / WW;
  float YwinScale = nCols / WH;

  // 9) CALCULATE STEPSIZES FOR X AND Y BASED ON AREA SCALING FACTORS
  *Xstep = XwinScale / (sqrtDistAreaScale*XangScale);
  *Ystep = YwinScale / (sqrtDistAreaScale*YangScale);

//printf("sqrtDistAreaScale=%f\n",1/sqrtDistAreaScale);
//printf("XangScale=%f, YangScale=%f\n",1/XangScale,1/YangScale);
//printf("Xstep=%f, Ystep=%f\n",*Xstep,*Ystep);
}

void TexturedDepthMesh::DisplayAsPts_Adaptive(Camera *NewView)
{
  float Xstep, Ystep;
  CalcStepSizes(NewView->Orig, &Xstep, &Ystep);
  DisplayAsPts(Xstep,Ystep);
}

void TexturedDepthMesh::DisplayAsPtsWithColor_Adaptive(Camera *NewView)
{
glPushAttrib(GL_POINT_BIT);
glPointSize(5);
glColor3f(1,0,0);
glBegin(GL_POINTS);
  Vec3f RefPt = CalcRefPt();
  RefPt = CamToWorld(RefPt);
  glVertex3fv(&RefPt.x);
glEnd();
glPopAttrib();

  float Xstep, Ystep;
  CalcStepSizes(NewView->Orig, &Xstep, &Ystep);
  DisplayAsPtsWithColor(Xstep,Ystep);
}

/*
float CalcMagnificationPtSize(float *Xstep, float *Ystep)
{
  float MinStep = *Xstep;
  if (*Ystep < *Xstep) MinStep=*Ystep;
  if (MinStep>=1) return(1);
  if (*Xstep < 1) *Xstep=1;
  if (*Ystep < 1) *Ystep=1;
//printf("PointSize=%f\n",1/MinStep);
  return( 1/MinStep );
}

// WITH POINT ENLARGEMENT FOR MAGNIFICATION
void TexturedDepthMesh::DisplayAsPts_Adaptive(Camera *NewView)
{
  float Xstep, Ystep;
  CalcStepSizes(NewView->Orig, &Xstep, &Ystep);
float OldPtSize;
glGetFloatv(GL_POINT_SIZE,&OldPtSize);
glPointSize( CalcMagnificationPtSize(&Xstep,&Ystep) );
  DisplayAsPts(Xstep,Ystep);
glPointSize(OldPtSize);
}

void TexturedDepthMesh::DisplayAsPtsWithColor_Adaptive(Camera *NewView)
{
  float Xstep, Ystep;
  CalcStepSizes(NewView->Orig, &Xstep, &Ystep);
float OldPtSize;
glGetFloatv(GL_POINT_SIZE,&OldPtSize);
glPointSize( CalcMagnificationPtSize(&Xstep,&Ystep) );
  DisplayAsPtsWithColor(Xstep,Ystep);
glPointSize(OldPtSize);
}
*/
