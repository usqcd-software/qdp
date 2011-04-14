#include <stdlib.h>
#include "qdp_$lib_internal.h"

#define N $QDP_NC
#define NC ($C+0)

$QDPPCTYPE *
QDP$PC_create_$ABBR($NCVOID)
{
  QDP_Lattice *lat = QDP_get_default_lattice();
  return QDP$PC_create_$ABBR_L($NCVAR lat);
}

$QDPPCTYPE *
QDP$PC_create_$ABBR_L($NC QDP_Lattice *lat)
{
  typedef $QLAPCTYPE($NCVAR foo);
  $QDPPCTYPE *m;

  m = ($QDPPCTYPE *) malloc(sizeof($QDPPCTYPE));
  if(m!=NULL) {
    m->data = NULL;
    m->ptr = NULL;
    m->dc.data = (char **) (void *) &(m->data); /* extra void * is just to */
    m->dc.ptr = (char ***) (void *) &(m->ptr);  /* avoid a compiler warning */
    m->dc.qmpmem = NULL;
    m->dc.size = sizeof(foo);
    m->dc.discarded = 1;
    m->dc.exposed = 0;
    m->dc.srcprep = 0;
    m->dc.destprep = 0;
    m->dc.shift_src = NULL;
    m->dc.shift_dest = NULL;
    m->dc.nc = NC;
    m->dc.lat = lat;
  }

  return m;
}

void
QDP$PC_destroy_$ABBR($QDPPCTYPE *field)
{
  QDP_prepare_destroy(&field->dc);
  if(field->dc.qmpmem) QMP_free_memory(field->dc.qmpmem);
  free((void*)field->ptr);
  free(field);
}

QDP_Lattice *
QDP$PC_get_lattice_$ABBR($QDPPCTYPE *field)
{
  return get_lat(field);
}
