#include "qdp_$lib_internal.h"

#define OFF(x,o,s) ((void *)(((char *)(x))+(o)*(s)))

void
QDP$PC_extract_$ABBR(void *dest, $QDPPCTYPE *src, QDP_Subset subset)
{
  QDP_prepare_src(&src->dc);

  if(src->ptr==NULL) {
    if(subset->indexed==0) {
      memcpy(OFF(dest,subset->offset,src->dc.size), QDP_offset_data(src,subset->offset), src->dc.size*subset->len);
    } else {
      int i,j;
      for(i=0; i<subset->len; ++i) {
	j = subset->index[i];
	memcpy(OFF(dest,j,src->dc.size), QDP_offset_data(src,j), src->dc.size);
      }
    }
  } else {
    if(subset->indexed==0) {
      int i,last;
      last = subset->offset + subset->len;
      for(i=subset->offset; i<last; i++) {
	memcpy(OFF(dest,i,src->dc.size), QDP_offset_ptr(src,i), src->dc.size);
      }
    } else {
      int i,j;
      for(i=0; i<subset->len; ++i) {
	j = subset->index[i];
	memcpy(OFF(dest,j,src->dc.size), QDP_offset_ptr(src,j), src->dc.size);
      }
    }
  }
}

void
QDP$PC_insert_$ABBR($QDPPCTYPE *dest, void *src, QDP_Subset subset)
{
  QDP_prepare_dest(&dest->dc);

  if(subset->indexed==0) {
    memcpy(QDP_offset_data(dest,subset->offset), OFF(src,subset->offset,dest->dc.size), dest->dc.size*subset->len);
  } else {
    int i,j;
    for(i=0; i<subset->len; ++i) {
      j = subset->index[i];
      memcpy(QDP_offset_data(dest,j), OFF(src,j,dest->dc.size), dest->dc.size);
    }
  }
}
