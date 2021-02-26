//============================================================================
// plyview_binfile: PLY viewer that reads in objects from a .bin/.index file
//
// USER COMMANDS:
//
// standard GLVU interface plus the following keystrokes:
//
// ! : reset view directions to default
//
// ` : enable/disable random object colors (also need *)
//
// * : enable/disable objects' inherent colors
//
// I : left-click somewhere in window and get back an object ID 
//     (useful for "What am I looking at?" queries)
//
//============================================================================

#include <glvu.hpp>

//----------------------------------------------------------------------------
// USER-PROVIDED INCLUDES
//----------------------------------------------------------------------------
#include <GL/glut.h>
#include <plymodel.hpp>
#include <vector>
#include <string.h>
#include <camera.hpp>
#include <camutils.hpp>
#include <object_container.hpp>

using namespace std;

// float ambientColor[4] = { 0, 0, 0, 1 };
float ambientColor[4] = { 0.1, 0.1, 0.1, 1 };
float diffuseColor[4] = { 1, 1, 1, 1 };
// float diffuseColor[4] = { 0, 0, 0, 1 };

float black[4] = { 0, 0, 0, 1 };
float grey20[4] = { 0.2, 0.2, 0.2, 1 };

float specularColor[4] = { 1, 1, 1, 1};
float lightPosition0[4] = { -1, 0, 0, 0 };
float lightPosition1[4] = { 1, 1, 0, 0 };


// ---------------------------------------------------------------------- 
// LOCAL PROTOTYPES
// ---------------------------------------------------------------------- 

int useRandomColor = 0;
int doLighting = 1;
int useGlobalAmbient = 1;
int useObjectColor = 1;
int selectObjectMode = 0;
int renderObjectIDs = 0;
int selectX = 0;
int selectY = 0;
int viewportWidth;
int viewportHeight;
int drawBoundingBoxes = 0;
int mode_drawNormals = 0;

#ifndef MIN
# define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#ifndef MAX
# define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#define OBJECT_MENU_ID_BASE 10000

//----------------------------------------------------------------------------
// GLOBALS
//----------------------------------------------------------------------------

GLVU glvu;

// These three things will hold all the objects in the world.
vector<PlyModel *> worldObjects;
vector<Vec3f> objectColors; 
vector<int> groupEnabled;
vector<const char *> groupNames;
vector<char *> filenames;

Vec3f worldMin, worldMax;
int groupMenuID;


/* ---------------------------------------------------------------------- */

// View frustum culling.

int
VFC(Vec3f min, Vec3f max, Camera *cam)
{
  int result;

  result = CamMinMaxBoxOverlap(cam, min, max);

#ifdef CULL_DEBUG  
  printf("box (%f %f %f) - (%f %f %f): ",
	 min[0], min[1], min[2], 
	 max[0], max[1], max[2]);

  switch (result) {
  case COMPLETEOUT:
    printf("completely outside\n"); break;
  case PARTIAL:
    printf("partial overlap\n"); break;
  case COMPLETEIN:
    printf("completely inside\n"); break;
  }
#endif

  if (result == COMPLETEOUT) 
    return 1; // completely culled
  else
    return 0; // at least partially visible
}

/* ---------------------------------------------------------------------- */
  
//----------------------------------------------------------------------------
// USER-PROVIDED DRAWING ROUTINE
//----------------------------------------------------------------------------


