//------------------------------------------------------------------------------
// File : example_keymouse.cpp
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
// example_keymouse.cpp : GLVU example using user provided keyboard/mouse input.
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
int CallingDefaultMouseFuncsOn=1;

//----------------------------------------------------------------------------
// USER-PROVIDED DRAWING ROUTINE
//----------------------------------------------------------------------------
void userDisplayFunc0()
{
  glvu.BeginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,0,0);
    glutSolidTeapot(1);
  glvu.EndFrame();
}

//----------------------------------------------------------------------------
// USER-PROVIDED MOUSE HANDLING ROUTINE
//----------------------------------------------------------------------------
void userMouseFunc0(int button, int state, int x, int y)
{
  int Modifiers = glutGetModifiers();
  if (Modifiers & GLUT_ACTIVE_CTRL)
    printf("CTRL-");
  if (Modifiers & GLUT_ACTIVE_SHIFT) 
    printf("SHIFT-");
  if (Modifiers & GLUT_ACTIVE_ALT) 
    printf("ALT-");

  if (button==GLUT_LEFT_BUTTON)
    if (state==GLUT_DOWN) printf("LEFT-DOWN\n"); else printf("LEFT-UP\n");
  if (button==GLUT_RIGHT_BUTTON)
    if (state==GLUT_DOWN) printf("RIGHT-DOWN\n"); else printf("RIGHT-UP\n");

  // OPTIONAL: CALL THE DEFAULT GLVU MOUSE HANDLER
  if (CallingDefaultMouseFuncsOn) glvu.Mouse(button,state,x,y);
}

void userMotionFunc0(int x, int y)
{
  printf("%d %d\n", x,y);

  // OPTIONAL: CALL THE DEFAULT GLVU MOTION HANDLER
  if (CallingDefaultMouseFuncsOn) glvu.Motion(x,y);
}

//----------------------------------------------------------------------------
// USER-PROVIDED KEYBOARD HANDLING ROUTINE
//----------------------------------------------------------------------------
void userKeyboardFunc0(unsigned char Key, int x, int y)
{
  switch(Key)
  {
    case ' ':
      CallingDefaultMouseFuncsOn = !CallingDefaultMouseFuncsOn;
      if (CallingDefaultMouseFuncsOn) printf("KEY: CallingDefaultMouseFuncsOn = 1\n");
      else printf("KEY: CallingDefaultMouseFuncsOn = 0\n");
      break;
  };

  // OPTIONAL: CALL THE DEFAULT GLVU KEYBOARD HANDLER
  glvu.Keyboard(Key,x,y);
}

//----------------------------------------------------------------------------
// MAIN ROUTINE. INIT USER OBJECTS AND ACTIVATE VIEWER.
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
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
  glutMouseFunc(userMouseFunc0);
  glutMotionFunc(userMotionFunc0);
  glutKeyboardFunc(userKeyboardFunc0);

  Vec3f ModelMin(-1,-1,-1), ModelMax(1,1,1), 
        Eye(1,2,3), LookAtCntr(0,0,0), Up(0,1,0);
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
