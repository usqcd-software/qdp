# Makefile.am for building the qdp_common library

#AUTOMAKE_OPTIONS = no-dependencies

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(QLADIR)/include -I$(QIODIR)/include -I$(QMPDIR)/include

INCDIR = $(top_builddir)/include
GENDIR = $(top_srcdir)/lib/generate
GEN_TYPES = $(GENDIR)/generate_types.pl
GEN_TYPES_DEP = $(GEN_TYPES) $(GENDIR)/datatypes.pl

lib_LIBRARIES = ../libqdp_common.a

___libqdp_common_a_SOURCES = \
  qdp_common.c \
  qdp_internal.c \
  com_common.c \
  layout_hyper.c \
  layout_shiftopt.c \
  qdp_layout.c \
  qdp_shift.c \
  qdp_subset.c \
  qdp_thread.c

#nodist_libqdp_common_a_SOURCES = \
#  ../include/qdp_common_internal.h

BUILT_SOURCES = \
  $(INCDIR)/qdp_common_internal.h

$(INCDIR)/qdp_common_internal.h: $(GENDIR)/qdp_common_internal.h $(GEN_TYPES_DEP)
	$(PERL) $(GEN_TYPES) common $(GENDIR)/qdp_common_internal.h $@

clean-local:
	-rm -f $(BUILT_SOURCES)
