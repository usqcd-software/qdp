!PCSHIFTTYPES
/*
 *  T = func
 */

#include <stdlib.h>
#include "qdp_$lib.h"
#include "qdp_$lib_internal.h"

void
QDP$PC_$ABBR_eq_func($NC$QDPPCTYPE *dest, void (*func)($QLAPCTYPE *dest, int coords[]), QDP_Subset subset)
{
  int i, j, *coords;

  coords = (int *)malloc(QDP_ndim()*sizeof(int));

  QDP_prepare_dest(&dest->dc);

  if(subset->indexed) {
    for(i=0; i<subset->len; ++i) {
      j = subset->index[i];
      QDP_get_coords(coords, QDP_this_node, j);
      func( &dest->data[j], coords );
    }
  } else {
    j = subset->offset + subset->len;
    for(i=subset->offset; i<j; ++i) {
      QDP_get_coords(coords, QDP_this_node, i);
      func( &dest->data[i], coords );
    }
  }

  free(coords);
}
!END
