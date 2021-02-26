//------------------------------------------------------------------------------
// File : wavy.cpp
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

//==========================================================================
// wavy.cpp : the animated wavy ground object. Like the groundplane object,
// This object is designed to serve as a backdrop from existing objects 
// bounded by a Min/Max bounding box. Y is considered up, the wavyground plane 
// can be scaled in the XZ plane and can be moved along Y in units of the 
// bounding box Y-height. Constructors are also provided for simply fitting
// the wavy object inside of a bounding box.
//==========================================================================

#include <GL/glut.h>
#include "wavy.hpp"

#define TWO_PI 6.2831853

//--------------------------------------------------------------------------
// Constructor that places a wavy object below a given bounding box.
// (min,max) here specifies the bounding box below which we will place the
// wavy bounding box. Actual wave bounding box top starts at Yoffset below
// given bounding box, is scaled by XZscale, and has thickness Ythickness
// all in units of the given bounding box.
//--------------------------------------------------------------------------
WavyGround::WavyGround(int NumPtsAlongX, int NumPtsAlongZ,
                       float NumWaves, float AngOffset,     
                       float CntrS, float CntrT,
                       Vec3f min, Vec3f max, 
                       float XZscale, float Yoffset, float Ythickness)
{
  float Dy = max.y - min.y;
  Vec3f Cntr=(min+max)*0.5;
  float Dx = max.x-Cntr.x, Dz=max.z-Cntr.z;
  Dx*=XZscale; Dz*=XZscale;
  if (Dx>Dz) Dz=Dx; else Dx=Dz;
  Cntr.y = min.y + Dy*Yoffset;
  Min.Set(Cntr.x-Dx, Cntr.y - Dy*Ythickness, Cntr.z-Dz);
  Max.Set(Cntr.x+Dx, Cntr.y,                 Cntr.z+Dz);

  AllocMesh(NumPtsAlongX,NumPtsAlongZ);
  UpdateWaveParams(NumWaves,AngOffset,CntrS,CntrT);
}


//--------------------------------------------------------------------------
// CORE constructor that create a NumPtsAlongX x NumPtsAlongY wavy mesh
// that fills the bounding box (min,max) with the specified animation params.
//--------------------------------------------------------------------------
WavyGround::WavyGround(int NumPtsAlongX, int NumPtsAlongZ,
                       Vec3f min, Vec3f max,
                       float NumWaves, float AngOffset,     
                       float CntrS, float CntrT)
{
  AllocMesh(NumPtsAlongX,NumPtsAlongZ);
  UpdateWaveExtents(min,max);
  UpdateWaveParams(NumWaves,AngOffset,CntrS,CntrT);
}

WavyGround::~WavyGround()
{
  FreeMesh();
}

void WavyGround::AllocMesh(int NumPtsAlongX, int NumPtsAlongZ)
{
  I=NumPtsAlongX;
  J=NumPtsAlongZ;

  // ALLOCATE (J+1)X(I+1) MESH
  int I_1=I+1, J_1=J+1;
  V = new V3fv*[J_1];
  N = new V3fv*[J_1];
  C = new V3fv*[J_1];
  int k;
  for (k=0; k<=J; k++) V[k] = new V3fv[I_1];
  for (k=0; k<=J; k++) N[k] = new V3fv[I_1];
  for (k=0; k<=J; k++) C[k] = new V3fv[I_1];
}

void WavyGround::FreeMesh()
{
  // FREE THE MESH
  int k;
  for (k=0; k<=J; k++) delete[] V[k];
  for (k=0; k<=J; k++) delete[] N[k];
  for (k=0; k<=J; k++) delete[] C[k];
  delete[] V;
  delete[] N;
  delete[] C;
}

//--------------------------------------------------------------------------
// This is useful for animating the extents of the wavy object.
//  (min,max)     : bounding box containing the wave. Wave fills the box.
//--------------------------------------------------------------------------
void WavyGround::UpdateWaveExtents(Vec3f min, Vec3f max)
{
  Min=min;
  Max=max;
}

