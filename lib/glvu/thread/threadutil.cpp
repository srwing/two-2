//------------------------------------------------------------------------------
// File : threadutil.cpp
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

/*****************************************************************************\
  util.c
  --
  Description : the header file has some nice macros and typedefs, this file
                just defines an object to effect a change before the "main"
        routine runs (it sets up a new new_handler)

        NOTE: if you never call any routine in this file, then
              the BeforeMain obj is not created, and the new 
              handler is not redirected.  ASSERT calls 
                      do_exit, so this should not be a problem from
              now on.

  ----------------------------------------------------------------------------
  Author: weberh
  Created: Wed Sep 17 03:30:10 1997
  Revised: Wed Dec 09 14:23:31 1998 by weberh
  $Source: /afs/cs.unc.edu/project/walk/playpen/cvs/glvu/thread/threadutil.cpp,v $
  $Locker:  $
  $Revision: 1.2 $
\*****************************************************************************/

#include <iostream>
using namespace std;
#include "threadutil.h"
#include <math.h>


#if defined(sgi) || defined(__linux__)
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#endif
void do_exit( int iExitCode ) {
#if defined(sgi) && defined(_DEBUG)
  cerr << "Util:do_exit: process/thread " << getpid() << " exited." NL;
#endif
  exit(iExitCode);
}


// includes for numProcessors call
#ifdef sgi
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#endif

int numProcessors() {
  int cProcessors=-1;
#ifdef sgi
  if ((cProcessors = (int) sysconf(_SC_NPROC_ONLN)) < 0) {
    perror("Util:numProcessors:");
  }
#elif defined(_WIN32)
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  cProcessors = (int) si.dwNumberOfProcessors;
#else
  cerr << "Util:numProcessors: not implemented on this architecture." NL;
#endif
#ifdef _DEBUG
  cerr << "Util:numProcessors: " << cProcessors << " detected." NL;
#endif
  return cProcessors;
}

////////////////////////////////////////////
// These calls switch from input
// endianness eIn to current endianness
////////////////////////////////////////////

#if 0
unsigned short usEndian( unsigned short us, Endianness eIn ) {
  if (eIn==ENDIANNESS) {
    return us;
  }
  unsigned short usTemp;
  ((char *)&usTemp)[0] = ((char *)&us)[1];
  ((char *)&usTemp)[1] = ((char *)&us)[0];
  return usTemp;
}

unsigned long ulEndian( unsigned long ul, Endianness eIn ) {
  if (eIn==ENDIANNESS) {
    return ul;
  }
  unsigned long ulTemp;
  ((char *)&ulTemp)[0] = ((char *)&ul)[3];
  ((char *)&ulTemp)[1] = ((char *)&ul)[2];
  ((char *)&ulTemp)[2] = ((char *)&ul)[1];
  ((char *)&ulTemp)[3] = ((char *)&ul)[0];
  return ulTemp;
}

float fEndian( float f, Endianness eIn ) {
  if (eIn==ENDIANNESS) {
    return f;
  }
  float fTemp;
  ((char *)&fTemp)[0] = ((char *)&f)[3];
  ((char *)&fTemp)[1] = ((char *)&f)[2];
  ((char *)&fTemp)[2] = ((char *)&f)[1];
  ((char *)&fTemp)[3] = ((char *)&f)[0];
  return fTemp;
}

double dEndian( double d, Endianness eIn ) {
  if (eIn==ENDIANNESS) {
    return d;
  }
  double dTemp;
  ((char *)&dTemp)[0] = ((char *)&d)[7];
  ((char *)&dTemp)[1] = ((char *)&d)[6];
  ((char *)&dTemp)[2] = ((char *)&d)[5];
  ((char *)&dTemp)[3] = ((char *)&d)[4];
  ((char *)&dTemp)[4] = ((char *)&d)[3];
  ((char *)&dTemp)[5] = ((char *)&d)[2];
  ((char *)&dTemp)[6] = ((char *)&d)[1];
  ((char *)&dTemp)[7] = ((char *)&d)[0];
  return dTemp;
}
#endif

/// These are binary writing functions and reading functions

#if 0
ostream& write( ostream& os, void * const & pv ) {
  return os.write( (char *)&pv, sizeof(pv) );
}

ostream& write( ostream& os, const char& ch ) {
  return os.write( &ch, sizeof(ch) );
}

ostream& write( ostream& os, const unsigned char& uch ) {
  return os.write( (char *)&uch, sizeof(uch) );
}

