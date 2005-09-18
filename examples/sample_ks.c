#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>
#include <time.h>
#include <qdp.h>
#include <qmp.h>
#include "congrad_ks.h"

//int lattice_size[4] = { 32,32,32,32 };
//int lattice_size[4] = { 30,30,30,30 };
int lattice_size[4] = { 16,16,16,16 };
//int lattice_size[4] = { 14,14,14,14 };
//int lattice_size[4] = { 10,10,10,10 };
//int lattice_size[4] = { 14,10,6,6 };
//int lattice_size[4] = { 5,5,10,10 };
//int lattice_size[4] = { 8,8,8,8 };
//int lattice_size[4] = { 6,6,6,6 };
//int lattice_size[4] = { 2,4,6,10 };
//int lattice_size[4] = { 4,4,4,8 };
//int lattice_size[4] = { 4,4,4,4 };
//int lattice_size[4] = { 2,4,4,4 };
int dir;

void
initial_li(QLA_Int *li, int coords[])
{
  int i,t;

  t = coords[0];
  for(i=1; i<4; i++) {
    t = t*lattice_size[i] + coords[i];
  }
  *li = t;
}

void
print_V(QLA_ColorVector *s, int coords[])
{
  QLA_Complex z;
  int i;

  for(i=0; i<4; i++) {
    if(QDP_this_node==0) printf("%-3i", coords[i]);
  }
  for(i=0; i<QLA_Nc; i++) {
    z = QLA_elem_V(*s,i);
    if(QDP_this_node==0) printf("% -11.8f% -11.8f", QLA_real(z), QLA_imag(z));
  }
  if(QDP_this_node==0) printf("\n");
}

/* used to initialize gauge field */
void
unit_M(QLA_ColorMatrix *g, int coords[])
{
  int i, j;

  for(i=0; i<QDP_Nc; i++) {
    for(j=0; j<QDP_Nc; j++) {
      if(i==j) {
	QLA_c_eq_r(QLA_elem_M(*g,i,j), 1);
      } else {
	QLA_c_eq_r(QLA_elem_M(*g,i,j), 0);
      }
    }
  }
}

/* used to initialize gauge field */
void
pm_unit_M(QLA_ColorMatrix *g, int coords[])
{
  int i, j, k;

  for(i=1; i<QDP_Nc; i++) {
    for(j=0; j<i; j++) {
      QLA_c_eq_r(QLA_elem_M(*g,i,j), 0);
      QLA_c_eq_r(QLA_elem_M(*g,j,i), 0);
    }
  }

  k = 0;
  for(i=0; i<dir; ++i) k += coords[i];
  if((k&1)==0) {
    for(i=0; i<QDP_Nc; i++) {
      QLA_c_eq_r(QLA_elem_M(*g,i,i), 1);
    }
  } else {
    for(i=0; i<QDP_Nc; i++) {
      QLA_c_eq_r(QLA_elem_M(*g,i,i), -1);
    }
  }
}

/* used to initialize source field */
void
point_V(QLA_ColorVector *s, int coords[])
{
  int i;

  for(i=0; i<QDP_Nc; i++) {
    if((coords[0]==0)&&(coords[1]==0)&&(coords[2]==0)&&(coords[3]==0)) {
      QLA_c_eq_r(QLA_elem_V(*s,i), 1);
    } else {
      QLA_c_eq_r(QLA_elem_V(*s,i), 0);
    }
  }
}

int
main(int argc, char *argv[])
{
  QDP_ColorMatrix *gauge[4];
  QDP_ColorVector *source, *result;
  QDP_RandomState *rs;
  QDP_Int *li;
  QLA_Real mass, t;
  int i, count=1;
  struct rusage ru;

  QDP_initialize(&argc, &argv);
  QDP_check_comm(1);
  if(argc>1) {
    int s=atoi(argv[1]);
    for(i=0; i<4; i++) lattice_size[i] = s;
  }
  QDP_set_latsize(4, lattice_size);
  QDP_create_layout();
  if(QDP_this_node==0) {
    printf("number of nodes = %i\n", QMP_get_number_of_nodes());
    printf("lattice_size =");
    for(i=0; i<4; i++) printf(" %i", lattice_size[i]);
    printf("\n");
    printf("volume = %i\n", QDP_volume());
    printf("subvolume = %i\n", QDP_sites_on_node);
    if(QMP_logical_topology_is_declared()) {
      int i, ld, *lt;
      ld = QMP_get_logical_number_of_dimensions();
      lt = QMP_get_logical_dimensions();
      printf("logical topology =");
      for(i=0; i<ld; i++) printf(" %i", lt[i]);
      printf("\n");
      printf("sublattice =");
      for(i=0; i<ld; i++) printf(" %i", lattice_size[i]/lt[i]);
      printf("\n");
    }
  }

  li = QDP_create_I();
  rs = QDP_create_S();

  QDP_I_eq_func(li, initial_li, QDP_all);
  QDP_S_eq_seed_i_I(rs, 987654321, li, QDP_all);

  for(i=0; i<4; ++i) {
    gauge[i] = QDP_create_M();
    //if(i==0) {
      dir = i;
      QDP_M_eq_func(gauge[i], pm_unit_M, QDP_all);
    //} else {
     // QDP_M_eq_func(gauge[i], unit_M, QDP_all);
    //}
    //QDP_M_eq_zero(gauge[i], QDP_all);
  }

  source = QDP_create_V();
  QDP_V_eq_func(source, point_V, QDP_all);
  //QDP_V_eq_func(source, print_V, QDP_all);

  result = QDP_create_V();

  mass = 0.1;

  do {
    //fprintf(stderr,"count=%i\n",count);
    QDP_V_eq_gaussian_S(result, rs, QDP_all);
    //QDP_V_eq_func(result, point_V, QDP_all);
    //t = 1/(4*mass*mass);
    //QDP_V_eq_r_times_V(result, &t, source, QDP_all);
    QDP_r_eq_norm2_V(&t, result, QDP_all);
    if(QDP_this_node==0) printf("initial norm = %g\n", t);
    i = congrad(result, gauge, source, mass, 100, 0.00001);
  } while(--count);
  if(QDP_this_node==0) printf("conjugate gradient steps: %i\n", i);

  if(QDP_this_node==0) {
    getrusage(RUSAGE_SELF,&ru);
    printf("cpu time: %.2f seconds\n", (ru.ru_utime.tv_sec+ru.ru_stime.tv_sec)
	   +0.000001*(ru.ru_utime.tv_usec+ru.ru_stime.tv_usec));
  }

  //QDP_V_eq_func(source, print_V, QDP_all);
  //if(QDP_this_node==0) printf("\n");
  //QDP_V_eq_func(result, print_V, QDP_all);

  for(i=0; i<4; ++i) {
    QDP_destroy_M(gauge[i]);
  }
  QDP_destroy_V(source);
  QDP_destroy_V(result);

  QDP_finalize();
  /* we never actually get here since QDP_finalize exits */
  return 0;
}
