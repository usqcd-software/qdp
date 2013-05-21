/* SciDAC Level 2 QDP/C Data Parallel library */

#ifndef _QDP_H
#define _QDP_H

#ifdef __cplusplus
extern "C" {
#endif

  // allow the user to specify QDP_Precision and/or QDP_PrecisionInt
  // default to single precision
#ifndef QDP_Precision
#  ifndef QDP_PrecisionInt
#    define QDP_PrecisionInt 1
#  endif
#  if QDP_PrecisionInt == 1
#    define QDP_Precision 'F'
#    define QDP_PrecisionLetter F
#  elif QDP_PrecisionInt == 2
#    define QDP_Precision 'D'
#    define QDP_PrecisionLetter D
#  else
#    error "bad QDP_PrecisionInt"
#  endif
#else
#  ifndef QDP_PrecisionInt
#    if QDP_Precision == 'F'
#      define QDP_PrecisionInt 1
#      define QDP_PrecisionLetter F
#    elif QDP_Precision == 'D'
#      define QDP_PrecisionInt 2
#      define QDP_PrecisionLetter D
#    else
#      error "bad QDP_Precision"
#    endif
#  else
#    if QDP_Precision == 'F'
#      if QDP_PrecisionInt != 1
#        error "inconsistent QDP_Precision='F' and QDP_PrecisionInt"
#      endif
#      define QDP_PrecisionLetter F
#    elif QDP_Precision == 'D'
#      if QDP_PrecisionInt != 2
#        error "inconsistent QDP_Precision='D' and QDP_PrecisionInt"
#      endif
#      define QDP_PrecisionLetter D
#    else
#      error "bad QDP_Precision"
#    endif
#  endif
#endif

  // allow the user to specify QDP_Colors and/or QDP_Nc
  // default to Nc=3
#ifndef QDP_Colors
#  ifndef QDP_Nc
#    define QDP_Nc 3
#  endif
#  if QDP_Nc == 2 || QDP_Nc == 3
#    define QDP_Colors QDP_Nc
#  elif QDP_Nc > 0
#    define QDP_Colors 'N'
#  else
#    error "bad QDP_Nc"
#  endif
#else
#  ifndef QDP_Nc
#    if QDP_Colors == 2 || QDP_Colors == 3
#      define QDP_Nc QDP_Colors
#    elif QDP_Colors == 'N'
#      define QDP_Nc 3
#    else
#      error "bad QDP_Colors"
#    endif
#  else
#    if QDP_Colors == 2 || QDP_Colors == 3
#      if QDP_Colors != QDP_Nc
#        error "inconsistent QDP_Colors and QDP_Nc"
#      endif
#    elif QDP_Colors == 'N'
#      if QDP_Nc <= 0
#        error "bad QDP_Nc"
#      endif
#    else
#      error "bad QDP_Colors"
#    endif
#  endif
#endif

  /* include QLA headers */
#ifndef QLA_Precision
#define QLA_Precision QDP_Precision
#endif
#ifndef QLA_Colors
#define QLA_Colors QDP_Colors
#endif
#ifndef QLA_Nc
#define QLA_Nc QDP_Nc
#endif
#include <qla.h>

  /* Define specific types and map them to generic types */
#include <qdp_types.h>

  /* Headers we always define */
#include <qdp_common.h>
#include <qdp_io.h>
#include <qdp_thread.h>
#include <qdp_int.h>
#include <qdp_f.h>
#include <qdp_d.h>

  /* Headers we define regardless of precision */
#if ( QDP_Colors == 3 )
#include <qdp_df3.h>
#elif ( QDP_Colors == 2 )
#include <qdp_df2.h>
#elif ( QDP_Colors == 'N' )
#include <qdp_dfn.h>
#endif

  /* Headers we define regardless of color */
#if ( QDP_Precision == 'F' )
#include <qdp_f.h>
#elif ( QDP_Precision == 'D' )
#include <qdp_d.h>
#endif

  /* Headers specific to color and precision */
#if ( QDP_Precision == 'F' )

#if   ( QDP_Colors == 3 )
#include <qdp_f3.h>
#elif ( QDP_Colors == 2 )
#include <qdp_f2.h>
#elif ( QDP_Colors == 'N' )
#include <qdp_fn.h>
#endif

#elif ( QDP_Precision == 'D' )

#if   ( QDP_Colors == 3 )
#include <qdp_d3.h>
#elif ( QDP_Colors == 2 )
#include <qdp_d2.h>
#elif ( QDP_Colors == 'N' )
#include <qdp_dn.h>
#endif

#endif /* QDP_Precision */

#ifdef __cplusplus
}
#endif

#endif /* _QDP_H */
