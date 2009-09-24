!GAUGEMULT1
#include "qdp_$lib_internal.h"
#include "com_common.h"
#include "com_common_internal.h"

#define fvdp QLA$PC_$ABBR3_v$EQOP_$ABBR1$ADJ1_times_p$ABBR2$ADJ2
#define fxdp QLA$PC_$ABBR3_x$EQOP_$ABBR1$ADJ1_times_p$ABBR2$ADJ2
#define fvpp QLA$PC_$ABBR3_v$EQOP_p$ABBR1$ADJ1_times_p$ABBR2$ADJ2
#define fxpp QLA$PC_$ABBR3_x$EQOP_p$ABBR1$ADJ1_times_p$ABBR2$ADJ2

void
QDP$PC_$ABBR3_$EQOP_$ABBR1$ADJ1_times_s$ABBR2$ADJ2(
  $QDPPCTYPE3 *dest,
  $QDPPCTYPE1 *src1,
  $QDPPCTYPE2 *src2,
  QDP_Shift shift,
  QDP_ShiftDir fb,
  QDP_Subset subset)
{
  char **temp2;
  QDP_msg_tag *mtag;

  temp2 = (char **)malloc(QDP_sites_on_node_L(get_lat(dest))*sizeof(char *));

  if((fb!=QDP_forward)&&(fb!=QDP_backward)) {
    fprintf(stderr,"QDP: error: bad fb in QDP$PC_$ABBR_eq_s$ABBR\n");
    QDP_abort(1);
  }

  /* prepare shift source */
  if(src2->ptr==NULL) {
    if(src2->data==NULL) {
      fprintf(stderr,"error: shifting from uninitialized source\n");
      QDP_abort(1);
    }
  } else {
    QDP_switch_ptr_to_data(&src2->dc);
  }

  mtag = QDP_declare_shift( temp2, (char *)src2->data, src2->dc.size,
			    shift, fb, subset );
  QDP_do_gather(mtag);
  QDP_prepare_dest(&dest->dc);
  QDP_prepare_src(&src1->dc);
  QDP_wait_gather(mtag);

#define SRC2O(o) ((void *)(((void **)(temp2))+(o)))
#define N -1
#if ($C+0) == -1
  int nc = QDP_get_nc(dest);
#endif
  if(src1->ptr==NULL) {
    if(subset->indexed==0) {
      fvdp($NCVAR QDP_offset_data(dest,subset->offset), QDP_offset_data(src1,subset->offset), SRC2O(subset->offset), subset->len );
    } else {
      fxdp($NCVAR QDP_offset_data(dest,0), QDP_offset_data(src1,0), SRC2O(0), subset->index, subset->len );
    }
  } else {
    if(subset->indexed==0) {
      fvpp($NCVAR QDP_offset_data(dest,subset->offset), QDP_offset_ptr(src1,subset->offset), SRC2O(subset->offset), subset->len );
    } else {
      fxpp($NCVAR QDP_offset_data(dest,0), QDP_offset_ptr(src1,0), SRC2O(0), subset->index, subset->len );
    }
  }

  QDP_cleanup_gather(mtag);
  free((void*)temp2);
}
!END
