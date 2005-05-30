ALL_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(QLADIR)/include -I$(QIODIR)/include -I$(QMPDIR)/include
ALL_LDADD = -L$(top_builddir)/lib -L$(QLADIR)/lib -L$(QIODIR)/lib -L$(QMPDIR)/lib -lqdp_common -lqdp_int -lqla_int -lqla_random -lqio -llime -lqmp -lm

F3_CFLAGS = -DQDP_Precision=1 -DQDP_Nc=3 $(ALL_CFLAGS)
F3_LDADD = -lqdp_f -lqdp_f3 -lqla_f -lqla_f3 -lqla_d -lqla_d3 -lqla_df -lqla_df3 $(ALL_LDADD)

check_PROGRAMS = qdp_test_f3 qdpio_test_f3 plaquette_f3 sample_ks_f3 sample_wilson_f3

CLEANFILES = *.bin

EXTRA_DIST = README

qdp_test_f3_SOURCES = qdp_test.c
qdp_test_f3_CFLAGS = $(F3_CFLAGS)
qdp_test_f3_LDADD = $(F3_LDADD)

qdpio_test_f3_SOURCES = qdpio_test.c
qdpio_test_f3_CFLAGS = $(F3_CFLAGS)
qdpio_test_f3_LDADD = $(F3_LDADD)

plaquette_f3_SOURCES = plaquette.c
plaquette_f3_CFLAGS = $(F3_CFLAGS)
plaquette_f3_LDADD = $(F3_LDADD)

sample_ks_f3_SOURCES = sample_ks.c congrad_ks.c dslash_ks.c congrad_ks.h
sample_ks_f3_CFLAGS = $(F3_CFLAGS)
sample_ks_f3_LDADD = $(F3_LDADD)

sample_wilson_f3_SOURCES = sample_wilson.c congrad_wilson.c dslash_wilson.c congrad_wilson.h
sample_wilson_f3_CFLAGS = $(F3_CFLAGS)
sample_wilson_f3_LDADD = $(F3_LDADD)
