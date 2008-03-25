# Makefile.am for building the qdp_d library

AUTOMAKE_OPTIONS = no-dependencies

QDPSRC1 = \
QDP_D_c1_eq_C_dot_C.c \
QDP_D_c1_eq_C_dot_C_multi.c \
QDP_D_c1_eq_sum_C.c \
QDP_D_c1_eq_sum_C_multi.c \
QDP_D_c1_veq_C_dot_C.c \
QDP_D_c1_veq_C_dot_C_multi.c \
QDP_D_C_eq_c1.c \
QDP_D_C_eq_c1_times_C.c \
QDP_D_C_eq_Ca.c \
QDP_D_C_eq_C.c \
QDP_D_C_eq_C_divide_C.c \
QDP_D_C_eq_C_dot_C.c \
QDP_D_C_eq_cexp_C.c \
QDP_D_C_eq_cexpi_R.c \
QDP_D_C_eq_clog_C.c \
QDP_D_C_eq_C_mask_I.c \
QDP_D_C_eq_C_minus_C.c \
QDP_D_C_eq_conj_C.c \
QDP_D_C_eq_C_plus_C.c \
QDP_D_C_eq_csqrt_C.c \
QDP_D_C_eq_C_times_C.c \
QDP_D_C_eq_c_times_C_minus_C.c \
QDP_D_C_eq_c_times_C_plus_C.c \
QDP_D_C_eq_func.c \
QDP_D_C_eq_funci.c \
QDP_D_C_eq_gaussian_S.c \
QDP_D_C_eq_i_C.c \
QDP_D_C_eqm_c1_times_C.c \
QDP_D_C_eqm_Ca.c \
QDP_D_C_eqm_C.c \
QDP_D_C_eqm_C_dot_C.c \
QDP_D_C_eqm_conj_C.c \
QDP_D_C_eqm_C_times_C.c \
QDP_D_C_eqm_i_C.c \
QDP_D_C_eqm_r1_times_C.c \
QDP_D_C_eq_r1_times_C.c \
QDP_D_C_eq_R.c \
QDP_D_C_eq_R_plus_i_R.c \
QDP_D_C_eq_r_times_C_minus_C.c \
QDP_D_C_eq_r_times_C_plus_C.c \
QDP_D_C_eq_sC.c \
QDP_D_C_eq_zero.c \
QDP_D_C_meq_c1_times_C.c \
QDP_D_C_meq_Ca.c \
QDP_D_C_meq_C.c \
QDP_D_C_meq_C_dot_C.c \
QDP_D_C_meq_conj_C.c \
QDP_D_C_meq_C_times_C.c \
QDP_D_C_meq_i_C.c \
QDP_D_C_meq_r1_times_C.c \
QDP_D_C_peq_c1_times_C.c \
QDP_D_C_peq_Ca.c \
QDP_D_C_peq_C.c \
QDP_D_C_peq_C_dot_C.c \
QDP_D_C_peq_conj_C.c \
QDP_D_C_peq_C_times_C.c \
QDP_D_C_peq_i_C.c \
QDP_D_C_peq_r1_times_C.c \
QDP_D_create_C.c \
QDP_D_create_R.c \
QDP_D_C_veq_c1_times_C.c \
QDP_D_C_veq_C.c \
QDP_D_C_veq_C_minus_C.c \
QDP_D_C_veq_C_plus_C.c \
QDP_D_C_veq_c_times_C_minus_C.c \
QDP_D_C_veq_c_times_C_plus_C.c \
QDP_D_C_veqm_c1_times_C.c \
QDP_D_C_veqm_C.c \
QDP_D_C_veqm_r1_times_C.c \
QDP_D_C_veq_r1_times_C.c \
QDP_D_C_veq_r_times_C_minus_C.c \
QDP_D_C_veq_r_times_C_plus_C.c \
QDP_D_C_veq_sC.c \
QDP_D_C_vmeq_c1_times_C.c \
QDP_D_C_vmeq_C.c \
QDP_D_C_vmeq_r1_times_C.c \
QDP_D_C_vpeq_c1_times_C.c \
QDP_D_C_vpeq_C.c \
QDP_D_C_vpeq_r1_times_C.c \
QDP_D_discard_C.c \
QDP_D_discard_R.c \
QDP_D_expose_C.c \
QDP_D_expose_R.c \
QDP_D_extract_C.c \
QDP_D_extract_packed_C.c \
QDP_D_extract_packed_R.c \
QDP_D_extract_R.c \
QDP_D_I_eq_R_eq_R.c \
QDP_D_I_eq_R_ge_R.c \
QDP_D_I_eq_R_gt_R.c \
QDP_D_I_eq_R_le_R.c \
QDP_D_I_eq_R_lt_R.c \
QDP_D_I_eq_R_ne_R.c \
QDP_D_I_eq_round_R.c \
QDP_D_I_eq_trunc_R.c \
QDP_D_r1_eq_I_dot_I.c \
QDP_D_r1_eq_I_dot_I_multi.c \
QDP_D_r1_eq_norm2_C.c \
QDP_D_r1_eq_norm2_C_multi.c \
QDP_D_r1_eq_norm2_I.c \
QDP_D_r1_eq_norm2_I_multi.c \
QDP_D_r1_eq_norm2_R.c \
QDP_D_r1_eq_norm2_R_multi.c \
QDP_D_r1_eq_R_dot_R.c \
QDP_D_r1_eq_R_dot_R_multi.c \
QDP_D_r1_eq_re_C_dot_C.c \
QDP_D_r1_eq_re_C_dot_C_multi.c \
QDP_D_r1_eq_sum_I.c \
QDP_D_r1_eq_sum_I_multi.c \
QDP_D_r1_eq_sum_R.c \
QDP_D_r1_eq_sum_R_multi.c \
QDP_D_r1_veq_I_dot_I.c \
QDP_D_r1_veq_I_dot_I_multi.c \
QDP_D_r1_veq_norm2_C.c \
QDP_D_r1_veq_norm2_C_multi.c \
QDP_D_r1_veq_norm2_I.c \
QDP_D_r1_veq_norm2_I_multi.c \
QDP_D_r1_veq_norm2_R.c \
QDP_D_r1_veq_norm2_R_multi.c \
QDP_D_r1_veq_R_dot_R.c \
QDP_D_r1_veq_R_dot_R_multi.c \
QDP_D_r1_veq_re_C_dot_C.c \
QDP_D_r1_veq_re_C_dot_C_multi.c \
QDP_D_read_C.c \
QDP_D_read_R.c \
QDP_D_R_eq_acos_R.c \
QDP_D_R_eq_arg_C.c \
QDP_D_R_eq_asin_R.c \
QDP_D_R_eq_atan_R.c \
QDP_D_R_eq_ceil_R.c \
QDP_D_R_eq_cosh_R.c \
QDP_D_R_eq_cos_R.c \
QDP_D_R_eq_exp_R.c \
QDP_D_R_eq_fabs_R.c \
QDP_D_R_eq_floor_R.c \
QDP_D_R_eq_func.c \
QDP_D_R_eq_funci.c \
QDP_D_R_eq_gaussian_S.c \
QDP_D_R_eq_I.c \
QDP_D_R_eq_im_C.c \
QDP_D_R_eq_log10_R.c \
QDP_D_R_eq_log_R.c \
QDP_D_R_eqm_r1_times_R.c \
QDP_D_R_eqm_R.c \
QDP_D_R_eqm_re_C_dot_C.c \
QDP_D_R_eqm_R_times_R.c \
QDP_D_R_eq_norm2_C.c \
QDP_D_R_eq_norm_C.c \
QDP_D_R_eq_r1.c \
QDP_D_R_eq_r1_times_R.c \
QDP_D_R_eq_random_S.c \
QDP_D_R_eq_R_atan2_R.c \
QDP_D_R_eq_R.c \
QDP_D_R_eq_R_divide_R.c \
QDP_D_R_eq_re_C.c \
QDP_D_R_eq_re_C_dot_C.c \
QDP_D_R_eq_R_ldexp_I.c \
QDP_D_R_eq_R_mask_I.c \
QDP_D_R_eq_R_max_R.c \
QDP_D_R_eq_R_min_R.c \
QDP_D_R_eq_R_minus_R.c \
QDP_D_R_eq_R_mod_R.c \
QDP_D_R_eq_R_plus_R.c \
QDP_D_R_eq_R_pow_R.c \
QDP_D_R_eq_R_times_R.c \
QDP_D_R_eq_r_times_R_minus_R.c \
QDP_D_R_eq_r_times_R_plus_R.c \
QDP_D_R_eq_sign_R.c \
QDP_D_R_eq_sinh_R.c \
QDP_D_R_eq_sin_R.c \
QDP_D_R_eq_sqrt_R.c \
QDP_D_R_eq_sR.c \
QDP_D_R_eq_tanh_R.c \
QDP_D_R_eq_tan_R.c \
QDP_D_R_eq_zero.c \
QDP_D_R_meq_r1_times_R.c \
QDP_D_R_meq_R.c \
QDP_D_R_meq_re_C_dot_C.c \
QDP_D_R_meq_R_times_R.c \
QDP_D_R_peq_r1_times_R.c \
QDP_D_R_peq_R.c \
QDP_D_R_peq_re_C_dot_C.c \
QDP_D_R_peq_R_times_R.c \
QDP_D_R_veqm_r1_times_R.c \
QDP_D_R_veqm_R.c \
QDP_D_R_veq_r1_times_R.c \
QDP_D_R_veq_R.c \
QDP_D_R_veq_R_minus_R.c \
QDP_D_R_veq_R_plus_R.c \
QDP_D_R_veq_r_times_R_minus_R.c \
QDP_D_R_veq_r_times_R_plus_R.c \
QDP_D_R_veq_sR.c \
QDP_D_R_vmeq_r1_times_R.c \
QDP_D_R_vmeq_R.c \
QDP_D_R_vpeq_r1_times_R.c \
QDP_D_R_vpeq_R.c \
QDP_D_write_C.c \
QDP_D_write_R.c

