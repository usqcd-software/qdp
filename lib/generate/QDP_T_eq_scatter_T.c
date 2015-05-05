!PCARITHTYPES
/*
 * T = scatter(T)
 */
#include "qdp_$lib_internal.h"
#include "qdp_scatter_internal.h"
#include <qmp.h>

/* The scatter() memory footprint is kept O(sublattice). This requires two extra messages per source,
 * but solves the scaling problem.
 */

void
QDP$PC_$ABBR_eq_scatter_$ABBR($QDPPCTYPE *dst,
                              QDP_Scatter *scatter,
                              $QDPPCTYPE *src,
                              QDP_Subset subset)
{
  /* sanity checks */
  QDP_assert(dst != NULL);
  QDP_assert(src != NULL);
  QDP_assert(scatter != NULL);
  QDP_assert(QDP$PC_get_lattice_$ABBR(dst) == scatter->dstLattice);
  QDP_assert(QDP$PC_get_lattice_$ABBR(src) == scatter->srcLattice);
  QDP_assert(QDP_subset_lattice(subset) == scatter->dstLattice);

  /* Just to be sure that no recursion happens */
  QDP_assert(scatter->in_use == 0);
  scatter->in_use = 1;

  {
    /* variables */
    int i, m;
#define N -1
#if (N+0) == -1
    int nc = QDP_get_nc(dst);
#endif
    typedef $QLAPCTYPE($NCVAR(X_Data));
    X_Data *dstPtr = QDP$PC_expose_$ABBR(dst);
    X_Data *srcPtr = QDP$PC_expose_$ABBR(src);
    X_Data *arena = NULL;
    X_Data *buffer = NULL;
    int *index = NULL;
    QMP_msgmem_t *rcvMem = NULL;
    QMP_msghandle_t *rcvHandle = NULL;

    /* check that dst and src have the same number of colors */
#if (N+0) == -1
    QDP_assert(nc == QDP_get_nc(src));
#endif
    /* allocate locals */
    if (scatter->remoteCount > 0) {
      QDP_malloc(rcvMem, QMP_msgmem_t, scatter->remoteCount);
      QDP_malloc(rcvHandle, QMP_msghandle_t, scatter->remoteCount);
      /* send meta messages and read maps */
      for (m = 0; m < scatter->remoteCount; m++) {
	QMP_start(scatter->remoteMetaH[m]);
	QMP_start(scatter->remoteMapH[m]);
      }
    }
    if (scatter->bufferSize > 0) {
      QDP_malloc(buffer, X_Data, scatter->bufferSize);
      QDP_malloc(index, int, scatter->bufferSize);
    }
    if (scatter->arenaSize > 0) {
      QDP_malloc(arena, X_Data, scatter->arenaSize);
    }

    /* copy local data reads */
    for (i = 0; i < scatter->localSize; i++) {
      QLA$PC_$ABBR_eq_$ABBR($NCVAR &arena[i], &srcPtr[scatter->localAddr[i]]);
    }
    /* start receives on data reads */
    for (i = 0; i < scatter->remoteCount; i++) {
      rcvMem[i] = QMP_declare_msgmem(&arena[scatter->remoteData[i].start],
				     scatter->remoteData[i].meta.count * sizeof (X_Data));
      rcvHandle[i] = QMP_declare_receive_from(rcvMem[i], scatter->remoteData[i].dstNode, 0);
      QMP_start(rcvHandle[i]);
    }
    /* loop over all all meta messages we expect to receive */
    for (m = 0; m < scatter->rcvCount; m++) {
      int rNode, rCount;
      QMP_msgmem_t iMem;
      QMP_msghandle_t iHandle;
      QMP_msgmem_t sMem;
      QMP_msghandle_t sHandle;

      /* receive the next meta message */
      QMP_start(scatter->rcvMetaH);
      QMP_wait(scatter->rcvMetaH);
      rNode = scatter->rcvMetaData.node;
      rCount = scatter->rcvMetaData.count;
      QDP_assert(rCount > 0);
      QDP_assert(rCount <= scatter->bufferSize);
      /* receive the next read map */
      iMem = QMP_declare_msgmem(&index[0], rCount * sizeof (int));
      iHandle = QMP_declare_tagged_receive_from(iMem, rNode, 0, 1);
      QMP_start(iHandle);
      QMP_wait(iHandle);
      QMP_free_msghandle(iHandle);
      QMP_free_msgmem(iMem);
      /* form the data to be sent */
      for (i = 0; i < rCount; i++) {
	QDP_assert(index[i] >= 0);
	QDP_assert(index[i] < scatter->srcSize);
	QLA$PC_$ABBR_eq_$ABBR($NCVAR &buffer[i], &srcPtr[index[i]]);
      }
      /* send the data to rNode */
      sMem = QMP_declare_msgmem(&buffer[0], rCount * sizeof (X_Data));
      sHandle = QMP_declare_send_to(sMem, rNode, 0);
      QMP_start(sHandle);
      QMP_wait(sHandle);
      QMP_free_msghandle(sHandle);
      QMP_free_msgmem(sMem);
    }
    /* wait for receives to finish */
    if (scatter->remoteCount)
      QMP_wait_all(rcvHandle, scatter->remoteCount);
    /* copy received data to dst */
    QDP_loop_sites(i, subset, QLA$PC_$ABBR_eq_$ABBR($NCVAR &dstPtr[i], &arena[scatter->dstAddr[i]]));
    /* free receive message and memory handles */
    for (i = 0; i < scatter->remoteCount; i++) {
      /* receives has been already completed */
      QMP_free_msghandle(rcvHandle[i]);
      QMP_free_msgmem(rcvMem[i]);
    }
    /* free locals */
    if (scatter->arenaSize > 0) {
      QDP_free(arena);
    }
    if (scatter->bufferSize > 0) {
      QDP_free(buffer);
      QDP_free(index);
    }

    if (scatter->remoteCount > 0) {
      QDP_free(rcvMem);
      QDP_free(rcvHandle);
      /* wait for send meta messages and read maps to complete*/
      QMP_wait_all(scatter->remoteMapH, scatter->remoteCount);
      QMP_wait_all(scatter->remoteMetaH, scatter->remoteCount);
    }

    /* Reset source and destination */
    QDP$PC_reset_$ABBR(dst);
    QDP$PC_reset_$ABBR(src);
  }
  QMP_barrier(); /* needed to avoid confusion in multiple scatters */
  scatter->in_use = 0;
}
!END
