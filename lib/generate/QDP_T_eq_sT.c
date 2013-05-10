!PCTYPES
#include "qdp_$lib_internal.h"
#include "com_common.h"

void
QDP$PC_$ABBR_eq_s$ABBR($QDPPCTYPE *dest, $QDPPCTYPE *src, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset)
{
  TGET;
  ONE {
    int restart;

    if(QDP_suspended) {
      fprintf(stderr,"QDP error: attempting shift while communications suspended\n");
      QDP_abort(1);
    }

    if((fb!=QDP_forward)&&(fb!=QDP_backward)) {
      fprintf(stderr,"QDP: error: bad fb in QDP$PC_$ABBR_eq_s$ABBR\n");
      QDP_abort(1);
    }

    TBARRIER;

    restart = QDP_prepare_shift(&dest->dc, &src->dc, shift, fb, subset);

    if((restart)&&(dest->dc.shift_src->st->nv!=1)) {
      restart = 0;
      QDP_remove_shift_tag_reference(dest->dc.shift_src->st);
    }

    QDP_restart = restart;

    if(!restart) {

      dest->dc.shift_src->st = QDP_alloc_shift_tag(1);

      dest->dc.shift_src->st->msgtag =
	QDP_declare_shift( (char **)dest->ptr, (char *)src->data,
			   src->dc.size, shift, fb, subset );

    }

    dest->dc.shift_src->st->shift_pending = 1;
    QDP_do_gather(dest->dc.shift_src->st->msgtag);
    //src->dc.srcprep = 1;
    src->dc.destprep = 0;
    dest->dc.srcprep = 0;
    dest->dc.destprep = 0;
  } else {
    TBARRIER;
  }
}
!END
