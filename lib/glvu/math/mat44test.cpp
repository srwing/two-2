//------------------------------------------------------------------------------
// File : mat44test.cpp
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

//----------------------------------------------------------------------------
// mat44test  -- test compilation of all the functions in mat44 library
//----------------------------------------------------------------------------
// $Id: mat44test.cpp,v 1.4 2002/10/09 02:37:31 harrism Exp $
//----------------------------------------------------------------------------
#include <iostream.h>
#include <vec3f.hpp>
#include <mat44.hpp>

void testMat44Inverse()
{
  Mat44f M(0.629f, 0.747f, -0.214f, 0,
           0.000f, 0.276f, 0.961f,  0,
           0.777f, -0.605f, 0.173f, 0,
           0    ,      0,     0, 1);
  printf("mat : %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n",
         M[0], M[4], M[8],  M[12],
         M[1], M[5], M[9],  M[13],
         M[2], M[6], M[10], M[14],
         M[3], M[7], M[11], M[15] );
  Mat44f Minv;
  M.Inverse(Minv);//Minv.Inverse();

  printf("Determinant: %f\n\n", M.Determinant());

  printf("inv : %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n",
         Minv[0], Minv[4], Minv[8],  Minv[12],
         Minv[1], Minv[5], Minv[9],  Minv[13],
         Minv[2], Minv[6], Minv[10], Minv[14],
         Minv[3], Minv[7], Minv[11], Minv[15]  );
 
  M *= Minv;

  printf("  I : %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n"
         "      %.3f %.3f %.3f %.3f\n",
         M[0], M[4], M[8],  M[12],
         M[1], M[5], M[9],  M[13],
         M[2], M[6], M[10], M[14],
         M[3], M[7], M[11], M[15]  );
}   


int main(int argc, char *argv[])
{
  cout << "This is not a comprehensive correctness test. " << endl
       << "It merely tests whether all the functions in mat44 will compile." <<endl;

  Mat44f m;
  float m2[16];
  Vec3f v(1.0, 1.0, 1.0);
  Vec3f up(0.0, 1.0, 0.0);

  m.Identity();
  m.Transpose();
  m.Translate(1.0, 1.0, 1.0);
  m.Translate(v);
  m.invTranslate(v);
  m.Scale(1.0, 1.0, 1.0);
  m.Scale(v);
  m.invScale(1.0, 1.0, 1.0);
  m.invScale(v);
  m.Rotate(90, v);
  m.invRotate(90, v);
  m.Frustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 20);
  m.invFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 0.01f, 20);
  m.Perspective(60, 1.4f, 0.01f, 20);
  m.invPerspective(60, 1.4f, 0.01f, 20);
  m.Viewport(200, 200);
  m.invViewport(200, 200);
  m.LookAt(Vec3f::ZERO, v, up);
  m.invLookAt(Vec3f::ZERO, v, up);
  m.Viewport2(200, 200);
  m.invViewport2(200, 200);
  m.Print();
  m.Transpose();
  m.Print();
  // These two should be transpose of one another
  m.CopyInto(m2);

  cout << "Ok there is this one test to make sure Transpose() is correct.\n"
    "The above two matricies should be the transpose of one another.\n" << endl;

  testMat44Inverse();

  return 0; 
}
