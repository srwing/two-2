//------------------------------------------------------------------------------
// File : thread.cpp
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
  thread.cpp
  --
  Description :

  ----------------------------------------------------------------------------
  Author: weberh
  Created: Mon Mar 23 16:51:16 1998
  Revised: Fri Nov 13 14:42:18 1998 by weberh
  $Source: /afs/cs.unc.edu/project/walk/playpen/cvs/glvu/thread/thread.cpp,v $
  $Locker:  $
  $Revision: 1.3 $
\*****************************************************************************/

#include "thread.hpp"

#if defined(__linux__)
#include <errno.h>
#include <unistd.h>
#elif defined(sgi)
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

#include <iostream>
using namespace std;
#include "threadutil.h"

//============================================================================
// SEMAPHORE CLASS
//============================================================================


//----------------------------------------------------------------------------
// init all fields in init()
Semaphore::Semaphore( int cNumResources ) : 
  cResources(cNumResources)
{
  init();
}

//----------------------------------------------------------------------------
// create a new internal structure for the semaphore
// (binary copy is not ok)
// This does not copy the state of the semaphore
Semaphore::Semaphore( const Semaphore& s ) : 
  cResources(s.cResources)
{
  init();
}

//----------------------------------------------------------------------------
void Semaphore::init() {
#ifdef sgi
  if (Semaphore::ppaArena==NULL) {
    Semaphore::allocArena();
  }
  if (cResources==1) {
    fUsingLock=TRUE;
    ps=NULL;
    // use lock instead of semaphore
    if ((l = usnewlock(Semaphore::ppaArena)) == NULL) {
      cerr << "Semaphore::Semaphore: error allocating lock from arena." NL;
      return;
    }
  } else {    
    fUsingLock=FALSE;
    l=NULL;
    if ((ps = usnewsema(Semaphore::ppaArena, cResources)) == NULL) {
      cerr << "Semaphore::Semaphore: error allocating semaphore from arena." NL;
      return;
    }
  }
#elif defined(_WIN32)
  // args are security, initial count, max count, and name
  int maxResources = 255 > cResources ? 255 : cResources;
  if ((hSemaphore = CreateSemaphore(NULL, cResources, maxResources, NULL)) == NULL) {
    // get error info from windows (from FormatMessage help page)
    LPVOID lpMsgBuf;
    
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
           FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,    GetLastError(),
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
             // Default language
           (LPTSTR) &lpMsgBuf,    0,    NULL );
    cerr << "Semaphore::Semaphore: error creating semaphore, "
      "WIN32 CreateSemaphore call caused the following error: "
     << (char*)lpMsgBuf NL;
    // Free the buffer.
    LocalFree( lpMsgBuf );
    return;
  }
#elif defined(__linux__)
  int err = sem_init(&sem, 0 /*pshared*/, cResources);
  if (err) {
    cerr << "Semaphore::Semaphore: error allocating semaphore." NL;
  }
#endif
}

//----------------------------------------------------------------------------
Semaphore::~Semaphore() {
#ifdef sgi
  if (fUsingLock) {
    usfreelock( l, Semaphore::ppaArena );
  } else {
    usfreesema( ps, Semaphore::ppaArena );
  }
#elif defined(_WIN32)
  if (!CloseHandle(hSemaphore)) {
    // get error info from windows (from FormatMessage help page)
    LPVOID lpMsgBuf;
    
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
           FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,    GetLastError(),
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
             // Default language
           (LPTSTR) &lpMsgBuf,    0,    NULL );
    cerr << "Semaphore::~Semaphore: error destroy semaphore, "
      "WIN32 CloseHandle call caused the following error: "
     << (char*)lpMsgBuf NL;
    // Free the buffer.
    LocalFree( lpMsgBuf );
  }
#elif defined(__linux__)
  int err = sem_destroy(&sem);
  if (err) {
    cerr << "Semaphore::~Semaphore: error destroying semaphore" NL;
  }
#endif
}

