CINC = -I$(top_srcdir)/include -I$(top_builddir)/include -I../../qdp_f3 -I$(QLADIR)/include -I$(QIODIR)/include -I$(QMPDIR)/include

OPT_LIBS = 
if NC3
  OPT_LIBS += libf3
endif
 
all: $(OPT_LIBS)
 
.PHONY: libf3
libf3:
	if test -n "`find $(srcdir)/src -maxdepth 1 -name QDP_F3_\*.c -print -quit`"; then \
	  for f in $(srcdir)/src/QDP_F3_*.c; do \
	    if [ -f $f ]; then \
	      echo compiling $$f; \
	      echo $(CC) $(CFLAGS) $(CINC) -c $$f; \
	      $(CC) $(CFLAGS) $(CINC) -c $$f; \
	    fi; \
	  done; \
	  mv -f QDP_F3_*.o ../../qdp_f3; \
	fi

EXTRA_DIST = src
