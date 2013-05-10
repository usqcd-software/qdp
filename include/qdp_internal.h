#ifndef _QDP_INTERNAL_H
#define _QDP_INTERNAL_H

#include <qmp.h>
#include "qdp_config.h"
#include "qdp_common.h"
#include "qdp_thread.h"
#include "qdp_common_internal.h"
#include "qdp_layout_internal.h"
#include "qdp_subset_internal.h"
#include "qdp_shift_internal.h"
#include "com_common.h"
#include "com_common_internal.h"
#include "com_specific.h"

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

#ifdef DO_TRACE
#define TRACE if(QDP_this_node==0) printf("%s %s %i\n", __FILE__, __func__, __LINE__)
#else
#define TRACE ((void)0)
#endif

#ifdef HAVE_PTHREADS

#define TDECL , QDP_Thread *_thrd
#define TARG _thrd
#define TGET QDP_Thread *_thrd = QDP_thread_get()
#define TNUM (_thrd->info.num)
#define TSIZE (_thrd->info.group_size)
#define ONE if(TNUM==0)
#define TBARRIER QDP_thread_barrier_thread(_thrd)
#define SHARE_SET(x) QDP_THREAD_SHARE(_thrd) = (x)
#define SHARE_GET(x) (x) = QDP_THREAD_SHARE(_thrd)
#define REDUCE_SET(x) QDP_THREAD_REDUCE(_thrd) = (x)
#define REDUCE_GET(i) QDP_THREAD_REDUCEI(_thrd,i)
#define TSPLITALIGN 2
#define TSPLIT(i0,i1,n) do { \
    i0 = TSPLITALIGN*((TNUM*(n/TSPLITALIGN))/TSIZE);	\
    i1 = (TNUM+1==TSIZE) ? n : TSPLITALIGN*(((TNUM+1)*(n/TSPLITALIGN))/TSIZE); \
  } while(0)

#else

#define TDECL
#define TARG
#define TGET ((void)(0))
#define TNUM 0
#define TSIZE 1
#define ONE if(TNUM==0)
#define TBARRIER ((void)(0))
#define SHARE_SET(x) ((void)(0))
#define SHARE_GET(x) ((void)(0))
#define REDUCE_SET(x) ((void)(0))
#define REDUCE_GET(i) (NULL)
#define TSPLITALIGN 2
#define TSPLIT(i0,i1,n) do { \
    i0 = TSPLITALIGN*((TNUM*(n/TSPLITALIGN))/TSIZE);	\
    i1 = (TNUM+1==TSIZE) ? n : TSPLITALIGN*(((TNUM+1)*(n/TSPLITALIGN))/TSIZE); \
  } while(0)

#endif //HAVE_PTHREADS

#endif
