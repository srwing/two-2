//------------------------------------------------------------------------------
// File : object.cpp
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
// object.cpp
//==========================================================================

#include <GL/glut.h>
#include "object.hpp"

//----------------------------------------------------------------------------
// DRAW A WIREFRAME AXIS-ALIGNED BOUNDING BOX
//----------------------------------------------------------------------------
void Object::DrawAABB(float r, float g, float b)
{
 glPushAttrib(GL_LIGHTING_BIT);
 glPushAttrib(GL_POLYGON_BIT);
 glDisable(GL_LIGHTING);
 glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glColor3f(r,g,b);

  Vec3f m=Min, M=Max;
  glBegin(GL_QUADS);
    glVertex3fv(&M.x); glVertex3f(m.x,M.y,M.z); glVertex3f(m.x,m.y,M.z); glVertex3f(M.x,m.y,M.z);
    glVertex3f(m.x,M.y,m.z); glVertex3f(M.x,M.y,m.z); glVertex3f(M.x,m.y,m.z); glVertex3fv(&m.x);
    glVertex3f(M.x,M.y,m.z); glVertex3fv(&M.x); glVertex3f(M.x,m.y,M.z); glVertex3f(M.x,m.y,m.z);
    glVertex3f(m.x,M.y,M.z); glVertex3f(m.x,M.y,m.z); glVertex3fv(&m.x); glVertex3f(m.x,m.y,M.z);
    glVertex3f(M.x,M.y,m.z); glVertex3f(m.x,M.y,m.z); glVertex3f(m.x,M.y,M.z); glVertex3fv(&M.x);
    glVertex3f(M.x,m.y,M.z); glVertex3f(m.x,m.y,M.z); glVertex3fv(&m.x); glVertex3f(M.x,m.y,m.z);
  glEnd();

 glPopAttrib();
 glPopAttrib();
}
