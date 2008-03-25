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
#include "qdp_layout.h"
#include "com_specific.h"

static int *squaresize;   /* dimensions of hypercubes */
static int *nsquares;     /* number of hypercubes in each direction */
static int ndim;
static int *l2ie, *l2io, *i2le, *i2lo;
static int prime[] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53};
#define MAXPRIMES (sizeof(prime)/sizeof(int))

void
get_lex_x(int *x, int l, int *s)
{
  int i;
  for(i=0; i<ndim; i++) {
    x[i] = l % s[i];
    l = l / s[i];
  }
}

int
get_lex_i(int *x, int *s)
{
  int i, l;
  l = 0;
  for(i=ndim-1; i>=0; --i) {
    if(x[i]>=s[i]) { l = -1; break; }
    l = l*s[i] + (x[i]%s[i]);
  }
  return l;
}

int
get_sum(int *x)
{
  int i, p=0;
  for(i=0; i<ndim; i++) p += x[i];
  return p;
}

#ifdef MORTON
int
round_up_pow2(int k)
{
  int i;
  i = 2;
  while(i<k) i *= 2;
  return i;
}

void
get_morton_x(int *x, int l, int *s)
{
  int i, t[ndim], dir=0;
  for(i=0; i<ndim; i++) {
    t[i] = 1;
    x[i] = 0;
  }
  while(l!=0) {
    while(t[dir]>=s[dir]) dir = (dir+1)%ndim;
    x[dir] += (l&1)*t[dir];
    t[dir] *= 2;
    l /= 2;
    dir = (dir+1)%ndim;
  }
}

void
setup_index_maps(void)
{
  int i, x[ndim], mx[ndim], nm, ie, io;

  l2ie = (int *) malloc(QDP_sites_on_node*sizeof(int));
  l2io = (int *) malloc(QDP_sites_on_node*sizeof(int));
  i2le = (int *) malloc(QDP_sites_on_node*sizeof(int));
  i2lo = (int *) malloc(QDP_sites_on_node*sizeof(int));

  nm = 1;
  for(i=0; i<ndim; i++) {
    mx[i] = round_up_pow2(squaresize[i]);
    nm *= mx[i];
  }
  //printf("%i: %i %i %i %i\n", nm, mx[0], mx[1], mx[2], mx[3]);
  ie = io = 0;
  for(i=0; i<nm; i++) {
    int l;
    get_morton_x(x, i, mx);
    l = get_lex_i(x, squaresize);
    if(l>=0) {
      //printf("%i: %i: %i %i %i %i\n", i, l, x[0], x[1], x[2], x[3]);
      int p = get_sum(x);
      if((p&1)==0) {
	l2ie[l] = ie;
	i2le[ie] = l;
	ie++;
      } else {
	l2io[l] = io;
	i2lo[io] = l;
	io++;
      }
    }
  }
  for(i=0; i<ie; i++) {
    i2lo[i+io] = i2le[i];
    l2io[i2le[i]] = i+io;
  }
  for(i=0; i<io; i++) {
    i2le[i+ie] = i2lo[i];
    l2ie[i2lo[i]] = i+ie;
  }
}
#else
void
setup_index_maps(void)
{
  int i, x[ndim], ie, io;

  l2ie = (int *) malloc(QDP_sites_on_node*sizeof(int));
  l2io = (int *) malloc(QDP_sites_on_node*sizeof(int));
  i2le = (int *) malloc(QDP_sites_on_node*sizeof(int));
  i2lo = (int *) malloc(QDP_sites_on_node*sizeof(int));

  ie = io = 0;
  for(i=0; i<QDP_sites_on_node; i++) {
    get_lex_x(x, i, squaresize);
    int p = get_sum(x);
    if((p&1)==0) {
      l2ie[i] = ie;
      i2le[ie] = i;
      ie++;
    } else {
      l2io[i] = io;
      i2lo[io] = i;
      io++;
    }
  }
  for(i=0; i<ie; i++) {
    i2lo[i+io] = i2le[i];
    l2io[i2le[i]] = i+io;
  }
  for(i=0; i<io; i++) {
    i2le[i+ie] = i2lo[i];
    l2ie[i2lo[i]] = i+ie;
  }
}
#endif

