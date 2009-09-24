#ifndef _QDP_SHIFT_H
#define _QDP_SHIFT_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct QDP_Shift_struct * QDP_Shift;

  typedef enum {QDP_forward=0, QDP_backward=1} QDP_ShiftDir;

  extern int QDP_this_node;
  extern QDP_Shift *QDP_neighbor;

  extern QDP_Shift QDP_create_shift(int disp[]);
  extern QDP_Shift QDP_create_map(void (*func)(int rx[], int sx[], QDP_ShiftDir fb, void *args), void *args, int argsize);
  extern void QDP_destroy_shift(QDP_Shift s);

  extern QDP_Shift *QDP_neighbor_L(QDP_Lattice *lat);

  extern QDP_Shift QDP_create_shift_L(QDP_Lattice *lat, int disp[]);
  extern QDP_Shift QDP_create_map_L(QDP_Lattice *rlat, QDP_Lattice *slat,
				    void (*func)(QDP_Lattice *rlat, QDP_Lattice *slat,
						 int rx[], int sx[], int *num,
						 int idx, QDP_ShiftDir fb, void *args),
				    void *args, int argsize);

#ifdef __cplusplus
}
#endif

#endif
