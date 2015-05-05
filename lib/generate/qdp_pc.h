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
  $QDPPCTYPE * QDP$PC_create_$ABBR_L($NC QDP_Lattice *lat);
  void QDP$PC_destroy_$ABBR($QDPPCTYPE *field);
!END

/* data access and extraction */

!PCTYPES
  QDP_Lattice * QDP$PC_get_lattice_$ABBR($QDPPCTYPE *field);
  void * QDP$PC_expose_$ABBR($QDPPCTYPE *field);
  void QDP$PC_reset_$ABBR($QDPPCTYPE *field);
  void QDP$PC_extract_$ABBR(void *dest, $QDPPCTYPE *src, QDP_Subset subset);
  void QDP$PC_insert_$ABBR($QDPPCTYPE *dest, void *src, QDP_Subset subset);
  void QDP$PC_extract_packed_$ABBR(void *dest, $QDPPCTYPE *src, QDP_Subset subset);
  void QDP$PC_insert_packed_$ABBR($QDPPCTYPE *dest, void *src, QDP_Subset subset);
!END

/* site access */

!PCTYPES
static inline void QDP$PC_site_ptr_readonly_prep_$ABBR($QDPPCTYPE *src);
static inline void *QDP$PC_site_ptr_readonly_prepped_$ABBR($QDPPCTYPE *src, int i);
static inline void *QDP$PC_site_ptr_readonly_$ABBR($QDPPCTYPE *src, int i);
static inline void QDP$PC_site_ptr_readwrite_prep_$ABBR($QDPPCTYPE *src);
static inline void *QDP$PC_site_ptr_readwrite_prepped_$ABBR($QDPPCTYPE *src, int i);
static inline void *QDP$PC_site_ptr_readwrite_$ABBR($QDPPCTYPE *src, int i);
!END

/* optimization */

!PCTYPES
  void QDP$PC_discard_$ABBR($QDPPCTYPE *field);
!END

/* set by function */

!PCTYPES
  void QDP$PC_$ABBR_eq_func($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int coords[]), QDP_Subset subset);
  void QDP$PC_$ABBR_eq_funci($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int index), QDP_Subset subset);
  void QDP$PC_$ABBR_eq_funca($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int coords[], void *args), void *args, QDP_Subset subset);
  void QDP$PC_$ABBR_eq_funcia($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int index, void *args), void *args, QDP_Subset subset);
  void QDP$PC_$ABBR_eq_funct($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int coords[]), QDP_Subset subset);
  void QDP$PC_$ABBR_eq_funcit($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int index), QDP_Subset subset);
  void QDP$PC_$ABBR_eq_funcat($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int coords[], void *args), void *args, QDP_Subset subset);
  void QDP$PC_$ABBR_eq_funciat($QDPPCTYPE *dest, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), int index, void *args), void *args, QDP_Subset subset);
!END

/* collect and scatter
 *  zval argument should be $NC$QLAPCTYPE($NCVAR(*zval)), but that would break lib/generate/generic.pl
 */

!PCARITHTYPES
void QDP$PC_$ABBR_eq_collect_$ABBR_funca($QDPPCTYPE *dest, QDP_Collect *collect, $QDPPCTYPE *src, void *zval, void (*func)($NC$QLAPCTYPE($NCVAR(*dest)), $QLAPCTYPE($NCVAR(*src)), void *args), void *args, QDP_Subset subset);
void QDP$PC_$ABBR_eq_scatter_$ABBR($QDPPCTYPE *dest, QDP_Scatter *scatter, $QDPPCTYPE *src, QDP_Subset subset);
!END

/* shift */

!PCTYPES
  void QDP$PC_$ABBR_eq_s$ABBR($QDPPCTYPE *dest, $QDPPCTYPE *src, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_$ABBR_veq_s$ABBR($QDPPCTYPE *dest[], $QDPPCTYPE *src[], QDP_Shift shift[], QDP_ShiftDir fb[], QDP_Subset subset, int nv);
!END

/* shift & multiply */

!EQOPS
!PCCOLORTYPES
  void QDP$PC_$ABBR_$EQOP_M_times_s$ABBR($QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_$ABBR_$EQOP_Ma_times_s$ABBR($QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_$ABBR_$EQOP_sM_times_$ABBR($QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_$ABBR_$EQOP_sMa_times_$ABBR($QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_$ABBR_$EQOP_sM_times_s$ABBR($QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_$ABBR_$EQOP_sMa_times_s$ABBR($QDPPCTYPE *dest, QDP$PC_ColorMatrix *src1, $QDPPCTYPE *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
!END

!EQOPS
  void QDP$PC_M_$EQOP_M_times_sMa(QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_M_$EQOP_Ma_times_sMa(QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_M_$EQOP_sM_times_Ma(QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_M_$EQOP_sMa_times_Ma(QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_M_$EQOP_sM_times_sMa(QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
  void QDP$PC_M_$EQOP_sMa_times_sMa(QDP$PC_ColorMatrix *dest, QDP$PC_ColorMatrix *src1, QDP$PC_ColorMatrix *src2, QDP_Shift shift, QDP_ShiftDir fb, QDP_Subset subset);
!END
