/*
 *  Binary operations with fields
 *
 *  Left multiplication by color matrix
 */

#include "qdp_f3_internal.h"

void
QDP_F3_V_vpeq_M_times_V( QDP_F3_ColorVector *__restrict__ dest[], QDP_F3_ColorMatrix *src1[], QDP_F3_ColorVector *src2[], QDP_Subset subset, int nv )
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
          QLA_F3_V_xpeq_pM_times_pV( dest[i]->data, src1[i]->ptr, src2[i]->ptr, subset->index+offset, blen );
          //QDP_math_time += QDP_time();
        } else {
          //QDP_math_time -= QDP_time();
          QLA_F3_V_xpeq_pM_times_V( dest[i]->data, src1[i]->ptr, src2[i]->data, subset->index+offset, blen );
          //QDP_math_time += QDP_time();
        }
      } else {
        if( src2[i]->ptr ) {
          //QDP_math_time -= QDP_time();
          QLA_F3_V_xpeq_M_times_pV( dest[i]->data, src1[i]->data, src2[i]->ptr, subset->index+offset, blen );
          //QDP_math_time += QDP_time();
        } else {
          //QDP_math_time -= QDP_time();
          QLA_F3_V_xpeq_M_times_V( dest[i]->data, src1[i]->data, src2[i]->data, subset->index+offset, blen );
          //QDP_math_time += QDP_time();
        }
      }
    } else {
      if( src1[i]->ptr ) {
        if( src2[i]->ptr ) {
          //QDP_math_time -= QDP_time();
          QLA_F3_V_vpeq_pM_times_pV( dest[i]->data+subset->offset+offset, src1[i]->ptr+subset->offset+offset, src2[i]->ptr+subset->offset+offset, blen );
          //QDP_math_time += QDP_time();
        } else {
          //QDP_math_time -= QDP_time();
          QLA_F3_V_vpeq_pM_times_V( dest[i]->data+subset->offset+offset, src1[i]->ptr+subset->offset+offset, src2[i]->data+subset->offset+offset, blen );
          //QDP_math_time += QDP_time();
        }
      } else {
        if( src2[i]->ptr ) {
          //QDP_math_time -= QDP_time();
          QLA_F3_V_vpeq_M_times_pV( dest[i]->data+subset->offset+offset, src1[i]->data+subset->offset+offset, src2[i]->ptr+subset->offset+offset, blen );
          //QDP_math_time += QDP_time();
        } else {
          //QDP_math_time -= QDP_time();
          QLA_F3_V_vpeq_M_times_V( dest[i]->data+subset->offset+offset, src1[i]->data+subset->offset+offset, src2[i]->data+subset->offset+offset, blen );
          //QDP_math_time += QDP_time();
        }
      }
    }
    }
    offset += blen;
  }
}
