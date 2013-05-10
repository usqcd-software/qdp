#include <stdio.h>
#include "qdp_config.h"
#include <qdp.h>
#include "congrad_ks.h"

#define dclock QDP_time
#define TINFO  QDP_ThreadInfo *tinfo = QDP_thread_info()
#define TZERO  if(QDP_thread_num_info(tinfo)==0)
#define MASTER if(QDP_this_node==0&&QDP_thread_num_info(tinfo)==0)

int
congrad(QDP_ColorVector  *result,
	QDP_ColorMatrix  **gauge,
	QDP_ColorVector  *rhs,
	QLA_Real         mass,
	int              max_iter,
	double           epsilon)
{
  QLA_Real eps, a, b, rn, rnold, mrn, d, rv[2];
  QLA_Real rhs_norm, mass2;
  QDP_ColorVector *r, *p, *Mr, *MMp, *MMr, *vv[2];
  QDP_Subset sv[2];
  int i, dscount=0, cgcount=0;
  double dstime=0, cgtime=0;
  TINFO;

  prepare_dslash();

  r = QDP_create_V();
  p = QDP_create_V();
  Mr = QDP_create_V();
  MMp = QDP_create_V();
  MMr = QDP_create_V();

  vv[0] = r;
  vv[1] = Mr;
  sv[0] = QDP_even;
  sv[1] = QDP_odd;

  mass2 = 4*mass*mass;

  QDP_r_eq_norm2_V(&rhs_norm, rhs, QDP_even);
  eps = rhs_norm * epsilon;

  MASTER printf("norm=%g  eps=%g\n",rhs_norm,eps);

  do {
    QDP_r_eq_norm2_V(&a,result,QDP_even);
    MASTER printf("result norm=%g\n",a);
    //dstime -= dclock();
    dslash(p, gauge, result, QDP_odd);
    dslash(r, gauge, p, QDP_even);
    //dstime += dclock();
    //dscount += 2;
    QDP_V_meq_r_times_V(r, &mass2, result, QDP_even);
    QDP_V_peq_V(r, rhs, QDP_even); /* r += rhs */
    QDP_V_eq_V(p, r, QDP_even);
    QDP_r_eq_norm2_V(&rn, r, QDP_even);

    TZERO cgtime -= dclock();
    for (i = 0; i < max_iter; i++) {
      //rnold = rn;
      //QDP_r_eq_norm2_V(&rn, r, QDP_even);
      //MASTER printf("%6i c=%g\n",i,rn);
      if (rn < eps) break;

      TZERO dstime -= dclock();
      dslash(Mr, gauge, r, QDP_odd);
      dslash(MMr, gauge, Mr, QDP_even);
      TZERO dstime += dclock();
      dscount += 2;
      QDP_V_meq_r_times_V(MMr, &mass2, r, QDP_even);

      rnold = rn;
      //QDP_r_eq_norm2_V(&rn, r, QDP_even);
      //QDP_r_eq_re_V_dot_V(&mrn, r, MMr, QDP_even);
      //QDP_r_eq_norm2_V(&mrn, Mr, QDP_odd);
      QDP_r_veq_norm2_V_multi(rv, vv, sv, 2);
      rn = rv[0];
      mrn = rv[1];
      mrn = -mrn - mass2*rn;
      if(i==0) {
	b = 0;
	d = mrn;
      } else {
	b = rn/rnold;
	d = mrn + rn*b/a;
      }
      a = - rn / d;
      //printf("%g\n", d);
      //printf("%g\t%g\t", mrn, d);
      //QDP_r_eq_re_V_dot_V(&d, p, MMp, QDP_even);
      //printf("%g\n", d);
      QDP_V_eq_r_times_V_plus_V(p, &b, p, r, QDP_even);  /* p = b*p + r */
      QDP_V_peq_r_times_V(result, &a, p, QDP_even);  /* result += a*p */
      QDP_V_eq_r_times_V_plus_V(MMp, &b, MMp, MMr, QDP_even);/*MMp=b*MMp+MMr*/
      QDP_V_peq_r_times_V(r, &a, MMp, QDP_even);  /* r += a*MMp */
      cgcount++;
    }
    TZERO cgtime += dclock();
  } while(rn>eps);

  QDP_destroy_V(r);
  QDP_destroy_V(p);
  QDP_destroy_V(Mr);
  QDP_destroy_V(MMp);
  QDP_destroy_V(MMr);

  //fprintf(stderr,"done free\n");

  cleanup_dslash();

  //fprintf(stderr,"done cleanup\n");
  MASTER {
    double dsmflop, cgmflop;
    dsmflop = 0.000570*QDP_sites_on_node/2;
    cgmflop = 2*dsmflop + 0.000072*QDP_sites_on_node;
    printf("dslash  time = %g  count = %i  mflops = %g\n", dstime, dscount,
           (dsmflop*dscount)/(dstime));
    printf("congrad time = %g  count = %i  mflops = %g\n", cgtime, cgcount,
           (cgmflop*cgcount)/(cgtime));
  }
  return i;
}
