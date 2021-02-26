//------------------------------------------------------------------------------
// File : example_2windows.cpp
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
// example_2window.cpp : most basic multi-window GLVU example
//============================================================================
#include <glvu.hpp>

//----------------------------------------------------------------------------
// USER-PROVIDED INCLUDES
//----------------------------------------------------------------------------
#include <GL/glut.h>

//----------------------------------------------------------------------------
// GLOBALS
//----------------------------------------------------------------------------
#define NUM_GLVUS 2
GLVU GLVUs[NUM_GLVUS];

//----------------------------------------------------------------------------
// USER-PROVIDED DRAWING ROUTINE
//----------------------------------------------------------------------------
void userDisplayFunc0()
{
  GLVUs[0].BeginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,0,0);
    glutSolidTeapot(1);
  GLVUs[0].EndFrame();
}

void userDisplayFunc1()
{
  GLVUs[1].BeginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0,1,0);
    glutSolidTeapot(1);
  GLVUs[1].EndFrame();
}

void InitOpenGL0()
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

void InitOpenGL1()
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
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

  Vec3f m,M,Cntr, Eye,LookAtCntr,Up;
  m.Set(-1,-1,-1);
  M.Set(1,1,1);
  Up.Set(0,1,0);
  float Yfov = 45;
  float Aspect = 1;     // WIDTH OVER HEIGHT
  float Near = 0.1f;    // NEAR PLANE DISTANCE RELATIVE TO MODEL DIAGONAL LENGTH
  float Far = 10.0f;    // FAR PLANE DISTANCE (ALSO RELATIVE)


  // ---- WINDOW 0 INITIALIZATION ----
  
  GLVUs[0].Init("GLVU 2-Window Example (window 0)",
                GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA,
                50,50,512,512);

  InitOpenGL0();

  glutDisplayFunc(userDisplayFunc0);

  Eye.Set(0,0,3);
  LookAtCntr.Set(0,0,0);
  GLVUs[0].SetAllCams(m,M, Eye,LookAtCntr,Up, Yfov,Aspect, Near,Far);


  // ---- WINDOW 1 INITIALIZATION ----
  
  GLVUs[1].Init("GLVU 2-Window Example (window 1)",
                GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA,
                600,100,500,400);

  InitOpenGL1();

  glutDisplayFunc(userDisplayFunc1);

  Eye.Set(2,2,3);
  LookAtCntr.Set(0,0,0);
  GLVUs[1].SetAllCams(m,M, Eye,LookAtCntr,Up, Yfov,Aspect, Near,Far);

  //--------------------------------------------------------------------------
  // (3) start the viewer event loop.
  //--------------------------------------------------------------------------

  glutMainLoop();

  // Control flow will never reach here
  return EXIT_SUCCESS;
}
