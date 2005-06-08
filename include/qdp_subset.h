#ifndef _QDP_SUBSET_H
#define _QDP_SUBSET_H

typedef struct QDP_Subset_struct * QDP_Subset;

extern QDP_Subset QDP_all;
extern QDP_Subset *QDP_even_and_odd;
extern QDP_Subset QDP_even;
extern QDP_Subset QDP_odd;

extern QDP_Subset *QDP_create_subset(int (*func)(int x[], void *args), void *args, int argsize, int n);
extern void QDP_destroy_subset(QDP_Subset *s);
extern int QDP_subset_len(QDP_Subset s);

#endif