//----------------------------------------------------------------------------
// routine to reset it
int Semaphore::reset( int cNumResources ) {
#ifdef sgi
  cResources = cNumResources;
  if (fUsingLock) {
    if (cResources==1) {
      if (usinitlock( l )) {
        perror("Semaphore::reset: usinitlock:");
        return -1;
      }
    } else {
      // need to make new semaphore
      usfreelock( l, Semaphore::ppaArena );
      init();
    }
  } else {
    if (cResources!=1) {
      if (usinitsema( ps, cNumResources )) {
    perror("Semaphore::reset: usinitsema:");
    return -1;
      }
    } else {
      // need to make new lock
      usfreesema( ps, Semaphore::ppaArena );
      init();
    }
  }
#elif defined(_WIN32)
  cResources = cNumResources;
  // close the old one and create a new one
  if (!CloseHandle(hSemaphore)) {
    // get error info from windows (from FormatMessage help page)
    LPVOID lpMsgBuf;
    
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
           FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,    GetLastError(),
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
             // Default language
           (LPTSTR) &lpMsgBuf,    0,    NULL );
    cerr << "Semaphore::reset: error destroying semaphore, "
      "WIN32 CloseHandle call caused the following error: "
     << (char*)lpMsgBuf NL;
    // Free the buffer.
    LocalFree( lpMsgBuf );
  }
  init();
#elif defined(__linux__)
  cerr << "Reset not implemented for linux yet" NL;
#endif
  return 0;
}

//----------------------------------------------------------------------------
// routines to use it (p blocks, cond p does not)
// 1 on success, -1 fail
int Semaphore::p() {
#ifdef sgi
  if (fUsingLock) {
    if (ussetlock(l)!=1) {
      perror("Semaphore::p: ussetlock:");
      return -1;
    }
  } else {
    if (uspsema(ps)!=1) {
      perror("Semaphore::p: uspsema:");
      return -1;
    }
  }
#elif defined(_WIN32)
  switch (WaitForSingleObject(hSemaphore, INFINITE)) {
  case WAIT_OBJECT_0:
    // got the resource
    break;
  case WAIT_TIMEOUT:
    ASSERT(0,"Semaphore::p: infinite wait time timed out!");
    return -1;
    break;
  case WAIT_ABANDONED:
    ASSERT(0,"Semaphore::p: thread holding resource died");
    return -1;
    break;
  case WAIT_FAILED:
    // get error info from windows (from FormatMessage help page)
    LPVOID lpMsgBuf;
    
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
           FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,    GetLastError(),
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
           // Default language
           (LPTSTR) &lpMsgBuf,    0,    NULL );
    cerr << "Semaphore::p: error waiting for resource, "
      "WIN32 WaitForSingleObject call caused the following error: "
     << (char*)lpMsgBuf NL;
    // Free the buffer.
    LocalFree( lpMsgBuf );
    return -1;
    break;
  default:
    ASSERT(0,"Semaphore::p: unknown return code");
    return -1;
  }
#elif defined(__linux__)
  int err = sem_wait(&sem);
  if (err) {
    ASSERT(0,"Semaphore::p: unknown return code");
  }
#endif
  return 1;
}

//----------------------------------------------------------------------------
// 0 on success, -1 fail
int Semaphore::v() {
#ifdef sgi
  if (fUsingLock) {
    if (usunsetlock(l)) {
      perror("Semaphore::v: usunsetlock:");
      return -1;
    }
  } else {
    if (usvsema(ps)) {
      perror("Semaphore::v: uspsema:");
      return -1;
    }
  }
#elif defined(_WIN32)
  if (!ReleaseSemaphore(hSemaphore,1,NULL)) {
    // get error info from windows (from FormatMessage help page)
    LPVOID lpMsgBuf;
    
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
           FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,    GetLastError(),
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
             // Default language
           (LPTSTR) &lpMsgBuf,    0,    NULL );
    cerr << "Semaphore::v: error v'ing semaphore, "
      "WIN32 ReleaseSemaphore call caused the following error: "
     << (char*)lpMsgBuf NL;
    // Free the buffer.
    LocalFree( lpMsgBuf );
    return -1;
  }
