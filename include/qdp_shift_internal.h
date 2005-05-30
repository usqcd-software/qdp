#ifndef _QDP_SHIFT_INTERNAL_H
#define _QDP_SHIFT_INTERNAL_H

#include "com_common.h"

struct QDP_Shift_struct {
  int *disp;
  void (*func)(int sx[], int rx[], QDP_ShiftDir fb, void *args);
  void *args;
  int argsize;
  QDP_gather *gather;
};

extern void QDP_make_shifts(void);

#endif
