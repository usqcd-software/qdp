#include <stdio.h>
//#define QDP_Precision 'F'
//#define QDP_Nc 3
#include <qdp.h>

#define printf0 if(QDP_this_node==0) printf

void
initial_gauge(QLA_ColorMatrix *g, int coords[])
{
  QLA_Complex z;
  int i;

  QLA_M_eq_zero(g);
  for(i=0; i<QLA_Nc; i++) {
    QLA_c_eq_r_plus_ir(z, i, 0);
    QLA_elem_M(*g,i,i) = z;
  }
}

void
initial_df(QLA_DiracFermion *df, int coords[])
{
  QLA_Complex z;
  int i,j;

  for(i=0; i<QLA_Nc; i++) {
    for(j=0; j<4; j++) {
      QLA_c_eq_r_plus_ir(z, i+j, coords[0]);
      QLA_elem_D(*df,i,j) = z;
    }
  }
}

void
initial_li(QLA_Int *li, int coords[])
{
  int i,t;

  t = coords[3];
  for(i=2; i>=0; --i) {
    t = t*QDP_coord_size(i) + coords[i];
  }
  *li = t;
}

void
print_df(QLA_DiracFermion *df, int coords[])
{
  QLA_Complex z;
  int i,j;

  for(i=0; i<4; i++) if(coords[i]) return;
  for(i=0; i<QLA_Nc; i++) {
    for(j=0; j<4; j++) {
      z = QLA_elem_D(*df,i,j);
      printf0("%g\t%g\n", QLA_real(z), QLA_imag(z));
    }
  }
}

int
timeslices(int x[], void *args)
{
  return x[3];
}

