# Makefile.am for building the qdp_df library

AUTOMAKE_OPTIONS = no-dependencies

QDPSRC1 = \
QDP_DF_c1_eq_C_dot_C.c \
QDP_DF_c1_eq_C_dot_C_multi.c \
QDP_DF_c1_eq_sum_C.c \
QDP_DF_c1_eq_sum_C_multi.c \
QDP_DF_c1_veq_C_dot_C.c \
QDP_DF_c1_veq_C_dot_C_multi.c \
QDP_DF_C_eq_C.c \
QDP_DF_r1_eq_norm2_C.c \
QDP_DF_r1_eq_norm2_C_multi.c \
QDP_DF_r1_eq_norm2_R.c \
QDP_DF_r1_eq_norm2_R_multi.c \
QDP_DF_r1_eq_R_dot_R.c \
QDP_DF_r1_eq_R_dot_R_multi.c \
QDP_DF_r1_eq_re_C_dot_C.c \
QDP_DF_r1_eq_re_C_dot_C_multi.c \
QDP_DF_r1_eq_sum_R.c \
QDP_DF_r1_eq_sum_R_multi.c \
QDP_DF_r1_veq_norm2_C.c \
QDP_DF_r1_veq_norm2_C_multi.c \
QDP_DF_r1_veq_norm2_R.c \
QDP_DF_r1_veq_norm2_R_multi.c \
QDP_DF_r1_veq_R_dot_R.c \
QDP_DF_r1_veq_R_dot_R_multi.c \
QDP_DF_r1_veq_re_C_dot_C.c \
QDP_DF_r1_veq_re_C_dot_C_multi.c \
QDP_DF_R_eq_R.c \
QDP_FD_C_eq_C.c \
QDP_FD_R_eq_R.c

QDPOBJ1 = \
QDP_DF_c1_eq_C_dot_C.o \
QDP_DF_c1_eq_C_dot_C_multi.o \
QDP_DF_c1_eq_sum_C.o \
QDP_DF_c1_eq_sum_C_multi.o \
QDP_DF_c1_veq_C_dot_C.o \
QDP_DF_c1_veq_C_dot_C_multi.o \
QDP_DF_C_eq_C.o \
QDP_DF_r1_eq_norm2_C.o \
QDP_DF_r1_eq_norm2_C_multi.o \
QDP_DF_r1_eq_norm2_R.o \
QDP_DF_r1_eq_norm2_R_multi.o \
QDP_DF_r1_eq_R_dot_R.o \
QDP_DF_r1_eq_R_dot_R_multi.o \
QDP_DF_r1_eq_re_C_dot_C.o \
QDP_DF_r1_eq_re_C_dot_C_multi.o \
QDP_DF_r1_eq_sum_R.o \
QDP_DF_r1_eq_sum_R_multi.o \
QDP_DF_r1_veq_norm2_C.o \
QDP_DF_r1_veq_norm2_C_multi.o \
QDP_DF_r1_veq_norm2_R.o \
QDP_DF_r1_veq_norm2_R_multi.o \
QDP_DF_r1_veq_R_dot_R.o \
QDP_DF_r1_veq_R_dot_R_multi.o \
QDP_DF_r1_veq_re_C_dot_C.o \
QDP_DF_r1_veq_re_C_dot_C_multi.o \
QDP_DF_R_eq_R.o \
QDP_FD_C_eq_C.o \
QDP_FD_R_eq_R.o

QDPSRC = $(QDPSRC1)
QDPOBJ = $(QDPOBJ1)
QDPHDR = $(top_builddir)/include/qdp_df.h qdp_df_internal.h $(top_builddir)/include/qdp_df_profile.h

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

lib_LIBRARIES = ../libqdp_df.a

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(QLADIR)/include -I$(QIODIR)/include -I$(QMPDIR)/include

# this is just to force automake to include the .c.o rule
nodist____libqdp_df_a_SOURCES = QDP_DF_c1_eq_C_dot_C.c

nodist_include_HEADERS = $(QDPHDR)

source-stamp-c: $(BUILD_SCRIPT_C) $(BUILD_DEPS)
	@rm -f source-temp-c
	@touch source-temp-c
	$(BUILD_SCRIPT_C) df
	@mv -f source-temp-c $@

source-stamp-h: $(BUILD_SCRIPT_H) $(BUILD_DEPS)
	@rm -f source-temp-h
	@touch source-temp-h
	$(BUILD_SCRIPT_H) df $(top_builddir)/include
	@mv -f source-temp-h $@

$(QDPSRC): $(QDPHDR) source-stamp-c
	## Recover from the removal of $@
	@if test -f $@; then :; else \
	  trap 'rm -rf source-lock-c source-stamp-c' 1 2 13 15; \
	  if mkdir source-lock-c 2>/dev/null; then \
	    rm -f source-stamp-c; \
	    $(MAKE) $(AM_MAKEFLAGS) source-stamp-c; \
	    rmdir source-lock-c; \
	  else \
	    while test -d source-lock-c; do sleep 1; done; \
	    test -f source-stamp-c; exit $$?; \
	  fi; \
	fi

$(QDPHDR): source-stamp-h
	## Recover from the removal of $@
	@if test -f $@; then :; else \
	  trap 'rm -rf source-lock-h source-stamp-h' 1 2 13 15; \
	  if mkdir source-lock-h 2>/dev/null; then \
	    rm -f source-stamp-h; \
	    $(MAKE) $(AM_MAKEFLAGS) source-stamp-h; \
	    rmdir source-lock-h; \
	  else \
	    while test -d source-lock-h; do sleep 1; done; \
	    test -f source-stamp-h; exit $$?; \
	  fi; \
	fi

../libqdp_df.a: $(QDPHDR) $(QDPOBJ) ../optimized/Makefile
	-rm -f ../libqdp_df.a
	$(AR) $(ARFLAGS) ../libqdp_df.a $(QDPOBJ1)
	$(RANLIB) ../libqdp_df.a

clean-local:
	-rm -f $(QDPHDR) source-stamp-c source-stamp-h
	-rm -f $(QDPSRC1)
	-rm -f $(QDPOBJ1)
