QDP_C0_DIRS = qdp_f qdp_d qdp_df
QDP_C2_DIRS = qdp_f2 qdp_d2 qdp_df2
QDP_C3_DIRS = qdp_f3 qdp_d3 qdp_df3
QDP_CN_DIRS = qdp_fn qdp_dn qdp_dfn
QDP_ALL_DIRS = $(QDP_C0_DIRS) $(QDP_C2_DIRS) $(QDP_C3_DIRS) $(QDP_CN_DIRS)

QDP_DIRS = $(QDP_C0_DIRS)
if NC2
  QDP_DIRS += $(QDP_C2_DIRS)
endif
if NC3
  QDP_DIRS += $(QDP_C3_DIRS)
endif
if NCN
  QDP_DIRS += $(QDP_CN_DIRS)
endif

SUBDIRS = \
	qdp_common \
	qdp_int \
	$(QDP_DIRS) \
	optimized \
	$(QDP_DIRS)

EXTRA_DIST = make_qdp_dirs generate
