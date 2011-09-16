#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <limits.h>
#include "qdp_common_internal.h"
#include "qdp_internal.h"

/*  Exported Globals  */

int QDP_suspended = 0;
int QDP_block_size = 256;
int QDP_mem_align = 64;
int QDP_mem_flags = QDP_MEM_FAST | QDP_MEM_COMMS;

/* Private Globals */

static int qdp_initialized=0;
static QDP_prof *prof_list=NULL, **prof_last;
static const char *vs = VERSION;
static int readnodes = INT_MAX;
static int writenodes = INT_MAX;

const char *
QDP_version_str(void)
{
  return vs;
}

int
QDP_version_int(void)
{
  int maj, min, bug;
  sscanf(vs, "%i.%i.%i", &maj, &min, &bug);
  return ((maj*1000)+min)*1000 + bug;
}

int
QDP_profcontrol(int new)
{
  int old;
  old = QDP_prof_level;
  QDP_prof_level = new;
  return old;
}

void
QDP_register_prof(QDP_prof *qp)
{
  *prof_last = qp;
  prof_last = &qp->next;
}

double
QDP_time(void)
{
  struct timeval tp;
  gettimeofday(&tp,NULL);
  return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
  //struct tms buf;
  //times(&buf);
  //return buf.tms_utime + buf.tms_stime;
  //  return (double)clock()/CLOCKS_PER_SEC;
}

int
QDP_initialize(int *argc, char **argv[])
{
  if(qdp_initialized) {
    fprintf(stderr,
    	    "error: QDP_initialize() called but QDP already initialized!\n");
    QDP_abort(1);
  }
  qdp_initialized = 1;
  if(!QMP_is_initialized()) {
    QDP_initialize_comm(argc, argv);
    qdp_initialized = 2;
  }
  QDP_this_node = QDP_mynode();
  prof_last = &prof_list;
  return 0;
}

void
QDP_finalize(void)
{
  if(!qdp_initialized) {
    fprintf(stderr,"error: QDP_finalize() called but QDP not initialized!\n");
    QDP_abort(1);
  }
  if((prof_list)&&(QDP_this_node==0)) {
    double s;
    QDP_prof *qp;
    s = 1000.0;
    //s = 1000.0/sysconf(_SC_CLK_TCK);
    *prof_last = NULL;
    qp = prof_list;
    printf("%-31s          time us per ns per\n","");
    printf("%-31s  count   (ms)  call   site  caller:line\n","QDP function");
    printf("----------------------------------------");
    printf("---------------------------------------\n");
    while(qp) {
      if(qp->count) {
	printf("%-31s %6i %6i %6i %6i %s:%i\n", qp->func, qp->count,
	       (int)(s*qp->time+0.5), (int)((1000*s*qp->time/qp->count)+0.5),
	       (int)((1000000*s*qp->time/(qp->nsites))+0.5),
	       qp->caller, qp->line);
	if(0*qp->math_time) {
	  printf("%-31s %6i %6i %6i %6i %s:%i\n", "  -math", qp->count,
		 (int)(s*qp->math_time+0.5),
	       (int)((1000*s*qp->math_time/qp->count)+0.5),
		 (int)((1000000*s*qp->math_time/(qp->nsites))+0.5),
		 qp->caller, qp->line);
	}
	if(qp->comm_time) {
	  printf("%-31s %6i %6i %6i %6i %s:%i\n", "  -comm", qp->count,
		 (int)(s*qp->comm_time+0.5),
		 (int)((1000*s*qp->comm_time/qp->count)+0.5),
		 (int)((1000000*s*qp->comm_time/(qp->nsites))+0.5),
		 qp->caller, qp->line);
	}
      }
      qp = qp->next;
    }
  }
  if(qdp_initialized==2) QDP_finalize_comm();
}

int
QDP_is_initialized(void)
{
  return qdp_initialized;
}

void
QDP_abort(int status)
{
  QDP_abort_comm();
  exit(status);
}

void
QDP_suspend_comm(void)
{
  QDP_suspended = 1;
  QDP_clear_shift_list();
}

void
QDP_resume_comm(void)
{
  QDP_suspended = 0;
}

int
QDP_get_block_size(void)
{
  return QDP_block_size;
}

void
QDP_set_block_size(int bs)
{
  if(bs>0) QDP_block_size = bs;
}

int
QDP_get_mem_align(void)
{
  return QDP_mem_align;
}

void
QDP_set_mem_align(int align)
{
  QDP_mem_align = align;
}

int
QDP_get_mem_flags(void)
{
  return QDP_mem_flags;
}

void
QDP_set_mem_flags(int flags)
{
  QDP_mem_flags = flags;
}

/* IO routines */

static QDP_Lattice *iolat = NULL;

void
QDP_set_iolat(QDP_Lattice *lat)
{
  iolat = lat;
}

static int
node_number_io(const int x[])
{
  return QDP_node_number_L(iolat, x);
}

