#include <stdio.h>
#include "qdp_config.h"
#include <qdp.h>
#include "congrad_wilson.h"

#define dclock QDP_time

int
congrad(QDP_DiracFermion  *result,
	QDP_ColorMatrix   **gauge,
	QDP_DiracFermion  *rhs,
	QLA_Real          kappa,
	int               max_iter,
	double            epsilon)
{
  QLA_Real eps, a, b, c, d;
  QLA_Real rhs_norm;
  QDP_DiracFermion *r, *p, *Mp, *MMp;
  int i, dscount=0, cgcount=0;
  double dstime=0, cgtime=0;

  prepare_dslash();

  r = QDP_create_D();
  p = QDP_create_D();
  Mp = QDP_create_D();
  MMp = QDP_create_D();

  QDP_r_eq_norm2_D(&rhs_norm, rhs, QDP_all);
  eps = rhs_norm * epsilon;

  if(QDP_this_node==0) printf("norm=%g  eps=%g\n",rhs_norm,eps);

  do {
    QDP_r_eq_norm2_D(&a, result, QDP_all);
    if(QDP_this_node==0) printf("result norm=%g\n",a);
    //dstime -= dclock();
    dslash(p, gauge, result, kappa, +1);
    dslash(r, gauge, p, kappa, -1);
    //dstime += dclock();
    //dscount += 2;
    QDP_D_meq_D(r, rhs, QDP_all);  /* r -= rhs */
    QDP_D_eq_D(p, r, QDP_all);
    QDP_r_eq_norm2_D(&c, r, QDP_all);

    cgtime -= dclock();
    for(i=0; i<max_iter; i++) {
      if(QDP_this_node==0) printf("%6i c=%g\n", i, c);
      if(c<eps) break;

      dstime -= dclock();
      dslash(Mp, gauge, p, kappa, +1);
      dslash(MMp, gauge, Mp, kappa, -1);
      dstime += dclock();
      dscount += 2;

      QDP_r_eq_norm2_D(&d, Mp, QDP_all);
      a = - c / d;
      QDP_D_peq_r_times_D(result, &a, p, QDP_all);  /* result += a*p */
      QDP_D_peq_r_times_D(r, &a, MMp, QDP_all);  /* r += a*MMp */
      QDP_r_eq_norm2_D(&d, r, QDP_all);
      b = d / c;
      c = d;
      QDP_D_eq_r_times_D_plus_D(p, &b, p, r, QDP_all);  /* p = b*p + r */
      cgcount++;
    }
    cgtime += dclock();
  } while(c>eps);

  QDP_destroy_D(r);
  QDP_destroy_D(p);
  QDP_destroy_D(Mp);
  QDP_destroy_D(MMp);

  //fprintf(stderr,"done free\n");

  cleanup_dslash();

  //fprintf(stderr,"done cleanup\n");
  if(QDP_this_node==0) {
    double dsmflop, cgmflop;
    dsmflop = 0.001536*QDP_sites_on_node;
    cgmflop = 2*dsmflop + 0.000240*QDP_sites_on_node;
    //cgmflop = 0.005616*QDP_sites_on_node;
    printf("dslash  time = %g  count = %i  mflops = %g\n", dstime, dscount,
           (dsmflop*dscount)/(dstime));
    printf("congrad time = %g  count = %i  mflops = %g\n", cgtime, cgcount,
           (cgmflop*cgcount)/(cgtime));
  }
  return i;
}
