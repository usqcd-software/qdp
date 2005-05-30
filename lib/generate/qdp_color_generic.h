/* QDP color generic header for $PC */

#ifdef _QDP_COLOR_GENERIC_H
#error already included a generic header: _QDP_COLOR_GENERIC_H
#else
#define _QDP_COLOR_GENERIC_H

!PCTYPES
#define $QDPPTYPE $QDPPCTYPE
!END

!PCTYPES
#define QDP_$P_read_$ABBR(out, md, field) QDP$PC_read_$ABBR($QDPNCVARout, md, field)
#define QDP_$P_write_$ABBR(out, md, field) QDP$PC_write_$ABBR($QDPNCVARout, md, field)
!END
