#include "qdp_$lib_internal.h"

#define OFF(x,o,s) ((void *)(((char *)(x))+(o)*(s)))

void
QDP$PC_extract_packed_$ABBR(void *dest, $QDPPCTYPE *src, QDP_Subset subset)
{
  TGET;
  ONE {
    QDP_prepare_src(&src->dc);

    if(src->ptr==NULL) {
      if(subset->indexed==0) {
	memcpy(dest, QDP_offset_data(src,subset->offset), src->dc.size*subset->len);
      } else {
	int i;
	for(i=0; i<subset->len; i++) {
	  int j = subset->index[i];
	  memcpy(OFF(dest,i,src->dc.size), QDP_offset_data(src,j), src->dc.size);
	}
      }
    } else {
      if(subset->indexed==0) {
	int i;
	for(i=0; i<subset->len; i++) {
	  int j = subset->offset + i;
	  memcpy(OFF(dest,i,src->dc.size), QDP_offset_ptr(src,j), src->dc.size);
	}
      } else {
	int i;
	for(i=0; i<subset->len; i++) {
	  int j = subset->index[i];
	  memcpy(OFF(dest,i,src->dc.size), QDP_offset_ptr(src,j), src->dc.size);
	}
      }
    }
  }
  TBARRIER;
}

void
QDP$PC_insert_packed_$ABBR($QDPPCTYPE *dest, void *src, QDP_Subset subset)
{
  TGET;
  ONE {
    QDP_prepare_dest(&dest->dc);

    if(subset->indexed==0) {
      memcpy(QDP_offset_data(dest,subset->offset), src, dest->dc.size*subset->len);
    } else {
      int i;
      for(i=0; i<subset->len; i++) {
	int j = subset->index[i];
	memcpy(QDP_offset_data(dest,j), OFF(src,i,dest->dc.size), dest->dc.size);
      }
    }
  }
  TBARRIER;
}
