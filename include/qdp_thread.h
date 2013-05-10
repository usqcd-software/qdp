#ifndef _QDP_THREAD_H
#define _QDP_THREAD_H

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_PTHREADS
  //#if !defined(_POSIX_THREADS) || (_POSIX_THREADS <= 0)
  // dummy implementation

  typedef struct { // one for each thread
    void *reduce;
  } QDP_Thread;

  typedef struct {
    int num;
    int group_num;
    int group_size;
  } QDP_ThreadInfo;
#define QDP_thread_num_info(i) (0)
#define QDP_thread_group_num_info(i) (0)
#define QDP_thread_group_size_info(i) (1)
#define QDP_thread_num() (0)
#define QDP_thread_group_num() (0)
#define QDP_thread_group_size() (1)

  typedef struct {
    int dummy;
  } QDP_ThreadBarrier;

  typedef struct {
    int inited, locked;
  } QDP_ThreadLock;


#else  // HAVE_PTHREADS

#include <pthread.h>

  // need to test for __USE_XOPEN2K to workaround bug in gcc
  //#if defined(_POSIX_BARRIERS) && (_POSIX_BARRIERS > 0) && defined(__USE_XOPEN2K)
#ifdef USE_POSIX_BARRIER
  typedef pthread_barrier_t QDP_ThreadBarrier;
#else
#define NEWBARRIER
#ifndef NEWBARRIER
  typedef struct {
    pthread_mutex_t m;
    pthread_cond_t  c;
    volatile int k,n;
  } QDP_ThreadBarrier;
#else
  typedef struct {
    int n;
    //volatile int count[19];
    //volatile int count[23];
    volatile int *count;
  } QDP_ThreadBarrier;
#endif
#endif

  typedef pthread_mutex_t QDP_ThreadLock;
#define QDP_THREADLOCK_INITIALIZER PTHREAD_MUTEX_INITIALIZER

  typedef struct {
    int num;
    int group_num;
    int group_size;
  } QDP_ThreadInfo;
#define QDP_thread_num_info(i) ((i)->num)
#define QDP_thread_group_num_info(i) ((i)->group_num)
#define QDP_thread_group_size_info(i) ((i)->group_size)
#define QDP_thread_num() QDP_thread_num_info(QDP_thread_info())
#define QDP_thread_group_num() QDP_thread_group_num_info(QDP_thread_info())
#define QDP_thread_group_size() QDP_thread_group_size_info(QDP_thread_info())

  typedef struct QDP_ThreadGroup QDP_ThreadGroup;

  typedef struct QDP_Thread { // one for each thread
    QDP_ThreadInfo info; // must be first
    pthread_t pthread;
    void *reduce;
    struct QDP_Thread *parent;
    QDP_ThreadGroup *group;
  } QDP_Thread;

  struct QDP_ThreadGroup { // one for each group
    void (*func)(void *);
    void *args;
    void *share;
    QDP_ThreadLock mutex;
    QDP_ThreadBarrier barrier;
    int num, size;
    QDP_Thread thread[];
  };
#define QDP_THREAD_SHARE(t) ((t)->group->share)
#define QDP_THREAD_REDUCE(t) ((t)->reduce)
#define QDP_THREAD_REDUCEI(t,i) ((t)->group->thread[i].reduce)

#endif

  extern void QDP_init_threads(void);
  extern int QDP_create_threads(int num, int join, void (*func)(void *), void *args);
  extern QDP_Thread *QDP_thread_get(void);
  extern QDP_ThreadInfo *QDP_thread_info(void);
  extern int QDP_thread_group_split(int num);
  extern int QDP_thread_group_join(void);
  extern void QDP_thread_barrier(void);
  extern void QDP_thread_barrier_thread(QDP_Thread *t);
  extern void QDP_thread_lock(void);
  extern void QDP_thread_unlock(void);
  extern int QDP_threadlock_init(QDP_ThreadLock *lock);
  extern int QDP_threadlock_lock(QDP_ThreadLock *lock);
  extern int QDP_threadlock_trylock(QDP_ThreadLock *lock);
  extern int QDP_threadlock_unlock(QDP_ThreadLock *lock);
  extern int QDP_threadlock_destroy(QDP_ThreadLock *lock);



#if 0
  extern int QDP_thread_global_num(void);
  extern int QDP_thread_global_size(void);
  // thread has: process rank, thread rank, local thread rank, group id
  // total # threads in process, and rank
  // 

  extern int QDP_rank(void); // rank in group
  extern int QDP_size(void); // size of group
  extern int QDP_group_rank(void); // group rank on node
  extern int QDP_group_size(void); // number of groups on node
  extern int QDP_group_split(int subgroup, int nsubgroups);
  extern int QDP_group_join(void);
  extern void QDP_barrier(void);
  extern int QDP_node_rank(void); // rank of node in communicator
  extern int QDP_node_size(void); // number of nodes in group


  extern int QDP_group_num(void); // group id
  extern int QDP_group_size(void); // number of total threads in group
  extern int QDP_group_threads(void); // number of threads in group in this process
  extern int QDP_group_split(int subgroup, int nsubgroups);
  extern int QDP_group_join(void);
  // make default lattice be group specific

  extern int QDP_thread_group_num(void);
  extern int QDP_thread_group_split(int num);
  extern int QDP_thread_group_join(void);
#define QDP_restrict_threads(num, expr) { \
    if(QDP_thread_group_split(num)) {	  \
      expr;				  \
    }					  \
    QDP_thread_group_join();		  \
  }

#endif


#ifdef __cplusplus
}
#endif

#endif
