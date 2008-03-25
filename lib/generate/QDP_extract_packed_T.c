#include "qdp_$lib_internal.h"

void
QDP$PC_extract_packed_$ABBR($NC$QLAPCTYPE *dest, $QDPPCTYPE *src, QDP_Subset subset)
{
  QDP_prepare_src(&src->dc);

  if(src->ptr==NULL) {
    if(subset->indexed==0) {
      memcpy(dest, src->data+subset->offset, src->dc.size*subset->len);
    } else {
      int i;
      for(i=0; i<subset->len; i++) {
	int j = subset->index[i];
	memcpy(&dest[i], &src->data[j], src->dc.size);
      }
    }
  } else {
    if(subset->indexed==0) {
      int i;
      for(i=0; i<subset->len; i++) {
	int j = subset->offset + i;
	memcpy(&dest[i], src->ptr[j], src->dc.size);
      }
    } else {
      int i;
      for(i=0; i<subset->len; i++) {
	int j = subset->index[i];
	memcpy(&dest[i], src->ptr[j], src->dc.size);
      }
    }
  }
}

void
QDP$PC_insert_packed_$ABBR($NC$QDPPCTYPE *dest, $QLAPCTYPE *src, QDP_Subset subset)
{
  QDP_prepare_dest(&dest->dc);

  if(subset->indexed==0) {
    memcpy(dest->data+subset->offset, src, dest->dc.size*subset->len);
  } else {
    int i;
    for(i=0; i<subset->len; i++) {
      int j = subset->index[i];
      memcpy(&dest->data[j], &src[i], dest->dc.size);
    }
  }
}
