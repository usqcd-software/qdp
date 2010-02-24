#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "qdp_internal.h"

int QDP_sites_on_node = -1;

static QDP_Layout *def_layout = NULL;
static QDP_Lattice *def_lattice = NULL;

static int ndim=-1;
static int *latsize;
static int vol;

static int def_ndim = -1;
static int *def_latsize = NULL;

void
QDP_set_latsize(int nd, const int size[])
{
  QDP_assert(nd>0);
  QDP_assert(size!=NULL);

  QDP_free(def_latsize);
  def_ndim = nd;
  QDP_malloc(def_latsize, int, nd);
  memcpy(def_latsize, size, nd*sizeof(int));
}

int
QDP_create_layout(void)
{
  QDP_assert(def_latsize!=NULL);

  if(!def_layout) {
    TRACE;
    QDP_set_default_layout(QDP_layout_hyper_eo);
    TRACE;
  }

  TRACE;
  QDP_Lattice *lat = QDP_create_lattice(def_layout, NULL, def_ndim, def_latsize);
  TRACE;
  QDP_assert(lat!=NULL);
  TRACE;
  QDP_set_default_lattice(lat);
  TRACE;

  return 0;
}

/*
 *  Layout Query functions
 */

int
QDP_ndim(void)
{
  return ndim;
}

int
QDP_coord_size(int i)
{
  return latsize[i];
}

void
QDP_latsize(int ls[])
{
  memcpy(ls, latsize, ndim*sizeof(int));
}

int
QDP_volume(void)
{
  return vol;
}

int
QDP_numsites(int node)
{
  return def_lattice->layout->numsites(def_lattice, node);
}

int
QDP_node_number(const int x[])
{
  return def_lattice->layout->node_number(def_lattice, x);
}

int
QDP_index(const int x[])
{
  return def_lattice->layout->index(def_lattice, x);
}

void
QDP_get_coords(int x[], int node, int index)
{
  def_lattice->layout->get_coords(def_lattice, x, node, index);
}

QDP_Layout *
QDP_get_default_layout(void)
{
  return def_layout;
}

void
QDP_set_default_layout(QDP_Layout *layout)
{
  def_layout = layout;
}

QDP_Lattice *
QDP_get_default_lattice(void)
{
  return def_lattice;
}

void
QDP_set_default_lattice(QDP_Lattice *lat)
{
  def_lattice = lat;
  QDP_sites_on_node = QDP_sites_on_node_L(lat);
  ndim = QDP_ndim_L(lat);
  latsize = lat->dims;
  vol = lat->vol;

  TRACE;
  QDP_neighbor = QDP_neighbor_L(lat);
  TRACE;
  QDP_all = QDP_all_L(lat);
  QDP_even_and_odd = QDP_even_and_odd_L(lat);
  QDP_even = QDP_even_L(lat);
  QDP_odd = QDP_odd_L(lat);
  TRACE;
}

QDP_Lattice *
QDP_create_lattice(QDP_Layout *layout, void *args, int nd, int size[])
{
  QDP_Lattice *lat;
  QDP_malloc(lat, QDP_Lattice, 1);
  lat->layout = layout;
  lat->ndim = nd;
  QDP_malloc(lat->dims, int, nd);
  int vol=1;
  for(int i=0; i<nd; ++i) {
    lat->dims[i] = size[i];
    vol *= size[i];
  }
  lat->vol = vol;
  lat->params = NULL;
  lat->all = NULL;
  lat->eo = NULL;
  lat->neighbor = NULL;
  lat->refcount = 1;
  layout->setup(lat, args);
  lat->sites_on_node = QDP_numsites_L(lat, QDP_this_node);
  return lat;
}

// should really reference count
// +1 for create, +1 for each new field using it
// -1 for free, -1 for each freed field using it
// then free at 0
void
QDP_destroy_lattice(QDP_Lattice *lat)
{
  QDP_assert(lat);
  lat->refcount--;
  if(lat->refcount==0) {
    lat->layout->free(lat);
    if(lat->neighbor) {
      for(int i=0; i<lat->ndim; i++) {
	if(lat->neighbor[i]) QDP_destroy_shift(lat->neighbor[i]);
      }
      QDP_free(lat->neighbor);
    }
    if(lat->all) QDP_destroy_subset(lat->all);
    if(lat->eo) QDP_destroy_subset(lat->eo);
    QDP_free(lat->params);
    QDP_free(lat->dims);
    QDP_free(lat);
  }
}

QDP_Layout *
QDP_get_lattice_layout(QDP_Lattice *lat)
{
  QDP_assert(lat);
  return lat->layout;
}

void
QDP_allocate_lattice_params(QDP_Lattice *lat, size_t size)
{
  QDP_assert(lat);
  if(lat->params) {
    QDP_error("lattice params already allocated");
    QDP_abort(1);
  }
  QDP_malloc(lat->params, char, size);
}

void *
QDP_get_lattice_params(QDP_Lattice *lat)
{
  QDP_assert(lat);
  return lat->params;
}

int
QDP_sites_on_node_L(QDP_Lattice *lat)
{
  QDP_assert(lat);
  return lat->sites_on_node;
}

int
QDP_ndim_L(QDP_Lattice *lat)
{
  QDP_assert(lat);
  return lat->ndim;
}

int
QDP_coord_size_L(QDP_Lattice *lat, int i)
{
  QDP_assert(lat);
  QDP_assert(i>=0 && i<lat->ndim);
  return lat->dims[i];
}

void
QDP_latsize_L(QDP_Lattice *lat, int ls[])
{
  QDP_assert(lat);
  memcpy(ls, lat->dims, lat->ndim*sizeof(int));
}

int
QDP_volume_L(QDP_Lattice *lat)
{
  QDP_assert(lat);
  return lat->vol;
}

int
QDP_numsites_L(QDP_Lattice *lat, int node)
{
  QDP_assert(lat);
  return lat->layout->numsites(lat, node);
}

int
QDP_node_number_L(QDP_Lattice *lat, const int x[])
{
  QDP_assert(lat);
  return lat->layout->node_number(lat, x);
}

int
QDP_index_L(QDP_Lattice *lat, const int x[])
{
  QDP_assert(lat);
  return lat->layout->index(lat, x);
}

void
QDP_get_coords_L(QDP_Lattice *lat, int x[], int node, int index)
{
  QDP_assert(lat);
  lat->layout->get_coords(lat, x, node, index);
}
