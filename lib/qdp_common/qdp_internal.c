#include <stdlib.h>
#include <string.h>
#include "qdp_common_internal.h"
#include "qdp_internal.h"

// prevent use of default lattice variables/functions
#define QDP_sites_on_node ERROR
#define QDP_ndim() ERROR
#define QDP_coord_size(i) ERROR
#define QDP_latsize(ls) ERROR
#define QDP_volume() ERROR
#define QDP_numsites(node) ERROR
#define QDP_node_number(x) ERROR
#define QDP_index(x) ERROR
#define QDP_get_coords(x, node, index) ERROR
#define QDP_all ERROR
#define QDP_even_and_odd ERROR
#define QDP_even ERROR
#define QDP_odd ERROR
#define QDP_neighbor ERROR
////////////////////////////////////

//#define DEBUG

#if !defined(DEBUG) || DEBUG==0

#define ENTER
#define LEAVE

#else

static int QDP_Debug=0;
static int debug_indent = 0;
static int debug_indent_rate = 2;
//#define debug_out stderr
#define debug_out stdout
#define ENTER really_do_enter(__func__);
#define LEAVE really_do_leave(__func__);

void
indent(void)
{
  int i;
  for(i=0; i<(debug_indent*debug_indent_rate); i++) fputc(' ', debug_out);
}

void
really_do_enter(const char *procName)
{
  if((QDP_this_node==0)&&(debug_indent<QDP_Debug)) {
    indent();
    fprintf(debug_out, "-> %s\n", procName);
  }
  debug_indent++;
}

void
really_do_leave(const char *procName)
{
  debug_indent--;
  if((QDP_this_node==0)&&(debug_indent<QDP_Debug)) {
    indent();
    fprintf(debug_out, "<- %s\n", procName);
  }
}

#endif

int QDP_restart;
int QDP_prof_level=1;
int QDP_keep_time=0;
double QDP_comm_time=0.0;
double QDP_math_time=0.0;

static QDP_shift_list_t *shift_list=NULL;
static QDP_shift_list_t *sl_free_list=NULL;
static QDP_shift_src_t *ss_free_list=NULL;
static QDP_shift_dest_t *sd_free_list=NULL;

/*******************
 * shift functions *
 *******************/

/* finish all shifts into a field given it's data_common structure */
/* also remove it from the shift_list */
static void
QDP_finish_shifts(QDP_data_common_t *dc)
{
  QDP_shift_src_t *ss;
  ENTER;
  ss = dc->shift_src;
  while(ss!=NULL) {
    if(ss->sl!=NULL) {
      QDP_shift_list_t *sl;
      sl = ss->sl->next;
      if(sl!=NULL) {
	sl->prev = ss->sl->prev;
      }
      if(ss->sl->prev!=NULL) {
	ss->sl->prev->next = sl;
      } else {
	shift_list = sl;
      }
      //printf("sl_free_list\n");
      ss->sl->next = sl_free_list;
      sl_free_list = ss->sl;
      ss->sl = NULL;
      if(ss->st->shift_pending) {
	ss->st->shift_pending = 0;
	if(QDP_keep_time) QDP_comm_time -= QDP_time();
	QDP_wait_gather(ss->st->msgtag);
	if(QDP_keep_time) QDP_comm_time += QDP_time();
      }
    }
    ss = ss->next;
  }
  LEAVE;
}

/************************
 * dependency functions *
 ************************/

/* calls cleanup_gather and removes all shift_src structures from a field */
static void
QDP_clear_shift_src(QDP_data_common_t *dc)
{
  QDP_shift_src_t **pss;
  ENTER;
  pss = &(dc->shift_src);
  if( *pss != NULL ) {
    //QDP_remove_shift_tag_reference((*pss)->st);
    do {
      QDP_shift_dest_t **psd;
      psd = &((*pss)->dc->shift_dest);
      while( *psd != NULL ) {
	if( (*psd)->dc == dc ) {
	  QDP_shift_dest_t *tsd;
	  tsd = *psd;
	  *psd = tsd->next;
	  //printf("sd_free_list\n");
	  tsd->next = sd_free_list;
	  sd_free_list = tsd;
	  //fprintf(stderr,"cleared shift dest\n");
	  break;
	}
	psd = &((*psd)->next);
      }
      QDP_remove_shift_tag_reference((*pss)->st);
      pss = &((*pss)->next);
    } while(*pss!=NULL);
    //printf("ss_free_list\n");
    *pss = ss_free_list;
    ss_free_list = dc->shift_src;
    dc->shift_src = NULL;
    //fprintf(stderr,"cleared shift src\n");
  }
  LEAVE;
}

