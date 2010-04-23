#ifndef _QDP_TYPES
#define _QDP_TYPES

#include <qmp.h>
#include <qdp_layout.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct QDP_prof_t {
    int count;
    double time;
    double comm_time;
    double math_time;
    double nsites;
    const char *func;
    const char *caller;
    int line;
    struct QDP_prof_t *next;
  } QDP_prof;

  typedef struct QDP_shift_src_t QDP_shift_src_t;
  typedef struct QDP_shift_dest_t QDP_shift_dest_t;
  typedef struct QDP_data_common_t {
    char **data;
    char ***ptr;
    QMP_mem_t *qmpmem;
    size_t size;
    int discarded;
    int exposed;
    QDP_shift_src_t *shift_src;
    QDP_shift_dest_t *shift_dest;
    int nc;
    QDP_Lattice *lat;
  } QDP_data_common_t;
#define get_lat(x) ((x)->dc.lat)
#define QDP_offset_ptr(f,o) ((void *)(((void **)((f)->ptr))+(o)))
#define QDP_offset_data(f,o) ((void *)(((char *)((f)->data))+(o)*((f)->dc.size)))
#define QDP_get_nc(x) ((x)->dc.nc)

!ALLTYPES
struct $QDPPCTYPE_struct {
  //  $QLAPCTYPE *data;
  //$QLAPCTYPE **ptr;
  void *data;
  void **ptr;
  QDP_data_common_t dc;
};

  typedef struct $QDPPCTYPE_struct $QDPPCTYPE;
!END

extern void QDP_prepare_dest(QDP_data_common_t *dc);
extern void QDP_prepare_src(QDP_data_common_t *dc);

/* site access */

!ALLTYPES
static inline void *
QDP$PC_site_ptr_readonly_$ABBR($QDPPCTYPE *src, int i) {
  QDP_prepare_src(&src->dc);
  return src->ptr==NULL ? ((void *)(((char *)(src->data))+i*(src->dc.size))) : (src->ptr[i]);
} 
!END

!ALLTYPES
static inline void *
QDP$PC_site_ptr_readwrite_$ABBR($QDPPCTYPE *src, int i) {
  QDP_prepare_dest(&src->dc);
  //return src->ptr==NULL ? ((void *)(((char *)(src->data))+i*(src->dc.size))) : (src->ptr[i]);
  return ((void *)(((char *)(src->data))+i*(src->dc.size)));
} 
!END

#ifdef __cplusplus
}
#endif

#endif
