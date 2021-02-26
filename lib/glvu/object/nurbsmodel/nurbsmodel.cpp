//------------------------------------------------------------------------------
// File : nurbsmodel.cpp
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
// nurbsmodel.cpp : trimmed NURBS surface models (see nurbsformat.txt)
//============================================================================

#include "nurbsmodel.hpp"

NURBSmodel::NURBSmodel()
{
  Obj=NULL;
}

NURBSmodel::NURBSmodel(const char *FileName)
{
  Read(FileName);
}

NURBSmodel::~NURBSmodel()
{
  if (Obj) delete Obj;
}

void NURBSmodel::Read(const char *FileName)
{
  Obj = new NURBSobj;
  Obj->ReadInNurbsObject(FileName);
  Min = Obj->Min;
  Max = Obj->Max;
}

void NURBSmodel::UpdateMinMax()
{
  Obj->CalcAABB();
  Min = Obj->Min;
  Max = Obj->Max;
}

void NURBSmodel::Display(unsigned int Attribs)
{
  // STRIP OFF ATTRIBS THAT DO NOT RELATE TO THIS OBJECT
  Attribs &= (~OBJ_TEXCOORDS);

  switch (Attribs)
  {
    case OBJ_NONE : // only make glVertex calls.
     Obj->Display_NO_ATTRIBS();
     break;

    case OBJ_COLORS : // vertex colors only
     Obj->Display_COLORS();
     break;

    case OBJ_NORMALS : // vertex normals only
     Obj->Display_NORMALS();
     break;

    case (OBJ_COLORS | OBJ_NORMALS) : // vertex colors and normals only
     Obj->Display_COLORS_NORMALS();
     break;

    default: printf("ERROR (NURBSmodel::Display): invalid display attribs\n");
     break;
  };
}
