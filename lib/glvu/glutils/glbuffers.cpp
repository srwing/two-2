//------------------------------------------------------------------------------
// File : glbuffers.cpp
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
// glbuffers.cpp : OpenGL buffer handling module. This routine was created
//   to modularize dealing with saving, restoring, and modifying the various 
//   OpenGL buffer (color, depth, stencil, etc).
//============================================================================

#include "glbuffers.hpp"
#include <iostream.h>
#include <ppm.hpp>

//----------------------------------------------------------------------------
// THIS SIMPLY SETS A PIXEL IN AN RGB COLOR IMAGE OF A GIVEN PIXEL WIDTH
// WITH THE GIVEN Color.
//----------------------------------------------------------------------------
inline void SetPixel(const GLubyte *Color, 
                     const int x, const int y, 
                     GLubyte *ColorBuffer, 
                     const int Width)
{
  int i = 3*(y*Width+x);
  ColorBuffer[i] = Color[0];
  ColorBuffer[i+1] = Color[1];
  ColorBuffer[i+2] = Color[2];
}

//----------------------------------------------------------------------------
// Draws a 1-pixel wide border of RGB color C around a color image
// (for proper clamping).
// NOTE: C is a 24-bit color, each component is an unsigned byte!
// Typical value for C are { 0,0,0 } or { 255,255,255 }
//----------------------------------------------------------------------------
void DrawBorder(const GLubyte *C, GLubyte *ColorBuffer, const int W,
		const int H)
{
  int i, W_1=W-1, H_1=H-1;
  for (i=0; i<W; i++)
  { SetPixel(C,i,0,ColorBuffer,W);
    SetPixel(C,i,H_1,ColorBuffer,W); }
  for (i=0; i<H; i++)
  { SetPixel(C,0,i,ColorBuffer,W);
    SetPixel(C,W_1,i,ColorBuffer,W); }
}

//----------------------------------------------------------------------------
// Saves and stores the current COLOR framebuffer in a color buffer array.
// PERFORMS AUTO-ALLOCATION OF Color ARRAY IF SET TO NULL BEFORE CALLING;
// OTHERWISE ASSUMES THAT COLOR HAS BEEN PRE-ALLOCED. If pre-alloced, must
// be large enough to hold entire window's color buffer.
//----------------------------------------------------------------------------
void SaveColorBuffer(GLubyte* &Color, int &Width, int &Height,
		     const GLint ReadBuffer)
{
  // GRAB THE BUFFER WITHIN THE CURRENTLY DEFINED VIEWPORT
  int VP[4];  // START (X,Y) AND WIDTH,HEIGHT
  glGetIntegerv(GL_VIEWPORT,VP);
  Width = VP[2];
  Height = VP[3];

  // SET PIXEL PACKING ALIGNMENTS TO BE ABLE TO STORE ARBITRARY RES IMAGE
  GLint PackAlignment;
  glGetIntegerv(GL_PACK_ALIGNMENT,&PackAlignment); 
  glPixelStorei(GL_PACK_ALIGNMENT,1); 

  GLint OldReadBuffer;
  glGetIntegerv(GL_READ_BUFFER,&OldReadBuffer);
  glReadBuffer((GLenum)ReadBuffer);

  if (Color==NULL) Color = new GLubyte[Width*Height*3];
  glReadPixels(VP[0],VP[1],VP[2],VP[3], GL_RGB, GL_UNSIGNED_BYTE, Color);
  
  glReadBuffer((GLenum)OldReadBuffer); 
}

//----------------------------------------------------------------------------
// Saves and stores the current ALPHA channel of the framebuffer in a grayscale.
//  array PERFORMS AUTO-ALLOCATION OF grayscale ARRAY IF SET TO NULL BEFORE CALLING;
// OTHERWISE ASSUMES THAT grayscale HAS BEEN PRE-ALLOCED. If pre-alloced, must
// be large enough to hold entire window's alpha channel [ W*H*sizeof(GLubyte) ].
//----------------------------------------------------------------------------
void SaveAlphaBuffer(GLubyte* &Alpha, int &Width, int &Height,
		     const GLint ReadBuffer)
{
  // GRAB THE BUFFER WITHIN THE CURRENTLY DEFINED VIEWPORT
  int VP[4];  // START (X,Y) AND WIDTH,HEIGHT
  glGetIntegerv(GL_VIEWPORT,VP);
  Width = VP[2];
  Height = VP[3];

  // SET PIXEL PACKING ALIGNMENTS TO BE ABLE TO STORE ARBITRARY RES IMAGE
  GLint PackAlignment;
  glGetIntegerv(GL_PACK_ALIGNMENT,&PackAlignment); 
  glPixelStorei(GL_PACK_ALIGNMENT,1); 

  GLint OldReadBuffer;
  glGetIntegerv(GL_READ_BUFFER,&OldReadBuffer);
  glReadBuffer((GLenum)ReadBuffer);

  if (Alpha==NULL) Alpha = new GLubyte[Width*Height];
  glReadPixels(VP[0],VP[1],VP[2],VP[3], GL_ALPHA, GL_UNSIGNED_BYTE, Alpha);
  
  glReadBuffer((GLenum)OldReadBuffer); 
}