#elif defined(__linux__)
  int err = sem_post(&sem);
  if (err) {
    cerr << "Semaphore:v: error v'ing semaphore" NL;
      return -1;
  }
#endif
  return 0;
}

//----------------------------------------------------------------------------
// 0 if it can't get the resource, 1 if it can
// -1 if fail
int Semaphore::condP() {
  int iRetVal=1;
#ifdef sgi
  if (fUsingLock) {
    // don't spin at all
    iRetVal = uscsetlock(l, 0);
    if (iRetVal<0) {
      perror("Semaphore::condP: uscsetlock:");
      return -1;
    }
  } else {
    iRetVal = uscpsema(ps);
    if (iRetVal<0) {
      perror("Semaphore::condP: uscpsema:");
      return -1;
    }
  }
#elif defined(_WIN32)
  switch (WaitForSingleObject(hSemaphore, 0)) {
  case WAIT_OBJECT_0:
    // got the resource
    break;
  case WAIT_TIMEOUT:
    // resource not free
    iRetVal=0;
    break;
  case WAIT_ABANDONED:
    ASSERT(0,"Semaphore::condP: thread holding resource died");
    return -1;
    break;
  case WAIT_FAILED:
    // get error info from windows (from FormatMessage help page)
    LPVOID lpMsgBuf;
    
    FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
           FORMAT_MESSAGE_FROM_SYSTEM,
           NULL,    GetLastError(),
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
           // Default language
           (LPTSTR) &lpMsgBuf,    0,    NULL );
    cerr << "Semaphore::condP: error waiting for resource, "
      "WIN32 WaitForSingleObject call caused the following error: "
     << (char*)lpMsgBuf NL;
    // Free the buffer.
    LocalFree( lpMsgBuf );
    return -1;
    break;
  default:
    ASSERT(0,"Semaphore::p: unknown return code");
    return -1;
  }
#elif defined(__linux__)
  int err = sem_trywait(&sem);
  if (!err) 
    iRetVal = 1;
  if (err == EAGAIN)
    iRetVal = 0;
  else 
    iRetVal = -1;
#endif
  return iRetVal;
}

//----------------------------------------------------------------------------
int Semaphore::numResources() {
  return cResources;
}

// static var definition
#ifdef sgi
usptr_t *Semaphore::ppaArena = NULL;

// this will set things up so that usinit creates temp files which never
// show up in the file system (can't be used across processes, but fine
// in a single process with multiple threads).
static ptrdiff_t __gpdDummy = usconfig( CONF_ARENATYPE, US_SHAREDONLY );
// this will allow more than 8 threads (it will allow 128 threads)
static ptrdiff_t __gpdDummy2 = usconfig( CONF_INITUSERS, 128 );

// for umask stuff
#include <sys/types.h>
#include <sys/stat.h>

//----------------------------------------------------------------------------
void Semaphore::allocArena() {
#if 0
  char rgchTemp[]="/tmp/weberh.XXXXXX";
  // generate a unique filename
  mktemp(rgchTemp);
  if (rgchTemp[0]=='\0') {
    cerr << "Thread::allocArena: could not generate unique file name." NL;
    return;
  }
  // set mask to read/write/execute by all so temp file can be overwritten
  mode_t mOldUMask = umask(S_IRWXO);
#endif
  // /dev/zero is a special file which can only be shared between
  // processes/threads which share file descriptors.
  // It never shows up in the file system.
  if ((ppaArena = usinit("/dev/zero"))==NULL) {
    perror("Thread::allocArena: usinit:");
  }
#if 0
  umask(mOldUMask);
#endif
}
#endif

//============================================================================
// THREAD CLASS
//============================================================================

