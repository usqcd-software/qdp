# Makefile.am for building the qdp_df3 library

AUTOMAKE_OPTIONS = no-dependencies

QDPSRC1 = \
QDP_DF3_c1_eq_D_dot_D.c \
QDP_DF3_c1_eq_D_dot_D_multi.c \
QDP_DF3_c1_eq_H_dot_H.c \
QDP_DF3_c1_eq_H_dot_H_multi.c \
QDP_DF3_c1_eq_Ma_dot_M.c \
QDP_DF3_c1_eq_Ma_dot_M_multi.c \
QDP_DF3_c1_eq_M_dot_M.c \
QDP_DF3_c1_eq_M_dot_M_multi.c \
QDP_DF3_c1_eq_Pa_dot_P.c \
QDP_DF3_c1_eq_Pa_dot_P_multi.c \
QDP_DF3_c1_eq_P_dot_P.c \
QDP_DF3_c1_eq_P_dot_P_multi.c \
QDP_DF3_c1_eq_V_dot_V.c \
QDP_DF3_c1_eq_V_dot_V_multi.c \
QDP_DF3_c1_veq_D_dot_D.c \
QDP_DF3_c1_veq_D_dot_D_multi.c \
QDP_DF3_c1_veq_H_dot_H.c \
QDP_DF3_c1_veq_H_dot_H_multi.c \
QDP_DF3_c1_veq_Ma_dot_M.c \
QDP_DF3_c1_veq_Ma_dot_M_multi.c \
QDP_DF3_c1_veq_M_dot_M.c \
QDP_DF3_c1_veq_M_dot_M_multi.c \
QDP_DF3_c1_veq_Pa_dot_P.c \
QDP_DF3_c1_veq_Pa_dot_P_multi.c \
QDP_DF3_c1_veq_P_dot_P.c \
QDP_DF3_c1_veq_P_dot_P_multi.c \
QDP_DF3_c1_veq_V_dot_V.c \
QDP_DF3_c1_veq_V_dot_V_multi.c \
QDP_DF3_d1_eq_sum_D.c \
QDP_DF3_d1_eq_sum_D_multi.c \
QDP_DF3_D_eq_D.c \
QDP_DF3_h1_eq_sum_H.c \
QDP_DF3_h1_eq_sum_H_multi.c \
QDP_DF3_H_eq_H.c \
QDP_DF3_m1_eq_sum_M.c \
QDP_DF3_m1_eq_sum_M_multi.c \
QDP_DF3_M_eq_M.c \
QDP_DF3_p1_eq_sum_P.c \
QDP_DF3_p1_eq_sum_P_multi.c \
QDP_DF3_P_eq_P.c \
QDP_DF3_r1_eq_norm2_D.c \
QDP_DF3_r1_eq_norm2_D_multi.c \
QDP_DF3_r1_eq_norm2_H.c \
QDP_DF3_r1_eq_norm2_H_multi.c \
QDP_DF3_r1_eq_norm2_M.c \
QDP_DF3_r1_eq_norm2_M_multi.c \
QDP_DF3_r1_eq_norm2_P.c \
QDP_DF3_r1_eq_norm2_P_multi.c \
QDP_DF3_r1_eq_norm2_V.c \
QDP_DF3_r1_eq_norm2_V_multi.c \
QDP_DF3_r1_eq_re_D_dot_D.c \
QDP_DF3_r1_eq_re_D_dot_D_multi.c \
QDP_DF3_r1_eq_re_H_dot_H.c \
QDP_DF3_r1_eq_re_H_dot_H_multi.c \
QDP_DF3_r1_eq_re_M_dot_M.c \
QDP_DF3_r1_eq_re_M_dot_M_multi.c \
QDP_DF3_r1_eq_re_P_dot_P.c \
QDP_DF3_r1_eq_re_P_dot_P_multi.c \
QDP_DF3_r1_eq_re_V_dot_V.c \
QDP_DF3_r1_eq_re_V_dot_V_multi.c \
QDP_DF3_r1_veq_norm2_D.c \
QDP_DF3_r1_veq_norm2_D_multi.c \
QDP_DF3_r1_veq_norm2_H.c \
QDP_DF3_r1_veq_norm2_H_multi.c \
QDP_DF3_r1_veq_norm2_M.c \
QDP_DF3_r1_veq_norm2_M_multi.c \
QDP_DF3_r1_veq_norm2_P.c \
QDP_DF3_r1_veq_norm2_P_multi.c \
QDP_DF3_r1_veq_norm2_V.c \
QDP_DF3_r1_veq_norm2_V_multi.c \
QDP_DF3_r1_veq_re_D_dot_D.c \
QDP_DF3_r1_veq_re_D_dot_D_multi.c \
QDP_DF3_r1_veq_re_H_dot_H.c \
QDP_DF3_r1_veq_re_H_dot_H_multi.c \
QDP_DF3_r1_veq_re_M_dot_M.c \
QDP_DF3_r1_veq_re_M_dot_M_multi.c \
QDP_DF3_r1_veq_re_P_dot_P.c \
QDP_DF3_r1_veq_re_P_dot_P_multi.c \
QDP_DF3_r1_veq_re_V_dot_V.c \
QDP_DF3_r1_veq_re_V_dot_V_multi.c \
QDP_DF3_v1_eq_sum_V.c \
QDP_DF3_v1_eq_sum_V_multi.c \
QDP_DF3_V_eq_V.c \
QDP_FD3_D_eq_D.c \
QDP_FD3_H_eq_H.c \
QDP_FD3_M_eq_M.c \
QDP_FD3_P_eq_P.c \
QDP_FD3_V_eq_V.c

