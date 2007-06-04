!PCSHIFTTYPES
/*
 *  T = funci
 */

#include <stdlib.h>
#include "qdp_$lib_internal.h"

void
QDP$PC_$ABBR_eq_funci($NC$QDPPCTYPE *dest, void (*func)($QLAPCTYPE *dest, int index), QDP_Subset subset)
{
  int i, j;

  QDP_prepare_dest(&dest->dc);

  if(subset->indexed) {
    for(i=0; i<subset->len; ++i) {
      j = subset->index[i];
      func( &dest->data[j], j );
    }
  } else {
    j = subset->offset + subset->len;
    for(i=subset->offset; i<j; ++i) {
      func( &dest->data[i], i );
    }
  }
}
!END