//----------------------------------------------------------------------------
// ThreadGlobals is a singleton class that ensures static data is always
// initialized properly.  Since construction of globals happens before main() 
// is called, this is guaranteed to happen before multi-threading kicks in
// (i.e. before go() is called on any Thread).
class ThreadGlobals 
{
public:
  static ThreadGlobals &get() {
    return globals;
  }
#if defined(__linux__)
  pthread_key_t key;
#elif defined(_WIN32)
  static __declspec( thread ) Thread* cur;
#endif

#ifdef _WIN32
public:
#else
private:
#endif
  ThreadGlobals() {
# if defined(__linux__)
    if (pthread_key_create(&key,0)) {
      perror("pthread TLD key creation failed!");
    }
# elif defined(_WIN32)
    cur = 0;
#endif
  }
  ~ThreadGlobals() {
# if defined(__linux__)
    if (pthread_key_delete(key)) {
      perror("pthread TLD key deletion failed!");
    }
#endif
  }
private:
  static ThreadGlobals globals;
};
ThreadGlobals ThreadGlobals::globals;
#if defined(_WIN32)
__declspec(thread) Thread *ThreadGlobals::cur;
#endif

//----------------------------------------------------------------------------
Thread::Thread(void (*_pfThread)(void *pvThreadData), void *pvUD)
  : pfThread(_pfThread), pvUserdata(pvUD), ulProcID(0) 
{ 
  // Make sure globals have been constructed.
  ThreadGlobals::get();
}

//----------------------------------------------------------------------------
Thread::Thread()
  : pfThread(0), pvUserdata(0), ulProcID(0) 
{ 
  ThreadGlobals::get();
}

//----------------------------------------------------------------------------
int Thread::go(void *pvNewUserData)
{
  userData(pvNewUserData);
  return go();
}

//----------------------------------------------------------------------------
int Thread::go() 
{
  if (ulProcID) {
    cerr << "Thread::go: already running (pid=" << ulProcID << ")." NL;
    return -1;
  }
  PID_t pid;
#ifdef sgi
  if ((pid=sproc( &threadFuncShell, PR_SALL, (void *)this ))==
      ((PID_t)-1)) {
    perror("Thread::go: sproc");
    return -1;
  }
#elif defined(_WIN32)
  // pass in func, let it pick stack size, and arg to pass to thread
  unsigned int thrdID;
  if ((ulProcID=_beginthreadex( 
         NULL, // security
         0, // stack size
         &threadUIntFuncShell, // start address
         (void *)this, // arglist
         0,  // create running
         &thrdID)
       ) == 0 )
  {
    perror("Thread::go: _beginthreadex");
    return -1;
  }
  //  cerr << "Thread:go: thread id is " << ulProcID << "." NL;
#elif defined(__linux__)
  int err = pthread_create(&pid, 0, threadVoidFuncShell, (void*)this);
  if (err) {
    perror("Thread::go: pthread_create");
    return -1;
  }
#else
  cerr << "Thread::go: threads not yet implemented for this arch." NL;
  return -1;
#endif
  return 0;
}

//----------------------------------------------------------------------------
int Thread::kill() 
{
  // kill the os thread
  if (ulProcID>0) {
#ifdef sgi
    if (::kill(ulProcID, SIGKILL)<0) {
      perror("Thread::kill: kill:");
      return -1;
    }
#elif defined(_WIN32)
    if (!TerminateThread( (HANDLE) ulProcID, -1 )) {
      cerr << "Thread::kill: problem with terminateThread call." NL;
    }
    CloseHandle((HANDLE)ulProcID);
#elif defined(__linux__)
    int err = pthread_cancel(ulProcID);
    if (err) {
      cerr << "Thread::kill: problem with pthread_cancel." NL;
      return -1;
    }
#else
#endif
  } else {
    cerr << "Thread::kill: thread is not currently alive." NL;
    return -1;
  }
  ulProcID = 0;
  return 0;
}

