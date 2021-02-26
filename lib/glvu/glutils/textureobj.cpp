//------------------------------------------------------------------------------
// File : textureobj.cpp
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
// textureobj.cpp
//============================================================================

#include "textureobj.hpp"
#include <stdio.h>

//----------------------------------------------------------------------------
// CREATES A TEXTURE OBJECT FROM A TIGHTLY-PACKED RGB COLOR ARRAY
// USER SPECIFIES:
//   WRAP            (GL_CLAMP,GL_REPEAT)
//   INTERP MODES    (GL_NEAREST,GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, ETC...)
//   INTERNAL FORMAT (1,2,3,4, GL_RGB, GL_RGBA, ETC...)
//   DATA FORMAT     (GL_RGB, GL_RGBA, ETC...)
//----------------------------------------------------------------------------
GLuint CreateTextureObjFromColorBuffer(GLubyte *Color, int Width, int Height,
                       GLfloat       WrapMode,
				       GLfloat     InterpMode,
				       GLint   internalFormat,
                       GLenum          format)
{
  GLuint TexID;
  
  // GET AN UNUSED TEXTURE ID
  glGenTextures(1,&TexID);             

  // CREATE A NEW TEXTURE OBJECT NAMED TexID
  glBindTexture(GL_TEXTURE_2D,TexID);  

  // SET TEXTURE WRAPPING AND FILTERING PARAMS
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, InterpMode); 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, InterpMode); 

  // UNPACKING FROM A PACKED COLOR ARRAY, SET BYTE ALIGNMENT TO 1
  int UnpackAlignment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT,&UnpackAlignment);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);

  // DEFINE THE TEXTURE FROM THE COLOR ARRAY
  glTexImage2D(GL_TEXTURE_2D,0,internalFormat,Width,Height,0,
	       format,GL_UNSIGNED_BYTE,Color);

  // RESTORE PREVIOUS UNPACK ALIGNMENT
  glPixelStorei(GL_UNPACK_ALIGNMENT,UnpackAlignment);

  return( TexID );
}

//----------------------------------------------------------------------------
// CREATES A TEXTURE OBJECT FROM A TIGHTLY-PACKED RGB COLOR ARRAY
// USER SPECIFIES WRAP (GL_CLAMP,GL_REPEAT) AND INTERP MODES (GL_NEAREST,GL_LINEAR)
// IF USEALHPA IS TRUE, ASSUMES THAT THE COLOR ARRAY CONTAINS 4 BYTES (RGBA).
//----------------------------------------------------------------------------
GLuint CreateTextureObjFromColorBuffer(GLubyte *Color, int Width, int Height,
                                       GLfloat WrapMode, GLfloat InterpMode,
                                       bool UseAlpha)
{
  GLint  internalFormat;
  GLenum format;
  if (UseAlpha) {
    internalFormat = GL_RGBA;
    format = GL_RGBA;
  } else {
    internalFormat = GL_RGB;
    format = GL_RGB;
  }
  return 
    CreateTextureObjFromColorBuffer(
      Color, Width, Height, 
      WrapMode, 
      InterpMode, 
      internalFormat, format);
}

//----------------------------------------------------------------------------
// CREATES A TEXTURE OBJECT FROM THE SPECIFIED BUFFER
// ReadBuffer IS TYPICALLY GL_BACK OR GL_FRONT
// USER SPECIFIES WRAP (GL_CLAMP,GL_REPEAT) AND INTERP MODES (GL_NEAREST,GL_LINEAR)
//----------------------------------------------------------------------------
GLuint CreateTextureObjFromColorBuffer(int Width, int Height,
				       GLint ReadBuffer,
                       GLfloat WrapMode, GLfloat InterpMode,
				       bool UseAlpha)
{
  GLuint TexID;
  GLenum colorFormat;
  
  // GET AN UNUSED TEXTURE ID
  glGenTextures(1,&TexID);             

  // CREATE A NEW TEXTURE OBJECT NAMED TexID
  glBindTexture(GL_TEXTURE_2D,TexID);  

  // SET TEXTURE WRAPPING AND FILTERING PARAMS
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, InterpMode); 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, InterpMode); 

  GLint OldReadBuffer;
  glGetIntegerv(GL_READ_BUFFER,&OldReadBuffer);
  glReadBuffer((GLenum)ReadBuffer);

  if (UseAlpha) colorFormat = GL_RGBA;
  else colorFormat = GL_RGB;
  
  // DEFINE THE TEXTURE FROM THE FRAMEBUFFER
  glCopyTexImage2D(GL_TEXTURE_2D,0,colorFormat,0,0,Width,Height,0);

  glReadBuffer((GLenum)OldReadBuffer); 

  return( TexID );
}

