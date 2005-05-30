#include <stdlib.h>
#include "qdp_layout.h"
#include "qdp_subset.h"
#include "qdp_subset_internal.h"
#include "qdp_shift.h"

QDP_Subset *QDP_all_array = NULL;
QDP_Subset QDP_all;
QDP_Subset *QDP_even_and_odd = NULL;
QDP_Subset QDP_even;
QDP_Subset QDP_odd;


static int
QDP_all_func(int x[], void *args)
{
  return 0;
}

static int
QDP_even_and_odd_func(int x[], void *args)
{
  int i, r=0, nd=QDP_ndim();
  for(i=0; i<nd; ++i) r += x[i];
  return r&1;
}

void
QDP_make_subsets(void)
{
  QDP_all_array = QDP_create_subset(QDP_all_func, NULL, 1);
  QDP_all = QDP_all_array[0];

  QDP_even_and_odd = QDP_create_subset(QDP_even_and_odd_func, NULL, 2);
  QDP_even = QDP_even_and_odd[0];
  QDP_odd = QDP_even_and_odd[1];
}


/*
 *  Subset functions
 */

QDP_Subset *
QDP_create_subset(int (*func)(int x[], void *args), void *args, int n)
{
  int i, c, *x;
  QDP_Subset obj, *ptr;

  x = (int *) malloc(QDP_ndim()*sizeof(int));

  obj = (QDP_Subset) malloc(n*sizeof(struct QDP_Subset_struct));
  ptr = (QDP_Subset *) malloc(n*sizeof(QDP_Subset));

  for(i=0; i<n; ++i) {
    ptr[i] = &obj[i];
    obj[i].func = func;
    obj[i].args = args;
    obj[i].colors = n;
    obj[i].coloring = i;
    obj[i].first = ptr[0];
    obj[i].offset = 0;
    obj[i].len = 0;
    obj[i].indexed = 0;
  }

  for(i=0; i<QDP_sites_on_node; ++i) {
    QDP_get_coords(x, QDP_this_node, i);
    c = func(x, args);
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

  for(i=0; i<QDP_sites_on_node; ++i) {
    QDP_get_coords(x, QDP_this_node, i);
    c = func(x, args);
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

void
QDP_destroy_subset(QDP_Subset *s)
{
  int i, n;
  QDP_Subset obj;

  obj = s[0]->first;
  n = obj[0].colors;
  for(i=0; i<n; ++i) {
    if(obj[i].indexed) free(obj[i].index);
  }
  free(obj);
  free((void*)s);
}

int
QDP_subset_len(QDP_Subset s)
{
  return s->len;
}
