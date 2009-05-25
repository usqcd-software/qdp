!PCSHIFTTYPES
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

  int i, j, *coords;

  coords = (int *)malloc(QDP_ndim()*sizeof(int));

  QDP_prepare_dest(&dest->dc);

  if(subset->indexed) {
    for(i=0; i<subset->len; ++i) {
      j = subset->index[i];
      QDP_get_coords(coords, QDP_this_node, j);
      func($NCVAR QDP_offset_data(dest,j), coords );
    }
  } else {
    j = subset->offset + subset->len;
    for(i=subset->offset; i<j; ++i) {
      QDP_get_coords(coords, QDP_this_node, i);
      func($NCVAR QDP_offset_data(dest,i), coords );
    }
  }

  free(coords);
}
!END
