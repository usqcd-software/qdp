#ifndef _QDP_COMMON_INTERNAL
#define _QDP_COMMON_INTERNAL

#include "qdp_config.h"
#include "qio_string.h"
#include "qla_types.h"
#include "qla_complex.h"
#include "qla_random.h"
#include "qdp_types.h"
#include "qdp_common.h"
#include "qdp_io.h"
#include "com_common.h"
#include "com_specific.h"

#ifdef __cplusplus
extern "C" {
#endif

struct QDP_Reader_struct {
  QIO_Reader *qior;
  QDP_Lattice *lat;
};

struct QDP_Writer_struct {
  QIO_Writer *qiow;
  QDP_Lattice *lat;
};

struct QDP_IO_field {
  char *data;
  int size;
  int nc;
  int ns;
  int word_size;
};

typedef struct QDP_shift_list_t {
  struct QDP_shift_src_t *ss;
  struct QDP_shift_list_t *next;
  struct QDP_shift_list_t *prev;
} QDP_shift_list_t;

typedef struct QDP_shift_tag_t {
  int shift_pending;
  int nref;
  int nv;
  QDP_msg_tag *msgtag;
} QDP_shift_tag_t;

typedef struct QDP_shift_src_t {
  QDP_Shift shift;
  QDP_ShiftDir fb;
  QDP_Subset subset;
  char **ptr;
  QDP_shift_tag_t *st;
  QDP_shift_list_t *sl;
  struct QDP_data_common_t *dc;
  struct QDP_shift_src_t *next;
} QDP_shift_src_t;

typedef struct QDP_shift_dest_t {
  struct QDP_data_common_t *dc;
  struct QDP_shift_dest_t *next;
} QDP_shift_dest_t;

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

!ALLTYPES
struct $QDPPCTYPE_struct {
  //  $QLAPCTYPE *data;
  //$QLAPCTYPE **ptr;
  void *data;
  void **ptr;
  QDP_data_common_t dc;
};

!END

#define QDP_offset_ptr(f,o) ((void *)(((void **)((f)->ptr))+(o)))
#define QDP_offset_data(f,o) ((void *)(((char *)((f)->data))+(o)*((f)->dc.size)))
#define QDP_get_nc(x) ((x)->dc.nc)

extern int QDP_suspended;
extern int QDP_block_size;
extern int QDP_mem_align;
extern int QDP_mem_flags;

extern QDP_msg_tag *QDP_declare_shift(char **dest, char *src, int size, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
extern void QDP_declare_accumulate_shift(QDP_msg_tag **mt, char **dest, char *src, int size, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
extern void QDP_binary_reduce(void func(), int size, void *data);
extern void QDP_binary_reduce_multi(void func(), int size, void *data, int ns);
extern void QDP_N_binary_reduce(int nc, void func(), int size, void *data);
extern void QDP_N_binary_reduce_multi(int nc, void func(), int size, void *data, int ns);

extern struct QDP_shift_tag_t *QDP_alloc_shift_tag(int nv);
extern void QDP_remove_shift_tag_reference(struct QDP_shift_tag_t *st);
extern void QDP_clear_shift_list(void);
extern void QDP_prepare_destroy(QDP_data_common_t *dc);
extern void QDP_prepare_dest(QDP_data_common_t *dc);
extern void QDP_prepare_src(QDP_data_common_t *dc);
extern int QDP_prepare_shift(QDP_data_common_t *dest_dc, QDP_data_common_t *src_dc, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
extern void QDP_switch_ptr_to_data(QDP_data_common_t *dc);

extern int QDP_read_check(QDP_Reader *qdpr, QDP_String *md, int globaldata,
	     void (* put)(char *buf, size_t index, int count, void *qfin),
	     struct QDP_IO_field *qf, int count, QIO_RecordInfo *cmp_info);
extern int QDP_write_check(QDP_Writer *qdpw, QDP_String *md, int globaldata,
	     void (*get)(char *buf, size_t index, int count, void *qfin),
	     struct QDP_IO_field *qf, int count, QIO_RecordInfo *rec_info);

#define QDPIO_nc_S(x) 0
#define QDPIO_ns_S(x) 0
#define QDPIO_size_S(p, nc, ns) sizeof(QLA_RandomState)
#define QDPIO_word_S(p) sizeof(QLA_Int)
#define QDPIO_get_S(NC, p, pc, buf, s, nc, ns)	\
  memcpy(buf, s, sizeof(QLA_RandomState))
#define QDPIO_put_S(NC, p, pc, s, buf, nc, ns)	\
  memcpy(s, buf, sizeof(QLA_RandomState))

#define QDPIO_nc_I(x) 0
#define QDPIO_ns_I(x) 0
#define QDPIO_size_I(p, nc, ns) sizeof(QLA_Int)
#define QDPIO_word_I(p) sizeof(QLA_Int)
#define QDPIO_get_I(NC, p, pc, buf, s, nc, ns) *((QLA_Int*)(buf)) = *(s)
#define QDPIO_put_I(NC, p, pc, s, buf, nc, ns) *(s) = *((QLA_Int*)(buf))

#define QDPIO_nc_R(x) 0
#define QDPIO_ns_R(x) 0
#define QDPIO_size_R(p, nc, ns) sizeof(QLA_##p##_Real)
#define QDPIO_word_R(p) sizeof(QLA_##p##_Real)
#define QDPIO_get_R(NC, p, pc, buf, s, nc, ns) *((QLA_##p##_Real*)(buf)) = *(s)
#define QDPIO_put_R(NC, p, pc, s, buf, nc, ns) *(s) = *((QLA_##p##_Real*)(buf))

#define QDPIO_nc_C(x) 0
#define QDPIO_ns_C(x) 0
#define QDPIO_size_C(p, nc, ns) sizeof(QLA_##p##_Complex)
#define QDPIO_word_C(p) sizeof(QLA_##p##_Real)
#define QDPIO_get_C(NC, p, pc, buf, s, nc, ns) *((QLA_##p##_Complex*)(buf)) = *(s)
#define QDPIO_put_C(NC, p, pc, s, buf, nc, ns) *(s) = *((QLA_##p##_Complex*)(buf))

#define QDPIO_nc_V(x) (x)
#define QDPIO_ns_V(x) 0
#define QDPIO_size_V(p, nc, ns) ((nc)*sizeof(QLA_##p##_Complex))
#define QDPIO_word_V(p) sizeof(QLA_##p##_Real)
#define QDPIO_get_V(NC, p, pc, buf, s, nc, ns) {			\
    int _ic;								\
    for(_ic=0; _ic<(nc); ++_ic) {					\
      QLA##pc##_C_eq_elem_V(NC ((QLA_##p##_Complex *)(buf))+_ic, s, _ic); \
    }									\
  }
#define QDPIO_put_V(NC, p, pc, s, buf, nc, ns) {			\
    int _ic;								\
    for(_ic=0; _ic<(nc); ++_ic) {					\
      QLA##pc##_V_eq_elem_C(NC s, ((QLA_##p##_Complex *)(buf))+_ic, _ic); \
    }									\
  }

#define QDPIO_nc_H(x) (x)
#define QDPIO_ns_H(x) ((x)/2)
#define QDPIO_size_H(p, nc, ns) ((ns/2)*(nc)*sizeof(QLA_##p##_Complex))
#define QDPIO_word_H(p) sizeof(QLA_##p##_Real)
#define QDPIO_get_H(NC, p, pc, buf, s, nc, ns) {			\
    int _is, _ic;							\
    for(_is=0; _is<(ns); ++_is) {					\
      for(_ic=0; _ic<(nc); ++_ic) {					\
	QLA##pc##_C_eq_elem_H(NC ((QLA_##p##_Complex *)(buf))+(nc)*_is+_ic, s, _ic, _is); \
      }									\
    }									\
  }
#define QDPIO_put_H(NC, p, pc, s, buf, nc, ns) {			\
    int _is, _ic;							\
    for(_is=0; _is<(ns); ++_is) {					\
      for(_ic=0; _ic<(nc); ++_ic) {					\
	QLA##pc##_H_eq_elem_C(NC s, ((QLA_##p##_Complex *)(buf))+(nc)*_is+_ic, _ic, _is); \
      }									\
    }									\
  }

