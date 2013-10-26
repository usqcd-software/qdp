#include <stdlib.h>
#include <stdio.h>
#include "qdp_internal.h"

// prevent use of default lattice variables/functions
#define QDP_sites_on_node ERROR
#define QDP_ndim() ERROR
#define QDP_coord_size(i) ERROR
#define QDP_latsize(ls) ERROR
#define QDP_volume() ERROR
#define QDP_numsites(node) ERROR
#define QDP_node_number(x) ERROR
#define QDP_index(x) ERROR
#define QDP_get_coords(x, node, index) ERROR
#define QDP_all ERROR
#define QDP_even_and_odd ERROR
#define QDP_even ERROR
#define QDP_odd ERROR
#define QDP_neighbor ERROR
////////////////////////////////////

typedef struct indexlist {
  int *i2l[2];
  int *l2i[2];
  int *dx;
  struct indexlist *next;
} indexlist;

typedef struct {
  int *len;          /* lattice dimensions */
  int *nsquares;     /* number of hypercubes in each direction */
  int ndim;
  int numsites;
  QMP_comm_t comm;
  int use_qmp_topology;
  indexlist *indexlist;
} params;

static int prime[] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53};
#define MAXPRIMES (sizeof(prime)/sizeof(int))

/*****************  node layout *******************/

static void
node2coord(int *x, int n, params *p)
{
  if(p->use_qmp_topology) {
    QMP_comm_get_logical_coordinates_from2(p->comm, x, n);
  } else {
    for(int i=0; i<p->ndim; i++) {
      x[i] = n % p->nsquares[i];
      n = n / p->nsquares[i];
    }
  }
}

static int
coord2node(int *x, params *p)
{
  if(p->use_qmp_topology) {
    return QMP_comm_get_node_number_from(p->comm, x);
  }
  int l = 0;
  for(int i=p->ndim-1; i>=0; --i) {
    if(x[i]>=p->nsquares[i]) { l = -1; break; }
    l = l*p->nsquares[i] + (x[i]%p->nsquares[i]);
  }
  return l;
}

/*****************  site layout *******************/

static void
get_lex_x(int *x, int l, int *s, int ndim)
{
  //for(int i=0; i<ndim; i++) { // x fastest
  for(int i=ndim-1; i>=0; --i) { // x slowest
    x[i] = l % s[i];
    l = l / s[i];
  }
}

static int
get_lex_i(int *x, int *s, int ndim)
{
  int l = 0;
  //for(int i=ndim-1; i>=0; --i) { // x fastest
  for(int i=0; i<ndim; i++) { // x slowest
    if(x[i]>=s[i]) { l = -1; break; }
    l = l*s[i] + x[i];
  }
  return l;
}

static int
get_parity(int p, int *x, int n)
{
  for(int i=0; i<n; i++) p += x[i];
  return p&1;
}

static void
getblockcoord(int *c, int *x, int *l, int d, int nd)
{
  for(int i=0; i<nd; i++) {
    c[i] = 1;
    if(x[i]<d) c[i]--;
    if(x[i]>=l[i]-d) c[i]++;
  }
}

static int
gray3index(int *x, int nd)
{
  int s=0, g=0;
  for(int i=0; i<nd; i++) {
    if(s==0) {
      g = 3*g + x[i];
    } else {
      g = 3*g + 2 - x[i];
    }
    s = (s + x[i]) & 1;
  }
  return g;
}

static int
shiftsort(int *a, int *b, int *dx, int d, int nd)
{
  int ca[nd], cb[nd];
  getblockcoord(ca, a, dx, d, nd);
  getblockcoord(cb, b, dx, d, nd);
  int ga = gray3index(ca, nd);
  int gb = gray3index(cb, nd);
  return ga-gb;
}

//static int shiftopts[] = { 1 };
static int shiftopts[] = { 3 };
static int nshiftopts = sizeof(shiftopts)/sizeof(shiftopts[0]);
static int g_s0, *g_dx, g_nd;

