//------------------------------------------------------------------------------
// File : lightcam.cpp
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
// lightcam.cpp
//==========================================================================

#include "lightcam.hpp"

//----------------------------------------------------------------------------
// CONSTRUCTORS (initialization): Contructor 1: merely sets the light colors;
// the lighting geometry must be handled through the normal camera functions.
// Constructor 2: takes in a camera as well and copies it to define geometry.
// Constructor 3: is the completely "manual" version.
//----------------------------------------------------------------------------
LightCam::LightCam(float ambient[4], float diffuse[4], float specular[4])
{
  SetLightColors(ambient,diffuse,specular);
  IsActive=1;
  IsSpotLight=0;
  SpotExponent=0;
  // default attenuation
  ConstantAttenuation = 1.0;
  LinearAttenuation = 0.0;
  QuadraticAttenuation = 0.0;
}

LightCam::LightCam(Camera *cam, float ambient[4], float diffuse[4], float specular[4])
{
  Copy(*cam);
  SetLightColors(ambient,diffuse,specular);
  IsActive=1;
  IsSpotLight=0;
  SpotExponent=0;
  // default attenuation
  ConstantAttenuation = 1.0;
  LinearAttenuation = 0.0;
  QuadraticAttenuation = 0.0;
}

LightCam::LightCam()
{
  SetLightColors(0,0,0,1,  // DEFAULT AMBIENT
                 1,1,1,1,  // DEFAULT DIFFUSE
                 1,1,1,1); // DEFAULT SPECULAR
  IsActive=1;
  IsSpotLight=0;
  SpotExponent=0;
  // default attenuation
  ConstantAttenuation = 1.0;
  LinearAttenuation = 0.0;
  QuadraticAttenuation = 0.0;
}

void LightCam::SetLightColors(float ambient[4], float diffuse[4],
			      float specular[4])
{
  Ambient[0]=ambient[0]; Ambient[1]=ambient[1];
  Ambient[2]=ambient[2]; Ambient[3]=ambient[3];
  Diffuse[0]=diffuse[0]; Diffuse[1]=diffuse[1];
  Diffuse[2]=diffuse[2]; Diffuse[3]=diffuse[3];
  Specular[0]=specular[0]; Specular[1]=specular[1];
  Specular[2]=specular[2]; Specular[3]=specular[3];
}

void LightCam::SetLightColors(float ar, float ag, float ab, float aa,     
                              float dr, float dg, float db, float da,   
                              float sr, float sg, float sb, float sa)
{
  Ambient[0]=ar;  Ambient[1]=ag;  Ambient[2]=ab;  Ambient[3]=aa;
  Diffuse[0]=dr;  Diffuse[1]=dg;  Diffuse[2]=db;  Diffuse[3]=da;
  Specular[0]=sr; Specular[1]=sg; Specular[2]=sb; Specular[3]=sa;
}

//----------------------------------------------------------------------------
// Sets the specified attenuation type to the float value specified.
// Stores the values until the light is activated with SetOpenglLight();
// type is one of: GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, and
// GL_QUADRATIC_ATTENUATION, falloff is the constant of attenuation
//----------------------------------------------------------------------------
void LightCam::SetLightAttenuation(GLenum type, float falloff)
{
  switch (type) {
    case GL_CONSTANT_ATTENUATION:
      ConstantAttenuation = falloff;
      break;

    case GL_LINEAR_ATTENUATION:
      LinearAttenuation = falloff;
      break;

    case GL_QUADRATIC_ATTENUATION:
      QuadraticAttenuation = falloff;
      break;

    default:
      printf("ERROR (SetLightAttenuation): Invalid GL Attenuation type!\n");
      break;
  }
}

//----------------------------------------------------------------------------
// Set up a spotlight by explicitly providing all parameters including
// cutoff angle -- for spots wider or smaller than camera.  Useful for Env
// mapped hemicubes with spotlights
//----------------------------------------------------------------------------
void LightCam::SetSpotParams(float dir[3], float cutoffangle, float exponent)
{
  SpotDir[0] = dir[0]; SpotDir[1] = dir[1]; SpotDir[2] = dir[2];
  SpotCutoff = cutoffangle;
  SpotExp = exponent;
}

//----------------------------------------------------------------------------
// Uses the camera to calc the corresponding OpenGL lighting geometry params.
// Uses cam to calc the light.
//----------------------------------------------------------------------------
void LightCam::CalcLightGeom(float Pos[4], float SpotDirection[3],
			     float &SpotCutoffAngle)
{
  // PUT LIGHT POSITION AT ORIGIN OF CAMERA (Eye)
  Pos[0]= Orig.x;  Pos[1]= Orig.y;  Pos[2]= Orig.z; Pos[3]= 1;

  if (IsSpotLight)
  {
   // LIGHT "LOOKS" ALONG THE CAMERA'S NEGATIVE Z-AXIS
   SpotDirection[0]=-Z.x;  SpotDirection[1]=-Z.y;  SpotDirection[2]=-Z.z;

   // PICK A SPOTLIGHT ANGLE THAT FITS INSIDE THE CAMERA'S FRUSTUM
   // (HALF OF FOV IN DEGREES)
   SpotCutoffAngle = atan(wT/Near) * 57.29578;  // CONVERT TO DEGREES
  }

  // JUST A POINT LIGHT (ONLY CHANGE CutoffAngle to 180 - DISABLES SPOTLIGHT)
  else  
  {
   SpotCutoffAngle=180;
  }
}

//----------------------------------------------------------------------------
// Uses the camera to calc the corresponding OpenGL lighting geometry params.
// Uses cam to calc the light.  This version uses data members
//----------------------------------------------------------------------------


