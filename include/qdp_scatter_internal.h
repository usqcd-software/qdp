#ifndef _QDP_SCATTER_INTERNAL_H
#define _QDP_SCATTER_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif
    struct metaMessage {  /* meta message payload */
        int node;         /* the node id */
        int count;        /* number of elments requested */
    };
    struct request { /* request data */
        struct metaMessage meta;  /* (this_node, count) */
        int dstNode;              /* node to send the meta request to */
        int start;                /* start position for indices and data */
    };

    struct QDP_Scatter_struct {
        int in_use;                      /*    flag to avoid recursive use */
        QDP_Lattice *dstLattice;         /*    destination lattice */
        QDP_Lattice *srcLattice;         /*    source lattice */
        int srcSize;                     /*    number of elements in src */
        int dstSize;                     /*    number of elements in dst */
        int *dstAddr;                    /*  A [dstSize] address of the dst value in arena */
        int arenaSize;                   /*    number of elements in arena */
        int reqSize;                     /*    number of requests (may contain local node) */
        struct request *reqData;         /*  A [reqSize] request data (may contain local node) */
        int *idxData;                    /*  A [arenaSize] local and remote indices */
        int localSize;                   /*    number of local sources */
        int *localAddr;                  /*  p [localSize] local indices */
        int remoteCount;                 /*    number of remote nodes we need to talk to */
        QMP_msgmem_t *remoteMetaMem;     /* ^A [remoteCount] memory handles for send metas */
        QMP_msgmem_t *remoteMapMem;      /* ^A [remoteCount] mamort handles for send maps */
        QMP_msghandle_t *remoteMetaH;    /*  A [remoteCount] send meta handles (only if remoteCount > 0) */
        QMP_msghandle_t *remoteMapH;     /*  A [remoteCount] send map handles  (only if remoteCount > 0) */
        struct request *remoteData;      /*  p [remoteCount] remote requests */
        int rcvCount;                    /*    number of nodes we receive requests from */
        QMP_msghandle_t rcvMetaH;        /* ^  receive handle for receive meta messages */
        QMP_msgmem_t rcvMetaMem;         /* ^  receive memory handle for receive meta messages */
        struct metaMessage rcvMetaData;  /*    receive meta message payload */
        int bufferSize;                  /*    receive buffer size (maps and data) in elements */
    };

#ifdef __cplusplus
}
#endif

#endif /* !defined(_QDP_SCATTER_INTERNAL_H) */
