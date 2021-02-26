//------------------------------------------------------------------------------
// File : glvu_menu.cpp
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
// glvu_menu.cpp : creates standard viewer menus
//   Used to add additional pop-up menus to the main viewer menu.
//   Several viewer global state variables are also defined in menu.cpp;
//   access function are provided.
//============================================================================

#include "glvu.hpp"
#include "snapshot.hpp"  // JUST FOR SCREEN SNAPSHOT MENU OPTION

void GLVU::MainMenuHandler(int value)
{
  switch(value)
  {
  case NAV_MODE_TRACKBALL:
  case NAV_MODE_HYPERBALL:
  case NAV_MODE_DRIVE:
  case NAV_MODE_TRANSLATE:
  case NAV_MODE_LOOK:

/*
  WVB: This messes up anyone who sets their own mouse function
  (pretty common).  Usually they will call DefaultMouseFunc
  themselves in the override, if they want this type of NAV_MODE
  behavior.  If they DON'T set their own mouse func then this isn't
  necessary at all.

  glutMouseFunc(DefaultMouseFunc);   // SET DEFAULT MOUSE FUNCS
  glutMotionFunc(DefaultMotionFunc);
*/
      GLVU *g = GetGLVU();
      g->WorldNavMode=value;
      g->SetInertiaOn(0);
      break;
  };
}

void GLVU::vuOptionsMenuHandler(int value)
{
  Camera *Cam;
  Vec3f Eye, ViewRefPt, ViewUp;
  float Yfov, Aspect, Ndist, Fdist;

  switch(value)
  {
    case 1: 
      SnapShot(); 
      break;
    case 2:  // TOGGLE BETWEEN INSIDE-LOOKING-OUT AND OUTSIDE-LOOKING-IN NAV MODES
      GetGLVU()->ToggleInOutMode();
      break;
    case 3:  // RESTORE ALL CAMS TO ORIGINAL (STARTING) CAM POS
      GetGLVU()->AllCamsResetToOrig();
      glutPostRedisplay();
      break;
    case 4: {
      GLVU *g = GetGLVU();
      if (g->GetInertiaEnabled()) { g->SetInertiaEnabled(0); g->SetInertiaOn(0); }
      else g->SetInertiaEnabled(1); }
      break;
    case 5: 
      Cam = GetGLVU()->GetCurrentCam();
      Cam->GetLookAtParams(&Eye,&ViewRefPt,&ViewUp);
      Cam->GetPerspectiveParams(&Yfov,&Aspect,&Ndist,&Fdist);
      printf("--- CURRENT CAM PARAMS ---\n");
      printf("       Eye: "); Eye.Print();
      printf("LookAtCntr: "); ViewRefPt.Print();
      printf("    ViewUp: "); ViewUp.Print();
      printf("     Y FOV: %f\n", Yfov);  
      printf("    Aspect: %f\n", Aspect);
      printf("      Near: %f\n", Ndist);
      printf("       Far: %f\n", Fdist);
      break;
  };
}

void GLVU::glOptionsMenuHandler(int value)
{
  int State;
  int PolygonState[2];

  switch(value)
  {
    case 2: 
      glGetIntegerv(GL_POLYGON_MODE,PolygonState);
      if (PolygonState[0]==GL_POINT) glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); 
      else if (PolygonState[0]==GL_FILL) {
        glGetIntegerv(GL_LIGHT_MODEL_TWO_SIDE,&State);
        glPolygonMode(State?GL_FRONT_AND_BACK:GL_FRONT,GL_LINE);
      }
      else glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      break;
    case 3:
      if (glIsEnabled(GL_CULL_FACE)) glDisable(GL_CULL_FACE); else glEnable(GL_CULL_FACE);
      break;
    case 6:
      glGetIntegerv(GL_SHADE_MODEL,&State);
      if (State==GL_SMOOTH) glShadeModel(GL_FLAT); else glShadeModel(GL_SMOOTH);
      break;
    case 8:
      if (glIsEnabled(GL_LIGHTING)) glDisable(GL_LIGHTING); else glEnable(GL_LIGHTING);
      break;
    case 7:
      glGetIntegerv(GL_LIGHT_MODEL_TWO_SIDE,&State);
      if (State == GL_TRUE) 
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
      else
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      break;
    case 1:
      glGetIntegerv(GL_LIGHT_MODEL_LOCAL_VIEWER,&State);
      if (State == GL_TRUE) 
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
      else
        glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
      break;
    case 9:
      glGetIntegerv(GL_CULL_FACE_MODE,&State);
      if (State==GL_BACK) glCullFace(GL_FRONT); else glCullFace(GL_BACK);
      break;
    case 10:
      if (glIsEnabled(GL_COLOR_MATERIAL)) glDisable(GL_COLOR_MATERIAL); else glEnable(GL_COLOR_MATERIAL);
      break;
  };
  glutPostRedisplay();
}

