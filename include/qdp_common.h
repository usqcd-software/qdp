#ifndef _QDP_COMMON
#define _QDP_COMMON

#include "qdp_types.h"
#include "qdp_layout.h"
#include "qdp_subset.h"
#include "qdp_shift.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int QDP_restart;
extern int QDP_prof_level;
extern int QDP_keep_time;
extern double QDP_comm_time;
extern double QDP_math_time;

extern int QDP_initialize(int *argc, char **argv[]);
extern void QDP_finalize(void);
extern void QDP_abort(void);
extern int QDP_initialized(void);

extern int QDP_profcontrol(int new);
extern double QDP_time(void);
extern void QDP_register_prof(QDP_prof *qp);

extern void QDP_suspend_comm(void);
extern void QDP_resume_comm(void);

extern int QDP_get_block_size(void);
extern void QDP_set_block_size(int bs);

#ifdef __cplusplus
}
#endif

#endif
