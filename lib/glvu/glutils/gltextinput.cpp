//------------------------------------------------------------------------------
// File : gltextinput.cpp
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

#include "gltextinput.hpp"

#include <GL/glut.h>
#include "glutils.hpp"

#ifdef _WIN32
#include <windows.h> // for clipboard stuff
#else
#define MUST_BE_XWIN
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#endif

#include "glvutext.hpp"

using namespace GLVUText;
using std::string;

static void defaultCommitFunc(GLTextInputField *field)
{
  fprintf(stderr, "You typed: %s\n", field->getText());
  fprintf(stderr, "Set a commit func to make this actually do something!\n");
  field->close();
}

#ifdef MUST_BE_XWIN
// This is an ugly hack that reaches into GLUT's internals
// to get the needed pointer to the XDisplay
extern Display *__glutDisplay;

static bool XWinGetClipboardContents(string& output)
{
  // Based on code from http://www.enfin.com/getweb/X-copy+paste.txt
  // This sort of works, but it usually returns the old selection once
  // and I can't figure out how to make it return the latest.
  // Maybe I need to flush the event queue or something?
  // I have no idea.  X baffles me.

  output = "";

  Display *dpy = __glutDisplay;
  Atom  type;
  // XSelectInput(dpy, w, StructureNotifyMask+ExposureMask);
  int format, result;
  unsigned long len, bytes_left, dummy;
  unsigned char *data;
  Window Sown;

  /* Create an atom for CLIPBOARD */
  //Atom XA_CLIPBOARD = XInternAtom (dpy, "CLIPBOARD", False);
  //Sown = XGetSelectionOwner (dpy, XA_CLIPBOARD);
  Sown = XGetSelectionOwner (dpy, XA_PRIMARY);

  if (Sown == None) { return false; }
  //printf ("Selection owner %i\n", (int)Sown);
  
  // Maybe I should do an XSendEvent of SelectionRequest?

  XConvertSelection (dpy, 
                     XA_PRIMARY, 
                     XA_STRING, 
                     None,
                     Sown,
                     CurrentTime);
  
  XFlush (dpy);
  //
  // Do not get any data, see how much data is there
  //
  XGetWindowProperty (dpy, Sown, 
                      XA_STRING, 	  // Tricky..
                      0, 0,	  	  // offset - len
                      0, 	 	  // Delete 0==FALSE
                      AnyPropertyType,    //flag
                      &type,		  // return type
                      &format,	          // return format
                      &len, &bytes_left,  //that 
                      &data);				
  /*
  printf ("type:%i len:%i format:%i byte_left:%i\n", 
          (int)type, len, format, bytes_left);
  */

  // DATA found
  if (bytes_left > 0)
  {
    result = XGetWindowProperty (dpy, Sown, 
                                 XA_STRING, 0,bytes_left,0,
                                 AnyPropertyType, &type,&format,
                                 &len, &dummy, &data);
    if (result == Success) {
      // printf ("DATA IS HERE!!```%s'''\n", data);
      output += (char*)(data);
    }
    // else
    //  printf ("FAIL\n");
    XFree (data);
  }
  return true;
}
static bool XWinSetClipboardContents(const string& cliptext )
{
  // not implemented -- In X, the owner of the clipboard data
  // is responsible for giving the data out to requestors.
  // There is no global clipboard per se.  That means you must handle
  // the SelectionRequest messages in the event queue, and thanks to
  // GLUT we have no way to get inside the event queue.
  return false;
}
#endif /* X11 CLIPBOARD HANDLING */

#ifdef _WIN32
static bool Win32GetClipboardContents(string& cliptext)
{
  if (IsClipboardFormatAvailable(CF_TEXT)) 
  {
    OpenClipboard(0);
    HANDLE clip = GetClipboardData(CF_TEXT);
    if (clip)
    {
      cliptext = ((const char*)clip);
      // turn \n\r into space
      string::size_type start = 0;
      for (;;) {
        start = cliptext.find_first_of("\n\r", start);
        if (start == string::npos) break;
        cliptext[start] = ' ';
        start++;
      }
    }
    CloseClipboard();
    return true;
  }
  return false;
}
static bool Win32SetClipboardContents(const string& clipstr)
{
  OpenClipboard(0);
  EmptyClipboard();
  int len = clipstr.size();
  HANDLE hnd = GlobalAlloc(GMEM_MOVEABLE,sizeof(char)*len+1);
  LPTSTR  lptstrCopy;
  lptstrCopy = (LPTSTR)GlobalLock(hnd); 
  memcpy(lptstrCopy, clipstr.c_str(), len+1 * sizeof(TCHAR)); 
  lptstrCopy[len] = (TCHAR) 0;    // null character 
  GlobalUnlock(hnd);
  SetClipboardData(CF_TEXT, hnd);
  CloseClipboard();
  return true;
}
#endif /* WIN32 CLIPBOARD HANDLING */

