#ifndef _QDP_LAYOUT_INTERNAL_H
#define _QDP_LAYOUT_INTERNAL_H

#include "qdp_layout.h"

#ifdef __cplusplus
extern "C" {
#endif

  struct QDP_Lattice_struct {
    int sites_on_node;
    int vol;
    int ndim;
    int *dims;
    QDP_Layout *layout;
    void *params;
    QDP_Subset *all;
    QDP_Subset *eo;
    QDP_Shift *neighbor;
    int refcount;
  };

#ifdef __cplusplus
}
#endif

#endif
