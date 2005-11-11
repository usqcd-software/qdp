//#include <stdlib.h>
#include <stdio.h>
#include "qdp_config.h"
#include <qdp.h>

#define LOCAL_SUM

//int lattice_size[4] = { 4,4,4,4 };
//int lattice_size[4] = { 8,8,8,8 };
int lattice_size[4] = { 16,16,16,32 };
int vol;

void
initial_iseed(QLA_Int *li, int coords[])
{
  int i,t;

  t = coords[0];
  for(i=1; i<4; ++i) {
    t = t*lattice_size[i] + coords[i];
  }

  *li = t;
}

QLA_Real
plaquette(QDP_ColorMatrix *link[])
{
  int mu, nu;
  QLA_Real plaq;
  QDP_ColorMatrix *temp1, *temp2, *temp3, *temp4;

#ifdef LOCAL_SUM
  QDP_Real *treal1, *treal2;
  treal1 = QDP_create_R();
  treal2 = QDP_create_R();
  QDP_R_eq_zero(treal2, QDP_all);
#else
  QLA_Real tplaq;
  plaq = 0;
#endif

  temp1 = QDP_create_M();
  temp2 = QDP_create_M();
  temp3 = QDP_create_M();
  temp4 = QDP_create_M();

  for(mu=0; mu<3; ++mu) {
    for(nu=mu+1; nu<4; ++nu) {

      QDP_M_eq_sM(temp1, link[nu], QDP_neighbor[mu], QDP_forward, QDP_all);
      QDP_M_eq_sM(temp2, link[mu], QDP_neighbor[nu], QDP_forward, QDP_all);

      QDP_M_eq_Ma_times_M(temp3, link[nu], link[mu], QDP_all);

      QDP_M_eq_M_times_M(temp4, temp3, temp1, QDP_all);
      QDP_discard_M(temp1);

#ifdef LOCAL_SUM
      QDP_R_eq_re_M_dot_M(treal1, temp2, temp4, QDP_all);
      QDP_discard_M(temp2);
      QDP_R_peq_R(treal2, treal1, QDP_all);
#else
      QDP_r_eq_re_M_dot_M(&tplaq, temp2, temp4, QDP_all);
      QDP_discard_M(temp2);
      plaq += tplaq;
#endif

    }
  }

#ifdef LOCAL_SUM
  QDP_r_eq_sum_R(&plaq, treal2, QDP_all);
  QDP_destroy_R(treal1);
  QDP_destroy_R(treal2);
#endif

  QDP_destroy_M(temp1);
  QDP_destroy_M(temp2);
  QDP_destroy_M(temp3);
  QDP_destroy_M(temp4);

  return plaq/(3*vol);
}

int
main(int argc, char *argv[])
{
  int i;
  QDP_Int *iseed;
  QDP_RandomState *rs;
  QLA_Complex unit;
  QDP_ColorMatrix *link[4];
  QLA_Real plaq;

  QDP_initialize(&argc, &argv);
  QDP_set_latsize(4, lattice_size);
  QDP_create_layout();
  QDP_check_comm(1);

  vol = 1;
  for(i=0; i<4; ++i) vol *= lattice_size[i];

  iseed = QDP_create_I();
  rs = QDP_create_S();

  QDP_I_eq_func(iseed, initial_iseed, QDP_all);
  QDP_S_eq_seed_i_I(rs, 987654321, iseed, QDP_all);

  plaq = 1;
  QLA_C_eq_R(&unit, &plaq);
  for(i=0; i<4; ++i) {
    link[i] = QDP_create_M();
    QDP_M_eq_gaussian_S(link[i], rs, QDP_all);
    //QDP_M_eq_c(link[i], &unit, QDP_all);
  }

  plaq = plaquette(link);
  if(QDP_this_node==0) printf("plaquette = %g\n", plaq);

  QDP_finalize();
  return 0;
}
