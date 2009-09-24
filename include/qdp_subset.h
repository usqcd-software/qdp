#ifndef _QDP_SUBSET_H
#define _QDP_SUBSET_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct QDP_Subset_struct * QDP_Subset;

  extern QDP_Subset QDP_all;
  extern QDP_Subset *QDP_even_and_odd;
  extern QDP_Subset QDP_even;
  extern QDP_Subset QDP_odd;

  extern QDP_Subset *QDP_create_subset(int (*func)(int x[], void *args),
				       void *args, int argsize, int n);
  extern void QDP_destroy_subset(QDP_Subset *s);
  extern int QDP_subset_len(QDP_Subset s);

  extern QDP_Subset QDP_all_L(QDP_Lattice *lat);
  extern QDP_Subset *QDP_even_and_odd_L(QDP_Lattice *lat);
  extern QDP_Subset QDP_even_L(QDP_Lattice *lat);
  extern QDP_Subset QDP_odd_L(QDP_Lattice *lat);

  extern QDP_Subset *QDP_create_subset_L(QDP_Lattice *lat,
					 int (*func)(QDP_Lattice *lat, int x[], void *args),
					 void *args, int argsize, int n);
  extern QDP_Lattice *QDP_subset_lattice(QDP_Subset s);

#ifdef __cplusplus
}
#endif

#endif
