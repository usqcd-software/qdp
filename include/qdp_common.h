#ifndef _QDP_COMMON
#define _QDP_COMMON

#include "qdp_types.h"
#include "qdp_layout.h"
#include "qdp_subset.h"
#include "qdp_shift.h"

#define QDP_ALIGN_ANY QMP_ALIGN_ANY
#define QDP_ALIGN_DEFAULT QMP_ALIGN_DEFAULT

#define QDP_MEM_NONCACHE QMP_MEM_NONCACHE
#define QDP_MEM_COMMS QMP_MEM_COMMS
#define QDP_MEM_FAST QMP_MEM_FAST
#define QDP_MEM_DEFAULT QMP_MEM_DEFAULT

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
extern void QDP_abort(int status);
extern int QDP_is_initialized(void);

extern int QDP_check_comm(int newval);
extern int QDP_profcontrol(int newval);
extern double QDP_time(void);
extern void QDP_register_prof(QDP_prof *qp);

extern void QDP_suspend_comm(void);
extern void QDP_resume_comm(void);

extern int QDP_get_block_size(void);
extern void QDP_set_block_size(int bs);

extern int QDP_get_mem_align(void);
extern void QDP_set_mem_align(int align);
extern int QDP_get_mem_flags(void);
extern void QDP_set_mem_flags(int flags);

#ifdef __cplusplus
}
#endif

#endif
