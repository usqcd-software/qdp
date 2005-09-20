/****************** com_common.c *********************************************/
/* Communications routines for QDP/C modified from MILC version 6.
   Uses QMP for communications.
*/
/*
  Exported Functions:

   QDP_initialize_comm()   initializes communications
   QDP_finalize_comm()     closes communications
   QDP_abort_comm()        halts communications

   QDP_make_gather_map()    calculates and stores necessary communications
                             lists for a given gather map function
   QDP_make_gather_shift()  calculates and stores necessary communications
                             lists for a given gather shift displacement
   QDP_free_gather()        destroys a made gather

   QDP_declare_strided_gather()  creates a message tag that defines specific
                                  details of a gather to be used later
   QDP_do_gather()               executes a previously declared gather
   QDP_wait_gather()             waits for gather to finish, insuring that the
                                  data has actually arrived.
   QDP_cleanup_gather()          frees all the buffers that were allocated,
                                  WHICH FREES THE GATHERED DATA.

   QDP_accumulate_gather()       combines gathers into single message tag
   QDP_declare_accumulate_strided_gather()
                                 does declare_gather() and accumulate_gather()
                                  in single step.

  QDP_mynode()           returns node number of this node.
  QDP_numnodes()         returns number of nodes

  QDP_barrier()          provides a synchronization point for all nodes.
  QDP_sum_float()        sums a floating point number over all nodes.
  QDP_sum_float_array()  sums an array of floats over all nodes
  QDP_sum_double()       sums a double over all nodes.
  QDP_sum_double_array() sums an array of doubles over all nodes.
  QDP_global_xor()       finds global exclusive or of long
  QDP_max_float()        finds maximum floating point number over all nodes.
  QDP_max_double()       finds maximum double over all nodes.
  QDP_binary_reduction() binary reduction
  QDP_broadcast()        broadcasts a number of bytes
  QDP_send_bytes()       sends some bytes to one other node.
  QDP_recv_bytes()       receives some bytes from some other node.

  QDP_alloc_mh()         allocate space for message handles
  QDP_free_mh()          free space for message handles
  QDP_alloc_msgmem()     allocate communications buffer
  QDP_free_msgmem()      free communications buffer
  QDP_prepare_send()     prepare to send message
  QDP_prepare_recv()     prepare to receive message
  QDP_prepare_msgs()     prepare message group (multi, if possible)
  QDP_start_send()       start sending messages
  QDP_start_recv()       start receiving messages
  QDP_wait_send()        wait for sent messages
  QDP_wait_recv()        wait for received messages

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <qmp.h>

#include "qdp_layout.h"
#include "com_common.h"
#include "com_common_internal.h"
#include "com_specific.h"

#define NOWHERE -1	/* Not an index in array of fields */

/* If we want to do our own checksums */
#include <stdint.h>
#define CRCBYTES 8
static int do_checksum = 0;
static uint32_t crc32(uint32_t crc, const unsigned char *buf, size_t len);


/**********************************************************************
 *                      INTERNAL DATA TYPES                           *
 **********************************************************************/

/* Structure to keep track of outstanding sends and receives */
struct QDP_mh_struct {
  int n;
  QMP_msgmem_t *mmv;
  QMP_msghandle_t *mhv;
  QMP_msghandle_t mh;
};


/***************************************************
 *  Global variables for the communications stuff  *
 ***************************************************/

int QDP_this_node = -1;

static int comm_initialized = 0;
static int check_comm = 0;

/* array storing gather setup info */
static QDP_gather *gather_list=NULL, *free_gather_list=NULL;
static int gather_number=1;


/**********************************************************************
 *                BASIC COMMUNICATIONS FUNCTIONS                      *
 **********************************************************************/

/*
**  Communications initialization
*/
void
QDP_initialize_comm(int *argc, char ***argv)
{
  QMP_thread_level_t provided;
  int err;

  if(!QMP_is_initialized()) {
    err = QMP_init_msg_passing(argc, argv, QMP_THREAD_SINGLE, &provided);
    if(err!=0) {
      printf("%s\n", QMP_error_string(err));
      printf("com_qmp: Initialize QMP failed.\n");
      fflush(stdout);
      exit(err);
    }
  }
  comm_initialized = 1;
}

/*
**  normal exit
*/
void
QDP_finalize_comm(void)
{
  QMP_finalize_msg_passing();
}

/*
**  abrupt halt
*/
void
QDP_abort_comm(void)
{
  QMP_finalize_msg_passing();
}

/*
**  error exit
*/
static void
QDP_comm_error(void)
{
  QDP_abort_comm();
  exit(1);
}

/*
**  check if communications have been initialized
*/
int
QDP_comm_initialized(void)
{
  return comm_initialized;
}

/*
**  check communications integrity
*/
int
QDP_check_comm(int new)
{
  int old = check_comm;
  check_comm = new;
  if(check_comm) {
    do_checksum = 1;
  } else {
    do_checksum = 0;
  }
  return old;
}


/**********************************************************************
 *                  FUNCTIONS USED TO MAKE GATHERS                    *
 **********************************************************************/

static QDP_gather *
new_gather()
{
  QDP_gather *g;

  if(free_gather_list==NULL) {
    g = (QDP_gather *) malloc(sizeof(struct QDP_gather_struct));
  } else {
    g = free_gather_list;
    free_gather_list = g->next;
  }
  if((g->next=gather_list)!=NULL) gather_list->prev = g;
  g->prev = NULL;
  gather_list = g;

  return g;
}

static void
free_gather_t(gather_t *gt)
{
  int i;

  for(i=0; i<gt->nrecvs; ++i) {
    free(gt->recvlist[i].sitelist);
    free(gt->recvlist[i].srclist);
  }
  free(gt->recvlist);

  for(i=0; i<gt->nsends; ++i) {
    free(gt->sendlist[i].sitelist);
    free(gt->sendlist[i].destlist);
  }
  free(gt->sendlist);

  free(gt->fromlist);
}

void
QDP_free_gather(QDP_gather *g)
{
  free_gather_t(&g->g[0]);
  if(g->g[1].fromlist) free_gather_t(&g->g[1]);
  if(g->next) g->next->prev = g->prev;
  if(g->prev) g->prev->next = g->next;
  if(gather_list==g) gather_list = g->next;
  g->next = free_gather_list;
  free_gather_list = g;
}

static void
sort_sendlist(int *list, int *key, int n)
{
  int j, k, in1, in2, flag;

  /* bubble sort, if this takes too long fix it later */
  for(j=n-1; j>0; j--) {
    flag=0;
    for(k=0; k<j; k++) {
      in1 = key[k];
      in2 = key[k+1];
      if(in1>in2) {
	flag = 1;
	key[k]   = in2;
	key[k+1] = in1;
	in1 = list[k];
	list[k] = list[k+1];
	list[k+1] = in1;
      }
    }
    if(flag==0) break;
  }
}