static int
index_io(const int x[])
{
  return QDP_index_L(iolat, x);
}

static void
get_coords_io(int x[], int node, int index)
{
  QDP_get_coords_L(iolat, x, node, index);
}

static int
numsites_io(int node)
{
  return QDP_numsites_L(iolat, node);
}

int
QDP_set_read_group_size(int nodes)
{
  int oldnodes = readnodes;
  readnodes = nodes;
  return oldnodes;
}

int
QDP_set_write_group_size(int nodes)
{
  int oldnodes = writenodes;
  writenodes = nodes;
  return oldnodes;
}

static int
read_io_node(const int node)
{
  return readnodes*(node/readnodes);
}

static int
write_io_node(const int node)
{
  return writenodes*(node/writenodes);
}

static int
master_io_node(void)
{
  return 0;
}

QDP_Reader *
QDP_open_read(QDP_String *md, char *filename)
{
  QDP_Lattice *lat = QDP_get_default_lattice();
  return QDP_open_read_L(lat, md, filename);
}

QDP_Reader *
QDP_open_read_L(QDP_Lattice *lat, QDP_String *md, char *filename)
{
  QDP_Reader *qdpr;
  QIO_Layout layout;
  QIO_Filesystem fs;
  QIO_Iflag iflag;
  QIO_String *qio_md = QIO_string_create(0);

  qdpr = (QDP_Reader *)malloc(sizeof(struct QDP_Reader_struct));
  if(qdpr == NULL) return NULL;

  qdpr->lat = lat;
  iolat = qdpr->lat;

  layout.node_number = node_number_io;
  layout.node_index = index_io;
  layout.get_coords = get_coords_io;
  layout.num_sites = numsites_io;
  layout.latdim = QDP_ndim_L(lat);
  layout.latsize = (int *)malloc(layout.latdim*sizeof(int));
  QDP_latsize_L(lat, layout.latsize);
  layout.volume = QDP_volume_L(lat);
  layout.sites_on_node = QDP_sites_on_node_L(lat);
  layout.this_node = QDP_this_node;
  layout.number_of_nodes = QDP_numnodes();

  fs.my_io_node = read_io_node;
  fs.master_io_node = master_io_node;

  iflag.serpar = QIO_PARALLEL;
  //iflag.serpar = QIO_SERIAL;
  iflag.volfmt = QIO_SINGLEFILE;
  //iflag.volfmt = QIO_UNKNOWN;

  qdpr->qior = QIO_open_read(qio_md, filename, &layout, &fs, &iflag);

  QDP_string_set(md, QIO_string_ptr(qio_md));
  QIO_string_destroy(qio_md);

  free(layout.latsize);

  if(!qdpr->qior) {
    free(qdpr);
    return NULL;
  }

  return qdpr;
}

QDP_Writer *
QDP_open_write(QDP_String *md, char *filename, int volfmt)
{
  QDP_Lattice *lat = QDP_get_default_lattice();
  return QDP_open_write_L(lat, md, filename, volfmt);
}

QDP_Writer *
QDP_open_write_L(QDP_Lattice *lat, QDP_String *md, char *filename, int volfmt)
{
  QDP_Writer *qdpw;
  QIO_Layout *layout;
  QIO_String *qio_md;
  QIO_Filesystem fs;
  QIO_Oflag oflag;

  qdpw = (QDP_Writer *)malloc(sizeof(struct QDP_Writer_struct));
  if(qdpw == NULL) return NULL;

  layout = (QIO_Layout *)malloc(sizeof(QIO_Layout));
  if(layout == NULL) {
    free(qdpw);
    return NULL;
  }

  qdpw->lat = lat;
  iolat = qdpw->lat;

  layout->node_number = node_number_io;
  layout->node_index = index_io;
  layout->get_coords = get_coords_io;
  layout->num_sites = numsites_io;
  layout->latdim = QDP_ndim_L(lat);
  layout->latsize = (int *)malloc(layout->latdim*sizeof(int));
  QDP_latsize_L(lat, layout->latsize);
  layout->volume = QDP_volume_L(lat);
  layout->sites_on_node = QDP_sites_on_node_L(lat);
  layout->this_node = QDP_this_node;
  layout->number_of_nodes = QDP_numnodes();

  fs.my_io_node = write_io_node;
  fs.master_io_node = master_io_node;

  oflag.serpar = QIO_PARALLEL;
  //oflag.serpar = QIO_SERIAL;
  oflag.mode = QIO_TRUNC;
  oflag.ildgstyle = QIO_ILDGLAT;                                           
  oflag.ildgLFN = NULL;                                                    

  qio_md = QIO_string_create();
  QIO_string_set(qio_md, QDP_string_ptr(md));
  qdpw->qiow = QIO_open_write(qio_md, filename, volfmt, layout, &fs, &oflag);
  QIO_string_destroy(qio_md);

  free(layout->latsize);
  free(layout);

  if(!qdpw->qiow) {
    free(qdpw);
    return NULL;
  }

  return qdpw;
}