#if defined(_WIN32)
 inline bool WinGetClipboardContents(string &cliptext)
 {  return Win32GetClipboardContents(cliptext); }
 inline bool WinSetClipboardContents(const string &cliptext)
 {  return Win32SetClipboardContents(cliptext); }
#elif defined(MUST_BE_XWIN)
 inline bool WinGetClipboardContents(string &cliptext)
 {  
   return XWinGetClipboardContents(cliptext); 
 }
 inline bool WinSetClipboardContents(const string &cliptext)
 {  return XWinSetClipboardContents(cliptext); }
#else
 inline bool WinGetClipboardContents(string &cliptext)
 {  return false; }
 inline bool WinSetClipboardContents(const string &cliptext)
 {  return false; }
#endif


GLTextInputField::GLTextInputField()
{
  common_init();
}

void GLTextInputField::common_init()
{
  m_caretpos = 0;
  m_alignment = ALIGN_LEFT;
  m_insertMode = true;
  m_commitFunc = defaultCommitFunc;
  m_shown = false;
}


void GLTextInputField::draw(int x, int y)
{
  if (!m_shown) return;
  int WW = glutGet((GLenum)GLUT_WINDOW_WIDTH);
  int WH = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

  glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_COLOR_MATERIAL);
  glPushAttrib(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DITHER);     
  glPushAttrib(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

  glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(0, 0, WW, WH);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WW, 0, WH);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  // Draw the caret -- recalc coords based on font width & current characters
  // glColor3f(1.0,1.0,1.0); use current gl color instead

  if (m_alignment == ALIGN_RIGHT ||
      m_alignment == ALIGN_CENTER) {
    int totalw = 0;
    const char *s = m_text.c_str();
    while (*s) {
      totalw += glutBitmapWidth( GetCurrentFont(), *s++);
    }
    if (m_label.size()!=0) {
      s = m_label.c_str();
      while (*s) {
        totalw += glutBitmapWidth( GetCurrentFont(), *s++);
      }
      totalw += glutBitmapWidth( GetCurrentFont(), ' ');
    }
    if (m_alignment==ALIGN_CENTER) totalw/=2;
    x-=totalw;
  }
  if (x<0) { 
    x=0;
  }
  glRasterPos2i(x,y);

  if (!m_label.empty()) {
    const char *l = m_label.c_str();
    while (*l)
    {
      glutBitmapCharacter(GetCurrentFont(),*l++);
    }
    glutBitmapCharacter(GetCurrentFont(),' ');
  }
  const char *p = m_text.c_str();
  int c = 0;
  int caretp[4];
  while (*p) 
  {
    if (c==m_caretpos) {
      glGetIntegerv(GL_CURRENT_RASTER_POSITION, caretp);
    }
    c++;
    glutBitmapCharacter(GetCurrentFont(),*p++);
  }
  if (c==m_caretpos) {
    glGetIntegerv(GL_CURRENT_RASTER_POSITION, caretp);
  }

  drawCaret(caretp[0], caretp[1]);
  /*
  const unsigned char caret_bmap[] = 
  {
    0x01, 0x80, //.... ...1 1... ....
    0x03, 0xC0, //.... ..11 11.. ....
    0x07, 0xE0, //.... .111 111. ....
    0x0E, 0x70, //.... 111. .111 ....
    0x1C, 0x38, //...1 11.. ..11 1...
    0x38, 0x1C, //..11 1... ...1 11..
    0x70, 0x0E, //.111 .... .... 111.
    0xE0, 0x07  //111. .... .... .111
  };
  glPushMatrix();
  glLoadIdentity();
  {
    glRasterPos2i(caretp[0],caretp[1]);
  //GLint UnpackAlignment;
  //glGetIntegerv(GL_UNPACK_ALIGNMENT,&UnpackAlignment); 
  //glPixelStorei(GL_UNPACK_ALIGNMENT,1); 
    glBitmap(16,8,0,0,0,0,caret_bmap);
  //glPixelStorei(GL_UNPACK_ALIGNMENT,UnpackAlignment); 
  }
  glPopMatrix();
*/


  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glPopAttrib();

  glPopAttrib();
  glPopAttrib();
  glPopAttrib();

}

void GLTextInputField::drawCaret(int x, int y)
{
  if ( /*((int)(m_blinkTime.GetTime()*2))%2*/ 1)
  {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(x,y, 0);
    if ( m_insertMode ) {
      glBegin(GL_LINE_STRIP); {
        glVertex2i(-4,-5);
        glVertex2i(0,-2);
        glVertex2i(4,-5);
      }
      glEnd();
    }
    else {
      
      int cw =
        ( m_caretpos < m_text.size() ) 
        ? glutBitmapWidth( GetCurrentFont(), m_text[m_caretpos] ) 
        : glutBitmapWidth( GetCurrentFont(), 'n' );
      glBegin(GL_QUADS); {
        glVertex2i( 0,-2);
        glVertex2i( 0,-4);
        glVertex2i( cw,-4);
        glVertex2i( cw,-2);
      }
      glEnd();
    }
    glPopMatrix();
  }
  
}

