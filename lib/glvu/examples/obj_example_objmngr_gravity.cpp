//------------------------------------------------------------------------------
// File : obj_example_objmngr_gravity.cpp
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
// obj_example_objmngr_vu.cpp : object manager viewer extensions
//============================================================================
#include <glvu.hpp>

//----------------------------------------------------------------------------
// USER-PROVIDED INCLUDES
//----------------------------------------------------------------------------
#include <GL/glut.h>
#include <iostream.h>
#include <mat44.hpp>
#include <objmngr.hpp>
#include <objmodel.hpp>
#include <trimodel.hpp>
#include <groundplane.hpp>
#include <glbuffers.hpp>
#include <glutils.hpp>

//----------------------------------------------------------------------------
// GLOBALS
//----------------------------------------------------------------------------
GLVU glvu;
ObjectManager *OM;      // THE OBJECT MANAGER PTR
GroundPlane *gp;
ObjectFrame *GP;        // The ground for objects to fall upon
int SelectedObj;        // CURRENTLY SELECTED OBJECT ID
int ObjManipMode=0;     // 0=ROTATE, 1=TRANSLATE, 2=SCALE

bool firstTime = true;
bool grav = false;

int LeftButtonDown, OldX, OldY, NewX, NewY;  

GLfloat *depth = NULL;
GLdouble collisionModelView[16], collisionProj[16];
GLint vp[4];

void idleFunc() {
  glutPostRedisplay();
}

void SetupCollisionTest() {
  vp[0] = 0; vp[1] = 0;
  vp[2] = 64; vp[3] = 64;

  GLint pvp[4];
  glGetIntegerv(GL_VIEWPORT, pvp);  
  glViewport(vp[0], vp[1], vp[2], vp[3]);

  glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(0, 0, 0, 0, -10, 0, 0, 0, -1);
   glGetDoublev(GL_MODELVIEW_MATRIX, collisionModelView);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-20, 20, -20, 20, 5, 25);
   glGetDoublev(GL_PROJECTION_MATRIX, collisionProj);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDepthFunc(GL_ALWAYS);
  GP->FastDisplay();
  int width, height;
  SaveDepthBuffer(depth, width, height, GL_BACK);
  glDepthFunc(GL_LESS);

  glViewport(pvp[0], pvp[1], pvp[2], pvp[3]);
  
  // init forces:
  for (int i = 0; i < OM->NumObjInstances; i++) {
    OM->ObjInstances[i]->SetUnits(10.0f);
    OM->ObjInstances[i]->SetMass(1.0f);
    OM->ApplyGravity(i);
  }
  OM->ResetTime();

  //DrawDepthBuffer(depth, width, height, GL_BACK);
  //glutSwapBuffers();
  //int a; cin >> a;
}

#define TODEGREES 57.29578

void Reposition(ObjectFrame *Obj, Vec3f collisionPt) {
  Vec3f obbVerts[8];
  Obj->GetOBBverts(obbVerts);
  float minY = 100000.0;
  for (int i = 0; i < 8; i++)
    if (obbVerts[i].y < minY) minY = obbVerts[i].y;
  
  Obj->TranslateObj(Vec3f(0, collisionPt.y - minY, 0)); 
}

// reorients the object to lie on floor
void Reorient(ObjectFrame *Obj, Vec3f collisionPt) {
  float xdot, ydot, zdot;
    
  Vec3f up(0, 1, 0), best, x, y, z;
  xdot = Obj->X*up;
  if (xdot > 0) x = Obj->X;
  else { x = -Obj->X; xdot = -xdot; }
  ydot = Obj->Y*up;
  if (ydot > 0) y = Obj->Y;
  else { y = -Obj->Y; ydot = -ydot; }
  zdot = Obj->Z*up;
  if (zdot > 0) z = Obj->Z;
  else { z = -Obj->Z; zdot = -zdot; }

  float l = xdot; best = x;
  if (l < ydot) { l = ydot; best = y; }
  if (l < zdot) { l = zdot; best = z; }

  if (fabs(1.0-l) < 1.0e-4) {
    Reposition(Obj, collisionPt);
    return;
  }

  Vec3f rotAxis = best^Vec3f(0, 1, 0);
  float ang = acos(best.y)*TODEGREES;
  Obj->RotateObjAboutMinMaxCntr(ang, rotAxis);

  Reposition(Obj, collisionPt);
}

bool CollisionTest(int whichObject)
{
  Vec3f collisionPt;
  if (OM->CollisionTest(depth, vp, collisionModelView, collisionProj,
			whichObject, collisionPt, GL_BACK)) { 
    OM->ObjInstances[whichObject]->ResetVelocity();
    OM->ObjInstances[whichObject]->ResetForce();
    Reorient(OM->ObjInstances[whichObject], collisionPt);
  }
  return true;
}

