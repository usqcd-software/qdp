/******** layout_hyper.c *********/
/* SciDAC QDP Data Parallel API */
/* adapted from MILC version 6 by James Osborn */

/* Comments from original MILC routine */
/* ROUTINES WHICH DETERMINE THE DISTRIBUTION OF SITES ON NODES */

/* This version divides the lattice by factors of prime numbers in any of the
   four directions.  It prefers to divide the longest dimensions,
   which mimimizes the area of the surfaces.  Similarly, it prefers
   to divide dimensions which have already been divided, thus not
   introducing more off-node directions.

        S. Gottlieb, May 18, 1999
        The code will start trying to divide with the largest prime factor
        and then work its way down to 2.  The current maximum prime is 53.
        The array of primes on line 46 may be extended if necessary.

   This requires that the lattice volume be divisible by the number
   of nodes.  Each dimension must be divisible by a suitable factor
   such that the product of the four factors is the number of nodes.

   3/29/00 EVENFIRST is the rule now. CD.
   12/10/00 Fixed so k = MAXPRIMES-1 DT
*/
/* End of comments from original MILC routine */

/*
   QDP_setup_layout()  sets up layout
   QDP_numsites()      returns the number of sites on a node
   QDP_node_number()   returns the node number on which a site lives
   QDP_index()         returns the index of the site on the node
   QDP_get_coords()    gives lattice coords from node & index
*/

//#define MORTON

#include <stdlib.h>
#include <stdio.h>
#include "qdp_internal.h"

typedef struct {
  int *squaresize;   /* dimensions of hypercubes */
  int *nsquares;     /* number of hypercubes in each direction */
  int *l2ie, *l2io, *i2le, *i2lo;
  int ndim;
  int numsites;
} params;

static int prime[] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53};
#define MAXPRIMES (sizeof(prime)/sizeof(int))

static void
get_lex_x(int *x, int l, int *s, int ndim)
{
  int i;
  for(i=0; i<ndim; i++) {
    x[i] = l % s[i];
    l = l / s[i];
  }
}

static int
get_lex_i(int *x, int *s, int ndim)
{
  int i, l;
  l = 0;
  for(i=ndim-1; i>=0; --i) {
    if(x[i]>=s[i]) { l = -1; break; }
    l = l*s[i] + (x[i]%s[i]);
  }
  return l;
}

static int
get_sum(int *x, int ndim)
{
  int i, p=0;
  for(i=0; i<ndim; i++) p += x[i];
  return p;
}

static void
setup_index_maps(params *p)
{
  int i, x[p->ndim], ie, io;

  p->l2ie = (int *) malloc(p->numsites*sizeof(int));
  p->l2io = (int *) malloc(p->numsites*sizeof(int));
  p->i2le = (int *) malloc(p->numsites*sizeof(int));
  p->i2lo = (int *) malloc(p->numsites*sizeof(int));

  ie = io = 0;
  for(i=0; i<p->numsites; i++) {
    get_lex_x(x, i, p->squaresize, p->ndim);
    int s = get_sum(x, p->ndim);
    if((s&1)==0) {
      p->l2ie[i] = ie;
      p->i2le[ie] = i;
      ie++;
    } else {
      p->l2io[i] = io;
      p->i2lo[io] = i;
      io++;
    }
  }
  for(i=0; i<ie; i++) {
    p->i2lo[i+io] = p->i2le[i];
    p->l2io[p->i2le[i]] = i+io;
  }
  for(i=0; i<io; i++) {
    p->i2le[i+ie] = p->i2lo[i];
    p->l2ie[p->i2lo[i]] = i+ie;
  }
}

static void
layout_hyper_eo_setup(QDP_Lattice *lat, void *args)
{
  QDP_allocate_lattice_params(lat, sizeof(params));
  params *p = (params *) QDP_get_lattice_params(lat);

  int nd = QDP_ndim_L(lat);
  int len[nd];
  QDP_latsize_L(lat, len);

  p->ndim = nd;
  p->squaresize = (int *) malloc(nd*sizeof(int));
  p->nsquares = (int *) malloc(nd*sizeof(int));

  int *squaresize = p->squaresize;
  int *nsquares = p->nsquares;

  for(int i=0; i<nd; ++i) {
    squaresize[i] = len[i];
    nsquares[i] = 1;
  }

  if(QMP_get_msg_passing_type()!=QMP_SWITCH) {
    int nd2 = QMP_get_allocated_number_of_dimensions();
    const int *nsquares2 = QMP_get_allocated_dimensions();
    for(int i=0; i<nd; i++) {
      if(i<nd2) nsquares[i] = nsquares2[i];
      else nsquares[i] = 1;
    }
    for(int i=0; i<nd; i++) {
      if(len[i]%nsquares[i] != 0) {
	printf("LATTICE SIZE DOESN'T FIT GRID\n");
	QMP_abort(0);
      }
      squaresize[i] = len[i]/nsquares[i];
    }
  } else { /* not QMP_GRID */
    /* Figure out dimensions of rectangle */
    int n = QDP_numnodes();   /* remaining number of nodes to be factored */
    int k = MAXPRIMES-1;
    while(n>1) {
      /* figure out which prime to divide by starting with largest */
      while( (n%prime[k]!=0) && (k>0) ) --k;

      /* figure out which direction to divide */
      /* find largest divisible dimension of h-cubes */
      /* if one direction with largest dimension has already been
	 divided, divide it again.  Otherwise divide first direction
	 with largest dimension. */
      int j = -1;
      for(int i=0; i<nd; i++) {
	if(squaresize[i]%prime[k]==0) {
	  if( (j<0) || (squaresize[i]>squaresize[j]) ) {
	    j = i;
	  } else if(squaresize[i]==squaresize[j]) {
	    //if((nsquares[j]==1)&&(nsquares[i]!=1))
	    j = i;
	  }
	}
      }

      /* This can fail if we run out of prime factors in the dimensions */
      if(j<0) {
	if(QDP_mynode()==0) {
	  fprintf(stderr, "LAYOUT: Not enough prime factors in lattice dimensions\n");
	}
	QDP_abort_comm();
	exit(1);
      }

      /* do the surgery */
      n /= prime[k];
      squaresize[j] /= prime[k];
      nsquares[j] *= prime[k];
    }
  } /* not QMP_GRID */

  /* setup QMP logical topology */
  // *** fix to avoid QMP topology ***
  if(!QMP_logical_topology_is_declared()) {
    QMP_declare_logical_topology(nsquares, nd);
  }

  int numsites = 1;
  for(int i=0; i<nd; ++i) {
    numsites *= squaresize[i];
  }
  p->numsites = numsites;

  setup_index_maps(p);
}

static void
layout_hyper_eo_free(QDP_Lattice *lat)
{
  params *p = (params *) QDP_get_lattice_params(lat);
  free(p->squaresize);
  free(p->nsquares);
  free(p->l2ie);
  free(p->l2io);
  free(p->i2le);
  free(p->i2lo);
}


static int
layout_hyper_eo_numsites(QDP_Lattice *lat, int node)
{
  params *p = (params *) QDP_get_lattice_params(lat);
  return p->numsites;
}

// *** fix to avoid QMP topology ***
static int
layout_hyper_eo_node_number(QDP_Lattice *lat, const int x[])
{
#if 0
  int i, r=0;

  for(i=ndim-1; i>=0; --i) {
    r = r*nsquares[i] + (x[i]/squaresize[i]);
  }
  return r;
#endif
  params *p = (params *) QDP_get_lattice_params(lat);
  int i, m[p->ndim];

  for(i=0; i<p->ndim; i++) {
    m[i] = x[i]/p->squaresize[i];
  }
  return QMP_get_node_number_from(m);
}

static int
layout_hyper_eo_index(QDP_Lattice *lat, const int x[])
{
  params *p = (params *) QDP_get_lattice_params(lat);
  int i, s=0, l=0, r;

  for(i=p->ndim-1; i>=0; --i) {
    l = l*p->squaresize[i] + (x[i]%p->squaresize[i]);
    s += (x[i]/p->squaresize[i])*p->squaresize[i];
  }

  if( s%2==0 ) { /* even node offset */
    r = p->l2ie[l];
  } else {
    r = p->l2io[l];
  }
  return r;
}

static void
layout_hyper_eo_get_coords(QDP_Lattice *lat, int x[], int node, int index)
{
  TRACE;
  params *p = (params *) QDP_get_lattice_params(lat);
  int i, s, l;
  int *m, sx[p->ndim];

  TRACE;
  m = QMP_get_logical_coordinates_from(node);

  TRACE;
  s = 0;
  for(i=0; i<p->ndim; ++i) {
    x[i] = m[i] * p->squaresize[i];
    s += x[i];
  }
  free(m);

  TRACE;
  if(s%2==0) {
    l = p->i2le[index];
  } else {
    l = p->i2lo[index];
  }
  TRACE;
  get_lex_x(sx, l, p->squaresize, p->ndim);
  TRACE;
  for(i=0; i<p->ndim; ++i) x[i] += sx[i];

  TRACE;
  if(QDP_index(x)!=index) {
    int k;
    if(s%2==0) k = p->l2ie[l];
    else k = p->l2io[l];
    if(QDP_this_node==0) {
      fprintf(stderr,"QDP: error in layout!\n");
      fprintf(stderr,"%i %i -> ", node, index);
      for(i=0; i<p->ndim; i++) fprintf(stderr," %i", x[i]);
      fprintf(stderr," : %i %i %i\n",s,l,k);
    }
    QDP_abort_comm();
    exit(1);
  }
  TRACE;
}

static QDP_Layout layout_hyper_eo = {
  layout_hyper_eo_setup,
  layout_hyper_eo_free,
  layout_hyper_eo_numsites,
  layout_hyper_eo_node_number,
  layout_hyper_eo_index,
  layout_hyper_eo_get_coords
};
QDP_Layout *QDP_layout_hyper_eo = &layout_hyper_eo;
