/**********************

 QDP_$LIB header file

**********************/

#ifndef _QDP_$LIB
#define _QDP_$LIB

#include "qdp_common.h"
#include "qdp_types.h"
#include "qla_types.h"
#include "qla_complex.h"
#include "qla_random.h"

#ifdef __cplusplus
extern "C" {
#endif

/* create and destroy fields */

!PCTYPES
$QDPPCTYPE * QDP$PC_create_$ABBR($NCVOID);
void QDP$PC_destroy_$ABBR($NC$QDPPCTYPE *field);
!END

/* data access and extraction */

!PCTYPES
$QLAPCTYPE * QDP$PC_expose_$ABBR($NC$QDPPCTYPE *field);
void QDP$PC_reset_$ABBR($NC$QDPPCTYPE *field);
void QDP$PC_extract_$ABBR($NC$QLAPCTYPE *dest, $QDPPCTYPE *src, QDP_Subset subset);
void QDP$PC_insert_$ABBR($NC$QDPPCTYPE *dest, $QLAPCTYPE *src, QDP_Subset subset);
!END

/* optimization */

!PCSHIFTTYPES
void QDP$PC_discard_$ABBR($NC$QDPPCTYPE *field);
!END

/* set by function */

!PCSHIFTTYPES
void QDP$PC_$ABBR_eq_func($NC$QDPPCTYPE *dest, void (*func)($QLAPCTYPE *dest, int coords[]), QDP_Subset subset );
void QDP$PC_$ABBR_eq_funci($NC$QDPPCTYPE *dest, void (*func)($QLAPCTYPE *dest, int index), QDP_Subset subset );
!END

/* shift */

!PCSHIFTTYPES
void QDP$PC_$ABBR_eq_s$ABBR($NC$QDPPCTYPE *dest, $QDPPCTYPE *src, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_$ABBR_veq_s$ABBR($NC$QDPPCTYPE *dest[], $QDPPCTYPE *src[], QDP_Shift shift[], QDP_ShiftDir fb[], QDP_Subset subset, int nv);
!END

/* shift & multiply */

!EQOPS
!PCCOLORTYPES
void QDP$PC_$ABBR_$EQOP_M_times_s$ABBR($NC$QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_$ABBR_$EQOP_Ma_times_s$ABBR($NC$QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_$ABBR_$EQOP_sM_times_$ABBR($NC$QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_$ABBR_$EQOP_sMa_times_$ABBR($NC$QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_$ABBR_$EQOP_sM_times_s$ABBR($NC$QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_$ABBR_$EQOP_sMa_times_s$ABBR($NC$QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
!END

!EQOPS
void QDP$PC_M_$EQOP_M_times_sMa($NC QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_M_$EQOP_Ma_times_sMa($NC QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_M_$EQOP_sM_times_Ma($NC QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_M_$EQOP_sMa_times_Ma($NC QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_M_$EQOP_sM_times_sMa($NC QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
void QDP$PC_M_$EQOP_sMa_times_sMa($NC QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
!END