static void
make_gather_map_dir(gather_t *gt,
		    void (*func)(int[], int[], QDP_ShiftDir, void *),
		    void *args, QDP_ShiftDir dir)
{
  int i, j;
  int *xr,*xs;		/* coordinates */
  int *ls, *ns, **sl, **dl;

  xs = (int *) malloc(QDP_ndim()*sizeof(int));
  xr = (int *) malloc(QDP_ndim()*sizeof(int));

  ls = (int *) malloc(QDP_numnodes()*sizeof(int));
  ns = (int *) malloc(QDP_numnodes()*sizeof(int));
  sl = (int **) malloc(QDP_numnodes()*sizeof(int *));
  dl = (int **) malloc(QDP_numnodes()*sizeof(int *));
  for(i=0; i<QDP_numnodes(); ++i) {
    ls[i] = 0;
    ns[i] = 0;
    sl[i] = NULL;
    dl[i] = NULL;
  }

  gt->fromlist = (int *)malloc( QDP_sites_on_node*sizeof(int) );
  if( gt->fromlist==NULL ) {
    printf("make_gather: NODE %d: no room for fromlist array\n",QDP_this_node);
    QDP_comm_error();
  }

  /* RECEIVE LISTS */
  gt->nrecvs = 0;
  for(i=0; i<QDP_sites_on_node; ++i) {
    QDP_get_coords(xr, QDP_this_node, i);
    func(xr, xs, dir, args);
    j = QDP_node_number(xs);
    if( j==QDP_mynode() ) {
      gt->fromlist[i] = QDP_index(xs);
    } else {
      gt->fromlist[i] = NOWHERE;
      if(ns[j]==0) ++gt->nrecvs;
      if(ns[j]>=ls[j]) {
	ls[j] += 16;
	sl[j] = (int *) realloc(sl[j], ls[j]*sizeof(int));
	dl[j] = (int *) realloc(dl[j], ls[j]*sizeof(int));
      }
      sl[j][ns[j]] = i;
      dl[j][ns[j]] = QDP_index(xs);
      ++ns[j];
    }
  }

  if(gt->nrecvs) {
    gt->recvlist = (recvlist_t *) malloc(gt->nrecvs*sizeof(recvlist_t));
    j = 0;
    for(i=0; i<QDP_numnodes(); ++i) {
      if(ns[i]) {
	gt->recvlist[j].node = i;
	gt->recvlist[j].nsites = ns[i];
	gt->recvlist[j].sitelist = sl[i];
	gt->recvlist[j].srclist = dl[i];
	++j;
      }
    }
  } else {
    gt->recvlist = NULL;
  }

  /* SEND LISTS: */
  for(i=0; i<QDP_numnodes(); ++i) {
    ls[i] = 0;
    ns[i] = 0;
    sl[i] = NULL;
    dl[i] = NULL;
  }
  dir = (QDP_forward+QDP_backward) - dir;
  gt->nsends = 0;
  for(i=0; i<QDP_sites_on_node; ++i) {
    QDP_get_coords(xs, QDP_this_node, i);
    func(xs, xr, dir, args);
    j = QDP_node_number(xr);
    if( j!=QDP_mynode() ) {
      if(ns[j]==0) ++gt->nsends;
      if(ns[j]>=ls[j]) {
	ls[j] += 16;
	sl[j] = (int *) realloc(sl[j], ls[j]*sizeof(int));
	dl[j] = (int *) realloc(dl[j], ls[j]*sizeof(int));
      }
      sl[j][ns[j]] = i;
      dl[j][ns[j]] = QDP_index(xr);
      ++ns[j];
    }
  }

  if(gt->nsends) {
    gt->sendlist = (sendlist_t *) malloc(gt->nsends*sizeof(sendlist_t));
    j = 0;
    for(i=0; i<QDP_numnodes(); ++i) {
      if(ns[i]) {
	sort_sendlist(sl[i], dl[i], ns[i]);
	gt->sendlist[j].node = i;
	gt->sendlist[j].nsites = ns[i];
	gt->sendlist[j].sitelist = sl[i];
	gt->sendlist[j].destlist = dl[i];
	++j;
      }
    }
  } else {
    gt->sendlist = NULL;
  }

  free((void*)dl);
  free((void*)sl);
  free(ns);
  free(ls);
  free(xr);
  free(xs);
}

/*
**  add another gather to the list of tables
*/
QDP_gather *
QDP_make_gather_map(
  void (*func)(int[], int[], QDP_ShiftDir, void *),
                        /* function which defines sites to gather from */
  void *args,		/* list of arguments, to be passed to function */
  int argsize,
  int inverse)		/* OWN_INVERSE, WANT_INVERSE, or NO_INVERSE */
{
  QDP_gather *g;        /* gather being created */

  g = new_gather();

  make_gather_map_dir(&g->g[0], func, args, QDP_forward);

  if( inverse == QDP_NO_INVERSE ) {
    g->g[1].fromlist = NULL;
    g->g[1].recvlist = NULL;
    g->g[1].sendlist = NULL;
  } else {
    make_gather_map_dir(&g->g[1], func, args, QDP_backward);
  }

  return(g);
}

static void
disp_func(int x[], int t[], QDP_ShiftDir fb, void *args)
{
  int i;

  if(fb==QDP_forward) {
    for(i=0; i<QDP_ndim(); ++i) {
      t[i] = (QDP_coord_size(i)*abs(((int*)args)[i])+x[i]+((int*)args)[i])%QDP_coord_size(i);
    }
  } else {
    for(i=0; i<QDP_ndim(); ++i) {
      t[i] = (QDP_coord_size(i)*abs(((int*)args)[i])+x[i]-((int*)args)[i])%QDP_coord_size(i);
    }
  }
}

/*
**  add another gather to the list of tables from displacement
*/
QDP_gather *
QDP_make_gather_shift(
  int disp[],		/* displacement */
  int inverse)		/* OWN_INVERSE, WANT_INVERSE, or NO_INVERSE */
{
  return QDP_make_gather_map(disp_func, (void *)disp, QDP_ndim()*sizeof(int),
			     inverse);
}


/**********************************************************************
 *                         GATHER ROUTINES                            *
 **********************************************************************

 QDP_declare_strided_gather() returns a pointer to msg_tag which will
   be used as input to subsequent prepare_gather() (optional), do_gather(),
   wait_gather() and cleanup_gather() calls.

 QDP_prepare_gather() allocates buffers needed for the gather.  This call is
   optional since it will automatically be called from do_gather() if
   not explicitly called before.

 QDP_do_gather() starts the actual gather.  This may be repeated after a
    QDP_wait_gather() to repeat the exact same gather.

 QDP_wait_gather() waits for the gather to finish.

 QDP_cleanup_gather() frees memory associated with the QDP_msg_tag.

*/

recv_msg_t *
alloc_rm(void)
{
  return (recv_msg_t *) malloc(sizeof(recv_msg_t));
}

send_msg_t *
alloc_sm(void)
{
  return (send_msg_t *) malloc(sizeof(send_msg_t));
}

gmem_t *
alloc_gmem(void)
{
  return (gmem_t *) malloc(sizeof(gmem_t));
}

int
make_recv_msg(recv_msg_t ***pprm, recvlist_t *rl, QDP_Subset subset,
	      char **mem, int size, char *src)
{
  int n=0;

  if(subset->indexed) {
    int i, j=0, len=0;

    for(i=0; i<subset->len; ++i) {
      while((j<rl->nsites)&&(rl->sitelist[j]<subset->index[i])) ++j;
      if(j>=rl->nsites) break;
      if(rl->sitelist[j]==subset->index[i]) ++len;
    }

    if(len!=0) {
      recv_msg_t *rm;
      rm = alloc_rm();
      rm->node = rl->node;
      rm->size = len*size;
      rm->buf = NULL;
      rm->gmem = alloc_gmem();
      rm->gmem->next = NULL;
      rm->gmem->src = src;
      //rm->gmem->offset = 0;
      rm->gmem->merged = 0;
      rm->gmem->mem = (char *)mem;
      rm->gmem->stride = sizeof(char *);
      rm->gmem->size = size;
      rm->gmem->begin = 0;
      rm->gmem->end = len;
      rm->gmem->sitelist = (int *) malloc(len*sizeof(int));
      rm->gmem->sitelist_allocated = 1;
      rm->gmem->otherlist = (int *) malloc(len*sizeof(int));
      rm->gmem->otherlist_allocated = 1;
      **pprm = rm;
      *pprm = &rm->next;
      n = 1;
      j = 0;
      len = 0;
      for(i=0; i<subset->len; ++i) {
	while((j<rl->nsites)&&(rl->sitelist[j]<subset->index[i])) ++j;
	if(j>=rl->nsites) break;
	if(rl->sitelist[j]==subset->index[i]) {
	  rm->gmem->sitelist[len] = rl->sitelist[j];
	  rm->gmem->otherlist[len] = rl->srclist[j];
	  ++len;
	}
      }
    }

  } else {
    int i, j;

    i = 0;
    while( ( i < rl->nsites ) &&
	   ( rl->sitelist[i] < subset->offset ) ) i++;
    j = i;
    while( ( j < rl->nsites ) &&
	   ( rl->sitelist[j] < subset->offset+subset->len ) ) j++;

    if(j!=i) {
      recv_msg_t *rm;
      rm = alloc_rm();
      rm->node = rl->node;
      rm->size = (j-i)*size;
      rm->buf = NULL;
      rm->gmem = alloc_gmem();
      rm->gmem->next = NULL;
      rm->gmem->src = src;
      //rm->gmem->offset = 0;
      rm->gmem->merged = 0;
      rm->gmem->mem = (char *)mem;
      rm->gmem->stride = sizeof(char *);
      rm->gmem->size = size;
      rm->gmem->begin = i;
      rm->gmem->end = j;
      rm->gmem->sitelist = rl->sitelist;
      rm->gmem->sitelist_allocated = 0;
      rm->gmem->otherlist = rl->srclist;
      rm->gmem->otherlist_allocated = 0;
      **pprm = rm;
      *pprm = &rm->next;
      n = 1;
    }
  }
  return n;
}

int
make_send_msg(send_msg_t ***ppsm, sendlist_t *sl, QDP_Subset subset,
	      char *mem, int stride, int size)
{
  send_msg_t *sm;
  int n=0, *x;
  int c, i, len;

  x = (int *) malloc(QDP_ndim()*sizeof(int));

  len = 0;
  //fprintf(stderr, "nsites=%i\n", sl->nsites);
  for(i=0; i<sl->nsites; i++) {
    //fprintf(stderr, "i=%i node=%i dest=%i\n", i, sl->node, sl->destlist[i]);
    QDP_get_coords(x, sl->node, sl->destlist[i]);
    //fprintf(stderr, "%i %i %i %i\n", x[0], x[1], x[2], x[3]);
    c = subset->func(x, subset->args);
    //fprintf(stderr, "c=%i\n", c);
    if(c==subset->coloring) ++len;
  }
  //fprintf(stderr,"len=%i\n", len);

  if(len!=0) {
    //send_msg_t *sm;
    sm = alloc_sm();
    sm->node = sl->node;
    sm->size = len*size;
    sm->buf = NULL;
    sm->gmem = alloc_gmem();
    sm->gmem->next = NULL;
    sm->gmem->mem = mem;
    sm->gmem->merged = 0;
    sm->gmem->stride = stride;
    sm->gmem->size = size;
    sm->gmem->begin = 0;
    sm->gmem->end = len;
    sm->gmem->sitelist = (int *) malloc(len*sizeof(int));
    sm->gmem->sitelist_allocated = 1;
    sm->gmem->otherlist = (int *) malloc(len*sizeof(int));
    sm->gmem->otherlist_allocated = 1;
    sm->gmem->sn = 0;
    **ppsm = sm;
    *ppsm = &sm->next;
    n = 1;
    len = 0;

    //if(QDP_this_node==0) printf("begin\n");
    for(i=0; i<sl->nsites; i++) {
      //if((QDP_this_node==0)&&(subset==QDP_even))
      //printf("-- %i %i\n", i, sl->sitelist[i]);
      QDP_get_coords(x, sl->node, sl->destlist[i]);
      c = subset->func(x, subset->args);
      //if(QDP_this_node==0)
      //printf("                          %i %i %i %i %i\n", c,
      //x[0], x[1], x[2], x[3]);
      if(c==subset->coloring) {
	//if(QDP_this_node==0)
	//if((QDP_this_node==0)&&(subset==QDP_even))
	//printf("%i %i %i %i %i %i\n", sl->sitelist[i], sl->destlist[i],
	//x[0], x[1], x[2], x[3]);
        sm->gmem->sitelist[len] = sl->sitelist[i];
        sm->gmem->otherlist[len] = sl->destlist[i];
        ++len;
      }
    }
    //if(QDP_this_node==0) printf("end len=%i\n", len);
  }

  free(x);

  return n;
}

/*
**  returns msg_tag containing details for specific gather
**  handles gathers from both field offset and temp
*/
QDP_msg_tag *
QDP_declare_strided_gather(
  char *src,	        /* source buffer aligned to desired field */
  int stride,           /* bytes between source fields in source buffer */
  int size,		/* size in bytes of the source field */
  QDP_gather *g,        /* gather to do */
  QDP_ShiftDir fb,      /* forwards or backwards */
  QDP_Subset subset,	/* subset of sites to gather to */
  char **dest)		/* array of pointers for result */
{
  int i, j;	        /* scratch */
  QDP_msg_tag *mtag;	/* message tag structure we will return a pointer to */
  recv_msg_t **rm;
  send_msg_t **sm;
  gather_t *gt;         /* pointer to current gather */

  gt = &g->g[fb];

  /* set pointers in sites whose neighbors are on this node */
  if(subset->indexed) {
    for(i=0; i<subset->len; ++i) {
      j = subset->index[i];
      if(gt->fromlist[j] != NOWHERE) {
	dest[j] = src + gt->fromlist[j]*stride;
      }
    }
  } else {
    i = subset->offset + subset->len;
    for(j=subset->offset; j<i; ++j) {
      if(gt->fromlist[j] != NOWHERE) {
	dest[j] = src + gt->fromlist[j]*stride;
      }
    }
  }

  /*  allocate the message tag */
  mtag = (QDP_msg_tag *)malloc(sizeof(QDP_msg_tag));
  mtag->prepared = 0;

  mtag->nrecvs = 0;
  rm = &mtag->recv_msgs;
  for(i=0; i<gt->nrecvs; ++i) {
    mtag->nrecvs += make_recv_msg(&rm, &gt->recvlist[i], subset,
				  dest, size, src);
  }
  *rm = NULL;

  mtag->nsends = 0;
  sm = &mtag->send_msgs;
  for(i=0; i<gt->nsends; ++i) {
    mtag->nsends += make_send_msg(&sm, &gt->sendlist[i], subset,
				  src, stride, size);
  }
  *sm = NULL;

  return mtag;
}

/*
**  allocate buffers for gather
*/
static void
prepare_gather(QDP_msg_tag *mtag)
{
  int i, j;
  recv_msg_t *rm;
  send_msg_t *sm;
  gmem_t *gmem;
  char *tpt;

  mtag->prepared = 1;

  mtag->mhrecv = QDP_alloc_mh(mtag->nrecvs);
  rm = mtag->recv_msgs;
  j = 0;
  /* for each node which has neighbors of my sites */
  while(rm!=NULL) {
    if(do_checksum) rm->size += CRCBYTES;
    rm->mem = QMP_allocate_memory( rm->size );
    rm->buf = QMP_get_memory_pointer( rm->mem );
    if(do_checksum) memset(rm->buf, '\0', rm->size);
    QDP_prepare_recv(mtag->mhrecv, rm, j);
    /* set pointers in sites to correct location */
    gmem = rm->gmem;
    tpt = rm->buf;
    do {
      for(i=gmem->begin; i<gmem->end; ++i, tpt+=gmem->size) {
	if(gmem->otherlist[i]<0) {
	  ((char **)gmem->mem)[gmem->sitelist[i]] = tpt + gmem->otherlist[i]*gmem->size;
	  tpt -= gmem->size;
	} else {
	  ((char **)gmem->mem)[gmem->sitelist[i]] = tpt;
	}
      }
    } while((gmem=gmem->next)!=NULL);
    rm = rm->next;
    ++j;
  }
  QDP_prepare_msgs(mtag->mhrecv);

  mtag->mhsend = QDP_alloc_mh(mtag->nsends);
  sm = mtag->send_msgs;
  j = 0;
  /* for each node whose neighbors I have */
  while(sm!=NULL) {
    QDP_prepare_send(mtag->mhsend, sm, j);
    sm = sm->next;
    ++j;
  }
  QDP_prepare_msgs(mtag->mhsend);
}

#define inline_copy(dest, src, type, count) \
{ \
  int _k; \
  for(_k=0; _k<count; _k++) { \
    ((type *)(dest))[_k] = ((type *)(src))[_k];	\
  } \
}

#define COPY_TYPE double
/*
**  actually execute the gather
*/
void
QDP_do_gather(QDP_msg_tag *mtag)  /* previously returned by start_gather */
{
  int i;	/* scratch */
  char *tpt;	/* scratch pointer in buffers */
  send_msg_t *sm;
  gmem_t *gmem;

  if(!mtag->prepared) prepare_gather(mtag);

  if(mtag->nrecvs>0) QDP_start_recv(mtag, gather_number);

  sm = mtag->send_msgs;
  /* for each node whose neighbors I have */
  while(sm!=NULL) {
    /* gather data into the buffer */
    tpt = sm->buf;
    gmem = sm->gmem;
    do {
      if(gmem->sn==0) {
	if(gmem->size%sizeof(COPY_TYPE)!=0) {
	  for(i=gmem->begin; i<gmem->end; ++i,tpt+=gmem->size) {
	    memcpy(tpt, gmem->mem+gmem->sitelist[i]*gmem->stride, gmem->size);
	  }
	} else {
          char *pt;
          int n;
          pt = tpt - gmem->begin*gmem->size;
          n = gmem->size / sizeof(COPY_TYPE);
          for(i=gmem->begin; i<gmem->end; ) {
            COPY_TYPE *dest, *src;
            int j, nn, si = gmem->sitelist[i];
            for(j=i+1; (j<gmem->end)&&(gmem->sitelist[j]==si+j-i); j++);
            dest = (COPY_TYPE *) (pt+i*gmem->size);
            src = (COPY_TYPE *) (gmem->mem + si*gmem->stride);
            nn = n*(j-i);
            inline_copy(dest, src, COPY_TYPE, nn);
            i = j;
          }
          tpt += (gmem->end-gmem->begin)*gmem->size;
	}
      }
    } while((gmem=gmem->next)!=NULL);
    if(do_checksum) {
      uint32_t crc = 0;
      uint32_t *crc_pt = (uint32_t *)(sm->buf + sm->size - CRCBYTES);

      gmem = sm->gmem;
      do {
	for(i=gmem->begin; i<gmem->end; ++i) {
	  crc = crc32(crc,gmem->mem+gmem->sitelist[i]*gmem->stride,gmem->size);
	}
      } while((gmem=gmem->next)!=NULL);
      *crc_pt = crc;
    }
    sm = sm->next;
  }
  /* start the send */
  if(mtag->nsends>0) QDP_start_send(mtag, gather_number);
  ++gather_number;
}

/*
**  wait for gather to finish
*/
void
QDP_wait_gather(QDP_msg_tag *mtag)
{
  /* wait for all receive messages */
  if(mtag->nrecvs>0) QDP_wait_recv( mtag );

  /* wait for all send messages */
  if(mtag->nsends>0) QDP_wait_send( mtag );

  /* Verify the checksums received */
  if(do_checksum) {
    if(mtag->nrecvs>0){
      uint32_t crcgot;
      recv_msg_t *rm;
      char *tpt;
      int msg_size;
      char *crc_pt;
      uint32_t *crc;
      int fail = 0;

      rm = mtag->recv_msgs;
      while(rm != NULL){
	tpt = rm->buf;
	msg_size = rm->size - CRCBYTES;
	crc_pt = tpt + msg_size;
	crc = (uint32_t *)crc_pt;
	crcgot = crc32(0, tpt, msg_size );
	if(*crc != crcgot) {
	  fprintf(stderr,
	          "QDP error: node %d received checksum %x but node %d sent checksum %x\n",
		  QDP_this_node, crcgot, rm->node, *crc);
	  fflush(stdout);
	  fail = 1;
	}

	QMP_sum_int(&fail);
	if(fail > 0) QMP_abort(1);

	rm = rm->next;
      }
    }
  }
}

/*
**  free buffers associated with message tag
*/
void
QDP_cleanup_gather(QDP_msg_tag *mtag)
{
  gmem_t *gmem, *next;
  recv_msg_t *rm, *rml;
  send_msg_t *sm, *sml;

  /* free all receive buffers */
  if(mtag->prepared) QDP_free_mh( mtag->mhrecv );
  rm = mtag->recv_msgs;
  while(rm!=NULL) {
    if(mtag->prepared) QMP_free_memory( rm->mem );
    gmem = rm->gmem;
    do {
      if(gmem->sitelist_allocated) free(gmem->sitelist);
      if(gmem->otherlist_allocated) free(gmem->otherlist);
      next = gmem->next;
      free(gmem);
      gmem = next;
    } while(gmem!=NULL);
    rml = rm;
    rm = rm->next;
    free(rml);
  }

  /*  free all send buffers */
  if(mtag->prepared) QDP_free_mh( mtag->mhsend );
  sm = mtag->send_msgs;
  while(sm!=NULL) {
    if((mtag->prepared)&&(sm->mem)) QMP_free_memory( sm->mem );
    gmem = sm->gmem;
    do {
      if(gmem->sitelist_allocated) free(gmem->sitelist);
      if(gmem->otherlist_allocated) free(gmem->otherlist);
      next = gmem->next;
      free(gmem);
      gmem = next;
    } while(gmem!=NULL);
    sml = sm;
    sm = sm->next;
    free(sml);
  }

  free(mtag);
}


/**********************************************************************
 *                      MULTI-GATHER ROUTINES                         *
 **********************************************************************

 QDP_accumulate_gather(QDP_msg_tag **mtag, QDP_msg_tag *tag)
   Joins declared gathers together under a single msg_tag.
   The second argument (tag) would be merged with the first (mtag).
   If mtag is NULL then this just copies tag into mtag.

 QDP_declare_accumulate_strided_gather() declares and joins gathers.

*/

static gmem_t *
copy_gmem(gmem_t *src)
{
  gmem_t *gm;
  gm = (gmem_t *)malloc(sizeof(gmem_t));
  if(gm==NULL) {
    printf("error copy_gmem malloc node:%i\n", QDP_mynode());
    QDP_comm_error();
  }
  memcpy(gm, src, sizeof(gmem_t));
  if(src->sitelist_allocated) {
    int n = (src->end - src->begin) * sizeof(int);
    gm->sitelist = malloc(n);
    memcpy(gm->sitelist, src->sitelist, n);
  }
  if(src->otherlist_allocated) {
    int n = (src->end - src->begin) * sizeof(int);
    gm->otherlist = malloc(n);
    memcpy(gm->otherlist, src->otherlist, n);
  }
  return gm;
}

static void
free_gmem(gmem_t *gm)
{
  if(gm->sitelist_allocated) free(gm->sitelist);
  if(gm->otherlist_allocated) free(gm->otherlist);
  free(gm);
}

static int
merge_recv_gmem(gmem_t *dest, gmem_t *src)
{
  int i, j, ns, nn;

  ns = src->end - src->begin;
  nn = ns;
  for(i=src->begin; i<src->end; i++) {
    for(j=dest->begin; j<dest->end; j++) {
      if(src->otherlist[i]==dest->otherlist[j]) {
	nn--;
	break;
      }
    }
  }

  if(nn!=ns) {
    int nd, k;
    if(!src->sitelist_allocated) {
      int *sl;
      sl = malloc(ns*sizeof(int));
      for(i=0; i<ns; i++) {
	sl[i] = src->sitelist[src->begin+i];
      }
      src->sitelist = sl;
      src->sitelist_allocated = 1;
    }
    if(!src->otherlist_allocated) {
      int *sl;
      sl = malloc(ns*sizeof(int));
      for(i=0; i<ns; i++) {
	sl[i] = src->otherlist[src->begin+i];
      }
      src->otherlist = sl;
      src->otherlist_allocated = 1;
    }
    src->begin = 0;
    src->end = ns;

    nd = dest->end - dest->begin;
    k = 0;
    //printf("%i: begin %p %p\n", QDP_this_node, dest->mem, src->mem);
    for(i=src->begin; i<src->end; i++) {
      for(j=dest->begin; j<dest->end; j++) {
	if(src->otherlist[i]==dest->otherlist[j]) {
	  //printf("%i: dest=%i src=%i other=%i\n", QDP_this_node, dest->sitelist[j], src->sitelist[i], src->otherlist[i]);
	  src->otherlist[i] = j-dest->begin + k - i - nd;
	  k++;
	  break;
	}
      }
    }
    //printf("%i: end\n", QDP_this_node);
  }
  src->merged = 1;
  dest->merged = 1;

  return nn * src->size;
}

/*
**  helper function to copy the QDP_gmem_t structure
*/
static void
add_recv_gmem(recv_msg_t *dest_rm, recv_msg_t *src_rm)
{
  gmem_t **dgm, *gm, *src;
  src = src_rm->gmem;
  do {
    dgm = &(dest_rm->gmem);
    if(*dgm!=NULL) {
      do {
	if( (!(*dgm)->merged) &&
	    ((*dgm)->src==src->src) &&
	    ((*dgm)->size==src->size) ) {
	  break;
	}
	dgm = &(*dgm)->next;
      } while(*dgm!=NULL);
    }
    if(*dgm==NULL) {
      gm = copy_gmem(src);
      *dgm = gm;
      gm->next = NULL;
      dest_rm->size += src->size * (src->end - src->begin);
    } else {
      if( (*dgm)->end-(*dgm)->begin < src->end-src->begin ) {
	dest_rm->size -= (*dgm)->size * ((*dgm)->end - (*dgm)->begin);
	dest_rm->size += src->size * (src->end - src->begin);
	gm = copy_gmem(src);
	gm->next = *dgm;
	*dgm = gm;
	dest_rm->size += merge_recv_gmem(gm, gm->next);
      } else {
	gm = copy_gmem(src);
	gm->next = (*dgm)->next;
	(*dgm)->next = gm;
	dest_rm->size += merge_recv_gmem(*dgm, gm);
      }
    }
    src = src->next;
  } while(src!=NULL);
}

/*
**  helper function that merges a source recv_msg_t structure into the dest
*/
static void
add_recv_msg(QDP_msg_tag *amtag, QDP_msg_tag *mtag)
{
  recv_msg_t *arm, *rm;

  rm = mtag->recv_msgs;
  while(rm!=NULL) {
    arm = amtag->recv_msgs;
    while(1) {
      if(arm==NULL) {
	arm = (recv_msg_t *) malloc(sizeof(recv_msg_t));
	arm->next = amtag->recv_msgs;
	amtag->recv_msgs = arm;
	arm->node = rm->node;
	arm->size = 0;
	arm->gmem = NULL;
	amtag->nrecvs++;
	break;
      }
      if(arm->node==rm->node) {
	break;
      }
      arm = arm->next;
    }
    add_recv_gmem( arm, rm );
    rm = rm->next;
  }
}

static int
merge_send_gmem(gmem_t *dest, gmem_t *src)
{
  int i, j, ns, nn;

  ns = src->end - src->begin;
  nn = ns;
  for(i=src->begin; i<src->end; i++) {
    for(j=dest->begin; j<dest->end; j++) {
      if(src->sitelist[i]==dest->sitelist[j]) {
	nn--;
	break;
      }
    }
  }

  if(nn>0) {
    int nd, nt, k;
    nd = dest->end - dest->begin;
    nt = nd + nn;
    if(dest->sitelist_allocated) {
      dest->sitelist = realloc(dest->sitelist, nt*sizeof(int));
    } else {
      int *sl;
      sl = malloc(nt*sizeof(int));
      for(i=0; i<nd; i++) {
	sl[i] = dest->sitelist[dest->begin+i];
      }
      dest->sitelist = sl;
      dest->sitelist_allocated = 1;
    }
    if(dest->otherlist_allocated) {
      dest->otherlist = realloc(dest->otherlist, nt*sizeof(int));
    } else {
      int *sl;
      sl = malloc(nt*sizeof(int));
      for(i=0; i<nd; i++) {
	sl[i] = dest->otherlist[dest->begin+i];
      }
      dest->otherlist = sl;
      dest->otherlist_allocated = 1;
    }
    dest->begin = 0;
    dest->end = nt;

    k = nd;
    for(i=src->begin; i<src->end; i++) {
      for(j=0; j<nd; j++) {
	if(src->sitelist[i]==dest->sitelist[j]) {
	  break;
	}
      }
      if(j>=nd) {
	dest->sitelist[k] = src->sitelist[i];
	dest->otherlist[k] = src->otherlist[i];
	k++;
      }
    }
  }
  src->merged = 1;
  dest->merged = 1;

  return nn * dest->size;
}

/*
**  helper function to copy the QDP_gmem_t structure
*/
static void
add_send_gmem(send_msg_t *dest_sm, send_msg_t *src_sm)
{
  gmem_t **dgm, *gm, *src;
  src = src_sm->gmem;
  do {
    dgm = &(dest_sm->gmem);
    while(*dgm!=NULL) {
      if( (!(*dgm)->merged) &&
	  ((*dgm)->mem==src->mem) &&
	  ((*dgm)->stride==src->stride) &&
	  ((*dgm)->size==src->size) ) {
	break;
      }
      dgm = &((*dgm)->next);
    }
    if(*dgm==NULL) {
      gm = copy_gmem(src);
      *dgm = gm;
      gm->next = NULL;
      dest_sm->size += src->size * (src->end - src->begin);
    } else {
      if( (*dgm)->end-(*dgm)->begin < src->end-src->begin ) {
	gmem_t *tgm;
	gm = copy_gmem(src);
	tgm = *dgm;
	*dgm = gm;
	gm->next = tgm->next;
	dest_sm->size -= tgm->size * (tgm->end - tgm->begin);
	dest_sm->size += src->size * (src->end - src->begin);
	dest_sm->size += merge_send_gmem(gm, tgm);
	free_gmem(tgm);
      } else {
	dest_sm->size += merge_send_gmem(*dgm, src);
      }
    }
    src = src->next;
  } while(src!=NULL);
}

/*
**  helper function that merges a source send_msg_t structure into the dest
*/
static void
add_send_msg(QDP_msg_tag *amtag, QDP_msg_tag *mtag)
{
  send_msg_t *sma, *sm;

  sm = mtag->send_msgs;
  while(sm!=NULL) {
    sma = amtag->send_msgs;
    while(1) {
      if(sma==NULL) {
	sma = (send_msg_t *) malloc(sizeof(send_msg_t));
	sma->next = amtag->send_msgs;
	amtag->send_msgs = sma;
	sma->node = sm->node;
	sma->size = 0;
	sma->gmem = NULL;
	amtag->nsends++;
	break;
      }
      if(sma->node==sm->node) {
	break;
      }
      sma = sma->next;
    }
    add_send_gmem( sma, sm );
    sm = sm->next;
  }
}

/*
**  merges already declared gather
*/
void
QDP_accumulate_gather(QDP_msg_tag **mmtag, QDP_msg_tag *mtag)
{
  QDP_msg_tag *amtag;

  if(*mmtag==NULL) {
    amtag = (QDP_msg_tag *)malloc(sizeof(QDP_msg_tag));
    if(amtag==NULL) {
      printf("error accumulate_gather malloc node:%i\n", QDP_mynode());
      QDP_comm_error();
    }
    amtag->nrecvs = 0;
    amtag->recv_msgs = NULL;
    amtag->nsends = 0;
    amtag->send_msgs = NULL;
    *mmtag = amtag;
  } else {
    amtag = *mmtag;
  }

  add_recv_msg( amtag, mtag );
  add_send_msg( amtag, mtag );
}

/*
**  declares and merges gather
**  handles both field offset and temp
*/
void
QDP_declare_accumulate_strided_gather(
  QDP_msg_tag **mmtag,      /* tag to accumulate gather into */
  char *field,	        /* which field? Some member of structure "site" */
  int stride,           /* bytes between fields in source buffer */
  int size,		/* size in bytes of the field (eg sizeof(su3_vector))*/
  QDP_gather *g,        /* gather to do */
  QDP_ShiftDir fb,      /* forwards or backwards */
  QDP_Subset subset,	/* parity of sites whose neighbors we gather.
			   one of EVEN, ODD or EVENANDODD. */
  char **dest)		/* one of the vectors of pointers */
{
  QDP_msg_tag *mtag;

  mtag = QDP_declare_strided_gather(field, stride, size, g, fb, subset, dest);
  if(*mmtag==NULL) {
    *mmtag = mtag;
  } else {
    QDP_accumulate_gather( mmtag, mtag );
    QDP_cleanup_gather( mtag );
  }
}


/* Stuff from com_qmp.c */

/*
**  Return my node number
*/
int
QDP_mynode(void)
{
  return QMP_get_node_number();
}

/*
**  Return number of nodes
*/
int
QDP_numnodes(void)
{
  return QMP_get_number_of_nodes();
}

/*
**  Synchronize all nodes
*/
void
QDP_barrier(void)
{
  QMP_barrier();
}

/*
**  Sum float over all nodes
*/
void
QDP_sum_float(float *fpt)
{
  QMP_sum_float(fpt);
}

/*
**  Sum a array of floats over all nodes
*/
void
QDP_sum_float_array(float *fpt, int nfloats)
{
  QMP_sum_float_array(fpt, nfloats);
}

/*
**  Sum double over all nodes
*/
void
QDP_sum_double(double *dpt)
{
  QMP_sum_double(dpt);
}

/*
**  Sum a array of doubles over all nodes
*/
void
QDP_sum_double_array(double *dpt, int ndoubles)
{
  QMP_sum_double_array(dpt, ndoubles);
}

