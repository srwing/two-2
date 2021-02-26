//------------------------------------------------------------------------------
// File : glvu.cpp
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
// glvu.cpp : OpenGL/GLUT -based viewer
//============================================================================

#include "glvu.hpp"

GLVU *GLVU::GLVUs[MAX_GLVUS];
static int GLVU_Initialized = 0;

/// The constructor
/**
 * Create a new GLVU object with default settings.
 *
 * @note To actually do anything useful with it you will need to call
 * either Init() or InitWin(), and probably SetAllCams() too.
 *
 * @see Init, InitWin 
 */
GLVU::GLVU()
{
  if (!GLVU_Initialized) {
    for (int i=0; i<MAX_GLVUS; i++)
      GLVUs[i]=NULL;
    GLVU_Initialized = 1;
  }

  WindowID = -1;

  WorldNavMode=0;
  InsideLookingOutMode=0;
  NumCams=4;
  Cams = new Camera[NumCams];
  CamDisplayOn = new int[NumCams];
  for (int i=0;i<NumCams;i++) CamDisplayOn[i]=0;
  Cam = &(Cams[0]);
  MainMenuID=0;
  LeftButtonDown=0;
  RightButtonDown=0;
  MiddleButtonDown=0;
  OldX=0; OldY=0; NewX=0; NewY=0;
  moveSpeed = 1;
  CtrlPressed=0;
  AltPressed=0;
  ShiftPressed=0;
  InertiaOn=0, 
  InertiaEnabled=1;
  InertiaDelay=10;
  SetInertiaFunc(NULL);
  calcFPS = 0;
  lastFPS = 0.0;
  lastFPSCount = 0;
  RecordingOn = 0;
  PlaybackOn = 0;
  RecordPlaybackFP = 0;
}

/// The destructor
/**
 * Virtual since GLVU has virtual methods and allocates some members on the heap. 
 */
GLVU::~GLVU()
{
  delete[] Cams;
  delete[] CamDisplayOn;
}

/// Initialize a viewer
/**
 * Create a GLVU window and initialize it, via GLUT calls. This must
 * be done \em before calling \c glutMainLoop(). OpenGL calls can be made
 * after this to set state for lights, textures, materials, etc.  This
 * version creates its own GLUT window with a call to
 * \c glutCreateWindow().  If you already have a glut window handy for
 * whatever reason, you can call InitWin() instead, to wrap a GLVU
 * around that window.
 *
 * Also set up default keyboard, mouse, and reshape callback with GLUT.
 *
 * @param WindowTitle What should appear on the window's title bar
 * (and in the task manager on Windows, or on the icon on an XWindows
 * desktop).
 *
 * @param VisualMode a set of GLUT flags combined with logical or'ing.
 * These are passed to glutIinitDisplayMode before creating the
 * window.  Possible values include \c GLUT_DEPTH, \c, GLUT_DOUBLE, \c
 * GLUT_ALPHA, \c GLUT_ACCUM, and on SGIs the all-important \c
 * GLUT_MULTISAMPLE.  See GLUT documentation for more information.
 *
 * @param WindowStartX Desired X position for the window's upper left corner.
 * @param WindowStartX Desired Y position for the window's upper left corner.
 * @param WindowWidth Desired width of window in pixels
 * @param WindowHeight Desired height of window in pixels
 * 
 * @note \em Every GLVU object should have this method called on it
 *       (this or InitWin()).
 *
 * @see InitWin */
int GLVU::Init(char *WindowTitle, 
               unsigned int VisualMode,
               int WindowStartX, int WindowStartY,
               int WindowWidth, int WindowHeight)
{
  // OPENGL/GLUT STUFF STUFF TO CREATE WINDOW
  glutInitDisplayMode(VisualMode);
  glutInitWindowPosition(WindowStartX,WindowStartY);
  glutInitWindowSize(WindowWidth,WindowHeight);
  WindowID = glutCreateWindow(WindowTitle);  // STARTS AT WindowID=1

  // DO GLVU INITIALIZATION PART
  return InitWin(WindowID);
}

/// Initialize a viewer with an existing GLUT window
/**
 * Set a GLVU window to use the existing GLUT window referred to by \p
 * wID.  This is something you way wish to do if you are integrating
 * with a legacy GLUT application, for instance, or if you need to use
 * some special GLUT functionality to create a window with a
 * particular display mode.
 *
 * Also set up default keyboard, mouse, and reshape callback with GLUT.
 * 
 * @param wID The window identifier obtained from GLUT.
 *
 * @see Init
 */
int GLVU::InitWin(int wID)
{
  // MAKE SURE GLUT LIKES THIS ID
  glutSetWindow(wID);

  // IF WE GET HERE THEN GLUT LIKED THE WINDOW ID
  WindowID = wID;

  // ADD WINDOW TO STATIC GLVU ARRAY; FIRST CHECK IF TOO MANY ARE BEING ADDED
  if (WindowID>=MAX_GLVUS) { 
    printf("ERROR: too many windows added!\n");
    glutDestroyWindow(WindowID);
    WindowID=0;
    return(WindowID); 
  }
  GLVUs[WindowID] = this;

  // REGISTER PER-WINDOW DEFAULT CALLBACKS
  glutKeyboardFunc(DefaultKeyboardFunc);
  glutMouseFunc(DefaultMouseFunc);
  glutMotionFunc(DefaultMotionFunc);
  glutReshapeFunc(DefaultReshapeFunc);
  glutDisplayFunc(DefaultDisplayFunc);
  SetInertiaFunc(DefaultInertiaFunc);

  InitMenu();

#if defined(DEBUG) | defined(_DEBUG)
  PrintVisualInfo();
#endif

  return(WindowID);
}

