!PCSHIFTTYPES
#include "qdp_$lib_internal.h"

void
QDP$PC_discard_$ABBR($NC$QDPPCTYPE *dest)
{
  dest->dc.discarded = 1;
}
!END
