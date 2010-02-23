#ifndef _COM_COMMON_INTERNAL_H
#define _COM_COMMON_INTERNAL_H

#include <qmp.h>

typedef struct recvlist_t {
  int node;            /* number of the node to which we connect */
  int nsites;
  int *sitelist;
  int *srclist;
} recvlist_t;

typedef struct sendlist_t {
  int node;            /* number of the node to which we connect */
  int nsites;
  int *sitelist;
  int *destlist;
} sendlist_t;

/* structure to hold all necessary info for a gather */
typedef struct {
  int *fromlist;        /* keeps track of where a gathered site comes from */
  recvlist_t *recvlist; /* list for receiving messages */
  sendlist_t *sendlist; /* list for sending messages */
  int nrecvs, nsends;   /* number of messages to receive and send */ 
  //QDP_lattice *rlat, *slat;
} gather_t;

struct QDP_gather_struct {
  gather_t g[2];
  struct QDP_gather_struct *prev, *next; /* stored as linked list */
};

/* structure to keep track of details of a declared gather */
typedef struct gmem_t {
  char *mem;            /* source (destination) address for send (receive) */
  char *src;            /* source address for receive */
  int size;             /* size of sent field */
  int stride;           /* stride of source/destination field */
  //int offset;           /* offset of sitelist in message buffer */
  int merged;
  int sb, se, ss, sn;
  int begin;
  int end;
  int *sitelist;        /* sites gathered to/from */
  int sitelist_allocated;
  int *otherlist;       /* sites gathered to/from on other node */
  int otherlist_allocated;
  struct gmem_t *next;  /* linked list */
} gmem_t;

/* Structure to keep track of outstanding sends and receives */
typedef struct recv_msg_struct {
  int node;         /* node sending or receiving message */
  int size;         /* size of message in bytes */
  QMP_mem_t *mem; /* QMP memory structure */
  char *buf;        /* address of buffer malloc'd for message */
  gmem_t *gmem;     /* linked list explaining detailed usage for buffer */
  struct recv_msg_struct *next;
} recv_msg_t;

/* Structure to keep track of outstanding sends and receives */
typedef struct send_msg_struct {
  int node;         /* node sending or receiving message */
  int size;         /* size of message in bytes */
  QMP_mem_t *mem; /* QMP memory structure */
  char *buf;        /* address of buffer malloc'd for message */
  gmem_t *gmem;     /* linked list explaining detailed usage for buffer */
  struct send_msg_struct *next;
} send_msg_t;

typedef struct QDP_mh_struct QDP_mh;

/* structure to store declared gathers
   this is the actual structure used internally
   it has the same name as the typedef which contains this structure which
   the user sees */
struct QDP_msg_tag_struct {
  int nrecvs;        /* number of messages to receive in gather */
  int nsends;        /* number of messages to send in gather */
  recv_msg_t *recv_msgs;  /* array of messages to receive */
  send_msg_t *send_msgs;  /* array of messages to send */
  QDP_mh *mhrecv, *mhsend;
  int prepared;
  int pointers;
};

#endif
