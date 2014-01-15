#include <stdlib.h>
#include <string.h>
#include "qdp_internal.h"

QDP_Subset QDP_all;
QDP_Subset QDP_empty;
QDP_Subset *QDP_all_and_empty = NULL;
QDP_Subset QDP_even;
QDP_Subset QDP_odd;
QDP_Subset *QDP_even_and_odd = NULL;
static int subsetId = 0;

static int
QDP_all_func(QDP_Lattice *lat, int x[], void *args)
{
  return 0;
}

static int
QDP_even_and_odd_func(QDP_Lattice *lat, int x[], void *args)
{
  int i, r=0, nd=QDP_ndim_L(lat);
  for(i=0; i<nd; ++i) r += x[i];
  return r&1;
}

/*
 *  Subset functions
 */

QDP_Subset *
QDP1_create_subset_L(QDP_Lattice *lat,
		     int (*func)(QDP_Lattice *lat, int x[], void *args),
		     void *args, int argsize, int n)
{
  int i, c, *x;
  QDP_Subset *ptr, obj;

  x = (int *) malloc(QDP_ndim_L(lat)*sizeof(int));

  obj = (QDP_Subset) malloc(n*sizeof(struct QDP_Subset_struct));
  ptr = (QDP_Subset *) malloc(n*sizeof(QDP_Subset));

  for(i=0; i<n; ++i) {
    ptr[i] = &obj[i];
    obj[i].func = func;
    if(argsize>0) {
      obj[i].args = malloc(argsize);
      memcpy(obj[i].args, args, argsize);
    } else {
      obj[i].args = NULL;
    }
    obj[i].colors = n;
    obj[i].coloring = i;
    obj[i].first = ptr[0];
    obj[i].offset = 0;
    obj[i].len = 0;
    obj[i].indexed = 0;
    obj[i].lattice = lat;
    obj[i].id = subsetId;
    subsetId++;
  }

  for(i=0; i<QDP_sites_on_node_L(lat); ++i) {
    QDP_get_coords_L(lat, x, QDP_this_node, i);
    c = func(lat, x, args);
    if((c>=0)&&(c<n)) {
      if(obj[c].len==0) {
	obj[c].offset = i;
      } else {
	if(i!=obj[c].offset+obj[c].len) {
	  obj[c].indexed = 1;
	}
      }
      obj[c].len++;
    }
  }

  for(i=0; i<n; ++i) {
    if(obj[i].indexed) {
      obj[i].index = (int *)malloc(obj[i].len*sizeof(int));
      obj[i].len = 0;
    } else {
      obj[i].index = NULL;
    }
  }

  for(i=0; i<QDP_sites_on_node_L(lat); ++i) {
    QDP_get_coords_L(lat, x, QDP_this_node, i);
    c = func(lat, x, args);
    if((c>=0)&&(c<n)) {
      if(obj[c].indexed) {
	obj[c].index[obj[c].len] = i;
	obj[c].len++;
      }
    }
  }

  free(x);
  return ptr;
}

QDP_Subset
QDP_all_L(QDP_Lattice *lat)
{
  if(!lat->all) {
    TGET;
    ONE {
      lat->all = QDP1_create_subset_L(lat, QDP_all_func, NULL, 0, 2);
    }
    TBARRIER;
  }
  return lat->all[0];
}

QDP_Subset
QDP_empty_L(QDP_Lattice *lat)
{
  if(!lat->all) {
    TGET;
    ONE {
      lat->all = QDP1_create_subset_L(lat, QDP_all_func, NULL, 0, 2);
    }
    TBARRIER;
  }
  return lat->all[1];
}

QDP_Subset *
QDP_all_and_empty_L(QDP_Lattice *lat)
{
  if(!lat->all) {
    TGET;
    ONE {
      lat->all = QDP1_create_subset_L(lat, QDP_all_func, NULL, 0, 2);
    }
    TBARRIER;
  }
  return lat->all;
}

QDP_Subset
QDP_even_L(QDP_Lattice *lat)
{
  if(!lat->eo) {
    TGET;
    ONE {
      lat->eo = QDP1_create_subset_L(lat, QDP_even_and_odd_func, NULL, 0, 2);
    }
    TBARRIER;
  }
  return lat->eo[0];
}

QDP_Subset
QDP_odd_L(QDP_Lattice *lat)
{
  if(!lat->eo) {
    TGET;
    ONE {
      lat->eo = QDP1_create_subset_L(lat, QDP_even_and_odd_func, NULL, 0, 2);
    }
    TBARRIER;
  }
  return lat->eo[1];
}

QDP_Subset *
QDP_even_and_odd_L(QDP_Lattice *lat)
{
  if(!lat->eo) {
    TGET;
    ONE {
      lat->eo = QDP1_create_subset_L(lat, QDP_even_and_odd_func, NULL, 0, 2);
    }
    TBARRIER;
  }
  return lat->eo;
}

typedef struct {
  int (*func)(int x[], void *args);
  char args[];
} func_dat;

static int
func_L(QDP_Lattice *lat, int x[], void *args)
{
  func_dat *fd = (func_dat *) args;
  return fd->func(x, fd->args);
}

QDP_Subset *
QDP_create_subset(int (*func)(int x[], void *args), void *args, int argsize, int n)
{
  // for now, we only use the data from thread 0
  QDP_Subset *ret;
  TGET;
  ONE {
    QDP_Lattice *lat = QDP_get_default_lattice();
    int fdsize = sizeof(func_dat) + argsize;
    func_dat *fd = (func_dat *) malloc(fdsize);
    fd->func = func;
    memcpy(fd->args, args, argsize);
    ret = QDP_create_subset_L(lat, func_L, fd, fdsize, n);
    free(fd);
  } else {
    ret = QDP_create_subset_L(NULL, NULL, NULL, 0, n);
  }
  return ret;
}

QDP_Subset *
QDP_create_subset_L(QDP_Lattice *lat,
		    int (*func)(QDP_Lattice *lat, int x[], void *args),
		    void *args, int argsize, int n)
{
  QDP_Subset *ptr;
  TGET;
  ONE {
    ptr = QDP1_create_subset_L(lat, func, args, argsize, n);
    SHARE_SET(ptr);
    TBARRIER;
  } else {
    TBARRIER;
    SHARE_GET(ptr);
  }
  TBARRIER;
  return ptr;
}

void
QDP_destroy_subset(QDP_Subset *s)
{
  TGET;
  ONE {
    int i, n;
    QDP_Subset obj;

    obj = s[0]->first;
    n = obj[0].colors;
    for(i=0; i<n; ++i) {
      if(obj[i].args) free(obj[i].args);
      if(obj[i].indexed) free(obj[i].index);
    }
    free(obj);
    free((void*)s);
  }
}

int
QDP_subset_len(QDP_Subset s)
{
  return s->len;
}

QDP_Lattice *
QDP_subset_lattice(QDP_Subset s)
{
  return s->lattice;
}