ostream& write( ostream& os, const int& i ) {
  return os.write( (char *)&i, sizeof(i) );
}

ostream& write( ostream& os, const unsigned int& ui ) {
  return os.write( (char *)&ui, sizeof(ui) );
}

ostream& write( ostream& os, const float& f ) {
  return os.write( (char *)&f, sizeof(f) );
}

ostream& write( ostream& os, const double& d ) {
  return os.write( (char *)&d, sizeof(d) );
}

istream& read( istream& is, char& ch ) {
  return is.read( (char *)&ch, sizeof(ch) );
}

istream& read( istream& is, unsigned char& uch ) {
  return is.read( (char *)&uch, sizeof(uch) );
}

istream& read( istream& is, int& i ) {
  return is.read( (char *)&i, sizeof(i) );
}

istream& read( istream& is, unsigned int& ui ) {
  return is.read( (char *)&ui, sizeof(ui) );
}

istream& read( istream& is, float& f ) {
  return is.read( (char *)&f, sizeof(f) );
}

istream& read( istream& is, double& d ) {
  return is.read( (char *)&d, sizeof(d) );
}
#endif

#if 0
Boolean gGlDebug=FALSE;

void glDebugOn() {
  gGlDebug=TRUE;
}

void glDebugOff() {
  gGlDebug=FALSE;
}
#endif

#if 0
// timeval math ops

struct timeval operator+( const struct timeval& tv1, 
              const struct timeval& tv2 ) {
  return timevalSum( tv1, tv2 );
}

struct timeval operator-( const struct timeval& tv1, 
              const struct timeval& tv2 ) {
  return timevalDiff( tv1, tv2 );
}

// Calcs the sum of tv1 and tv2.  Returns the sum in a timeval struct.
// Calcs negative times properly, with the appropriate sign on both tv_sec
// and tv_usec (these signs will match unless one of them is 0).
// NOTE: both abs(tv_usec)'s must be < 1000000 (ie, normal timeval format)
struct timeval timevalSum( const struct timeval& tv1, 
               const struct timeval& tv2 ) {
  struct timeval tvSum=tv1;

  tvSum.tv_sec += tv2.tv_sec;
  tvSum.tv_usec += tv2.tv_usec;

  // do borrows, etc to get the time the way i want it: both signs the same,
  // and abs(usec) less than 1e6
  if (tvSum.tv_sec>0) {
    if (tvSum.tv_usec<0) {
      tvSum.tv_sec--;
      tvSum.tv_usec += 1000000;
    } else if (tvSum.tv_usec >= 1000000) {
      tvSum.tv_sec++;
      tvSum.tv_usec -= 1000000;
    }
  } else if (tvSum.tv_sec<0) {
    if (tvSum.tv_usec>0) {
      tvSum.tv_sec++;
      tvSum.tv_usec -= 1000000;
    } else if (tvSum.tv_usec <= -1000000) {
      tvSum.tv_sec--;
      tvSum.tv_usec += 1000000;
    }
  } else {
    // == 0, so just adjust usec
    if (tvSum.tv_usec >= 1000000) {
      tvSum.tv_sec++;
      tvSum.tv_usec -= 1000000;
    } else if (tvSum.tv_usec <= -1000000) {
      tvSum.tv_sec--;
      tvSum.tv_usec += 1000000;
    }
  }

  return tvSum;
}


// Calcs the diff between tv1 and tv2.  Returns the diff in a timeval struct.
// Calcs negative times properly, with the appropriate sign on both tv_sec
// and tv_usec (these signs will match unless one of them is 0)
struct timeval timevalDiff( const struct timeval& tv1, 
                const struct timeval& tv2 ) {
  struct timeval tv;

  tv.tv_sec = -tv2.tv_sec;
  tv.tv_usec = -tv2.tv_usec;

  return timevalSum( tv1, tv );
}

double timevalToMsecs( const struct timeval& tv ) {
  // problem with ms optimizations, so temp var
  // double d;
  // d =  tv.tv_sec*1000.0 + tv.tv_usec/1000.0;
  //  return d;
  return tv.tv_sec*1000.0 + tv.tv_usec/1000.0;
}

struct timeval timevalFromMsecs( const double dMsecs ) {
  struct timeval tv;
  tv.tv_sec = (long) floor(dMsecs/1000.0);
  tv.tv_usec = (long) ((dMsecs/1000.0 - tv.tv_sec)*1e6);
  return tv;
}
#endif