//----------------------------------------------------------------------------
// Saves and stores the current COLOR framebuffer (RGBA, 4-channels) in a
// color buffer array.
// PERFORMS AUTO-ALLOCATION OF Color ARRAY IF SET TO NULL BEFORE CALLING;
// OTHERWISE ASSUMES THAT COLOR HAS BEEN PRE-ALLOCED. If pre-alloced, must
// be large enough to hold entire window's color buffer.
//----------------------------------------------------------------------------
void SaveColorAndAlphaBuffer(GLubyte* &Color, int &Width, int &Height,
                             const GLint ReadBuffer)
{
  // GRAB THE BUFFER WITHIN THE CURRENTLY DEFINED VIEWPORT
  int VP[4];  // START (X,Y) AND WIDTH,HEIGHT
  glGetIntegerv(GL_VIEWPORT,VP);
  Width = VP[2];
  Height = VP[3];

  // SET PIXEL PACKING ALIGNMENTS TO BE ABLE TO STORE ARBITRARY RES IMAGE
  GLint PackAlignment;
  glGetIntegerv(GL_PACK_ALIGNMENT,&PackAlignment); 
  glPixelStorei(GL_PACK_ALIGNMENT,1); 

  GLint OldReadBuffer;
  glGetIntegerv(GL_READ_BUFFER,&OldReadBuffer);
  glReadBuffer((GLenum)ReadBuffer);

  if (Color==NULL) Color = new GLubyte[Width*Height*4];
  glReadPixels(VP[0],VP[1],VP[2],VP[3], GL_RGBA, GL_UNSIGNED_BYTE, Color);
  
  glReadBuffer((GLenum)OldReadBuffer); 
}


//----------------------------------------------------------------------------
// Stores the current COLOR framebuffer in a ppm file.
//----------------------------------------------------------------------------
void SaveColorBufferToPPM(char *FileName, const GLint ReadBuffer)
{
  // GRAB THE BUFFER WITHIN THE CURRENTLY DEFINED VIEWPORT
  int VP[4];  // START (X,Y) AND WIDTH,HEIGHT
  glGetIntegerv(GL_VIEWPORT,VP);

  // SET PIXEL PACKING ALIGNMENTS TO BE ABLE TO STORE ARBITRARY RES IMAGE
  GLint PackAlignment;
  glGetIntegerv(GL_PACK_ALIGNMENT,&PackAlignment); 
  glPixelStorei(GL_PACK_ALIGNMENT,1); 

  GLint OldReadBuffer;
  glGetIntegerv(GL_READ_BUFFER,&OldReadBuffer);
  glReadBuffer((GLenum)ReadBuffer);

  GLubyte *Color = new GLubyte[VP[2]*VP[3]*3];
  glReadPixels(VP[0],VP[1],VP[2],VP[3], GL_RGB, GL_UNSIGNED_BYTE, Color);

  WritePPM(FileName, Color, VP[2], VP[3]);

  delete[] Color;
  
  glReadBuffer((GLenum)OldReadBuffer); 
}

//----------------------------------------------------------------------------
// Draw an RGB color buffer (24-bits per pixel) into the selected buffer.
//----------------------------------------------------------------------------
void DrawColorBuffer(GLubyte *Color, int Width, int Height,
		     const GLint DrawBuffer)
{
  GLint UnpackAlignment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT,&UnpackAlignment); 
  glPixelStorei(GL_UNPACK_ALIGNMENT,1); 

  GLint OldDrawBuffer;
  glGetIntegerv(GL_DRAW_BUFFER,&OldDrawBuffer);
  glDrawBuffer((GLenum)DrawBuffer);

  glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
  glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);
  glPushAttrib(GL_TEXTURE_BIT);
    glDisable(GL_TEXTURE_2D);

  glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet((GLenum)GLUT_WINDOW_WIDTH) - 1, 
               0, glutGet((GLenum)GLUT_WINDOW_HEIGHT) - 1);

  glRasterPos2i(0,0);
  glDrawPixels(Width,Height, GL_RGB, GL_UNSIGNED_BYTE, Color);

  glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

  glPopAttrib();  // GL_TEXTURE_BIT
  glPopAttrib();  // GL_DEPTH_BUFFER_BIT
  glPopAttrib();  // GL_LIGHTING_BIT

  glDrawBuffer((GLenum)OldDrawBuffer);
  glPixelStorei(GL_UNPACK_ALIGNMENT,UnpackAlignment); 
}