void userDisplayFunc0()
{
  int i;
  Camera *cam;
  int numObjectsDisplayed = 0;
  GLfloat color[4];
  Vec3f eye, look, up;
  float y, aspect, neardist, fardist;

  int numPolys = 0;

  cam = glvu.GetCurrentCam();

  glvu.BeginFrame();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (doLighting) {
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      glEnable(GL_COLOR_MATERIAL);
      glLightfv(GL_LIGHT0, GL_AMBIENT, ambientColor);
      glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseColor);
      //    glLightfv(GL_LIGHT0, GL_SPECULAR, specularColor);
      glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
      glEnable(GL_LIGHT1);
      glLightfv(GL_LIGHT1, GL_AMBIENT, ambientColor);
      glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseColor);
      glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);
		
      // temp
      //      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);


    }

    GLenum err;

    err = glGetError();
    while (err != GL_NO_ERROR) {
      printf("gl error is %x\n", err);
      err = glGetError();
    }
    
    for (i = 0; i < worldObjects.size(); i++) {
      
      if (! VFC(glvu.GetCurrentCam(), worldObjects[i]->Min, worldObjects[i]->Max)) {
	if (renderObjectIDs) {
	  glColor3ub((i+1) % 256, ((i+1) >> 8) % 256, ((i+1) >> 16) % 256);
	  worldObjects[i]->Display(OBJ_NONE);
	} else {
	  if (drawBoundingBoxes) { 
	  worldObjects[i]->DrawAABB();
	  } else {
	    if (useObjectColor) {
	      worldObjects[i]->Display(OBJ_ALL);
	    } else {
	      if (useRandomColor) {
		glColor3fv((float *) objectColors[i]);
	      } else {
		glColor3f(1, 1, 1);
	      }
	      worldObjects[i]->Display(OBJ_NORMALS);
	    }
	  }
	  numPolys += worldObjects[i]->GetNumFaces();
	}
      }
    }
    glvu.EndFrame();
  
    // if we rendered object IDs, we're trying to identify an object,
    // so read back the framebuffer and decide what the user picked.
    // There's a bug in here that I haven't been able to track down,
    // but the nonsense trap seems to get it.
    if (renderObjectIDs) {
    unsigned char *framebuffer;
    GLint viewport[4];
    unsigned int objectID;
    unsigned char r, g, b;
    int offset;

    glGetIntegerv(GL_VIEWPORT, viewport);
    viewportWidth = viewport[2];
    viewportHeight = viewport[3];

    framebuffer = new unsigned char[viewport[2] * viewport[3] * 3];

    glReadPixels(0, 0, 
		 viewport[2], viewport[3], 
		 GL_RGB,
		 GL_UNSIGNED_BYTE,
		 framebuffer);

    offset = 3 * (selectY * viewport[2] + selectX);
    r = framebuffer[offset];
    g = framebuffer[offset+1];
    b = framebuffer[offset + 2];

    objectID = (r | (g << 8) | (b << 16));
    if (objectID == 0) {
      printf("SELECT: No object selected\n");
    } else {
      if (objectID-1 < filenames.size()) { 
	printf("SELECT: Object %d selected\n", objectID-1);
      } else {
	printf("SELECT: ERROR: Nonsense object ID %d, try again\n", objectID);
      }
    }

    selectObjectMode = 0;
    renderObjectIDs = 0;

    glutPostRedisplay();
    delete [] framebuffer;
  }
  //  printf("%d polys rendered\n", numPolys);
}

/* ---------------------------------------------------------------------- */

void
loadModelsFromBinFile( const char *prefix )
{
  ObjectContainer< PlyModel > archive;
  float r, g, b;

  bool status = archive.Read( prefix );

  if (status == false) { 
    printf("ERROR: Couldn't open bin file with prefix '%s'\n", prefix);
    exit(1);
  }

  for (int i = 0; i < archive.getNumObjects(); ++i) { 
    printf("Retrieving object %d\n", i);

    r = g = b = 0;

    PlyModel *p = archive.getObject(i);

    if (p != NULL) { 
      worldObjects.push_back(p);
    }

    while ((r*r + g*g + b*b) < 0.75) {
      r = drand48();
      g = drand48();
      b = drand48();
  }
  
  objectColors.push_back(Vec3f(r, g, b));

  }
}
 
/* ---------------------------------------------------------------------- */

void handleMouse(int button, int state, int x, int y)
{
  if (selectObjectMode) {
    if (state == GLUT_DOWN) {
      selectX = x;
      selectY = viewportHeight - y;
      renderObjectIDs = 1;
      glutPostRedisplay();
    } else {
      glvu.DefaultMouseFunc(button, state, x, y);
    }
  } else {
    glvu.DefaultMouseFunc(button, state, x, y);
  }
}

// ---------------------------------------------------------------------- 

