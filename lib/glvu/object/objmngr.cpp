//------------------------------------------------------------------------------
// File : objmngr.cpp
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

//==========================================================================
// objmngr.cpp
//==========================================================================

#include "objmngr.hpp"
#include <glbuffers.hpp>
#include <glutils.hpp>
#include <mat44.hpp>
#include <ppm.hpp>

ObjectManager::ObjectManager()
{
  NumObjInstances=0;   // NONE ADDED YET
  NumObjInstancesAlloced=1;
  ObjInstances = new ObjectFrame*[NumObjInstancesAlloced];
  IsSelected = new int[NumObjInstancesAlloced];

  NumObjects=0;        // NONE ADDED YET
  NumObjectsAlloced=1;
  Objects = new Object*[NumObjectsAlloced];

  RecordingOn=PlaybackOn=0;

  ObjectDrawingOn=1;
  SelectionBoxDrawingOn=1;
  watch.Start();
}


ObjectManager::~ObjectManager()
{
  delete[] Objects;
  for (int i=0; i<NumObjInstances; i++)
    delete ObjInstances[i];
  delete[] ObjInstances;
  delete[] IsSelected;
}


//----------------------------------------------------------------------------
// Add/Delete routines for object instances (ObjectFrame array)
//----------------------------------------------------------------------------
void ObjectManager::AddObjectFrame(Object *Obj)
{
  // ADD AN OBJECT INSTANCE: GROW OBJECT FRAME LIST, IF NECESSARY
  if (NumObjInstances==NumObjInstancesAlloced) // IF SO, MUST GROW!
  {
    NumObjInstancesAlloced*=2;            // DOUBLE THE SIZE OF THE ARRAY
    ObjectFrame* *NewInstances = new ObjectFrame*[NumObjInstancesAlloced]; // ALLOCATE THE NEW ARRAY
    int *NewIsSelected = new int[NumObjInstancesAlloced];
    for (int i=0; i<NumObjInstances; i++) // COPY OVER THE OLD
      { NewInstances[i]=ObjInstances[i];
        NewIsSelected[i]=IsSelected[i]; }
    delete[] ObjInstances;                // FREE THE OLD ARRAY
    delete[] IsSelected;
    ObjInstances = NewInstances;          // COPY "HEAD" OF NEW ARRAY
    IsSelected = NewIsSelected;
  }

  // NORMAL ADD OF NEW OBJECT
  ObjInstances[NumObjInstances] = new ObjectFrame(Obj);
  IsSelected[NumObjInstances] = 0;             // BY DEFAULT, OBJECT IS NOT SELECTED
  NumObjInstances++;
}

void ObjectManager::DeleteObjectFrame(int iObjInstance)
{
  // PERFORM A SAFETY CHECK
  if (iObjInstance<0 || iObjInstance>=NumObjInstances)
    printf("ERROR (DeleteObjectFrame): iObjInstance out-of-bounds\n");

  // DELETE THE INSTANCE (WE CAN DELETE BECAUSE *WE* ALLOCED IT)
  delete ObjInstances[iObjInstance];

  // PACK THE ARRAY
  for (int i=iObjInstance+1; i<NumObjInstances; i++)
    { ObjInstances[i-1]=ObjInstances[i];
      IsSelected[i-1]=IsSelected[i]; }

  NumObjInstances--;
}


