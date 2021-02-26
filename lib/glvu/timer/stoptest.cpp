//------------------------------------------------------------------------------
// File : stoptest.cpp
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

//------------------------------------------------------------------------------
// Stopwatch testing routines
// $Id: stoptest.cpp,v 1.7 2002/03/13 08:22:05 harrism Exp $
//------------------------------------------------------------------------------

#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#else /* _WIN32 */
#include <windows.h>
#define sleep(x) Sleep(x*1000)
#define usleep(x) Sleep(x/1000)
#endif /* _WIN32 */
#include "stopwatch.hpp"

#if 0
#include <iostream.h>
#else
#include <iostream>
using std::cout;
using std::endl;
using std::ostream;
#endif

static Stopwatch wglob("glob");  // make sure named constructor works
static CPUStopwatch cglob("CPUglob"); //  named constructor here too

void clock_tests(StopwatchBase &glob, StopwatchBase &loc, StopwatchBase *newed)
{
  int i;

  /////////////////////////////////////////////////////////////////////////

  cout << "--Timing one sleep(1) w/ some anonymous timers" << endl;
  glob.Start();  loc.Start();  newed->Start();
  sleep(1);
  newed->Stop();  loc.Stop();  glob.Stop();
  cout << glob << endl << loc << endl << *newed << endl;

  /////////////////////////////////////////////////////////////////////////

  cout << "--Timing one sleep(1) w/ named timers" << endl;
  loc.SetName("loc");  newed->SetName("newed");
  glob.Reset(); loc.Reset(); newed->Reset();
  glob.Start();  loc.Start();  newed->Start();
  sleep(1);
  newed->Stop();  loc.Stop();  glob.Stop();
  cout << glob << endl << loc << endl << *newed << endl;
  
  /////////////////////////////////////////////////////////////////////////

  cout << "--Timing 5 periods of usleep(200000)" << endl;
  glob.Reset();
  for (i=0; i<5; i++) {
    glob.Start();
    usleep(200000);
  }  
  glob.Stop();
  cout << glob << endl;

  /////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
  cout << "--Timing 5 more periods of usleep(200000) with gettimeofday for comparison" << endl;
  struct timeval t, tf;
  gettimeofday(&t,NULL);
  int j=0;
  for (i=0; i<5; i++) {
    j++;
    usleep(200000);
  }
  gettimeofday(&tf, NULL);
  float et = ((((double)tf.tv_sec) + ((double)tf.tv_usec / 1e6f)) 
    - (((double)t.tv_sec) + ((double)t.tv_usec / 1e6f))) / j;
  cout << "gettimeofday avg time: " << et << " sec, (avg of " << j << " periods" << endl;
#else
  cout << "--Timing 5 more periods of usleep(200000) with ftime for comparison" << endl;
  struct timeb t, tf;
  ftime(&t);
  int j=0;
  for (i=0; i<5; i++) {
    j++;
    usleep(200000);
  }
  ftime(&tf);
  float et = (float) ((((double)tf.time) + ((double)tf.millitm / 1e3f)) 
    - (((double)t.time) + ((double)t.millitm / 1e3f))) / j;
  cout << "ftime avg time: " << et << " sec, (avg of " << j << " periods" << endl;
#endif 

  /////////////////////////////////////////////////////////////////////////

  cout << "--Timing 5 periods of busywait counter to 10,000,000" << endl;
  glob.Reset();
  for (i=0; i<5; i++) {
    glob.Start();
    for (int j=0; j<10000000; j++) ;
  }  
  glob.Stop();
  cout << glob << endl;

  delete newed;
}

void wall_period_test()
{
  cout << "\n////// Testing Wall Clock Min Period & Overhead /////////" << endl;
  Stopwatch s;
  Stopwatch t;
  s.SetName("WallClockPeriod");
  t.SetName("WallClockTotal");
  t.Start();
  for (int i=0; i<100; i++)
  {
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
    s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();s.Start();s.Stop();
  }
  t.Stop();
  cout << s << endl;
  cout << "Estimated overhead of wall clock Start and Stop: " 
       << (t.GetTime()-s.GetTime())/s.GetNumStarts() << " sec" << endl;
}

int main(int argc, char *argv[])
{
  Stopwatch wloc;
  CPUStopwatch cloc;

  cout << "////// Clock Types for this platform /////////" << endl;
  cout << "CPU Timer: " << cloc.GetType() << endl;
  cout << "Wall Clock: " << wloc.GetType() << endl;


  cout << "\n////// Testing Wall Clock Stopwatch Class /////////" << endl;
  Stopwatch *wnewed = new Stopwatch();
  clock_tests(wglob, wloc, wnewed);

  cout << "\n////// Testing CPU Clock Stopwatch Class /////////" << endl;
  CPUStopwatch *cnewed = new CPUStopwatch();
  clock_tests(cglob, cloc, cnewed);

  wall_period_test();

  return 0;
}
	
