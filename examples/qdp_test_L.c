#include <stdio.h>
//#define QDP_Precision 'F'
//#define QDP_Nc 3
#include "qdp_config.h"
#include <qdp.h>

#define printf0 if(QDP_this_node==0) printf

void
initial_gauge(QLA_ColorMatrix *g, int coords[])
{
  QLA_Complex z;
  QLA_M_eq_zero(g);
  for(int i=0; i<QLA_Nc; i++) {
    QLA_c_eq_r_plus_ir(z, i, 0);
    QLA_elem_M(*g,i,i) = z;
  }
}

void
initial_df(QLA_DiracFermion *df, int coords[])
{
  QLA_Complex z;
  for(int i=0; i<QLA_Nc; i++) {
    for(int j=0; j<QLA_Ns; j++) {
      QLA_c_eq_r_plus_ir(z, i+j, coords[0]);
      QLA_elem_D(*df,i,j) = z;
    }
  }
}

void
initial_li(QLA_Int *li, int coords[])
{
  int td = QDP_ndim() - 1;
  int t = coords[td];
  for(int i=td-1; i>=0; --i) {
    t = t*QDP_coord_size(i) + coords[i];
  }
  *li = t;
}

void
print_df(QLA_DiracFermion *df, int coords[])
{
  QLA_Complex z;
  for(int i=0; i<QDP_ndim(); i++) if(coords[i]) return;
  for(int i=0; i<QLA_Nc; i++) {
    for(int j=0; j<QLA_Ns; j++) {
      z = QLA_elem_D(*df,i,j);
      printf0("%g\t%g\n", QLA_real(z), QLA_imag(z));
    }
  }
}

int
timeslices(int x[], void *args)
{
  int td = *(int*)args;
  return x[td];
}

void
run_tests(void)
{
  int i, j, k;
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
  int nd = QDP_ndim();
  int td = nd - 1;
  ts = QDP_create_subset(timeslices, &td, sizeof(td), QDP_coord_size(td));
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
  for(k=0; k<10; k++) {
    for(i=0; i<QDP_coord_size(td); i++) {
      for(j=0; j<nd; j++) {
	QDP_D_eq_sD(d2, d1, QDP_neighbor[j], QDP_forward, ts[i]);
	QDP_D_eq_M_times_D(d1, m, d2, QDP_all);
	QDP_D_eq_sD(d3, d1, QDP_neighbor[j], QDP_backward, ts[i]);
	QDP_D_eq_M_times_D(d1, m, d3, QDP_all);
      }
    }
  }
  printf0("done\n");

  QDP_DiracFermion *dv[nd], *d1v[nd];
  QDP_ShiftDir sd[nd];
  for(i=0; i<nd; i++) {
    d1v[i] = d1;
    dv[i] = QDP_create_D();
    sd[i] = QDP_forward;
  }
  printf0("Calling QDP_D_veq_sD on custom subset... ");
  fflush(stdout);
  for(i=0; i<QDP_coord_size(td); i++) {
    QDP_D_veq_sD(dv, d1v, QDP_neighbor, sd, ts[i], nd);
  }
  printf0("done\n");
  for(i=0; i<nd; i++) {
    QDP_destroy_D(dv[i]);
  }

  printf0("Freeing fields... ");
  fflush(stdout);
  QDP_destroy_S(rs);
  QDP_destroy_I(li);
  QDP_destroy_R(r);
  QDP_destroy_C(c);
  QDP_destroy_V(v);
  QDP_destroy_H(hf);
  QDP_destroy_D(d1);
  QDP_destroy_D(d2);
  QDP_destroy_D(d3);
  QDP_destroy_M(m);
  QDP_destroy_P(p);
  printf0("done\n");
}

static int
subCopyHyper(QDP_Lattice *rlat, int x[], void *args)
{
  int color = 0;
  int nd = QDP_ndim_L(rlat);
  int *sublen = (int *)args;
  int *rof = sublen + nd;
  int *rls = rof + nd;
  for(int i=0; i<nd; i++) {
    int k = (x[i] - rof[i] + rls[i])%rls[i];
    if(k>=sublen[i]) color = 1;
  }
#if 0
  if(color==0) {
    printf("SUB:");
    for(int i=0; i<nd; i++) printf(" %i", x[i]);
    printf("\n");
  }
#endif
  return color;
}

