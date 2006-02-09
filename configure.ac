# Process this file with autoconf to produce a configure script.

AC_INIT([qdp],[1.5.4],[josborn@physics.bu.edu])
AC_CONFIG_AUX_DIR(config)
AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR(include/qdp.h)
AC_CONFIG_SRCDIR(lib/qdp_common/qdp_common.c)
AM_CONFIG_HEADER([include/qdp_config.h])

dnl --enable-sse
AC_ARG_ENABLE(
  sse,
  AC_HELP_STRING([--enable-sse],
    [use SSE instructions in certain routines (requires gcc)]),
  AC_SUBST(OPT_SSE, [$enableval]),
  AC_SUBST(OPT_SSE, [no])
)

dnl --enable-nc=<ColorLib>
AC_ARG_ENABLE(
  nc,
  AC_HELP_STRING([--enable-nc],
    [make just the libraries necessary for the given color]),
  AC_SUBST(NC, [$enableval]),
  AC_SUBST(NC, [all])
)
if test "X$NC" = Xn -o "X$NC" = X1; then NC="N"; fi
if test `expr match $NC '[0-9]*$' \& $NC \> 3` -ne 0; then NC="N"; fi
if test "X$NC" != X2 -a "X$NC" != X3 -a "X$NC" != XN; then NC="all"; fi 

QMPDIR="other_libs/qmp"
AC_ARG_WITH(
  qmp,
  AC_HELP_STRING([--with-qmp=QMP_DIR],
    [location of QMP libraries]),
  [QMPDIR="$with_qmp"; WITH_QMP=1]
)
AC_SUBST(QMPDIR)

AC_ARG_WITH(
  qio,
  AC_HELP_STRING([--with-qio=QIO_DIR],
    [location of QIO libraries]),
  [QIODIR="$with_qio"; WITH_QIO=1]
)
AC_SUBST(QIODIR)

AC_ARG_WITH(
  qla,
  AC_HELP_STRING([--with-qla=QLA_DIR],
    [location of QLA libraries]),
  [QLADIR="$with_qla"; WITH_QLA=1]
)
AC_SUBST(QLADIR)

# get QMP configuration
if test "X${QMPDIR}X" = "XX" ; then
  AC_PATH_PROG([QMP_CONFIG], [qmp-config], [], [${PATH}])
else
  AC_PATH_PROG([QMP_CONFIG], [qmp-config], [], [${QMPDIR}/bin ${PATH}])
fi

if test "X${QMP_CONFIG}X" != "XX" ; then
  AC_MSG_NOTICE([Found QMP configuration program ${QMP_CONFIG}])
  # use the QMP CC unless it is explicitly set by the user
  if ! set | grep -q '^CC=' ; then
    AC_SUBST(CC,  "`${QMP_CONFIG} --cc`")
    AC_MSG_NOTICE([QMP compiler: ${CC}])
  fi
  AC_SUBST(QMP_CFLAGS,  "`${QMP_CONFIG} --cflags`")
  AC_MSG_NOTICE([QMP compile flags: ${QMP_CFLAGS}])
  AC_SUBST(QMP_LDFLAGS, "`${QMP_CONFIG} --ldflags`")
  AC_MSG_NOTICE([QMP linking flags: ${QMP_LDFLAGS}])
  AC_SUBST(QMP_LIBS,    "`${QMP_CONFIG} --libs`")
  AC_MSG_NOTICE([QMP libraries flags: ${QMP_LIBS}])
else
  AC_MSG_WARN([QMP configuration program qmp-config not found.])
  AC_MSG_WARN([Set environment variables QMP_CFLAGS QMP_LDFAGS QMP_LIBS
    before configure])
fi

