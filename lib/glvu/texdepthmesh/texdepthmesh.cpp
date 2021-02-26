//------------------------------------------------------------------------------
// File : texdepthmesh.cpp
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
// texdepthmesh.cpp
//============================================================================

#include <stdio.h>
#include <iostream.h>
#include <glbuffers.hpp>
#include <textureobj.hpp>
#include "texdepthmesh.hpp"

//-----------------------------------------------------------------------------
// DEFAULT CONSTRUCTOR
//-----------------------------------------------------------------------------
TexturedDepthMesh::TexturedDepthMesh()
{
  Init();
}

//-----------------------------------------------------------------------------
// CONSTRUCTOR for creation of a new depth-map from the current existing
// depth-buffer (in the back buffer) given the camera from which the
// depth-buffer was created. NOTE: USES THE DIMENSIONS OF THE CURRENT VIEWPORT.
//-----------------------------------------------------------------------------
TexturedDepthMesh::TexturedDepthMesh(Camera *cam)
{
  Init();
  Copy(*cam);
  SaveColorAndDepth(1);
}

//-----------------------------------------------------------------------------
// Given an object and a camera, display and create a TexDepthMesh
//-----------------------------------------------------------------------------
TexturedDepthMesh::TexturedDepthMesh(Object *Obj, Camera *Cam, int NumCols,
				     int NumRows)
{
  Init();
  Snapshot(Obj, Cam, NumCols, NumRows, 1);
}

//-----------------------------------------------------------------------------
// TextureDepthMesh INIT ROUTINE
//-----------------------------------------------------------------------------
void TexturedDepthMesh::Init()
{
  Color=NULL;
  Depth=NULL;
  nCols=nRows=allocPixels=0;
  UseDisplayLists=UseProjTex=0;
  DisplayListID=TexObjID=0;
  MinDepth=MaxDepth=AvDepth=0;
  deferDeleteTex=0;
  deleteTexFlag=0;
}

//-----------------------------------------------------------------------------
// TextureDepthMesh SNAPSHOT ROUTINES
//-----------------------------------------------------------------------------
void TexturedDepthMesh::Snapshot(Camera *cam, int realloc)
{
  if (cam != NULL) Copy(*cam); // else reuse current cam
  SaveColorAndDepth(realloc);
}

void TexturedDepthMesh::Snapshot(Object *Obj, Camera *Cam, int NumCols,
				 int NumRows, int realloc)
{
  // SET TDM CAMERA IF NEW ONE SPECIFIED
  if (Cam!=NULL) Copy(*Cam); // else reuse current cam

  // BE SURE REQUESTED RESOLUTION FITS IN CURRENT WINDOW
  int WW = glutGet((GLenum)GLUT_WINDOW_WIDTH);
  int WH = glutGet((GLenum)GLUT_WINDOW_HEIGHT);
  if (NumCols>WW) return;
  if (NumRows>WH) return;

  // SET UP VIEWING FROM TDM CAMERA (ALSO SET VIEWPORT)
  GLfloat M[16];
  glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0,0,NumCols,NumRows);
  glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(GetProjectionMatrix(M));
  glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(GetModelviewMatrix(M));

  // DRAW TO THE BACK BUFFER
  GLint DrawBuffer;
  glGetIntegerv(GL_DRAW_BUFFER,&DrawBuffer);
  glDrawBuffer(GL_BACK);

  // RENDER THE SCENE
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Obj->Display();
  glFinish();
  
  // SAVE THE COLOR AND DEPTH BUFFERS INTO THE TDM
  SaveColorAndDepth(realloc);

  // RESTORE DRAW BUFFER
  glDrawBuffer((GLenum)DrawBuffer);

  // RESTORE VIEWING PARAMS
  glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  glPopAttrib();  // VIEWPORT
}