#ifdef _WIN32
// sleeps for iSeconds
unsigned int sleep( unsigned int cSeconds ) {
  Sleep( cSeconds*1000 );
  // clock_t ctGoal = CLOCKS_PER_SEC*cSeconds + clock();
  //   while( ctGoal > clock() )
  //      ;
  //   // no signals, no "unslept" time
   return 0;
} 
#else
#if 0
void Sleep( unsigned int cMSecs ) {
  sleepMsecs( cMSecs );
}
#endif
#endif

#if 0
class __BeforeMain {
public:
  // this constructor will get called for the static var below
  // and will set the new_handler appropriately
  __BeforeMain() { 
#ifdef _WIN32
    _set_new_handler(my_new_handler);
#else
    set_new_handler(my_new_handler); 
#endif
  }
};

static __BeforeMain __bm;
#endif

#ifndef _WIN32
#if 0
//extern int gettimeofday(struct timeval *tp, struct timezone *tzp);
extern int gettimeofday(struct timeval *tp, void *tzp);

void sleepMsecs( double dMsecs ) {
  struct timeval tvStart, tvNow;
  gettimeofday(&tvStart, NULL);
  do {
    gettimeofday(&tvNow, NULL);
  } while (timevalToMsecs(tvNow-tvStart)<dMsecs);
}
#endif
#endif


// need to init and shutdown socket dll -- 0 is success
// (windows socket dll keeps track of number of open/close for us,
// we just need to call the same number of closes as opens)

// (note, however, because of vrpn we can't close and open --
// it still thinks the connections are alive, so we only
// close when the program ends and only if gfSocketsInited 
// is true)

// SO, TO USE SOCKETS for win and non-win apps, be sure to include
// an object of type UseSockets in one of your .cpp files.
#if 0

#ifdef _WIN32
#include <winsock.h>

static int gfSocketsInited = 0;

static int initSockets() {
  WSADATA wsaData; 
  int status;
  if (gfSocketsInited==0) {
    if ((status = WSAStartup(MAKEWORD(1,1), &wsaData)) != 0) {
      cerr << "initSockets: WSAStartup failed (error code " << status 
       << " returned)." NL;
      return status;
    }
    cerr << "initSockets: WSAStartup succeeded." NL;
    gfSocketsInited=1;
  } else {
    // cerr << "initSockets: WSA already started up." NL;
  }
  return 0;
}

static int shutDownSockets() {
  if (gfSocketsInited==1) {
    int status;
    if ((status = WSACleanup()) !=0) {
      cerr << "shutDownSockets: WSACleanup failed  (error code " << status 
       << " returned)." NL;
      return status;
    } 
    cerr << "shutDownSockets: WSACleanup succeeded." NL;
    gfSocketsInited = 0;
    return 0;
  } else {
    // cerr << "shutDownSockets: socket package not inited." NL;
    return -1;
  }
}
#endif
#endif

#if 0
UseSockets::UseSockets() {
#ifdef _WIN32
    initSockets();
#endif
}

UseSockets::~UseSockets() {
#ifdef _WIN32
    shutDownSockets();
#endif
}
#endif

#ifdef _WIN32
#include <conio.h>
#else
#include <termio.h>
static int gfNonblockingInputInited=0;
static struct termio gsttyResetArgs;
#endif

// returns 0 if no char, 1 if char (and fills it in pch), -1 on error
int nonblockingGetch( char *pch, int fEcho ) {
#ifdef _WIN32
  // no init necessary
  if (_kbhit()) {
    *pch = (char) _getch();
    // echo the character (since this line setting does not)
    if (fEcho) {
      cerr << *pch;
    }
    return 1;
  } else {
    return 0;
  }
#elif defined(sgi)
  if (!gfNonblockingInputInited) {
    struct termio sttyArgs;
    
    /* get current settings */
    if ( ioctl(STDIN_FILENO, TCGETA, &gsttyResetArgs) == -1 ) {
      perror("nonblockingGetch: ioctl get on stdin failed");
      return(-1);
    }
    sttyArgs = gsttyResetArgs;
    
    // so it will recognize ctrl-z and ctrl-backslash
    // LDISC1 is bsd, LDISC0 is system V (ignores crtl-z)
    sttyArgs.c_line = LDISC1;
    
    // put into non-canonical mode (not line buffered) ...
    sttyArgs.c_lflag &= ~ICANON;
    
    // but pay attention to signals
    sttyArgs.c_lflag &= ISIG;
    
    // don't block on a read (return imm if no chars available)
    sttyArgs.c_cc[VMIN] = 0;
    sttyArgs.c_cc[VTIME] = 0;
    
    if ( ioctl(STDIN_FILENO, TCSETA, &sttyArgs) == -1 ) {
      perror("nonblockingGetch: ioctl set on stdin failed");
      return(-1);
    }

    gfNonblockingInputInited = 1;
  }

  int cRead;
  if (cRead = read(STDIN_FILENO, pch, 1)) {
    if (cRead<0) {
      perror("nonblockingGetch: error reading from stdin");
      return -1;
    } else {
      // echo the character (since this line setting does not)
      if (fEcho) {
    cerr << *pch;
      }
      return 1;
    }
  } else {
    return 0;
  }

  // could use this and make a class that inits and resets 
  // terminal characteristics, but it seems unnecessary.
  // reset terminal
  //  if ( ioctl(STDIN_FILENO, TCSETA, &sttyResetArgs) == -1 ) {
  //    perror("tRecord: ioctl reset on stdin failed");
  //   return(-1);
  //  }

#else
  cerr << "nonblockingGetch: not yet supported on this platform" NL;
  return -1;
#endif
}

