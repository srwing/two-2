//------------------------------------------------------------------------------
// File : glvu_mouse.cpp
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
// glvu_mouse.cpp : this routine provides the user-interface for world navigation
//   and the inertial system that allows for repeated motion upon application
//   of sufficient force. The user can bypass the normal mouse input handlers
//   by calling SetNewMouseHandlers(); the normal viewer mouse handling can
//   then be restored by calling RestoreMouseHandlers();
//============================================================================

#include "glvu.hpp"

// SUPPORT FOR INERTIAL SYSTEM

#ifndef ABS
template <class T>
static bool ABS(T x)
{
  return (((x)<0)?(-(x)):x);  
}
#endif
#define MINMOVE 2  // AMOUNT (in pixels) TO MOVE TO INVOKE INERTIAL SYSTEM

/// The default GLUT mouse button callback implementation
/**
 * This just calls GetGLVU()->Mouse(), thus allowing Object-Oriented
 * people to customize GLVU's behavior by overriding the Mouse()
 * method instead of by dealing directly with GLUTs callbacks.
 *
 * @see Mouse, DefaultDisplayFunc, DefaultInertiaFunc, DefaultKeyboardFunc,
 *     DefaultMotionFunc, DefaultReshapeFunc 
 */
void GLVU::DefaultMouseFunc(int button, int state, int x, int y)
{
  GetGLVU()->Mouse(button, state, x, y);
}

/// Handler for mouse clicks (called when a button is pressed or released)
/**
 * This method can be overridden to perform application specific
 * funcionality (e.g. picking).  The default implementation does some
 * important work for handling camera manipulation (world
 * navigation), so if you override this method you should always call
 * GLVU::Mouse() from your override.
 * 
 * Users not interested in creating an object-oriented app can simply
 * call GLUT's \c glutMouseFunc to set a callback directly.  If you
 * do so you can still call GetCurrent()->Mouse() or 
 * GLVU::DefaultMouseFunc(), in your handler to get the default 
 * behavior back.
 *
 *  @param button One of \c GLUT_LEFT_BUTTON, \c GLUT_MIDDLE_BUTTON, 
 *         or \c GLUT_RIGHT_BUTTON
 *  @param state One of \c GLUT_UP or \c GLUT_DOWN
 *  @param x,y The pointer location when the event took place 
 *         in window coordinates.
 * 
 *  @see DefaultMouseFunc, Motion, SetWorldNavMode
 */
void GLVU::Mouse(int button, int state, int x, int y)
{
  int iModifiers = glutGetModifiers();
  CtrlPressed    = (iModifiers & GLUT_ACTIVE_CTRL) != 0;   // IF CTRL BIT SET
  AltPressed     = (iModifiers & GLUT_ACTIVE_ALT) != 0;    // IF ALT BIT SET
  ShiftPressed   = (iModifiers & GLUT_ACTIVE_SHIFT) != 0;  // IF SHIFT BIT SET

  // SET APPROPRIATE FLAGS FOR A LEFT-BUTTON MOUSE EVENT
  if (button==GLUT_LEFT_BUTTON)
  {
    // STORE THE NEW MOUSE POS
    NewX=x; NewY=y;

    if (state==GLUT_DOWN)  // LEFT-BUTTON DOWN
    {
      OldX=x; OldY=y;      // RESET THE OLD TO THE CURRENT (starting over)
      LeftButtonDown=true; // SET LEFT-BUTTON-DOWN FLAG
      SetInertiaOn(0);     // TURN-OFF INERTIA WHEN USER PRESSES LEFT-BUTTON
    }
    else if (LeftButtonDown)    // LEFT-BUTTON UP after LEFT-BUTTON DOWN
    {
      LeftButtonDown=false;    // UNSET LEFT-BUTTON-DOWN FLAG

      // INVOKE THE INERTIAL SYSTEM IF THE LEFT BUTTON IS BEING RELEASED, THE
      // AMOUNT OF MOVEMENT IF "ENOUGH" AS DEFINED BY MINMOVE (IN PIXELS), AND
      // THE GLOBAL InertiaEnabled FLAG IS SET (CAN BE SET BY SetInertiaEnabled).
      if ((ABS(NewX-OldX) >= MINMOVE) || (ABS(NewY-OldY) >= MINMOVE)) 
        if (InertiaEnabled)
          SetInertiaOn(1);
    }
  }
  else if (button==GLUT_RIGHT_BUTTON) {
    if (state==GLUT_DOWN) RightButtonDown = true;
    else                  RightButtonDown = false;
  }
  else if (button==GLUT_MIDDLE_BUTTON) {
    if (state==GLUT_DOWN) MiddleButtonDown = true;
    else                  MiddleButtonDown = false;
  }
  if (state == GLUT_DOWN) {
    OldX = x;
    OldY = y;
  }
}

