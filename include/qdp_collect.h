#ifndef _QDP_COLLECT_H
#define _QDP_COLLECT_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct QDP_Collect_struct QDP_Collect;

  extern QDP_Collect *QDP_create_collect(QDP_Lattice *dstLat,
					 QDP_Lattice *srcLat,
					 QDP_Int *addr[]); /* [QDP_ndim_L(dstLat)] */
  extern void QDP_destroy_collect(QDP_Collect *);

#ifdef __cplusplus
}
#endif

#endif /* !defined(_QDP_COLLECT_H) */
