/* $Id$ */

#define MAIN_PROGRAM

#ifdef HAVE_CONFIG_H
# include<config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#ifdef MPI
# include <mpi.h>
#endif
#include "global.h"
#include "getopt.h"
#include "io.h"
#include "read_input.h"
#include "mpi_init.h"
#ifdef MPI
# include "xchange.h"
#endif
#include "geometry_eo.h"
#include "observables.h"
#include "init_gauge_field.h"
#include "init_geometry_indices.h"

void usage(){
  fprintf(stderr, "ildg2gwc gauge conversion program\n\n");
  fprintf(stderr, "Usage: [-f parameter input-filename]\n");
  fprintf(stderr, "Usage: [-i gague input-filename]\n");
  fprintf(stderr, "Usage: [-o gauge output-filename]\n");
  exit(1);
}

int main(int argc,char *argv[]) {
  char * ifilename = NULL;
  char * ofilename = NULL;
  char * input_filename = NULL;
  double plaquette_energy = 0;
  int trajectory_counter = 0;
  int c, j;

  L=0; 
  T=0;
  while ((c = getopt(argc, argv, "h?f:i:o:")) != -1) {
    switch (c) {
    case 'f': 
      input_filename = calloc(200, sizeof(char));
      strcpy(input_filename,optarg);
      break;
    case 'i': 
      ifilename = calloc(200, sizeof(char));
      strcpy(ifilename,optarg);
      break;
    case 'o':
      ofilename = calloc(200, sizeof(char));
      strcpy(ofilename,optarg);
      break;
    case 'h':
    case '?':
    default:
      usage();
      break;
    }
  }

  if(ifilename == NULL) {
    fprintf(stderr, "You have to give an input file name! Aborting...\n");
    exit(1);
  }
  if(ofilename == NULL) {
    fprintf(stderr, "You have to give an output file name! Aborting...\n");
    exit(2);
  }
  if(input_filename == NULL){
    input_filename = "hmc.input";
  }

  verbose = 0;

  if(read_input(input_filename) != 0) {
    fprintf(stderr, "Input file required!\n Aborting...!\n");
  }

  mpi_init(argc, argv);

  if(g_rgi_C1 == 0.) {
    g_dbw2rand = 0;
  }
#ifndef MPI
  g_dbw2rand = 0;
#endif

#ifdef _GAUGE_COPY
  j = init_gauge_field(VOLUMEPLUSRAND + g_dbw2rand, 1);
#else
  j = init_gauge_field(VOLUMEPLUSRAND + g_dbw2rand, 0);
#endif
  if ( j!= 0) {
    fprintf(stderr, "Not enough memory for gauge_fields! Aborting...\n");
    exit(0);
  }
  j = init_geometry_indices(VOLUMEPLUSRAND + g_dbw2rand);
  if ( j!= 0) {
    fprintf(stderr, "Not enough memory for geometry_indices! Aborting...\n");
    exit(0);
  }

  geometry();

  if (g_proc_id == 0){
    printf("Reading Gauge field from file %s in ILDG format\n", ifilename); fflush(stdout);
  }
  read_lime_gauge_field( ifilename ); 

#ifdef MPI
  xchange_gauge();
#endif

  /*compute the energy of the gauge field*/
  plaquette_energy = measure_gauge_action();

  if (g_proc_id == 0){
    printf("The plaquette value is %e\n", plaquette_energy/(6.*VOLUME*g_nproc)); fflush(stdout);
  }
  if (g_proc_id == 0){
    printf("Writing Gauge field to file %s in ILDG format...\n", ofilename); fflush(stdout);
  }
  write_gauge_field_time_p( ofilename );
  return(0);
}