#define QDPIO_nc_D(x) (x)
#define QDPIO_ns_D(x) (x)
#define QDPIO_size_D(p, nc, ns) ((ns)*(nc)*sizeof(QLA_##p##_Complex))
#define QDPIO_word_D(p) sizeof(QLA_##p##_Real)
#define QDPIO_get_D(NC, p, pc, buf, s, nc, ns) {			\
    int _is, _ic;							\
    for(_is=0; _is<(ns); ++_is) {					\
      for(_ic=0; _ic<(nc); ++_ic) {					\
	QLA##pc##_C_eq_elem_D(NC ((QLA_##p##_Complex *)(buf))+(nc)*_is+_ic, s, _ic, _is); \
      }									\
    }									\
  }
#define QDPIO_put_D(NC, p, pc, s, buf, nc, ns) {			\
    int _is, _ic;							\
    for(_is=0; _is<(ns); ++_is) {					\
      for(_ic=0; _ic<(nc); ++_ic) {					\
	QLA##pc##_D_eq_elem_C(NC s, ((QLA_##p##_Complex *)(buf))+(nc)*_is+_ic, _ic, _is); \
      }									\
    }									\
  }

#define QDPIO_nc_M(x) (x)
#define QDPIO_ns_M(x) 0
#define QDPIO_size_M(p, nc, ns) ((nc)*(nc)*sizeof(QLA_##p##_Complex))
#define QDPIO_word_M(p) sizeof(QLA_##p##_Real)
#define QDPIO_get_M(NC, p, pc, buf, s, nc, ns) {	\
    int _ic, _jc;							\
    for(_ic=0; _ic<(nc); ++_ic) {					\
      for(_jc=0; _jc<(nc); ++_jc) {					\
	QLA##pc##_C_eq_elem_M(NC ((QLA_##p##_Complex *)(buf))+(nc)*_ic+_jc, s, _ic, _jc); \
      }									\
    }									\
  }
