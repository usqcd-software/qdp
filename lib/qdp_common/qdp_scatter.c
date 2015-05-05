#include "qdp_int_internal.h"
#include "qdp_scatter_internal.h"

#include <stdarg.h>
void XXXdump(const char *fmt, ...)
{
  char name[100];
  FILE *f;
  va_list va;

  sprintf(name, "xx%d", QDP_this_node);
  f = fopen(name, "at");
  va_start(va, fmt);
  vfprintf(f, fmt, va);
  va_end(va);
  fclose(f);
}

/* ASSUMPTIONS:
 *  1. This code assumes that node numbering on different lattices is consistent.
 *  2. Cost of doing SUM_BLOCK QMP_sum_int() is higher than QMP_sum_int_array(..., SUM_BLOCK).
 *  3. If the global reduction block size is too large, QMP (or whatever is below it) will
 *     break the operation into pieces, with resulting performance as good as one would
 *     get by reducing SUM_BLOCK appropriately.
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

/* Compute the number of nodes that are going to request data from a given node
 * This routine uses the global sum to combine local results. This implementation
 * combines multiple requctions together to reduce network traffic.
 */
static void
build_rcv_count(QDP_Scatter *scatter)
{
  int totalNodes = QMP_get_number_of_nodes();
  int loNode = 0;
  int hiNode = SUM_BLOCK;
  int remoteCount = scatter->remoteCount;
  int r = 0;
  int reqBuffer[SUM_BLOCK];
  int thisCount = -1;

  while (loNode < totalNodes) {
    memset(reqBuffer, 0, SUM_BLOCK * sizeof (int));
    while ((r < remoteCount) && (scatter->remoteData[r].dstNode < hiNode)) {
      reqBuffer[scatter->remoteData[r].dstNode - loNode] = 1;
      r++;
    }
    QMP_sum_int_array(reqBuffer, SUM_BLOCK);
    if ((loNode <= QDP_this_node) && (QDP_this_node < hiNode))
      thisCount = reqBuffer[QDP_this_node - loNode];

    loNode = hiNode;
    hiNode += SUM_BLOCK;
  }
  scatter->rcvCount = thisCount;
}

