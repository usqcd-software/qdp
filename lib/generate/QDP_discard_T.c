!PCTYPES
#include "qdp_$lib_internal.h"

void
QDP$PC_discard_$ABBR($QDPPCTYPE *dest)
{
  TGET;
  ONE {
    dest->dc.discarded = 1;
    dest->dc.srcprep = 0;
  }
  TBARRIER;
}
!END
