#ifndef _QDP_SCATTER_H
#define _QDP_SCATTER_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct QDP_Scatter_struct QDP_Scatter;

  extern QDP_Scatter *QDP_create_scatter(QDP_Lattice *dstLat,
					 QDP_Lattice *srcLat,
					 QDP_Int *addr[]); /* [QDP_ndim_L(srcLat) */
  extern void QDP_destroy_scatter(QDP_Scatter *);

#ifdef __cplusplus
}
#endif

#endif /* !defined(_QDP_SCATTER_H) */