void
run_tests(void)
{
  int i;
  QDP_RandomState *rs;
  QDP_Int *li;
  QDP_Real *r;
  QDP_Complex *c;
  QDP_ColorVector *v;
  QDP_HalfFermion *hf;
  QDP_DiracFermion *d1, *d2, *d3;
  QDP_ColorMatrix *m;
  QDP_DiracPropagator *p;
  QDP_Subset *ts;
  //QDP_Shift shift;
  QLA_Int qlai;
  QLA_Real qlar;
  //QLA_Complex qlac;
  //QLA_ColorVector qlav;
  //QLA_HalfFermion qlah;
  //QLA_DiracFermion qlad;
  //QLA_ColorMatrix qlam;
  //QLA_DiracPropagator qlap;

  printf0("checking create and destroy... ");
  fflush(stdout);
  for(i=0; i<100; ++i) {
    p = QDP_create_P();
    QDP_P_eq_zero(p, QDP_all);
    QDP_destroy_P(p);
  }
  printf0("done\n");

  printf0("creating test fields... ");
  fflush(stdout);
  rs = QDP_create_S();
  li = QDP_create_I();
  r = QDP_create_R();
  c = QDP_create_C();
  v = QDP_create_V();
  hf = QDP_create_H();
  d1 = QDP_create_D();
  d2 = QDP_create_D();
  d3 = QDP_create_D();
  m = QDP_create_M();
  p = QDP_create_P();
  printf0("done\n");

  printf0("setting test fields... ");
  fflush(stdout);
  QDP_I_eq_func(li, initial_li, QDP_all);
  QDP_S_eq_seed_i_I(rs, 987654321, li, QDP_all);
  qlai = 1;
  QDP_I_eq_i(li, &qlai, QDP_all);
  qlar = 1;
  QDP_R_eq_r(r, &qlar, QDP_all);
  printf0("done\n");

  printf0("creating subset... ");
  fflush(stdout);
  ts = QDP_create_subset(timeslices, NULL, 0, QDP_coord_size(3));
  printf0("done\n");

  printf0("Calling QDP_M_eq_gaussian_S... ");
  fflush(stdout);
  QDP_M_eq_gaussian_S(m, rs, QDP_all);
  printf0("done\n");

  printf0("Calling QDP_D_eq_gaussian_S... ");
  fflush(stdout);
  QDP_D_eq_gaussian_S(d1, rs, QDP_all);
  printf0("done\n");

  printf0("Calling QDP_D_eq_sD... ");
  fflush(stdout);
  QDP_D_eq_sD(d2, d1, QDP_neighbor[0], QDP_forward, QDP_all);
  printf0("done\n");

  printf0("Calling QDP_D_eq_M_times_D... ");
  fflush(stdout);
  QDP_D_eq_M_times_D(d3, m, d2, QDP_all);
  printf0("done\n");

  printf0("Calling QDP_D_eq_zero... ");
  fflush(stdout);
  QDP_D_eq_zero(d3, QDP_odd);
  printf0("done\n");

  printf0("Calling QDP_D_eq_zero... ");
  fflush(stdout);
  QDP_D_eq_zero(d3, ts[0]);
  printf0("done\n");

  printf0("Calling QDP_D_eq_zero... ");
  fflush(stdout);
  QDP_D_eq_zero(d2, QDP_odd);
  printf0("done\n");

  printf0("Calling QDP_D_eq_zero... ");
  fflush(stdout);
  QDP_D_eq_zero(d2, QDP_even);
  printf0("done\n");

  printf0("Calling QDP_D_eq_sD... ");
  fflush(stdout);
  QDP_D_eq_sD(d2, d1, QDP_neighbor[0], QDP_backward, QDP_odd);
  printf0("done\n");

  printf0("Calling QDP_D_eq_sD... ");
  fflush(stdout);
  QDP_D_eq_sD(d2, d1, QDP_neighbor[0], QDP_backward, QDP_even);
  printf0("done\n");

  printf0("Calling QDP_expose_D... ");
  fflush(stdout);
  QDP_expose_D(d2);
  printf0("done\n");

  printf0("Calling QDP_reset_D... ");
  fflush(stdout);
  QDP_reset_D(d2);
  printf0("done\n");

  printf0("Calling QDP_D_eq_zero... ");
  fflush(stdout);
  QDP_D_eq_zero(d2, QDP_all);
  printf0("done\n");

  printf0("Calling QDP_D_eq_sD on custom subset... ");
  fflush(stdout);
  for(i=0; i<QDP_coord_size(3); i++) {
    QDP_D_eq_sD(d2, d1, QDP_neighbor[0], QDP_forward, ts[i]);
    QDP_D_eq_sD(d3, d1, QDP_neighbor[3], QDP_backward, ts[i]);
  }
  printf0("done\n");

  QDP_DiracFermion *dv[4], *d1v[4];
  QDP_ShiftDir sd[4];
  for(i=0; i<4; i++) {
    d1v[i] = d1;
    dv[i] = QDP_create_D();
    sd[i] = QDP_forward;
  }
  printf0("Calling QDP_D_veq_sD on custom subset... ");
  fflush(stdout);
  for(i=0; i<QDP_coord_size(3); i++) {
    QDP_D_veq_sD(dv, d1v, QDP_neighbor, sd, ts[i], 4);
  }
  printf0("done\n");
  for(i=0; i<4; i++) {
    QDP_destroy_D(dv[i]);
  }

  printf0("Freeing fields... ");
  fflush(stdout);
  QDP_destroy_M(m);
  QDP_destroy_D(d1);
  QDP_destroy_D(d2);
  QDP_destroy_D(d3);
  printf0("done\n");
}

int
main(int argc, char *argv[])
{
  int lattice_size[4] = { 8,8,8,8 };

  printf0("Initializing lattice... ");
  fflush(stdout);
  QDP_initialize(&argc, &argv);
  printf0("done\n");
  printf0("Setting lattice size... ");
  fflush(stdout);
  QDP_set_latsize(4, lattice_size);
  printf0("done\n");
  printf0("Creating layout... ");
  fflush(stdout);
  QDP_create_layout();
  printf0("done\n");

  run_tests();

  printf0("Finished tests, now closing QDP... ");
  fflush(stdout);
  QDP_finalize();
  printf0("done\n");
  fflush(stdout);
  return 0;
}
