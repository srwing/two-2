//------------------------------------------------------------------------------
// File : groundplane.cpp
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
// groundplane.cpp : bounding box ground plane. This object is designed to
// serve as a backdrop from existing objects bounded by a Min/Max bounding
// box. Y is considered up, the ground plane can be scaled in the XZ plane
// and can be moved along Y in units of the bounding box Y-height.
//==========================================================================

#include "groundplane.hpp"
#include <ppm.hpp>
#include <textureobj.hpp>

GroundPlane::GroundPlane(Vec3f min, Vec3f max, float XZscale, float Yoffset,
                         char *TextureFileName, int numtiles, Vec3f color)
{
  Min=min;
  Max=max;
  N.Set(0,1,0);

  C[0].Set(color);  // WHITE
  C[1].Set(color);
  C[2].Set(color);
  C[3].Set(color);

  Vec3f Cntr=(Min+Max)*0.5;
  float Dx = Max.x-Cntr.x, Dz=Max.z-Cntr.z;
  Dx*=XZscale; Dz*=XZscale;
  if (Dx>Dz) Dz=Dx; else Dx=Dz;
  Cntr.y = Min.y + (Max.y-Min.y)*Yoffset;
  Min.Set(Cntr.x-Dx, Cntr.y, Cntr.z-Dz);
  Max.Set(Cntr.x+Dx, Cntr.y, Cntr.z+Dz);

  V[0].Set(Min.x,Min.y,Min.z);
  V[1].Set(Min.x,Min.y,Max.z);
  V[2].Set(Max.x,Min.y,Max.z);
  V[3].Set(Max.x,Min.y,Min.z);

  Color=NULL;
  Width=Height=0;
  TexID=0;

  if (TextureFileName!=NULL)
  {
    LoadPPM(TextureFileName,Color,Width,Height);
    TexID = CreateTextureObjFromColorBuffer(Color,Width,Height);
  }

  NumTiles=numtiles;
}

GroundPlane::~GroundPlane()
{
  delete[] Color;              // DELETE THE TEXTURE RGB ARRAY
  DeleteTextureObj(TexID);     // DESTROY THE TEXTURE OBJECT BY DELETING IT
}

void GroundPlane::UpdateMinMax()
{
}