//----------------------------------------------------------------------------
// CREATES A TEXTURE OBJECT FROM A TIGHTLY-PACKED UCHAR DEPTH ARRAY
// USER SPECIFIES WRAP (GL_CLAMP,GL_REPEAT) AND INTERP MODES (GL_NEAREST,GL_LINEAR)
//----------------------------------------------------------------------------
GLuint CreateTextureObjFromDepthBuffer(GLubyte *Depth, int Width, int Height,
                                       GLfloat WrapMode, GLfloat InterpMode)
{
  GLuint TexID;

  // GET AN UNUSED TEXTURE ID
  glGenTextures(1,&TexID);             

  // CREATE A NEW TEXTURE OBJECT NAMED TexID
  glBindTexture(GL_TEXTURE_2D,TexID);  

  // SET TEXTURE WRAPPING AND FILTERING PARAMS
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, InterpMode); 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, InterpMode); 

  // UNPACKING FROM A PACKED COLOR ARRAY, SET BYTE ALIGNMENT TO 1
  int UnpackAlignment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT,&UnpackAlignment);
  glPixelStorei(GL_UNPACK_ALIGNMENT,1);

  // DEFINE THE TEXTURE FROM THE DEPTH ARRAY
  glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,Width,Height,0,GL_LUMINANCE,GL_UNSIGNED_BYTE,Depth);

  // RESTORE PREVIOUS UNPACK ALIGNMENT
  glPixelStorei(GL_UNPACK_ALIGNMENT,UnpackAlignment);

  return( TexID );
}

//----------------------------------------------------------------------------
// CREATES A TEXTURE OBJECT FROM THE SPECIFIED BUFFER
// ReadBuffer IS TYPICALLY GL_BACK OR GL_FRONT
// USER SPECIFIES WRAP (GL_CLAMP,GL_REPEAT) AND INTERP MODES (GL_NEAREST,GL_LINEAR)
//----------------------------------------------------------------------------
GLuint CreateTextureObjFromDepthBuffer(int Width, int Height, GLint ReadBuffer,
                                       GLfloat WrapMode, GLfloat InterpMode)
{
  GLuint TexID;

  // GET AN UNUSED TEXTURE ID
  glGenTextures(1,&TexID);             

  // CREATE A NEW TEXTURE OBJECT NAMED TexID
  glBindTexture(GL_TEXTURE_2D,TexID);  

  // SET TEXTURE WRAPPING AND FILTERING PARAMS
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapMode);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, InterpMode); 
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, InterpMode); 

  GLint OldReadBuffer;
  glGetIntegerv(GL_READ_BUFFER,&OldReadBuffer);
  glReadBuffer((GLenum)ReadBuffer);

  // DEFINE THE TEXTURE FROM THE FRAMEBUFFER
  glCopyTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE,0,0,Width,Height,0);

  glReadBuffer((GLenum)OldReadBuffer); 

  return( TexID );
}

void DeleteTextureObj(GLuint TexID)
{
  glDeleteTextures(1,&TexID);
}

int TextureObjExists(GLuint TexID)
{
  return( glIsTexture(TexID) );
}

//----------------------------------------------------------------------------
// This routine simply draws the texture in the bottom-left corner of the
// window (in the original form).
//----------------------------------------------------------------------------
void DrawTextureObj(GLuint TexID, int Width, int Height)
{
  glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
  glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
  glPushAttrib(GL_TEXTURE_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,TexID);  
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
    glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
  glMatrixMode(GL_TEXTURE); 
    glPushMatrix();
    glLoadIdentity(); 
  glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet((GLenum)GLUT_WINDOW_WIDTH), 
               0, glutGet((GLenum)GLUT_WINDOW_HEIGHT));

  glBegin(GL_QUADS);
    glTexCoord2f(0,0); glVertex2i(0,0);
    glTexCoord2f(1,0); glVertex2i(Width,0);
    glTexCoord2f(1,1); glVertex2i(Width,Height);
    glTexCoord2f(0,1); glVertex2i(0,Height);
  glEnd();

  glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  glMatrixMode(GL_TEXTURE); 
    glPopMatrix();
  glPopAttrib();  // GL_TEXTURE_BIT
  glPopAttrib();  // GL_DEPTH_BUFFER_BIT
  glPopAttrib();  // GL_LIGHTING_BIT
}
