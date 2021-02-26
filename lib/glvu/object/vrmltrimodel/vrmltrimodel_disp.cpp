//------------------------------------------------------------------------------
// File : vrmltrimodel_disp.cpp
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
// vrmltridisp.cpp : TriModel OPENGL display routines
//============================================================================

#include <GL/glut.h>
#include "vrmltri.hpp"
#include "vrmltrimodel.hpp"

void VrmlTri::Display(unsigned int Attribs)  // WITH PER-VERTEX NORMALS
{
  // STRIP OFF ATTRIBS THAT DO NOT RELATE TO THIS OBJECT
  Attribs &= (~OBJ_TEXCOORDS);

  int i;
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
     glColor3fv( &(Color.x) );
     glBegin(GL_TRIANGLES);
     for (i=0; i<NumTris; i++)
     {
       glVertex3fv(&Verts[Tris[i].iA].V.x);
       glVertex3fv(&Verts[Tris[i].iB].V.x);
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
     glColor3fv( &(Color.x) );
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

    default: printf("ERROR (VrmlTri::Display): invalid display attribs\n");
     break;
  };
}

void VrmlTriModel::Display(unsigned int Attribs)
{
  for (int i=0; i<NumObjects; i++)
    Objects[i]->Display(Attribs);
}