static int
shiftopt(const void *aa, const void *bb)
{
  int la = *(int *)aa;
  int lb = *(int *)bb;
  int a[g_nd], b[g_nd];
  get_lex_x(a, la, g_dx, g_nd);
  get_lex_x(b, lb, g_dx, g_nd);
  int pa = get_parity(g_s0, a, g_nd);
  int pb = get_parity(g_s0, b, g_nd);
  if(pa!=pb) return pa-pb;
  for(int i=0; i<nshiftopts; i++) {
    int t = shiftsort(a, b, g_dx, shiftopts[i], g_nd);
    if(t) return t;
  }
  for(int i=1; ; i++) {
    int t = shiftsort(a, b, g_dx, i, g_nd);
    if(t) return t;
  }
  return 0;
}

static int
lexeo(const void *aa, const void *bb)
{
  int la = *(int *)aa;
  int lb = *(int *)bb;
  int a[g_nd], b[g_nd];
  get_lex_x(a, la, g_dx, g_nd);
  get_lex_x(b, lb, g_dx, g_nd);
  int pa = get_parity(g_s0, a, g_nd);
  int pb = get_parity(g_s0, b, g_nd);
  if(pa!=pb) return pa-pb;
  return la-lb;
}

static void
geti2l(int *i2l, int s0, int *dx, int nd, int ns)
{
  for(int i=0; i<ns; i++) i2l[i] = i;
  g_s0 = s0;
  g_dx = dx;
  g_nd = nd;
  //qsort(i2l, ns, sizeof(int), lexeo);
  qsort(i2l, ns, sizeof(int), shiftopt);
}

static void
invertarray(int *r, int *a, int n)
{
  for(int i=0; i<n; i++) r[a[i]] = i;
}

static indexlist *
getindexlist(int *dx, params *p)
{
  int nd = p->ndim;
  indexlist **n = &(p->indexlist);
  while(*n!=NULL) {
    int same = 1;
    for(int i=0; i<nd; i++) {
      if(dx[i]!=(*n)->dx[i]) same = 0;
    }
    if(same) break;
    n = &((*n)->next);
  }
  if(*n==NULL) {
    *n = malloc(sizeof(indexlist));
    (*n)->next = NULL;
    (*n)->dx = malloc(nd*sizeof(int));
    int ns = 1;
    for(int i=0; i<nd; i++) {
      (*n)->dx[i] = dx[i];
      ns *= dx[i];
    }
    (*n)->i2l[0] = malloc(ns*sizeof(int));
    (*n)->i2l[1] = malloc(ns*sizeof(int));
    (*n)->l2i[0] = malloc(ns*sizeof(int));
    (*n)->l2i[1] = malloc(ns*sizeof(int));
    geti2l((*n)->i2l[0], 0, dx, nd, ns);
    geti2l((*n)->i2l[1], 1, dx, nd, ns);
    invertarray((*n)->l2i[0], (*n)->i2l[0], ns);
    invertarray((*n)->l2i[1], (*n)->i2l[1], ns);
#if 0
    if(QDP_this_node==0&&QDP_thread_num()==0) {
      for(int i=0; i<ns; i++) {
	int l = (*n)->i2l[0][i];
	int x[nd];
	get_lex_x(x, l, dx, nd);
	printf("%i\t%i\t:", i, l);
	for(int j=0; j<nd; j++) printf("\t%i", x[j]);
	printf("\n");
      }
    }
#endif
  }
  return *n;
}

static int
l2i(int l, int s0, int *dx, params *p)
{
  indexlist *n = getindexlist(dx, p);
  return n->l2i[s0][l];
}

static int
i2l(int index, int s0, int *dx, params *p)
{
  indexlist *n = getindexlist(dx, p);
  return n->i2l[s0][index];
}

static void
setup_sitelayout(params *p)
{
  p->indexlist = NULL;
}

static void
free_sitelayout(params *p)
{
  for(indexlist *n=p->indexlist; n!=NULL; n=n->next) {
    free(n->i2l[0]);
    free(n->i2l[1]);
    free(n->l2i[0]);
    free(n->l2i[1]);
    free(n->dx);
  }
}

/**************************** Layout API ***************************/

