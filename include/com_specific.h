#ifndef _COM_SPECIFIC_H
#define _COM_SPECIFIC_H

#include "com_common.h"
#include "com_common_internal.h"

void QDP_initialize_comm_specific(int argc, char **argv);
void QDP_finalize_comm_specific(void);
void QDP_abort_comm_specific(void);
char *QDP_comm_type(void);
int QDP_mynode(void);
int QDP_numnodes(void);
void QDP_barrier(void);

void QDP_sum_float(float *fpt);
void QDP_sum_float_array(float *fpt, int nfloats);
void QDP_sum_double(double *dpt);
void QDP_sum_double_array(double *dpt, int ndoubles);
void QDP_global_xor(unsigned long *pt);
void QDP_max_float(float *fpt);
void QDP_max_double(double *dpt);
void QDP_binary_reduction(void *data, int size, void func());
void QDP_broadcast(char *buf, int size);
void QDP_send_bytes(char *buf, int size, int tonode);
void QDP_recv_bytes(char *buf, int size, int fromnode);

QDP_mh *QDP_alloc_mh(int n);
void QDP_free_mh(QDP_mh *mh);
char *QDP_alloc_msgmem(int n);
void QDP_free_msgmem(char *buf);
void QDP_prepare_recv(QDP_mh *mh, recv_msg_t *rm, int i);
void QDP_prepare_send(QDP_mh *mh, send_msg_t *sm, int i);
void QDP_prepare_msgs(QDP_mh *mh);
void QDP_start_recv(QDP_msg_tag *mtag, int gather_number);
void QDP_start_send(QDP_msg_tag *mtag, int gather_number);
void QDP_wait_recv(QDP_msg_tag *mtag);
void QDP_wait_send(QDP_msg_tag *mtag);
void QDP_clear_to_send(QDP_msg_tag *mtag);

#endif