//----------------------------------------------------------------------------
int Thread::join()
{
  if (ulProcID>0) {
#if defined(sgi)
    PID_t pid = waitpid(ulProcID, 0, 0);
    if (pid <= 0) {
      perror("Thread::join: waitpid:");
      return -1;
    }
#elif defined(_WIN32)
    DWORD ret = WaitForSingleObject( (HANDLE)ulProcID, INFINITE );
    if (ret == WAIT_FAILED) {
      // get error info from windows (from FormatMessage help page)
      LPVOID lpMsgBuf;
    
      FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                     FORMAT_MESSAGE_FROM_SYSTEM,
                     NULL,    GetLastError(),
                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
                     // Default language
                     (LPTSTR) &lpMsgBuf,    0,    NULL );
      cerr << "Thread::join: error waiting for thread to exit, "
        "WIN32 WaitForSingleObject call caused the following error: "
           << (char*)lpMsgBuf NL;
      // Free the buffer.
      LocalFree( lpMsgBuf );
      return -1;
    }
#elif defined(__linux__)
    int ret = pthread_join(ulProcID, 0);
    if (ret) {
      perror("Thread::join: pthread_join:");
      return -1;
    }
#endif
  }
  return 0;
}

//----------------------------------------------------------------------------
bool Thread::running() {
  return ulProcID!=0;
}

//----------------------------------------------------------------------------
Thread::PID_t Thread::pid() {
  return ulProcID;
}
//----------------------------------------------------------------------------

Thread *Thread::getCurrent() {
  Thread *t = 0;
#if defined(sgi)
  t = *(Thread**)(&(PRDA->usr_prda.fill));
#elif defined(_WIN32)
  t = ThreadGlobals::get().cur;
#elif defined(__linux__)
  t = (Thread*) pthread_getspecific(ThreadGlobals::get().key);
#endif
  return t;
}

//----------------------------------------------------------------------------
bool Thread::available() {
#ifdef THREADS_AVAILABLE
  return TRUE;
#else
  return FALSE;
#endif
}

//----------------------------------------------------------------------------
void Thread::userData( void *pvNewUserData ) {
  pvUserdata = pvNewUserData;
}

//----------------------------------------------------------------------------
void *Thread::userData() {
  return pvUserdata;
}

//----------------------------------------------------------------------------
void Thread::run() {
  ASSERT(0,"Thread::run: run() method should be overridden in a subclass\n"
         "if no callback func is specified");
}


//----------------------------------------------------------------------------
// utility funcs
void Thread::threadFuncShell( void *pvThread ) {
  Thread *pth = (Thread *)pvThread;
  // Make sure PID is up to date, not defined if parent will set it
  // before we start running.  So to make sure pid() returns the right
  // thing either way, both parent and child set it.
#if defined(sgi)
  pth->ulProcID = getpid();
#elif defined(__linux__)
  pth->ulProcID = pthread_self();
#elif defined(_WIN32)
  // doesn't seem to be a way to get the same thread handle from the child.
  // just spin lock wait for it.  (Probably won't have to wait-- seems parent
  // always runs first on win32)
  while (pth->ulProcID == 0) {
    Sleep(0); // this is yield on win32
  }
#endif  

  // Stash the current Thread in thread-specific storage for 
  // getCurrent() to use later.
#if defined(sgi)
  // PRDA is a system-defined global, thread-specific pointer.  The
  // sproc man page mentions it but just refers the reader to the
  // header file <sys/prctl.h> itself for details.  The header,
  // however, doesn't explain how to use PRDA, so I'm just guessing
  // that 'usr_prda' is the part reserved for user applications.
  Thread **thrhdl = (Thread**)(&(PRDA->usr_prda.fill));
  *thrhdl = pth;
  ASSERT(*(Thread**)(&(PRDA->usr_prda.fill)) == pth,
         "Unable to set thread local data!");
#elif defined(_WIN32)
  ThreadGlobals::get().cur = pth;
#elif defined(__linux__)
  pthread_setspecific(ThreadGlobals::get().key, (void*)pth);
#endif
  if (pth->pfThread)
    pth->pfThread( pth->pvUserdata );
  else
    pth->run();
  // Thread has stopped running if we get here.  Erase id.
  pth->ulProcID = 0;
}
//----------------------------------------------------------------------------
void* Thread::threadVoidFuncShell( void *pvThread ) {
  threadFuncShell(pvThread);
  return 0;
}
//----------------------------------------------------------------------------
unsigned int THREAD_STDCALL Thread::threadUIntFuncShell( void *pvThread ) {
  threadFuncShell(pvThread);
  return 0;
}

//----------------------------------------------------------------------------
Thread::~Thread() {
  if (running()) {
    kill();
  }
}