void GLTextInputField::show()
{
  m_shown = true;
  glutPostRedisplay();
}
void GLTextInputField::hide()
{
  m_shown = false;
  glutPostRedisplay();
}
void GLTextInputField::clear()
{
  m_text.erase();
  m_caretpos = 0;
}
void GLTextInputField::close()
{
  hide();
  clear();
}
void GLTextInputField::setLabel( const char *text)
{
  m_label = text;
}
void GLTextInputField::setText( const char *text)
{
  m_text = text;
}
void GLTextInputField::setAlignment( int alignEnum )
{
  m_alignment = alignEnum;
}


bool GLTextInputField::filterKeyDown(unsigned char key, int x, int y)
{
  if (!m_shown) return false;

  if (isprint(key)) {
    if ( !m_insertMode && m_caretpos < m_text.size() ) {
      m_text.erase(m_caretpos, 1);
    }
    m_text.insert(m_caretpos, 1, key);
    m_caretpos++;
  }
  else if (key == 127) { // delete
    if (m_caretpos < m_text.size()) {
      m_text.erase(m_caretpos,1);
    }
  }
  else if (key == 8) { // backspace
    if (m_caretpos > 0) {
      m_caretpos--; 
      m_text.erase(m_caretpos,1);
    }
  }
  else if (key == 13) { // enter/return
    commit();
  }
  else if (key == 27) { // escape -- cancel input
    hide();
    clear();
  }
  else if (glutGetModifiers() & GLUT_ACTIVE_CTRL) {
    int ckey = 'A'-1 + key;
    switch (ckey) {
      // these are mostly emacs-style keys
      case 'A':
        m_caretpos = 0; break;
      case 'E':
        m_caretpos = m_text.size(); break;
      case 'B':
        if (m_caretpos > 0) m_caretpos--; break;
      case 'F':
        if (m_caretpos < m_text.size()) m_caretpos++; break;
      case 'D':
        if (m_caretpos < m_text.size()) {
          m_text.erase(m_caretpos,1);
        }
        break;
      case 'H': // backspace
        if (m_caretpos > 0) {
          m_caretpos--; 
          m_text.erase(m_caretpos,1);
        }
        break;
      case 'K': 
        {
          string cutstr = m_text.substr(m_caretpos);
          if (cutstr.size()!=0) 
            WinSetClipboardContents(cutstr);
          m_text.erase(m_caretpos);
        }
        break;
      case 'Y': // 'yank'
      case 'V': // paste
      {
        string cliptext;
        if (WinGetClipboardContents(cliptext))
        {
          m_text.insert(m_caretpos, cliptext);
          m_caretpos+=cliptext.size();
        }
      }
      break;
      case 'C': // copy current input contents
        WinSetClipboardContents(m_text);
        break;
      default:
        return false;
    }
  }
  else {
    return false;
  }
  glutPostRedisplay();
  return true;
}

bool GLTextInputField::filterSpecialKeyDown(int key, int x, int y)
{
  if (!m_shown) return false;
  switch (key)
  {
    case GLUT_KEY_LEFT:
      if (m_caretpos > 0) m_caretpos--;
      break;
    case GLUT_KEY_RIGHT:
      if (m_caretpos < m_text.size()) m_caretpos++;
      break;
    case GLUT_KEY_END:
      m_caretpos = m_text.size();
      break;
    case GLUT_KEY_HOME:
      m_caretpos = 0;
      break;
    case GLUT_KEY_INSERT:
      m_insertMode = !m_insertMode;
      break;
    default:
      return false;
  }

  glutPostRedisplay();
  return true;

}


bool GLTextInputField::filterMouseMotion(int x, int y)
{
  if (!m_shown) return false;
  // catch 

  return false;
}

bool GLTextInputField::filterPassiveMotion(int x, int y)
{
  if (!m_shown) return false;
  return false;
}

bool GLTextInputField::filterMouseButton(int button, int state, int x, int y)
{
  if (!m_shown) return false;
#ifdef MUST_BE_XWIN
  if ( button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN ) {
    string cliptext;
    if ( WinGetClipboardContents(cliptext) )
    {
      m_text.insert(m_caretpos, cliptext);
      m_caretpos+=cliptext.size();
      glutPostRedisplay();
    }
    return true;
  }
#endif
  return false;
}

void GLTextInputField::commit()
{
  // call commit func if set
  if (m_commitFunc)
    m_commitFunc( this );
}


