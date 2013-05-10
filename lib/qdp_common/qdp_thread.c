//#define DO_TRACE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "qdp_internal.h"

#ifndef HAVE_PTHREADS
//#if !defined(_POSIX_THREADS) || (_POSIX_THREADS <= 0)
// dummy implementation

#define THREADLOCKINITED 0x12345678

static int splitcount=0;

int
QDP_create_threads(int num, int join, void (*func)(void *), void *args)
{
  if(join) {
    QDP_assert(num==1);
    func(args);
  } else {
    QDP_assert(num==0);
  }
  return 0;
}

QDP_ThreadInfo *
QDP_thread_info(void)
{
  return NULL;
}

void
QDP_init_threads(void)
{
}

#if 0
int
QDP_thread_index(void)
{
  return 0;
}

int
QDP_thread_num(void)
{
  return 0;
}

int
QDP_thread_group_num(void)
{
  return 0;
}

int
QDP_thread_group_size(void)
{
  return 1;
}
#endif

int
QDP_thread_group_split(int num)
{
  QDP_assert(num==1);
  splitcount++;
  return 1;
}

int
QDP_thread_group_join(void)
{
  splitcount--;
  if(splitcount<0) {
    splitcount = 0;
    return 1;
  }
  return 0;
}

void
QDP_thread_barrier(void)
{
}

int
QDP_threadlock_init(QDP_ThreadLock *lock)
{
  lock->inited = THREADLOCKINITED;
  lock->locked = 0;
  return 0;
}

int
QDP_threadlock_lock(QDP_ThreadLock *lock)
{
  QDP_assert(lock->inited==THREADLOCKINITED);
  QDP_assert(lock->locked==0);
  lock->locked = 1;
  return 0;
}

int
QDP_threadlock_trylock(QDP_ThreadLock *lock)
{
  QDP_assert(lock->inited==THREADLOCKINITED);
  QDP_assert(lock->locked==0);
  lock->locked = 1;
  return 0;
}

int
QDP_threadlock_unlock(QDP_ThreadLock *lock)
{
  QDP_assert(lock->inited==THREADLOCKINITED);
  QDP_assert(lock->locked==1);
  lock->locked = 0;
  return 0;
}

int
QDP_threadlock_destroy(QDP_ThreadLock *lock)
{
  QDP_assert(lock->inited==THREADLOCKINITED);
  QDP_assert(lock->locked==0);
  lock->inited = 0;
  return 0;
}

void
QDP_thread_lock(void)
{
}

void
QDP_thread_unlock(void)
{
}

#else // have pthreads

#include <pthread.h>

  // need to test for __USE_XOPEN2K to workaround bug in gcc
//#if defined(_POSIX_BARRIERS) && (_POSIX_BARRIERS > 0) && defined(__USE_XOPEN2K)
#ifdef USE_POSIX_BARRIER

#define barrier_init(x,n)  pthread_barrier_init(x,NULL,n)
#define barrier_destroy(x) pthread_barrier_destroy(x)
#define barrier_wait(x)    pthread_barrier_wait(x)

#else

#ifndef NEWBARRIER
static int
barrier_init(QDP_ThreadBarrier *b, int n)
{
  pthread_mutex_init(&b->m, NULL);
  pthread_cond_init(&b->c, NULL);
  b->k = n;
  b->n = n;
  return 0;
}

static int
barrier_destroy(QDP_ThreadBarrier *b)
{
  pthread_mutex_destroy(&b->m);
  pthread_cond_destroy(&b->c);
  return 0;
}

static int
barrier_wait(QDP_ThreadBarrier *b)
{
  int rc = pthread_mutex_lock(&b->m);
  if(rc) return rc;
#if 0
  if(--b->k>0) {
    rc = pthread_cond_wait(&b->c, &b->m);
  } else {
    b->k = b->n;
    rc = pthread_cond_broadcast(&b->c);
  }
  if(rc) {
    pthread_mutex_unlock(&b->m); 
    return rc;
  }
  rc = pthread_mutex_unlock(&b->m); 
#else
  int v = b->k++;
  rc = pthread_mutex_unlock(&b->m);
  int d = ((v/b->n)+1)*b->n;
  while(b->k<d);
#endif
  return rc;
}
#else
static int
barrier_init(QDP_ThreadBarrier *b, int n)
{
  b->n = n;
  b->count = (int *)malloc(n*sizeof(int));
  //for(int i=0; i<n; i++) b->count[i] = 0;
  for(int i=0; i<n; i++) b->count[i] = 3;
  return 0;
}