static void
layout_shiftopt_setup(QDP_Lattice *lat, void *args)
{
  QDP_allocate_lattice_params(lat, sizeof(params));
  params *p = (params *) QDP_get_lattice_params(lat);

  int nd = QDP_ndim_L(lat);

  p->ndim = nd;
  p->len = (int *) malloc(nd*sizeof(int));
  p->nsquares = (int *) malloc(nd*sizeof(int));
  p->comm = NULL;
  p->use_qmp_topology = 0;
  int *len = p->len;
  int *nsquares = p->nsquares;
  QDP_latsize_L(lat, len);

  if(p->use_qmp_topology==0) {
    int nd2 = QMP_get_logical_number_of_dimensions();
    if(nd2>0) {
      const int *nsquares2 = QMP_get_logical_dimensions();
      for(int i=0; i<nd; i++) {
	if(i<nd2) nsquares[i] = nsquares2[i];
	else nsquares[i] = 1;
      }
      p->comm = QMP_comm_get_default();
      p->use_qmp_topology = 1;
    }
  }

  if(p->use_qmp_topology==0) {
    int nd2 = QMP_get_number_of_job_geometry_dimensions();
    if(nd2>0) {
      const int *nsquares2 = QMP_get_job_geometry();
      for(int i=0; i<nd; i++) {
	if(i<nd2) nsquares[i] = nsquares2[i];
	else nsquares[i] = 1;
      }
      p->comm = QMP_comm_get_job();
      p->use_qmp_topology = 1;
    }
  }

  if(p->use_qmp_topology==0) {
    int nd2 = QMP_get_allocated_number_of_dimensions();
    if(nd2>0) {
      const int *nsquares2 = QMP_get_allocated_dimensions();
      for(int i=0; i<nd; i++) {
	if(i<nd2) nsquares[i] = nsquares2[i];
	else nsquares[i] = 1;
      }
      p->comm = QMP_comm_get_allocated();
      p->use_qmp_topology = 1;
    }
  }

  if(p->use_qmp_topology==0) {
    int *squaresize = (int *) malloc(nd*sizeof(int));
    int *extrafactors = (int *) malloc(nd*sizeof(int));
    for(int i=0; i<nd; ++i) {
      squaresize[i] = len[i];
      extrafactors[i] = 1;
      nsquares[i] = 1;
    }

    /* Figure out dimensions of rectangle */
    int n = QMP_get_number_of_nodes();   /* remaining number of nodes to be factored */
    int k = MAXPRIMES-1;
    while(n>1) {
      /* figure out which prime to divide by starting with largest */
      /* if no factor found, assume n is prime */
      while( (k>=0) && (n%prime[k]!=0) ) --k;
      int pfac = (k>=0) ? prime[k] : n;

      /* figure out which direction to divide */
      /* find largest divisible dimension of h-cubes */
      /* if one direction with largest dimension has already been
	 divided, divide it again.  Otherwise divide first direction
	 with largest dimension. */
      int j = -1;
      for(int i=0; i<nd; i++) {
	if(squaresize[i]%pfac==0) {
	  if( (j<0) || (extrafactors[j]*squaresize[i]>extrafactors[i]*squaresize[j]) ) {
	    j = i;
	  } else if(extrafactors[j]*squaresize[i]==extrafactors[i]*squaresize[j]) {
	    if((nsquares[j]==1)||(nsquares[i]!=1)) j = i;
	  }
	}
      }

      /* This can fail if we run out of prime factors in the dimensions */
      /* then just choose largest dimension */
      if(j<0) {
	for(int i=0; i<nd; i++) {
	  if( (j<0) || (extrafactors[j]*squaresize[i]>extrafactors[i]*squaresize[j]) ) {
	    j = i;
	  } else if(extrafactors[j]*squaresize[i]==extrafactors[i]*squaresize[j]) {
	    if((nsquares[j]==1)||(nsquares[i]!=1)) j = i;
	  }
	}
	n /= pfac;
	extrafactors[j] *= pfac;
	nsquares[j] *= pfac;
      } else {
	n /= pfac;
	squaresize[j] /= pfac;
	nsquares[j] *= pfac;
      }
    }
    free(squaresize);
    free(extrafactors);
  } /* not QMP_GRID */

  /* setup QMP logical topology */
  // *** fix to avoid QMP topology ***
#if 0
  if(!QMP_logical_topology_is_declared()) {
    QMP_declare_logical_topology(nsquares, nd);
  }
#endif

  int numsites = 1;
  int mcoord[nd];
  node2coord(mcoord, QDP_this_node, p);
  for(int i=0; i<nd; ++i) {
    int x0 = (mcoord[i]*p->len[i]+p->nsquares[i]-1)/p->nsquares[i];
    int x1 = ((mcoord[i]+1)*p->len[i]+p->nsquares[i]-1)/p->nsquares[i];
    numsites *= x1-x0;
  }
  p->numsites = numsites;
  setup_sitelayout(p);

  /// need to make VERBOSE
  if(QDP_this_node==0) {
    printf("ndim = %i\n", p->ndim);
    printf("numsites = %i\n", p->numsites);
    printf("len =");
    for(int i=0; i<p->ndim; i++) printf(" %i", p->len[i]);
    printf("\n");
    printf("nsquares =");
    for(int i=0; i<p->ndim; i++) printf(" %i", p->nsquares[i]);
    printf("\n");
  }
}