//--------------------------------------------------------------------------
// Update the wavy mesh with the following parameters:
// 1) NumWaves      : Number of complete waves across the surface in each dir
// 2) AngOffset     : angular offset added to distance from wave center.
// 3) (CntrS,CntrT) : wave center in domain (s,t) of the wave in [0,1].
// Wave fills the current (Min,Max) bounding box.
//--------------------------------------------------------------------------
void WavyGround::UpdateWaveParams(float NumWaves, float AngOffset, 
                                  float CntrS, float CntrT)
{
  float WaveHeight = (Max.y-Min.y)*0.5;
  float WaveCenterY = (Max.y+Min.y)*0.5;

  float s,t;
  float X,Z,Dist,RadAng;
  float dX=Max.x-Min.x, dY=Max.y-Min.y, dZ=Max.z-Min.z;
  float DYst, l;
  for (int j=0; j<=J; j++)
    for (int i=0; i<=I; i++)
    {
      // COMPUTE PARAMETRIC VALUES (CAN BE DONE INCREMENTALLY)
      s=i/(float)I; 
      t=j/(float)J;

      // DIST FROM CNTR OF WAVE
      X = s-CntrS;
      Z = t-CntrT;
      Dist = (float)sqrt(X*X + Z*Z);

      // COMPUTE "ANGLE"
      RadAng = TWO_PI*NumWaves*Dist + AngOffset;

      // CALC Vertex:(X,Y,Z) FOR MESH POINT (s,t)
      V[j][i][0] = Min.x+dX*s;
      V[j][i][1] = (float)sin(RadAng)*WaveHeight + WaveCenterY;
      V[j][i][2] = Min.z+dZ*t;

      // CALC Normal:(X,Y,Z), NORMALIZE! CrossProd BETWEEN GRAD ALONG T AND GRAD ALONG S
      DYst = (float)cos(RadAng) * WaveHeight * TWO_PI*NumWaves/Dist;
      N[j][i][0] = -dZ * DYst * (s-CntrS);
      N[j][i][1] = dZ * dX;
      N[j][i][2] = -DYst * (t-CntrT) * dX;
      l = (float)sqrt(N[j][i][0]*N[j][i][0] + N[j][i][1]*N[j][i][1] + N[j][i][2]*N[j][i][2]);
      N[j][i][0] /= l;
      N[j][i][1] /= l;
      N[j][i][2] /= l;

      // CALC Color:(X,Y,Z)
      C[j][i][0] = s;
      C[j][i][1] = t;
      C[j][i][2] = (V[j][i][1]-Min.y) / dY;
    }    
}

// THIS ROUTINE DOES NOTHING SINCE THE BOUNDING BOX IS SET EXPLICITLY
void WavyGround::UpdateMinMax()
{
}

void WavyGround::DisplayNormals(float Color[3], float Scale)
{
  glPushAttrib(GL_LIGHTING_BIT);
  glDisable(GL_LIGHTING);
   glColor3fv(Color);
   glBegin(GL_LINES);
   for (int j=0; j<=J; j++)
     for (int i=0; i<=I; i++)
     {
       glVertex3fv(V[j][i]);
       glVertex3f(V[j][i][0] + N[j][i][0]*Scale,
                  V[j][i][1] + N[j][i][1]*Scale,
                  V[j][i][2] + N[j][i][2]*Scale);
     }
   glEnd();
  glPopAttrib();
}

void WavyGround::Display(unsigned int Attribs)
{
  // STRIP OFF ATTRIBS THAT DO NOT RELATE TO THIS OBJECT
  Attribs &= (~OBJ_TEXCOORDS);

  int i,j;
  switch (Attribs)
  {
    case OBJ_NONE : // only make glVertex calls.
     for (j=0; j<J; j++)
     {
       glBegin(GL_TRIANGLE_STRIP);
       for (i=0; i<=I; i++)
       {
         glVertex3fv(V[j][i]);      
         glVertex3fv(V[j+1][i]);      
       }    
       glEnd();
     }
     break;

    case OBJ_COLORS : // vertex colors only
     for (j=0; j<J; j++)
     {
       glBegin(GL_TRIANGLE_STRIP);
       for (i=0; i<=I; i++)
       {
         glColor3fv(C[j][i]);
         glVertex3fv(V[j][i]);      

         glColor3fv(C[j+1][i]);
         glVertex3fv(V[j+1][i]);      
       }    
       glEnd();
     }
     break;

    case OBJ_NORMALS : // vertex normals only
     for (j=0; j<J; j++)
     {
       glBegin(GL_TRIANGLE_STRIP);
       for (i=0; i<=I; i++)
       {
         glNormal3fv(N[j][i]);
         glVertex3fv(V[j][i]);      

         glNormal3fv(N[j+1][i]);
         glVertex3fv(V[j+1][i]);      
       }    
       glEnd();
     }
     break;

    case (OBJ_COLORS | OBJ_NORMALS) : // vertex colors and normals only
     for (j=0; j<J; j++)
     {
       glBegin(GL_TRIANGLE_STRIP);
       for (i=0; i<=I; i++)
       {
         glColor3fv(C[j][i]);
         glNormal3fv(N[j][i]);
         glVertex3fv(V[j][i]);      

         glColor3fv(C[j+1][i]);
         glNormal3fv(N[j+1][i]);
         glVertex3fv(V[j+1][i]);      
       }    
       glEnd();
     }
     break;

    default: printf("ERROR (WavyGround::Display): invalid display attrib\n");
     break;
  };
}
