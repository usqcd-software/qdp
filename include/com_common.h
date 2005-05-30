#ifndef _COM_COMMON_H
#define _COM_COMMON_H

#include "qdp_subset.h"
#include "qdp_subset_internal.h"
#include "qdp_shift.h"

#define QDP_OWN_INVERSE      0
#define QDP_WANT_INVERSE     1
#define QDP_NO_INVERSE       2

typedef struct QDP_gather_struct QDP_gather;
typedef struct QDP_msg_tag_struct QDP_msg_tag;

/**********************************************************************/
/* Declarations for all exported routines in com_common.c */

extern void QDP_initialize_comm(int *argc, char ***argv);
extern void QDP_finalize_comm(void);
extern void QDP_abort_comm(void);
extern int QDP_comm_initialized(void);

extern QDP_gather *QDP_make_gather_map(
  void (*func)(int[], int[], QDP_ShiftDir, void *),
         		/* function which defines sites to gather from */
  void *args,		/* list of arguments, to be passed to function */
  int argsize,
  int inverse);		/* OWN_INVERSE, WANT_INVERSE, or NO_INVERSE */

extern QDP_gather *QDP_make_gather_shift(
  int disp[],           /* shift displacement */
  int inverse);		/* OWN_INVERSE, WANT_INVERSE, or NO_INVERSE */

extern QDP_msg_tag * QDP_declare_strided_gather(
  char *src,            /* source array */
  int stride,           /* bytes between fields in source buffer */
  int size,		/* size in bytes of the field (eg sizeof(su3_vector))*/
  QDP_gather *g,        /* gather to do */
  QDP_ShiftDir fb,      /* forwards or backwards */
  QDP_Subset subset,	/* parity of sites whose neighbors we gather.
			   one of EVEN, ODD or EVENANDODD. */
  char **dest);         /* one of the vectors of pointers */

extern void QDP_prepare_gather(QDP_msg_tag *);
extern void QDP_do_gather(QDP_msg_tag *);
extern void QDP_wait_gather(QDP_msg_tag *mbuf);
extern void QDP_cleanup_gather(QDP_msg_tag *mbuf);
extern void QDP_free_gather(QDP_gather *g);

extern void QDP_accumulate_gather(
  QDP_msg_tag **mmtag,      /* msg_tag to accumulate into */
  QDP_msg_tag *mtag);       /* msg_tag to add to the gather */

extern void QDP_declare_accumulate_strided_gather(
  QDP_msg_tag **mmtag,  /* msg_tag to accumulate into */
  char *src,            /* source array */
  int stride,           /* bytes between fields in source buffer */
  int size,		/* size in bytes of the field (eg sizeof(su3_vector))*/
  QDP_gather *g,        /* gather to do */
  QDP_ShiftDir fb,      /* forwards or backwards */
  QDP_Subset subset,		/* parity of sites whose neighbors we gather.
			   one of EVEN, ODD or EVENANDODD. */
  char ** dest);	/* one of the vectors of pointers */

#endif
