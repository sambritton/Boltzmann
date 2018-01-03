/* compute_ke.c
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

#include "compute_ke.h"
int compute_ke(struct state_struct *state) {
  /*
    Fill the dg0s vector from the reactions struct
    and compute the equilibrium constants from the delta G_0's.

    K_eq  = exp(-delta_G_0/RT)
    
    Called by: energy_init
    Calls:      exp
  */
  struct rxn_struct *reactions;
  struct rxn_struct *reaction;
  /*
  struct species_matrix_struct species_matrix;
  */
  double dg0;
  double *dg0s;
  double *ke;
  double m_r_rt;
  double cals_per_joule;

  int success;
  int nrxns;
  int i;
  int padi;
  /*
  int print_output;
  FILE *lfp;
  */
  success   = 1;
  nrxns     = (int)state->number_reactions;
  reactions = state->reactions;
  dg0s      = state->dg0s;
  ke        = state->ke;
  /*
  print_output = (int)state->print_output;
  lfp          = state->lfp;
  */
  /*
    m_r_rt = -1/(RT)
  */
  cals_per_joule    = state->cals_per_joule;
  m_r_rt = state->m_r_rt;
  reaction = reactions;
  /*
  if (print_output) {
    if (lfp) {
      fprintf(lfp,"\n\nOutput from compute_ke\n");
    }
  }
  */
  for (i=0;i<nrxns;i++) {
    if (reaction->unit_i == 1) {
      dg0 = reaction->delta_g0;
    } else {
      /*
	Delta_g0 units was in Kcals, convert to KJoules.
      */
      dg0 = reaction->delta_g0 / cals_per_joule;
    }
    dg0s[i] = dg0;
    ke[i] = exp(dg0 * m_r_rt);
    /*
    if (print_output) {
      if (lfp) {
	fprintf(lfp,"dg0s[%d] = %le, ke[%d] = %le\n",
		i,dg0s[i],i,ke[i]);
      }
    }
    */
    reaction += 1; /* Caution address arithmetic */
  }
  /*
  if (print_output) {
    if (lfp) {
      fflush(lfp);
    }
  }
  */
  return (success);
}
  