QDPOBJ1 = \
QDP_DF3_c1_eq_D_dot_D.o \
QDP_DF3_c1_eq_D_dot_D_multi.o \
QDP_DF3_c1_eq_H_dot_H.o \
QDP_DF3_c1_eq_H_dot_H_multi.o \
QDP_DF3_c1_eq_Ma_dot_M.o \
QDP_DF3_c1_eq_Ma_dot_M_multi.o \
QDP_DF3_c1_eq_M_dot_M.o \
QDP_DF3_c1_eq_M_dot_M_multi.o \
QDP_DF3_c1_eq_Pa_dot_P.o \
QDP_DF3_c1_eq_Pa_dot_P_multi.o \
QDP_DF3_c1_eq_P_dot_P.o \
QDP_DF3_c1_eq_P_dot_P_multi.o \
QDP_DF3_c1_eq_V_dot_V.o \
QDP_DF3_c1_eq_V_dot_V_multi.o \
QDP_DF3_c1_veq_D_dot_D.o \
QDP_DF3_c1_veq_D_dot_D_multi.o \
QDP_DF3_c1_veq_H_dot_H.o \
QDP_DF3_c1_veq_H_dot_H_multi.o \
QDP_DF3_c1_veq_Ma_dot_M.o \
QDP_DF3_c1_veq_Ma_dot_M_multi.o \
QDP_DF3_c1_veq_M_dot_M.o \
QDP_DF3_c1_veq_M_dot_M_multi.o \
QDP_DF3_c1_veq_Pa_dot_P.o \
QDP_DF3_c1_veq_Pa_dot_P_multi.o \
QDP_DF3_c1_veq_P_dot_P.o \
QDP_DF3_c1_veq_P_dot_P_multi.o \
QDP_DF3_c1_veq_V_dot_V.o \
QDP_DF3_c1_veq_V_dot_V_multi.o \
QDP_DF3_d1_eq_sum_D.o \
QDP_DF3_d1_eq_sum_D_multi.o \
QDP_DF3_D_eq_D.o \
QDP_DF3_h1_eq_sum_H.o \
QDP_DF3_h1_eq_sum_H_multi.o \
QDP_DF3_H_eq_H.o \
QDP_DF3_m1_eq_sum_M.o \
QDP_DF3_m1_eq_sum_M_multi.o \
QDP_DF3_M_eq_M.o \
QDP_DF3_p1_eq_sum_P.o \
QDP_DF3_p1_eq_sum_P_multi.o \
QDP_DF3_P_eq_P.o \
QDP_DF3_r1_eq_norm2_D.o \
QDP_DF3_r1_eq_norm2_D_multi.o \
QDP_DF3_r1_eq_norm2_H.o \
QDP_DF3_r1_eq_norm2_H_multi.o \
QDP_DF3_r1_eq_norm2_M.o \
QDP_DF3_r1_eq_norm2_M_multi.o \
QDP_DF3_r1_eq_norm2_P.o \
QDP_DF3_r1_eq_norm2_P_multi.o \
QDP_DF3_r1_eq_norm2_V.o \
QDP_DF3_r1_eq_norm2_V_multi.o \
QDP_DF3_r1_eq_re_D_dot_D.o \
QDP_DF3_r1_eq_re_D_dot_D_multi.o \
QDP_DF3_r1_eq_re_H_dot_H.o \
QDP_DF3_r1_eq_re_H_dot_H_multi.o \
QDP_DF3_r1_eq_re_M_dot_M.o \
QDP_DF3_r1_eq_re_M_dot_M_multi.o \
QDP_DF3_r1_eq_re_P_dot_P.o \
QDP_DF3_r1_eq_re_P_dot_P_multi.o \
QDP_DF3_r1_eq_re_V_dot_V.o \
QDP_DF3_r1_eq_re_V_dot_V_multi.o \
QDP_DF3_r1_veq_norm2_D.o \
QDP_DF3_r1_veq_norm2_D_multi.o \
QDP_DF3_r1_veq_norm2_H.o \
QDP_DF3_r1_veq_norm2_H_multi.o \
QDP_DF3_r1_veq_norm2_M.o \
QDP_DF3_r1_veq_norm2_M_multi.o \
QDP_DF3_r1_veq_norm2_P.o \
QDP_DF3_r1_veq_norm2_P_multi.o \
QDP_DF3_r1_veq_norm2_V.o \
QDP_DF3_r1_veq_norm2_V_multi.o \
QDP_DF3_r1_veq_re_D_dot_D.o \
QDP_DF3_r1_veq_re_D_dot_D_multi.o \
QDP_DF3_r1_veq_re_H_dot_H.o \
QDP_DF3_r1_veq_re_H_dot_H_multi.o \
QDP_DF3_r1_veq_re_M_dot_M.o \
QDP_DF3_r1_veq_re_M_dot_M_multi.o \
QDP_DF3_r1_veq_re_P_dot_P.o \
QDP_DF3_r1_veq_re_P_dot_P_multi.o \
QDP_DF3_r1_veq_re_V_dot_V.o \
QDP_DF3_r1_veq_re_V_dot_V_multi.o \
QDP_DF3_v1_eq_sum_V.o \
QDP_DF3_v1_eq_sum_V_multi.o \
QDP_DF3_V_eq_V.o \
QDP_FD3_D_eq_D.o \
QDP_FD3_H_eq_H.o \
QDP_FD3_M_eq_M.o \
QDP_FD3_P_eq_P.o \
QDP_FD3_V_eq_V.o