static int
barrier_destroy(QDP_ThreadBarrier *b)
{
  free(b->count);
  return 0;
}

static void
barrier_wait2(QDP_ThreadBarrier *b, int tid)
{
  int v = b->count[tid];
  int v2 = v<<1;
  v2 = (v2&7) + (v2>>3);
  int vb = v ^ 7;
#if 0
  if(tid==0) {
    for(int i=1; i<b->n; i++) while((b->count[i]&vb)==0);
    b->count[tid] = v2;
  } else {
    b->count[tid] = v2;
    while((b->count[0]&vb)==0);
  }
#else
  b->count[tid] = v2;
  for(int i=0; i<b->n; i++) while((b->count[i]&vb)==0);
#endif
}

static int
barrier_wait(QDP_ThreadBarrier *b)
{
  int tid = QDP_thread_num();
  int v = b->count[tid];
  for(int i=0; i<b->n; i++) while(b->count[i]<v);
  b->count[tid] = ++v;
  for(int i=0; i<b->n; i++) while(b->count[i]<v);
  return 0;
}
#endif

#endif

static QDP_ThreadLock grouplock;
static QDP_ThreadGroup **grouparray = NULL;
static int grouplen = 0;

static pthread_key_t key_thread;
#define getkey() ((QDP_Thread *)pthread_getspecific(key_thread))
#define setkey(t) pthread_setspecific(key_thread, (void *)(t))
#define getgroup() (getkey()->group)

//static int threads_inited=0;

static QDP_ThreadGroup *
create_group1(int num)
{
  int k=0;
  QDP_threadlock_lock(&grouplock);
  while(k<grouplen && grouparray[k]!=NULL) k++;
  if(k==grouplen) {
    grouplen = k + 1;
    grouparray = realloc(grouparray, grouplen*sizeof(*grouparray));
  }
  QDP_ThreadGroup *g = malloc(sizeof(**grouparray)+num*sizeof(QDP_Thread));
  grouparray[k] = g;
  QDP_threadlock_unlock(&grouplock);

  g->func = NULL;
  g->args = NULL;
  g->num = k;
  g->size = num;
  g->share = NULL;
  pthread_mutex_init(&g->mutex, NULL);
  barrier_init(&g->barrier, num);

  return g;
}

static QDP_ThreadGroup *
create_group(int num)
{
  QDP_Thread *t = getkey();
  int tid = t->info.num;
  QDP_ThreadGroup *g1 = NULL;
  if(tid==0) {
    g1 = create_group1(num);
    t->group->share = (void *) g1;
    QDP_thread_barrier();
  } else {
    QDP_thread_barrier();
    g1 = (QDP_ThreadGroup *) t->group->share;
  }
  QDP_thread_barrier();
  return g1;
}

static void
set_thread(QDP_ThreadGroup *g, int tid, QDP_Thread *parent)
{
  QDP_Thread *t = &g->thread[tid];
  t->info.num = tid;
  t->info.group_num = g->num;
  t->info.group_size = g->size;
  t->parent = parent;
  t->group = g;
}

static int
end_thread(void)
{
  QDP_Thread *t = getkey();
  QDP_Thread *parent = t->parent;
  int tid = t->info.num;
  QDP_thread_barrier();
  if(tid==0) {
    QDP_ThreadGroup *g = t->group;
    pthread_mutex_destroy(&g->mutex);
    barrier_destroy(&g->barrier);
    QDP_threadlock_lock(&grouplock);
    grouparray[g->num] = NULL;
    QDP_threadlock_unlock(&grouplock);
    //free(g);  // FIXME memory leak
  }
  if(parent!=NULL) {
    int rc = setkey(parent);
    QDP_assert(rc==0);
  }
  return 0;
}

static void *
start_thread(void *arg)
{
  QDP_Thread *t = (QDP_Thread *) arg;
  int rc = setkey(t);
  QDP_assert(rc==0);
  QDP_thread_barrier();
  void (*func)(void *) = t->group->func;
  if(func!=NULL) {
    func(t->group->args);
    end_thread();
  }
  return NULL;
}