//----------------------------------------------------------------------------
// USER-PROVIDED DRAWING ROUTINE
//----------------------------------------------------------------------------
void userDisplayFunc0()
{
  if (firstTime) {
    SetupCollisionTest(); firstTime = false; glutIdleFunc(idleFunc);
  }
  // FOR EACH FRAME, WE CALL UPDATE PATH RECORDING AND PLAYBACK OF OBJS
  OM->UpdatePathRecPlayback();

  OM->UpdateAllPositions();

  for (int i = 0; i < OM->NumObjInstances; i++)
    if (OM->ObjInstances[i]->IsMoving())
      CollisionTest(i);
  
  glvu.BeginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    OM->Display();
    GP->Display();
  glvu.EndFrame();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ObjMouseFunc0(int button, int state, int x, int y)
{
  // STORE THE NEW MOUSE POS
  NewX=x; NewY=y;

  int WW = glutGet((GLenum)GLUT_WINDOW_WIDTH);  // GET THE WINDOW DIMENSIONS
  int WH = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

  Vec3f Start, Dir;

  if (button==GLUT_LEFT_BUTTON)
  {
    if (state==GLUT_DOWN)  // LEFT-BUTTON DOWN
    {
      OldX=x; OldY=y;      // RESET THE OLD TO THE CURRENT (starting over)
      LeftButtonDown=1;

      // ---- OBJECT MANIPULATION HANDLER FOR LEFT-BUTTON DOWN ----
      Camera *cCam = glvu.GetCurrentCam();
      // GET PIXEL RAY FOR OLD POS
      cCam->GetPixelRay(NewX,NewY,WW,WH,&Start,&Dir);
      Dir.Normalize();                             // NORMALIZE DIRECTION
      SelectedObj = OM->SelectObject(Start,Dir);
      if (SelectedObj>=0)
      {
        if (glutGetModifiers() & (GLUT_ACTIVE_ALT | GLUT_ACTIVE_CTRL))
          OM->ToggleSelect(SelectedObj);
        else
          if (!OM->IsAlreadySelected(SelectedObj))
          {
            OM->DeselectAll();
            OM->Select(SelectedObj);
          }
      }
      else
      {
        OM->DeselectAll();
      }
      glutPostRedisplay();
    }
    else                   // LEFT-BUTTON UP
    {
      LeftButtonDown=0;
    }
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ObjMotionFunc0(int x, int y)
{
  // STORE PREVIOUS NEW MOUSE POSITION (OLD) AND THE NEW MOUSE POSITION
  OldX=NewX; OldY=NewY;
  NewX=x;    NewY=y;

  Vec3f Eye2Cntr, t, v, T, V;
  float RelX, RelY;

  if (LeftButtonDown)
  {
    int WW = glutGet((GLenum)GLUT_WINDOW_WIDTH);  // GET THE WINDOW DIMENSIONS
    int WH = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

    if (SelectedObj>=0)                          // IF AN OBJECT WAS SELECTED
    {
     Camera *cCam = glvu.GetCurrentCam();

     switch(ObjManipMode)
     {
      case 0:  // ROTATION
         RelX=(NewX-OldX)/(float)WW;             // GET RELATIVE MOVEMENTS
         RelY=(NewY-OldY)/(float)WH;             // AS FRACTION OF SCREEN DIMS
         OM->RotateSelected(RelY*360,cCam->X);
         OM->RotateSelected(RelX*360,cCam->Y);
        break;
      case 1:  // TRANSLATION
         RelX=(NewX-OldX)/(float)WW;             // GET RELATIVE MOVEMENTS
         RelY=(NewY-OldY)/(float)WH;             // AS FRACTION OF SCREEN DIMS
         Eye2Cntr = OM->ObjCntr(SelectedObj) - cCam->Orig;
         t = (cCam->X * (RelX*(cCam->wR-cCam->wL))) +
             (cCam->Y * (-RelY*(cCam->wT-cCam->wB)));
         v = cCam->Z * -cCam->Near;
         V = (-cCam->Z) * (Eye2Cntr*(-cCam->Z));
         T = t * (V.Length() / v.Length());
         OM->TranslateSelected(T);              // APPLY OBJ TRANSLATION
        break;
      case 2: // SCALING
         RelY=(NewY-OldY)/(float)WH;       // AS FRACTION OF SCREEN DIMENSIONS
         OM->ScaleSelected(RelY*2+1);      // APPLY OBJ SCALING
        break;
     };
     // if we move, apply gravity again
     if (OM->ObjInstances[SelectedObj]->GetParent() == NULL) {
       OM->ObjInstances[SelectedObj]->ResetForce();
       OM->ObjInstances[SelectedObj]->ResetVelocity();
       OM->ApplyGravity(SelectedObj);
     }
    }
    glutPostRedisplay();
  }
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ObjPathPlaybackMenuFunc0(int value)
{
  switch(value)
  {
    case 1:
      glvu.StartRecording("cam_record0.dat"); 
      OM->StartRecording("obj_record0.dat");
      break;
    case 2: 
      glvu.StartPlayback("cam_record0.dat"); 
      OM->StartPlayback("obj_record0.dat");
      break;
    case 3: 
      glvu.StopRecordingPlayback(); 
      OM->StopRecordingPlayback();
      break;
  };
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ObjMenuFunc0(int value)
{
  switch(value)
  {
    case 0: case 1: case 2:
      glutMouseFunc(ObjMouseFunc0);
      glutMotionFunc(ObjMotionFunc0);
      ObjManipMode=value; // OBJECT MODES (0,1,2)
      break;
    case 4:               // SELECTED OBJECT DELETE
      OM->DeleteSelected();
      glutPostRedisplay();
      break;
    case 5:               // SELECTED OBJECT COPY
      OM->CopySelected();
      glutPostRedisplay();
      break;
  };
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ObjKeyboardFunc0(unsigned char Key, int x, int y)
{
  switch(Key)
  {
    case 'a': ObjMenuFunc0(0); break;
    case 's': ObjMenuFunc0(1); break;
    case 'd': ObjMenuFunc0(2); break;
    case '[': ObjMenuFunc0(4); break;
    case ']': ObjMenuFunc0(5); break;
    case 'r': ObjPathPlaybackMenuFunc0(1); break;
    case 'p': ObjPathPlaybackMenuFunc0(2); break;
    case 'o': ObjPathPlaybackMenuFunc0(3); break;
    case 'z':
    case 'x':
    case 'c': 
      glutMouseFunc(GLVU::DefaultMouseFunc);
      glutMotionFunc(GLVU::DefaultMotionFunc);
      glvu.Keyboard(Key,x,y); 
      break; 
    default: glvu.Keyboard(Key,x,y); break;
  };
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

  glvu.Init("GLVU Object Manager Viewer Extensions Example",
            GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA,
            50,50,512,512);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

  // CREATE ADDITIONAL MENUS FOR OBJECT MANIPULATION
  int ObjMenu = glutCreateMenu(ObjMenuFunc0);
   glutAddMenuEntry("Rotate (a)",   0);
   glutAddMenuEntry("Translate (s)",1);
   glutAddMenuEntry("Scale (d)",    2);
   glutAddMenuEntry("Delete ([)",   4);
   glutAddMenuEntry("Copy (])",     5);
  int ObjPathPlaybackMenu = glutCreateMenu(ObjPathPlaybackMenuFunc0);
   glutAddMenuEntry("Record (r)",   1);
   glutAddMenuEntry("Play (p)",   2);
   glutAddMenuEntry("Stop (o)",   3);

  int MainMenuID = glvu.GetMainMenuID();
  glutSetMenu(MainMenuID);
  glutAddSubMenu("Object Manip",ObjMenu);
  glutAddSubMenu("Object Paths",ObjPathPlaybackMenu);

  glutKeyboardFunc(ObjKeyboardFunc0);
  glutDisplayFunc(userDisplayFunc0);

  ObjModel *Obj0 = new ObjModel("models/obj/al.obj");
  TriModel *Obj1 = new TriModel("models/tri/tie.tri");
  
  gp = new GroundPlane(Vec3f(-20, -5, -20), Vec3f(20, 6, 20), 1.0, 0.0);
  GP = new ObjectFrame(gp);

  OM = new ObjectManager();
  OM->AddObject(Obj0);
  OM->AddObject(Obj1);

  Vec3f m=Obj0->Min, M=Obj0->Max, LookAtCntr=(m+M)*0.5;
  Vec3f Eye(0,0,20), Up(0,1,0);
  float Yfov = 45;
  float Aspect = 1;     // WIDTH OVER HEIGHT
  float Near = 0.1f;    // NEAR PLANE DISTANCE RELATIVE TO MODEL DIAGONAL LENGTH
  float Far = 40.0f;    // FAR PLANE DISTANCE (ALSO RELATIVE)
  glvu.SetAllCams(m,M, Eye,LookAtCntr,Up, Yfov,Aspect, Near,Far);

  //--------------------------------------------------------------------------
  // (3) start the viewer event loop.
  //--------------------------------------------------------------------------

  glutMainLoop();

  // Control flow will never reach here
  return EXIT_SUCCESS;
}
