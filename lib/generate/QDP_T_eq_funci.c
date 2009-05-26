!PCARITHTYPES
/*
 *  T = funci
 */

#include <stdlib.h>
#include "qdp_$lib_internal.h"

void
QDP$PC_$ABBR_eq_funci($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int index), QDP_Subset subset)
{
#define N -1
#if ($C+0) == -1
  int nc = QDP_get_nc(dest);
#endif
  int i, j;

  QDP_prepare_dest(&dest->dc);

  if(subset->indexed) {
    for(i=0; i<subset->len; ++i) {
      j = subset->index[i];
      func($NCVAR QDP_offset_data(dest,j), j );
    }
  } else {
    j = subset->offset + subset->len;
    for(i=subset->offset; i<j; ++i) {
      func($NCVAR QDP_offset_data(dest,i), i );
    }
  }
}
!END
