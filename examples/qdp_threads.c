#include <stdio.h>
//#define QDP_Precision 'F'
//#define QDP_Nc 3
#include "qdp_config.h"
#include <qdp.h>

#define printf0(...) if(QDP_this_node==0) do { QDP_threadlock_lock(&printlock); printf("%*s%i %i: ", QDP_thread_group_num(), "", QDP_thread_group_num(), QDP_thread_num()); printf(__VA_ARGS__); QDP_threadlock_unlock(&printlock); } while(0)
#define printft0 if(QDP_thread_num()==0) printf0

static QDP_ThreadLock printlock;
#define REPEAT 10000
#define NTHREADS 2
#define NX 12
#define NT 12

void
thread_func(void *args)
{
  double t0,t1;
  int *done = (int *)args;
  printf0("thread num: %i  group: %i  size: %i\n", QDP_thread_num(),
	  QDP_thread_group_num(), QDP_thread_group_size());

  printft0("timing barrier...\n");
  fflush(stdout);
  QDP_thread_barrier();
  t0 = QDP_time();
  for(int i=0; i<REPEAT; i++) QDP_thread_barrier();
  t1 = QDP_time();
  printf0("QDP_thread_barrier time = %g us\n", 1e6*(t1-t0)/REPEAT);
  fflush(stdout);
  QDP_thread_barrier();

  volatile QDP_ThreadInfo *info;
  t0 = QDP_time();
  for(int i=0; i<REPEAT; i++) info = QDP_thread_info();
  t1 = QDP_time();
  printf0("QDP_thread_info time = %g us\n", 1e6*(t1-t0)/REPEAT);
  fflush(stdout);
  QDP_thread_barrier();
  if(info==NULL) t1 = 0; // avoid warning for unused info

  printf0("testing split(1)...\n");
  if(QDP_thread_group_split(1)) {
    printf0("split true...\n");
  } else {
    printf0("split false...\n");
  }
  QDP_thread_group_join();
  printf0("done\n");
  fflush(stdout);
  QDP_thread_barrier();

  printf0("testing split(2)...\n");
  if(QDP_thread_group_split(2)) {
    printf0("split true...\n");
  } else {
    printf0("split false...\n");
  }
  QDP_thread_group_join();
  printf0("done\n");
  fflush(stdout);
  QDP_thread_barrier();

  printf0("creating lattice...\n");
  int latdim[4] = { NX, NX, NX, NT };
  QDP_Lattice *lat = QDP_create_lattice(NULL, NULL, 4, latdim);
  printf0("...got lattice %p\n", lat);
  fflush(stdout); QDP_thread_barrier();
  printf0("creating ColorMatrix...\n");
  fflush(stdout);
  QDP_ColorMatrix *cm = QDP_create_M_L(lat);
  QDP_ColorVector *cv = QDP_create_V_L(lat);
  QDP_ColorVector *cv2 = QDP_create_V_L(lat);
  printf0("...done creating ColorMatrix %p\n", cm);
  fflush(stdout);
  QDP_thread_barrier();
  QDP_M_eq_zero(cm, QDP_all_L(lat));
  QDP_V_eq_zero(cv, QDP_all_L(lat));
  QDP_V_eq_zero(cv2, QDP_all_L(lat));
  QDP_V_peq_M_times_V(cv2, cm, cv, QDP_all_L(lat));
  QDP_V_peq_M_times_V(cv2, cm, cv, QDP_all_L(lat));
  printf0("freeing lattice...\n");
  QDP_destroy_lattice(lat);
  printf0("...done freeing lattice\n");
  fflush(stdout);
  QDP_thread_barrier();

  QDP_thread_lock();
  int mydone = *done + 1;
  printf0("thread %i: done = %i\n", QDP_thread_num(), mydone);
  fflush(stdout);
  *done = mydone;
  QDP_thread_unlock();
}

int
main(int argc, char *argv[])
{
  volatile int done = 0;

  printf("sizeof(QDP_ThreadBarrier) = %i\n", (int)sizeof(QDP_ThreadBarrier));

  QDP_initialize(&argc, &argv);
  QDP_threadlock_init(&printlock);
  printf0("done QDP initialize\n");
  fflush(stdout);
  //QDP_profcontrol(0);
  QDP_check_comm(0);
  QDP_set_block_size(0);

  double t1, t0 = QDP_time();
  for(int i=0; i<REPEAT; i++) t1 = QDP_time();
  printf0("QDP_time time = %g us\n", 1e6*(t1-t0)/REPEAT);

  printf0("thread group size = %i\n", QDP_thread_group_size());
  printf0("testing barrier... ");
  fflush(stdout);
  QDP_thread_barrier();
  printf0("done\n");

  printf0("testing multiple barriers...\n");
  fflush(stdout);
  QDP_thread_barrier();
  QDP_thread_barrier();
  QDP_thread_barrier();
  QDP_thread_barrier();
  QDP_thread_barrier();
  printf0("done barriers\n");

  printf0("testing split(1)... ");
  if(QDP_thread_group_split(1)) {
    printf0("split true... ");
  } else {
    printf0("split false... ");
  }
  QDP_thread_group_join();
  printf0("done\n");
  fflush(stdout);

  printf0("creating lattice...\n");
  int latdim[4] = { NX, NX, NX, NT };
  QDP_Lattice *lat = QDP_create_lattice(NULL, NULL, 4, latdim);
  printf0("...got lattice %p\n", lat);
  fflush(stdout); QDP_thread_barrier();
  QDP_ColorMatrix *cm = QDP_create_M_L(lat);
  QDP_ColorVector *cv = QDP_create_V_L(lat);
  QDP_ColorVector *cv2 = QDP_create_V_L(lat);
  QDP_M_eq_zero(cm, QDP_all_L(lat));
  QDP_V_eq_zero(cv, QDP_all_L(lat));
  QDP_V_eq_zero(cv2, QDP_all_L(lat));
  QDP_V_peq_M_times_V(cv2, cm, cv, QDP_all_L(lat));
  QDP_V_peq_M_times_V(cv2, cm, cv, QDP_all_L(lat));

  printf0("creating 2 threads join=1... \n");
  fflush(stdout);
  done = 0;
  QDP_create_threads(NTHREADS, 1, thread_func, (void*)&done);
  printf0("done = %i\n", done);
  printf0("done 2 threads join=1\n");

  printf0("my thread num = %i\n", QDP_thread_num());
  printf0("my thread group num = %i\n", QDP_thread_group_num());
  printf0("my thread group size = %i\n", QDP_thread_group_size());
  printf0("testing barrier... ");
  fflush(stdout);
  QDP_thread_barrier();
  printf0("done\n");

  printf0("creating 2 threads join=0... \n");
  fflush(stdout);
  done = 0;
  QDP_create_threads(NTHREADS, 0, thread_func, (void*)&done);
  printf0("done = %i\n", done);
  printf0("done 2 threads join=0\n");

  printf0("my thread num = %i\n", QDP_thread_num());
  printf0("my thread group num = %i\n", QDP_thread_group_num());
  printf0("my thread group size = %i\n", QDP_thread_group_size());
  printf0("testing barrier...\n");
  fflush(stdout);
  QDP_thread_barrier();
  printf0("done barrier\n");

  printf0("main thread waiting...\n");
  fflush(stdout);
  while(done<NTHREADS);
  printf0("done waiting\n");
  printf0("done = %i\n", done);

  printf0("Finalizing QDP...\n");
  fflush(stdout);
  QDP_threadlock_destroy(&printlock);
  QDP_finalize();
  return 0;
}
