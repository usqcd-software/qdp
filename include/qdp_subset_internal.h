#ifndef _QDP_SUBSET_INTERNAL_H
#define _QDP_SUBSET_INTERNAL_H

struct QDP_Subset_struct {
  int indexed;
  int offset;
  int *index;
  int len;
  int (*func)(QDP_Lattice *lat, int x[], void *args);
  void *args;
  int colors;
  int coloring;
  QDP_Lattice *lattice;
  struct QDP_Subset_struct *first;
};

extern void QDP_make_subsets(void);

#endif
