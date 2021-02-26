//------------------------------------------------------------------------------
// File : glutils.cpp
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
// glutils.cpp : OpenGL handy rendering utilities
//============================================================================

#include "glutils.hpp"

void GLerror( char *msg )
{
 GLenum errCode;
 const GLubyte *errStr;

 if ((errCode = glGetError()) != GL_NO_ERROR) {
    errStr = gluErrorString(errCode);
    fprintf(stderr,"OpenGL ERROR: %s: %s\n", errStr, msg);
 }
}

void DisplayMinMaxBox(float Min[3], float Max[3], 
                      float r, float g, float b, float LineWidth)
{
  // CALCULATE VERTICES OF BOX
  #define SET(v,x,y,z) v[0]=x; v[1]=y; v[2]=z;
  float V[8][3];
  SET(V[0],Max[0],Max[1],Max[2]); SET(V[4],Max[0],Max[1],Min[2]);
  SET(V[1],Min[0],Max[1],Max[2]); SET(V[5],Min[0],Max[1],Min[2]);
  SET(V[2],Min[0],Min[1],Max[2]); SET(V[6],Min[0],Min[1],Min[2]);
  SET(V[3],Max[0],Min[1],Max[2]); SET(V[7],Max[0],Min[1],Min[2]);

  glPushAttrib(GL_LIGHTING_BIT);
  glDisable(GL_LIGHTING);
  glPushAttrib(GL_LINE_BIT);
  glLineWidth(LineWidth);
  glColor3f(r,g,b);

  glBegin(GL_LINE_LOOP);  // TOP FACE
    glVertex3fv(V[4]); glVertex3fv(V[5]); 
    glVertex3fv(V[1]); glVertex3fv(V[0]);
  glEnd();
  glBegin(GL_LINE_LOOP);  // BOTTOM FACE
    glVertex3fv(V[3]); glVertex3fv(V[2]);
    glVertex3fv(V[6]); glVertex3fv(V[7]); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // LEFT FACE
    glVertex3fv(V[1]); glVertex3fv(V[5]);
    glVertex3fv(V[6]); glVertex3fv(V[2]); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // RIGHT FACE
    glVertex3fv(V[0]); glVertex3fv(V[3]);
    glVertex3fv(V[7]); glVertex3fv(V[4]); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // NEAR FACE
    glVertex3fv(V[1]); glVertex3fv(V[2]);
    glVertex3fv(V[3]); glVertex3fv(V[0]); 
  glEnd();
  glBegin(GL_LINE_LOOP);  // FAR FACE
    glVertex3fv(V[4]); glVertex3fv(V[7]);
    glVertex3fv(V[6]); glVertex3fv(V[5]); 
  glEnd();

  glPopAttrib();
  glPopAttrib();
}