/* copies pointer references into data array (if pointers exist) */
/* assumes data array exists */
static void
QDP_copy_ptr_to_data(QDP_data_common_t *dc)
{
  char *data, **ptr;
  ENTER;
  data = *(dc->data);
  ptr = *(dc->ptr);
  if(ptr!=NULL) {
    int i;
    for(i=0; i<QDP_sites_on_node_L(dc->lat); i++) {
      if(ptr[i]) memcpy(&data[i*dc->size], ptr[i], dc->size);
    }
  }
  LEAVE;
}

/* removes all shift dependencies from field */
static void
QDP_clear_shift_dest(QDP_data_common_t *dc)
{
  ENTER;
  while(dc->shift_dest!=NULL) {
    QDP_switch_ptr_to_data(dc->shift_dest->dc);
  }
  LEAVE;
}

/* removes non-discarded shift dependencies from field */
static void
QDP_clear_valid_shift_dest(QDP_data_common_t *dc)
{
  QDP_shift_dest_t *sd;
  ENTER;
  sd = dc->shift_dest;
  while(sd!=NULL) {
    if(sd->dc->discarded) {
      sd = sd->next;
    } else {
      QDP_shift_dest_t *tsd;
      tsd = sd;
      sd = sd->next;
      QDP_switch_ptr_to_data(tsd->dc);
    }
  }
  LEAVE;
}

/* make and fill in a shift_src structure */
static QDP_shift_src_t *
QDP_alloc_shift_src_t(QDP_data_common_t *dc, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset)
{
  QDP_shift_src_t *ss;
  ENTER;
  if(ss_free_list==NULL) {
    ss = (QDP_shift_src_t *) malloc(sizeof(QDP_shift_src_t));
  } else {
    ss = ss_free_list;
    ss_free_list = ss->next;
  }
  //printf("alloc ss\n");
  ss->dc = dc;
  ss->shift = shift;
  ss->fb = fb;
  ss->subset = subset;
  LEAVE;
  return ss;
}

/* make and fill in a shift_dest structure */
static QDP_shift_dest_t *
QDP_alloc_shift_dest_t(QDP_data_common_t *dc, QDP_shift_dest_t *next)
{
  QDP_shift_dest_t *sd;
  ENTER;
  if(sd_free_list==NULL) {
    sd = (QDP_shift_dest_t *) malloc(sizeof(QDP_shift_dest_t));
  } else {
    sd = sd_free_list;
    sd_free_list = sd->next;
  }
  //printf("alloc sd\n");
  sd->dc = dc;
  sd->next = next;
  LEAVE;
  return sd;
}

/************************
 * data & ptr functions *
 ************************/

/* copy pointer references to data array */
/* creates data array if necessary and destroys pointers */
void
QDP_switch_ptr_to_data(QDP_data_common_t *dc)
{
  ENTER;
  if(*(dc->data)==NULL) {
    //*(dc->data) = (char *) malloc(QDP_sites_on_node*dc->size);
    dc->qmpmem = QMP_allocate_aligned_memory( QDP_sites_on_node_L(dc->lat)*dc->size,
					      QDP_mem_align, QDP_mem_flags );
    if(!dc->qmpmem) {
      QMP_error("QDP error: can't allocate memory\n");
      QDP_abort(1);
    }
    *(dc->data) = QMP_get_memory_pointer(dc->qmpmem);
  } else {
    QDP_clear_valid_shift_dest(dc);
  }
  if(*(dc->ptr)!=NULL) {
    QDP_finish_shifts(dc);
    if(!dc->discarded) QDP_copy_ptr_to_data(dc);
    QDP_clear_shift_src(dc);
    free((void*)*(dc->ptr));
    *(dc->ptr) = NULL;
  }
  LEAVE;
}

/**********************
 * external functions *
 **********************/

struct QDP_shift_tag_t *
QDP_alloc_shift_tag(int nv)
{
  struct QDP_shift_tag_t *st;
  ENTER;
  st = (struct QDP_shift_tag_t *) malloc(sizeof(struct QDP_shift_tag_t));
  st->nv = nv;
  st->nref = nv;
  st->shift_pending = 1;
  LEAVE;
  return st;
}

void
QDP_remove_shift_tag_reference(struct QDP_shift_tag_t *st)
{
  ENTER;
  if(--(st->nref)==0) {
    QDP_cleanup_gather(st->msgtag);
    free(st);
  }
  LEAVE;
}

/* finish all pending shifts and remove shift_list */
void
QDP_clear_shift_list(void)
{
  QDP_shift_list_t *sl;
  ENTER;
  sl = shift_list;
  if(sl!=NULL) {
    while(1) {
      sl->ss->sl = NULL;
      if(sl->ss->st->shift_pending) {
	sl->ss->st->shift_pending = 0;
	QDP_wait_gather(sl->ss->st->msgtag);
      }
      if(sl->next==NULL) break;
      sl = sl->next;
    }
    //printf("sl_free_list\n");
    sl->next = sl_free_list;
    sl_free_list = shift_list;
    shift_list = NULL;
  }
  LEAVE;
}

/* free all shift structures from a field */
void
QDP_prepare_destroy(QDP_data_common_t *dc)
{
  ENTER;
  QDP_finish_shifts(dc);
  QDP_clear_shift_src(dc);
  QDP_clear_shift_dest(dc);
  LEAVE;
}

/* prepare data array of field for writing */
void
QDP_prepare_dest(QDP_data_common_t *dc)
{
  ENTER;
  if(dc->exposed) {
    fprintf(stderr,"error: attempt to use exposed field\n");
    QDP_abort(1);
  }
  dc->discarded = 0;
  QDP_switch_ptr_to_data(dc);
  LEAVE;
}

/* prepare field as source field */
void
QDP_prepare_src(QDP_data_common_t *dc)
{
  ENTER;
  if(dc->discarded) {
    fprintf(stderr,"error: attempt to use discarded data\n");
    QDP_abort(1);
  }
  if(dc->exposed) {
    fprintf(stderr,"error: attempt to use exposed field\n");
    QDP_abort(1);
  }
  QDP_finish_shifts(dc);
  LEAVE;
}

/* prepare pair of fields for shift */
/* return value tells whether restart is possible */
int
QDP_prepare_shift(QDP_data_common_t *dest_dc, QDP_data_common_t *src_dc,
		  QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset)
{
  QDP_shift_src_t **pss, *ss;
  int restart=0;

  ENTER;

  if(src_dc->discarded) {
    fprintf(stderr,"error: attempt to use discarded data\n");
    QDP_abort(1);
  }
  if(src_dc->exposed) {
    fprintf(stderr,"error: attempt to use exposed field\n");
    QDP_abort(1);
  }
  if(dest_dc->exposed) {
    fprintf(stderr,"error: attempt to use exposed field\n");
    QDP_abort(1);
  }

  /* prepare shift source */
  if(*(src_dc->ptr)==NULL) {
    if(*(src_dc->data)==NULL) {
      fprintf(stderr,"error: shifting from uninitialized source\n");
      QDP_abort(1);
    }
  } else {
    QDP_switch_ptr_to_data(src_dc);
  }

  /* check if this shift has been done before */
  pss = &dest_dc->shift_src;
  while(1) {
    if(*pss==NULL) {
      ss = QDP_alloc_shift_src_t(src_dc, shift, fb, subset);
      ss->next = dest_dc->shift_src;
      dest_dc->shift_src = ss;
      src_dc->shift_dest = QDP_alloc_shift_dest_t(dest_dc, src_dc->shift_dest);
      break;
    }
    ss = *pss;
    if( (ss->dc==src_dc) && (ss->shift==shift) &&
	(ss->fb==fb) && (ss->subset==subset) ) {
      if(ss->st->shift_pending) {
	ss->st->shift_pending = 0;
	QDP_wait_gather(ss->st->msgtag);
      }
      if(ss==dest_dc->shift_src) {
	restart = 1;
      } else {
	*pss = ss->next;
	QDP_clear_shift_src(dest_dc); // don't save old shifts
	ss->next = dest_dc->shift_src;
	dest_dc->shift_src = ss;
	QDP_remove_shift_tag_reference(ss->st);
      }
      break;
    }
#if 0
    if(ss->subset==subset) {
      if(ss==dest_dc->shift_src) {
	if(ss->st->shift_pending) {
	  QDP_wait_gather(ss->st->msgtag);
	}
	QDP_remove_shift_tag_reference(ss->st);
      }
    }
#endif
    pss = &(ss->next);
  }
  dest_dc->discarded = 0;
  {
    QDP_shift_list_t *sl;
    if(sl_free_list==NULL) {
      sl = (QDP_shift_list_t *) malloc(sizeof(QDP_shift_list_t));
    } else {
      sl = sl_free_list;
      sl_free_list = sl->next;
    }
    //printf("alloc sl\n");
    sl->next = shift_list;
    sl->prev = NULL;
    if(shift_list) shift_list->prev = sl;
    shift_list = sl;
    sl->ss = ss;
    ss->sl = sl;
  }

  /* prepare shift destination */
  if(*(dest_dc->ptr)==NULL) {
    *(dest_dc->ptr) = (char **)malloc(QDP_sites_on_node_L(dest_dc->lat)*sizeof(char *));
    if(*(dest_dc->data)!=NULL) { 
      char *data, **ptr;
      int i;
      data = *(dest_dc->data);
      ptr = *(dest_dc->ptr);
      for(i=0; i<QDP_sites_on_node_L(dest_dc->lat); ++i) {
	ptr[i] = data + i*dest_dc->size;
      }
    } else {
      char **ptr;
      int i;
      ptr = *(dest_dc->ptr);
      for(i=0; i<QDP_sites_on_node_L(dest_dc->lat); ++i) {
	ptr[i] = NULL;
      }
    }
  }

  LEAVE;

  return restart;
}