QDPOBJ1 = \
QDP_D_c1_eq_C_dot_C.o \
QDP_D_c1_eq_C_dot_C_multi.o \
QDP_D_c1_eq_sum_C.o \
QDP_D_c1_eq_sum_C_multi.o \
QDP_D_c1_veq_C_dot_C.o \
QDP_D_c1_veq_C_dot_C_multi.o \
QDP_D_C_eq_c1.o \
QDP_D_C_eq_c1_times_C.o \
QDP_D_C_eq_Ca.o \
QDP_D_C_eq_C.o \
QDP_D_C_eq_C_divide_C.o \
QDP_D_C_eq_C_dot_C.o \
QDP_D_C_eq_cexp_C.o \
QDP_D_C_eq_cexpi_R.o \
QDP_D_C_eq_clog_C.o \
QDP_D_C_eq_C_mask_I.o \
QDP_D_C_eq_C_minus_C.o \
QDP_D_C_eq_conj_C.o \
QDP_D_C_eq_C_plus_C.o \
QDP_D_C_eq_csqrt_C.o \
QDP_D_C_eq_C_times_C.o \
QDP_D_C_eq_c_times_C_minus_C.o \
QDP_D_C_eq_c_times_C_plus_C.o \
QDP_D_C_eq_func.o \
QDP_D_C_eq_funci.o \
QDP_D_C_eq_gaussian_S.o \
QDP_D_C_eq_i_C.o \
QDP_D_C_eqm_c1_times_C.o \
QDP_D_C_eqm_Ca.o \
QDP_D_C_eqm_C.o \
QDP_D_C_eqm_C_dot_C.o \
QDP_D_C_eqm_conj_C.o \
QDP_D_C_eqm_C_times_C.o \
QDP_D_C_eqm_i_C.o \
QDP_D_C_eqm_r1_times_C.o \
QDP_D_C_eq_r1_times_C.o \
QDP_D_C_eq_R.o \
QDP_D_C_eq_R_plus_i_R.o \
QDP_D_C_eq_r_times_C_minus_C.o \
QDP_D_C_eq_r_times_C_plus_C.o \
QDP_D_C_eq_sC.o \
QDP_D_C_eq_zero.o \
QDP_D_C_meq_c1_times_C.o \
QDP_D_C_meq_Ca.o \
QDP_D_C_meq_C.o \
QDP_D_C_meq_C_dot_C.o \
QDP_D_C_meq_conj_C.o \
QDP_D_C_meq_C_times_C.o \
QDP_D_C_meq_i_C.o \
QDP_D_C_meq_r1_times_C.o \
QDP_D_C_peq_c1_times_C.o \
QDP_D_C_peq_Ca.o \
QDP_D_C_peq_C.o \
QDP_D_C_peq_C_dot_C.o \
QDP_D_C_peq_conj_C.o \
QDP_D_C_peq_C_times_C.o \
QDP_D_C_peq_i_C.o \
QDP_D_C_peq_r1_times_C.o \
QDP_D_create_C.o \
QDP_D_create_R.o \
QDP_D_C_veq_c1_times_C.o \
QDP_D_C_veq_C.o \
QDP_D_C_veq_C_minus_C.o \
QDP_D_C_veq_C_plus_C.o \
QDP_D_C_veq_c_times_C_minus_C.o \
QDP_D_C_veq_c_times_C_plus_C.o \
QDP_D_C_veqm_c1_times_C.o \
QDP_D_C_veqm_C.o \
QDP_D_C_veqm_r1_times_C.o \
QDP_D_C_veq_r1_times_C.o \
QDP_D_C_veq_r_times_C_minus_C.o \
QDP_D_C_veq_r_times_C_plus_C.o \
QDP_D_C_veq_sC.o \
QDP_D_C_vmeq_c1_times_C.o \
QDP_D_C_vmeq_C.o \
QDP_D_C_vmeq_r1_times_C.o \
QDP_D_C_vpeq_c1_times_C.o \
QDP_D_C_vpeq_C.o \
QDP_D_C_vpeq_r1_times_C.o \
QDP_D_discard_C.o \
QDP_D_discard_R.o \
QDP_D_expose_C.o \
QDP_D_expose_R.o \
QDP_D_extract_C.o \
QDP_D_extract_packed_C.o \
QDP_D_extract_packed_R.o \
QDP_D_extract_R.o \
QDP_D_I_eq_R_eq_R.o \
QDP_D_I_eq_R_ge_R.o \
QDP_D_I_eq_R_gt_R.o \
QDP_D_I_eq_R_le_R.o \
QDP_D_I_eq_R_lt_R.o \
QDP_D_I_eq_R_ne_R.o \
QDP_D_I_eq_round_R.o \
QDP_D_I_eq_trunc_R.o \
QDP_D_r1_eq_I_dot_I.o \
QDP_D_r1_eq_I_dot_I_multi.o \
QDP_D_r1_eq_norm2_C.o \
QDP_D_r1_eq_norm2_C_multi.o \
QDP_D_r1_eq_norm2_I.o \
QDP_D_r1_eq_norm2_I_multi.o \
QDP_D_r1_eq_norm2_R.o \
QDP_D_r1_eq_norm2_R_multi.o \
QDP_D_r1_eq_R_dot_R.o \
QDP_D_r1_eq_R_dot_R_multi.o \
QDP_D_r1_eq_re_C_dot_C.o \
QDP_D_r1_eq_re_C_dot_C_multi.o \
QDP_D_r1_eq_sum_I.o \
QDP_D_r1_eq_sum_I_multi.o \
QDP_D_r1_eq_sum_R.o \
QDP_D_r1_eq_sum_R_multi.o \
QDP_D_r1_veq_I_dot_I.o \
QDP_D_r1_veq_I_dot_I_multi.o \
QDP_D_r1_veq_norm2_C.o \
QDP_D_r1_veq_norm2_C_multi.o \
QDP_D_r1_veq_norm2_I.o \
QDP_D_r1_veq_norm2_I_multi.o \
QDP_D_r1_veq_norm2_R.o \
QDP_D_r1_veq_norm2_R_multi.o \
QDP_D_r1_veq_R_dot_R.o \
QDP_D_r1_veq_R_dot_R_multi.o \
QDP_D_r1_veq_re_C_dot_C.o \
QDP_D_r1_veq_re_C_dot_C_multi.o \
QDP_D_read_C.o \
QDP_D_read_R.o \
QDP_D_R_eq_acos_R.o \
QDP_D_R_eq_arg_C.o \
QDP_D_R_eq_asin_R.o \
QDP_D_R_eq_atan_R.o \
QDP_D_R_eq_ceil_R.o \
QDP_D_R_eq_cosh_R.o \
QDP_D_R_eq_cos_R.o \
QDP_D_R_eq_exp_R.o \
QDP_D_R_eq_fabs_R.o \
QDP_D_R_eq_floor_R.o \
QDP_D_R_eq_func.o \
QDP_D_R_eq_funci.o \
QDP_D_R_eq_gaussian_S.o \
QDP_D_R_eq_I.o \
QDP_D_R_eq_im_C.o \
QDP_D_R_eq_log10_R.o \
QDP_D_R_eq_log_R.o \
QDP_D_R_eqm_r1_times_R.o \
QDP_D_R_eqm_R.o \
QDP_D_R_eqm_re_C_dot_C.o \
QDP_D_R_eqm_R_times_R.o \
QDP_D_R_eq_norm2_C.o \
QDP_D_R_eq_norm_C.o \
QDP_D_R_eq_r1.o \
QDP_D_R_eq_r1_times_R.o \
QDP_D_R_eq_random_S.o \
QDP_D_R_eq_R_atan2_R.o \
QDP_D_R_eq_R.o \
QDP_D_R_eq_R_divide_R.o \
QDP_D_R_eq_re_C.o \
QDP_D_R_eq_re_C_dot_C.o \
QDP_D_R_eq_R_ldexp_I.o \
QDP_D_R_eq_R_mask_I.o \
QDP_D_R_eq_R_max_R.o \
QDP_D_R_eq_R_min_R.o \
QDP_D_R_eq_R_minus_R.o \
QDP_D_R_eq_R_mod_R.o \
QDP_D_R_eq_R_plus_R.o \
QDP_D_R_eq_R_pow_R.o \
QDP_D_R_eq_R_times_R.o \
QDP_D_R_eq_r_times_R_minus_R.o \
QDP_D_R_eq_r_times_R_plus_R.o \
QDP_D_R_eq_sign_R.o \
QDP_D_R_eq_sinh_R.o \
QDP_D_R_eq_sin_R.o \
QDP_D_R_eq_sqrt_R.o \
QDP_D_R_eq_sR.o \
QDP_D_R_eq_tanh_R.o \
QDP_D_R_eq_tan_R.o \
QDP_D_R_eq_zero.o \
QDP_D_R_meq_r1_times_R.o \
QDP_D_R_meq_R.o \
QDP_D_R_meq_re_C_dot_C.o \
QDP_D_R_meq_R_times_R.o \
QDP_D_R_peq_r1_times_R.o \
QDP_D_R_peq_R.o \
QDP_D_R_peq_re_C_dot_C.o \
QDP_D_R_peq_R_times_R.o \
QDP_D_R_veqm_r1_times_R.o \
QDP_D_R_veqm_R.o \
QDP_D_R_veq_r1_times_R.o \
QDP_D_R_veq_R.o \
QDP_D_R_veq_R_minus_R.o \
QDP_D_R_veq_R_plus_R.o \
QDP_D_R_veq_r_times_R_minus_R.o \
QDP_D_R_veq_r_times_R_plus_R.o \
QDP_D_R_veq_sR.o \
QDP_D_R_vmeq_r1_times_R.o \
QDP_D_R_vmeq_R.o \
QDP_D_R_vpeq_r1_times_R.o \
QDP_D_R_vpeq_R.o \
QDP_D_write_C.o \
QDP_D_write_R.o