static void
layout_shiftopt_free(QDP_Lattice *lat)
{
  params *p = (params *) QDP_get_lattice_params(lat);
  free_sitelayout(p);
  free(p->len);
  free(p->nsquares);
}


static int
layout_shiftopt_numsites(QDP_Lattice *lat, int node)
{
  params *p = (params *) QDP_get_lattice_params(lat);
  if(node==QDP_this_node) {
    return p->numsites;
  } else {
    int numsites = 1;
    int nd = p->ndim;
    int mcoord[nd];
    node2coord(mcoord, node, p);
    for(int i=0; i<nd; ++i) {
      int x0 = (mcoord[i]*p->len[i]+p->nsquares[i]-1)/p->nsquares[i];
      int x1 = ((mcoord[i]+1)*p->len[i]+p->nsquares[i]-1)/p->nsquares[i];
      numsites *= x1-x0;
    }
    return numsites;
  }
}

static int
layout_shiftopt_node_number(QDP_Lattice *lat, const int x[])
{
  params *p = (params *) QDP_get_lattice_params(lat);
  int nd = p->ndim;
  int m[nd];
  for(int i=0; i<nd; i++) {
    m[i] = (x[i]*p->nsquares[i])/p->len[i];
  }
  return coord2node(m, p);
}

static int
layout_shiftopt_index(QDP_Lattice *lat, const int x[])
{
  params *p = (params *) QDP_get_lattice_params(lat);
  int nd = p->ndim;
  int lc[nd], ll[nd];

  int s0 = 0;
  for(int i=0; i<nd; ++i) {
    int m = (x[i]*p->nsquares[i])/p->len[i];
    int x0 = (m*p->len[i]+p->nsquares[i]-1)/p->nsquares[i];
    int x1 = ((m+1)*p->len[i]+p->nsquares[i]-1)/p->nsquares[i];
    ll[i] = x1 - x0;
    lc[i] = x[i] - x0;
    s0 += x0;
  }
  int l = get_lex_i(lc, ll, nd);
  int index = l2i(l, s0&1, ll, p);
  return index;
}

static void
layout_shiftopt_get_coords(QDP_Lattice *lat, int x[], int node, int index)
{
  params *p = (params *) QDP_get_lattice_params(lat);
  int nd = p->ndim;
  int m[nd], dx[nd], sx[nd];

  node2coord(m, node, p);

  int s0 = 0;
  for(int i=0; i<nd; ++i) {
    x[i] = (m[i]*p->len[i]+p->nsquares[i]-1)/p->nsquares[i];
    int x1 = ((m[i]+1)*p->len[i]+p->nsquares[i]-1)/p->nsquares[i];
    dx[i] = x1 - x[i];
    s0 += x[i];
  }

  int l = i2l(index, s0&1, dx, p);
  get_lex_x(sx, l, dx, nd);
  for(int i=0; i<nd; ++i) x[i] += sx[i];

  if(QDP_index_L(lat, x)!=index) {
    if(QDP_this_node==0) {
      fprintf(stderr,"QDP: error in layout!\n");
      fprintf(stderr,"%i %i  -> ", node, index);
      for(int i=0; i<nd; i++) fprintf(stderr," %i", x[i]);
      fprintf(stderr,"  ->  %i %i\n", QDP_node_number_L(lat,x), QDP_index_L(lat,x));
    }
    QDP_abort(1);
    exit(1);
  }
}

static QDP_Layout layout_shiftopt = {
  layout_shiftopt_setup,
  layout_shiftopt_free,
  layout_shiftopt_numsites,
  layout_shiftopt_node_number,
  layout_shiftopt_index,
  layout_shiftopt_get_coords
};
QDP_Layout *QDP_layout_shiftopt = &layout_shiftopt;
