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
QDP$PC_vget_$ABBR(char *buf, size_t index, int count, void *qfin)
{
  struct QDP_IO_field *qf = qfin;
  $QDPPCTYPE **field = ($QDPPCTYPE **)(qf->data);
#define nc qf->nc
  $QLAPCTYPE($NCVAR(*src));
  $QLAPCTYPE($NCVAR(*dest)) = (void *)buf;
#undef nc
  int i;

/* For the site specified by "index", move an array of "count" data
   from the array of fields to the write buffer */
  for(i = 0; i < count; i++, dest++) {
    //src = QDP$PC_expose_$ABBR(NC field[i] ) + index;
    src = QDP_offset_data(field[i],index);
    QDPIO_get_$ABBR(NC, $P, $PC, dest, src, qf->nc, qf->ns);
    //QDP$PC_reset_$ABBR(NC field[i] );
  }
}

/* Internal factory function for global data */
static void
QDP$PC_vget_$QLAABBR(char *buf, size_t index, int count, void *qfin)
{
  struct QDP_IO_field *qf = qfin;
#define nc qf->nc
  $QLAPCTYPE($NCVAR(*src)) = (void *)(qf->data);
  $QLAPCTYPE($NCVAR(*dest)) = (void *)buf;
#undef nc
  int i;

  for(i = 0; i < count; i++, src++, dest++)
    QDPIO_get_$ABBR(NC, $P, $PC, dest, src, qf->nc, qf->ns);
}

/* Write an array of QDP fields */
int
QDP$PC_vwrite_$ABBR(QDP_Writer *qdpw, QDP_String *md, $QDPPCTYPE *field[],
		    int nv)
{
  struct QDP_IO_field qf;
  int i, status;
  QIO_RecordInfo *rec_info;

  qf.data = (char *) field;
  qf.size = QDPIO_size_$ABBR($P, QDP_get_nc(field[0]), QLA_Ns);
  qf.nc = QDPIO_nc_$ABBR(QDP_get_nc(field[0]));
  qf.ns = QDPIO_ns_$ABBR(QLA_Ns);
  qf.word_size = WS;

  QDP_set_iolat(qdpw->lat);
  rec_info = QIO_create_record_info(QIO_FIELD, 0, 0, 0, 
				    "$QDPPCTYPE", "$P", qf.nc,
				    qf.ns, qf.size, nv);

  for(i=0; i<nv; i++)
    QDP_prepare_dest( &field[i]->dc );

  status = QDP_write_check(qdpw, md, QIO_FIELD, QDP$PC_vget_$ABBR, &qf, nv,
			   rec_info);

  QIO_destroy_record_info(rec_info);

  return status;
}

/* Write a single QDP field */
int
QDP$PC_write_$ABBR(QDP_Writer *qdpw, QDP_String *md, $QDPPCTYPE *f)
{
  $QDPPCTYPE *field[1];
  field[0] = f;
  return QDP$PC_vwrite_$ABBR(qdpw, md, field, 1);
}

/* Write a global array of QLA data */
int
QDP$PC_vwrite_$QLAABBR($NC QDP_Writer *qdpw, QDP_String *md,
		       $QLAPCTYPE($NCVAR(*array)), int count)
{
  struct QDP_IO_field qf;
  QIO_RecordInfo *rec_info;
  int status;

  qf.data = (char *) array;
  qf.size = QDPIO_size_$ABBR($P, $QDP_NC, QLA_Ns);
  qf.nc = QDPIO_nc_$ABBR($QDP_NC);
  qf.ns = QDPIO_ns_$ABBR(QLA_Ns);
  qf.word_size = WS;

  QDP_set_iolat(qdpw->lat);
  rec_info = QIO_create_record_info(QIO_GLOBAL, 0, 0, 0, 
				    "$QDPPCTYPE", "$P", qf.nc,
				    qf.ns, qf.size, count);

  status = QDP_write_check(qdpw, md, QIO_GLOBAL, QDP$PC_vget_$QLAABBR,
			   &qf, count, rec_info);

  QIO_destroy_record_info(rec_info);

  return status;
}
