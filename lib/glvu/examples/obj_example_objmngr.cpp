//------------------------------------------------------------------------------
// File : obj_example_objmngr.cpp
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
// obj_example_2objs.cpp : 2 object example
//============================================================================
#include <glvu.hpp>

//----------------------------------------------------------------------------
// USER-PROVIDED INCLUDES
//----------------------------------------------------------------------------
#include <GL/glut.h>
#include <mat44.hpp>
#include <objmngr.hpp>
#include <objmodel.hpp>
#include <trimodel.hpp>

//----------------------------------------------------------------------------
// GLOBALS
//----------------------------------------------------------------------------
GLVU glvu;
ObjectManager *ObjMngr;
//----------------------------------------------------------------------------
// USER-PROVIDED DRAWING ROUTINE
//----------------------------------------------------------------------------
void userDisplayFunc0()
{
  // ANIMATION
  static float X=0, Y=0, dX=0.1f, dY=0.13f, MX=6, MY=6;
  X+=dX;
  Y+=dY;
  if (X<-MX || X>MX) { dX=-dX; X+=dX; }
  if (Y<-MY || Y>MY) { dY=-dY; Y+=dY; }

  glvu.BeginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ObjMngr->ObjInstances[0]->O.Set(X,Y,0);
    ObjMngr->ObjInstances[1]->O.Set(-X,-Y,0);
    ObjMngr->Display();
  glvu.EndFrame();
}

void userIdleFunc0()
{
  glutPostRedisplay();
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

  glvu.Init("GLVU Object Manager Basic Example",
            GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA,
            50,50,512,512);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

  glutDisplayFunc(userDisplayFunc0);
  glutIdleFunc(userIdleFunc0);

  ObjModel *Obj0 = new ObjModel("models/obj/al.obj");
  TriModel *Obj1 = new TriModel("models/tri/tie.tri");

  ObjMngr = new ObjectManager();
  ObjMngr->AddObject(Obj0);
  ObjMngr->AddObject(Obj1);

  Vec3f m=Obj0->Min, M=Obj0->Max, LookAtCntr=(m+M)*0.5;
  Vec3f Eye(0,0,20), Up(0,1,0);
  float Yfov = 45;
  float Aspect = 1;     // WIDTH OVER HEIGHT
  float Near = 0.1f;    // NEAR PLANE DISTANCE RELATIVE TO MODEL DIAGONAL LENGTH
  float Far = 10.0f;    // FAR PLANE DISTANCE (ALSO RELATIVE)
  glvu.SetAllCams(m,M, Eye,LookAtCntr,Up, Yfov,Aspect, Near,Far);

  //--------------------------------------------------------------------------
  // (3) start the viewer event loop.
  //--------------------------------------------------------------------------

  glutMainLoop();

  // Control flow will never reach here
  return EXIT_SUCCESS;
}
