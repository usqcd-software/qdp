#include <stdlib.h>
#include "qdp_$lib_internal.h"

$QDPPCTYPE *
QDP$PC_create_$ABBR($NCVOID)
{
  $QDPPCTYPE *m;

  m = ($QDPPCTYPE *) malloc(sizeof($QDPPCTYPE));
  if(m!=NULL) {
    m->data = NULL;
    m->ptr = NULL;
    m->dc.data = (char **) &(m->data);
    m->dc.ptr = (char ***) &(m->ptr);
    m->dc.qmpmem = NULL;
    m->dc.size = sizeof($QLAPCTYPE);
    m->dc.discarded = 1;
    m->dc.exposed = 0;
    m->dc.shift_src = NULL;
    m->dc.shift_dest = NULL;
  }

  return m;
}

void
QDP$PC_destroy_$ABBR($NC$QDPPCTYPE *field)
{
  QDP_prepare_destroy(&field->dc);
  //free(field->data);
  if(field->dc.qmpmem) QMP_free_memory(field->dc.qmpmem);
  free((void*)field->ptr);
  free(field);
}
