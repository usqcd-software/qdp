# For making Postscript and html documentation

info_TEXINFOS = qdpc.texi
qdpc.texi = qdpdefs.texi functions.texi

EXTRA_DIST = README make_function_tables.pl qdp_1p1.tex

#PERL = @PERL@
#TEXI2DVI = @TEXI2DVI@
#DVIPS = @DVIPS@
#MAKEINFO = @MAKEINFO@

#HTMLDIR = html
#PS = qdpc.ps
#INFO = qdpc.info
#SRC = $(srcdir)/qdpc.texi
#SRCDEP = ${SRC} $(srcdir)/qdpdefs.texi ${FUNC}
#DVI = qdpc.dvi
#FUNC = $(srcdir)/functions.texi
#MFUNC = $(srcdir)/make_function_tables.pl
#FUNCSRC = $(top_srcdir)/lib/generate/functions.pl
#FUNCDEP = ${FUNC} ${MFUNC} ${FUNCSRC}

#.PHONY: ps info
#ps: ${PS}
#info: ${INFO}

#${PS}: ${SRCDEP}
#	${TEXI2DVI} ${SRC}
#	${DVIPS} ${DVI} -o

#${INFO}: ${SRCDEP}
#	${MAKEINFO} ${SRC}

#${HTMLDIR}: ${SRCDEP}
#	-rm -rf $@
#	mkdir $@
#	${MAKEINFO} --html -o ${HTMLDIR} ${SRC}

#${FUNC}:
#	${PERL} ${MFUNC}

# Removes TeXinfo debris
#doc-clean:
#	-/bin/rm -f *.aux *.log *.err *.dvi *.cp *.cps *.fn *.ky *.pg *.tmp *.toc *.tp *.vr

# Removes all but the source files
#doc-distclean: doc-clean
#	-/bin/rm -f *.info *.ps
#	-/bin/rm -rf ${HTMLDIR}

# also clean on regular make clean and distclean
#clean-local: doc-clean
#distclean-local: doc-distclean

#EXTRA_DIST = make_function_tables.pl qdpc.texi qdpdefs.texi functions.texi \
#             README qdp_1p1.tex

