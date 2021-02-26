//------------------------------------------------------------------------------
// File : lightcamprojector.cpp
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
// lightcamprojector.cpp : projective texture in a lightcam
//  We use this module to support projective texturing with a variety
//  of application modes. By specifying a camera, a texture, and a set of
//  of objects upon which to apply the projective texture, we can handle a
//  wide variety of effects including: shadow-casting, "accurate" spotlight
//  rendering, caustics and transparency effects, slideshows, etc.
//  Since objects may already be textured, we have to provide 2nd-pass routines
//  as well.
//============================================================================

#include "lightcamprojector.hpp"

//----------------------------------------------------------------------------
// CONSTRUCTORS (initialization): Contructor 1: merely sets the light colors;
// the lighting geometry must be handled through the normal camera functions.
// Constructor 2: takes in a camera as well and copies it to define geometry.
// Constructor 3: is the completely "manual" version.
//----------------------------------------------------------------------------
LightCamProjector::LightCamProjector(float ambient[4], float diffuse[4],
				     float specular[4])
{
  SetLightColors(ambient,diffuse,specular);
  IsActive=1;
  IsSpotLight=0;
  SpotExponent=0;
  // default attenuation
  ConstantAttenuation = 1.0;
  LinearAttenuation = 0.0;
  QuadraticAttenuation = 0.0;
  TexID = -1;
}

LightCamProjector::LightCamProjector(Camera *cam, float ambient[4],
				     float diffuse[4], float specular[4])
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
  TexID = -1;
}

LightCamProjector::LightCamProjector()
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
  TexID = -1;
}


//----------------------------------------------------------------------------
// Enable projective textures.
// All object drawn hereafter will have a projected texture applied. This
// means that textures objects must have there textures disabled or we must
// use two-passes to properly apply the object texture and the modulation
// projected texture. The texture is bound and the appropriate modes are set.
// WARNING: THIS ROUTINE PUSHES THE TEXTURE ATTRIBUTE BIT, POP IS IN
// DisableProjTex.
//----------------------------------------------------------------------------
void LightCamProjector::EnableProjTex()
{
  if (!glIsTexture(TexID)) 
    { printf("ERROR (EnableProjTex): TexID does not exist!\n"); return; }

  glPushAttrib(GL_TEXTURE_BIT);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,TexID);  

  // INIT AUTOMATIC TEXTURE COORDINATE GENERATION, SUCH THAT COORDS ARE DEFINED
  // DIRECTLY FROM VERTICES IN OBJECT SPACE (NO glTexCoords calls needed).
  GLfloat objPlaneS[] = {1.0, 0.0, 0.0, 0.0}; 
  GLfloat objPlaneT[] = {0.0, 1.0, 0.0, 0.0}; 
  GLfloat objPlaneR[] = {0.0, 0.0, 1.0, 0.0}; 
  GLfloat objPlaneQ[] = {0.0, 0.0, 0.0, 1.0}; 
  glTexGenfv(GL_S, GL_OBJECT_PLANE, objPlaneS); 
  glTexGenfv(GL_T, GL_OBJECT_PLANE, objPlaneT); 
  glTexGenfv(GL_R, GL_OBJECT_PLANE, objPlaneR); 
  glTexGenfv(GL_Q, GL_OBJECT_PLANE, objPlaneQ); 
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR); 
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR); 
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR); 
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR); 
  glEnable(GL_TEXTURE_GEN_S); 
  glEnable(GL_TEXTURE_GEN_T); 
  glEnable(GL_TEXTURE_GEN_R); 
  glEnable(GL_TEXTURE_GEN_Q); 

  // DEFINE TEXTURE MATRIX STACK SO THAT OBJECT SPACE COORDS USED AS TEX COORDS
  // WILL BE PROJECTED ONTO THE PROJECTED TEXTURE CAMERA VIEWING PLANE. OBJECT
  // SPACE VERTICES ARE PROJECTED INTO PROJECTIVE-TEXTURE CAMERA "SCREEN" SPACE
  // TO DEFINE A PROJECTED TEXTURE COORD VALUE. THE ADDITIONAL SCALE AND
  // TRANSLATE IS REQUIRED TO GO FROM X,Y NDC COORDS IN [-1,1] TO TEX COORDS
  // [0,1]
  // WARNING: SINCE THE TEXTURE MATRIX STACK IS ONLY 2 DEEP, WE HAVE TO TREAT
  // THE CURRENT MATRIX AS THE TOP (THIS COULD BE FOILED IF A MATRIX HAS
  // ALREADY BEEN PUSHED - BE WARNED!
  GLfloat M[16];
  glMatrixMode(GL_TEXTURE); 
  //glPushMatrix();
  glLoadIdentity(); 
  glTranslatef(0.5,0.5,0); 
  glScalef(0.5,0.5,1); 
  glMultMatrixf( GetProjectionMatrix(M) ); 
  glMultMatrixf( GetModelviewMatrix(M) );
  //glPopMatrix();
}

//----------------------------------------------------------------------------
// Disable the projective texture by popping the texture matrix stack and
// by restoring texture properties. MUST BE CALLED AFTER USING PROJECTIVE
// TEXTURE! Must be paired with EnableProjTex.
//----------------------------------------------------------------------------
void LightCamProjector::DisableProjTex()
{
  glPopAttrib();  // GL_TEXTURE_BIT
}