void GroundPlane::Display(unsigned int Attribs)
{
float M[16];

  int n=NumTiles;

  // BE SURE TEXTURE IS READY IF TEXTURE IS BEING DISPLAYED
  if (Attribs & OBJ_TEXCOORDS)
    if (TexID!=0)
    {
      glPushAttrib(GL_TEXTURE_BIT);
      glEnable(GL_TEXTURE_2D);
      if (!glIsTexture(TexID))  // IF TEX OBJ DELETED, MUST RECREATE
      {
        printf("Groundplane texture object was deleted, regenerating. . .\n");
        TexID = CreateTextureObjFromColorBuffer(Color,Width,Height);
      }
      glBindTexture(GL_TEXTURE_2D,TexID);  
      glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

      glMatrixMode(GL_TEXTURE); 
      glGetFloatv(GL_TEXTURE_MATRIX,M);
      glLoadIdentity();
    }
    else 
    {
       Attribs &= (~OBJ_TEXCOORDS);  // UNSET TEX-COORDS BIT, NO TEXTURE AVAILABLE      
    }

  switch (Attribs)
  {
    case OBJ_NONE : // ONLY MAKE glVertex CALLS
     glBegin(GL_QUADS);
       glVertex3fv(&V[0].x);
       glVertex3fv(&V[1].x);
       glVertex3fv(&V[2].x);
       glVertex3fv(&V[3].x);
     glEnd();
     break;

    case OBJ_ALL : // DISPLAY WITH ALL VERTEX ATTRIBUTES
     glNormal3fv(&N.x);
     glBegin(GL_QUADS);
       glColor3fv(&C[0].x); glTexCoord2f(0,n); glVertex3fv(&V[0].x);
       glColor3fv(&C[1].x); glTexCoord2f(0,0); glVertex3fv(&V[1].x);
       glColor3fv(&C[2].x); glTexCoord2f(n,0); glVertex3fv(&V[2].x);
       glColor3fv(&C[3].x); glTexCoord2f(n,n); glVertex3fv(&V[3].x);
     glEnd();
     glMatrixMode(GL_TEXTURE); 
     glLoadMatrixf(M);
     glPopAttrib();  // GL_TEXTURE_BIT
     break;

    case OBJ_COLORS : // VERTEX COLORS ONLY
     glBegin(GL_QUADS);
       glColor3fv(&C[0].x); glVertex3fv(&V[0].x);
       glColor3fv(&C[1].x); glVertex3fv(&V[1].x);
       glColor3fv(&C[2].x); glVertex3fv(&V[2].x);
       glColor3fv(&C[3].x); glVertex3fv(&V[3].x);
     glEnd();
     break;

    case OBJ_NORMALS : // VERTEX NORMALS ONLY
     glNormal3fv(&N.x);
     glBegin(GL_QUADS);
       glVertex3fv(&V[0].x);
       glVertex3fv(&V[1].x);
       glVertex3fv(&V[2].x);
       glVertex3fv(&V[3].x);
     glEnd();
     break;

    case OBJ_TEXCOORDS : // VERTEX TEX-COORDS ONLY
     glBegin(GL_QUADS);
       glTexCoord2f(0,n); glVertex3fv(&V[0].x);
       glTexCoord2f(0,0); glVertex3fv(&V[1].x);
       glTexCoord2f(n,0); glVertex3fv(&V[2].x);
       glTexCoord2f(n,n); glVertex3fv(&V[3].x);
     glEnd();
     glMatrixMode(GL_TEXTURE); 
     glLoadMatrixf(M);
     glPopAttrib();  // GL_TEXTURE_BIT
     break;

    case (OBJ_COLORS | OBJ_NORMALS) : // VERTEX COLORS AND NORMALS ONLY
     glNormal3fv(&N.x);
     glBegin(GL_QUADS);
       glColor3fv(&C[0].x); glVertex3fv(&V[0].x);
       glColor3fv(&C[1].x); glVertex3fv(&V[1].x);
       glColor3fv(&C[2].x); glVertex3fv(&V[2].x);
       glColor3fv(&C[3].x); glVertex3fv(&V[3].x);
     glEnd();
     break;

    case (OBJ_COLORS | OBJ_TEXCOORDS) : // VERTEX COLORS AND TEX-COORDS ONLY
     glBegin(GL_QUADS);
       glColor3fv(&C[0].x); glTexCoord2f(0,n); glVertex3fv(&V[0].x);
       glColor3fv(&C[1].x); glTexCoord2f(0,0); glVertex3fv(&V[1].x);
       glColor3fv(&C[2].x); glTexCoord2f(n,0); glVertex3fv(&V[2].x);
       glColor3fv(&C[3].x); glTexCoord2f(n,n); glVertex3fv(&V[3].x);
     glEnd();
     glMatrixMode(GL_TEXTURE); 
     glLoadMatrixf(M);
     glPopAttrib();  // GL_TEXTURE_BIT
     break;

    case (OBJ_NORMALS | OBJ_TEXCOORDS) : // VERTEX NORMALS AND TEX-COORDS ONLY
     glNormal3fv(&N.x);
     glBegin(GL_QUADS);
       glTexCoord2f(0,n); glVertex3fv(&V[0].x);
       glTexCoord2f(0,0); glVertex3fv(&V[1].x);
       glTexCoord2f(n,0); glVertex3fv(&V[2].x);
       glTexCoord2f(n,n); glVertex3fv(&V[3].x);
     glEnd();
     glMatrixMode(GL_TEXTURE); 
     glLoadMatrixf(M);
     glPopAttrib();  // GL_TEXTURE_BIT
     break;

    default:
     printf("ERROR (Groundplane::Display) invalid display attribs!\n");
     break;
  };
}
