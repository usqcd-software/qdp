#include "qdp_int_internal.h"
#include "qdp_collect_internal.h"

/* ASSUMPTIONS:
 *  1. This code assumes that node numbering on different lattices is consistent.
 *  2. Cost of doing SUM_BLOCK QMP_sum_int() is higher than QMP_sum_int_array(..., SUM_BLOCK).
 *  3. If the global reduction block size is too large, QMP (or whatever is below it) will
 *     break the operation into pieces, with resulting performance as good as one would
 *     get by reducing SUM_BLOCK appropriately.
 */

/* Note bene:
 *  This code is very similar to qdp_scatter, but for now it's better to keep them separate.
 */

#define SUM_BLOCK  64 /* number of ints that are summed in parallel */

/* translation map structure */
struct trans {
  int local_idx;
  int remote_node;
  int remote_idx;
};

/* Put the current node before other nodes,
 * order remote indices to help with compaction
 * sort local indices for a good measure
 */
static int
trans_compare(const void *a_ptr, const void *b_ptr)
{
  const struct trans *a = a_ptr;
  const struct trans *b = b_ptr;
  int cmp = a->remote_node - b->remote_node;

  if (cmp == 0) {
    cmp = a->remote_idx - b->remote_idx;
    if (cmp == 0) {
      cmp = a->local_idx - b->local_idx;
    }
    return cmp;
  }
  if (a->remote_node == QDP_this_node)
    return -1;
  if (b->remote_node == QDP_this_node)
    return +1;
  return cmp;
}

/* Compute the number of nodes that are going to send data to a given node
 * This routine uses the global sum to combine local results. This implementation
 * combines multiple requctions together to reduce network traffic.
 */
static void
build_rcv_count(QDP_Collect *collect)
{
  int totalNodes = QMP_get_number_of_nodes();
  int loNode = 0;
  int hiNode = SUM_BLOCK;
  int remoteCount = collect->remoteCount;
  int r = 0;
  int reqBuffer[SUM_BLOCK];
  int thisCount = -1;

  while (loNode < totalNodes) {
    memset(reqBuffer, 0, SUM_BLOCK * sizeof (int));
    while ((r < remoteCount) && (collect->remoteData[r].dstNode < hiNode)) {
      reqBuffer[collect->remoteData[r].dstNode - loNode] = 1;
      r++;
    }
    QMP_sum_int_array(reqBuffer, SUM_BLOCK);
    if ((loNode <= QDP_this_node) && (QDP_this_node < hiNode))
      thisCount = reqBuffer[QDP_this_node - loNode];

    loNode = hiNode;
    hiNode += SUM_BLOCK;
  }
  collect->rcvCount = thisCount;
}