/// The default GLUT motion function implementation
/**
 * This just calls GetGLVU()->Motion(), thus allowing Object-Oriented
 * people to customize GLVU's behavior by overriding the Motion()
 * method instead of by dealing directly with GLUTs callbacks.
 *
 * @see Motion, DefaultDisplayFunc, DefaultInertiaFunc, DefaultKeyboardFunc,
 *     DefaultMouseFunc, DefaultReshapeFunc 
*/
void GLVU::DefaultMotionFunc(int x, int y)
{
  GetGLVU()->Motion(x, y);
}

/// Handler for 'active' mouse drag events, i.e. dragging with a button pressed.
/**
 *  This method can be overridden to perform application specific
 *  funcionality (e.g. direct manipulation of scene objects).  The
 *  default implementation does some important work for handling
 *  camera manipulation (world navigation), so if you override this
 *  method you should always call GLVU::Mouse() from your override if
 *  you wish to preserve the built-in navigation capabilities.  
 *
 *  The exact effect the default implementation has on the current 
 *  camera depends upon the current world navigation mode.  
 *  See SetWorldNavMode().
 *
 *  Users not interested in creating an object-oriented app can simply
 *  call GLUT's \c glutMotionFunc to set a callback directly.  If you
 *  do so you can still call GetCurrent()->Motion() or 
 *  GLVU::DefaultMotionFunc(), in your handler to get the default 
 *  behavior back.
 *
 *  This is hooked up to the \c glutMotionFunc().  GLUT also provides
 *  a \c glutPassiveMotionFunc() which can be used to handle mouse
 *  motion when there are no mouse buttons pressed.  GLVU does not
 *  have a wrapper for that one, however.
 * 
 *  @param x,y The most recent location of the pointer.
 * 
 *  @see DefaultMouseFunc, Motion, SetWorldNavMode */
void GLVU::Motion(int x, int y)
{
  // STORE PREVIOUS NEW MOUSE POSITION (OLD)
  OldX=NewX; OldY=NewY;
  if (LeftButtonDown)
  {
    // STORE THE NEW MOUSE POSITION
    NewX=x;    NewY=y;

    int WW = glutGet((GLenum)GLUT_WINDOW_WIDTH);  // GET THE WINDOW DIMENSIONS
    int WH = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

    switch(GetWorldNavMode())
    {
      // -------  WORLD NAVIGATION -------
      case NAV_MODE_TRACKBALL:
        if (CtrlPressed) { TrackBallX(OldX,NewX,WW); DriveY(OldY,NewY,WH); }
        else { TrackBallX(OldX,NewX,WW); TrackBallY(OldY,NewY,WH); }
        break;
      case NAV_MODE_HYPERBALL:
        if (CtrlPressed) { 
          HyperBall(OldX,OldY,NewX,OldY,WW,WH); DriveY(OldY,NewY,WH); }
        else { HyperBall(OldX,OldY,NewX,NewY,WW,WH); }
        break;
      case NAV_MODE_DRIVE:
        if (CtrlPressed) { TranslateX(NewX,OldX,WW); TranslateY(NewY,OldY,WH); }
        else if (AltPressed) { LookX(OldX,NewX,WW); LookY(OldY,NewY,WH); }
        else { LookX(OldX,NewX,WW); DriveY(OldY,NewY,WH); }
        break;
      case NAV_MODE_TRANSLATE:
        TranslateX(OldX,NewX,WW); 
        if (CtrlPressed) DriveY(OldY,NewY,WH); else TranslateY(OldY,NewY,WH);
        break;
      case NAV_MODE_LOOK:
        if (CtrlPressed) { TranslateX(OldX,NewX,WW); TranslateY(OldY,NewY,WH); }
        else if (AltPressed) { LookX(OldX,NewX,WW); DriveY(OldY,NewY,WH); }
        else { LookX(OldX,NewX,WW); LookY(OldY,NewY,WH); }
        break;
    };
    glutPostRedisplay();
  }
}


