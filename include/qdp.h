/* SciDAC Level 2 QDP/C Data Parallel library */

#ifndef _QDP_H
#define _QDP_H

/* The following macros specify the prevailing color and precision */
/* Defaults to SU(3) single precision */
#ifndef QDP_Precision
#define QDP_Precision 'F'
#endif

#ifndef QDP_Nc
#define QDP_Nc 3
#endif

/* allow numeric precision specification */
#if QDP_Precision == 1
#undef QDP_Precision
#define QDP_Precision 'F'
#endif
#if QDP_Precision == 2
#undef QDP_Precision
#define QDP_Precision 'D'
#endif

/* include QLA headers */
#ifndef QLA_Precision
#define QLA_Precision QDP_Precision
#endif

#ifndef QLA_Nc
#define QLA_Nc QDP_Nc
#endif

#include <qla.h>

/* Apply rule for default color namespace, based on QDP_Nc */
#ifdef QDP_Colors
#if (QDP_Colors != 'N') && (QDP_Colors != QDP_Nc)
#error "QDP_Colors" QDP_Colors != "QDP_Nc" QDP_Nc
#endif
#else
#if QDP_Nc == 2
#define QDP_Colors 2
#elif QDP_Nc == 3
#define QDP_Colors 3
#else
#define QDP_Colors 'N'
#endif
#endif

/* Define specific types and map them to generic types */
#include <qdp_types.h>

/* Headers we always define */
#include <qdp_io.h>
#include <qdp_int.h>
#include <qdp_f.h>
#include <qdp_d.h>
#include <qdp_f2.h>
#include <qdp_f3.h>
#include <qdp_fn.h>
#include <qdp_d2.h>
#include <qdp_d3.h>
#include <qdp_dn.h>

/* Headers we define regardless of color */
#if ( QDP_Precision == 'F' )
#include <qdp_f_generic.h>
#elif ( QDP_Precision == 'D' )
#include <qdp_d_generic.h>
#else
#error Invalid QDP_Precision
#endif

/* Headers specific to color and precision */

#if ( QDP_Precision == 'F' )

#if   ( QDP_Colors == 3 )
#include <qdp_f3_generic.h>
#include <qdp_f3_color_generic.h>
#elif ( QDP_Colors == 2 )
#include <qdp_f2_generic.h>
#include <qdp_f2_color_generic.h>
#elif ( QDP_Colors == 'N' )
#include <qdp_fn_generic.h>
#include <qdp_fn_color_generic.h>
#endif

#elif ( QDP_Precision == 'D' )

#if   ( QDP_Colors == 3 )
#include <qdp_d3_generic.h>
#include <qdp_d3_color_generic.h>
#elif ( QDP_Colors == 2 )
#include <qdp_d2_generic.h>
#include <qdp_d2_color_generic.h>
#elif ( QDP_Colors == 'N' )
#include <qdp_dn_generic.h>
#include <qdp_dn_color_generic.h>
#endif

#endif /* QDP_Precision */

#endif /* _QDP_H */