//----------------------------------------------------------------------------
// Add/Delete routines for objects (Object array). The user sees adding and deleting of
// object instances (instancing happens automatically). Internally we deal with
// adding/deleting of object frames, adding objects only when necessary.
//----------------------------------------------------------------------------
void ObjectManager::AddObject(Object* Obj)
{
  // FIRST SEE IF THE OBJECT IS ALREADY PRESENT IN THE OBJECT LIST;
  // IF SO, INSTANCE!
  int iObj=-1, i;
  for (i=0; i<NumObjects; i++)
    if (Objects[i] == Obj)         // JUST COMPARE OBJECT PTRS
    { iObj=i; break; }             // JUST BREAK OUT ASAP!

  // IF OBJECT EXISTS, ADD A ObjectFrame INSTANCE
  if (iObj>=0)
  {
    AddObjectFrame(Objects[iObj]);  // ADD AN INSTANCE ONLY
  }
  else // DO A NORMAL ADD OF AN OBJECT AND ITS ASSOCIATED INSTANCE
  {
    // ADD AN OBJECT: GROW OBJECT LIST, IF NECESSARY
    if (NumObjects==NumObjectsAlloced) // IF SO, MUST GROW!
    {
      NumObjectsAlloced*=2;            // DOUBLE THE SIZE OF THE ARRAY
      // ALLOCATE THE NEW ARRAY
      Object* *NewObjects = new Object*[NumObjectsAlloced]; 
      for (i=0; i<NumObjects; i++)                 // COPY OVER THE OLD
        NewObjects[i]=Objects[i];
      delete[] Objects;                            // FREE THE OLD ARRAY
      Objects = NewObjects;                        // COPY "HEAD" OF NEW ARRAY
    }
    Objects[NumObjects] = Obj;  // NORMAL ADD OF NEW OBJECT
    NumObjects++;               // ONE OBJECT ADDED

    AddObjectFrame(Obj);  // NORMAL ADD OF AN INSTANCE OF THE NEW OBJECT
  }
}

void ObjectManager::DeleteObject(int iObjInstance)
{
  // PERFORM A SAFETY CHECK
  if (iObjInstance<0 || iObjInstance>=NumObjInstances)
    printf("ERROR (DeleteObject): iObjInstance out-of-bounds\n");

  // STORE THE ADDRESS OF THE OBJECT POINTED TO BY THE INSTANCE TO BE DELETED
  Object *OldObj = ObjInstances[iObjInstance]->Obj;

  // GO AHEAD AND DELETE THE INSTANCE
  DeleteObjectFrame(iObjInstance);   

  // IF NO OTHER INSTANCES POINT TO THE OBJECT, DELETE THE OBJECT ITSELF
  int ObjectStillInUse=0, i;
  for (i=0; i<NumObjInstances; i++)
    if (ObjInstances[i]->Obj == OldObj)
    { ObjectStillInUse=1; break; }      // JUST BREAK OUT ASAP!

  // DELETE OBJECT ONLY IF NOT STILL IN USE
  if (!ObjectStillInUse)
  {
    // WE HAVE TO SEARCH OBJECTS TO FIND INDEX
    int iObj=-1;
    int i;
    for (i=0; i<NumObjects; i++)
      if (Objects[i] == OldObj)
      { iObj=i; break; }                // JUST BREAK OUT ASAP!
    if (iObj<0) printf("ERROR (DeleteObject): object not present to delete!\n");

    // WE ONLY PACK THE ARRAY OVER THE OLD OBJECT; WE CANNOT DELETE OBJ, *WE* DIDN'T ALLOC IT!
    for (i=iObj+1; i<NumObjects; i++)  
      Objects[i-1]=Objects[i];

    NumObjects--;
  }
}


//----------------------------------------------------------------------------
// Copy an object instance, same things as adding an object that already exists.
//----------------------------------------------------------------------------
void ObjectManager::CopyObject(int iObjInstance)
{
  // PERFORM A SAFETY CHECK
  if (iObjInstance<0 || iObjInstance>=NumObjInstances)
    printf("ERROR (CopyObject): iObjInstance out-of-bounds\n");

  AddObjectFrame(ObjInstances[iObjInstance]->Obj);
}

//----------------------------------------------------------------------------
// Updates object animation recording and playback - IF ENABLED. The user
// must call this at the beginning of every frame to support rec/playback.
// It is called in the ObjectManager::Display routine.
//----------------------------------------------------------------------------
void ObjectManager::UpdatePathRecPlayback()
{
  if (RecordingOn)
    WriteInstancesToFile(RecordPlaybackFP);
  else if (PlaybackOn) 
    if (!ReadInstancesFromFile(RecordPlaybackFP)) EndPlayback();
}