//----------------------------------------------------------------------------
// INERTIAL SYSTEM
//----------------------------------------------------------------------------

/// The default intertia function implementation
/**
 * This just calls GetGLVU()->Inertia(), thus allowing Object-Oriented
 * people to customize GLVU's behavior by overriding the Inertia()
 * method instead of by dealing with callbacks.
 *
 * Unlike the other methods of its ilk, this does not correspond to any GLUT
 * callback, it is purely a GLVU invention.
 *
 * @see Inertia, DefaultDisplayFunc, DefaultReshapeFunc, DefaultKeyboardFunc,
 *     DefaultMouseFunc, DefaultMotionFunc 
 */
void GLVU::DefaultInertiaFunc(int x, int y)
{
  GetGLVU()->Inertia(x, y);
}

/// Handler for inertia events
/**
 * The default implementation calls the Motion() method, causing the
 * camera to move a little more in the direction it was moving when
 * inertia kicked in.
 *
 * This method only gets called when inertia has been triggered by an
 * appropriate mouse drag and release action.
 *
 * @param x, y 
 * @see DefaultInertiaFunc, Motion, Mouse, Keyboard, Reshape, Display 
 */
void GLVU::Inertia(int x, int y)
{
  GLVU::Motion(x, y);
}

/// Call the inertia handler after some setup
/**
 * The way inertia is handled is to trick GLVU into thinking it is
 * getting the exact same mouse motion again and again, i.e, that the
 * mouse was dragged from x1,y1 to x2,y2 repeatedly.  This method munges the 
 * various internal data members as necessary to pull this trick off, then 
 * calls the inertia callback.
 * 
 * Returns TRUE (1) if inertia is enabled, FALSE (0) otherwise.
 * @see SetInertiaOn, Inertia
 */
//----------------------------------------------------------------------------
int GLVU::DoInertia()
{
  if (InertiaOn)
  {
    // SIMPLY REPEATEDLY CALL THE MOUSE MOVE HANDLER (MUST SET APPROPRIATE STATES)
    int tNewX=NewX, tNewY=NewY;          // COPY NEW VALUES TO TEMPS
    NewX=OldX; NewY=OldY;                // COPY OLD TO NEW (MOUSE MOVE COPIES)
    LeftButtonDown=true;                 // "PRETEND" BUTTON IS PRESSED
    if (UserInertiaFunc)
      UserInertiaFunc(tNewX, tNewY);     // CALL MOUSE MOVE HANDLER (INDIRECTLY)
    LeftButtonDown=false;                // RESET BUTTON STATE
    return(1);
  }
  return(0);
}

/// Turn inertia on or off.
/**
 * This is not about whether inertia is \em enabled or not, but whether 
 * it is currently active.  Usually called internally only.
 *
 * @see Inertia, DoInertia, SetInertiaEnabled
 */
void GLVU::SetInertiaOn(int onOrOff)
{
  InertiaOn = onOrOff;
  if (InertiaOn)
    InertialTimerFunc(WindowID);
}

void GLVU::InertialTimerFunc(int winID)
{
  GLVU *g = GLVUs[winID];
  if (glutGetWindow()!=winID) glutSetWindow(winID);
  g->DoInertia();
  if (g->InertiaOn) 
    glutTimerFunc(g->InertiaDelay, InertialTimerFunc, g->WindowID);
}