/// The default GLUT display function implementation
/**
 * This just calls GetGLVU()->Display(), thus allowing Object-Oriented
 * people to customize GLVU's behavior by overriding the Display()
 * method instead of by dealing directly with GLUTs callbacks.
 *
 * @see Display, DefaultReshapeFunc, DefaultInertiaFunc, DefaultKeyboardFunc,
 *     DefaultMouseFunc, DefaultMotionFunc         
 */
void GLVU::DefaultDisplayFunc()
{
  GetGLVU()->Display();
}

/// Handler for redrawing application OpenGL content.
/**
 * This method can be overridden to perform the application's actual
 * OpenGL drawing.  Typically one begins by calling BeginFrame() as
 * the first call, and ends with EndFrame() as the last call.  Those
 * two methods handle camera setup (path playback and recording),
 * buffer swapping for double buffered windows, and frame rate timing
 * calculations.
 *
 * The default implementation does nothing.
 *
 * In the default configuration, this method is called indirectly via
 * GLUTs \c glutDisplayFunc callback.  Users can override this method,
 * or register their own glut display callback with GLUT directly.  
 *
 * @see DefaultDisplayFunc, DefaultReshapeFunc, DefaultInertiaFunc, 
 *     DefaultKeyboardFunc, DefaultMouseFunc, DefaultMotionFunc
 */
void GLVU::Display()
{
}

/// The default GLUT display function implementation
/**
 * This just calls GetGLVU()->Reshape(), thus allowing Object-Oriented
 * people to customize GLVU's behavior by overriding the Reshape()
 * method instead of by dealing directly with GLUTs callbacks.
 *
 * @see Reshape, DefaultDisplayFunc, DefaultInertiaFunc, DefaultKeyboardFunc,
 *     DefaultMouseFunc, DefaultMotionFunc 
*/
void GLVU::DefaultReshapeFunc(int WW, int WH)
{
  GetGLVU()->Reshape(WW, WH);
}

/// Handler for changed window size
/**
 * Typically this method is overridden to handle setting up the
 * perspective matrix and viewport matrix.  It corresponds to the \c
 * \c glutReshapeFunc(), and is indirectly called via that very mechanism.  
 *
 * The default implementation is quite serviceable.  It sets the
 * viewport to be the whole visible window area, and adjusts all the
 * Cameras so that the aspect ratio of the perspective transformation
 * is not all whacked out.  One reason you might have to override this
 * is if you are implementing stereo using multiple viewports, or if
 * you are using multiple viewports for any other reason.
 *
 * Rather than overriding this method, you could also call
 * glutReshapeFunc directly, to register your function with GLUT.  If
 * you do so then this method will no longer get called. To invoke the
 * default functionality from your callback you can call
 * GetCurrent()->Reshape() or GLVU::DefaultReshapeFunc().
 *
 * @see DefaultReshapeFunc, DefaultDisplayFunc, DefaultInertiaFunc, 
 *     DefaultKeyboardFunc, DefaultMouseFunc, DefaultMotionFunc
 */
void GLVU::Reshape(int WW, int WH)
{
  glViewport(0, 0, WW, WH);  
  AllCamsPerspectiveAspectChange((GLfloat)WW/(GLfloat)WH);
}

/// Dump info about the selected visuals to standard out
/*
 * Currently, this is called automatically when a glut window is
 * initialized with Init() or InitWin().  Can't you tell that we're
 * researchers?  We use stdout, \em and we spit a bunch of geek
 * information out to it even if you don't explicitly ask.
 */
void GLVU::PrintVisualInfo()
{
  GLint i;
  GLboolean j;
  printf("GRAPHICS VISUAL INFO (# bits of each):\n");
  glGetIntegerv(GL_RED_BITS,&i);    printf("RGBA: %d ", i);
  glGetIntegerv(GL_GREEN_BITS,&i);  printf("%d ", i);
  glGetIntegerv(GL_BLUE_BITS,&i);   printf("%d ", i);
  glGetIntegerv(GL_ALPHA_BITS,&i);  printf("%d\n", i);
  glGetIntegerv(GL_ACCUM_RED_BITS,&i);    printf("Accum RGBA: %d ", i);
  glGetIntegerv(GL_ACCUM_GREEN_BITS,&i);  printf("%d ", i);
  glGetIntegerv(GL_ACCUM_BLUE_BITS,&i);   printf("%d ", i);
  glGetIntegerv(GL_ACCUM_ALPHA_BITS,&i);  printf("%d\n", i);
  glGetIntegerv(GL_INDEX_BITS,&i);  printf("Color Index: %d\n", i);
  glGetIntegerv(GL_DEPTH_BITS,&i);  printf("Depth: %d\n", i);
  glGetIntegerv(GL_STENCIL_BITS,&i);  printf("Stencil: %d\n", i);
  glGetBooleanv(GL_DOUBLEBUFFER,&j); printf("Double Buffer?  %d\n", j);
  glGetBooleanv(GL_STEREO,&j); printf("Stereo Buffer?  %d\n", j);
  glGetIntegerv(GL_AUX_BUFFERS,&i);  printf("# Aux Buffers: %d\n", i);
}

/// Print information about current OpenGL errors. (Debug only)
void GLVU::CheckForGLError( char *msg )
{
#if defined(DEBUG) | defined(_DEBUG)
 GLenum errCode;
 const GLubyte *errStr;
 if ((errCode = glGetError()) != GL_NO_ERROR) 
 {
    errStr = gluErrorString(errCode);
    fprintf(stderr,"OpenGL ERROR: %s: %s\n", errStr, msg);
 }
#endif
}
