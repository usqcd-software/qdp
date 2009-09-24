#ifndef _QDP_TYPES
#define _QDP_TYPES

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

!ALLTYPES
  typedef struct $QDPPCTYPE_struct $QDPPCTYPE;
!END

#ifdef __cplusplus
}
#endif

#endif
