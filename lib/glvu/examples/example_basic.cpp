//------------------------------------------------------------------------------
// File : example_basic.cpp
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
// example_basic.cpp : most basic GLVU example
//============================================================================
#include <glvu.hpp>

//----------------------------------------------------------------------------
// USER-PROVIDED INCLUDES
//----------------------------------------------------------------------------
#include <GL/glut.h>

//----------------------------------------------------------------------------
// GLOBALS
//----------------------------------------------------------------------------
GLVU glvu;

//----------------------------------------------------------------------------
// USER-PROVIDED DRAWING ROUTINE
//----------------------------------------------------------------------------
void userDisplayFunc0()
{
  glvu.BeginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,0,0);
    glutSolidTorus(0.25,1,32,64);
  glvu.EndFrame();
}

//----------------------------------------------------------------------------
// MAIN ROUTINE. INIT USER OBJECTS AND ACTIVATE VIEWER.
//----------------------------------------------------------------------------
int main(int argc, char *arv[])
{
  //--------------------------------------------------------------------------
  // TO USE THE VIEWER:
  // (1) Instantiate some global GLVUs (one for each window). See top of file.
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  // (2) Init each viewer by specifying menu string, visual mode mask, 
  //     and starting position and extents of the window.
  //     After Init, perform any OpenGL initializations and
  //     initialize the viewer cameras.
  //--------------------------------------------------------------------------

  glvu.Init("GLVU Basic Example",
            GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA,
            50,50,512,512);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

  glutDisplayFunc(userDisplayFunc0);

  Vec3f ModelMin(-1,-1,-1), ModelMax(1,1,1); 
  Vec3f LookAtCntr( ModelMin+ModelMax ); LookAtCntr*=0.5;
  Vec3f mintoCtr( LookAtCntr - ModelMin );
  Vec3f Up(0,1,0);
  Vec3f Eye(LookAtCntr - 3*(mintoCtr - Up * (mintoCtr * Up) ));
  float Yfov = 45;
  float Aspect = 1;     // WIDTH OVER HEIGHT
  float Near = 0.1f;    // NEAR PLANE DISTANCE RELATIVE TO MODEL DIAGONAL LENGTH
  float Far = 10.0f;    // FAR PLANE DISTANCE (ALSO RELATIVE)
  glvu.SetAllCams(ModelMin,ModelMax, Eye,LookAtCntr,Up, Yfov,Aspect, Near,Far);

  //--------------------------------------------------------------------------
  // (3) start the viewer event loop.
  //--------------------------------------------------------------------------

  glutMainLoop();

  // Control flow will never reach here
  return EXIT_SUCCESS;
}
