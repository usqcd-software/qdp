#ifndef _QDP_SHIFT_INTERNAL_H
#define _QDP_SHIFT_INTERNAL_H

#include "com_common.h"
#include "qdp_shift.h"

#ifdef __cplusplus
extern "C" {
#endif

  struct QDP_Shift_struct {
    int *disp;
    void (*func)(QDP_Lattice *rlat, QDP_Lattice *slat, int rx[], int sx[],
		 int *num, int idx, QDP_ShiftDir fb, void *args);
    void *args;
    int argsize;
    QDP_gather *gather;
  };

  extern void QDP_make_shifts(void);

#ifdef __cplusplus
}
#endif

#endif
