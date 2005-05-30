#include <stdio.h>
#include <qdp.h>
#include "congrad_ks.h"

static QDP_ColorVector *temp0, **temp1, **temp2, **temp3;
static QDP_ColorVector *temp01, *temp11[4], *temp21[4], *temp31[4];
static QDP_ColorVector *temp02, *temp12[4], *temp22[4], *temp32[4];

/* allocate fields before calling Dirac_n */
/* this is so we can reuse the fields & gathers */
void
prepare_dslash(void)
{
  int mu;

  temp01 = QDP_create_V();
  temp02 = QDP_create_V();
  for (mu = 0; mu < 4; mu++) {
    temp11[mu] = QDP_create_V();
    temp21[mu] = QDP_create_V();
    temp31[mu] = QDP_create_V();
    temp12[mu] = QDP_create_V();
    temp22[mu] = QDP_create_V();
    temp32[mu] = QDP_create_V();
  }
}

/* free temporary fields when we're done using Dirac_n */
void
cleanup_dslash(void)
{
  int mu;

  QDP_destroy_V(temp01);
  QDP_destroy_V(temp02);
  for (mu = 0; mu < 4; mu++) {
    QDP_destroy_V(temp11[mu]);
    QDP_destroy_V(temp21[mu]);
    QDP_destroy_V(temp31[mu]);
    QDP_destroy_V(temp12[mu]);
    QDP_destroy_V(temp22[mu]);
    QDP_destroy_V(temp32[mu]);
  }
}

#define VECT

/* Staggered D-slash */
void
dslash(QDP_ColorVector *result,
       QDP_ColorMatrix **gauge,
       QDP_ColorVector *source,
       QDP_Subset subset)
{
  int mu;
  //QLA_Real t;
  QDP_Subset osubset;

#ifdef VECT
  QDP_ShiftDir fwd[4], bck[4];
  QDP_ColorVector *vsource[4];
  QDP_ColorVector *vresult[4];
#endif

  if(subset==QDP_even) {
    osubset = QDP_odd;
    temp0 = temp01;
    temp1 = temp11;
    temp2 = temp21;
    temp3 = temp31;
  } else if(subset==QDP_odd) {
    osubset = QDP_even;
    temp0 = temp02;
    temp1 = temp12;
    temp2 = temp22;
    temp3 = temp32;
  } else {
    osubset = QDP_all;
    temp0 = temp01;
    temp1 = temp11;
    temp2 = temp21;
    temp3 = temp31;
  }

#ifdef VECT

  for (mu = 0; mu < 4; mu++) {
    vsource[mu] = temp0;
    vresult[mu] = result;
    fwd[mu] = QDP_forward;
    bck[mu] = QDP_backward;
  }

  QDP_V_eq_V(temp0, source, osubset);
  //QDP_r_eq_norm2_V(&t, source, QDP_all);
  //printf("%g\n", t);

  //if(QDP_this_node==0) printf("vsource\n");
  //QDP_V_eq_func(vsource[0], print_V, QDP_all);
  QDP_V_veq_sV(temp1, vsource, QDP_neighbor, fwd, subset, 4);
  //if(QDP_this_node==0) printf("temp1\n");
  //QDP_V_eq_func(temp1[0], print_V, QDP_all);

  QDP_V_veq_Ma_times_V(temp2, gauge, vsource, osubset, 4);
  //if(QDP_this_node==0) printf("temp2\n");
  //QDP_V_eq_func(temp2[0], print_V, QDP_all);
  //QDP_r_eq_norm2_V(&t, temp2[0], QDP_all);
  //printf("%g\n", t);
  //QDP_r_eq_norm2_V(&t, temp2[1], QDP_all);
  //printf("%g\n", t);
  //QDP_r_eq_norm2_V(&t, temp2[2], QDP_all);
  //printf("%g\n", t);
  //QDP_r_eq_norm2_V(&t, temp2[3], QDP_all);
  //printf("%g\n", t);

  QDP_V_veq_sV(temp3, temp2, QDP_neighbor, bck, subset, 4);
  //if(QDP_this_node==0) printf("temp3\n");
  //QDP_V_eq_func(temp3[0], print_V, QDP_all);
  //QDP_r_eq_norm2_V(&t, temp3[0], QDP_all);
  //printf("%g\n", t);
  //QDP_r_eq_norm2_V(&t, temp3[1], QDP_all);
  //printf("%g\n", t);
  //QDP_r_eq_norm2_V(&t, temp3[2], QDP_all);
  //printf("%g\n", t);
  //QDP_r_eq_norm2_V(&t, temp3[3], QDP_all);
  //printf("%g\n", t);

  QDP_V_eq_zero(result, subset);
  QDP_V_vpeq_M_times_V(vresult, gauge, temp1, subset, 4);
  //QDP_r_eq_norm2_V(&t, result, QDP_all);
  //printf("%g\n", t);
  for (mu = 0; mu < 4; mu++) {
    QDP_discard_V(temp1[mu]);
  }

  QDP_V_vmeq_V(vresult, temp3, subset, 4);
  for (mu = 0; mu < 4; mu++) {
    QDP_discard_V(temp3[mu]);
  }

  //QDP_r_eq_norm2_V(&t, result, QDP_all);
  //printf("%g\n", t);

  //if(QDP_this_node==0) printf("vresult\n");
  //QDP_V_eq_func(vresult[0], print_V, QDP_all);
#else

  for (mu = 0; mu < 4; mu++) {
    QDP_V_eq_sV(temp1[mu], source, QDP_neighbor[mu], QDP_forward, subset);
  }

  for (mu = 0; mu < 4; mu++) {
    QDP_V_eq_Ma_times_V(temp2[mu], gauge[mu], source, osubset);
    QDP_V_eq_sV(temp3[mu], temp2[mu], QDP_neighbor[mu], QDP_backward, subset);
  }

  //if(QDP_this_node==0) printf("temp1\n");
  //QDP_V_eq_func(temp1[0], print_V, subset);
  QDP_V_eq_M_times_V(result, gauge[0], temp1[0], subset);
  //if(QDP_this_node==0) printf("result\n");
  //QDP_V_eq_func(result, print_V, QDP_all);
  QDP_discard_V(temp1[0]);
  for (mu = 1; mu < 4; mu++) {
    QDP_V_peq_M_times_V(result, gauge[mu], temp1[mu], subset);
    QDP_discard_V(temp1[mu]);
  }

  for (mu = 0; mu < 4; mu++) {
    QDP_V_meq_V(result, temp3[mu], subset);
    QDP_discard_V(temp3[mu]);
  }
#endif
}
