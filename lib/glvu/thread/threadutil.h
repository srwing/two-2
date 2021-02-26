//------------------------------------------------------------------------------
// File : threadutil.h
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
  threadutil.h
  --
  Description : A collection of common utilties macros and typedefs
                This library also sets up a new new_handler for when
        dynamic allocation fails.
        
        It also helps to define some pc equivalents for unix calls
        so that code can be compiled on both (ie, unistd.h,
        sockets, and sleep).

        Debugging is turned off by defining NDEBUG, while some
        extra debugging might be turned on by defining _DEBUG.
        These both conform with the MSVC++ compiler defaults.
        
        This file also includes windows.h (needed for glut.h
        GL/gl.h).

        This file should be free of my classes so that there are
        no include difficulties.

  ----------------------------------------------------------------------------
  Author: weberh
  Created: Tue Sep  9 02:20:27 1997
  Revised: Wed Dec 09 14:15:06 1998 by weberh
  $Source: /afs/cs.unc.edu/project/walk/playpen/cvs/glvu/thread/threadutil.h,v $
  $Locker:  $
  $Revision: 1.2 $
\*****************************************************************************/
#ifndef _MYUTIL_H_
#define _MYUTIL_H_

//#include <iostream.h>
//#include <iomanip.h>
//#include <fstream.h>
//#include <new.h>
//#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>

#endif

/*-----------------------------*/
/*****  Constants/#defines *****/
/*-----------------------------*/

// for ease in writing print statements
#define NL << "\n"
#define TAB << "\t"


/*-----------------------------*/
/*****       Macros        *****/
/*-----------------------------*/
// for those that need to be tested at run time regardless of debugging status
// do_exit is defined to allow user to set breakpoint there to find failure
// and have stack trace

#define ALL_ASSERT(exp, msg) if(!(exp)){ cerr NL << "Assertion failed! " << \
endl << msg << " (" << __FILE__ << ", " << __LINE__ << ")" NL; do_exit(-1);}

// if you define NDEBUG, then regular asserts are removed
#ifndef NDEBUG
#define ASSERT(exp, msg) ALL_ASSERT(exp, msg)
#else
#define ASSERT(exp, msg) ((void) 0)
#endif


/*-----------------------------*/
/*****      Typedefs       *****/
/*-----------------------------*/
#ifdef FALSE
#undef FALSE
#endif
#ifdef TRUE
#undef TRUE
#endif
enum { FALSE=0, TRUE=1 };
// want bool to be an int so we can
// return comparisons as bools
typedef int Boolean;
typedef Boolean Bool;

/*-----------------------------*/
/***** Function Prototypes *****/
/*-----------------------------*/
// all assertions go thru this, so you can set a breakpoint here
// when an assertion happens and then look at the stack
void do_exit( int iExitCode );

/************************************************************
  Config Info Utilties
************************************************************/
// returns -1 on failure, otherwise# of processors on current machine
int numProcessors();

/************************************************************
  Time and Timeval utilities
************************************************************/

// these were grabbed from vrpn_Shared.h
// for sockets and gettimeofday

#ifdef _WIN32
#include <sys/timeb.h>
#include <winsock.h>   // timeval is defined here
  /* from HP-UX */
#ifndef _STRUCT_TIMEVAL
#define _STRUCT_TIMEVAL
struct timezone {
    int     tz_minuteswest; /* minutes west of Greenwich */
    int     tz_dsttime;     /* type of dst correction */
};
#endif //ifndef _STRUCT_TIMEVAL
#else
// unix time headers
#include <sys/time.h>
#endif


// timeval calculation routines
struct timeval operator+( const struct timeval& tv1, 
              const struct timeval& tv2 );
struct timeval operator-( const struct timeval& tv1, 
              const struct timeval& tv2 );
extern struct timeval timevalSum( const struct timeval& tv1, 
                  const struct timeval& tv2 );
extern struct timeval timevalDiff( const struct timeval& tv1, 
                   const struct timeval& tv2 );
extern double timevalToMsecs( const struct timeval& tv1 );
extern struct timeval timevalFromMsecs( const double dMsecs );

