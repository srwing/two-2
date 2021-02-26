#include "thread.hpp"

#include <queue>
using namespace std;

//----------------------------------------------------------------------------
struct TData {
  int id;
  unsigned long sleep;
};

void threadfunc(void *arg)
{
  TData *td = (TData*)arg;
  Thread *t = Thread::getCurrent();
  if (td->id==2) printf("\t\t\t\t");
  printf("Current Thread is 0x%x\n", t);
  for (int i=1; i<=10; i++) {
    if (td->id==2) printf("\t\t\t\t");
    printf("Thread %d, pid %d,  count %d\n", td->id, t->pid(), i);
    Thread::msleep(td->sleep);
  }
}

void test1()
{
  printf("-------------------------------------------------\n");
  printf("Test1: Testing basic threading.  Two threads that count to 10\n"
     "   will be spawned, and the main process will wait for them to\n"
     "   finish.  Thread 1 counts faster than Thread 2, so thread 2 should\n"
     "   still have a few left to go after 1 exits.\n");
  Thread *th1 = new Thread(threadfunc);
  Thread *th2 = new Thread(threadfunc);
  printf("New Thread 1, addr 0x%x\n", th1);
  printf("\t\t\t\tNew Thread 2, addr 0x%x\n", th2);
  TData td1 = { 1,  30 };
  TData td2 = { 2,  60 };
  th1->go((void*)&td1);
  th2->go((void*)&td2);
  printf("--- waiting on 1 ---\n");
  th1->join();
  printf("--- joined 1 ---\n");
  printf("--- waiting on 2 ---\n");
  th2->join();
  printf("--- joined 2 ---\n");
  delete th1;
  delete th2;
  printf("\n");
}
//----------------------------------------------------------------------------
class MyThread : public Thread
{
public:
  MyThread(int _id, unsigned long _sleep)
    : Thread() {
    id = _id; sleep = _sleep;
  }
private:
  void run(void) {
    for (int i=1; i<=10; i++) {
      if (id==2) printf("\t\t\t\t");
      printf("Thread %d, pid %d,  count %d\n", id, pid(), i);
      Thread::msleep(sleep);
    }
  }
  int id;
  unsigned long sleep;
};

void test2()
{
  printf("-------------------------------------------------\n");
  printf("Test 2: Basically the same test as before, but using subclassed\n"
         "        Threads to hold our user data.\n");
  MyThread *th1 = new MyThread(1, 30);
  MyThread *th2 = new MyThread(2, 60);
  th1->go();
  th2->go();
  printf("--- waiting on 1 ---\n");
  th1->join();
  printf("--- joined 1 ---\n");
  printf("--- waiting on 2 ---\n");
  th2->join();
  printf("--- joined 2 ---\n");
  delete th1;
  delete th2;
  printf("\n");
}

//----------------------------------------------------------------------------
class Worker
{
public:
  Worker(Semaphore &fulls, Semaphore &empties, 
         Semaphore &mutex, queue<int> &q)
    : m_full(fulls), m_empty(empties), m_mutex(mutex), m_q(q)
  {
  }
protected:
  Semaphore &m_full;
  Semaphore &m_empty;
  Semaphore &m_mutex;
  queue<int> &m_q;
};

class Producer : public Worker
{
public:
  Producer(Semaphore &f, Semaphore &e, 
           Semaphore &m, queue<int> &q, unsigned long _delay)
    : Worker(f,e,m,q)
  {
    delay = _delay;
  }
  void run() 
  {
    for (int i=1; i<=10; i++) {
      Thread::msleep(delay);
      m_empty.down();
      m_mutex.down();
      m_q.push(i);
      printf("Put #%d (now %d in queue)\n", i, m_q.size());
      m_mutex.up();
      m_full.up();
    }
  }
  static void RunFunc(void *arg) {
    Producer *p = (Producer *)arg;
    p->run();
  }
private:
  unsigned long delay;
};
class Consumer : public Worker
{
public:
  Consumer(Semaphore &f, Semaphore &e, 
           Semaphore &m, queue<int> &q, unsigned long _delay)
    : Worker(f,e,m,q)
  {
    delay = _delay;
  }
  void run()
  {
    int i = 0;
    while (i!=10) {
      m_full.down();
      m_mutex.down();
      i = m_q.front();
      m_q.pop();
      printf("\t\t\tGot #%d (now %d in queue)\n", i, m_q.size());
      m_mutex.up();
      m_empty.up();
      Thread::msleep(delay);
    }
  }
  static void RunFunc(void *arg) {
    Consumer *c = (Consumer *)arg;
    c->run();
  }
private:
  unsigned long delay;
};


void test3()
{
  printf("-------------------------------------------------\n");
  printf("Test 3: Classic producer consumer with semaphores.\n"
         "        First with slow producer, fast consumer. \n");
  Semaphore *s_full = new Semaphore(0);
  Semaphore *s_empty = new Semaphore(5);
  Semaphore *s_mutex = new Semaphore();
  Thread *p_thr = new Thread(Producer::RunFunc);
  Thread *c_thr = new Thread(Consumer::RunFunc);
  queue<int> the_queue;
  Producer prod(*s_full, *s_empty, *s_mutex, the_queue, 40);
  Consumer cons(*s_full, *s_empty, *s_mutex, the_queue, 0);
  c_thr->go((void*)&cons);
  p_thr->go((void*)&prod);
  c_thr->join();
  p_thr->join();
  delete p_thr;
  delete c_thr;
  delete s_full;
  delete s_empty;
  delete s_mutex;
  printf("\n");
}

void test4()
{
  printf("-------------------------------------------------\n");
  printf("Test 4: Fast producer, slow consumer. \n"
         "        The number of items in the queue shouldn't exceed 5\n");
  Semaphore *s_full = new Semaphore(0);
  Semaphore *s_empty = new Semaphore(5);
  Semaphore *s_mutex = new Semaphore();
  Thread *p_thr = new Thread(Producer::RunFunc);
  Thread *c_thr = new Thread(Consumer::RunFunc);
  queue<int> the_queue;
  Producer prod(*s_full, *s_empty, *s_mutex, the_queue, 0);
  Consumer cons(*s_full, *s_empty, *s_mutex, the_queue, 80);
  c_thr->go((void*)&cons);
  p_thr->go((void*)&prod);
  c_thr->join();
  p_thr->join();
  delete p_thr;
  delete c_thr;
  delete s_full;
  delete s_empty;
  delete s_mutex;
  printf("\n");
}

int main(int argc, char *argv[])
{
  test1();
  test2();
  test3();
  test4();

  return 0;
}
