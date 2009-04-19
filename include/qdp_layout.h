#ifndef _QDP_LAYOUT_H
#define _QDP_LAYOUT_H

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif
