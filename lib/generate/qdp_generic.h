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
#define QDP_vread_$ABBR(out, md, field, n) QDP$PC_vread_$ABBR($QDPNCVARout, md, field, n)
#define QDP_write_$ABBR(out, md, field) QDP$PC_write_$ABBR($QDPNCVARout, md, field)
#define QDP_vwrite_$ABBR(out, md, field, n) QDP$PC_vwrite_$ABBR($QDPNCVARout, md, field, n)
!END
