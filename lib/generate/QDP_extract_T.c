#include "qdp_$lib_internal.h"
#ifdef _OPENMP
#include <omp.h>
#endif

#define OFF(x,o,s) ((void *)(((char *)(x))+(o)*(s)))

void
QDP$PC_extract_$ABBR(void *dest, $QDPPCTYPE *src, QDP_Subset subset)
{
  TGET;
  ONE {
    QDP_prepare_src(&src->dc);

    if(src->ptr==NULL) {
      if(subset->indexed==0) {
#ifdef _OPENMP
#pragma omp parallel
#endif
	{
	  char *from = QDP_offset_data(src,subset->offset);
	  char *to = OFF(dest,subset->offset,src->dc.size);
	  size_t bytes = src->dc.size*subset->len;
#ifdef _OPENMP
	  int nid = omp_get_num_threads();
	  int tid = omp_get_thread_num();
	  size_t offset = ((tid*bytes)/nid);
	  size_t end = (((tid+1)*bytes)/nid);
	  from += offset;
	  to += offset;
	  bytes = end - offset;
#endif
	  memcpy(to, from, bytes);
	}
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
  TBARRIER;
}

void
QDP$PC_insert_$ABBR($QDPPCTYPE *dest, void *src, QDP_Subset subset)
{
  TGET;
  ONE {
    QDP_prepare_dest(&dest->dc);

    if(subset->indexed==0) {
#ifdef _OPENMP
#pragma omp parallel
#endif
	{
	  char *from = OFF(src,subset->offset,dest->dc.size);
	  char *to = QDP_offset_data(dest,subset->offset);
	  size_t bytes = dest->dc.size*subset->len;
#ifdef _OPENMP
	  int nid = omp_get_num_threads();
	  int tid = omp_get_thread_num();
	  size_t offset = ((tid*bytes)/nid);
	  size_t end = (((tid+1)*bytes)/nid);
	  from += offset;
	  to += offset;
	  bytes = end - offset;
#endif
	  memcpy(to, from, bytes);
	}
    } else {
      int i,j;
      for(i=0; i<subset->len; ++i) {
	j = subset->index[i];
	memcpy(QDP_offset_data(dest,j), OFF(src,j,dest->dc.size), dest->dc.size);
      }
    }
  }
  TBARRIER;
}
