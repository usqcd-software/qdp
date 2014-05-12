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

#define QDP_time() QMP_time()

  extern int QDP_restart;
  extern int QDP_verbose_level;
  extern int QDP_prof_level;
  extern int QDP_keep_time;
  extern double QDP_comm_time;
  extern double QDP_math_time;

  int QDP_initialize(int *argc, char **argv[]);
  void QDP_finalize(void);
  void QDP_abort(int status);
  int QDP_is_initialized(void);

  const char *QDP_version_str(void);
  int QDP_version_int(void);

  int QDP_verbose(int level);
  int QDP_check_comm(int newval);
  int QDP_profcontrol(int newval);
  //double QDP_time(void);
  void QDP_register_prof(QDP_prof *qp);

  void QDP_suspend_comm(void);
  void QDP_resume_comm(void);

  int QDP_get_block_size(void);
  void QDP_set_block_size(int bs);

  int QDP_get_mem_align(void);
  void QDP_set_mem_align(int align);
  int QDP_get_mem_flags(void);
  void QDP_set_mem_flags(int flags);

#ifdef __cplusplus
}
#endif

#define QDP_PROFX(_QDPfunc, _QDPargs, ...) \
  do { \
    if(QDP_prof_level>0&&QDP_thread_num()==0) { \
      static QDP_prof qp; \
      static int first_time=1; \
      if(first_time) { \
        first_time = 0; \
        qp.count = 0; \
        qp.time = 0; \
        qp.comm_time = 0; \
        qp.math_time = 0; \
        qp.nsites = 0; \
        qp.func = #_QDPfunc; \
        qp.caller = __func__; \
        qp.line = __LINE__; \
        QDP_register_prof(&qp); \
      } \
      QDP_keep_time = 1; \
      QDP_comm_time = 0; \
      QDP_math_time = 0; \
      qp.time -= QDP_time(); \
      _QDPfunc _QDPargs; \
      qp.time += QDP_time(); \
      qp.comm_time += QDP_comm_time; \
      qp.math_time += QDP_math_time; \
      qp.count++; \
      __VA_ARGS__; \
      QDP_keep_time = 0; \
    } else { \
      _QDPfunc _QDPargs; \
    } \
  } while(0)

#define QDP_PROFXX(_QDPfunc, _QDPargs, _QDPsubset) \
  QDP_PROFX(_QDPfunc, _QDPargs, \
	    qp.nsites += QDP_subset_len(_QDPsubset))

#define QDP_PROFXV(_QDPfunc, _QDPargs, _QDPsubset, _QDPnv) \
  QDP_PROFX(_QDPfunc, _QDPargs, \
	    qp.nsites += (_QDPnv)*QDP_subset_len(_QDPsubset))

#define QDP_PROFXM(_QDPfunc, _QDPargs, _QDPsubset, _QDPns) \
  QDP_PROFX(_QDPfunc, _QDPargs, \
            { int _i; for(_i=0; _i<_QDPns; ++_i) \
	      qp.nsites += QDP_subset_len(_QDPsubset[_i]); })

#define QDP_PROFS(_QDPfunc, _QDPargs, ...) \
  do { \
    if(QDP_prof_level>0&&QDP_thread_num()==0) { \
      static QDP_prof qp[2]; \
      static int first_time=1; \
      double _time; \
      if(first_time) { \
        first_time = 0; \
        qp[0].count = 0; \
        qp[0].time = 0; \
        qp[0].comm_time = 0; \
        qp[0].math_time = 0; \
        qp[0].nsites = 0; \
        qp[0].func = #_QDPfunc"(no restart)"; \
        qp[0].caller = __func__; \
        qp[0].line = __LINE__; \
        QDP_register_prof(&qp[0]); \
        qp[1].count = 0; \
        qp[1].time = 0; \
        qp[1].comm_time = 0; \
        qp[1].math_time = 0; \
        qp[1].nsites = 0; \
        qp[1].func = #_QDPfunc"(restart)"; \
        qp[1].caller = __func__; \
        qp[1].line = __LINE__; \
        QDP_register_prof(&qp[1]); \
      } \
      QDP_keep_time = 1; \
      QDP_comm_time = 0; \
      QDP_math_time = 0; \
      QDP_restart = 0; \
      _time = QDP_time(); \
      _QDPfunc _QDPargs; \
      qp[QDP_restart].time += QDP_time() - _time; \
      qp[QDP_restart].comm_time += QDP_comm_time; \
      qp[QDP_restart].math_time += QDP_math_time; \
      qp[QDP_restart].count++; \
      __VA_ARGS__; \
      QDP_keep_time = 0; \
    } else { \
      _QDPfunc _QDPargs; \
    } \
  } while(0)

#define QDP_PROFSX(_QDPfunc, _QDPargs, _QDPsubset) \
  QDP_PROFS(_QDPfunc, _QDPargs, \
	    qp[QDP_restart].nsites += QDP_subset_len(_QDPsubset))

#define QDP_PROFSV(_QDPfunc, _QDPargs, _QDPsubset, _QDPnv) \
  QDP_PROFS(_QDPfunc, _QDPargs, \
	    qp[QDP_restart].nsites += (_QDPnv)*QDP_subset_len(_QDPsubset))

#define QDP_PROFSM(_QDPfunc, _QDPargs, _QDPsubset, _QDPns) \
  QDP_PROFS(_QDPfunc, _QDPargs, \
            { int _i; for(_i=0; _i<_QDPns; ++_i) \
	      qp[QDP_restart].nsites += QDP_subset_len(_QDPsubset[_i]); })

#endif
