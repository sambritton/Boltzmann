/* unique_molecules_core.h
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
extern int unique_molecules_core(int nzr,
				 struct molecule_struct *sorted_molecules,
				 char *molecules_text,
				 char *solvent_string,
				 int64_t *molecules_indices,
				 int64_t *compartment_indices,
				 int64_t *nunique_molecules,
				 int64_t *sum_molecule_len,
				 int     *solvent_pos,
				 int64_t align_len,
				 int64_t align_mask);
