# header files to be installed for the user

GENDIR = $(top_srcdir)/lib/generate
GEN_TYPES = $(GENDIR)/generate_types.pl
INCDIR = $(top_builddir)/include
 
include_HEADERS = qdp.h \
        qdp_common.h \
	qdp_layout.h \
	qdp_subset.h \
	qdp_shift.h \
	qdp_string.h

BUILT_SOURCES = qdp_types.h qdp_io.h

nodist_include_HEADERS = $(BUILT_SOURCES)

EXTRA_DIST = \
	com_common.h \
	com_specific.h \
	com_common_internal.h \
	qdp_shift_internal.h \
	qdp_subset_internal.h

qdp_types.h: $(GENDIR)/qdp_types.h $(GEN_TYPES)
	$(PERL) $(GEN_TYPES) common $(GENDIR)/$@ $(INCDIR)/$@

qdp_io.h: $(GENDIR)/qdp_io.h $(GEN_TYPES)
	$(PERL) $(GEN_TYPES) common $(GENDIR)/$@ $(INCDIR)/$@

clean-local:
	-rm -f $(BUILT_SOURCES)

