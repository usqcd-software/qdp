ALL_CFLAGS = $(CFLAGS) -I$(top_srcdir)/include -I$(top_builddir)/include -I$(QLADIR)/include -I$(QIODIR)/include $(QMP_CFLAGS) -DQDP_PROFILE
ALL_LDADD = -L$(top_builddir)/lib -L$(QLADIR)/lib -L$(QIODIR)/lib $(QMP_LDFLAGS) -lqdp_common -lqdp_int -lqla_int -lqla_random -lqio -llime $(QMP_LIBS) -lm

F3_CFLAGS = -DQDP_Precision=1 -DQDP_Nc=3 $(ALL_CFLAGS)
F3_LDADD = -lqdp_f -lqdp_f3 -lqla_f -lqla_f3 -lqla_d -lqla_d3 -lqla_df -lqla_df3 $(ALL_LDADD)

D3_CFLAGS = -DQDP_Precision=2 -DQDP_Nc=3 $(ALL_CFLAGS)
D3_LDADD = -lqdp_d -lqdp_d3 -lqla_d -lqla_d3 $(ALL_LDADD)

FN_CFLAGS = -DQDP_Precision=1 -DQDP_Nc=10 $(ALL_CFLAGS)
FN_LDADD = -lqdp_f -lqdp_fn -lqla_f -lqla_fn -lqla_d -lqla_dn -lqla_df -lqla_dfn $(ALL_LDADD)

check_PROGRAMS =
if NC3
check_PROGRAMS += \
 qdp_test_f3 qdp_test_d3 \
 qdp_test_L_f3 qdp_test_L_d3 \
 qdpio_test_f3 qdpio_test_d3 \
 qdpio_test_L_f3 qdpio_test_L_d3 \
 qdp_shifts_f3 \
 qdp_threads_f3 \
 qdp_bench_f3 qdp_bench_d3 \
 plaquette_f3 plaquette_d3 \
 sample_ks_f3 \
 sample_wilson_f3
endif
if NCN
check_PROGRAMS += \
 qdp_test_fn \
 qdp_test_L_fn \
 qdpio_test_fn \
 qdpio_test_L_fn \
 plaquette_fn
endif

CLEANFILES = *.bin

EXTRA_DIST = README

qdp_test_f3_SOURCES = qdp_test.c
qdp_test_f3_CFLAGS = $(F3_CFLAGS)
qdp_test_f3_LDADD = $(F3_LDADD)

qdp_test_d3_SOURCES = qdp_test.c
qdp_test_d3_CFLAGS = $(D3_CFLAGS)
qdp_test_d3_LDADD = $(D3_LDADD)

qdp_test_fn_SOURCES = qdp_test.c
qdp_test_fn_CFLAGS = $(FN_CFLAGS)
qdp_test_fn_LDADD = $(FN_LDADD)

qdp_test_L_f3_SOURCES = qdp_test_L.c
qdp_test_L_f3_CFLAGS = $(F3_CFLAGS)
qdp_test_L_f3_LDADD = $(F3_LDADD)

qdp_test_L_d3_SOURCES = qdp_test_L.c
qdp_test_L_d3_CFLAGS = $(D3_CFLAGS)
qdp_test_L_d3_LDADD = $(D3_LDADD)

qdp_test_L_fn_SOURCES = qdp_test_L.c
qdp_test_L_fn_CFLAGS = $(FN_CFLAGS)
qdp_test_L_fn_LDADD = $(FN_LDADD)

qdpio_test_f3_SOURCES = qdpio_test.c
qdpio_test_f3_CFLAGS = $(F3_CFLAGS)
qdpio_test_f3_LDADD = $(F3_LDADD)

qdpio_test_d3_SOURCES = qdpio_test.c
qdpio_test_d3_CFLAGS = $(D3_CFLAGS)
qdpio_test_d3_LDADD = $(D3_LDADD)

qdpio_test_fn_SOURCES = qdpio_test.c
qdpio_test_fn_CFLAGS = $(FN_CFLAGS)
qdpio_test_fn_LDADD = $(FN_LDADD)

qdpio_test_L_f3_SOURCES = qdpio_test_L.c
qdpio_test_L_f3_CFLAGS = $(F3_CFLAGS)
qdpio_test_L_f3_LDADD = $(F3_LDADD)

qdpio_test_L_d3_SOURCES = qdpio_test_L.c
qdpio_test_L_d3_CFLAGS = $(D3_CFLAGS)
qdpio_test_L_d3_LDADD = $(D3_LDADD)

qdpio_test_L_fn_SOURCES = qdpio_test_L.c
qdpio_test_L_fn_CFLAGS = $(FN_CFLAGS)
qdpio_test_L_fn_LDADD = $(FN_LDADD)

qdp_shifts_f3_SOURCES = qdp_shifts.c
qdp_shifts_f3_CFLAGS = $(F3_CFLAGS)
qdp_shifts_f3_LDADD = $(F3_LDADD)

qdp_threads_f3_SOURCES = qdp_threads.c
qdp_threads_f3_CFLAGS = $(F3_CFLAGS)
qdp_threads_f3_LDADD = $(F3_LDADD)

qdp_bench_f3_SOURCES = qdp_bench.c
qdp_bench_f3_CFLAGS = $(F3_CFLAGS)
qdp_bench_f3_LDADD = $(F3_LDADD)

qdp_bench_d3_SOURCES = qdp_bench.c
qdp_bench_d3_CFLAGS = $(D3_CFLAGS)
qdp_bench_d3_LDADD = $(D3_LDADD)

plaquette_f3_SOURCES = plaquette.c
plaquette_f3_CFLAGS = $(F3_CFLAGS)
plaquette_f3_LDADD = $(F3_LDADD)

plaquette_d3_SOURCES = plaquette.c
plaquette_d3_CFLAGS = $(D3_CFLAGS)
plaquette_d3_LDADD = $(D3_LDADD)

plaquette_fn_SOURCES = plaquette.c
plaquette_fn_CFLAGS = $(FN_CFLAGS)
plaquette_fn_LDADD = $(FN_LDADD)

sample_ks_f3_SOURCES = sample_ks.c congrad_ks.c dslash_ks.c congrad_ks.h
sample_ks_f3_CFLAGS = $(F3_CFLAGS)
sample_ks_f3_LDADD = $(F3_LDADD)

sample_wilson_f3_SOURCES = sample_wilson.c congrad_wilson.c dslash_wilson.c congrad_wilson.h
sample_wilson_f3_CFLAGS = $(F3_CFLAGS)
sample_wilson_f3_LDADD = $(F3_LDADD)
