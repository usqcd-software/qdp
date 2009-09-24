!GAUGEMULT2
#include "qdp_$lib_internal.h"
#include "com_common.h"
#include "com_common_internal.h"

#define fvpp QLA$PC_$ABBR3_v$EQOP_p$ABBR1$ADJ1_times_p$ABBR2$ADJ2
#define fxpp QLA$PC_$ABBR3_x$EQOP_p$ABBR1$ADJ1_times_p$ABBR2$ADJ2

void
QDP$PC_$ABBR3_$EQOP_s$ABBR1$ADJ1_times_s$ABBR2$ADJ2(
  $QDPPCTYPE3 *dest,
  $QDPPCTYPE1 *src1,
  $QDPPCTYPE2 *src2,
  QDP_Shift shift,
  QDP_ShiftDir fb,
  QDP_Subset subset)
{
  char **temp1, **temp2;
  QDP_msg_tag *mtag1, *mtag2;

  temp1 = (char **)malloc(QDP_sites_on_node_L(get_lat(dest))*sizeof(char *));
  temp2 = (char **)malloc(QDP_sites_on_node_L(get_lat(dest))*sizeof(char *));

  if((fb!=QDP_forward)&&(fb!=QDP_backward)) {
    fprintf(stderr,"QDP: error: bad fb in QDP$PC_$ABBR_eq_s$ABBR\n");
    QDP_abort(1);
  }

  /* prepare shift source 1 */
  if(src1->ptr==NULL) {
    if(src1->data==NULL) {
      fprintf(stderr,"error: shifting from uninitialized source\n");
      QDP_abort(1);
    }
  } else {
    QDP_switch_ptr_to_data(&src1->dc);
  }
  mtag1 = QDP_declare_shift( temp1, (char *)src1->data, src1->dc.size,
			     shift, fb, subset );
  QDP_do_gather(mtag1);

  /* prepare shift source 2 */
  if(src2->ptr==NULL) {
    if(src2->data==NULL) {
      fprintf(stderr,"error: shifting from uninitialized source\n");
      QDP_abort(1);
    }
  } else {
    QDP_switch_ptr_to_data(&src2->dc);
  }
  mtag2 = QDP_declare_shift( temp2, (char *)src2->data, src2->dc.size,
			     shift, fb, subset );
  QDP_do_gather(mtag2);

  QDP_prepare_dest(&dest->dc);
  QDP_prepare_src(&src2->dc);
  QDP_wait_gather(mtag1);
  QDP_wait_gather(mtag2);

#define SRC1O(o) ((void *)(((void **)(temp1))+(o)))
#define SRC2O(o) ((void *)(((void **)(temp2))+(o)))
#define N -1
#if ($C+0) == -1
  int nc = QDP_get_nc(dest);
#endif
  if(subset->indexed==0) {
    fvpp($NCVAR QDP_offset_data(dest,subset->offset), SRC1O(subset->offset), SRC2O(subset->offset), subset->len );
  } else {
    fxpp($NCVAR QDP_offset_data(dest,0), SRC1O(0), SRC2O(0), subset->index, subset->len );
  }

  QDP_cleanup_gather(mtag1);
  QDP_cleanup_gather(mtag2);
  free((void*)temp1);
  free((void*)temp2);
}
!END
