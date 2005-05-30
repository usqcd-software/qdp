#include "qdp_$lib.h"
#include "qdp_$lib_internal.h"

$QLAPCTYPE *
QDP$PC_expose_$ABBR($NC$QDPPCTYPE *dest)
{
  QDP_prepare_dest(&dest->dc);
  dest->dc.exposed = 1;
  return dest->data;
}

void
QDP$PC_reset_$ABBR($NC$QDPPCTYPE *dest)
{
  if(!dest->dc.exposed) {
    fprintf(stderr,"error: trying to restore non-exposed data\n");
    QDP_abort();
  }
  dest->dc.exposed = 0;
}
