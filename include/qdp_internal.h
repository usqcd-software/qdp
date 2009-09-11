#ifndef _QDP_INTERNAL_H
#define _QDP_INTERNAL_H

#include <qmp.h>
#include "qdp_common.h"
#include "qdp_layout_internal.h"
#include "qdp_subset_internal.h"
#include "qdp_shift_internal.h"
#include "com_common.h"
#include "com_specific.h"

#ifdef __cplusplus
extern "C" {
#endif

#define QDP_error(...) \
  fprintf(stderr, "error on %i/%i: %s %s %i: ", QDP_this_node, QMP_get_number_of_nodes(), __FILE__, __func__, __LINE__); \
  fprintf(stderr, __VA_ARGS__); \
  fprintf(stderr, "\n");

#define QDP_malloc(var, type, count) \
  var = (type *) malloc(sizeof(type)*(count));	\
  if(!(var)) {						\
    QDP_error("malloc failed"); \
    QDP_abort(1); \
  }

#define QDP_free(var) \
  if(var) free(var);

#ifdef QDP_NDEBUG
#define QDP_assert(expr) ((void) 0)
#else
#define QDP_assert(expr) \
  if(!(expr)) { \
    QDP_error("assertion failed: %s", #expr); \
    QDP_abort(1); \
  }
#endif

#define TRACE
  //#define TRACE if(QDP_this_node==0) printf("%s %s %i\n", __FILE__, __func__, __LINE__);

#ifdef __cplusplus
}
#endif

#endif
