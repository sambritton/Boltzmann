/* read_initial_concentrations.c
*******************************************************************************
boltzmann

Pacific Northwest National Laboratory, Richland, WA 99352.

Copyright (c) 2010 Battelle Memorial Institute.

Publications based on work performed using the software should include 
the following citation as a reference:


Licensed under the Educational Community License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
The terms and conditions of the License may be found in 
ECL-2.0_LICENSE_TERMS.TXT in the directory containing this file.
        
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
CONDITIONS OF ANY KIND, either express or implied. See the License for the 
specific language governing permissions and limitations under the License.
******************************************************************************/

#include "boltzmann_structs.h"

#include "molecules_lookup.h"
#include "compartment_lookup.h"
#include "upcase.h"

#include "read_initial_concentrations.h"
int read_initial_concentrations(struct state_struct *state) {
  /*
    Read the concs.in file for initial concentrations, and
    set the concentrations array.
    Called by: boltzmann_init
    Calls:     molecules_lookup
               compartment_lookup,
	       upcase,
               fopen, fgets, fclose, fprintf, fflush (intrinsic)
  */
  struct  molecule_struct *sorted_molecules;
  struct  molecule_struct *molecule;
  double  volume;
  double  conc_units;
  double  conc;
  double  count;
  double  conc_multiple;
  double  c_multiple;
  double  multiplier;
  double  avogadro;
  double  half;
  double  e_val;
  double  u_val;
  double  *bndry_flux_counts;
  double  *counts;
  double  *kss_e_val;
  double  *kss_u_val;
  int64_t molecules_buff_len;
  int64_t one_l;
  char *molecules_buffer;
  char *molecule_name;
  char *compartment_name;
  char *variable_c;
  char *compute_c;
  char *fgp;

  int success;
  int nzr;

  int nu_molecules;
  int i;

  int nscan;
  int variable;

  int si;
  int ci;

  int mol_len;
  int cmpt_len;

  int num_fixed_concs;
  int solvent;

  int compute_conc;
  int padi;
  
  char vc[2];
  char cc[2];
  
  FILE *conc_fp;
  FILE *counts_out_fp;
  avogadro           = state->avogadro;
  half               = 0.5;
  nu_molecules       = state->nunique_molecules;
  molecules_buff_len = state->max_param_line_len;
  molecules_buffer   = state->param_buffer;
  molecule_name      = molecules_buffer + state->max_param_line_len;
  compartment_name   = molecule_name + (state->max_param_line_len>>1);
  sorted_molecules   = state->sorted_molecules;
  counts             = state->current_counts;
  kss_e_val          = state->kss_e_val; 
  kss_u_val          = state->kss_u_val; 
  bndry_flux_counts  = (double *)state->bndry_flux_counts;
  success = 1;
  one_l = (int64_t)1;
  variable_c = (char *)&vc[0];
  compute_c  = (char *)&cc[0];
  for (i=0;i<nu_molecules;i++) {
    counts[i] = -1.0;
    bndry_flux_counts[i] = -1.0;
  }
  num_fixed_concs = 0;
  conc_fp = fopen(state->init_conc_file,"r");
  if (conc_fp) {
    /*
      Read the required volume line.
    */
    fgp = fgets(molecules_buffer,molecules_buff_len,conc_fp);
    if (fgp) {
      if (strncmp(molecules_buffer,"VOLUME",6) != 0) {
	fprintf(stderr,
		"read_intial_concentrations Error: Concentratiosn input file "
		"does not start with a VOLUME line.\n");
	fflush(stderr);
	success = 0;
      } else {
	nscan = sscanf((char*)&molecules_buffer[6],"%le",&volume);
	if (nscan != 1) {
	  fprintf(stderr,
		  "read_intial_concentrations Error: invalid volume spec.\n");
	  fflush(stderr);
	  success = 0;
	} else {
	  state->volume = volume;
	}
      }
    } else {
      fprintf(stderr,
	      "read_initial_concentrations Error: Empty concentrations "
	      "file.\n");
      fflush(stderr);
      success = 0;
    }
  } else {
    fprintf(stderr,
	    "read_intial_concentrations Error: Unable to open inital "
	    "concentrations file, %s\n",state->init_conc_file);
    fflush(stderr);
    success = 0;
  }
  if (success) {
    fgp = fgets(molecules_buffer,molecules_buff_len,conc_fp);
    if (fgp) {
      if (strncmp(molecules_buffer,"CONC_UNITS",10) != 0) {
	fprintf(stderr,
		"read_intial_concentrations Error: Concentratiosn input file "
		"does not have a second line with CONC_UNITS setting.\n");
	fflush(stderr);
	success = 0;
      } else {
	nscan = sscanf((char*)&molecules_buffer[10],"%le",&conc_units);
	if (nscan != 1) {
	  fprintf(stderr,
	  "read_intial_concentrations Error: invalid conc_units spec.\n");
	  fflush(stderr);
	  success = 0;
	} else {
	  state->conc_units = conc_units;
	}
      }
    } else {
      fprintf(stderr,
	      "read_initial_concentrations Error: No CONC_UNITS line in "
	      "concentrations file.\n");
      fflush(stderr);
      success = 0;
    }
  }
  if (success) {
    multiplier = conc_units * volume * avogadro;
    state->conc_to_count = multiplier;
    if (multiplier > 0.0) {
      state->count_to_conc = 1.0/multiplier;
    }
    while (!feof(conc_fp)) {
      fgp = fgets(molecules_buffer,molecules_buff_len,conc_fp);
      if (fgp) {
	e_val = 0.0;
	u_val = 0.0;
	nscan = sscanf(molecules_buffer,"%s %le %1s %1s %le %le",
		       molecule_name, &conc, variable_c, compute_c, 
		       &e_val, &u_val);
	variable = 1;
	solvent  = 0;
	if (nscan >= 3) {
	  /*
	    A variable or constant specifier was givven.
	  */
	  /*
	    Upper case the variable character.
          */
	  vc[0] = vc[0] & 95;

	  if (strncmp(variable_c,"F",one_l) == 0) {
	    variable = 0;
	    num_fixed_concs += 1;
	  }
	  if (strncmp(variable_c,"V",one_l) == 0) {
	    variable = 1;
	  }
	}
	compute_conc = 0;
	if (nscan >= 4) {
	  /*
	    A fixed or compute value was given for the initial concentration.
	    Upper case the compute value character.
	  */
	  cc[0] = cc[0] & 95;
	  if (strncmp(compute_c,"U",one_l) == 0) {
	    compute_conc = 0;
	  } else {
	    if (strncmp(compute_c,"I",one_l) == 0) {
	      compute_conc = 1;
	    } else {
	      if (strncmp(compute_c,"C",one_l) == 0) {
		compute_conc = 2;
	      } else {
		compute_conc =0;
	      }
	    }
	  }
	}
	if (nscan >= 6) {
	  /*
	    An expermental and user intial values were supplied.
	    Don't need to do anything yet as they are in e_val and u_val,
	    or else e_val and u_val are 1.0
	  */
	  if ((e_val == 0.0) || (u_val == 0.0)) {
	    /*
	      Do we want to print and error message ?
	      We would just set the 0 one to be the nonzero one,
	      or if both zero set them to 1. Since what we are
	      always interested in is their ratio, setting both
	      to 1 if either is 0 accomplishes that effect.
	    */
	    e_val = 1.0;
	    u_val = 1.0;
	  }
	} else {
	  /*
	    No modification of e_q is to happen.
	  */
	  u_val = 1.0;
	  e_val = 1.0;
	}
	mol_len = strlen(molecule_name);
	compartment_name = molecule_name;
	for (i=0;i<mol_len-1;i++) {
	  if (molecule_name[i] == ':') {
	    compartment_name = (char *)&molecule_name[i+1];
	    molecule_name[i] = '\0';
	    cmpt_len = mol_len - i - 1;
	    mol_len = i;
	    break;
	  }
	}
	if (compartment_name == molecule_name) {
	  ci = 0;
	} else {
	  upcase(cmpt_len,compartment_name,compartment_name);
	  ci = compartment_lookup(compartment_name,state);
	}
	if (nscan >= 2) {
	  upcase(mol_len,molecule_name,molecule_name);
	  si = molecules_lookup(molecule_name,ci,state);
	  if ((si >=0) && si < nu_molecules) {
	    /*
	      The following uses the nearest integer to (conc*multiplier)
	      for the count field stored in the counts array, where
	      multiplier is volume * units * Avogadro's number.
	    */
	    counts[si] = (double)((int64_t)((conc * multiplier) + half));
	    kss_e_val[si] = e_val;
	    kss_u_val[si] = u_val;
	    molecule = (struct molecule_struct *)&sorted_molecules[si];
	    molecule->variable = variable;
	    molecule->compute_init_conc = compute_conc;
	    molecule->solvent           = solvent;
	  } else {
	    fprintf(stderr,"read_initial_concentrations: Error "
		    "unrecognized molecule in %s was %s\n",
		    state->init_conc_file,molecule_name);
	    fflush(stderr);
	    success = 0;
	    break;
	  }
	} else {
	  fprintf(stderr,"read_initial_concentrations: Error "
		  "poorly formated line was\n%s\n",molecules_buffer);
	  fflush(stderr);
	  success = 0;
	}
      } /* end if (fgp) */
    } /* end while (!feof(conc_fp)) */
    fclose(conc_fp);
  } else {
    fprintf(stderr,
	    "read_initial_concentrations: Warning unable to open %s\n",
	    state->init_conc_file);
    fflush(stderr);
    success = 0;
  }
  if (success) {
    count = state->default_initial_count;
    for (i=0;i<nu_molecules;i++) {
      if (counts[i] < 0.0) {
	counts[i] = count;
      }
    }
    for (i=0;i<nu_molecules;i++) {
      bndry_flux_counts[i] = counts[i];
    }
    state->num_fixed_concs = num_fixed_concs;
    /*
      Print the initial counts to the counts output file.
    */
    if (state->print_output) {
      counts_out_fp = state->counts_out_fp;
      if (counts_out_fp) {
	fprintf(counts_out_fp,"init");
	for (i=0;i<nu_molecules;i++) {
	  fprintf(counts_out_fp,"\t%le",counts[i]);
	}
	fprintf(counts_out_fp,"\n");
      } else {
	fprintf(stderr,
		"read_initial_concentrations: Error counts_out_fp not open\n");
	fflush(stderr);
	success = 0;
      }
    }
  }
  return(success);
}