void
QDP_setup_layout(int len[], int nd)
{
  int i, j, k, n;

  ndim = nd;
  squaresize = (int *) malloc(ndim*sizeof(int));
  nsquares = (int *) malloc(ndim*sizeof(int));

  for(i=0; i<ndim; ++i) {
    squaresize[i] = len[i];
    nsquares[i] = 1;
  }

  if(QMP_get_msg_passing_type()!=QMP_SWITCH) {
    int ndim2, i;
    const int *nsquares2;
    ndim2 = QMP_get_allocated_number_of_dimensions();
    nsquares2 = QMP_get_allocated_dimensions();
    for(i=0; i<ndim; i++) {
      if(i<ndim2) nsquares[i] = nsquares2[i];
      else nsquares[i] = 1;
    }
    for(i=0; i<ndim; i++) {
      if(len[i]%nsquares[i] != 0) {
	printf("LATTICE SIZE DOESN'T FIT GRID\n");
	QMP_abort(0);
      }
      squaresize[i] = len[i]/nsquares[i];
    }
  } else { /* not QMP_GRID */
    /* Figure out dimensions of rectangle */
    n = QDP_numnodes();   /* remaining number of nodes to be factored */
    k = MAXPRIMES-1;
    while(n>1) {
      /* figure out which prime to divide by starting with largest */
      while( (n%prime[k]!=0) && (k>0) ) --k;

      /* figure out which direction to divide */
      /* find largest divisible dimension of h-cubes */
      /* if one direction with largest dimension has already been
	 divided, divide it again.  Otherwise divide first direction
	 with largest dimension. */
      j = -1;
      for(i=0; i<ndim; i++) {
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
  if(!QMP_logical_topology_is_declared()) {
    QMP_declare_logical_topology(nsquares, ndim);
  }

  QDP_sites_on_node = 1;
  for(i=0; i<ndim; ++i) {
    QDP_sites_on_node *= squaresize[i];
  }

  setup_index_maps();
}

int
QDP_numsites(int node)
{
  return QDP_sites_on_node;
}

int
QDP_node_number(const int x[])
{
#if 0
  int i, r=0;

  for(i=ndim-1; i>=0; --i) {
    r = r*nsquares[i] + (x[i]/squaresize[i]);
  }
  return r;
#endif
  int i, m[ndim];

  for(i=0; i<ndim; i++) {
    m[i] = x[i]/squaresize[i];
  }
  return QMP_get_node_number_from(m);
}

int
QDP_index(const int x[])
{
  int i, p=0, l=0, r;

  for(i=ndim-1; i>=0; --i) {
    l = l*squaresize[i] + (x[i]%squaresize[i]);
    p += (x[i]/squaresize[i])*squaresize[i];
  }

  if( p%2==0 ) { /* even node offset */
    r = l2ie[l];
  } else {
    r = l2io[l];
  }
  return r;
}

void
QDP_get_coords(int x[], int node, int index)
{
  int i, p, l;
  int *m, sx[ndim];

  m = QMP_get_logical_coordinates_from(node);

  p = 0;
  for(i=0; i<ndim; ++i) {
    x[i] = m[i] * squaresize[i];
    p += x[i];
  }
  free(m);

  if(p%2==0) {
    l = i2le[index];
  } else {
    l = i2lo[index];
  }
  get_lex_x(sx, l, squaresize);
  for(i=0; i<ndim; ++i) x[i] += sx[i];

  if(QDP_index(x)!=index) {
    int k;
    if(p%2==0) k = l2ie[l];
    else k = l2io[l];
    if(QDP_this_node==0) {
      fprintf(stderr,"QDP: error in layout!\n");
      fprintf(stderr,"%i %i -> ", node, index);
      for(i=0; i<ndim; i++) fprintf(stderr," %i", x[i]);
      fprintf(stderr," : %i %i %i\n",p,l,k);
    }
    QDP_abort_comm();
    exit(1);
  }
}
