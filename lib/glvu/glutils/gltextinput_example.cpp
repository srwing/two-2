//------------------------------------------------------------------------------
// File : gltextinput_example.cpp
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
// gltextinput example
//============================================================================
#include <glvu.hpp>
#include <vec3fv.hpp>
#include "glutils.hpp"
#include "gltextinput.hpp"

//----------------------------------------------------------------------------
// USER-PROVIDED INCLUDES
//----------------------------------------------------------------------------
#include <GL/glut.h>
#include "glvutext.hpp"
using namespace GLVUText;

//----------------------------------------------------------------------------
// GLOBALS
//----------------------------------------------------------------------------
GLVU glvu;

int g_moveLightMode = 0;
GLfloat light0_position[]= { 1,1,1,0 };

GLTextInputField g_textfield;

void commitFunc(GLTextInputField *field)
{
  printf("Hello from the custom commit func\n");
  printf("You typed \"%s\"\n", field->getText());
  field->close();
}


//----------------------------------------------------------------------------
// USER-PROVIDED DRAWING ROUTINE
//----------------------------------------------------------------------------
void userDisplayFunc0()
{
  glvu.BeginFrame(); {
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,0,0);
    glutSolidTorus(0.25,1,32,64);
  }

  glColor3f(1.0f,1.0f,1.0f);
  g_textfield.draw( );

  glvu.EndFrame();
}

//----------------------------------------------------------------------------
// USER-PROVIDED MOUSE HANDLING ROUTINE
//----------------------------------------------------------------------------
void userMouseFunc0(int button, int state, int x, int y)
{
  if (! g_textfield.filterMouseButton(button,state,x,y) )
    glvu.Mouse(button,state,x,y);
}

void userMotionFunc0(int x, int y)
{
  bool needsuper = false;
  static int oldx = x;
  static int oldy = y;
  if (g_moveLightMode) {
    // rotate light around
    float M[16];
    float tmpv[4];
    float xvec[] = { 0,1,0 }; 
    float yvec[] = { 0,0,1 }; 
    int dx = x - oldx;
    int dy = oldy - y;
    Rotate16fv(M, float(dx)/100.0*90.0,xvec);
    Mult16fv4fv(tmpv, M, light0_position);
    Rotate16fv(M, float(dy)/100.0*90.0,yvec);
    Mult16fv4fv(light0_position, M, tmpv);
    Normalize3fv(light0_position);
    oldx = x;
    oldy = y;
    glutPostRedisplay();
  }
  else {
    needsuper = true;
  }

  if (needsuper)
    glvu.Motion(x,y);
}

//----------------------------------------------------------------------------
// USER-PROVIDED KEYBOARD HANDLING ROUTINE
//----------------------------------------------------------------------------
void userKeyboardFunc(unsigned char Key, int x, int y)
{
  bool needsuper = false;
  if (g_textfield.filterKeyDown(Key, x, y)) return;
  switch (Key) {
    case 'p':
      g_moveLightMode = true;
      // turn off inertia
      glvu.SetInertiaEnabled(false);      
      break;
    default:
      needsuper = true;
      break;
  }

  if (needsuper) {
    glvu.Keyboard(Key,x,y);
  }
}

//----------------------------------------------------------------------------
// USER-PROVIDED KEYBOARD HANDLING ROUTINE
//----------------------------------------------------------------------------
void userSpecialKeyboardFunc(int Key, int x, int y)
{
  bool needsuper = false;
  if (g_textfield.filterSpecialKeyDown(Key, x, y)) return;

}

//----------------------------------------------------------------------------
// USER-PROVIDED KEYBOARD HANDLING ROUTINE
//----------------------------------------------------------------------------
void userKeyboardUpFunc(unsigned char Key, int x, int y)
{
  switch (Key) {
    case 'p':
      g_moveLightMode = false;
      glvu.SetInertiaEnabled(true);
      break;
    case ' ':
      g_textfield.setLabel("Type something:");
      g_textfield.setCommitFunc(commitFunc);
      //g_textfield.setAlignment(GLTextInputField::ALIGN_RIGHT);
      SetCurrentFont(GLUT_BITMAP_HELVETICA_18);
      g_textfield.show();
      break;
    default:
      break;
  }
}


//----------------------------------------------------------------------------
// MAIN ROUTINE. INIT USER OBJECTS AND ACTIVATE VIEWER.
//----------------------------------------------------------------------------
void main()
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

  glvu.Init("GLVU Text Input Example - Hit space bar for input field",
            GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA,
            50,50,512,512);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

  GLfloat light0_specular[] = { 1.0,1.0,1.0,1.0 };
  GLfloat light1_position[] = { 1,1,1,0 };
  GLfloat mat_specular[] = { 0.2,0.2,0.2,1.0 };
  GLfloat mat_shiny[] = {100.0};

  glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);

  glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light0_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shiny);

  glutDisplayFunc(userDisplayFunc0);
  glutMouseFunc(userMouseFunc0);
  glutMotionFunc(userMotionFunc0);
  glutKeyboardFunc(userKeyboardFunc);
  glutSpecialFunc(userSpecialKeyboardFunc);
  glutKeyboardUpFunc(userKeyboardUpFunc);

  Vec3f ModelMin(-1,-1,-1), ModelMax(1,1,1), 
        Eye(1,2,3), LookAtCntr(0,0,0), Up(0,1,0);
  float Yfov = 45;
  float Aspect = 1;     // WIDTH OVER HEIGHT
  float Near = 0.1f;    // NEAR PLANE DISTANCE RELATIVE TO MODEL DIAGONAL LENGTH
  float Far = 10.0f;    // FAR PLANE DISTANCE (ALSO RELATIVE)
  glvu.SetAllCams(ModelMin,ModelMax, Eye,LookAtCntr,Up, Yfov,Aspect, Near,Far);
 
  GLerror("After setup");

  //--------------------------------------------------------------------------
  // (3) start the viewer event loop.
  //--------------------------------------------------------------------------

  glutMainLoop();
}