//----------------------------------------------------------------------------
// THIS SIMPLY SETS A PIXEL IN A DEPTH IMAGE OF A GIVEN PIXEL WIDTH
// WITH THE GIVEN Depth VALUE.
//----------------------------------------------------------------------------
inline void SetPixel(const GLfloat Depth, 
                     const int x, const int y, 
                     GLfloat *DepthBuffer, 
                     const int Width)
{
  DepthBuffer[ y*Width + x ] = Depth;
}

//----------------------------------------------------------------------------
// Saves and stores the current DEPTH framebuffer in a depth buffer array.
// PERFORMS AUTO-ALLOCATION OF Depth ARRAY IF SET TO NULL BEFORE CALLING;
// OTHERWISE ASSUMES THAT Depth HAS BEEN PRE-ALLOCED. If pre-alloced, must
// be large enough to hold entire window's color buffer.
//----------------------------------------------------------------------------
void SaveDepthBuffer(GLfloat* &Depth, int &Width, int &Height,
		     const GLint ReadBuffer)
{
  // GRAB THE BUFFER WITHIN THE CURRENTLY DEFINED VIEWPORT
  int VP[4];  // START (X,Y) AND WIDTH,HEIGHT
  glGetIntegerv(GL_VIEWPORT,VP);
  Width = VP[2];
  Height = VP[3];

  // SET PIXEL PACKING ALIGNMENTS TO BE ABLE TO STORE ARBITRARY RES IMAGE
  GLint PackAlignment;
  glGetIntegerv(GL_PACK_ALIGNMENT,&PackAlignment); 
  glPixelStorei(GL_PACK_ALIGNMENT,1); 

  GLint OldReadBuffer;
  glGetIntegerv(GL_READ_BUFFER,&OldReadBuffer);
  glReadBuffer((GLenum)ReadBuffer);

    if (Depth==NULL) Depth = new GLfloat[Width*Height];
    glReadPixels(VP[0],VP[1],VP[2],VP[3], GL_DEPTH_COMPONENT, GL_FLOAT, Depth);

  glReadBuffer((GLenum)OldReadBuffer); 
}

//----------------------------------------------------------------------------
// Draws a DEPTH buffer (32-bits per pixel - float) into the selected buffer.
//----------------------------------------------------------------------------
void DrawDepthBuffer(GLfloat *Depth, int Width, int Height,
		     const GLint DrawBuffer)
{
  GLint UnpackAlignment;
  glGetIntegerv(GL_UNPACK_ALIGNMENT,&UnpackAlignment); 
  glPixelStorei(GL_UNPACK_ALIGNMENT,1); 

  GLint OldDrawBuffer;
  glGetIntegerv(GL_DRAW_BUFFER,&OldDrawBuffer);
  glDrawBuffer((GLenum)DrawBuffer);

  glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
  glPushAttrib(GL_COLOR_BUFFER_BIT);
    glDisable(GL_BLEND);
    glColorMask(0,0,0,0);
  glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
  glPushAttrib(GL_TEXTURE_BIT);
    glDisable(GL_TEXTURE_2D);

  glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet((GLenum)GLUT_WINDOW_WIDTH) - 1, 
               0, glutGet((GLenum)GLUT_WINDOW_HEIGHT) - 1);

  glRasterPos2i(0,0);
  glDrawPixels(Width,Height, GL_DEPTH_COMPONENT, GL_FLOAT, Depth);

  glMatrixMode(GL_PROJECTION);
    glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

  glPopAttrib();  // GL_TEXTURE_BIT
  glPopAttrib();  // GL_DEPTH_BUFFER_BIT
  glPopAttrib();  // GL_COLOR_BUFFER_BIT
  glPopAttrib();  // GL_LIGHTING_BIT

  glDrawBuffer((GLenum)OldDrawBuffer);
  glPixelStorei(GL_UNPACK_ALIGNMENT,UnpackAlignment); 
}