// shift map from slat into rlat with offsets passed in args
static void
mapCopyHyper(QDP_Lattice *rlat, QDP_Lattice *slat, int rx[], int sx[],
	     int *num, int idx, QDP_ShiftDir fb, void *args)
{
  int rnd = QDP_ndim_L(rlat);
  int snd = QDP_ndim_L(slat);
  *num = 1;
  if(fb==QDP_forward) {
    int *rof = (int *)args;
    int *rls = rof + rnd;
    int *sd = rls + rnd;
    int *sof = sd + rnd;
    int *sls = sof + snd;
    for(int j=0; j<snd; j++) sx[j] = sof[j];
    for(int i=0; i<rnd; i++) {
      int k = (rx[i] - rof[i] + rls[i])%rls[i];
      int j = sd[i];
      sx[j] = (k + sx[j])%sls[j];
      if(k>=sls[j]) *num = 0;
    }
#if 0
    if(*num) {
      printf("FWD:");
      for(int i=0; i<rnd; i++) printf(" %i", rx[i]);
      printf(" <-");
      for(int i=0; i<snd; i++) printf(" %i", sx[i]);
      printf("\n");
    }
#endif
  } else { // QDP_backward
    int *sof = (int *)args;
    int *sls = sof + snd;
    int *sd = sls + snd;
    int *rof = sd + snd;
    int *rls = rof + rnd;
    for(int j=0; j<snd; j++) {
      int i = sd[j];
      int k = 0;
      if(i>=0&&i<rnd) k = (rx[i] - rof[i] + rls[i])%rls[i];
      sx[j] = (k + sof[j])%sls[j];
      if(k>=sls[j]) *num = 0;
    }
#if 0
    if(*num) {
      printf("BCK:");
      for(int i=0; i<snd; i++) printf(" %i", sx[i]);
      printf(" ->");
      for(int i=0; i<rnd; i++) printf(" %i", rx[i]);
      printf("\n");
    }
#endif
  }
}

// creates shifts and maps for copying hypercubic region of size rlen
// from slat to rlat.
// the point soff in slat will get copied to roff in rlat.
// subsequent points in rlat will correspond to the permuted directions
// in slat given by sdir
// j = sdir[i]
// r[i] = ( roff[i] + (s[j]-soff[j]+ss[j])%ss[j] )%rs[i]
// s[j] = ( soff[j] + (r[i]-roff[i]+rs[i])%rs[i] )%ss[j]
void
getCopyHyper(QDP_Shift *map, QDP_Subset **subset,
	     QDP_Lattice *rlat, int roff[], int rlen[], int sdir[],
	     QDP_Lattice *slat, int soff[], int num)
{
#define printv(v,n) printf0(#v":"); for(int i=0; i<n; i++) printf0(" %i",v[i]); printf0("\n")
  int rnd = QDP_ndim_L(rlat);
  int snd = QDP_ndim_L(slat);
  //int sublen[rnd], rof[rnd], rs[rnd], sd[rnd], sof[snd], ss[snd];
  int sublen[4*rnd+2*snd], *rof, *rs, *sd, *sof, *ss, nsub[rnd], nsubs=1;
  rof = sublen + rnd;
  rs = rof + rnd;
  sd = rs + rnd;
  sof = sd + rnd;
  ss = sof + snd;
  QDP_latsize_L(rlat, rs);
  QDP_latsize_L(slat, ss);
  //printv(rs,rnd);
  //printv(ss,snd);
  // get subvolume size
  for(int i=0; i<rnd; i++) {
    sd[i] = sdir[i];
    int j = sdir[i];
    sublen[i] = rlen[i];
    if(sublen[i]>rs[i]) sublen[i] = rs[i];
    if(j<0||j>=snd) sublen[i] = 1;
    else if(sublen[i]>ss[j]) sublen[i] = ss[j];
    //printf("sublen[%i]: %i\n", i, sublen[i]);
    nsub[i] = (rlen[i]+sublen[i]-1)/sublen[i];
    nsubs *= nsub[i];
  }
  if(num<0||num>=nsubs) {
    *map = NULL;
    *subset = NULL;
    return;
  }
  // calculate which subvolume we will work on and adjust size if necessary
  int n = num;
  for(int j=0; j<snd; j++) sof[j] = soff[j];
  for(int i=0; i<rnd; i++) {
    int j = sdir[i];
    int k = n%nsub[i];
    n = n/nsub[i];
    int off = k*sublen[i];
    rof[i] = (roff[i]+off)%rs[i];
    if(j>=0&&j<snd) sof[j] = (sof[j]+off)%ss[j];
    if(sublen[i]>(rlen[i]-off)) sublen[i] = rlen[i]-off;
  }
  printv(sublen,rnd);
  printv(rof,rnd);
  printv(rs,rnd);
  printv(sd,rnd);
  printv(sof,snd);
  printv(ss,snd);
  *subset=QDP_create_subset_L(rlat,subCopyHyper,sublen,3*rnd*sizeof(int),2);
  *map=QDP_create_map_L(rlat,slat,mapCopyHyper,rof,(3*rnd+2*snd)*sizeof(int));
}