void GLVU::PathPlaybackMenuHandler(int value)
{
  switch(value)
  {
    case 1: 
      GetGLVU()->StartRecording("cam_record0.dat"); 
      break;
    case 2: 
      GetGLVU()->StartPlayback("cam_record0.dat"); 
      break;
    case 3: 
      GetGLVU()->StopRecordingPlayback(); 
      break;
  };
}

void GLVU::CamViewMenuHandler(int value)
{
  GetGLVU()->SelectCam(value);
  glutPostRedisplay();
}

void GLVU::CamViewDisplayMenuHandler(int value)
{
  GLVU *g = GetGLVU();
  if (g->CamDisplayOn[value]) g->CamDisplayOn[value]=0; 
  else g->CamDisplayOn[value]=1;
  glutPostRedisplay();
}

void GLVU::EscapeHandler(int value)
{
  exit(1);
}

#define GLVU_PATH_PLAY_RECORD    1
#define GLVU_PATH_PLAY_PLAY      2
#define GLVU_PATH_PLAY_STOP      3

#define GLVU_CAMVIEW_0           0
#define GLVU_CAMVIEW_1           1
#define GLVU_CAMVIEW_2           2
#define GLVU_CAMVIEW_3           3 

#define GLVU_VU_OPTIONS_SNAPSHOT    1
#define GLVU_VU_OPTIONS_INOUT_MODE  2
#define GLVU_VU_OPTIONS_RESET_VIEW  3
#define GLVU_VU_OPTIONS_INERTIA     4
#define GLVU_VU_OPTIONS_DUMP_CAM    5

#define GLVU_GL_OPTIONS_POLYMODE    2


/**
 * Sets up all of the default GLVU menu choices.  These are bound to the 
 * GLUT right-click menu.  You can subclass GLVU to override this method
 * if you need to implement a custom menu.
 * You can also get the GLUT ID for the menu by calling GetMainMenuID().
 *
 * This method is  called automatically from Init() and InitWin().
 */
void GLVU::InitMenu()
{
  int i;
  int PathPlayMenu = glutCreateMenu(PathPlaybackMenuHandler);
   glutAddMenuEntry("Record",1);
   glutAddMenuEntry("Play",  2);
   glutAddMenuEntry("Stop",  3);

  char Buffer[10];
  int CamViewMenu = glutCreateMenu(CamViewMenuHandler);
   for (i=0; i<NumCams; i++)
     { sprintf(Buffer,"Cam (%d)",i);
       glutAddMenuEntry(Buffer,i); }

  int CamViewDisplayMenu = glutCreateMenu(CamViewDisplayMenuHandler);
   glutAddMenuEntry("Cam 0 (!)",0);
   glutAddMenuEntry("Cam 1 (@)",1);
   glutAddMenuEntry("Cam 2 (#)",2);
   glutAddMenuEntry("Cam 3 ($)",3);

  int vuOptionsMenu = glutCreateMenu(vuOptionsMenuHandler);
   glutAddMenuEntry("SnapShot (=)",1);
   glutAddMenuEntry("InOut/OutIn (o)",2);
   glutAddMenuEntry("Reset Orig Views (r)",3);
   glutAddMenuEntry("Inertia (i)",4);
   glutAddMenuEntry("Dump Cam Params",5);

  int glOptionsMenu = glutCreateMenu(glOptionsMenuHandler);
   glutAddMenuEntry("Solid/Wire/Pt (w)",2);
   glutAddMenuEntry("Smooth/Flat Shading", 6);
   glutAddMenuEntry("Lighting (l)",8);
   glutAddMenuEntry("One/Two Sided Lighting",7);
   glutAddMenuEntry("Inf/Local Viewer",1);
   glutAddMenuEntry("Materials (m)",10);
   glutAddMenuEntry("Face Culling (b)",     3);
   glutAddMenuEntry("Back/Front Cull Face (n)",9);

  MainMenuID = glutCreateMenu(MainMenuHandler);
   glutAddMenuEntry("TrackBall (z)",    NAV_MODE_TRACKBALL);
   glutAddMenuEntry("HyperBall (h)",    NAV_MODE_HYPERBALL);
   glutAddMenuEntry("Drive (x)",        NAV_MODE_DRIVE);
   glutAddMenuEntry("Translate (c)",    NAV_MODE_TRANSLATE);
   glutAddMenuEntry("Look (v)",         NAV_MODE_LOOK);

   glutAddSubMenu("Viewer Options",vuOptionsMenu);
   glutAddSubMenu("OpenGL Options",glOptionsMenu);
   glutAddSubMenu("PathPlay",PathPlayMenu);
   glutAddSubMenu("Cam View",CamViewMenu);
   glutAddSubMenu("Cam Display",CamViewDisplayMenu);

  glutAttachMenu(GLUT_RIGHT_BUTTON);
}