QDPSRC = $(QDPSRC1)
QDPOBJ = $(QDPOBJ1)
QDPHDR = $(top_builddir)/include/qdp_d.h qdp_d_internal.h $(top_builddir)/include/qdp_d_generic.h $(top_builddir)/include/qdp_d_profile.h

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

lib_LIBRARIES = ../libqdp_d.a

AM_CFLAGS = -I$(top_srcdir)/include -I$(top_builddir)/include -I$(QLADIR)/include -I$(QIODIR)/include -I$(QMPDIR)/include

# this is just to force automake to include the .c.o rule
nodist____libqdp_d_a_SOURCES = QDP_D_c1_eq_C_dot_C.c

nodist_include_HEADERS = $(QDPHDR)

source-stamp-c: $(BUILD_SCRIPT_C) $(BUILD_DEPS)
	@rm -f source-temp-c
	@touch source-temp-c
	$(BUILD_SCRIPT_C) d
	@mv -f source-temp-c $@

source-stamp-h: $(BUILD_SCRIPT_H) $(BUILD_DEPS)
	@rm -f source-temp-h
	@touch source-temp-h
	$(BUILD_SCRIPT_H) d $(top_builddir)/include
	@mv -f source-temp-h $@

$(QDPSRC): source-stamp-c
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


../libqdp_d.a: $(QDPHDR) $(QDPOBJ) ../optimized/Makefile
	-rm -f ../libqdp_d.a
	$(AR) $(ARFLAGS) ../libqdp_d.a $(QDPOBJ1)
	$(RANLIB) ../libqdp_d.a

clean-local:
	-rm -f $(QDPHDR)
	-rm -f $(QDPSRC1)
	-rm -f $(QDPOBJ1)
