!PCSHIFTTYPES
#include "qdp_$lib_internal.h"
#include "com_common.h"

void
QDP$PC_$ABBR_veq_s$ABBR($QDPPCTYPE *dest[], $QDPPCTYPE *src[], QDP_Shift shift[], QDP_ShiftDir fb[], QDP_Subset subset, int nv)
{
  int *ra, restart, i;

  if(QDP_suspended) {
    fprintf(stderr,"QDP error: attempting shift while communications suspended\n");
    QDP_abort(1);
  }

  ra = (int *)malloc(nv*sizeof(int));
  restart = 1;
  for(i=0; i<nv; ++i) {
    ra[i] = QDP_prepare_shift( &dest[i]->dc, &src[i]->dc,
			       shift[i], fb[i], subset );
    if(!ra[i]) restart = 0;
    if(dest[i]->dc.shift_src->st!=dest[0]->dc.shift_src->st) restart = 0;
  }
  if( (restart) && (dest[0]->dc.shift_src->st->nv!=nv) ) restart = 0;
  if(!restart) {
    for(i=0; i<nv; ++i) {
      if(ra[i]) {
	QDP_remove_shift_tag_reference(dest[i]->dc.shift_src->st);
      }
    }
  }
  free(ra);

  for(i=0; i<nv; ++i) {
    if((fb[i]!=QDP_forward)&&(fb[i]!=QDP_backward)) {
      fprintf(stderr,"QDP: error: bad fb in QDP$PC_$ABBR_eq_s$ABBR\n");
      QDP_abort(1);
    }
  }

  QDP_restart = restart;

  if(!restart) {

    dest[0]->dc.shift_src->st = QDP_alloc_shift_tag(nv);
    dest[0]->dc.shift_src->st->msgtag =
      QDP_declare_shift( (char **)dest[0]->ptr, (char *)src[0]->data,
			 dest[0]->dc.size, shift[0], fb[0], subset);
    for(i=1; i<nv; ++i) {
      dest[i]->dc.shift_src->st = dest[0]->dc.shift_src->st;
      QDP_declare_accumulate_shift( &dest[0]->dc.shift_src->st->msgtag,
				    (char **)dest[i]->ptr,
				    (char *)src[i]->data,
				    dest[i]->dc.size,
				    shift[i], fb[i], subset );
    }

  }

  dest[0]->dc.shift_src->st->shift_pending = 1;
  QDP_do_gather(dest[0]->dc.shift_src->st->msgtag);
}
!END
