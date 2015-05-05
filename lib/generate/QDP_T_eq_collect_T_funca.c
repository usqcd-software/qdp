!PCARITHTYPES
/*
 * T = collect(op, T)
 */
#include "qdp_$lib_internal.h"
#include "qdp_collect_internal.h"
#include <qmp.h>

/* The collect() memory footprint is kept O(sublattice). This requires two extra messages per source, but solves the
 * scaling problem
 */

void
QDP$PC_$ABBR_eq_collect_$ABBR_funca($QDPPCTYPE *dst,
                                    QDP_Collect *collect,
                                    $QDPPCTYPE *src,
                                    void *zval,
                                    void (*funca)($NC$QLAPCTYPE($NCVAR(*dst)),
                                                  $QLAPCTYPE($NCVAR(*src)),
                                                  void *args),
                                    void *args,
                                    QDP_Subset subset)
{
  /* sanity checks */
  QDP_assert(dst != NULL);
  QDP_assert(src != NULL);
  QDP_assert(collect != NULL);
  QDP_assert(QDP$PC_get_lattice_$ABBR(dst) == collect->dstLattice);
  QDP_assert(QDP$PC_get_lattice_$ABBR(src) == collect->srcLattice);
  QDP_assert(QDP_subset_lattice(subset) == collect->srcLattice);
  QDP_assert(funca != NULL);

  /* Just to be sure that no recursion happens */
  QDP_assert(collect->in_use == 0);
  collect->in_use = 1;
  {
    int i, m;
#define N -1
#if ($C+0) == -1
    int nc = QDP_get_nc(dst);
#endif
    typedef $QLAPCTYPE($NCVAR(X_Data));
    X_Data *initValue = zval;
    X_Data *dstPtr = QDP$PC_expose_$ABBR(dst);
    X_Data *srcPtr = QDP$PC_expose_$ABBR(src);
    X_Data *arena = NULL;
    X_Data *buffer = NULL;
    int *index = NULL;
    QMP_msgmem_t *dataMem = NULL;
    QMP_msghandle_t *dataHandle = NULL;

    /* Check that dst and src have the same number of colors */
#if ($C+0) == -1
    QDP_assert(nc == QDP_get_nc(src));
#endif
    /* start meta and map sends */
    for (m = 0; m < collect->remoteCount; m++) {
      QMP_start(collect->remoteMetaH[m]);
      QMP_start(collect->remoteMapH[m]);
    }
    /* allocate locals (arena, buffer, index, dataMem, dataHandle) */
    if (collect->arenaSize > 0) {
      QDP_malloc(arena, X_Data, collect->arenaSize);
      if (collect->remoteCount > 0) {
	QDP_malloc(dataMem, QMP_msgmem_t, collect->remoteCount);
	QDP_malloc(dataHandle, QMP_msghandle_t, collect->remoteCount);
      }
      for (m = 0; m < collect->remoteCount; m++) {
	dataMem[m] = QMP_declare_msgmem(&arena[collect->remoteData[m].start],
					collect->remoteData[m].meta.count * sizeof (X_Data));
	dataHandle[m] = QMP_declare_send_to(dataMem[m], collect->remoteData[m].dstNode, 0);
      }
    }
    if (collect->bufferSize > 0) {
      QDP_malloc(buffer, X_Data, collect->bufferSize);
      QDP_malloc(index, int, collect->bufferSize);
    }
    /* clear arena */
    for (i = 0; i < collect->arenaSize; i++)
      QLA$PC_$ABBR_eq_$ABBR($NCVAR &arena[i], initValue);
    /* collect locals into the arena. Mind the subset */
    QDP_loop_sites(i, subset, funca($NCVAR &arena[collect->srcAddr[i]], &srcPtr[i], args));

    /* start data sends */
    for (m = 0; m < collect->remoteCount; m++) {
      QMP_start(dataHandle[m]);
    }
    /* clear dst */
    for (i = 0; i < collect->dstSize; i++)
      QLA$PC_$ABBR_eq_$ABBR($NCVAR &dstPtr[i], initValue);
    /* move local to dst */
    for (i = 0; i < collect->localSize; i++)
      QLA$PC_$ABBR_eq_$ABBR($NCVAR &dstPtr[collect->localAddr[i]], &arena[i]);
    /* get all sends to this node */
    for (m = 0; m < collect->rcvCount; m++) {
      QMP_msgmem_t idxMem, dataMem;
      QMP_msghandle_t idxHandle, dataHandle;
      /* get meta */
      QMP_start(collect->rcvMetaH);
      QMP_wait(collect->rcvMetaH);
      QDP_assert(collect->rcvMetaData.count <= collect->bufferSize);
      /* construct and get index */
      idxMem = QMP_declare_msgmem(&index[0], collect->rcvMetaData.count * sizeof (int));
      idxHandle = QMP_declare_tagged_receive_from(idxMem, collect->rcvMetaData.node, 0, 1);
      QMP_start(idxHandle);
      QMP_wait(idxHandle);
      QMP_free_msghandle(idxHandle);
      QMP_free_msgmem(idxMem);
      /* construct and get buffer */
      dataMem = QMP_declare_msgmem(&buffer[0], collect->rcvMetaData.count * sizeof (X_Data));
      dataHandle = QMP_declare_receive_from(dataMem, collect->rcvMetaData.node, 0);
      QMP_start(dataHandle);
      QMP_wait(dataHandle);
      QMP_free_msghandle(dataHandle);
      QMP_free_msgmem(dataMem);
      /* collect data from buffer to dst */
      for (i = 0; i < collect->rcvMetaData.count; i++) {
	QDP_assert(index[i] >= 0);
	QDP_assert(index[i] < collect->dstSize);
	funca($NCVAR &dstPtr[index[i]], &buffer[i], args);
      }
    }
    if (collect->remoteCount > 0) {
      /* wait for meta, map, and data sends */
      QMP_wait_all(collect->remoteMetaH, collect->remoteCount);
      QMP_wait_all(collect->remoteMapH, collect->remoteCount);
      QMP_wait_all(dataHandle, collect->remoteCount);
      /* free data messages (dataHandle, dataMem) */
      for (m = 0; m < collect->remoteCount; m++) {
	QMP_free_msghandle(dataHandle[m]);
	QMP_free_msgmem(dataMem[m]);
      }
      QDP_free(dataHandle);
      QDP_free(dataMem);
    }
    /* clear locals (arena, buffer, index) */
    if (collect->bufferSize > 0) {
      QDP_free(buffer);
      QDP_free(index);
    }
    if (collect->arenaSize > 0) {
      QDP_free(arena);
    }
    
    /* Reset source and destination */
    QDP$PC_reset_$ABBR(dst);
    QDP$PC_reset_$ABBR(src);
  }
  QMP_barrier(); /* needed to avoid confusion in multiple collects */
  collect->in_use = 0;
}
!END