#define QDPIO_put_M(NC, p, pc, s, buf, nc, ns) {			\
    int _ic, _jc;							\
    for(_ic=0; _ic<(nc); ++_ic) {					\
      for(_jc=0; _jc<(nc); ++_jc) {					\
	QLA##pc##_M_eq_elem_C(NC s, ((QLA_##p##_Complex *)(buf))+(nc)*_ic+_jc, _ic, _jc); \
      }									\
    }									\
  }

#define QDPIO_nc_P(x) (x)
#define QDPIO_ns_P(x) (x)
#define QDPIO_size_P(p, nc, ns) ((ns)*(ns)*(nc)*(nc)*sizeof(QLA_##p##_Complex))
#define QDPIO_word_P(p) sizeof(QLA_##p##_Real)
#define QDPIO_get_P(NC, p, pc, buf, s, nc, ns) {			\
    int _is, _js, _ic, _jc;						\
    for(_is=0; _is<(ns); ++_is) {					\
      for(_js=0; _js<(ns); ++_js) {					\
	for(_ic=0; _ic<(nc); ++_ic) {					\
	  for(_jc=0; _jc<(nc); ++_jc) {					\
	    QLA##pc##_C_eq_elem_P(NC ((QLA_##p##_Complex *)(buf))+((_is*(ns)+_js)*(nc)+_ic)*(nc)+_jc, s, _ic, _is, _jc, _js); \
	  }								\
	}								\
      }									\
    }									\
  }
#define QDPIO_put_P(NC, p, pc, s, buf, nc, ns) {			\
    int _is, _js, _ic, _jc;						\
    for(_is=0; _is<(ns); ++_is) {					\
      for(_js=0; _js<(ns); ++_js) {					\
	for(_ic=0; _ic<(nc); ++_ic) {					\
	  for(_jc=0; _jc<(nc); ++_jc) {					\
	    QLA##pc##_P_eq_elem_C(NC s, ((QLA_##p##_Complex *)(buf))+((_is*(ns)+_js)*(nc)+_ic)*(nc)+_jc, _ic, _is, _jc, _js); \
	  }								\
	}								\
      }									\
    }									\
  }

#ifdef __cplusplus
}
#endif

#endif
