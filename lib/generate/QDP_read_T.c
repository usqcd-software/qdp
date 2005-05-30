#include <stdlib.h>
#include "qdp_$lib_internal.h"
#include <qdp_string.h>

#undef N
#define N -1
#if ($C+0) == -1
#define NC qf->nc,
#else
#define NC
#endif
#undef N

#undef F
#undef D
#define F -1
#define D -1
#if ($P+0) == -1
#define WS QDPIO_word_$ABBR($P)
#else
#define WS QDPIO_word_$ABBR(dummy)
#endif
#undef F
#undef D

/* Internal factory function for array of field data */
static void
QDP$PC_vput_$ABBR(char *buf, size_t index, int count, void *qfin)
{
  struct QDP_IO_field *qf = qfin;
  $QDPPCTYPE **field = ($QDPPCTYPE **)(qf->data);
  $QLAPCTYPE *dest;
  $QLAPCTYPE *src = ($QLAPCTYPE *)buf;
  int i;

/* For the site specified by "index", move an array of "count" data
   from the read buffer to an array of fields */

  for(i=0; i<count; i++) {
    dest = QDP$PC_expose_$ABBR(NC field[i] ) + index;
    QDPIO_put_$ABBR(NC, $P, $PC, dest, src+i, qf->nc, QLA_Ns);
    QDP$PC_reset_$ABBR(NC field[i] );
  }
}

/* Internal factory function for global data */
static void
QDP$PC_vput_$QLAABBR(char *buf, size_t index, int count, void *qfin)
{
  struct QDP_IO_field *qf = qfin;
  $QLAPCTYPE *dest = ($QLAPCTYPE *)(qf->data);
  $QLAPCTYPE *src = ($QLAPCTYPE *)buf;
  int i;

  for(i=0; i<count; i++) {
    QDPIO_put_$ABBR(NC, $P, $PC, (dest+i), (src+i), qf->nc, QLA_Ns);
  }
}

/* read an array of QDP fields */
int
QDP$PC_vread_$ABBR($NC QDP_Reader *qdpr, QDP_String *md, $QDPPCTYPE *field[],
		   int nv)
{
  struct QDP_IO_field qf;
  int i, status;
  QIO_RecordInfo *cmp_info;

  qf.data = (char *) field;
  qf.size = QDPIO_size_$ABBR($P, $QDP_NC, QLA_Ns);
  qf.nc = $QDP_NC;
  qf.word_size = WS;

  cmp_info = QIO_create_record_info(QIO_FIELD, "$QDPPCTYPE", "$P", $QDP_NC,
				    QLA_Ns, qf.size, nv);

  for(i=0; i<nv; i++) QDP_prepare_dest( &field[i]->dc );

  status = QDP_read_check(qdpr, md, QIO_FIELD, QDP$PC_vput_$ABBR, &qf,
			  nv, cmp_info);

  QIO_destroy_record_info(cmp_info);

  return status;
}

/* read a single QDP field */
int
QDP$PC_read_$ABBR($NC QDP_Reader *qdpr, QDP_String *md, $QDPPCTYPE *field)
{
  $QDPPCTYPE *temp[1];
  temp[0] = field;
  return QDP$PC_vread_$ABBR($NCVAR qdpr, md, temp, 1);
}

/* read a global array of QLA data */
int
QDP$PC_vread_$QLAABBR($NC QDP_Reader *qdpr, QDP_String *md, $QLAPCTYPE *array,
		      int n)
{
  struct QDP_IO_field qf;
  int status;
  QIO_RecordInfo *cmp_info;

  qf.data = (char *) array;
  qf.size = QDPIO_size_$ABBR($P, $QDP_NC, QLA_Ns);
  qf.nc   = $QDP_NC;
  qf.word_size = WS;

  cmp_info = QIO_create_record_info(QIO_GLOBAL, "$QDPPCTYPE", "$P", $QDP_NC,
				    QLA_Ns, qf.size, n);

  status = QDP_read_check(qdpr, md, QIO_GLOBAL, QDP$PC_vput_$QLAABBR, &qf,
			  n, cmp_info);

  QIO_destroy_record_info(cmp_info);

  return status;
}