void userKeyboardFunc0(unsigned char Key, int x, int y)
{
  Vec3f eye, look, up, ref;
  int i;
  PlyModel *m;

  switch(Key)
  {
  case '!':
    {
      glvu.GetCurrentCam()->GetLookAtParams(&eye, &look, &up);
      eye = Vec3f(0, 0, 0);
      look = Vec3f(0, 0, -1);
      up = Vec3f(0, 1, 0);
      glvu.GetCurrentCam()->LookAt(eye, look, up);
    }; break;

  case '`':
    {
      if (useRandomColor) {
	printf("Disabling random colors.\n");
	useRandomColor = 0;
      } else {
	printf("Enabling random per-object colors.\n");
	useRandomColor = 1;
      }
    }; break;

  
  case '+': {
    dumpWarpInfo();
  }; break;
    
  case 'l': {
    doLighting = 1 - doLighting;
    if (doLighting) {
      printf("lighting on\n");
    } else {
      printf("lighting off\n");
    }
    glutPostRedisplay();
  }; break;

  case 'a': {
    useGlobalAmbient = 1 - useGlobalAmbient;
    if (useGlobalAmbient) {
      printf("ambient lighting on\n");
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, grey20);
    } else {
      printf("ambient lighting off\n");
      glLightModelfv(GL_LIGHT_MODEL_AMBIENT, black);
    }
    glutPostRedisplay();
  }; break;
  case 'B': { 
    drawBoundingBoxes = 1 - drawBoundingBoxes;
  }; break;

  case '*': {
    useObjectColor = 1 - useObjectColor;
    if (useObjectColor) {
      printf("object color enabled\n");
    } else {
      printf("object color disabled\n");
    }
    glutPostRedisplay();
  }; break;

  case 'I': {
    printf("Next left-click will identify filename of object under cursor.\n");
    selectObjectMode = 1;
  }; break;
  }

  // OPTIONAL: CALL THE DEFAULT GLVU KEYBOARD HANDLER
  glvu.Keyboard(Key,x,y);

  glutPostRedisplay();
}


 
//----------------------------------------------------------------------------
// MAIN ROUTINE. INIT USER OBJECTS AND ACTIVATE VIEWER.
//----------------------------------------------------------------------------
int main(int argc, char **argv)
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
 
  int i, j;
  char *progname;
  bool viewpoint_present = false;
  float view_x=-1, view_y=-1, view_z=-1;

  if (argc == 1) {
    fprintf(stderr, 
	    "ERROR: Need some files to render!\n");
    exit(1);
  }

  progname = argv[0];

  
  while (--argc > 0 && (*++argv)[0]=='-') {
      switch (*(argv[0]+1)) {
      case 'v': /* viewpoint */
	view_x = atof(*++argv);
	view_y = atof(*++argv);
	view_z = atof(*++argv);
	viewpoint_present = true;
	argc -= 3;
	break;
      default:
	printf("usage: %s [-v x y z] binfilePrefix ...\n", progname);
	printf("\t-v x y z: specify initial viewpoint\n");
	return -1;
	break;
      }
  }

  loadModelsFromBinFile( argv[0] );

  glvu.Init("PLY File Viewer",
	    //            GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE,
	    GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA,
            50,50,800,800);

  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

  glutDisplayFunc(userDisplayFunc0);

  //  initWorld(argc, argv);
  //  initMenus();

  worldMin = worldObjects[0]->Min;
  worldMax = worldObjects[0]->Max;

  // Find the bounding box of the whole world
  for (i = 1; i < worldObjects.size(); i++) {
    Vec3f low, high;
    
    low = worldObjects[i]->Min;
    high = worldObjects[i]->Max;

    for (j = 0; j < 3; j++) {
      worldMin[j] = MIN(worldMin[j], low[j]);
      worldMin[j] = MIN(worldMin[j], high[j]);
      worldMax[j] = MAX(worldMax[j], low[j]);
      worldMax[j] = MAX(worldMax[j], high[j]);
    }
  }
	

  printf("world bounding box: (%f %f %f) - (%f %f %f)\n",
	 worldMin[0], worldMin[1], worldMin[2],
	 worldMax[0], worldMax[1], worldMax[2]);
    
  // Find the polygon count of the world
  int totalPolyCount = 0;
  for (i = 0; i < worldObjects.size(); i++) {
    totalPolyCount += worldObjects[i]->GetNumTris();
  }

  printf("WORLD: %d polygons in %d objects\n",
	 totalPolyCount, worldObjects.size());


  Vec3f m=worldMin, M=worldMax, LookAtCntr=(m+M)*0.5;
  Vec3f Eye(1,4,8), Up(0,0,1);
  float Yfov = 45;
  float Aspect = 1;     // WIDTH OVER HEIGHT
  float Near = 1000.0;    // NEAR PLANE DISTANCE RELATIVE TO MODEL DIAGONAL LENGTH
  float Far = 10000000.0f;    // FAR PLANE DISTANCE (ALSO RELATIVE)
  float eyeUp;
  float standoff;

  eyeUp = M[1] - LookAtCntr[1];

  standoff = MAX(MAX(M[0]-m[0], M[1]-m[1]), M[2] - m[2]);

  Up = Vec3f(0, 1, 0);
  Eye = LookAtCntr + Vec3f(0, 0, standoff);
  Near = 1;
  Far = 100000;
  
  //  Near = 0.01 * standoff;
  //  Far = 100 * standoff;
  printf("near %f far %f\n", Near, Far);
  glvu.SetAllCams(m,M, Eye,LookAtCntr,Up, Yfov,Aspect, Near,Far);
  glDepthFunc(GL_LESS);

  Camera *cam = glvu.GetCurrentCam();
  cam->Perspective(90, 1.0, Near, Far);
  cam->LookAt(Eye, LookAtCntr,Up);

  cam->Perspective(90, 1.0, 1000, 10000000);

#if 0
  cam->Perspective(90, 1.0, 1000, 10000000);
  cam->LookAt(Vec3f(0, 0, 2), 
	      Vec3f(0, 0, 1), 
	      Vec3f(0, 1, 0));
  glutDisplayFunc(display_drawPoints);

#endif
  glutKeyboardFunc(userKeyboardFunc0);
  glutMouseFunc(handleMouse);

  glDisable(GL_CLIP_PLANE0);
  glDisable(GL_CLIP_PLANE1);
  glDisable(GL_CLIP_PLANE2);


  //--------------------------------------------------------------------------
  // (3) start the viewer event loop.
  //--------------------------------------------------------------------------


  glutMainLoop();
  return 0;
}

// ---------------------------------------------------------------------- 

char *
bool2string(bool foo)
{
  if (foo == true) return "true";
  else return "false";
}
