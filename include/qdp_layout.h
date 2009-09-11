#ifndef _QDP_LAYOUT_H
#define _QDP_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

  // default layout/lattice functions

  extern int QDP_sites_on_node;

  extern void QDP_set_latsize(int nd, const int size[]);
  extern int QDP_create_layout(void);

  extern int QDP_ndim(void);
  extern int QDP_coord_size(int i);
  extern void QDP_latsize(int ls[]);
  extern int QDP_volume(void);

  extern int QDP_numsites(int node);
  extern int QDP_node_number(const int x[]);
  extern int QDP_index(const int x[]);
  extern void QDP_get_coords(int x[], int node, int index);

  // custom layout/lattice functions

  typedef struct QDP_Lattice_struct QDP_Lattice;
  typedef struct {
    void (*setup)(QDP_Lattice *lat, void *args);
    void (*free)(QDP_Lattice *lat);
    int (*numsites)(QDP_Lattice *lat, int node);  // ?
    int (*node_number)(QDP_Lattice *lat, const int x[]);
    int (*index)(QDP_Lattice *lat, const int x[]);
    void (*get_coords)(QDP_Lattice *lat, int x[], int node, int index);
  } QDP_Layout;

  extern QDP_Layout *QDP_layout_hyper_eo;

  extern QDP_Layout *QDP_get_default_layout(void);
  extern void QDP_set_default_layout(QDP_Layout *layout);
  extern QDP_Lattice *QDP_get_default_lattice(void);
  extern void QDP_set_default_lattice(QDP_Lattice *lat);

  extern QDP_Lattice *QDP_create_lattice(QDP_Layout *layout, void *args, int nd, int size[]);
  extern void QDP_destroy_lattice(QDP_Lattice *lat);
  extern QDP_Layout *QDP_get_lattice_layout(QDP_Lattice *lat);

  extern void QDP_allocate_lattice_params(QDP_Lattice *lat, size_t size);
  extern void *QDP_get_lattice_params(QDP_Lattice *lat);

  extern int QDP_sites_on_node_L(QDP_Lattice *lat);

  extern int QDP_ndim_L(QDP_Lattice *lat);
  extern int QDP_coord_size_L(QDP_Lattice *lat, int i);
  extern void QDP_latsize_L(QDP_Lattice *lat, int ls[]);
  extern int QDP_volume_L(QDP_Lattice *lat);

  extern int QDP_numsites_L(QDP_Lattice *lat, int node);
  extern int QDP_node_number_L(QDP_Lattice *lat, const int x[]);
  extern int QDP_index_L(QDP_Lattice *lat, const int x[]);
  extern void QDP_get_coords_L(QDP_Lattice *lat, int x[], int node, int index);

  // need to add e.g. QDP_Lattice *QDP_get_lattice_V(QDP_ColorVector *f);

  /*
  layout (nd, coords); <-> node, index;
  mynode = node_number(lattice);

  field {
  lattice: layout;
    data size;
    alignment;
  }

  shift(X->1);
  shift(1->X);
  shift(X->X);

  QDP_prepare_src(a, subset);
  QDP_prepare_dest(d, subset);
  QDP_site_loop(i, subset) {
    QDP_get_dest_ptr(d, i);
    QDP_get_src_ptr(a, i);
    QLA_V_peq_V(d, a);
  }
  */

#ifdef __cplusplus
}
#endif

#endif