//----------------------------------------------------------------------------
// GLOBAL DISPLAY ROUTINE FOR ALL OBJECTS. 
//   If ObjectDrawingOn, normal drawing of objects with selection box around
//   selected objects. If !ObjectDrawingOn then only bounding boxes are
//   drawn for all objects (different color for selected objects).
//----------------------------------------------------------------------------
void ObjectManager::Display(unsigned int Attribs)
{
  UpdatePathRecPlayback();

  for (int i=0; i<NumObjInstances; i++)
  {
    if (ObjectDrawingOn) // NORMAL DRAWING OF OBJECTS
    {
      if (ObjInstances[i]->GetParent() == NULL) // parent will draw me
        ObjInstances[i]->Display(Attribs);  // OBJECT DRAWING
      if (IsSelected[i] && SelectionBoxDrawingOn) // SELECTION BOX DRAWING
        ObjInstances[i]->DrawOBB(1,1,1);              
    }
    else // NO DRAWING OF ACTUAL OBJECTS
    {
      if (IsSelected[i])                   // BOUNDING BOX DRAWING ONLY
        ObjInstances[i]->DrawOBB(1,1,1);              
      else
        ObjInstances[i]->DrawOBB(0.5,0.5,0.5);
    }
  }
}

//----------------------------------------------------------------------------
// UPDATING OF MINMAX_BOX
//   COMPUTES MIN_MAX OF ALL CONTAINED OBJECTS
//----------------------------------------------------------------------------
void ObjectManager::UpdateMinMax()
{
  if (NumObjInstances == 0)
  {
    Min.Set(0,0,0);
    Max.Set(0,0,0);
    return;
  }
  Vec3f min, max;
  Vec3f objmin, objmax;
  ObjInstances[0]->GetWorldMinMax(min, max);
  for (int i=1; i<NumObjInstances; i++)
  {
    ObjInstances[i]->GetWorldMinMax(objmin, objmax);
    objmin.UpdateMinMax(min, max);
    objmax.UpdateMinMax(min, max);
  }
  Min = min;
  Max = max;
}


//----------------------------------------------------------------------------
// TOGGLING OF DISPLAY PARAMS
//  1) TOGGLES THE OBJECT DRAWING FLAG (ON/OFF)
//  2) TOGGLE SELECTION BOX DRAWING (ON/OFF)
//----------------------------------------------------------------------------
void ObjectManager::ToggleObjectDrawing()
{
  if (ObjectDrawingOn) ObjectDrawingOn=0; else ObjectDrawingOn=1;
}

void ObjectManager::ToggleSelectionBoxDrawing()
{
  if (SelectionBoxDrawingOn) SelectionBoxDrawingOn=0; else SelectionBoxDrawingOn=1;
}

//----------------------------------------------------------------------------
// SELECTED OBJECT ROUTINES
//----------------------------------------------------------------------------
void ObjectManager::RotateSelected(float Ang, Vec3f &Axis)
{
  for (int i=0; i<NumObjInstances; i++)
    if (IsSelected[i]) ObjInstances[i]->RotateObj(Ang,Axis);
}

void ObjectManager::TranslateSelected(Vec3f &T)
{
  for (int i=0; i<NumObjInstances; i++)
    if (IsSelected[i]) ObjInstances[i]->TranslateObj(T);
}

void ObjectManager::ScaleSelected(float S)
{
  for (int i=0; i<NumObjInstances; i++)
    if (IsSelected[i]) ObjInstances[i]->ScaleObj(S);
}

void ObjectManager::DeleteSelected()
{
  int i=0;
  while (i<NumObjInstances)
  {
    if (IsSelected[i]) DeleteObject(i);
    else i++;
  }
}