/*
**  Global exclusive or acting on long
*/
void
QDP_global_xor(unsigned long *pt)
{
  unsigned long work;
  work = (unsigned long)*pt;
  QMP_xor_ulong(&work);
  *pt = (unsigned long)work;
}

/*
**  Find maximum of float over all nodes
*/
void
QDP_max_float(float *fpt)
{
  QMP_max_float(fpt);
}

/*
**  Find maximum of double over all nodes
*/
void
QDP_max_double(double *dpt)
{
  QMP_max_double(dpt);
}

/*
**  Binary reduction
*/
void
QDP_binary_reduction(void *data, int size, void func(void *, void *))
{
  QMP_binary_reduction(data, size, func);
}

/*
**  Broadcast bytes from node 0 to all others
*/
void
QDP_broadcast(char *buf, int size)
{
  QMP_broadcast(buf, size);
}

/********************************************************************
 *                   SEND AND RECEIVE BYTES                         *
 ********************************************************************/

/*
**  send_sytes is to be called only by the node doing the sending
*/
void
QDP_send_bytes(char *buf, int size, int tonode)
{
  QMP_mem_t *mbuf;
  QMP_msgmem_t mm;
  QMP_msghandle_t mh;

  mbuf = QMP_allocate_memory(size);
  memcpy(QMP_get_memory_pointer(mbuf), buf, size);
  mm = QMP_declare_msgmem(QMP_get_memory_pointer(mbuf), size);
  mh = QMP_declare_send_to(mm, tonode, 0);
  QMP_start(mh);
  QMP_wait(mh);
  QMP_free_msghandle(mh);
  QMP_free_msgmem(mm);
  QMP_free_memory(mbuf);
}

/*
**  recv_bytes is to be called only by the node to which the bytes were sent
*/
void
QDP_recv_bytes(char *buf, int size, int fromnode)
{
  QMP_mem_t *mbuf;
  QMP_msgmem_t mm;
  QMP_msghandle_t mh;

  mbuf = QMP_allocate_memory(size);
  mm = QMP_declare_msgmem(QMP_get_memory_pointer(mbuf), size);
  mh = QMP_declare_receive_from(mm, fromnode, 0);
  QMP_start(mh);
  QMP_wait(mh);
  memcpy(buf, QMP_get_memory_pointer(mbuf), size);
  QMP_free_msghandle(mh);
  QMP_free_msgmem(mm);
  QMP_free_memory(mbuf);
}

/**********************************************************************
 *                         GATHER ROUTINES                            *
 **********************************************************************/

/*
**  allocate space for message handles
*/
QDP_mh *
QDP_alloc_mh(int n)
{
  QDP_mh *mh;

  mh = (QDP_mh *) malloc(sizeof(struct QDP_mh_struct));
  mh->n = n;
  if(n) {
    mh->mmv = (QMP_msgmem_t *) malloc(n*sizeof(QMP_msgmem_t));
    mh->mhv = (QMP_msghandle_t *) malloc(n*sizeof(QMP_msghandle_t));
  } else {
    mh->mmv = NULL;
    mh->mhv = NULL;
  }
  return mh;
}

/*
**  free space for message handles
*/
void
QDP_free_mh(QDP_mh *mh)
{
  int i;

  if(mh->n) QMP_free_msghandle(mh->mh);
  for(i=0; i<mh->n; ++i) {
    //QMP_free_msghandle(mh->mhv[i]);
    QMP_free_msgmem(mh->mmv[i]);
  }
  free((void*)mh->mhv);
  free((void*)mh->mmv);
  free(mh);
}

static int
get_stride(gmem_t *gmem, void **base, size_t *size, int *num, ptrdiff_t *stride)
{
  int i, kb, ke=0, sb, se=0, sn=0, ss=0, nn=0;

  kb = -1;
  sb = -1;
  for(i=gmem->begin; i<gmem->end; i++) {
    if(kb<0) {
      kb = gmem->sitelist[i];
      ke = kb;
    } else {
      if(gmem->sitelist[i]==(ke+1)) {
	ke = gmem->sitelist[i];
      } else {
	//if(QDP_this_node==0) printf("  %i %i\n", kb, ke);
	if(sb<0) {
	  sb = kb;
	  se = ke;
	  ss = 0;
	  sn = 1;
	  nn++;
	} else {
	  if( ((se-sb)==(ke-kb)) && (kb>sb) && ((sn==1)||(sn*ss==kb-sb)) ) {
	    if(sn==1) ss = kb - sb;
	    sn++;
	  } else {
	    //if(QDP_this_node==0) printf(" %i - %i : %i x %i\n", sb, se, ss, sn);
	    sb = kb;
	    se = ke;
	    ss = 0;
	    sn = 1;
	    nn++;
	  }
	}
	kb = gmem->sitelist[i];
	ke = kb;
      }
    }
  }
  if(sb<0) {
    sb = kb;
    se = ke;
    ss = 0;
    sn = 1;
    nn++;
    //if(QDP_this_node==0) printf(" %i - %i : %i x %i\n", sb, se, ss, sn);
  } else {
    if( ((se-sb)==(ke-kb)) && (kb>sb) && ( (sn==1)||(sn*ss==kb-sb) ) ) {
      if(sn==1) ss = kb - sb;
      sn++;
      //printf(" %i - %i : %i x %i\n", sb, se, ss, sn);
    } else {
      //printf(" %i - %i : %i x %i\n", sb, se, ss, sn);
      sb = kb;
      se = ke;
      ss = 0;
      sn = 1;
      nn++;
      //printf(" %i - %i : %i x %i\n", sb, se, ss, sn);
    }
  }
  //printf("  %i %i %i\n", sb, se, nn);
  if(nn==1) {
    //printf(" %i - %i : %i x %i\n", sb, se, ss, sn);
    gmem->sb = sb;
    gmem->se = se;
    gmem->ss = ss;
    gmem->sn = sn;

    int n = gmem->se - gmem->sb + 1;
    *base = gmem->mem + gmem->sb*gmem->stride;
    *size = n * gmem->size;
    *num = gmem->sn;
    *stride = gmem->ss * gmem->stride;
  } else {
    gmem->sn = 0;
    return 0;
  }
  return 1;
}

