/* QDP precision generic header for $PC */

#ifdef _QDP_$C_PRECISION_GENERIC_H
#error already included a generic header: _QDP_$C_PRECISION_GENERIC_H
#else
#define _QDP_$C_PRECISION_GENERIC_H

!PCTYPES
#define $QDPCTYPE $QDPPCTYPE
!END

!PCTYPES
#define QDP_$C_read_$ABBR(out, md, field) QDP$PC_read_$ABBR(out, md, field)
#define QDP_$C_vread_$ABBR(out, md, field, n) QDP$PC_vread_$ABBR(out, md, field, n)
#define QDP_$C_write_$ABBR(out, md, field) QDP$PC_write_$ABBR(out, md, field)
#define QDP_$C_vwrite_$ABBR(out, md, field, n) QDP$PC_vwrite_$ABBR(out, md, field, n)
#define QDP_$C_vread_$QLAABBR(out, md, array, n) QDP$PC_vread_$QLAABBR($QDPNCVAR out, md, array, n)
#define QDP_$C_vwrite_$QLAABBR(out, md, array, n) QDP$PC_vwrite_$QLAABBR($QDPNCVAR out, md, array, n)
!END
