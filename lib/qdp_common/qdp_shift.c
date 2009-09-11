#include <stdlib.h>
#include "qdp_internal.h"

QDP_Shift *QDP_neighbor = NULL;

void
QDP_make_shifts()
{
  int i;
  int *disp;

  TRACE;
  QDP_neighbor = (QDP_Shift *) malloc(QDP_ndim()*sizeof(QDP_Shift));
  disp = (int *) malloc(QDP_ndim()*sizeof(int));
  for(i=0; i<QDP_ndim(); ++i) disp[i] = 0;
  for(i=0; i<QDP_ndim(); ++i) {
    disp[i] = 1;
    QDP_neighbor[i] = QDP_create_shift(disp);
    disp[i] = 0;
  }
  free(disp);
  TRACE;
}

QDP_Shift
QDP_create_shift(int disp[])
{
  QDP_Shift shift;

  TRACE;
  shift = (QDP_Shift) malloc(sizeof(struct QDP_Shift_struct));
  shift->func = NULL;
  shift->args = NULL;
  shift->argsize = 0;
  shift->disp = NULL;
  TRACE;
  shift->gather = QDP_make_gather_shift( disp, QDP_WANT_INVERSE );
  TRACE;
  return shift;
}

QDP_Shift
QDP_create_map(void (*func)(int sx[], int rx[], QDP_ShiftDir fb, void *args), void *args, int argsize)
{
  QDP_Shift shift;

  shift = (QDP_Shift) malloc(sizeof(struct QDP_Shift_struct));
  shift->func = func;
  shift->args = args;
  shift->argsize = argsize;
  shift->disp = NULL;
  shift->gather = QDP_make_gather_map(func, args, argsize, QDP_WANT_INVERSE);
  return shift;
}

void
QDP_destroy_shift(QDP_Shift s)
{
  QDP_free_gather(s->gather);
  free(s);
}
