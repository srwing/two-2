//------------------------------------------------------------------------------
// File : glvu_keyinput.cpp
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
// glvu_keyinput.cpp : used to add keyboard key events to the viewer.
//============================================================================

#include "glvu.hpp"

/// The default GLUT keyboard function implementation
/**
 * This just calls GetGLVU()->Keyboard(), thus allowing Object-Oriented
 * people to customize GLVU's behavior by overriding the Keyboard()
 * method instead of by dealing directly with GLUT's callbacks.
 *
 * @see Keyboard, DefaultDisplayFunc, DefaultInertiaFunc, DefaultReshapeFunc,
 *     DefaultMouseFunc, DefaultMotionFunc 
 */
void GLVU::DefaultKeyboardFunc(unsigned char Key, int x, int y)
{
  GetGLVU()->Keyboard(Key, x, y);
}

/// Handler for keyboard events
/**
 * The default implementation handles all the GLVU default key bindings.
 * Override this method to add your own key bindings, but if you don't handle 
 * the key, be sure to call the superclass (i.e. call GLVU::Keyboard()) 
 *
 * This only handles "normal" key events, i.e. those that correspond
 * to a key with an ascii character. GLUT also has \c glutSpecialFunc
 * that can be called to set a handler for other key events like the
 * F-keys, or arrow keys.  There are also "Up" versions of the key
 * events in GLUT that can be used to detect key release as well as 
 * key press.
 *
 * Users not interested in creating an object-oriented app can simply
 * call GLUT's \c glutKeyboardFunc to set a callback directly.  If you
 * do so, however, you should call GetCurrent()->Keyboard() or 
 * GLVU::DefaultKeyboardFunc(), in your handler when you don't use the 
 * key.
 *
 * @see DefaultKeyboardFunc, DefaultReshapeFunc, DefaultDisplayFunc, 
 *     DefaultInertiaFunc, DefaultMouseFunc, DefaultMotionFunc 
 */
void GLVU::Keyboard(unsigned char Key, int x, int y)
{
  int iModifiers = glutGetModifiers();
  CtrlPressed    = (iModifiers & GLUT_ACTIVE_CTRL) != 0;   // IF CTRL BIT SET
  AltPressed     = (iModifiers & GLUT_ACTIVE_ALT) != 0;    // IF ALT BIT SET
  ShiftPressed   = (iModifiers & GLUT_ACTIVE_SHIFT) != 0;  // IF SHIFT BIT SET

  switch(Key)
  {
    case 27: EscapeHandler(-1); break;

    case 'z': MainMenuHandler(0); break;
    case 'h': MainMenuHandler(1); break;
    case 'x': MainMenuHandler(2); break;
    case 'c': MainMenuHandler(3); break;
    case 'v': MainMenuHandler(4); break;

    case '=': vuOptionsMenuHandler(1); break;
    case 'o': vuOptionsMenuHandler(2); break;
    case 'r': vuOptionsMenuHandler(3); break;
    case 'i': vuOptionsMenuHandler(4); break;

    case 'w': glOptionsMenuHandler(2); break;
    case 'b': glOptionsMenuHandler(3); break;
    case 'l': glOptionsMenuHandler(8); break;
    case 'n': glOptionsMenuHandler(9); break;
    case 'm': glOptionsMenuHandler(10); break;

    case '1': CamViewMenuHandler(0); break;
    case '2': CamViewMenuHandler(1); break;
    case '3': CamViewMenuHandler(2); break;
    case '4': CamViewMenuHandler(3); break;

    case '!': CamViewDisplayMenuHandler(0); break;
    case '@': CamViewDisplayMenuHandler(1); break;
    case '#': CamViewDisplayMenuHandler(2); break;
    case '$': CamViewDisplayMenuHandler(3); break;
  };
}

