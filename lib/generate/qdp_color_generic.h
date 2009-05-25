/* QDP color generic header for $PC */

#ifdef _QDP_$P_COLOR_GENERIC_H
#error already included a generic header: _QDP_$P_COLOR_GENERIC_H
#else
#define _QDP_$P_COLOR_GENERIC_H

!PCTYPES
#define $QDPPTYPE $QDPPCTYPE
!END

!PCTYPES
#define QDP_$P_read_$ABBR(out, md, field) QDP$PC_read_$ABBR(out, md, field)
#define QDP_$P_vread_$ABBR(out, md, field, n) QDP$PC_vread_$ABBR(out, md, field, n)
#define QDP_$P_write_$ABBR(out, md, field) QDP$PC_write_$ABBR(out, md, field)
#define QDP_$P_vwrite_$ABBR(out, md, field, n) QDP$PC_vwrite_$ABBR(out, md, field, n)
#define QDP_$P_vread_$QLAABBR(out, md, array, n) QDP$PC_vread_$QLAABBR($QDPNCVAR out, md, array, n)
#define QDP_$P_vwrite_$QLAABBR(out, md, array, n) QDP$PC_vwrite_$QLAABBR($QDPNCVAR out, md, array, n)
!END