void ObjectManager::CopySelected()
{
  for (int i=0; i<NumObjInstances; i++)
    if (IsSelected[i]) CopyObject(i);
}

//----------------------------------------------------------------------------
// SELECTION ROUTINES
//----------------------------------------------------------------------------
void ObjectManager::Select(int iObj)
{
  IsSelected[iObj]=1;
}

void ObjectManager::DeselectAll()
{
  for (int i=0; i<NumObjInstances; i++)
    IsSelected[i]=0;
}

void ObjectManager::ToggleSelect(int iObj)
{
  if (IsSelected[iObj]) IsSelected[iObj]=0;
  else IsSelected[iObj]=1;
}

int ObjectManager::IsAlreadySelected(int iObj)
{
  return(IsSelected[iObj]);
}

Vec3f ObjectManager::ObjCntr(int iObj)
{
  return(ObjInstances[iObj]->O);
}


//----------------------------------------------------------------------------
// Given a RAY (start and direction) in world coordinates emanating from
// the center-of-projection through a pixel on the screen, returns the index
// of the first object hit (-1 if no object is hit) The routine can also
// return the HitPt and the HitDist (in units of Dir's length) if an appropriate
// address is sent.
//----------------------------------------------------------------------------
int ObjectManager::SelectObject(Vec3f &Start, Vec3f &Dir, Vec3f *HitPt, float* HitDist)
{
  #define INFINITY 99999
  float NearestHitT=INFINITY, InT, OutT;
  int NearestObj=-1;
  for (int i=0; i<NumObjInstances; i++)
    if ( ObjInstances[i]->RayIsect(Start,Dir,InT,OutT) )
    {
      if (InT>=0 && InT<NearestHitT) { NearestHitT=InT; NearestObj=i; }
      if (OutT>=0 && OutT<NearestHitT) { NearestHitT=OutT; NearestObj=i; }
    }
  if (NearestObj>=0 && HitPt!=NULL)
    { *HitPt=Start+Dir*NearestHitT;
      if (HitDist!=NULL) *HitDist=NearestHitT; }
  return(NearestObj);
}


//----------------------------------------------------------------------------
// OBJECT INSTANCE POSE RECORDING AND PLAYBACK
//----------------------------------------------------------------------------
void ObjectManager::StartRecording(char *FileName)
{
  if (RecordingOn) { printf("Already recording!\n"); return; }
  if (PlaybackOn) EndPlayback();
  RecordPlaybackFP = fopen(FileName,"w");
  if (!RecordPlaybackFP) { printf("ERROR (StartRecording) fp==NULL\n"); return; }
  RecordingOn=1;
}

void ObjectManager::EndRecording()
{
  if (!RecordingOn) { printf("Not recording!\n"); return; }
  if (PlaybackOn) { printf("Playback is on! Not recording!\n"); return; }  // NOT SUPPOSED TO HAPPEN
  fclose(RecordPlaybackFP);
  RecordingOn=0;
}

void ObjectManager::StartPlayback(char *FileName)
{
  if (RecordingOn) EndRecording();
  if (PlaybackOn) EndPlayback();
  RecordPlaybackFP = fopen(FileName,"r");
  if (!RecordPlaybackFP) { printf("ERROR (StartPlayback) fp==NULL\n"); return; }
  PlaybackOn=1;

  printf("OBJ PLAYBACK...\n");
  PlaybackTime = clock();
}

void ObjectManager::EndPlayback()
{
  if (!PlaybackOn) { printf("Not playing back!\n"); return; }
  if (RecordingOn) { printf("Recording is on! Not playing back!\n"); return; } // NOT SUPPOSED TO HAPPEN
  fclose(RecordPlaybackFP);
  PlaybackOn=0;

  printf("%.2f seconds\n", (clock()-PlaybackTime)/(float)CLOCKS_PER_SEC);
}

void ObjectManager::StopRecordingPlayback()
{
  if (PlaybackOn) EndPlayback();
  else if (RecordingOn) EndRecording();
  else printf("Not recording or playing back!\n");
}

