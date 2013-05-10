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
      func($NCVAR QDP_offset_data(dest,j), j);
    }
  } else {
    for(int i=i0; i<i1; ++i) {
      int j = subset->offset + i;
      func($NCVAR QDP_offset_data(dest,j), j);
    }
  }
}

/*
 *  T = funcia
 */

void
QDP$PC_$ABBR_eq_funcia($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int index, void *args), void *args, QDP_Subset subset)
{
#define N -1
#if ($C+0) == -1
  int nc = QDP_get_nc(dest);
#endif
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
      func($NCVAR QDP_offset_data(dest,j), j, args);
    }
  } else {
    for(int i=i0; i<i1; ++i) {
      int j = subset->offset + i;
      func($NCVAR QDP_offset_data(dest,j), j, args);
    }
  }
}
!END
