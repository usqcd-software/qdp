# Makefile.am for building the qdp_int library

AUTOMAKE_OPTIONS = no-dependencies

QDPSRC1 = \
QDP_create_I.c \
QDP_create_S.c \
QDP_discard_I.c \
QDP_expose_I.c \
QDP_expose_S.c \
QDP_extract_I.c \
QDP_extract_S.c \
QDP_I_eq_func.c \
QDP_I_eq_I_and_I.c \
QDP_I_eq_i.c \
QDP_I_eq_I.c \
QDP_I_eq_I_divide_I.c \
QDP_I_eq_I_eq_I.c \
QDP_I_eq_I_ge_I.c \
QDP_I_eq_I_gt_I.c \
QDP_I_eq_I_le_I.c \
QDP_I_eq_I_lshift_I.c \
QDP_I_eq_I_lt_I.c \
QDP_I_eq_I_mask_I.c \
QDP_I_eq_I_max_I.c \
QDP_I_eq_I_min_I.c \
QDP_I_eq_I_minus_I.c \
QDP_I_eq_I_mod_I.c \
QDP_I_eq_I_ne_I.c \
QDP_I_eq_I_or_I.c \
QDP_I_eq_I_plus_I.c \
QDP_I_eq_I_rshift_I.c \
QDP_I_eq_i_times_I.c \
QDP_I_eq_I_times_I.c \
QDP_I_eq_i_times_I_minus_I.c \
QDP_I_eq_i_times_I_plus_I.c \
QDP_I_eq_I_xor_I.c \
QDP_I_eqm_I.c \
QDP_I_eqm_i_times_I.c \
QDP_I_eqm_I_times_I.c \
QDP_I_eq_not_I.c \
QDP_I_eq_sI.c \
QDP_I_eq_zero.c \
QDP_I_meq_I.c \
QDP_I_meq_i_times_I.c \
QDP_I_meq_I_times_I.c \
QDP_I_peq_I.c \
QDP_I_peq_i_times_I.c \
QDP_I_peq_I_times_I.c \
QDP_I_veq_I.c \
QDP_I_veqm_I.c \
QDP_I_veq_sI.c \
QDP_I_vmeq_I.c \
QDP_I_vpeq_I.c \
QDP_read_I.c \
QDP_read_S.c \
QDP_S_eq_S.c \
QDP_S_eq_seed_i_I.c \
QDP_S_veq_S.c \
QDP_write_I.c \
QDP_write_S.c

QDPOBJ1 = \
QDP_create_I.o \
QDP_create_S.o \
QDP_discard_I.o \
QDP_expose_I.o \
QDP_expose_S.o \
QDP_extract_I.o \
QDP_extract_S.o \
QDP_I_eq_func.o \
QDP_I_eq_I_and_I.o \
QDP_I_eq_i.o \
QDP_I_eq_I.o \
QDP_I_eq_I_divide_I.o \
QDP_I_eq_I_eq_I.o \
QDP_I_eq_I_ge_I.o \
QDP_I_eq_I_gt_I.o \
QDP_I_eq_I_le_I.o \
QDP_I_eq_I_lshift_I.o \
QDP_I_eq_I_lt_I.o \
QDP_I_eq_I_mask_I.o \
QDP_I_eq_I_max_I.o \
QDP_I_eq_I_min_I.o \
QDP_I_eq_I_minus_I.o \
QDP_I_eq_I_mod_I.o \
QDP_I_eq_I_ne_I.o \
QDP_I_eq_I_or_I.o \
QDP_I_eq_I_plus_I.o \
QDP_I_eq_I_rshift_I.o \
QDP_I_eq_i_times_I.o \
QDP_I_eq_I_times_I.o \
QDP_I_eq_i_times_I_minus_I.o \
QDP_I_eq_i_times_I_plus_I.o \
QDP_I_eq_I_xor_I.o \
QDP_I_eqm_I.o \
QDP_I_eqm_i_times_I.o \
QDP_I_eqm_I_times_I.o \
QDP_I_eq_not_I.o \
QDP_I_eq_sI.o \
QDP_I_eq_zero.o \
QDP_I_meq_I.o \
QDP_I_meq_i_times_I.o \
QDP_I_meq_I_times_I.o \
QDP_I_peq_I.o \
QDP_I_peq_i_times_I.o \
QDP_I_peq_I_times_I.o \
QDP_I_veq_I.o \
QDP_I_veqm_I.o \
QDP_I_veq_sI.o \
QDP_I_vmeq_I.o \
QDP_I_vpeq_I.o \
QDP_read_I.o \
QDP_read_S.o \
QDP_S_eq_S.o \
QDP_S_eq_seed_i_I.o \
QDP_S_veq_S.o \
QDP_write_I.o \
QDP_write_S.o

QDPSRC = $(QDPSRC1)
QDPOBJ = $(QDPOBJ1)
QDPHDR = $(top_builddir)/include/qdp_int.h qdp_int_internal.h $(top_builddir)/include/qdp_int_profile.h

BUILD_SCRIPT_DIR = $(top_srcdir)/lib/generate

BUILD_SCRIPT_C = $(BUILD_SCRIPT_DIR)/generate_sources
BUILD_SCRIPT_H = $(BUILD_SCRIPT_DIR)/generate_headers

BUILD_DEPS = \
	$(BUILD_SCRIPT_DIR)/generate_qdp.pl \
	$(BUILD_SCRIPT_DIR)/datatypes.pl \
	$(BUILD_SCRIPT_DIR)/functions.pl \
	$(BUILD_SCRIPT_DIR)/generate_types.pl \
	$(BUILD_SCRIPT_DIR)/generic.pl \
	$(BUILD_SCRIPT_DIR)/profile.pl

lib_LIBRARIES = ../libqdp_int.a

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(QLADIR)/include -I$(QIODIR)/include -I$(QMPDIR)/include

# this is just to force automake to include the .c.o rule
nodist____libqdp_int_a_SOURCES = QDP_create_I.c

nodist_include_HEADERS = $(QDPHDR)

$(QDPSRC): $(BUILD_SCRIPT_C) $(BUILD_DEPS)
	$(BUILD_SCRIPT_C) int

$(QDPHDR): $(BUILD_SCRIPT_H) $(BUILD_DEPS)
	$(BUILD_SCRIPT_H) int $(top_builddir)/include

../libqdp_int.a: $(QDPHDR) $(QDPOBJ) ../optimized/Makefile
	-rm -f ../libqdp_int.a
	$(AR) $(ARFLAGS) ../libqdp_int.a $(QDPOBJ1)
	$(RANLIB) ../libqdp_int.a

clean-local:
	-rm -f $(QDPHDR)
	-rm -f $(QDPSRC1)
	-rm -f $(QDPOBJ1)