void ObjectManager::WriteInstancesToFile(FILE *fp)
{
  if (fp==NULL) { printf("ERROR (WriteInstancesToFile) fp==NULL\n"); return; }
  fprintf(fp, "%d\n", NumObjInstances);
  for (int i=0; i<NumObjInstances; i++)
    ObjInstances[i]->WriteToFile(fp);
}

int ObjectManager::ReadInstancesFromFile(FILE *fp)
{
  if (feof(fp)) return(0);
  if (fp==NULL) { printf("ERROR (ReadInstancesFromFile) fp==NULL\n"); return(0); }
  int tNumObjInstances;
  if (fscanf(fp,"%d\n", &tNumObjInstances)==EOF) return(0);
  if (tNumObjInstances!=NumObjInstances)
    { printf("ERROR (ReadInstancesFromFile) NumObjInstances in file differs from NumObjInstances in ObjMngr\n"); return(0); }
  for (int i=0; i<NumObjInstances; i++)
    if (!(ObjInstances[i]->ReadFromFile(fp))) return(0);
  return(1);
}


//----------------------------------------------------------------------------
// Retrieves the extents of the set of objects (Instances). This gives a
// bounding-box for the entire world.
//----------------------------------------------------------------------------
void ObjectManager::GetMinMax(Vec3f &Min, Vec3f &Max)
{
  if (NumObjInstances<=0) return;

  int j;
  Vec3f V[8];  // OBB VERTICES
  ObjInstances[0]->GetOBBverts(V);
  Min=Max=V[0];
  for (j=1; j<8; j++)
    V[j].UpdateMinMax(Min,Max);

  for (int i=1; i<NumObjInstances; i++)
  {
    ObjInstances[i]->GetOBBverts(V);
    for (j=0; j<8; j++)
      V[j].UpdateMinMax(Min,Max);
  }
}

const Vec3f gravity(0, -9.81f, 0);

void ObjectManager::ApplyGravity(int whichObject)
{
  if (whichObject < 0 || whichObject >= NumObjInstances) {
    fprintf(stderr,
	    "ObjectManager::ApplyGravity: Error: invalid Object index\n");
    return;
  }
  ObjInstances[whichObject]->ApplyForce(
    ObjInstances[whichObject]->GetMass() * gravity);
}

void ObjectManager::UpdatePosition(int whichObject)
{
  if (whichObject < 0 || whichObject >= NumObjInstances) {
    fprintf(stderr, 
	    "ObjectManager::UpdatePosition: Error: invalid Object index\n");
    return;
  }
  float deltaTime = watch.GetTime() - markedTime;
  markedTime += deltaTime;
  ObjInstances[whichObject]->UpdatePosition(deltaTime);
}

void ObjectManager::UpdateAllPositions()
{
  float deltaTime = watch.GetTime() - markedTime;
  markedTime += deltaTime;
  for (int i = 0; i < NumObjInstances; i++) {
    if (ObjInstances[i]->IsMoving()) {
      ObjInstances[i]->UpdatePosition(deltaTime);
    }
  }
}