QDP_Collect *
QDP_create_collect(QDP_Lattice *dstLat,
                   QDP_Lattice *srcLat,
                   QDP_Int *dstAddr[]) /* [QDP_ndim_L(dstLat)] */
{
  int i, m, n;
  int dstRank;
  int srcVolume;
  int dstVolume;
  QDP_Collect *collect = NULL;
  QLA_Int **addr = NULL;
  int *dstPos = NULL;
  int *dstDim = NULL;
  struct trans *trans = NULL;

  QDP_assert(dstLat != NULL);
  QDP_assert(srcLat != NULL);
  QDP_assert(dstAddr != NULL);

  dstRank = QDP_ndim_L(dstLat);
  srcVolume = QDP_sites_on_node_L(srcLat);
  dstVolume = QDP_sites_on_node_L(dstLat);

  /* allocate and preinit collect */
  QDP_malloc(collect, QDP_Collect, 1);
  memset(collect, 0, sizeof (QDP_Collect));

  collect->srcLattice = srcLat;
  collect->dstLattice = dstLat;
  collect->dstSize = dstVolume;
  collect->srcSize = srcVolume;

  /* allocate locals */
  QDP_malloc(trans, struct trans, srcVolume);
  QDP_malloc(dstDim, int, dstRank);
  QDP_latsize_L(dstLat, dstDim);

  /* expose indices */
  QDP_malloc(addr, QLA_Int *, dstRank);
  for (i = 0; i < dstRank; i++) {
    QDP_assert(dstAddr[i] != NULL);
    QDP_assert(QDP_get_lattice_I(dstAddr[i]) == srcLat);
    addr[i] = QDP_expose_I(dstAddr[i]);
  }
  /* build translations */
  QDP_malloc(dstPos, int, dstRank);
  for (m = 0; m < srcVolume; m++) {
    for (i = 0; i < dstRank; i++) {
      QDP_assert(addr[i][m] >= 0);
      QDP_assert(addr[i][m] < dstDim[i]);
      dstPos[i] = addr[i][m];
    }
    trans[m].local_idx = m;
    trans[m].remote_node = QDP_node_number_L(dstLat, dstPos);
    trans[m].remote_idx = QDP_index_L(dstLat, dstPos);
  }
  /* reset indices */
  for (i = 0; i < dstRank; i++) {
    QDP_reset_I(dstAddr[i]);
  }
  QDP_free(dstPos);
  QDP_free(addr);

  if (srcVolume > 0) {
    int nodeCount = 1;
    int siteCount = 1;
    /* sort the trans */
    qsort(trans, srcVolume, sizeof (struct trans), trans_compare);
    /* compact trans, extracting srcAddr in the process */
    collect->srcSize = srcVolume;
    QDP_malloc(collect->srcAddr, int, srcVolume);
    collect->srcAddr[trans[0].local_idx] = 0;
    for (m = 1; m < srcVolume; m++) {
      int change = 0;
      if (trans[m].remote_node != trans[m-1].remote_node) {
	change = 1;
	nodeCount++;
      } else {
	if (trans[m].remote_idx != trans[m-1].remote_idx) {
	  change = 1;
	}
      }
      if (change) {
	trans[siteCount] = trans[m];
	siteCount++;
      }
      collect->srcAddr[trans[m].local_idx] = siteCount - 1;
    }
    /* build meta messages, index maps and associated QMP structures */
    collect->arenaSize = siteCount;
    collect->sndSize = nodeCount;
    QDP_malloc(collect->sndData, struct request, nodeCount);
    QDP_malloc(collect->idxData, int, siteCount);
    for (i = m = n = 0; n < siteCount; n++) {
      collect->idxData[n] = trans[n].remote_idx;
      if ((n + 1 == siteCount) ||
	  (trans[n].remote_node != trans[n+1].remote_node)) {
	collect->sndData[i].dstNode = trans[m].remote_node;
	collect->sndData[i].start = m;
	collect->sndData[i].meta.node = QDP_this_node;
	collect->sndData[i].meta.count = n + 1 - m;
	m = n + 1;
	i++;
      }
    }
    QDP_assert(i == nodeCount);
    /* separate locals from remotes */
    if (collect->sndData[0].dstNode == QDP_this_node) {
      collect->localSize = collect->sndData[0].meta.count;
      collect->localAddr = collect->idxData;
      collect->remoteCount = nodeCount - 1;
      collect->remoteData = collect->sndData + 1;
    } else {
      collect->localSize = 0;
      collect->localAddr = NULL;
      collect->remoteCount = nodeCount;
      collect->remoteData = collect->sndData;
    }
    if (collect->remoteCount > 0) {
      /* allocate and create meta handles */
      QDP_malloc(collect->remoteMetaH, QMP_msghandle_t, collect->remoteCount);
      QDP_malloc(collect->remoteMetaMem, QMP_msgmem_t, collect->remoteCount);
      for (m = 0; m < collect->remoteCount; m++) {
	collect->remoteMetaMem[m] = QMP_declare_msgmem(&collect->remoteData[m].meta,
						       sizeof (struct metaMessage));
	collect->remoteMetaH[m] = QMP_declare_multisend_to(collect->remoteMetaMem[m],
							   collect->remoteData[m].dstNode, 0);
      }

      /* allocate and create map handles */
      QDP_malloc(collect->remoteMapH, QMP_msghandle_t, collect->remoteCount);
      QDP_malloc(collect->remoteMapMem, QMP_msgmem_t, collect->remoteCount);
      for (m = 0; m < collect->remoteCount; m++) {
	collect->remoteMapMem[m] = QMP_declare_msgmem(&collect->idxData[collect->remoteData[m].start],
						      collect->remoteData[m].meta.count * sizeof (int));
	collect->remoteMapH[m] = QMP_declare_tagged_send_to(collect->remoteMapMem[m],
							    collect->remoteData[m].dstNode, 0, 1);
      }
    }
  }
  /* find the number of remote nodes that will send us messages */
  build_rcv_count(collect);
  QDP_assert(collect->rcvCount >= 0);

  /* construct a receive handle for incoming meta messages */
  if (collect->rcvCount > 0) {
    collect->rcvMetaMem = QMP_declare_msgmem(&collect->rcvMetaData, sizeof (struct metaMessage));
    collect->rcvMetaH = QMP_declare_multireceive(collect->rcvMetaMem, 0);
  }

  /* compute bufferSize */
  for (m = 0; m < collect->remoteCount; m++) {
    QMP_start(collect->remoteMetaH[m]);
  }
  collect->bufferSize = 0;
  for (m = 0; m < collect->rcvCount; m++) {
    QMP_start(collect->rcvMetaH);
    QMP_wait(collect->rcvMetaH);
    QDP_assert(collect->rcvMetaData.count > 0);
    QDP_assert(collect->rcvMetaData.node >= 0);
    if (collect->bufferSize < collect->rcvMetaData.count) {
      collect->bufferSize = collect->rcvMetaData.count;
    }
  }
  if (collect->remoteCount > 0) {
    QMP_wait_all(collect->remoteMetaH, collect->remoteCount);
  }
  
  /* free locals */
  QDP_free(dstDim);
  QDP_free(trans);
  QMP_barrier(); /* needed to avoid confusion in multiple scatters */
  
  return collect;
}

void
QDP_destroy_collect(QDP_Collect *collect)
{
  /* sanity checks */
  if (collect == NULL)
    return;
  QDP_assert(collect->in_use == 0);
  /* free communicators */
  if (collect->remoteCount) {
    int m;
    for (m = 0; m < collect->remoteCount; m++) {
      QMP_free_msghandle(collect->remoteMetaH[m]);
      QMP_free_msghandle(collect->remoteMapH[m]);
      QMP_free_msgmem(collect->remoteMetaMem[m]);
      QMP_free_msgmem(collect->remoteMapMem[m]);
    }
  }
  if (collect->rcvCount) {
    QMP_free_msghandle(collect->rcvMetaH);
    QMP_free_msgmem(collect->rcvMetaMem);
  }
  /* free allocated memory inside collect structure */
  if (collect->srcAddr)
    QDP_free(collect->srcAddr);
  if (collect->sndData)
    QDP_free(collect->sndData);
  if (collect->idxData)
    QDP_free(collect->idxData);
  if (collect->remoteMetaMem)
    QDP_free(collect->remoteMetaMem);
  if (collect->remoteMapMem)
    QDP_free(collect->remoteMapMem);
  if (collect->remoteMetaH)
    QDP_free(collect->remoteMetaH);
  if (collect->remoteMapH)
    QDP_free(collect->remoteMapH);

  /* more defensive coding: help catch some errors */
  memset(collect, 0, sizeof (QDP_Collect));
  collect->in_use = 1;
  QDP_free(collect);
}
