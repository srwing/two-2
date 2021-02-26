//------------------------------------------------------------------------------
// File : boundingbox.cpp
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
// boundingbox.cpp : bounding box object (used as a backdrop)
//==========================================================================

#include <GL/glut.h>
#include "boundingbox.hpp"

BoundingBox::BoundingBox(Vec3f min, Vec3f max, float scale)
{
  Min=min;
  Max=max;
  Scale=scale;
}

void BoundingBox::UpdateMinMax()
{
}

void BoundingBox::Display(unsigned int Attribs)
{
  glPushAttrib(GL_POLYGON_BIT);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  Vec3f Cntr=(Max+Min)*0.5, Diag=(Max-Min)*0.5*Scale;
  Vec3f m=Cntr-Diag, M=Cntr+Diag;

  // STRIP OFF ATTRIBS THAT DO NOT RELATE TO THIS OBJECT
  Attribs &= (~OBJ_TEXCOORDS);

  glBegin(GL_QUADS);

  switch (Attribs)
  {
    case OBJ_NONE : // only make glVertex calls.
      glVertex3f(m.x,m.y,m.z); glVertex3f(m.x,m.y,M.z);
      glVertex3f(M.x,m.y,M.z); glVertex3f(M.x,m.y,m.z);
      glVertex3f(M.x,M.y,m.z); glVertex3f(M.x,M.y,M.z); 
      glVertex3f(m.x,M.y,M.z); glVertex3f(m.x,M.y,m.z); 
      glVertex3f(m.x,M.y,M.z); glVertex3f(m.x,m.y,M.z);
      glVertex3f(m.x,m.y,m.z); glVertex3f(m.x,M.y,m.z);
      glVertex3f(M.x,M.y,m.z); glVertex3f(M.x,m.y,m.z); 
      glVertex3f(M.x,m.y,M.z); glVertex3f(M.x,M.y,M.z);
      glVertex3f(m.x,M.y,m.z); glVertex3f(m.x,m.y,m.z); 
      glVertex3f(M.x,m.y,m.z); glVertex3f(M.x,M.y,m.z);
      glVertex3f(M.x,M.y,M.z); glVertex3f(M.x,m.y,M.z); 
      glVertex3f(m.x,m.y,M.z); glVertex3f(m.x,M.y,M.z); 
     break;

    case OBJ_COLORS : // vertex colors only
      glColor3f(0,0,0); glVertex3f(m.x,m.y,m.z);
      glColor3f(0,0,1); glVertex3f(m.x,m.y,M.z);
      glColor3f(1,0,1); glVertex3f(M.x,m.y,M.z);
      glColor3f(1,0,0); glVertex3f(M.x,m.y,m.z);
      glColor3f(1,1,0); glVertex3f(M.x,M.y,m.z);
      glColor3f(1,1,1); glVertex3f(M.x,M.y,M.z); 
      glColor3f(0,1,1); glVertex3f(m.x,M.y,M.z);
      glColor3f(0,1,0); glVertex3f(m.x,M.y,m.z); 
      glColor3f(0,1,1); glVertex3f(m.x,M.y,M.z);
      glColor3f(0,0,1); glVertex3f(m.x,m.y,M.z);
      glColor3f(0,0,0); glVertex3f(m.x,m.y,m.z);
      glColor3f(0,1,0); glVertex3f(m.x,M.y,m.z);
      glColor3f(1,1,0); glVertex3f(M.x,M.y,m.z);
      glColor3f(1,0,0); glVertex3f(M.x,m.y,m.z); 
      glColor3f(1,0,1); glVertex3f(M.x,m.y,M.z);
      glColor3f(1,1,1); glVertex3f(M.x,M.y,M.z);
      glColor3f(0,1,0); glVertex3f(m.x,M.y,m.z);
      glColor3f(0,0,0); glVertex3f(m.x,m.y,m.z); 
      glColor3f(1,0,0); glVertex3f(M.x,m.y,m.z);
      glColor3f(1,1,0); glVertex3f(M.x,M.y,m.z);
      glColor3f(1,1,1); glVertex3f(M.x,M.y,M.z);
      glColor3f(1,0,1); glVertex3f(M.x,m.y,M.z); 
      glColor3f(0,0,1); glVertex3f(m.x,m.y,M.z);
      glColor3f(0,1,1); glVertex3f(m.x,M.y,M.z); 
     break;

    case OBJ_NORMALS : // vertex normals only
      glNormal3f(0,1,0);  // BOTTOM
      glVertex3f(m.x,m.y,m.z); glVertex3f(m.x,m.y,M.z);
      glVertex3f(M.x,m.y,M.z); glVertex3f(M.x,m.y,m.z);
      glNormal3f(0,-1,0); // TOP
      glVertex3f(M.x,M.y,m.z); glVertex3f(M.x,M.y,M.z); 
      glVertex3f(m.x,M.y,M.z); glVertex3f(m.x,M.y,m.z); 
      glNormal3f(1,0,0);  // LEFT
      glVertex3f(m.x,M.y,M.z); glVertex3f(m.x,m.y,M.z);
      glVertex3f(m.x,m.y,m.z); glVertex3f(m.x,M.y,m.z);
      glNormal3f(-1,0,0); // RIGHT
      glVertex3f(M.x,M.y,m.z); glVertex3f(M.x,m.y,m.z); 
      glVertex3f(M.x,m.y,M.z); glVertex3f(M.x,M.y,M.z);
      glNormal3f(0,0,1);  // BACK
      glVertex3f(m.x,M.y,m.z); glVertex3f(m.x,m.y,m.z); 
      glVertex3f(M.x,m.y,m.z); glVertex3f(M.x,M.y,m.z);
      glNormal3f(0,0,-1); // FRONT
      glVertex3f(M.x,M.y,M.z); glVertex3f(M.x,m.y,M.z); 
      glVertex3f(m.x,m.y,M.z); glVertex3f(m.x,M.y,M.z); 
     break;

    case (OBJ_COLORS | OBJ_NORMALS) : // vertex colors and normals only
      glNormal3f(0,1,0);  // BOTTOM
      glColor3f(0,0,0); glVertex3f(m.x,m.y,m.z);
      glColor3f(0,0,1); glVertex3f(m.x,m.y,M.z);
      glColor3f(1,0,1); glVertex3f(M.x,m.y,M.z);
      glColor3f(1,0,0); glVertex3f(M.x,m.y,m.z);
      glNormal3f(0,-1,0); // TOP
      glColor3f(1,1,0); glVertex3f(M.x,M.y,m.z);
      glColor3f(1,1,1); glVertex3f(M.x,M.y,M.z); 
      glColor3f(0,1,1); glVertex3f(m.x,M.y,M.z);
      glColor3f(0,1,0); glVertex3f(m.x,M.y,m.z); 
      glNormal3f(1,0,0);  // LEFT
      glColor3f(0,1,1); glVertex3f(m.x,M.y,M.z);
      glColor3f(0,0,1); glVertex3f(m.x,m.y,M.z);
      glColor3f(0,0,0); glVertex3f(m.x,m.y,m.z);
      glColor3f(0,1,0); glVertex3f(m.x,M.y,m.z);
      glNormal3f(-1,0,0); // RIGHT
      glColor3f(1,1,0); glVertex3f(M.x,M.y,m.z);
      glColor3f(1,0,0); glVertex3f(M.x,m.y,m.z); 
      glColor3f(1,0,1); glVertex3f(M.x,m.y,M.z);
      glColor3f(1,1,1); glVertex3f(M.x,M.y,M.z);
      glNormal3f(0,0,1);  // BACK
      glColor3f(0,1,0); glVertex3f(m.x,M.y,m.z);
      glColor3f(0,0,0); glVertex3f(m.x,m.y,m.z); 
      glColor3f(1,0,0); glVertex3f(M.x,m.y,m.z);
      glColor3f(1,1,0); glVertex3f(M.x,M.y,m.z);
      glNormal3f(0,0,-1); // FRONT
      glColor3f(1,1,1); glVertex3f(M.x,M.y,M.z);
      glColor3f(1,0,1); glVertex3f(M.x,m.y,M.z); 
      glColor3f(0,0,1); glVertex3f(m.x,m.y,M.z);
      glColor3f(0,1,1); glVertex3f(m.x,M.y,M.z); 
     break;

    default: printf("ERROR (BoundingBox::Display): invalid display attribs\n");
     break;
  };

  glEnd();

  glPopAttrib();
}