void LightCam::CalcLightGeom()
{
  if (IsSpotLight)
  {
    // LIGHT "LOOKS" ALONG THE CAMERA'S NEGATIVE Z-AXIS
    SpotDir[0]=-Z.x;  SpotDir[1]=-Z.y;  SpotDir[2]=-Z.z;
    
    // PICK A SPOTLIGHT ANGLE THAT FITS INSIDE THE CAMERA'S FRUSTUM
    // (HALF OF FOV IN DEGREES)
    SpotCutoff = atan(wT/Near) * 57.29578;  // CONVERT TO DEGREES
  }
  // JUST A POINT LIGHT (ONLY CHANGE CutoffAngle to 180 - DISABLES SPOTLIGHT)
  else 
  {
    SpotCutoff=180;
  }
}

//----------------------------------------------------------------------------
// Uses an OpenGL light to calculate the corresponding camera. The light must
// be a spotlight and the user must supply an UP vector to create a frame
// in EYE-SPACE (VERY IMPORTANT - the resulting camera will be in eye-space).
// near and far plane distances from the light can be specified; values that
// are <=0 will not overwrite the original camera values. The light must also
// be a point light, NOT directional, with a SPOT_CUTTOFF angles < 90 degrees.
//----------------------------------------------------------------------------
void LightCam::CalcCamGeom(GLenum LightID, float viewup[3], float Ndist, float Fdist)
{
  // GET LIGHT PARAMETERS (IN EYE SPACE)
  float Pos[4], Dir[3], HalfAng;
  glGetLightfv(LightID,GL_POSITION,Pos);           // RETURNED IN EYE-SPACE
  glGetLightfv(LightID,GL_SPOT_DIRECTION,Dir);     // RETURNED IN EYE-SPACE
  glGetLightfv(LightID,GL_SPOT_CUTOFF,&HalfAng);

  // MAKE SURE IT IS A POINT SPOTLIGHT, WITH APPROPRIATE FOV
  if (Pos[3]==0) {
    printf("Cannot create LightCam from directional light (w=0)!\n");
    return;
  }
  if (HalfAng>=90) {
    printf("Can only create LightCam from a spotlight!\n");
    return;
  }

  // CONVERT SPOTLIGHT SUBTENDED HALF-ANGLE TO RADIANS
  HalfAng *= (float)0.0174532;

  // SET CAMERA COORDINATE FRAME IN EYE-SPACE
  Vec3f viewrefpt( Pos[0]+Dir[0], Pos[1]+Dir[1], Pos[2]+Dir[2] );
  LookAt( Vec3f(Pos[0],Pos[1],Pos[2]), 
          viewrefpt,
          Vec3f(viewup[0],viewup[1],viewup[2]) );

  // RESET NEAR AND FAR PLANES, IF REQUESTED
  if (Ndist>0) Near=Ndist;
  if (Fdist>0) Far=Fdist;

  // RECALC WINDOW COORDINATES - LIGHTCAM IS SYMMETRICAL
  wT=tan(HalfAng)*Near;  
  wB=-wT;
  wR=wT; 
  wL=-wR;
}

//----------------------------------------------------------------------------
// Sets the OpenGL light parameters for the given LightCamID (i.e. GL_LIGHT0,
// GL_LIGHT1, ...). The light must already be enabled or an error will occur.
// In most cases, only one light will be needed (even for multi-light shadowed
// scenes - they are composited from single light scenes).
//----------------------------------------------------------------------------
void LightCam::SetOpenglLight(GLenum LightID)
{
  if ( !glIsEnabled(GL_LIGHTING) )
    printf("ERROR (SetOpenglLight): GL_LIGHTING is not enabled!\n");
  if ( !glIsEnabled(LightID) )
    printf("ERROR (SetOpenglLight): GL_LIGHT* is not enabled!\n");

  // CALC LIGHTING GEOMETRY PARAMS: POSITION, SPOTLIGHT DIRECTION, CUTOFF ANGLE
  // AND EXPONENT FOR SPOTLIGHT DROPOFF
  float Position[4], SpotDirection[3], SpotCutoffAngle;
  CalcLightGeom(Position,SpotDirection,SpotCutoffAngle);

  glLightfv(LightID, GL_AMBIENT, Ambient);
  glLightfv(LightID, GL_DIFFUSE, Diffuse);
  glLightfv(LightID, GL_SPECULAR, Specular);
  glLightfv(LightID, GL_POSITION, Position);
  glLightf(LightID, GL_CONSTANT_ATTENUATION, ConstantAttenuation);
  glLightf(LightID, GL_LINEAR_ATTENUATION, LinearAttenuation);
  glLightf(LightID, GL_QUADRATIC_ATTENUATION, QuadraticAttenuation);
  
  glLightfv(LightID, GL_SPOT_DIRECTION, SpotDirection);
  glLightf(LightID, GL_SPOT_CUTOFF, SpotCutoffAngle);
  glLightf(LightID, GL_SPOT_EXPONENT, SpotExponent);
}


//----------------------------------------------------------------------------
// DISABLES THE OPENGL SPOTLIGHT TEMPORARILY WITHOUT CHANGING THE LIGHT CAM
// PARAMS (INCLUDING THE IsSpotLight FLAG). Calling SetOpenglLight again
// will restore original spotlight params. This is useful when the spotlight
// geometry is desired, but we do not wish to render the scene with actual
// spotlighting (perhaps a projective texture spotlight is going to be used).
// Projective texture spotlight and spotlight cone drawing routines could
// use the OpenGL spotlight geometry without actually rendering using a HW spotlight.
//----------------------------------------------------------------------------
void LightCam::DisableOpenglSpotLight(GLenum LightID)
{
  glLightf(LightID, GL_SPOT_CUTOFF, 180);
}