/*
**  prepare to send message
*/
void
QDP_prepare_send(QDP_mh *mh, send_msg_t *sm, int i)
{
  int k, mem_size;
  gmem_t *gmem;
  void **base;
  size_t *size;
  int *num;
  ptrdiff_t *stride;

  for(k=0, gmem=sm->gmem; gmem!=NULL; k++, gmem=gmem->next);

  if(do_checksum) k++;
  base = (void **) malloc(k*sizeof(void *));
  size = (size_t *) malloc(k*sizeof(size_t));
  num = (int *) malloc(k*sizeof(int));
  stride = (ptrdiff_t *) malloc(k*sizeof(ptrdiff_t));

  //if(QDP_this_node==0) printf("%i: %i\n", QDP_this_node, sm->node);

  mem_size = 0;
  k=0;
  gmem = sm->gmem;
  do {
    int sn;

    sn = get_stride(gmem, base+k, size+k, num+k, stride+k);

    if(sn) {
      //fprintf(stderr, "strided gather: %i %i %i %i\n", gmem->sb, gmem->se,
      //gmem->ss, gmem->sn);
      k++;
    } else {
      size[k] = (gmem->end-gmem->begin)*gmem->size;
      num[k] = 1;
      stride[k] = 0;
      mem_size += size[k];
      k++;
    }

    gmem = gmem->next;
  } while(gmem!=NULL);

  if(do_checksum) {
    size[k] = CRCBYTES;
    num[k] = 1;
    stride[k] = 0;
    mem_size += size[k];
    k++;
  }

  if(mem_size) {
    int i, offset;
    sm->size = mem_size;
    sm->mem = QMP_allocate_memory( sm->size );
    sm->buf = QMP_get_memory_pointer( sm->mem );
    if(do_checksum) memset(sm->buf, '\0', sm->size);
    //if(k==0) {
    //mh->mmv[i] = QMP_declare_msgmem(sm->buf, sm->size);
    //} else {
    //base[k] = sm->buf;
    //size[k] = sm->size;
    //num[k] = 1;
    //stride[k] = 0;
    //k++;
    //}
    i = 0;
    offset = 0;
    gmem = sm->gmem;
    do {
      if(!gmem->sn) {
	base[i] = sm->buf + offset;
	offset += size[i];
      }
      i++;
      gmem = gmem->next;
    } while(gmem!=NULL);
    if(do_checksum) {
      base[i] = sm->buf + offset;
    }
  } else {
    sm->mem = NULL;
    sm->buf = NULL;
  }

  if(k)
    mh->mmv[i] = QMP_declare_strided_array_msgmem(base, size, num, stride, k);

  mh->mhv[i] = QMP_declare_send_to(mh->mmv[i], sm->node, 0);

  free((void*)base);
  free(size);
  free(num);
  free(stride);
}

/*
**  prepare to receive message
*/
void
QDP_prepare_recv(QDP_mh *mh, recv_msg_t *rm, int i)
{
  mh->mmv[i] = QMP_declare_msgmem(rm->buf, rm->size);
  mh->mhv[i] = QMP_declare_receive_from(mh->mmv[i], rm->node, 0);
}

/*
**  prepare message group (multi, if possible)
*/
void
QDP_prepare_msgs(QDP_mh *mh)
{
  if(mh->n) mh->mh = QMP_declare_multiple( mh->mhv, mh->n );
}

/*
**  start sending messages
*/
void
QDP_start_send(QDP_msg_tag *mtag, int gather_number)
{
  QMP_start(mtag->mhsend->mh);
}

/*
**  start receiving messages
*/
void
QDP_start_recv(QDP_msg_tag *mtag, int gather_number)
{
  QMP_start(mtag->mhrecv->mh);
}

/*
**  wait for sent messages
*/
void
QDP_wait_send(QDP_msg_tag *mtag)
{
  QMP_wait(mtag->mhsend->mh);
}

/*
**  wait for received messages
*/
void
QDP_wait_recv(QDP_msg_tag *mtag)
{
  QMP_wait(mtag->mhrecv->mh);
}

/*
** compute crc32 checksum
*/
/* Taken from the GNU CVS distribution and
   modified for SciDAC use  C. DeTar 10/11/2003
   and MILC use 5/3/2005 */

/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-1996 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* Copyright notice reproduced from zlib.h -- (C. DeTar)

  version 1.0.4, Jul 24th, 1996.

  Copyright (C) 1995-1996 Jean-loup Gailly and Mark Adler

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  Jean-loup Gailly        Mark Adler
  gzip@prep.ai.mit.edu    madler@alumni.caltech.edu


  The data format used by the zlib library is described by RFCs (Request for
  Comments) 1950 to 1952 in the files ftp://ds.internic.net/rfc/rfc1950.txt
  (zlib format), rfc1951.txt (deflate format) and rfc1952.txt (gzip format).
*/

typedef uint32_t uLong;            /* At least 32 bits */
typedef unsigned char Byte;
typedef Byte Bytef;
typedef uLong uLongf;
#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

#define local static

#ifdef DYNAMIC_CRC_TABLE

local int crc_table_empty = 1;
local uLongf crc_table[256];
local void make_crc_table OF((void));

/*
  Generate a table for a byte-wise 32-bit CRC calculation on the polynomial:
  x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x+1.

  Polynomials over GF(2) are represented in binary, one bit per coefficient,
  with the lowest powers in the most significant bit.  Then adding polynomials
  is just exclusive-or, and multiplying a polynomial by x is a right shift by
  one.  If we call the above polynomial p, and represent a byte as the
  polynomial q, also with the lowest power in the most significant bit (so the
  byte 0xb1 is the polynomial x^7+x^3+x+1), then the CRC is (q*x^32) mod p,
  where a mod b means the remainder after dividing a by b.

  This calculation is done using the shift-register method of multiplying and
  taking the remainder.  The register is initialized to zero, and for each
  incoming bit, x^32 is added mod p to the register if the bit is a one (where
  x^32 mod p is p+x^32 = x^26+...+1), and the register is multiplied mod p by
  x (which is shifting right by one and adding x^32 mod p if the bit shifted
  out is a one).  We start with the highest power (least significant bit) of
  q and repeat for all eight bits of q.

  The table is simply the CRC of all possible eight bit values.  This is all
  the information needed to generate CRC's on data a byte at a time for all
  combinations of CRC register values and incoming bytes.
*/
local void
make_crc_table()
{
  uLong c;
  int n, k;
  uLong poly;            /* polynomial exclusive-or pattern */
  /* terms of polynomial defining this crc (except x^32): */
  static Byte p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

  /* make exclusive-or pattern from polynomial (0xedb88320L) */
  poly = 0L;
  for (n = 0; n < sizeof(p)/sizeof(Byte); n++)
    poly |= 1L << (31 - p[n]);

  for (n = 0; n < 256; n++)
  {
    c = (uLong)n;
    for (k = 0; k < 8; k++)
      c = c & 1 ? poly ^ (c >> 1) : c >> 1;
    crc_table[n] = c;
  }
  crc_table_empty = 0;
}
#else
/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
local uLongf crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8l, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};
#endif

/* =========================================================================
 * This function can be used by asm versions of crc32()
 */
static uLongf *get_crc_table()
{
#ifdef DYNAMIC_CRC_TABLE
  if (crc_table_empty) make_crc_table();
#endif
  return (uLongf *)crc_table;
}

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
static uint32_t
crc32(uint32_t crc, const unsigned char *buf, size_t len)
{
    if (buf == Z_NULL) return 0L;
#ifdef DYNAMIC_CRC_TABLE
    if (crc_table_empty)
      make_crc_table();
#endif
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}
