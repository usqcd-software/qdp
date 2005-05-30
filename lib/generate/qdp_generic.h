/* QDP generic header for $PC */

#ifdef _QDP_$PORPC_GENERIC_H
#error already included a generic header: _QDP_$PORPC_GENERIC_H
#else
#define _QDP_$PORPC_GENERIC_H

!PCTYPES
#define $QDPTYPE $QDPPCTYPE
!END

!PCTYPES
#define QDP_read_$ABBR(out, md, field) QDP$PC_read_$ABBR($QDPNCVARout, md, field)
#define QDP_write_$ABBR(out, md, field) QDP$PC_write_$ABBR($QDPNCVARout, md, field)
!END