int
QDP_close_read(QDP_Reader *qdpr)
{
  int status;
  status = QIO_close_read(qdpr->qior);
  free(qdpr);
  return status;
}

int
QDP_close_write(QDP_Writer *qdpw)
{
  int status;
  status = QIO_close_write(qdpw->qiow);
  free(qdpw);
  return status;
}

int
QDP_read_record_info(QDP_Reader *qdpr, QDP_String *md)
{
  int status;
  QIO_RecordInfo record_info;  /* Private data ignored */
  QIO_String *qio_md = QIO_string_create(0);

  status = QIO_read_record_info(qdpr->qior, &record_info, qio_md);
  QDP_string_set(md, QIO_string_ptr(qio_md));
  QIO_string_destroy(qio_md);
  //printf("QDP_read_record_info has datacount %d\n",
  //record_info.datacount.value);
  return status;
}

int
QDP_read_qio_record_info(QDP_Reader *qdpr, QIO_RecordInfo *ri, QDP_String *md)
{
  int status;
  QIO_RecordInfo ri0, *record_info;
  QIO_String *qio_md = QIO_string_create(0);

  iolat = qdpr->lat;
  record_info = &ri0;
  if(ri) record_info = ri;
  status = QIO_read_record_info(qdpr->qior, record_info, qio_md);
  if(md) QDP_string_set(md, QIO_string_ptr(qio_md));
  QIO_string_destroy(qio_md);
  //printf("QDP_read_record_info has datacount %d\n",
  //record_info.datacount.value);
  if(!ri) QIO_destroy_record_info(record_info);
  return status;
}

int
QDP_next_record(QDP_Reader *qdpr)
{
  int status;

  iolat = qdpr->lat;
  status = QIO_next_record(qdpr->qior);
  return status;
}

/* Internal: Read and check */
int
QDP_read_check(QDP_Reader *qdpr, QDP_String *md, int globaldata,
	       void (* put)(char *buf, size_t index, int count, void *qfin),
	       struct QDP_IO_field *qf, int count, QIO_RecordInfo *cmp_info)
{
  QIO_RecordInfo *rec_info;
  QIO_String *qio_md = QIO_string_create(0);
  int status;

  iolat = qdpr->lat;
  rec_info = QIO_create_record_info(0, 0, 0, 0, "", "", 0, 0, 0, 0);

  status = QIO_read(qdpr->qior, rec_info, qio_md, put, qf->size*count,
		    qf->word_size, (void *)qf);
  QDP_string_set(md, QIO_string_ptr(qio_md));
  QIO_string_destroy(qio_md);

  /* Check for consistency */
  if(QIO_compare_record_info(rec_info, cmp_info)) {
    status = 1;
  }

  QIO_destroy_record_info(rec_info);

  return status;
}

/* Internal: Write and check */
int
QDP_write_check(QDP_Writer *qdpw, QDP_String *md, int globaldata,
		void (*get)(char *buf, size_t index, int count, void *qfin),
		struct QDP_IO_field *qf, int count, QIO_RecordInfo *rec_info)
{
  int status;
  QIO_String *qio_md;

  iolat = qdpw->lat;
  qio_md = QIO_string_create();
  QIO_string_set(qio_md, QDP_string_ptr(md));

  status = QIO_write(qdpw->qiow, rec_info, qio_md,
		     get, qf->size*count, qf->word_size, (void *)qf);

  QIO_string_destroy(qio_md);

  return status;
}

/*
 *  QDP_String manipulation utilities
 */

/* String creation */
QDP_String *
QDP_string_create(void)
{
  QDP_String *qs;

  qs = (QDP_String *)malloc(sizeof(QDP_String));
  if(qs == NULL) return NULL;
  qs->string = NULL;
  qs->length = 0;
  return qs;
}

void
QDP_string_destroy(QDP_String *qs)
{
  if(qs->length>0) free(qs->string);
  free(qs);
}

/* set String */
void
QDP_string_set(QDP_String *qs, char *string)
{
  if(string==NULL) {
    if(qs->length>0) free(qs->string);
    qs->string = NULL;
    qs->length = 0;
  } else {
    size_t len = strlen(string) + 1;

    qs->string = realloc(qs->string, len);
    memcpy(qs->string, string, len);
    qs->length = len;
  }
}

/* Size of string */
size_t
QDP_string_length(QDP_String *qs)
{
  return qs->length;
}

/* Return pointer to string data */
char *
QDP_string_ptr(QDP_String *qs)
{
  return qs->string;
}

void
QDP_string_copy(QDP_String *dest, QDP_String *src)
{
  size_t len = src->length;
  dest->string = realloc(dest->string, len);
  memcpy(dest->string, src->string, len);
}
