!GAUGEMULT2
#include "qdp_$lib_internal.h"
#include "com_common.h"
#include "com_common_internal.h"

#define fvpd QLA$PC_$ABBR3_v$EQOP_p$ABBR1$ADJ1_times_$ABBR2$ADJ2
#define fxpd QLA$PC_$ABBR3_x$EQOP_p$ABBR1$ADJ1_times_$ABBR2$ADJ2
#define fvpp QLA$PC_$ABBR3_v$EQOP_p$ABBR1$ADJ1_times_p$ABBR2$ADJ2
#define fxpp QLA$PC_$ABBR3_x$EQOP_p$ABBR1$ADJ1_times_p$ABBR2$ADJ2

void
QDP$PC_$ABBR3_$EQOP_s$ABBR1$ADJ1_times_$ABBR2$ADJ2(
  $NC$QDPPCTYPE3 *dest,
  $QDPPCTYPE1 *src1,
  $QDPPCTYPE2 *src2,
  QDP_Shift shift,
  QDP_ShiftDir fb,
  QDP_Subset subset)
{
  char **temp1;
  QDP_msg_tag *mtag;

  temp1 = (char **)malloc(QDP_sites_on_node*sizeof(char *));

  if((fb!=QDP_forward)&&(fb!=QDP_backward)) {
    fprintf(stderr,"QDP: error: bad fb in QDP$PC_$ABBR_eq_s$ABBR\n");
    QDP_abort();
  }

  /* prepare shift source */
  if(src1->ptr==NULL) {
    if(src1->data==NULL) {
      fprintf(stderr,"error: shifting from uninitialized source\n");
      QDP_abort();
    }
  } else {
    QDP_switch_ptr_to_data(&src1->dc);
  }

  mtag = QDP_declare_shift( temp1, (char *)src1->data, sizeof($QLAPCTYPE1),
			    shift, fb, subset );
  QDP_do_gather(mtag);
  QDP_prepare_dest(&dest->dc);
  QDP_prepare_src(&src2->dc);
  QDP_wait_gather(mtag);

#define SRC1 (($QLAPCTYPE1 **)temp1)
#define SRC2D src2->data
#define SRC2P src2->ptr
  if(src2->ptr==NULL) {
    if(subset->indexed==0) {
      fvpd($NCVAR dest->data+subset->offset, SRC1+subset->offset, SRC2D+subset->offset, subset->len );
    } else {
      fxpd($NCVAR dest->data, SRC1, SRC2D, subset->index, subset->len );
    }
  } else {
    if(subset->indexed==0) {
      fvpp($NCVAR dest->data+subset->offset, SRC1+subset->offset, SRC2P+subset->offset, subset->len );
    } else {
      fxpp($NCVAR dest->data, SRC1, SRC2P, subset->index, subset->len );
    }
  }

  QDP_cleanup_gather(mtag);
  free((void*)temp1);
}
!END
