#include "qdp_$lib_internal.h"

void
QDP$PC_extract_$ABBR($NC$QLAPCTYPE *dest, $QDPPCTYPE *src, QDP_Subset subset)
{
  QDP_prepare_src(&src->dc);

  if(src->ptr==NULL) {
    if(subset->indexed==0) {
      memcpy(dest+subset->offset, src->data+subset->offset, src->dc.size*subset->len);
    } else {
      int i,j;
      for(i=0; i<subset->len; ++i) {
	j = subset->index[i];
	memcpy(&dest[j], &src->data[j], src->dc.size);
      }
    }
  } else {
    if(subset->indexed==0) {
      int i,last;
      last = subset->offset + subset->len;
      for(i=subset->offset; i<last; i++) {
	memcpy(&dest[i], src->ptr[i], src->dc.size);
      }
    } else {
      int i,j;
      for(i=0; i<subset->len; ++i) {
	j = subset->index[i];
	memcpy(&dest[j], src->ptr[j], src->dc.size);
      }
    }
  }
}

void
QDP$PC_insert_$ABBR($NC$QDPPCTYPE *dest, $QLAPCTYPE *src, QDP_Subset subset)
{
  QDP_prepare_dest(&dest->dc);

  if(subset->indexed==0) {
    memcpy(dest->data+subset->offset, src+subset->offset, dest->dc.size*subset->len);
  } else {
    int i,j;
    for(i=0; i<subset->len; ++i) {
      j = subset->index[i];
      memcpy(&dest->data[j], &src[j], dest->dc.size);
    }
  }
}