//-----------------------------------------------------------------------------
// CONSTRUCTOR THAT LOADS A DEPTH TEXTURE FROM A FILE: The file is in a packed
// RGBD format (ASCII unsigned char for R,G,B, and Depth). This reads the file
// including the (viewing params - in same file) and converts it to the 
// TexturedDepthMesh format.
//-----------------------------------------------------------------------------
TexturedDepthMesh::TexturedDepthMesh(char *FileName)
{
  UseDisplayLists=UseProjTex=0;
  DisplayListID=TexObjID=0;
  MinDepth=MaxDepth=AvDepth=0;

  // WRITE TO AN ASCII FILE
  FILE* fp = fopen(FileName, "r");
  if (fp==NULL) 
  {
    printf("ERROR (TexturedDepthMesh): unable to open %s for reading!\n",
	   FileName);
    return;
  }

  // gluPerspective PARAMS
  float FovY, Aspect, Near, Far;
  fscanf(fp, "%f %f %f %f", &FovY,&Aspect,&Near,&Far);
  Perspective(FovY,Aspect,Near,Far);

  // gluLookAt PARAMS
  Vec3f Eye, Cntr, Up;
  fscanf(fp, "%f %f %f %f %f %f %f %f %f", 
    &Eye.x,&Eye.y,&Eye.z,&Cntr.x,&Cntr.y,&Cntr.z,&Up.x,&Up.y,&Up.z);
  LookAt(Eye,Cntr,Up);

  // TEXTURE DIMENSION (N: VIEWPORT SIZE)
  fscanf(fp, "%d %d%*c", &nCols, &nRows);

  // ALLOCATE COLOR AND DEPTH BUFFERS
  allocPixels=nCols*nRows;
  Depth = new GLfloat[allocPixels];
  Color = new unsigned char[allocPixels*3];

  // READ IN THE DEPTH AND COLOR VALUES
  int i;    
  for (i=0; i<allocPixels; i++)
    fscanf(fp,"%f",&Depth[i]);
  fscanf(fp,"\n");
  for (i=0; i<allocPixels*3; i++)
    fscanf(fp,"%c",&Color[i]);

  fclose(fp);
}

//-----------------------------------------------------------------------------
// DESTRUCTOR: just frees the arrays.
//-----------------------------------------------------------------------------
TexturedDepthMesh::~TexturedDepthMesh()
{
  delete[] Color;
  delete[] Depth;
  if (TexObjID!=0) { DeleteTextureObj(TexObjID); TexObjID=0; }
}

//-----------------------------------------------------------------------------
// Write the TexturedDepthMesh to a file
//-----------------------------------------------------------------------------
void TexturedDepthMesh::Write(char *FileName)
{
  // WRITE TO AN ASCII FILE
  FILE* fp = fopen(FileName, "w");
  if (fp==NULL) 
  {
    printf("ERROR (TexturedDepthMesh): unable to open %s for writing!\n",
	   FileName);
    return;
  }

  // gluPerspective PARAMS
  float FovY, Aspect, Near, Far;
  GetPerspectiveParams(&FovY,&Aspect,&Near,&Far);
  fprintf(fp, "%f %f %f %f\n", FovY,Aspect,Near,Far);

  // gluLookAt PARAMS
  Vec3f Eye, Cntr, Up;
  GetLookAtParams(&Eye,&Cntr,&Up);
  fprintf(fp, "%f %f %f\n%f %f %f\n%f %f %f\n", 
    Eye.x,Eye.y,Eye.z,Cntr.x,Cntr.y,Cntr.z,Up.x,Up.y,Up.z);

  // TEXTURE DIMENSION (N: VIEWPORT SIZE)
  fprintf(fp, "%d %d\n", nCols, nRows);

  int NumPixels=nCols*nRows;

  // WRITE OUT THE DEPTH AND COLOR VALUES
  int i;
  for (i=0; i<NumPixels; i++)
    fprintf(fp,"%f ",Depth[i]);
  fprintf(fp,"\n");
  for (i=0; i<NumPixels*3; i++)
    fprintf(fp,"%c",Color[i]);

  fclose(fp);
}

//-----------------------------------------------------------------------------
// Find min, max, and average depth values in depth buffer array, converts
// to linear z (dist from eye plane - like near/far planes dists), and stores.
//-----------------------------------------------------------------------------
inline float TexturedDepthMesh::LinearizeZ(float DepthZ, float NearDist,
					   float FarDist)
{
  return (NearDist*FarDist) / ( FarDist - DepthZ*(FarDist-NearDist) );
}

