#include <stdio.h>
#include "qdp_config.h"
#include <qdp.h>

#define printf0 if(QDP_this_node==0) printf

QDP_Lattice *g_lat = NULL;

void
initial_li(QLA_Int *li, int coords[])
{
  int nd = QDP_ndim_L(g_lat);
  int t = coords[nd-1];
  for(int i=nd-2; i>=0; --i) {
    t = t*QDP_coord_size_L(g_lat, i) + coords[i];
  }
  *li = t;
}

void
test(QDP_Lattice *lat)
{
  char fn[]="qdpio-test.bin";
  QDP_DiracFermion *d1out, *d2out, *d1in, *d2in;
  QDP_ColorMatrix *mout[2], *min[2];
  QDP_RandomState *rs;
  QDP_Int *li;
  QDP_Reader *qr;
  QDP_Writer *qw;
  QDP_String *md;
  QLA_Real x, y;
  QDP_Subset sub = QDP_all_L(lat);

  d1out = QDP_create_D_L(lat);
  d2out = QDP_create_D_L(lat);
  d1in = QDP_create_D_L(lat);
  d2in = QDP_create_D_L(lat);
  mout[0] = QDP_create_M_L(lat);
  mout[1] = QDP_create_M_L(lat);
  min[0] = QDP_create_M_L(lat);
  min[1] = QDP_create_M_L(lat);
  rs = QDP_create_S_L(lat);
  li = QDP_create_I_L(lat);

  g_lat = lat;
  QDP_I_eq_func(li, initial_li, sub);
  QDP_S_eq_seed_i_I(rs, 987654321, li, sub);

  printf0("creating string\n");
  md = QDP_string_create();
  printf0("setting string\n");
  QDP_string_set(md, "test");
  printf0("open write\n");
  qw = QDP_open_write_L(lat, md, fn, QDP_SINGLEFILE);
  printf0("done open write\n");

  QDP_D_eq_gaussian_S(d1out, rs, sub);
  QDP_r_eq_norm2_D(&x, d1out, sub);
  printf0("writing field, norm = %g\n", x);
  QDP_write_D(qw, md, d1out);

  QDP_D_eq_gaussian_S(d2out, rs, sub);
  QDP_r_eq_norm2_D(&x, d2out, sub);
  printf0("writing field, norm = %g\n", x);
  QDP_write_D(qw, md, d2out);

  QDP_M_eq_gaussian_S(mout[0], rs, sub);
  QDP_M_eq_gaussian_S(mout[1], rs, sub);
  QDP_r_eq_norm2_M(&x, mout[0], sub);
  printf0("writing mout[0], norm = %g\n", x);
  QDP_r_eq_norm2_M(&x, mout[1], sub);
  printf0("writing mout[1], norm = %g\n", x);
  QDP_vwrite_M(qw, md, mout, 2);

  QDP_close_write(qw);

  QDP_set_read_group_size(1);
  qr = QDP_open_read_L(lat, md, fn);
  printf0("file metadata: %s\n", QDP_string_ptr(md));

  QDP_read_D(qr, md, d1in);
  printf0("record metadata: %s\n", QDP_string_ptr(md));
  QDP_r_eq_norm2_D(&x, d1in, sub);
  QDP_D_meq_D(d1out, d1in, sub);
  QDP_r_eq_norm2_D(&y, d1out, sub);
  printf0("read field,   norm = %g  error = %g\n", x, y);

  QDP_read_D(qr, md, d2in);
  printf0("record metadata: %s\n", QDP_string_ptr(md));
  QDP_r_eq_norm2_D(&x, d2in, sub);
  QDP_D_meq_D(d2out, d2in, sub);
  QDP_r_eq_norm2_D(&y, d2out, sub);
  printf0("read field,   norm = %g  error = %g\n", x, y);

  QDP_vread_M(qr, md, min, 2);
  printf0("record metadata: %s\n", QDP_string_ptr(md));
  QDP_r_eq_norm2_M(&x, min[0], sub);
  QDP_M_meq_M(mout[0], min[0], sub);
  QDP_r_eq_norm2_M(&y, mout[0], sub);
  printf0("read mout[0],   norm = %g  error = %g\n", x, y);
  QDP_r_eq_norm2_M(&x, min[1], sub);
  QDP_M_meq_M(mout[1], min[1], sub);
  QDP_r_eq_norm2_M(&y, mout[1], sub);
  printf0("read mout[1],   norm = %g  error = %g\n", x, y);

  QDP_close_read(qr);

  QDP_string_destroy(md);
  QDP_destroy_D(d1out);
  QDP_destroy_D(d1in);
  QDP_destroy_D(d2out);
  QDP_destroy_D(d2in);
  QDP_destroy_S(rs);
  QDP_destroy_I(li);
}

int
main(int argc, char *argv[])
{
  QDP_initialize(&argc, &argv);

  if(argc>1) {
    QIO_verbose(QIO_VERB_DEBUG);
  }

  int x = 8;

  int nd1 = 4;
  int ls1[nd1];
  for(int i=0; i<nd1; i++) ls1[i] = x;
  QDP_set_latsize(nd1, ls1);
  QDP_create_layout();

  QDP_Lattice *lat1 = QDP_get_default_lattice();

  test(lat1);

  QDP_Layout *layout = QDP_get_default_layout();
  int nd2 = 5;
  int ls2[nd2];
  for(int i=0; i<nd2; i++) ls2[i] = x;
  QDP_Lattice *lat2 = QDP_create_lattice(layout, NULL, nd2, ls2);

  test(lat2);

  QDP_finalize();
  return 0;
}
