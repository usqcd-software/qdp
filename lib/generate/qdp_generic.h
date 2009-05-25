/* QDP generic header for $PC */

#ifdef _QDP_$PORPC_GENERIC_H
#error already included a generic header: _QDP_$PORPC_GENERIC_H
#else
#define _QDP_$PORPC_GENERIC_H

!PCTYPES
#define $QDPTYPE $QDPPCTYPE
!END

!PCTYPES
#define QDP_read_$ABBR(out, md, field) QDP$PC_read_$ABBR(out, md, field)
#define QDP_vread_$ABBR(out, md, field, n) QDP$PC_vread_$ABBR(out, md, field, n)
#define QDP_write_$ABBR(out, md, field) QDP$PC_write_$ABBR(out, md, field)
#define QDP_vwrite_$ABBR(out, md, field, n) QDP$PC_vwrite_$ABBR(out, md, field, n)
#define QDP_vread_$QLAABBR(out, md, array, n) QDP$PC_vread_$QLAABBR($QDPNCVAR out, md, array, n)
#define QDP_vwrite_$QLAABBR(out, md, array, n) QDP$PC_vwrite_$QLAABBR($QDPNCVAR out, md, array, n)
!END