// need to have this called from QDP_init...
void
QDP_init_threads(void)
{
  TRACE;
  int rc = pthread_key_create(&key_thread, NULL);
  QDP_assert(rc==0);
  TRACE;
  QDP_threadlock_init(&grouplock);
  TRACE;
  QDP_ThreadGroup *g = create_group1(1);
  TRACE;
  set_thread(g, 0, NULL);
  TRACE;
  start_thread((void *)&g->thread[0]);
  TRACE;
}

int
QDP_create_threads(int num, int join, void (*func)(void *), void *args)
{
  QDP_Thread *t = getkey();
  QDP_assert(!join || t->info.group_size<=num);
  QDP_ThreadGroup *g = create_group(num);
  int tid = t->info.num;
  if(tid==0) {
    g->func = func;
    g->args = args;
    QDP_thread_barrier();
    int start = 0;
    if(join) start = t->info.group_size;
    for(int i=start; i<num; i++) {
      QDP_Thread *tnew = &g->thread[i];
      set_thread(g, i, t);
      int rc = pthread_create(&tnew->pthread, NULL, start_thread, (void *)tnew);
      QDP_assert(rc==0);
    }
  } else {
    QDP_thread_barrier();
  }
  if(join) {
    QDP_Thread *tnew = &g->thread[tid];
    set_thread(g, tid, t);
    tnew->pthread = t->pthread;
    start_thread((void *)tnew);
    if(tid==0) {
      int start = 0;
      if(join) start = t->info.group_size;
      for(int i=start; i<num; i++) {
	pthread_join(g->thread[i].pthread, NULL);
      }
    }
  }
  return 0;
}

QDP_ThreadInfo *
QDP_thread_info(void)
{
  return &getkey()->info;
}

QDP_Thread *
QDP_thread_get(void)
{
  return getkey();
}

// returns 1 if in the num sized group, 0 if in the other group
int
QDP_thread_group_split(int num)
{
  // should really have the first num threads to reach this be in the group
  QDP_assert(num>0);
  QDP_Thread *t = getkey();
  int size = t->info.group_size;
  QDP_assert(size>=num);
  QDP_ThreadGroup *g0, *g1=NULL;
  g0 = create_group(num);
  if(size>num) g1 = create_group(size-num);
  int tid = t->info.num;
  if(tid<num) {
    QDP_Thread *tnew = &g0->thread[tid];
    set_thread(g0, tid, t);
    tnew->pthread = t->pthread;
    start_thread((void *)tnew);
    return 1;
  } else {
    int newtid = tid - num;
    QDP_Thread *tnew = &g1->thread[newtid];
    set_thread(g1, newtid, t);
    tnew->pthread = t->pthread;
    start_thread((void *)tnew);
    return 0;
  }
  return -1; // not reached
}

// return 0 on success
int
QDP_thread_group_join(void)
{
  return end_thread();
}

void
QDP_thread_barrier_thread(QDP_Thread *t)
{
  //barrier_wait(&t->group->barrier);
  barrier_wait2(&t->group->barrier, t->info.num);
}

void
QDP_thread_barrier(void)
{
  QDP_Thread *t = getkey();
  //barrier_wait(&t->group->barrier);
  barrier_wait2(&t->group->barrier, t->info.num);
}

void
QDP_thread_lock(void)
{
  QDP_Thread *t = getkey();
  pthread_mutex_lock(&t->group->mutex);
}

void
QDP_thread_unlock(void)
{
  QDP_Thread *t = getkey();
  pthread_mutex_unlock(&t->group->mutex);
}

int
QDP_threadlock_init(QDP_ThreadLock *lock)
{
  return pthread_mutex_init(lock, NULL);
}

int
QDP_threadlock_lock(QDP_ThreadLock *lock)
{
  return pthread_mutex_lock(lock);
}

int
QDP_threadlock_trylock(QDP_ThreadLock *lock)
{
  return pthread_mutex_trylock(lock);
}

int
QDP_threadlock_unlock(QDP_ThreadLock *lock)
{
  return pthread_mutex_unlock(lock);
}

int
QDP_threadlock_destroy(QDP_ThreadLock *lock)
{
  return pthread_mutex_destroy(lock);
}

#endif // pthreads
