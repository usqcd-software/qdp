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

#include <stdlib.h>
#include <stdio.h>
#include "qdp_layout.h"
#include "com_specific.h"

static int *squaresize;   /* dimensions of hypercubes */
static int *nsquares;     /* number of hypercubes in each direction */
static int ndim;
static int *size1[2], *size2;
static int prime[] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53};
#define MAXPRIMES (sizeof(prime)/sizeof(int))

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

  size1[0] = malloc(2*(ndim+1)*sizeof(int));
  size1[1] = size1[0] + ndim + 1;
  size2 = malloc((ndim+1)*sizeof(int));

  size1[0][0] = 1;
  size1[1][0] = 0;
  size2[0] = 1;
  for(i=1; i<=ndim; i++) {
    size1[0][i] = size2[i-1]*(squaresize[i-1]/2)
                + size1[0][i-1]*(squaresize[i-1]%2);
    size1[1][i] = size2[i-1]*(squaresize[i-1]/2)
                + size1[1][i-1]*(squaresize[i-1]%2);
    size2[i] = size1[0][i] + size1[1][i];
    //printf("%i\t%i\t%i\n", size1[0][i], size1[1][i], size2[i]);
  }
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
  int i, r=0, p=0;

  for(i=ndim-1; i>=0; --i) {
    r = r*squaresize[i] + (x[i]%squaresize[i]);
    p += x[i];
  }

  if( p%2==0 ) { /* even site */
    r /= 2;
  } else {
    r = (r+QDP_sites_on_node)/2;
  }
  return r;
}

void
QDP_get_coords(int x[], int node, int index)
{
  int i, s, si;
  int *m;
  si = index;

  m = QMP_get_logical_coordinates_from(node);

  s = 0;
  //p = node;
  for(i=0; i<ndim; ++i) {
    //x[i] = (p%nsquares[i])*squaresize[i];
    x[i] = m[i] * squaresize[i];
    s += x[i];
    //p /= nsquares[i];
  }
  s &= 1;

  if(index>=size1[s][ndim]) {
    index -= size1[s][ndim];
    s ^= 1;
  }

  for(i=ndim-1; i>0; i--) {
    x[i] += 2*(index/size2[i]);
    index %= size2[i];
    if(index>=size1[s][i]) {
      index -= size1[s][i];
      s ^= 1;
      x[i]++;
    }
  }
  x[0] += 2*index + s;

  free(m);

  if(QDP_index(x)!=si) {
    if(QDP_this_node==0) {
    fprintf(stderr,"QDP: error in layout!\n");
    for(i=0; i<ndim; i++) {
      fprintf(stderr,"%i\t%i\t%i\n", size1[0][i], size1[1][i], size2[i]);
    }
    fprintf(stderr,"%i\t%i", node, si);
    for(i=0; i<ndim; i++) fprintf(stderr,"\t%i", x[i]);
    fprintf(stderr,"\n");
    }
    QDP_abort_comm();
    exit(1);
  }
}
