!GAUGEMULT1
#include "qdp_$lib.h"
#include "qdp_$lib_internal.h"
#include "com_common.h"
#include "com_common_internal.h"

#define fvdp QLA$PC_$ABBR3_veq_$ABBR1$ADJ1_times_p$ABBR2$ADJ2
#define fxdp QLA$PC_$ABBR3_xeq_$ABBR1$ADJ1_times_p$ABBR2$ADJ2
#define fvpp QLA$PC_$ABBR3_veq_p$ABBR1$ADJ1_times_p$ABBR2$ADJ2
#define fxpp QLA$PC_$ABBR3_xeq_p$ABBR1$ADJ1_times_p$ABBR2$ADJ2

void
QDP$PC_$ABBR3_$EQOP_$ABBR1$ADJ1_times_s$ABBR2$ADJ2(
  $NC$QDPPCTYPE3 *__restrict__ dest,
  $QDPPCTYPE1 *src1,
  $QDPPCTYPE2 *src2,
  QDP_Shift shift,
  QDP_ShiftDir fb,
  QDP_Subset subset)
{
  char **temp2;
  QDP_msg_tag *mtag;

  temp2 = (char **)malloc(QDP_sites_on_node*sizeof(char *));

  if((fb!=QDP_forward)&&(fb!=QDP_backward)) {
    fprintf(stderr,"QDP: error: bad fb in QDP$PC_$ABBR_eq_s$ABBR\n");
    QDP_abort();
  }

  /* prepare shift source */
  if(src2->ptr==NULL) {
    if(src2->data==NULL) {
      fprintf(stderr,"error: shifting from uninitialized source\n");
      QDP_abort();
    }
  } else {
    QDP_switch_ptr_to_data(&src2->dc);
  }

  mtag = QDP_declare_shift( temp2, (char *)src2->data, sizeof($QLAPCTYPE2),
			    shift, fb, subset );
  QDP_do_gather(mtag);
  QDP_prepare_dest(&dest->dc);
  QDP_prepare_src(&src1->dc);
  QDP_wait_gather(mtag);

#define SRC2 (($QLAPCTYPE2 **)temp2)
#define SRC1D src1->data
#define SRC1P src1->ptr
  if(src1->ptr==NULL) {
    if(subset->indexed==0) {
      fvdp($NCVAR dest->data+subset->offset, SRC1D+subset->offset, SRC2+subset->offset, subset->len );
    } else {
      fxdp($NCVAR dest->data, SRC1D, SRC2, subset->index, subset->len );
    }
  } else {
    if(subset->indexed==0) {
      fvpp($NCVAR dest->data+subset->offset, SRC1P+subset->offset, SRC2+subset->offset, subset->len );
    } else {
      fxpp($NCVAR dest->data, SRC1P, SRC2, subset->index, subset->len );
    }
  }

  QDP_cleanup_gather(mtag);
  free((void*)temp2);
}
!END
