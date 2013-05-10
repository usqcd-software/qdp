#include <stdio.h>
//#define QDP_Precision 'F'
//#define QDP_Nc 3
#include "qdp_config.h"
#include <qdp.h>

#define printf0(...) if(QDP_this_node==0) do { QDP_threadlock_lock(&printlock); printf("%*s%i %i: ", QDP_thread_group_num(), "", QDP_thread_group_num(), QDP_thread_num()); printf(__VA_ARGS__); QDP_threadlock_unlock(&printlock); } while(0)
#define printft0 if(QDP_thread_num()==0) printf0

static QDP_ThreadLock printlock;
#define REPEAT 1000

int nthreads = 2;
int nx = 12;
int nt = 12;

QDP_Lattice *lat;
QDP_ColorMatrix *cm;
QDP_ColorVector *cv, *cv2, *cv3;

void
thread_func(void *args)
{
  double t0,t1;
  int repeat = REPEAT*QDP_thread_group_size();
  QDP_thread_barrier();
  QDP_V_peq_M_times_V(cv2, cm, cv, QDP_all_L(lat));
  QDP_thread_barrier();
  t0 = QDP_time();
  for(int i=0; i<repeat; i++) {
    QDP_V_peq_M_times_V(cv2, cm, cv, QDP_all_L(lat));
  }
  t1 = QDP_time();
  int sites = QDP_subset_len(QDP_all_L(lat));
  double mflops = 72e-6*sites*repeat/(t1-t0);
  printf0("QDP_V_peq_M_times_V mflops = %g\n", mflops);
  fflush(stdout);
  QDP_thread_barrier();
}

int
main(int argc, char *argv[])
{
  volatile int done = 0;

  QDP_initialize(&argc, &argv);
  QDP_threadlock_init(&printlock);
  printf0("done QDP initialize\n");
  fflush(stdout);

  if(argc>1) nthreads = atoi(argv[1]);
  if(argc>2) nx = atoi(argv[2]);
  if(argc>3) nt = atoi(argv[3]);

  //QDP_profcontrol(0);
  QDP_check_comm(0);
  QDP_set_block_size(0);

  double t1, t0 = QDP_time();
  for(int i=0; i<REPEAT; i++) t1 = QDP_time();
  printf0("QDP_time time = %g us\n", 1e6*(t1-t0)/REPEAT);

  printf0("creating lattice...\n");
  int latdim[4] = { nx, nx, nx, nt };
  lat = QDP_create_lattice(NULL, NULL, 4, latdim);
  printf0("...got lattice %p\n", lat);
  fflush(stdout); QDP_thread_barrier();
  cm = QDP_create_M_L(lat);
  cv = QDP_create_V_L(lat);
  cv2 = QDP_create_V_L(lat);
  cv3 = QDP_create_V_L(lat);
  QDP_M_eq_zero(cm, QDP_all_L(lat));
  QDP_V_eq_zero(cv2, QDP_all_L(lat));
  QDP_V_eq_zero(cv3, QDP_all_L(lat));
  QDP_V_eq_sV(cv, cv3, QDP_neighbor_L(lat)[0], QDP_forward, QDP_all_L(lat));

  for(int n=1; n<=nthreads; n++) {
    printf0("creating %i threads join=1... \n", n);
    fflush(stdout);
    done = 0;
    QDP_create_threads(n, 1, thread_func, (void*)&done);
  }

  printf0("Finalizing QDP...\n");
  fflush(stdout);
  QDP_threadlock_destroy(&printlock);
  QDP_finalize();
  return 0;
}