void TexturedDepthMesh::CalcMinMaxAvDepths()
{
  if (Depth==NULL) return;
  MinDepth=MaxDepth=AvDepth=Depth[0];
  for (int i=1; i<(nCols*nRows); i++)
  {
    if (Depth[i]<MinDepth) MinDepth=Depth[i];
    else if (Depth[i]>MaxDepth) MaxDepth=Depth[i];
    AvDepth+=Depth[i];
  }
  AvDepth/=(float)(nCols*nRows);
  MinDepth = LinearizeZ(MinDepth,Near,Far);
  MaxDepth = LinearizeZ(MaxDepth,Near,Far);
  AvDepth = LinearizeZ(AvDepth,Near,Far);
  //printf("Near=%f, Far=%f, Min=%f, Max=%f, Av=%f\n",Near,Far,MinDepth,MaxDepth,AvDepth);
}

//-----------------------------------------------------------------------------
// After the scene has been fully rendered from the TDM's camera view, save
// the color and depth buffers (within the defined viewport area).
//-----------------------------------------------------------------------------
void TexturedDepthMesh::SaveColorAndDepth(int realloc)
{
  if (!realloc) // make sure we have enough space alloced in the buffers
  {
    int VP[4];  // START (X,Y) AND WIDTH,HEIGHT
    glGetIntegerv(GL_VIEWPORT,VP);
    if (Color == NULL || Depth == NULL
	|| VP[2] * VP[3] > nCols * nRows)
      // doh! not enough space -- realloc after all
      realloc = 1;
  }
  if (realloc && Color!=NULL) { delete[] Color; Color=NULL; allocPixels=0;}
  SaveColorBuffer(Color,nCols,nRows,GL_BACK);
  if (realloc && Depth!=NULL) { delete[] Depth; Depth=NULL; allocPixels=0;}
  SaveDepthBuffer(Depth,nCols,nRows,GL_BACK);
  
  if (nCols * nRows > allocPixels) allocPixels = nCols*nRows;

  // mark any texture for proj texturing as needing re-generation
  // mark any texture for proj texturing as needing re-generation
  if (TexObjID)
  {
    if (deferDeleteTex) deleteTexFlag = 1;
    else 
    { DeleteTextureObj(TexObjID); TexObjID=0; }
  }
}