QDPSRC = $(QDPSRC1)
QDPOBJ = $(QDPOBJ1)
QDPHDR = $(top_builddir)/include/qdp_df3.h qdp_df3_internal.h $(top_builddir)/include/qdp_df3_color_generic.h $(top_builddir)/include/qdp_df3_profile.h

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

lib_LIBRARIES = ../libqdp_df3.a

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(QLADIR)/include -I$(QIODIR)/include -I$(QMPDIR)/include

# this is just to force automake to include the .c.o rule
nodist____libqdp_df3_a_SOURCES = QDP_DF3_c1_eq_D_dot_D.c

nodist_include_HEADERS = $(QDPHDR)

source-stamp-c: $(BUILD_SCRIPT_C) $(BUILD_DEPS)
	@rm -f source-temp-c
	@touch source-temp-c
	$(BUILD_SCRIPT_C) df3
	@mv -f source-temp-c $@

source-stamp-h: $(BUILD_SCRIPT_H) $(BUILD_DEPS)
	@rm -f source-temp-h
	@touch source-temp-h
	$(BUILD_SCRIPT_H) df3 $(top_builddir)/include
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

../libqdp_df3.a: $(QDPHDR) $(QDPOBJ) ../optimized/Makefile
	-rm -f ../libqdp_df3.a
	$(AR) $(ARFLAGS) ../libqdp_df3.a $(QDPOBJ1)
	$(RANLIB) ../libqdp_df3.a

clean-local:
	-rm -f $(QDPHDR) source-stamp-c source-stamp-h
	-rm -f $(QDPSRC1)
	-rm -f $(QDPOBJ1)
