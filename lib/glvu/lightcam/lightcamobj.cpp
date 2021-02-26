//------------------------------------------------------------------------------
// File : lightcamobj.cpp
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
// lightcamobj.cpp
//==========================================================================

#include "lightcamobj.hpp"

//----------------------------------------------------------------------------
// Light is drawn as a simple sphere DIFFUSEly colored sphere with a radius
// equal to some fraction of the camera near dist. The camera can also
// be displayed.
//----------------------------------------------------------------------------
void LightCamObj::Display(unsigned int Attribs)
{
/*
  if (lCam==NULL) { printf("ERROR (LightCamObj) lCam==NULL!\n"); return; }

  float Radius = lCam->Near * 0.9;
  glColor3fv(lCam->Diffuse);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glTranslatef(lCam->Orig.x,lCam->Orig.y,lCam->Orig.z);
    glutSolidSphere(Radius,12,12); 
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
*/
}

//----------------------------------------------------------------------------
// Update the min/max extents of the light-camera object by having a cube around
// the camera sphere object. The MIN/MAX extents of the LightCamObj can only
// be known when the light-camera is updated (which may be every frame). This
// object is not static if it corresponds to a moving light.
// Be sure to use AFTER the light-camera has been updated.
//----------------------------------------------------------------------------
void LightCamObj::UpdateMinMax()
{
  if (lCam==NULL) { printf("ERROR (LightCamObj) lCam==NULL!\n"); return; }

  float d = lCam->Near * 0.9;
  Vec3f O = lCam->Orig;
  Min.Set(O.x-d, O.y-d, O.z-d);
  Max.Set(O.x+d, O.y+d, O.z+d);
}

//----------------------------------------------------------------------------
// The light-camera object is unusual when it comes to update and display.
// when the LightCamObj is used in the ObjMngr, it will be wrapped with an
// ObjFrame which has it's own coordinate system. Normally objects exist in
// object-space and are only transformed to world-space at run-time. A Light-
// camera must be in world-space ALWAYS if we want to ensure proper viewing
// from the world-space lights. We could simply push the ObjFrame Obj2World
// matrix on the stack in advance, but then the camera user routines would
// have to be aware. Instead, we will use an update routine that will allow
// users to update the light camera based on a changing object frame
// (perform the obj2world space xform on the camera itself). Given the object
// coordinate frame, we transform the camera to world space.
//----------------------------------------------------------------------------
void LightCamObj::XformLightCam(Vec3f &X, Vec3f &Y, Vec3f &Z, Vec3f &O, float Scale)
{
  if (lCam==NULL) { printf("ERROR (LightCamObj) lCam==NULL!\n"); return; }

  Vec3f T;

  // ROTATE THE CAMERA AXES (Obj2World)
  Vec3f M33row1(X.x,Y.x,Z.x);
  Vec3f M33row2(X.y,Y.y,Z.y);
  Vec3f M33row3(X.z,Y.z,Z.z);
  T=lCam->X;
  lCam->X.Set( M33row1*T, M33row2*T, M33row3*T );
  T=lCam->Y;
  lCam->Y.Set( M33row1*T, M33row2*T, M33row3*T );
  T=lCam->Z;
  lCam->Z.Set( M33row1*T, M33row2*T, M33row3*T );

  // ROTATE AND TRANSLATE THE ORIGIN ABOUT THE FRAME ORIGIN (O)
  T = lCam->Orig;
  lCam->Orig.Set( M33row1*T + O.x, M33row2*T + O.y, M33row3*T + O.z );

  // SCALE THE NEAR AND FAR PLANE DISTANCES AND THE WINDOW EXTENTS
  lCam->Near *= Scale;
  lCam->Far *= Scale;
  lCam->wL *= Scale;
  lCam->wR *= Scale;
  lCam->wT *= Scale;
  lCam->wB *= Scale;
}