void
testShift(QDP_Lattice *rlat, QDP_Lattice *slat)
{
  int rnd = QDP_ndim_L(rlat);
  int snd = QDP_ndim_L(slat);
  printf0("rls:");
  for(int i=0; i<rnd; i++) printf0(" %i", QDP_coord_size_L(rlat,i));
  printf0("\n");
  printf0("sls:");
  for(int i=0; i<snd; i++) printf0(" %i", QDP_coord_size_L(slat,i));
  printf0("\n");
  QDP_Real *rf = QDP_create_R_L(rlat);
  QDP_Real *sf = QDP_create_R_L(slat);
  QLA_Real rs=1, ss=2;
  QLA_Real rs2=rs*rs, ss2=ss*ss;
  QDP_R_eq_r(rf, &rs, QDP_all_L(rlat));
  QDP_R_eq_r(sf, &ss, QDP_all_L(slat));
  QLA_Real rn, sn, rn2;
  QDP_r_eq_norm2_R(&rn, rf, QDP_all_L(rlat));
  QDP_r_eq_norm2_R(&sn, sf, QDP_all_L(slat));
  printf0("recv V: %g\n", rn/rs2);
  printf0("send V: %g\n", sn/ss2);

  int roff[rnd], rlen[rnd], sdir[rnd], soff[snd];
  for(int i=0; i<rnd; i++) {
    roff[i] = 0;
    rlen[i] = QDP_coord_size_L(rlat,i);
    sdir[i] = i;
  }
  for(int i=0; i<snd; i++) {
    soff[i] = 0;
  }

  for(int s=0; ; s++) {
    printf0("s: %i\n", s);
    QDP_Subset *sub;
    QDP_Shift shift;
    getCopyHyper(&shift, &sub, rlat, roff, rlen, sdir, slat, soff, s);
    if(sub==NULL) break;
    printf0("created subset and map\n");
    QDP_r_eq_norm2_R(&rn2, rf, sub[0]);
    printf0("subset V: %g\n", rn2/rs2);
    //QDP_R_eq_zero(rf, sub[0]);
    QDP_R_eq_sR(rf, sf, shift, QDP_forward, sub[0]);
    printf0("finished shift\n");
    QDP_r_eq_norm2_R(&rn2, rf, sub[0]);
    printf0("subset V: %g\n", rn2/ss2);
    QDP_r_eq_norm2_R(&rn2, rf, QDP_all_L(rlat));
    printf0("diff V: %g\n", (rn2-rn)/(ss2-rs2));
    QDP_destroy_shift(shift);
    printf0("destroyed map\n");
    QDP_destroy_subset(sub);
    printf0("destroyed subset\n");
  }
}

int
main(int argc, char *argv[])
{
  int lattice_size[] = { 8,8,8,8 };
  //int lattice_size[] = { 4,4 };
  int nd = sizeof(lattice_size)/sizeof(int);

  printf0("Initializing lattice... ");
  fflush(stdout);
  QDP_initialize(&argc, &argv);
  printf0("done\n");
  printf0("Setting lattice size... ");
  fflush(stdout);
  QDP_set_latsize(nd, lattice_size);
  printf0("done\n");
  printf0("Creating layout... ");
  fflush(stdout);
  QDP_create_layout();
  printf0("done\n");
  QDP_check_comm(1);

  run_tests();

  printf0("\nCreating new lattice... ");
  QDP_Lattice *lat = QDP_get_default_lattice();
  QDP_Layout *layout = QDP_get_default_layout();
  int nd2 = 5;
  int ls2[nd2];
  for(int i=0; i<nd2; i++) ls2[i] = 6;
  QDP_Lattice *lat2 = QDP_create_lattice(layout, NULL, nd2, ls2);
  printf0("done\n");

  printf0("Setting default lattice... ");
  QDP_set_default_lattice(lat2);
  printf0("done\n");

  printf0("Testing inter-lattice shift...\n");
  testShift(lat, lat2);
  printf0("reverse direction...\n");
  testShift(lat2, lat);
  printf0("done\n");

  printf0("Destroying old lattice... ");
  QDP_destroy_lattice(lat);
  printf0("done\n");

  run_tests();

  printf0("Finished tests, now closing QDP...\n");
  fflush(stdout);
  QDP_finalize();
  printf0("done\n");
  fflush(stdout);
  return 0;
}
