//============================================================================
// example_basic_oo.cpp : most basic GLVU example using OO style
//============================================================================
#include <glvu.hpp>

//----------------------------------------------------------------------------
// USER-PROVIDED INCLUDES
//----------------------------------------------------------------------------
#include <GL/glut.h>
#include <stdio.h>


//----------------------------------------------------------------------------
// GLVU SUBCLASS DEFINITION
//----------------------------------------------------------------------------
class myGLVU : public GLVU
{
  protected:
     //virtual void InitMenu();
  
  public:
     myGLVU() {};
     virtual ~myGLVU() { 
       // GLUT never returns from its main loop, but C++ gives
       // us a way to clean up anyway.  This gets called even if the
       // window is closed by the user.
       printf("<insert application cleanup here.>\n");
     }

     virtual void Display();
     //virtual void Mouse(int button, int state, int x, int y);
     //virtual void Motion(int x, int y);
     //virtual void Reshape(int WW, int WH);
     // virtual void Keyboard(unsigned char Key, int x, int y);
};

//----------------------------------------------------------------------------
// USER-PROVIDED METHOD OVERRIDES
//----------------------------------------------------------------------------
void myGLVU::Display()
{
  BeginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1,0,0);
    glutSolidTorus(0.25,1,32,64);
  EndFrame();
}

///// OTHER POSSIBLE METHOD OVERRIDES -- See glvu.h for definitive list //////
// void myGLVU::InitMenu() {}
// void myGLVU::Mouse(int button, int state, int x, int y) {}
// void myGLVU::Motion(int x, int y) {}
// void myGLVU::Reshape(int WW, int WH) {}
// void myGLVU::Keyboard(unsigned char Key, int x, int y) {}


//----------------------------------------------------------------------------
// GLOBALS
//----------------------------------------------------------------------------
myGLVU glvu;


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

  glvu.Init("GLVU Basic Object Oriented Example",
            GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA,
            50,50,512,512);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

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
}