//----------------------------------------------------------------------------
void Thread::msleep(unsigned long msec)
{
#if defined (_WIN32)
  Sleep(msec);
#elif defined (sgi)
  // sgi's crazy usleep will only sleep up to a second
  if (msec > 1000L) {
    sleep(msec/1000L);
    usleep( (msec%1000L) * 1000L );
  }
  else usleep( msec * 1000L );
#else
  usleep( msec * 1000L );
#endif
}

/*****************************************************************************\
  $Log: thread.cpp,v $
  Revision 1.3  2002/08/31 22:32:18  baxter
  Changed Win32 version to use beginthreadex instead of beginthread.
  Apparently the return value from beginthread isn't actually a valid
  Windows HANDLE.  That might explain some error message I've seen
  on occasion out of this Win32 threads implementation.

  Revision 1.2  2002/03/13 08:22:03  harrism
  Added a free software license / disclaimer to all GLVU / QGLVU files.

  Revision 1.1.1.1  2002/02/20 20:08:54  baxter
  Moved from glvuext to main glvu directory.  It's proven itself to be worthy.

  Revision 1.9  2001/12/11 22:45:33  baxter
  Fixed race condition in setting pid member.  Both parent and child
  were setting the thread, and if child exited before parent set it,
  thread would forever think it was still running.  Bad.  Very bad.
  Side effect now is that it may be possible that if parent calls go()
  then immediately pid(), the pid may not have been set yet.  Depends on
  whether the child gets the first time slice after launching a thread
  or the parent.  The man pages aren't too clear on whether the child is
  guaranteed to get the first time slice.

  Revision 1.8  2001/10/04 13:37:28  baxter
  Include iostreams now that threadutil.h doesn't do it for us.

  Revision 1.7  2000/12/03 21:17:36  baxter
  Cast void* to a char* for printing Windows error messages.  Otherwise you just see the pointer value.

  Revision 1.6  2000/10/23 00:00:49  baxter
  Fixed the code to make child wait until pid() is set for WIN32.  There's
  something odd about these HANDLEs on win32.  They aren't like a pid on unix.
  you have to duplicate them and such in order to share them.  Very odd.

  Revision 1.5  2000/10/22 23:29:54  baxter
  SGI's getCurrent was broken.  Fixed now.
  SGI also turned up race condition in pid().  Was possible for child
  to call pid() and get 0 before parent had a chance to set it to something.
  Also fixed a missing return type that the PC didn't complain about.

  Revision 1.4  2000/10/22 22:25:02  baxter
  Added static function 'msleep'.  Platform indep. way to suspend current
  thread.

  Revision 1.3  2000/10/22 20:20:46  baxter
  Implemented getCurrent for Win32. Fixed big bug with Win32 semaphores.
  They were always being allocated as mutexes with an intial resource count
  of 1.  Test program revealed this bug.  Go test program!

  Revision 1.2  2000/10/22 19:42:24  baxter
  Added getCurrent() API for sgi and linux.  Win32 still to go.
  Also added virtual run() method that can be used to subclass Thread
  instead of giving it a callback func.

  Revision 1.1.1.1  2000/10/16 15:08:48  baxter
  Improved WeberH Thread class

  Revision 1.1  2000/04/25 20:47:09  baxter
  initial checkin

  Revision 4.4  1998/11/13 19:46:32  weberh
  fixed <= to < in condP.

  Revision 4.3  1998/10/02 22:27:28  weberh
  *** empty log message ***

  Revision 4.2  1998/04/23 04:14:25  weberh
  *** empty log message ***

  Revision 4.1  1998/03/31 03:03:04  weberh
   pc threading compiles
  >

  Revision 4.0  1998/03/29 22:40:29  weberh
   sgi threading works. now on to pc threading.

  Revision 1.3  1998/03/28 21:07:40  weberh
  works.

  Revision 1.2  1998/03/25 23:06:22  weberh
  *** empty log message ***

  Revision 1.1  1998/03/23 22:57:19  weberh
  Initial revision

\*****************************************************************************/

