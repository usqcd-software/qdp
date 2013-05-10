#include <stdio.h>
#include "qdp_config.h"
#include <qdp.h>

#define printf0 if(QDP_this_node==0) printf
#define TRACE printf("%i: %s %s %i\n", QDP_this_node, __FILE__, __func__, __LINE__)
//#define TRACE ((void)0)
#define TRACE2 printf("%i: %s %s %i\n", QDP_this_node, __FILE__, __func__, __LINE__)

static int
timeslice_func(int *coord, void *dum)
{
  return coord[*((int *)dum)];
}

void
test(QDP_DiracFermion *src, QDP_ColorMatrix *gauge[], int n)
{
  QLA_Real s0, s1;
  QDP_DiracFermion *t, *tt[3], *ts[3], *tf[3], *tb1[3], *tb2[3];
  QDP_ShiftDir fwd[3], bck[3];
  t = QDP_create_D();
  for(int i=0; i<3; i++) {
    tt[i] = t;
    ts[i] = src;
    tf[i] = QDP_create_D();
    tb1[i] = QDP_create_D();
    tb2[i] = QDP_create_D();
    fwd[i] = QDP_forward;
    bck[i] = QDP_backward;
  }

  static QDP_Subset *tslice=NULL;
  int tdir = 3;
  int nt = QDP_coord_size(tdir);
  if(tslice==NULL)
    tslice = QDP_create_subset(timeslice_func, &tdir, sizeof(tdir), nt);

  double r = 0.2;
  s0 = 1.0/(1.0+6*r);
  s1 = s0 * r;
  for(int i=0; i<n; i++) {
    for(int j=0; j<nt; j++) {
      printf("%i:  i: %i  j: %i\n",QDP_this_node,i,j);
      QDP_D_eq_r_times_D(t, &s1, src, tslice[j]);
      TRACE;
      QDP_D_veq_sD(tf, tt, QDP_neighbor, fwd, tslice[j], 3);
      TRACE;
      QDP_D_veq_Ma_times_D(tb1, gauge, tt, tslice[j], 3);
      TRACE;
      QDP_D_eq_D(tb2[0], tb1[0], QDP_all);
      TRACE;
      QDP_D_veq_sD(tb2, tb1, QDP_neighbor, bck, tslice[j], 3);
      TRACE;
      QDP_D_eq_r_times_D(src, &s0, src, tslice[j]);
      TRACE;
      QDP_D_vpeq_M_times_D(ts, gauge, tf, tslice[j], 3);
      TRACE;
      QDP_D_vpeq_D(ts, tb2, tslice[j], 3);
      TRACE;
      for(int k=0; k<3; k++) {
	QDP_discard_D(tf[k]);
	QDP_discard_D(tb2[k]);
      }
    }
  }

  TRACE2;
  QDP_destroy_D(t);
  for(int i=0; i<3; i++) {
    QDP_destroy_D(tf[i]);
    QDP_destroy_D(tb1[i]);
    QDP_destroy_D(tb2[i]);
  }
  TRACE2;
}

int
main(int argc, char **argv)
{
  int lattice_size[4] = { 8,8,8,8 };

  QDP_initialize(&argc, &argv);

  printf0("Setting lattice size... ");
  fflush(stdout);
  QDP_set_latsize(4, lattice_size);
  printf0("done\n");

  printf0("Creating layout... ");
  fflush(stdout);
  QDP_create_layout();
  printf0("done\n");
  QDP_check_comm(1);

  QDP_ColorMatrix *gauge[4];
  for(int i=0; i<4; i++) {
    gauge[i] = QDP_create_M();
    QDP_M_eq_zero(gauge[i], QDP_all);
  }
  QDP_DiracFermion *src;
  src = QDP_create_D();
  QDP_D_eq_zero(src, QDP_all);

  test(src, gauge, 2);

  printf("%i Finished tests, now closing QDP...\n", QDP_this_node);
  fflush(stdout);
  QMP_barrier();
  QDP_finalize();
  return 0;
}