QDP_Scatter *
QDP_create_scatter(QDP_Lattice *dstLat,
                   QDP_Lattice *srcLat,
                   QDP_Int *srcAddr[]) /* [QDP_ndim_L(srcLat) */
{
  int i, m, n;
  int srcRank;
  int dstVolume;
  int srcVolume;
  QDP_Scatter *scatter = NULL;
  QLA_Int **addr = NULL;
  int *srcPos = NULL;
  int *srcDim = NULL;
  struct trans *trans = NULL;

  QDP_assert(dstLat != NULL);
  QDP_assert(srcLat != NULL);
  QDP_assert(srcAddr != NULL);

  srcRank = QDP_ndim_L(srcLat);
  dstVolume = QDP_sites_on_node_L(dstLat);
  srcVolume = QDP_sites_on_node_L(srcLat);

  /* allocate and preinit scatter */
  QDP_malloc(scatter, QDP_Scatter, 1);
  memset(scatter, 0, sizeof (QDP_Scatter));

  scatter->dstLattice = dstLat;
  scatter->srcLattice = srcLat;
  scatter->srcSize = srcVolume;

  /* allocate locals */
  QDP_malloc(trans, struct trans, dstVolume);
  QDP_malloc(srcDim, int, srcRank);
  QDP_latsize_L(srcLat, srcDim);

  /* Expose the indices */
  QDP_malloc(addr, QLA_Int *, srcRank);
  for (i = 0; i < srcRank; i++) {
    QDP_assert(srcAddr[i] != NULL);
    QDP_assert(QDP_get_lattice_I(srcAddr[i]) == dstLat);
    addr[i] = QDP_expose_I(srcAddr[i]);
  }

  /* build translation */
  QDP_malloc(srcPos, int, srcRank);
  for (m = 0; m < dstVolume; m++) {
    for (i = 0; i < srcRank; i++) {
      QDP_assert(addr[i][m] >= 0);
      QDP_assert(addr[i][m] < srcDim[i]);
      srcPos[i] = addr[i][m];
    }
    trans[m].local_idx = m;
    trans[m].remote_node = QDP_node_number_L(srcLat, srcPos);
    trans[m].remote_idx = QDP_index_L(srcLat, srcPos);
  }
  /* reset indices */
  for (i = 0; i < srcRank; i++) {
    QDP_reset_I(srcAddr[i]);
  }

  QDP_free(srcPos);
  QDP_free(addr);

  if (dstVolume > 0) {
    int nodeCount = 1;
    int siteCount = 1;
    /* sort the trans */
    qsort(trans, dstVolume, sizeof (struct trans), trans_compare);
    /* compact trans, extracting the dstAddr in the process */
    scatter->dstSize = dstVolume;
    QDP_malloc(scatter->dstAddr, int, dstVolume);
    scatter->dstAddr[trans[0].local_idx] = 0;
    for (m = 1; m < dstVolume; m++) {
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
      scatter->dstAddr[trans[m].local_idx] = siteCount - 1;
    }
    /* build the meta messages, index maps and associated QMP structures */
    scatter->arenaSize = siteCount;
    scatter->reqSize = nodeCount;
    QDP_malloc(scatter->reqData, struct request, nodeCount);
    QDP_malloc(scatter->idxData, int, siteCount);
    for (i = m = n = 0; n < siteCount; n++) {
      scatter->idxData[n] = trans[n].remote_idx;
      if ((n + 1 == siteCount) ||
	  (trans[n].remote_node != trans[n+1].remote_node)) {
	scatter->reqData[i].dstNode = trans[m].remote_node;
	scatter->reqData[i].start = m;
	scatter->reqData[i].meta.node = QDP_this_node;
	scatter->reqData[i].meta.count = n + 1 - m;
	m = n + 1;
	i++;
      }
    }
    QDP_assert(i == nodeCount);
    /* separate locals from remotes */
    if (scatter->reqData[0].dstNode == QDP_this_node) {
      scatter->localSize = scatter->reqData[0].meta.count;
      scatter->localAddr = scatter->idxData;
      scatter->remoteCount = nodeCount - 1;
      scatter->remoteData = scatter->reqData + 1;
    } else {
      scatter->localSize = 0;
      scatter->localAddr = NULL;
      scatter->remoteCount = nodeCount;
      scatter->remoteData = scatter->reqData;
    }
    if (scatter->remoteCount > 0) {
      /* allocate and create meta handles */
      QDP_malloc(scatter->remoteMetaH, QMP_msghandle_t, scatter->remoteCount);
      QDP_malloc(scatter->remoteMetaMem, QMP_msgmem_t, scatter->remoteCount);
      for (m = 0; m < scatter->remoteCount; m++) {
	scatter->remoteMetaMem[m] = QMP_declare_msgmem(&scatter->remoteData[m].meta,
						       sizeof (struct metaMessage));
	scatter->remoteMetaH[m] = QMP_declare_multisend_to(scatter->remoteMetaMem[m],
							   scatter->remoteData[m].dstNode, 0);
      }

      /* allocate and create map handles */
      QDP_malloc(scatter->remoteMapH, QMP_msghandle_t, scatter->remoteCount);
      QDP_malloc(scatter->remoteMapMem, QMP_msgmem_t, scatter->remoteCount);
      for (m = 0; m < scatter->remoteCount; m++) {
	scatter->remoteMapMem[m] = QMP_declare_msgmem(&scatter->idxData[scatter->remoteData[m].start],
						      scatter->remoteData[m].meta.count * sizeof (int));
	scatter->remoteMapH[m] = QMP_declare_tagged_send_to(scatter->remoteMapMem[m],
							    scatter->remoteData[m].dstNode, 0, 1);
      }
    }
  }

  /* find the number of remote nodes that will be asking for data from this node */
  build_rcv_count(scatter);
  QDP_assert(scatter->rcvCount >= 0);

  /* Construct a receive handle for incoming meta messages */
  if (scatter->rcvCount > 0) {
    scatter->rcvMetaMem = QMP_declare_msgmem(&scatter->rcvMetaData, sizeof (struct metaMessage));
    scatter->rcvMetaH = QMP_declare_multireceive(scatter->rcvMetaMem, 0);
  }

  /* compute bufferSize */
  for (m = 0; m < scatter->remoteCount; m++) {
    QMP_start(scatter->remoteMetaH[m]);
  }
  scatter->bufferSize = 0;
  for (m = 0; m < scatter->rcvCount; m++) {
    scatter->rcvMetaData.count = -1;
    scatter->rcvMetaData.node = -1;
    QMP_start(scatter->rcvMetaH);
    QMP_wait(scatter->rcvMetaH);
    QDP_assert(scatter->rcvMetaData.count > 0);
    QDP_assert(scatter->rcvMetaData.node >= 0);
    if (scatter->bufferSize < scatter->rcvMetaData.count) {
      scatter->bufferSize = scatter->rcvMetaData.count;
    }
  }
  if (scatter->remoteCount > 0) {
    QMP_wait_all(scatter->remoteMetaH, scatter->remoteCount);
  }

  /* free locals */
  QDP_free(trans);
  QDP_free(srcDim);
  QMP_barrier(); /* needed to avoid confusion in multiple scatters */

  return scatter;
}

void
QDP_destroy_scatter(QDP_Scatter *scatter)
{
  /* sanity checks */
  if (scatter == NULL)
    return;
  QDP_assert(scatter->in_use == 0);

  /* free communicators */
  if (scatter->remoteCount) {
    int m;
    for (m = 0; m < scatter->remoteCount; m++) {
      QMP_free_msgmem(scatter->remoteMetaMem[m]);
      QMP_free_msgmem(scatter->remoteMapMem[m]);
      QMP_free_msghandle(scatter->remoteMetaH[m]);
      QMP_free_msghandle(scatter->remoteMapH[m]);
    }
  }
  if (scatter->rcvCount) {
    QMP_free_msghandle(scatter->rcvMetaH);
    QMP_free_msgmem(scatter->rcvMetaMem);
  }
  /* free allocated memory inside scatter structure */
  if (scatter->dstAddr)
    QDP_free(scatter->dstAddr);
  if (scatter->reqData)
    QDP_free(scatter->reqData);
  if (scatter->idxData)
    QDP_free(scatter->idxData);
  if (scatter->remoteMetaMem)
    QDP_free(scatter->remoteMetaMem);
  if (scatter->remoteMapMem)
    QDP_free(scatter->remoteMapMem);
  if (scatter->remoteMetaH)
    QDP_free(scatter->remoteMetaH);
  if (scatter->remoteMapH)
    QDP_free(scatter->remoteMapH);

  /* more defensive coding: help catch some errors */
  memset(scatter, 0, sizeof (QDP_Scatter));
  scatter->in_use = 1;
  QDP_free(scatter);
}
