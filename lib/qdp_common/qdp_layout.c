#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "qdp_layout.h"
#include "qdp_subset_internal.h"
#include "qdp_shift.h"
#include "qdp_shift_internal.h"

int QDP_sites_on_node = -1;

static int ndim=-1;
static int *latsize;
static int vol;

extern void QDP_setup_layout(int len[], int nd);

void
QDP_set_latsize(int nd, int size[])
{
  int i;
  ndim = nd;
  latsize = (int *)malloc(ndim*sizeof(int));
  memcpy(latsize, size, ndim*sizeof(int));
  vol = latsize[0];
  for(i=1; i<ndim; ++i) vol *= latsize[i];
}

int
QDP_create_layout(void)
{
  if(QDP_sites_on_node==-1) {
    QDP_setup_layout(latsize, ndim);
  }

  //fprintf(stderr,"QDP_make_shifts\n");
  QDP_make_shifts();
  //fprintf(stderr,"QDP_make_subsets\n");
  QDP_make_subsets();
  //fprintf(stderr,"done\n");

  return 0;
}

/*
 *  Layout Query functions
 */

int
QDP_ndim(void)
{
  return ndim;
}

int
QDP_coord_size(int i)
{
  return latsize[i];
}

void
QDP_latsize(int ls[])
{
  memcpy(ls, latsize, ndim*sizeof(int));
}

int
QDP_volume(void)
{
  return vol;
}