# Checks for programs.
AC_CHECK_TOOL(AR, ar, [ar])
AC_PROG_RANLIB
AC_PATH_PROG([PERL], [perl], [perl])
AC_PATH_PROG([TEXI2DVI], [texi2dvi], [texi2dvi])
AC_PATH_PROG([DVIPS], [dvips], [dvips])
AC_PATH_PROG([MAKEINFO], [makeinfo], [makeinfo])
if ! set | grep '^CFLAGS='; then ac_CFLAGS_not_set=1; fi
AC_PROG_CC([c99 gcc])
if test "X$ac_CFLAGS_not_set" = "X1"; then
  if test "X$GCC" = "Xyes"; then
    CFLAGS="-std=c99 -O2"
  else
    CFLAGS=-O
  fi
fi
if test "X$GCC" = "Xyes"; then GNUCC="$CC"; else
AC_PATH_PROG([GNUCC], [gcc], [gcc])
fi
CFLAGS="$CFLAGS $QMP_CFLAGS"

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

AC_C_RESTRICT

# Checks for library functions.

# set makefile variables
AM_CONDITIONAL(OPT_SSE, [test "X$OPT_SSE" != "Xno"])
AM_CONDITIONAL(NC2, [test "X$NC" = "X2" -o "X$NC" = "Xall"])
AM_CONDITIONAL(NC3, [test "X$NC" = "X3" -o "X$NC" = "Xall"])
AM_CONDITIONAL(NCN, [test "X$NC" = "XN" -o "X$NC" = "Xall"])
AM_CONDITIONAL(WITH_QMP, [test "X$WITH_QMP" = "X1"])

# pass configuration to distcheck target
DISTCHECK_CONFIGURE_FLAGS="--enable-nc=$NC"
if test -n "$WITH_QMP"; then
  DISTCHECK_CONFIGURE_FLAGS="$DISTCHECK_CONFIGURE_FLAGS --with-qmp=$QMPDIR"
fi
if test -n "$WITH_QIO"; then
  DISTCHECK_CONFIGURE_FLAGS="$DISTCHECK_CONFIGURE_FLAGS --with-qio=$QIODIR"
fi
if test -n "$WITH_QLA"; then
  DISTCHECK_CONFIGURE_FLAGS="$DISTCHECK_CONFIGURE_FLAGS --with-qla=$QLADIR"
fi
AC_SUBST(DISTCHECK_CONFIGURE_FLAGS)

# export CC and CFLAGS so that the other_libs will use the same
export CC CFLAGS

if test "X$WITH_QMP" = "X"; then
  AC_CONFIG_SUBDIRS(other_libs/qmp)
fi

# configuration files
AC_CONFIG_FILES(Makefile)
AC_CONFIG_FILES(doc/Makefile)
AC_CONFIG_FILES(include/Makefile)
AC_CONFIG_FILES(lib/Makefile)
AC_CONFIG_FILES(lib/qdp_common/Makefile)
AC_CONFIG_FILES(lib/qdp_int/Makefile)
AC_CONFIG_FILES(lib/qdp_f/Makefile)
AC_CONFIG_FILES(lib/qdp_f2/Makefile)
AC_CONFIG_FILES(lib/qdp_f3/Makefile)
AC_CONFIG_FILES(lib/qdp_fn/Makefile)
AC_CONFIG_FILES(lib/qdp_d/Makefile)
AC_CONFIG_FILES(lib/qdp_d2/Makefile)
AC_CONFIG_FILES(lib/qdp_d3/Makefile)
AC_CONFIG_FILES(lib/qdp_dn/Makefile)
AC_CONFIG_FILES(lib/qdp_df/Makefile)
AC_CONFIG_FILES(lib/qdp_df2/Makefile)
AC_CONFIG_FILES(lib/qdp_df3/Makefile)
AC_CONFIG_FILES(lib/qdp_dfn/Makefile)
AC_CONFIG_FILES(lib/optimized/Makefile)
AC_CONFIG_FILES(lib/optimized/qdp_f3/Makefile)
AC_CONFIG_FILES(examples/Makefile)
AC_CONFIG_FILES(other_libs/Makefile)

AC_OUTPUT
