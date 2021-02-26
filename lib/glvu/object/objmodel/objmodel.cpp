//------------------------------------------------------------------------------
// File : objmodel.cpp
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
// objmodel.cpp : wavefront obj model
//============================================================================

#include "objmodel.hpp"


ObjModel::ObjModel()
{
  Obj=NULL;
}


ObjModel::ObjModel(const char *FileName, float Scale)
{
  Read(FileName, Scale);
}


ObjModel::~ObjModel()
{
  glmDelete(Obj);  
}


void ObjModel::Read(const char *FileName, float Scale)
{
  Obj = glmReadOBJ(FileName);
  glmFacetNormals(Obj);
  glmVertexNormals(Obj,90);
  UpdateMinMax();
}

void ObjModel::UpdateMinMax()
{
  glmExtents(Obj, &Min.x, &Max.x);
}


void ObjModel::Display(unsigned int Attribs)
{
  GLuint ObjAttribs = 0;
  switch (Attribs)
  {
    case OBJ_NONE : // ONLY MAKE glVertex CALLS
     glmDraw(Obj, GLM_NONE);
     break;
    case OBJ_ALL : // DISPLAY WITH ALL VERTEX ATTRIBUTES
     ObjAttribs = GLM_SMOOTH;
     if (Obj->nummaterials > 0) ObjAttribs |= GLM_MATERIAL; else ObjAttribs |= GLM_COLOR;
     if (Obj->numtexcoords > 0) ObjAttribs |= GLM_TEXTURE;     
     glmDraw(Obj, ObjAttribs);
     break;
    case OBJ_COLORS : // VERTEX COLORS ONLY
     if (Obj->nummaterials > 0) ObjAttribs=GLM_MATERIAL; else ObjAttribs|=GLM_COLOR;
     glmDraw(Obj, ObjAttribs);
     break;
    case OBJ_NORMALS : // VERTEX NORMALS ONLY
     glmDraw(Obj, GLM_SMOOTH);
     break;
    case OBJ_TEXCOORDS : // VERTEX TEX-COORDS ONLY
     if (Obj->numtexcoords > 0) ObjAttribs=GLM_TEXTURE;     
     glmDraw(Obj, ObjAttribs);
     break;
    case (OBJ_COLORS | OBJ_NORMALS) : // VERTEX COLORS AND NORMALS ONLY
     if (Obj->nummaterials > 0) ObjAttribs=GLM_MATERIAL; else ObjAttribs=GLM_COLOR;
     glmDraw(Obj, ObjAttribs | GLM_SMOOTH);
     break;
    case (OBJ_COLORS | OBJ_TEXCOORDS) : // VERTEX COLORS AND TEX-COORDS ONLY
     if (Obj->nummaterials > 0) ObjAttribs=GLM_MATERIAL; else ObjAttribs=GLM_COLOR;
     if (Obj->numtexcoords > 0) ObjAttribs |= GLM_TEXTURE;     
     glmDraw(Obj, ObjAttribs);
     break;
    case (OBJ_NORMALS | OBJ_TEXCOORDS) : // VERTEX NORMALS AND TEX-COORDS ONLY
     if (Obj->numtexcoords > 0) ObjAttribs=GLM_TEXTURE;     
     glmDraw(Obj, ObjAttribs | GLM_SMOOTH);
     break;
    default:
     printf("ERROR (ObjModel::Display) invalid display attribs!\n");
     break;
  };
}

void ObjModel::GetTris(float* &tris, int &numtris)
{
  glmGetTris(Obj,tris,numtris);
}

void ObjModel::GetTris(double* &tris, int &numtris)
{
  glmGetTris(Obj,tris,numtris);
}

int ObjModel::GetNumTris()
{
  return( glmGetNumTris(Obj) );
}
