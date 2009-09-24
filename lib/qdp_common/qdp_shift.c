#include <stdlib.h>
#include <string.h>
#include "qdp_internal.h"

QDP_Shift *QDP_neighbor = NULL;

static void
make_shifts_L(QDP_Lattice *lat)
{
  TRACE;
  if(!lat->neighbor) {
    int ndim = QDP_ndim_L(lat);
    lat->neighbor = (QDP_Shift *) malloc(ndim*sizeof(QDP_Shift));
    int disp[ndim];
    for(int i=0; i<ndim; ++i) disp[i] = 0;
    for(int i=0; i<ndim; ++i) {
      disp[i] = 1;
      lat->neighbor[i] = QDP_create_shift_L(lat, disp);
      disp[i] = 0;
    }
  }
  TRACE;
}

QDP_Shift *
QDP_neighbor_L(QDP_Lattice *lat)
{
  if(!lat->neighbor) make_shifts_L(lat);
  return lat->neighbor;
}

QDP_Shift
QDP_create_shift_L(QDP_Lattice *lat, int disp[])
{
  QDP_Shift shift;

  TRACE;
  shift = (QDP_Shift) malloc(sizeof(struct QDP_Shift_struct));
  shift->func = NULL;
  shift->args = NULL;
  shift->argsize = 0;
  shift->disp = NULL;
  TRACE;
  shift->gather = QDP_make_gather_shift_L( lat, disp, QDP_WANT_INVERSE );
  TRACE;
  return shift;
}

QDP_Shift
QDP_create_shift(int disp[])
{
  QDP_Lattice *lat = QDP_get_default_lattice();
  return QDP_create_shift_L(lat, disp);
}

QDP_Shift
QDP_create_map_L(QDP_Lattice *rlat, QDP_Lattice *slat,
		 void (*func)(QDP_Lattice *rlat, QDP_Lattice *slat,
			      int rx[], int sx[], int *num,
			      int idx, QDP_ShiftDir fb, void *args),
		 void *args, int argsize)
{
  QDP_Shift shift;

  shift = (QDP_Shift) malloc(sizeof(struct QDP_Shift_struct));
  shift->func = func;
  shift->args = args;
  shift->argsize = argsize;
  shift->disp = NULL;
  shift->gather = QDP_make_gather_map_L(rlat, slat, func, args, argsize, QDP_WANT_INVERSE);
  return shift;
}

typedef struct {
  void (*func)(int rx[], int sx[], QDP_ShiftDir fb, void *args);  
  char args[];
} func_dat;

static void
func_L(QDP_Lattice *rlat, QDP_Lattice *slat,
       int rx[], int sx[], int *num,
       int idx, QDP_ShiftDir fb, void *args)
{
  func_dat *fd = (func_dat *) args;
  fd->func(rx, sx, fb, fd->args);
}

QDP_Shift
QDP_create_map(void (*func)(int rx[], int sx[], QDP_ShiftDir fb, void *args),
	       void *args, int argsize)
{
  QDP_Lattice *lat = QDP_get_default_lattice();
  int fdsize = sizeof(func_dat) + argsize;
  func_dat *fd = (func_dat *) malloc(fdsize);
  fd->func = func;
  memcpy(fd->args, args, argsize);
  QDP_Shift ret = QDP_create_map_L(lat, lat, func_L, fd, fdsize);
  free(fd);
  return ret;
}

void
QDP_destroy_shift(QDP_Shift s)
{
  QDP_free_gather(s->gather);
  free(s);
}
