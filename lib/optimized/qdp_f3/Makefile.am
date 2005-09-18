CINC = -I$(top_srcdir)/include -I$(top_builddir)/include -I../../qdp_f3 -I$(QLADIR)/include -I$(QIODIR)/include -I$(QMPDIR)/include

OPT_LIBS = 
if NC3
  OPT_LIBS += libf3
endif
 
all: $(OPT_LIBS)
 
.PHONY: libf3
libf3:
	for f in $(srcdir)/src/*.c; do \
	  echo compiling $$f; \
	  $(CC) $(CFLAGS) $(CINC) -c $$f; \
	done
	-mv -f *.o ../../qdp_f3

EXTRA_DIST = src