// CollisionTest uses the depth buffer to test for collision with one
// of the objects managerd by the ObjectManager.  A precomputed
// depth buffer is needed for the geometry that is being tested against.
// Also the matrices from which this depth buffer was drawn, and the
// viewport parameters used to draw this depth buffer.  Returns
// true if any of the back faces of the object are deeper than what is in
// the depth buffer.  Also returns the world space coordinates of the
// point closest to the eye at which collision occured.  Accuracy and speed
// depend on the resolution of the depth buffer.  Speed also depends on the
// complexity of the object being tested.
bool ObjectManager::CollisionTest(GLfloat *depth,     // depth buffer to test
				  const GLint vp[4],  // viewport
				  GLdouble modelview[16], // mv matrix to use
				  GLdouble projection[16],// proj mat to use
				  int whichObject,    // object to test
				  Vec3f &collisionPt, // nearest collision pos
				  const GLint Buffer) // buffer to use
{
  if (whichObject < 0 || whichObject >= NumObjInstances) {
    fprintf(stderr,
	    "ObjectManager::CollisionTest: Error: invalid Object index\n");
    return false;
  }

  if (depth == NULL) {
    fprintf(stderr,
	    "ObjectManager::CollisionTest: Error: depth buffer is NULL\n");
    return false;
  }

  // first, get the screen area to search
  int i;
  GLdouble winX, winY, winZ;
  GLdouble minX = 10000, minY = 10000, maxX = -10000, maxY = -10000;
  Vec3f obbVerts[8];
  ObjInstances[whichObject]->GetOBBverts(obbVerts);
  for (i = 0; i < 8; i++) {
    gluProject(obbVerts[i].x, obbVerts[i].y, obbVerts[i].z,
	       modelview, projection, vp,
	       &winX, &winY, &winZ);
    if (winX < minX) minX = winX;
    if (winY < minY) minY = winY;
    if (winX > maxX) maxX = winX;
    if (winY > maxY) maxY = winY;
  }

  if (minX < 0) minX = 0;
  if (minY < 0) minY = 0;
  if (maxX >= vp[2]) maxX = vp[2] - 1;
  if (maxY >= vp[3]) maxY = vp[3] - 1;
  
  GLint OldDrawBuffer;
  glGetIntegerv(GL_DRAW_BUFFER,&OldDrawBuffer);
  glDrawBuffer((GLenum)Buffer);

  GLint pvp[4];
  glGetIntegerv(GL_VIEWPORT, pvp);
  glViewport(vp[0], vp[1], vp[2], vp[3]);

  glPushAttrib(GL_DEPTH_BUFFER_BIT); // push depth attribs
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_ALWAYS);

  // draw prestored depth into the depth buffer
  DrawDepthBuffer(depth, vp[2], vp[3], GL_BACK);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glDepthFunc(GL_GREATER);           // pass anything that collides
  
  // view from camera
  glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadMatrixd(projection);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadMatrixd(modelview);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glColor3f(1, 1, 1);
  ObjInstances[whichObject]->FastDisplay(); // draw object
  
  // Now, any pixels set in the color buffer will be locations of collision
  // scan the color buffer, and find the nearest point of collision
  GLubyte *color = NULL;
  int width, height;
  SaveColorBuffer(color, width, height, Buffer);
  
  float nearestz = 1000;
  int nearestx = 0; int nearesty = 0;
  //for (int i = 0; i < width*height; i++) {
  for (int v = minY; v < maxY; v++) {
    for (int h = minX; h < maxX; h++) {
      i = h + width*v;
      if (color[i*3] > 0 || color[i*3+1] > 0 || color[i*3+2] > 0) {
	if (nearestz > depth[i]) {
	  nearestz = depth[i];
	  nearestx = i%width; nearesty = i/width;
	}
      }
    }
  }

  GLdouble x, y, z;
  bool ret;
  // find world space coordinates of nearest collision
  if (nearestz < 1000) {
    if (gluUnProject((GLdouble) nearestx, (GLdouble) nearesty, nearestz,
		     modelview, projection, vp, &x, &y, &z)) {
      collisionPt = Vec3f(x, y, z);
      ret = true;
    }
    else {
      fprintf(stderr,
	      "ObjectManager::CollisionTest: Error: unable to unproject\n");
      ret = false;
    }
  }
  else
    ret = false;

  // reset GL state
  glPopMatrix(); // modelview
  glMatrixMode(GL_PROJECTION);
  glPopMatrix(); // projection
  
  glCullFace(GL_BACK);
  glPopAttrib(); // pop depth attribs
  glDrawBuffer((GLenum)OldDrawBuffer);

  // reset viewport
  glViewport(pvp[0], pvp[1], pvp[2], pvp[3]);
  
  delete [] color;
  return ret;
}




