//============================================================================
// simpleviewer.cpp : very simple model viewer GLVU example
//============================================================================
#include <glvu.hpp>

//----------------------------------------------------------------------------
// USER-PROVIDED INCLUDES
//----------------------------------------------------------------------------
#include <GL/glut.h>
#include <objmodel.hpp>
#include <plymodel.hpp>
#include <trimodel.hpp>

#include <vector>

using std::vector;

//----------------------------------------------------------------------------
// GLOBALS
//----------------------------------------------------------------------------
GLVU glvu;
typedef vector<Object *> ObjVec;
typedef ObjVec::iterator ObjVecIter;
ObjVec Objs;

//----------------------------------------------------------------------------
// USER-PROVIDED DRAWING ROUTINE
//----------------------------------------------------------------------------
void userDisplayFunc0()
{
  glvu.BeginFrame();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ObjVecIter oi = Objs.begin();
    for (; oi!= Objs.end(); ++oi) {
      Object *o = *oi;
      o->Display();
    }
  glvu.EndFrame();
}

//----------------------------------------------------------------------------
// MAIN ROUTINE. INIT USER OBJECTS AND ACTIVATE VIEWER.
//----------------------------------------------------------------------------
void main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Usage: %s [model.{ply|tri|obj}]+\n", argv[0]);
    exit(1);
  }

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

  glvu.Init("GLVU Simple Model Viewer",
            GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE,
            50,50,512,512);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);

  glutDisplayFunc(userDisplayFunc0);

  Vec3f m( 1e15, 1e15, 1e15);
  Vec3f M(-1e15,-1e15,-1e15);

  for (int i=1; i<argc; i++) {
    const char *f = argv[i];
    int flen = strlen(f);
    Object *obj = 0; 
    if (!strcmp(f+flen-4, ".ply")) {
      obj = new PlyModel(f);
    }
    else if (!strcmp(f+flen-4, ".obj")) {
      obj = new ObjModel(f);
    }
    if (!strcmp(f+flen-4, ".tri")) {
      obj = new TriModel(f);
    }
    if (obj) {
      Objs.push_back(obj);
      obj->Min.UpdateMinMax(m,M);
      obj->Max.UpdateMinMax(m,M);
      printf("Loaded file %s, %d tris\n", f, obj->GetNumTris());
    }
    else {
      printf("Unknown model type or unable to load: %s\n", f);
    }
  }

  Vec3f LookAtCntr=(m+M)*0.5;
  Vec3f Up(0,1,0);
  Vec3f mtoCtr = LookAtCntr - m;
  Vec3f Eye(LookAtCntr - 3*(mtoCtr - Up * (mtoCtr * Up) ));
  float Yfov = 45;
  float Aspect = 1;     // WIDTH OVER HEIGHT
  float Near = 0.1f;    // NEAR PLANE DISTANCE RELATIVE TO MODEL DIAGONAL LENGTH
  float Far = 10.0f;    // FAR PLANE DISTANCE (ALSO RELATIVE)
  glvu.SetAllCams(m,M, Eye,LookAtCntr,Up, Yfov,Aspect, Near,Far);

  //--------------------------------------------------------------------------
  // (3) start the viewer event loop.
  //--------------------------------------------------------------------------

  glutMainLoop();
}