#ifdef _WIN32
// header needed for unix sleep in pc
#include <time.h>
#include <winbase.h>
unsigned int sleep( unsigned int cSeconds );
#else
// for pc Sleep in unix
void Sleep( unsigned int cMSecs );
#endif



// short-time-period sleep
void sleepMsecs( double dMsecs );


/************************************************************
 Unbuffered, nonblocking input
************************************************************/
// returns 0 if no char, 1 if char (and fills it in pch), -1 on error
// fEcho says whether or not to print the char when it is read 
int nonblockingGetch(char *, int fEcho=1);

#endif  //_UTIL_H_



/*****************************************************************************\
  $Log: threadutil.h,v $
  Revision 1.2  2002/03/13 08:22:04  harrism
  Added a free software license / disclaimer to all GLVU / QGLVU files.

  Revision 1.1.1.1  2002/02/20 20:08:57  baxter
  Moved from glvuext to main glvu directory.  It's proven itself to be worthy.

  Revision 1.2  2001/10/04 13:36:47  baxter
  Removed unused cruft, and explicit inclusion of iostream.h

  Revision 1.1.1.1  2000/10/16 15:08:48  baxter
  Improved WeberH Thread class

  Revision 1.1  2000/04/25 20:47:10  baxter
  initial checkin

  Revision 1.5  1998/12/09 23:35:02  weberh
  separated out gettimeofday so util can be compiled independently of vrpn lib
  use

  Revision 1.4  1998/10/28 19:52:10  weberh
  fixes for unix/pc compatibility

  Revision 1.3  1998/10/02 22:27:42  weberh
  *** empty log message ***

  Revision 1.2  1998/05/01 05:10:39  weberh
  *** empty log message ***

  Revision 1.1  1998/04/23 04:14:33  weberh
  Initial revision

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

  Revision 1.19  1998/02/19 22:22:50  weberh
  *** empty log message ***

  Revision 1.18  1998/02/10 21:26:47  weberh
   got rid of all_assert

  Revision 1.17  1998/02/09 19:34:13  weberh
  *** empty log message ***

  Revision 1.16  1997/12/21 10:34:57  weberh
  better gettimeofday, etc. for pc

  Revision 1.15  1997/12/14 08:15:30  weberh
  timing calls added -- tvDiff, tvSum tvElapsedMsecs
  also windwos gettimeofday implemented.

  Revision 1.14  1997/12/11 06:38:08  weberh
  added pc sleep and gettimeofday

  Revision 1.13  1997/12/08 21:46:34  weberh
  *** empty log message ***

  Revision 1.12  1997/12/01 22:35:53  weberh
  *** empty log message ***

  Revision 1.11  1997/11/17 23:04:08  weberh
  added binary read/write

  Revision 1.10  1997/11/15 00:27:33  weberh
  lots of read and write.

  Revision 1.9  1997/11/12 18:10:06  weberh
  added binary write and read for basic types.

  Revision 1.8  1997/11/04 06:51:46  weberh
  utility routines for adjusting endianess

  Revision 1.7  1997/10/30 16:26:22  weberh
  added proper new handler call for windows

  Revision 1.6  1997/10/28 21:24:44  weberh
  compiles with proper enums

  Revision 1.5  1997/10/10 20:06:37  weberh
  *** empty log message ***

  Revision 1.4  1997/10/10 17:10:26  weberh
  fixed for windows

  Revision 1.3  1997/09/18 20:00:10  weberh
  fixed up assert msg

  Revision 1.2  1997/09/17 07:44:35  weberh
  *** empty log message ***

  Revision 1.1  1997/09/17 07:39:51  weberh
  Initial revision

  Revision 1.5  1997/09/16 20:28:20  weberh
  fixed up enum so true=1 is confirmed
  /

  Revision 1.4  1997/09/16 20:22:00  weberh
  added NL, SP, TAB, and new_handler

  Revision 1.3  1997/09/09 21:53:07  weberh
  small changes for windows

  Revision 1.2  1997/09/09 08:54:08  weberh
  got rid of assert all together -- just my own c++ macro now.

  Revision 1.1  1997/09/09 06:23:12  weberh
  Initial revision

\*****************************************************************************/

