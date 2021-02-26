//------------------------------------------------------------------------------
// File : trimodel_disp.cpp
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
// trimodel_disp.cpp : the OpenGL display routine for the TriModel
//============================================================================

#include <GL/glut.h>
#include "trimodel.hpp"

#include <glutils.hpp>

void TriModel::Display(unsigned int Attribs)
{
  // STRIP OFF ATTRIBS THAT DO NOT RELATE TO THIS OBJECT
  Attribs &= (~OBJ_TEXCOORDS);

  glBegin(GL_TRIANGLES);

  int i;
  switch (Attribs)
  {
    case OBJ_NONE : // only make glVertex calls.
     for (i=0; i<NumTris; i++)
     {
       glVertex3fv(&Tris[i].A.x);
       glVertex3fv(&Tris[i].B.x);
       glVertex3fv(&Tris[i].C.x);
     }
     break;

    case OBJ_COLORS : // vertex colors only
     for (i=0; i<NumTris; i++)
     {
       glColor3ubv(Tris[i].Color);
       glVertex3fv(&Tris[i].A.x);
       glVertex3fv(&Tris[i].B.x);
       glVertex3fv(&Tris[i].C.x);
     }
     break;

    case OBJ_NORMALS : // vertex normals only
     for (i=0; i<NumTris; i++)
     {
       glNormal3fv(&Tris[i].N.x);
       glVertex3fv(&Tris[i].A.x);
       glVertex3fv(&Tris[i].B.x);
       glVertex3fv(&Tris[i].C.x);
     }
     break;

    case (OBJ_COLORS | OBJ_NORMALS) : // vertex colors and normals only
     for (i=0; i<NumTris; i++)
     {
       glColor3ubv(Tris[i].Color);
       glNormal3fv(&Tris[i].N.x);
       glVertex3fv(&Tris[i].A.x);
       glVertex3fv(&Tris[i].B.x);
       glVertex3fv(&Tris[i].C.x);
     }
     
     break;

    default: printf("ERROR (TriModel::Display): invalid display attribs\n");
     break;
  };

  glEnd();
}