QDP_msg_tag *
QDP_declare_shift( char **dest, char *src, int size, QDP_Shift shift,
		   QDP_ShiftDir fb, QDP_Subset subset )
{
  QDP_msg_tag *mt;
  ENTER;
  //QDP_comm_time -= QDP_time();
  mt =  QDP_declare_strided_gather( src, size, size, shift->gather, fb,
				    subset, dest );
  //QDP_comm_time += QDP_time();
  LEAVE;
  return mt;
}

void
QDP_declare_accumulate_shift( QDP_msg_tag **mt, char **dest, char *src,
			      int size, QDP_Shift shift, QDP_ShiftDir fb,
			      QDP_Subset subset )
{
  ENTER;
  //QDP_comm_time -= QDP_time();
  QDP_declare_accumulate_strided_gather( mt, src, size, size,
					 shift->gather, fb, subset, dest );
  //QDP_comm_time += QDP_time();
  LEAVE;
}


/********************************************************************
 *                        Binary reductions                         *
 ********************************************************************/

static void (*gfuncm)(void *, void *, int);
static void (*gfuncn)(int, void *, void *);
static void (*gfuncnm)(int, void *, void *, int);
static int glen;
static int gnc;

void
QDP_binary_reduce(void func(void *, void *), int size, void *data)
{
  if(QDP_keep_time) QDP_comm_time -= QDP_time();
  QDP_binary_reduction(data, size, func);
  if(QDP_keep_time) QDP_comm_time += QDP_time();
}

static void
bfuncm(void *inoutvec, void *invec)
{
  gfuncm(inoutvec, invec, glen);
}

void
QDP_binary_reduce_multi(void func(void *, void *, int), int size, void *data, int ns)
{
  gfuncm = func;
  glen = ns;
  if(QDP_keep_time) QDP_comm_time -= QDP_time();
  QDP_binary_reduction(data, ns*size, bfuncm);
  if(QDP_keep_time) QDP_comm_time += QDP_time();
}

static void
bfuncn(void *inoutvec, void *invec)
{
  gfuncn(gnc, inoutvec, invec);
}

void
QDP_N_binary_reduce(int nc, void func(int, void *, void *), int size, void *data)
{
  gnc = nc;
  gfuncn = func;
  if(QDP_keep_time) QDP_comm_time -= QDP_time();
  QDP_binary_reduction(data, size, bfuncn);
  if(QDP_keep_time) QDP_comm_time += QDP_time();
}

static void
bfuncnm(void *inoutvec, void *invec)
{
  gfuncnm(gnc, inoutvec, invec, glen);
}

void
QDP_N_binary_reduce_multi(int nc, void func(int, void *, void *, int), int size, void *data, int ns)
{
  gnc = nc;
  gfuncnm = func;
  glen = ns;
  if(QDP_keep_time) QDP_comm_time -= QDP_time();
  QDP_binary_reduction(data, ns*size, bfuncnm);
  if(QDP_keep_time) QDP_comm_time += QDP_time();
}
