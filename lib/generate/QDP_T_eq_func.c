!PCARITHTYPES
/*
 *  T = func
 */

#include <stdlib.h>
#include "qdp_$lib_internal.h"

void
QDP$PC_$ABBR_eq_func($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int coords[]), QDP_Subset subset)
{
#define N -1
#if ($C+0) == -1
  int nc = QDP_get_nc(dest);
#endif
  int nd = QDP_ndim_L(get_lat(dest));
  int coords[nd];

  TGET;
  ONE {
    QDP_prepare_dest(&dest->dc);
  }
  TBARRIER;

  int i0, i1;
  TSPLIT(i0, i1, subset->len);
  if(subset->indexed) {
    for(int i=i0; i<i1; ++i) {
      int j = subset->index[i];
      QDP_get_coords_L(get_lat(dest), coords, QDP_this_node, j);
      func($NCVAR QDP_offset_data(dest,j), coords);
    }
  } else {
    for(int i=i0; i<i1; ++i) {
      int j = subset->offset + i;
      QDP_get_coords_L(get_lat(dest), coords, QDP_this_node, j);
      func($NCVAR QDP_offset_data(dest,j), coords);
    }
  }
}

/*
 *  T = funca
 */

void
QDP$PC_$ABBR_eq_funca($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int coords[], void *args), void *args, QDP_Subset subset)
{
#define N -1
#if ($C+0) == -1
  int nc = QDP_get_nc(dest);
#endif
  int nd = QDP_ndim_L(get_lat(dest));
  int coords[nd];

  TGET;
  ONE {
    QDP_prepare_dest(&dest->dc);
  }
  TBARRIER;

  int i0, i1;
  TSPLIT(i0, i1, subset->len);
  if(subset->indexed) {
    for(int i=i0; i<i1; ++i) {
      int j = subset->index[i];
      QDP_get_coords_L(get_lat(dest), coords, QDP_this_node, j);
      func($NCVAR QDP_offset_data(dest,j), coords, args);
    }
  } else {
    for(int i=i0; i<i1; ++i) {
      int j = subset->offset + i;
      QDP_get_coords_L(get_lat(dest), coords, QDP_this_node, j);
      func($NCVAR QDP_offset_data(dest,j), coords, args);
    }
  }
}
!END