/*****************************************************************************\
  $Log: threadutil.cpp,v $
  Revision 1.2  2002/03/13 08:22:04  harrism
  Added a free software license / disclaimer to all GLVU / QGLVU files.

  Revision 1.1.1.1  2002/02/20 20:08:57  baxter
  Moved from glvuext to main glvu directory.  It's proven itself to be worthy.

  Revision 1.4  2001/10/17 18:00:52  sud
  Added stdlib.h to sgi || linux includes to define exit() under linux

  Revision 1.3  2001/10/04 13:37:28  baxter
  Include iostreams now that threadutil.h doesn't do it for us.

  Revision 1.2  2000/10/22 19:39:48  baxter
  Commented out some stuff that isn't needed for thread operation

  Revision 1.1.1.1  2000/10/16 15:08:48  baxter
  Improved WeberH Thread class

  Revision 1.1  2000/04/25 20:47:10  baxter
  initial checkin

  Revision 4.8  1998/12/09 23:35:03  weberh
  separated out gettimeofday so util can be compiled independently of vrpn lib
  use

  Revision 4.7  1998/10/28 19:52:11  weberh
  fixes for unix/pc compatibility

  Revision 4.6  1998/10/02 22:27:42  weberh
  *** empty log message ***

  Revision 4.5  1998/05/01 05:10:39  weberh
  *** empty log message ***

  Revision 4.4  1998/04/23 19:01:03  weberh
  *** empty log message ***

  Revision 4.3  1998/04/23 04:14:33  weberh
  *** empty log message ***

  Revision 4.2  1998/03/31 21:32:38  weberh
  sgi shutdown good now.

  Revision 4.1  1998/03/31 03:03:17  weberh
   pc threading compiles
  >

  Revision 4.0  1998/03/29 22:40:41  weberh
   sgi threading works. now on to pc threading.

  Revision 3.4  1998/03/25 23:06:26  weberh
  *** empty log message ***

  Revision 3.3  1998/03/16 16:04:15  weberh
  *** empty log message ***

  Revision 3.2  1998/03/15 22:18:38  weberh
  *** empty log message ***

  Revision 3.1  1998/03/05 15:18:48  weberh
  added scale func

  Revision 3.0  1998/02/25 20:13:18  weberh
  all in good shape for viewer

  Revision 1.12  1998/02/19 22:22:50  weberh
  *** empty log message ***

  Revision 1.11  1997/12/21 10:34:57  weberh
  better gettimeofday, etc. for pc

  Revision 1.10  1997/12/14 08:15:30  weberh
  timing calls added -- tvDiff, tvSum tvElapsedMsecs
  also windwos gettimeofday implemented.

  Revision 1.9  1997/12/11 06:38:08  weberh
  added pc sleep and gettimeofday

  Revision 1.8  1997/12/09 19:42:14  weberh
  *** empty log message ***

  Revision 1.7  1997/12/01 22:35:53  weberh
  *** empty log message ***

  Revision 1.6  1997/11/17 23:04:08  weberh
  added binary read/write

  Revision 1.5  1997/11/15 00:27:33  weberh
  lots of read and write.

  Revision 1.4  1997/11/12 18:10:06  weberh
  added binary write and read for basic types.

  Revision 1.3  1997/11/04 06:51:46  weberh
  utility routines for adjusting endianess

  Revision 1.2  1997/10/30 16:26:22  weberh
  added proper new handler call for windows

  Revision 1.1  1997/09/17 07:40:10  weberh
  Initial revision

\*****************************************************************************/

