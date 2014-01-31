#include <stdio.h>
#include "qdp_config.h"
#include <qdp.h>
#include "congrad_wilson.h"

static QDP_DiracFermion *tempd, *tempr;
static QDP_HalfFermion *temp0, *temp1[4], *temp2[4], *temp3[4], *temp4[4];

/* allocate fields before calling dslash */
/* this is so we can reuse the fields & gathers */
void
prepare_dslash(void)
{
  int mu;

  tempr = QDP_create_D();
  tempd = QDP_create_D();
  temp0 = QDP_create_H();
  for(mu=0; mu<4; mu++) {
    temp1[mu] = QDP_create_H();
    temp2[mu] = QDP_create_H();
    temp3[mu] = QDP_create_H();
    temp4[mu] = QDP_create_H();
  }
}

/* free temporary fields when we're done using dslash */
void
cleanup_dslash(void)
{
  int mu;

  QDP_destroy_D(tempr);
  QDP_destroy_D(tempd);
  QDP_destroy_H(temp0);
  for(mu=0; mu<4; mu++) {
    QDP_destroy_H(temp1[mu]);
    QDP_destroy_H(temp2[mu]);
    QDP_destroy_H(temp3[mu]);
    QDP_destroy_H(temp4[mu]);
  }
}

/* Wilson dslash */
void
dslash(QDP_DiracFermion  *result,
       QDP_ColorMatrix   **gauge,
       QDP_DiracFermion  *source,
       QLA_Real          kappa,
       int               sign)
{
  //QDP_DiracFermion *vsource[4];
  QDP_DiracFermion *vresult[4];
  //QDP_ShiftDir fwd[4], bck[4];
  int dir[4], sgn[4], msgn[4];

  for(int mu=0; mu<4; mu++) {
    //vsource[mu] = source;
    vresult[mu] = tempr;
    //fwd[mu] = QDP_forward;
    //bck[mu] = QDP_backward;
    dir[mu] = mu;
    sgn[mu] = sign;
    msgn[mu] = -sign;
  }

  //QDP_D_eq_D(result, source, QDP_all);

  //QDP_H_veq_spproj_D(temp1, vsource, dir, sgn, QDP_all, 4);
  //QDP_H_veq_sH(temp2, temp1, QDP_neighbor, fwd, QDP_all, 4);
  for(int mu=0; mu<4; mu++) {
    QDP_H_eq_spproj_D(temp1[mu], source, mu, sign, QDP_all);
    QDP_H_eq_sH(temp2[mu], temp1[mu], QDP_neighbor[mu], QDP_forward, QDP_all);
  }

  for(int mu=0; mu<4; mu++) {
    QDP_H_eq_spproj_D(temp0, source, mu, -sign, QDP_all);
    QDP_H_eq_Ma_times_H(temp3[mu], gauge[mu], temp0, QDP_all);
    QDP_H_eq_sH(temp4[mu], temp3[mu], QDP_neighbor[mu], QDP_backward, QDP_all);
  }

  QDP_D_eq_zero(tempr, QDP_all);

  QDP_D_vpeq_sprecon_M_times_H(vresult, gauge, temp2, dir, sgn, QDP_all, 4);
  for(int mu=0; mu<4; mu++) {
    //QDP_D_peq_sprecon_M_times_H(tempr, gauge[mu], temp2[mu], mu, sign, QDP_all);
    //QDP_H_eq_M_times_H(temp0, gauge[mu], temp2[mu], QDP_all);
    //QDP_D_peq_sprecon_H(tempr, temp0, mu, sign, QDP_all);
    //QDP_D_meq_r_times_D(tempr, &kappa, tempd, QDP_all);
    //QDP_D_meq_D(tempr, tempd, QDP_all);
    QDP_discard_H(temp2[mu]);
  }

  QDP_D_vpeq_sprecon_H(vresult, temp4, dir, msgn, QDP_all, 4);
  for(int mu=0; mu<4; mu++) {
    //QDP_D_peq_sprecon_H(tempr, temp4[mu], mu, -sign, QDP_all);
    //QDP_D_meq_r_times_D(tempr, &kappa, tempd, QDP_all);
    //QDP_D_meq_D(tempr, tempd, QDP_all);
    QDP_discard_H(temp4[mu]);
  }

  kappa = -kappa;
  QDP_D_eq_r_times_D_plus_D(result, &kappa, tempr, source, QDP_all);

  //fprintf(stderr, "end dslash\n");
}