//-----------------------------------------------------------------------------
// Call this to specify your own externally allocated color and depth buffers 
// (handy if you want to allocate the memory in a funky way, say, in shared memory)
//-----------------------------------------------------------------------------
void TexturedDepthMesh::SetColorAndDepth(unsigned char *ColorBuf, float *DepthBuf,
					 int cols, int rows)
{
  if (Color) delete[] Color;
  Color = ColorBuf;
  if (Depth) delete[] Depth;
  Depth = DepthBuf;
  nCols = cols; nRows = rows;

  // mark any texture for proj texturing as needing re-generation
  if (TexObjID)
  {
    if (deferDeleteTex) deleteTexFlag = 1;
    else 
    { DeleteTextureObj(TexObjID); TexObjID=0; }
  }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
long TexturedDepthMesh::GetByteConsumption()
{
  long bytes = sizeof( *this );
  int npixels = nRows * nCols;
  bytes += 3 * npixels * sizeof( unsigned char );
  bytes += npixels * sizeof( GLfloat );
  return(bytes);
}

//-----------------------------------------------------------------------------
// DISPLAY LIST GENERATION ROUTINES : 
//  Uses globals UseDisplayLists, DisplayListID, and tID.
//-----------------------------------------------------------------------------
void TexturedDepthMesh::Begin_DisplayList()
{
  if (UseDisplayLists)   
    if (DisplayListID==0)   
      { tID=glGenLists(1); glNewList(tID,GL_COMPILE_AND_EXECUTE); }
    else
      { glCallList(DisplayListID); return; }
  else
    if (DisplayListID!=0)
      { glDeleteLists(DisplayListID,1); DisplayListID=0; }
}

void TexturedDepthMesh::End_DisplayList()
{
  if (UseDisplayLists && DisplayListID==0)
    { DisplayListID=tID; glEndList(); }
}

//-----------------------------------------------------------------------------
// PROJECTIVE TEXTURE OBJECT GENERATION (enables use of texture instead of calling
//  glColor for each point). The texture obj is created and enabled from the
//  TexturedDepthMesh color array. Uses globals 
//-----------------------------------------------------------------------------
void TexturedDepthMesh::Begin_ProjTexObj()
{
  // GENERATE TEXTURE OBJECT (IF NOT ALREADY), ENABLE PROJECTIVE TEXTURE
  if (UseProjTex)  
  {
    if (deferDeleteTex && deleteTexFlag) {
      DeleteTextureObj(TexObjID); TexObjID=0; deleteTexFlag=0;
    }
    if (TexObjID==0) 
      TexObjID = CreateTextureObjFromColorBuffer(Color,nCols,nRows,
						 GL_CLAMP,GL_LINEAR);
    EnableProjTex();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  } 
  else             // DELETE TEXTURE OBJECT IF ALREADY THERE
  {
    if (TexObjID!=0) {
      if (deferDeleteTex) deleteTexFlag = 1;
      else { DeleteTextureObj(TexObjID); TexObjID=0; }
    }
  }
}

void TexturedDepthMesh::End_ProjTexObj()
{
  // END TEXTURE-OBJECT GENERATION (JUST DISABLE PROJECTIVE TEXTURING)
  // DELETE OF OLD TEX OBJ IS HANDLED BY Begin_ProjTexObj
  if (UseProjTex)
    DisableProjTex();  
}

//-----------------------------------------------------------------------------
// DISPLAY STATE SETUP : required before and after drawing a TexturedDepthMesh.
//   This handles the inverse transformation setup and the disabling of all
//   unnecessary GL state.
//-----------------------------------------------------------------------------
void TexturedDepthMesh::Begin_DisplayState()
{
  glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
  GLfloat M[16];
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glMultMatrixf( GetXform_Screen2Obj(M,nCols,nRows) );
}

void TexturedDepthMesh::End_DisplayState()
{
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();  // LIGHTING
}

//-----------------------------------------------------------------------------
// Begin/End TexturedDepthMesh drawing setup
//-----------------------------------------------------------------------------
void TexturedDepthMesh::Begin_DrawSetup()
{
  Begin_DisplayList();
  Begin_ProjTexObj();
  Begin_DisplayState();
}

void TexturedDepthMesh::End_DrawSetup()
{
  End_DisplayState();
  End_ProjTexObj();
  End_DisplayList();
}

//----------------------------------------------------------------------------
// Enable projective textures.
// All object drawn hereafter will have a projected texture applied. This
// means that textures objects must have there textures disabled or we must
// use two-passes to properly apply the object texture and the modulation
// projected texture. The texture is bound and the appropriate modes are set.
// WARNING: THIS ROUTINE PUSHES THE TEXTURE ATTRIBUTE BIT, POP IS IN DisableProjTex.
//----------------------------------------------------------------------------
void TexturedDepthMesh::EnableProjTex()
{
  if (!glIsTexture(TexObjID)) 
    { printf("ERROR (EnableProjTex): TexObjID does not exist!\n"); return; }

  glPushAttrib(GL_TEXTURE_BIT);

  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D,TexObjID);  

  // INIT AUTOMATIC TEXTURE COORDINATE GENERATION, SUCH THAT COORDS ARE DEFINED
  // DIRECTLY FROM VERTICES IN OBJECT SPACE (NO glTexCoords calls needed).
  GLfloat eyePlaneS[] = {1.0, 0.0, 0.0, 0.0}; 
  GLfloat eyePlaneT[] = {0.0, 1.0, 0.0, 0.0}; 
  GLfloat eyePlaneR[] = {0.0, 0.0, 1.0, 0.0}; 
  GLfloat eyePlaneQ[] = {0.0, 0.0, 0.0, 1.0}; 
  glTexGenfv(GL_S, GL_EYE_PLANE, eyePlaneS); 
  glTexGenfv(GL_T, GL_EYE_PLANE, eyePlaneT); 
  glTexGenfv(GL_R, GL_EYE_PLANE, eyePlaneR); 
  glTexGenfv(GL_Q, GL_EYE_PLANE, eyePlaneQ); 
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR); 
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR); 
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR); 
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR); 
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
  glLoadIdentity(); 
  glTranslatef(0.5,0.5,0); 
  glScalef(0.5,0.5,1); 
  glMultMatrixf( GetProjectionMatrix(M) ); 
  glMultMatrixf( GetModelviewMatrix(M) );
}

//----------------------------------------------------------------------------
// Disable the projective texture by popping the texture matrix stack and
// by restoring texture properties. MUST BE CALLED AFTER USING PROJECTIVE
// TEXTURE! Must be paired with EnableProjTex.
//----------------------------------------------------------------------------
void TexturedDepthMesh::DisableProjTex()
{
  glPopAttrib();  // GL_TEXTURE_BIT
}
