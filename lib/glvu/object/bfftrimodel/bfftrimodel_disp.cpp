//------------------------------------------------------------------------------
// File : bfftrimodel_disp.cpp
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
// bfftridisp.cpp : TriModel OPENGL display routines
//============================================================================

#include <GL/glut.h>
#include <textureobj.hpp>
#include "bfftri.hpp"
#include "bfftrimodel.hpp"
#include <glutils.hpp>

void BffTri::Display(unsigned int Attribs)  // WITH PER-VERTEX NORMALS
{
  int i;
  GLint currTexId;
  GLbyte texEnabled;
  float M[16];

  switch (Attribs)
  {
    case OBJ_NONE : // only make glVertex calls.
     glBegin(GL_TRIANGLES);
     for (i=0; i<NumTris; i++)
     {
       glVertex3fv(&Verts[Tris[i].iA].V.x);
       glVertex3fv(&Verts[Tris[i].iB].V.x);
       glVertex3fv(&Verts[Tris[i].iC].V.x);
     }
     glEnd();
     break;

    case OBJ_COLORS : // vertex colors only
     glBegin(GL_TRIANGLES);
     for (i=0; i<NumTris; i++)
     {
       glColor3fv((GLfloat*)&Verts[Tris[i].iA].C);
       glVertex3fv(&Verts[Tris[i].iA].V.x);
       glColor3fv((GLfloat*)&Verts[Tris[i].iB].C);
       glVertex3fv(&Verts[Tris[i].iB].V.x);
       glColor3fv((GLfloat*)&Verts[Tris[i].iC].C);
       glVertex3fv(&Verts[Tris[i].iC].V.x);
     }
     glEnd();
     break;

    case OBJ_NORMALS : // vertex normals only
      glBegin(GL_TRIANGLES);
      for (i=0; i<NumTris; i++)
      {
       #ifdef USE_VERTEX_NORMALS
        glNormal3fv(&Verts[Tris[i].iA].N.x);
        glVertex3fv(&Verts[Tris[i].iA].V.x);
        glNormal3fv(&Verts[Tris[i].iB].N.x);
        glVertex3fv(&Verts[Tris[i].iB].V.x);
        glNormal3fv(&Verts[Tris[i].iC].N.x);
        glVertex3fv(&Verts[Tris[i].iC].V.x);
       #else
        glNormal3fv(&Tris[i].N.x);
        glVertex3fv(&Verts[Tris[i].iA].V.x);
        glVertex3fv(&Verts[Tris[i].iB].V.x);
        glVertex3fv(&Verts[Tris[i].iC].V.x);
       #endif
      }
      glEnd();
     break;

    case (OBJ_COLORS | OBJ_NORMALS) : // vertex colors and normals only
     glBegin(GL_TRIANGLES);
     for (i=0; i<NumTris; i++)
     {
       #ifdef USE_VERTEX_NORMALS
        glColor3fv(&Verts[Tris[i].iA].C.x);
        glNormal3fv(&Verts[Tris[i].iA].N.x);
        glVertex3fv(&Verts[Tris[i].iA].V.x);
        glColor3fv(&Verts[Tris[i].iB].C.x);
        glNormal3fv(&Verts[Tris[i].iB].N.x);
        glVertex3fv(&Verts[Tris[i].iB].V.x);
        glColor3fv(&Verts[Tris[i].iC].C.x);
        glNormal3fv(&Verts[Tris[i].iC].N.x);
        glVertex3fv(&Verts[Tris[i].iC].V.x);
       #else
        glNormal3fv(&Tris[i].N.x);
        glColor3fv(&Verts[Tris[i].iA].C.x);
        glVertex3fv(&Verts[Tris[i].iA].V.x);
        glColor3fv(&Verts[Tris[i].iB].C.x);
        glVertex3fv(&Verts[Tris[i].iB].V.x);
        glColor3fv(&Verts[Tris[i].iC].C.x);
        glVertex3fv(&Verts[Tris[i].iC].V.x);
       #endif
     }
     glEnd();
     break;

    case OBJ_ALL :
     currTexId = -1;
     texEnabled = 0;
     glPushAttrib(GL_TEXTURE_BIT);
     glBegin(GL_TRIANGLES);
     for (i=0; i<NumTris; i++)
     {
       if (Tris[i].TexId != currTexId) {
          glEnd();
          if (texEnabled) {
             glDisable(GL_TEXTURE_2D);
             texEnabled = 0;
          }
          currTexId = Tris[i].TexId;
          if (currTexId >= 0) {  // A VALID TEXTURE
             glEnable(GL_TEXTURE_2D);
             glBindTexture(GL_TEXTURE_2D,currTexId);
             glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
             glMatrixMode(GL_TEXTURE);
             glGetFloatv(GL_TEXTURE_MATRIX,M);
             glLoadIdentity();
             texEnabled = 1;
          }
          glBegin(GL_TRIANGLES);
       }

       #ifdef USE_VERTEX_NORMALS
        glNormal3fv(&Verts[Tris[i].iA].N.x);
        glColor3fv(&Verts[Tris[i].iA].C.x);
        glTexCoord2fv(&Verts[Tris[i].iA].UV.x);
        glVertex3fv(&Verts[Tris[i].iA].V.x);

        glNormal3fv(&Verts[Tris[i].iB].N.x);
        glColor3fv(&Verts[Tris[i].iB].C.x);
        glTexCoord2fv(&Verts[Tris[i].iB].UV.x);
        glVertex3fv(&Verts[Tris[i].iB].V.x);

        glNormal3fv(&Verts[Tris[i].iC].N.x);
        glColor3fv(&Verts[Tris[i].iC].C.x);
        glTexCoord2fv(&Verts[Tris[i].iC].UV.x);
        glVertex3fv(&Verts[Tris[i].iC].V.x);
       #else
        glNormal3fv(&Tris[i].N.x);

        glColor3fv   (&Verts[Tris[i].iA].C.x);
        glTexCoord2fv(&Verts[Tris[i].iA].UV.x);
        glVertex3fv  (&Verts[Tris[i].iA].V.x);

        glColor3fv   (&Verts[Tris[i].iB].C.x);
        glTexCoord2fv(&Verts[Tris[i].iB].UV.x);
        glVertex3fv  (&Verts[Tris[i].iB].V.x);

        glColor3fv   (&Verts[Tris[i].iC].C.x);
        glTexCoord2fv(&Verts[Tris[i].iC].UV.x);
        glVertex3fv  (&Verts[Tris[i].iC].V.x);
//fprintf(stderr,"%f %f\n",Verts[Tris[i].iA].UV.x,Verts[Tris[i].iC].UV.y);
       #endif
     }
     glEnd();
     glMatrixMode(GL_TEXTURE);
     glLoadMatrixf(M);
     glPopAttrib();  // GL_TEXTURE_BIT
     break;

    default: printf("ERROR (BffTri::Display): invalid display attribs\n");
     break;
  };
}

void BffTriModel::Display(unsigned int Attribs)
{
  for (int i=0; i<NumObjects; i++)
    Objects[i]->Display(Attribs);
}