//----------------------------------------------------------------------------
// In order to properly display projective textures on an object, object-space
// must be equal to world-space OR the object must be xformed into world-space.
// Each object is contained in a coordinate frame, we will simply use this
// to xform from obj 2 world. Remember, the vertices themselves are being used
// as the texture coords after being projected into a world-space camera. These
// vertices must first be xformed into the same space as the camera; an
// object's vertices are normally specified in object-space coords.
//----------------------------------------------------------------------------
void LightCamProjector::DisplayObjs(ObjectFrame* *Objs,
				    int NumObjs,
				    unsigned int Attribs)
{
  float M[16];
  for (int i=0; i<NumObjs; i++)
  {
    // MUST PUSH OBJECT-TO-WORLD XFORM ON THE TEXTURE MATRIX STACK 
    // (IF IDENTITY THEN OBJ=WORLD). SEE HOFF TECH REPORT FOR MORE DETAILS
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glMultMatrixf(Objs[i]->GetObj2WorldXform(M));
      Objs[i]->Display(Attribs);
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
  }
}

//--------------------------------------------------------------------------
// SINGLE-PASS APPLICATION : Given a texture-object ID and an array of ptrs
// to object frames, apply the texture-map as a projective-texture to the
// object using either a GL_DECAL or GL_MODULATE mode (apply direct or multiply
// colors respectively). The objects must not already be textured. MapMode =
// GL_DECAL, GL_MODULATE, etc.
//--------------------------------------------------------------------------
void LightCamProjector::DrawObjsWithProjTex(ObjectFrame* *Objs,
					    int NumObjs,
					    float MapMode)
{
  EnableProjTex();
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, MapMode);

  if (MapMode == GL_MODULATE)
  {
    // JUST DISABLE TEXTURING
    DisplayObjs(Objs,NumObjs,OBJ_COLORS | OBJ_NORMALS); 
  }
  else
  {
    glPushAttrib(GL_LIGHTING_BIT);
      glDisable(GL_LIGHTING);
      glDisable(GL_COLOR_MATERIAL);
      // FASTEST DISPLAY MODE - GEOMETRY ONLY
      DisplayObjs(Objs,NumObjs,OBJ_NONE);  
    glPopAttrib();  // GL_ENABLE_BIT
  }

  DisableProjTex();
}

//--------------------------------------------------------------------------
// Same as DrawObjsWithProjTex except that the application mode is always
// GL_MODULATE and the projective texture-map is modulated by a specific 
// reflective color.
//--------------------------------------------------------------------------
void LightCamProjector::DrawObjsWithModulatedProjTex(ObjectFrame* *Objs,
						     int NumObjs,
						     float Refl[3])
{
  EnableProjTex();
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glPushAttrib(GL_LIGHTING_BIT);
  glDisable(GL_LIGHTING);
  glDisable(GL_COLOR_MATERIAL);

    glColor3fv(Refl);
    // FASTEST DISPLAY MODE - GEOMETRY ONLY
    DisplayObjs(Objs,NumObjs,OBJ_NONE);  

  glPopAttrib();  // GL_LIGHTING_BIT

  DisableProjTex();
}

//--------------------------------------------------------------------------
// 2-PASS APPLICATION : Textured objects require a second pass texture-mapping
// step.  This routine draws over the already rendered textured objects with
// the specified sphere-map. Several application modes are possible:
// modulation, add, modulate sphere-map by refl and add.
//--------------------------------------------------------------------------
void LightCamProjector::DrawObjsWith2ndPassProjTex_MODULATE(ObjectFrame* *Objs,
							    int NumObjs)
{
   glPushAttrib(GL_COLOR_BUFFER_BIT);
     glEnable(GL_BLEND);
     // ==> DstColor*SrcColor + ZERO*DstColor
     glBlendFunc(GL_DST_COLOR, GL_ZERO);  
   glPushAttrib(GL_DEPTH_BUFFER_BIT);
     glDepthMask(0);
     glDepthFunc(GL_EQUAL);
   DrawObjsWithProjTex(Objs,NumObjs,GL_DECAL);
   glPopAttrib();  // GL_DEPTH_BUFFER_BIT
   glPopAttrib();  // GL_COLOR_BUFFER_BIT
}

void LightCamProjector::DrawObjsWith2ndPassProjTex_ADD(ObjectFrame* *Objs,
						       int NumObjs)
{
   glPushAttrib(GL_COLOR_BUFFER_BIT);
     glEnable(GL_BLEND); 
     glBlendFunc(GL_ONE, GL_ONE);        // ==> SrcColor + DstColor
   glPushAttrib(GL_DEPTH_BUFFER_BIT);
     glDepthMask(0);
     glDepthFunc(GL_EQUAL);
   DrawObjsWithProjTex(Objs,NumObjs,GL_DECAL);
   glPopAttrib();  // GL_DEPTH_BUFFER_BIT
   glPopAttrib();  // GL_COLOR_BUFFER_BIT
}

void LightCamProjector::DrawObjsWith2ndPassProjTex_MODULATE_ADD(
							ObjectFrame* *Objs,
							int NumObjs,
							float Refl[3])
{
   glPushAttrib(GL_COLOR_BUFFER_BIT);
     glEnable(GL_BLEND); 
     glBlendFunc(GL_ONE, GL_ONE);        // ==> SrcColor + DstColor
   glPushAttrib(GL_DEPTH_BUFFER_BIT);
     glDepthMask(0);
     glDepthFunc(GL_EQUAL);
   DrawObjsWithModulatedProjTex(Objs,NumObjs,Refl);
   glPopAttrib();  // GL_DEPTH_BUFFER_BIT
   glPopAttrib();  // GL_COLOR_BUFFER_BIT
}
