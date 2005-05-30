#ifndef _COM_SPECIFIC_H
#define _COM_SPECIFIC_H

#include "com_common.h"
#include "com_common_internal.h"

extern void QDP_initialize_comm_specific(int argc, char **argv);
extern void QDP_finalize_comm_specific(void);
extern void QDP_abort_comm_specific(void);
extern char *QDP_comm_type(void);
extern int QDP_mynode(void);
extern int QDP_numnodes(void);
extern void QDP_barrier(void);

extern void QDP_sum_float(float *fpt);
extern void QDP_sum_float_array(float *fpt, int nfloats);
extern void QDP_sum_double(double *dpt);
extern void QDP_sum_double_array(double *dpt, int ndoubles);
extern void QDP_global_xor(unsigned long *pt);
extern void QDP_max_float(float *fpt);
extern void QDP_max_double(double *dpt);
extern void QDP_binary_reduction(void *data, int size, void func());
extern void QDP_broadcast(char *buf, int size);
extern void QDP_send_bytes(char *buf, int size, int tonode);
extern void QDP_recv_bytes(char *buf, int size, int fromnode);

extern QDP_mh *QDP_alloc_mh(int n);
extern void QDP_free_mh(QDP_mh *mh);
extern char *QDP_alloc_msgmem(int n);
extern void QDP_free_msgmem(char *buf);
extern void QDP_prepare_recv(QDP_mh *mh, recv_msg_t *rm, int i);
extern void QDP_prepare_send(QDP_mh *mh, send_msg_t *sm, int i);
extern void QDP_prepare_msgs(QDP_mh *mh);
extern void QDP_start_recv(QDP_msg_tag *mtag, int gather_number);
extern void QDP_start_send(QDP_msg_tag *mtag, int gather_number);
extern void QDP_wait_recv(QDP_msg_tag *mtag);
extern void QDP_wait_send(QDP_msg_tag *mtag);

#endif
