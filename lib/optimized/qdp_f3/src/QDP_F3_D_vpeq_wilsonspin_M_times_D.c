/*
 *  Type conversion and component extraction and insertion
 *
 *  Matrix multiply and Wilson spin multiplication
 */

#include "qdp_f3_internal.h"

void
QDP_F3_D_vpeq_wilsonspin_M_times_D( QDP_F3_DiracFermion *__restrict__ dest[], QDP_F3_ColorMatrix *src1[], QDP_F3_DiracFermion *src2[], int dir[], int sign[], QDP_Subset subset, int nv )
{
  int i, offset, blen;
  //for(i=0; i<nv; ++i) {
  //  QDP_prepare_dest(&dest[i]->dc);
  //  QDP_prepare_src(&src1[i]->dc);
  //  QDP_prepare_src(&src2[i]->dc);
  //}

  offset = 0;
  blen = QDP_block_size;
  while(1) {
    if( blen > subset->len - offset ) blen = subset->len - offset;
    if( blen <= 0) break;
    for(i=0; i<nv; ++i) {
    if(offset==0) {
      QDP_prepare_dest(&dest[i]->dc);
      QDP_prepare_src(&src1[i]->dc);
      QDP_prepare_src(&src2[i]->dc);
    }
    if( subset->indexed ) {
      if( src1[i]->ptr ) {
        if( src2[i]->ptr ) {
          //QDP_math_time -= QDP_time();
          QLA_F3_D_xpeq_wilsonspin_pM_times_pD( dest[i]->data, src1[i]->ptr, src2[i]->ptr, dir[i], sign[i], subset->index+offset, blen );
          //QDP_math_time += QDP_time();
        } else {
          //QDP_math_time -= QDP_time();
          QLA_F3_D_xpeq_wilsonspin_pM_times_D( dest[i]->data, src1[i]->ptr, src2[i]->data, dir[i], sign[i], subset->index+offset, blen );
          //QDP_math_time += QDP_time();
        }
      } else {
        if( src2[i]->ptr ) {
          //QDP_math_time -= QDP_time();
          QLA_F3_D_xpeq_wilsonspin_M_times_pD( dest[i]->data, src1[i]->data, src2[i]->ptr, dir[i], sign[i], subset->index+offset, blen );
          //QDP_math_time += QDP_time();
        } else {
          //QDP_math_time -= QDP_time();
          QLA_F3_D_xpeq_wilsonspin_M_times_D( dest[i]->data, src1[i]->data, src2[i]->data, dir[i], sign[i], subset->index+offset, blen );
          //QDP_math_time += QDP_time();
        }
      }
    } else {
      if( src1[i]->ptr ) {
        if( src2[i]->ptr ) {
          //QDP_math_time -= QDP_time();
          QLA_F3_D_vpeq_wilsonspin_pM_times_pD( dest[i]->data+subset->offset+offset, src1[i]->ptr+subset->offset+offset, src2[i]->ptr+subset->offset+offset, dir[i], sign[i], blen );
          //QDP_math_time += QDP_time();
        } else {
          //QDP_math_time -= QDP_time();
          QLA_F3_D_vpeq_wilsonspin_pM_times_D( dest[i]->data+subset->offset+offset, src1[i]->ptr+subset->offset+offset, src2[i]->data+subset->offset+offset, dir[i], sign[i], blen );
          //QDP_math_time += QDP_time();
        }
      } else {
        if( src2[i]->ptr ) {
          //QDP_math_time -= QDP_time();
          QLA_F3_D_vpeq_wilsonspin_M_times_pD( dest[i]->data+subset->offset+offset, src1[i]->data+subset->offset+offset, src2[i]->ptr+subset->offset+offset, dir[i], sign[i], blen );
          //QDP_math_time += QDP_time();
        } else {
          //QDP_math_time -= QDP_time();
          QLA_F3_D_vpeq_wilsonspin_M_times_D( dest[i]->data+subset->offset+offset, src1[i]->data+subset->offset+offset, src2[i]->data+subset->offset+offset, dir[i], sign[i], blen );
          //QDP_math_time += QDP_time();
        }
      }
    }
    }
    offset += blen;
  }
}
