# Boltzmann

This is a beta release of Boltzmann, a program for simulating mass action kinetics from both a thermodynamic and kinetic perspective. 
Boltzmann has the following capabilities: 
* Allows one to use a maximum entropy assumption to determine rate parameters when they are not otherwise available.
* Predicts steady state metabolite concentrations.
* Allows one to infer regulation of reactions.

Boltzmann has been used for maximum entropy production simulations for both stochastic and deterministic kinetics:
* Thomas, D. G.; Jaramillo-Riveri, S.; Baxter, D. J.; Cannon, W. R., Comparison of optimal thermodynamic models of the tricarboxylic acid cycle 
from heterotrophs, cyanobacteria, and green sulfur bacteria. J Phys Chem B 2014.
* Cannon, W. R.; Zucker, J. D.; Baxter, D. J.; Kumar, N.; Hurley, J. M.; Dunlap, J. C., Prediction of metabolite concentrations, 
rate constants and post-translational regulation using maximum entropy-based simulations 
with application to central metabolism of neurospora crassa. Processes 2018, 6 (6).

Background papers on the methods used are:
* Cannon, W. R., Simulating metabolism with statistical thermodynamics. PLoS One 2014, 9 (8), e103582.
* Cannon, W. R.; Baker, S. E., Non-steady state mass action dynamics without rate constants: Dynamics of coupled reactions 
using chemical potentials. Phys Biol 2017, 14 (5), 055003.

Boltzmann is written in C.

## To build

1.  Untar,
2.  Modify the Makefile.head file to fit your build environment.
3.  ./configure
4.  make 

This builds a libboltzmann.a library and boltzmann executable.


Interface Routines 

- int boltzmann_boot(char *param_file_name, 
  	           struct super_state_struct **super_statep);

   This routine takes the name of a parameter file, and sets
a pointer to a memory mapped super_state struct, returning 1 upon success,
and 0 on failure. It reads in many reactions and initial concentrations files, 
building individual state structure templates for each reaction file, 
combining them into one superstate which is memory mapped to a file.
These state structure templates may then be replicated, initialized, and
passed as arguments to boltzmann_run which runs a simulation of the reactions
represented by the state structure, returning concentrations of the 
involved molecules after the end of the simulation in a field of the state
structure.
If the user wishes to call boltzmann_boot to set up the 
superstate in one job and then use the produced super_state file in another
run, the boltzmann_load routine may be used to import the super_state
data structures in the subsequent job.

- int boltzmann_load(const char *superstate_filename, 
		   struct super_state_struct **super_statep);
   This routine takes the name of the super_state file generated by
a prior call to boltzmann_boot and memory maps it setting a pointer to
its memory mapped location.


- int boltzmann_rep_state_i(struct super_state_struct *super_statep, 
	  	          int i,
			  struct state_struct **l_statep);

   This routine takes as arguments the pointer to the super_state struct and
an index, and if *l_state_p == NULL, allocates space for a replica of the 
state_struct for that reaction file and initializes it from the 
super_state struct, otherwise it overwrites the state_struct pointed to by
*l_statep with the state for reaction file i (i in [0:number_of_reaction_files-1])


- int boltzmann_run(struct state_struct *state);

   This routine takes as input a pointer to one of the local state_structs, and
runs the simulation specified by the input fields in the struct setting
the delta_g_forward, entropy, concentrations, boundary_fluxes and
random number generator state fields of the local state_struct.


- int boltzmann_global_to_local_concs(struct super_state_struct *super_state,
  				    double *global_concentrations,
 				    struct state_struct *local_state);

   This routine takes a pointer to the super_state, a pointer to a vector
of global concentrations, and a pointer to a local state_struct
and sets the concentration values in the local state_struct from the
global concentrations vector, using the molecules map in the superstate 
struct.


- int boltzmann_local_to_global_concs(struct super_state_struct *super_state,
				    double *global_concentrations,
				    struct state_struct *local_state);

  This routine provides the inverse mapping that boltzman_global_to_local_concs
provides, so it sets the corresponding elements of a global concentrations
vector from the concentrations field of the supplied local state_struct.



- int boltzmann_global_to_local_fluxes(struct super_state_struct *super_state,
				     double *global_fluxes,
				     struct state_struct *local_state);
   This routine takes a pointer to the super_state, a pointer to a vector
of global boundary fluxes, and a pointer to a local state_struct
and sets the boundary flux values in the local state_struct from the
global boundary_flux concentrations vector, using the molecules map in the 
super_state struct.


- int boltzmann_local_to_global_fluxes(struct super_state_struct *super_state,
				     double *global_fluxes,
				     struct state_struct *local_state);
  This routine peforms the inverse mapping that 
boltzman_global_to_local_fluxes provides, so it sets the corresponding 
elements of a global boundary fluxes vector from the boundary fluxes field 
of the supplied local state_struct.

	
- int64_t boltzmann_global_molecule_count(struct super_state_struct *super_state);
  This routine returns the global_number_of_molecules field of the super_state
struct.
	 

- int64_t boltzmann_length_state_i(struct super_state_struct *super_state,
				 int i);
This routine returns the length of the local state for the i'th reaction file.


- int64_t boltzmann_max_local_state_size(struct super_state_struct *super_state);

  This routine returns the maximum_state_size_in_bytes field of the
super_state_struct.


- int64_t boltzmann_size_superstate(struct super_state_struct *super_state);
  This routine returns the total_length_in_bytes field of the super_state
struct giving its length.


- int boltzmann_number_of_reaction_files(struct super_state_struct *super_state);
  This routine returns the number_of_reaction_files field of the super_state
struct.

## Exectuables
The *boltzmann* executable runs a simulation for a single reaction file.

The *boltzmann_boot_test* executable tests the execution of the boltzmann_boot
routine for a set of reaction files and their corresponding initial 
concentrations files.

Both the *boltzmann* and *boltzmann_boot_test executables* take a similar parameter
input file that specifies the reaction and initial concentration files for
boltzmann, and a list of such files for boltzmann_boot_test. These input files
also specify some other parameters which are ultimately stored in each 
individual state structure. Those parameter files are ascii text lines
of keyword value pairs. Ordering of the lines within the file is not important.



## Include Files 
The header files for all routines are in routine_name.h. For the data structs
being used including boltzmann_structs.h should suffice.
   

## Parameter Files
The boltzmann_boot routine takes as its first argument the name of the
parameter input file and returns a pointer to a memory mmap region of memory
corresponding to the initial byte of the superstate structure described below.

The parameter file for boltzmann_boot routine and boltzmann_boot_test have the 
same keyword format and the keywords and corresponding values and their
defaults are given. The kewords are all uppercase letters with underscores 
and separated from the following value by at least one space or tab.
The description of the values is given lowercase underscore separtated 
descriptions and the default value is specified in angle brackets after the
value discription (and should be removed from the line in the input field.
The values specifiying file names are case sensitive. default value is given
in angle brackets

Keywords for the boltzmann_boot routine/boltzmann_boot_test executable:

RXN_FILE filename_of_reaction_list <boltzmann.in> 
WARMUP_STEPS number_of_reaction_firings_before_recording <1000>
RECORD_STEPS number_of_reaction_firings_after_warmup_to_run <1000>
TEMP_KELVIN  the_environment_temperature_in_degrees_kelvin <298.15>
IDEAL_GAS_R  the_value_to_use_for_the_ideal_gas_constant <0.00198858775>
USE_ACTIVITES 0_for_no_1_for_yes <0>

The file specified by RXN_FILE should be a file with a lines containing 
pairs of file names. The first file-name on a line should be the name
of a reactions file (see below) for example test.dat or calvin.dat.
The second file-name on the line should be the name of the corresponding
intial-concentrations file for example test_concs.in or calvin_concs.in.
A sample RXN_FILE for the boltzmann_boot routines is in boot.lis

A sample parameter input file for boltzmann_boot is in boot.in

The parameter file for the boltzmann routine for running an isolated reactions file presumably for debugging/verification purposes has the same keywords 
mentioned above for the boltzmann_boot routines and some additional keywords 
for controlling the printing.
Also the RXN_FILE keyword has a different interpretation: It is the file name 
of a reactions file (eg. test.dat or calvin.dat) instead of a file 
containing  a list of reaction-file initial-concentration file pairs.
A  keyword value line of the form 

INIT_FILE inital_concentraions_file <rxns.concs>

is used to specify the initial concentrations for the boltzmann executable.
The additional keywords for the boltzmann parameter file are

PRINT_OUTPUT 0_or_1 <0>

If PRINT_OUTPUT is set to 0 no printing is done.
If PRINT_OUTPUT is set to 1 the following keyword value pairs may be used
to control the output.

LOG_FILE logfilename <rxns.log> 

The parameters read/set by read_params.c are echoed to the LOG_FILE

CONCS_OUT_FILE filename_for_printing_concentrations_for_recorded_steps <concs.out>

CONC_VIEW_FREQ frequency_in_number_of_reaction_firings_between_concentrations_printing <0>

RXN_LKLHD_FILE filename_for_printing_likelihoods_for_recored_steps <rxns.lklhd>

LKLHD_VIEW_FREQ frequency_in_number_of_reaction_firings_between_likelihood_printing <0>

RXN_VIEW_FREQ frequency_in_number_of_reaction_firings_between_likelihood_printing <0>

RXN_VIEW_FILE filename_for_printing_transpose_of_likelihoods_matrix <rxns.view>

RXN_VIEW_FREQ frequency_in_number_of_reaction_firings_between_reaction_view_printing <0>


RESTART_FILE filename_for_printing_last_set_of_concentrations_to <restart.concs>
BOUNDARY_FLUX_FILE filename_for_printing_last_boundary_fluxes <boundary_flux.txt>

FREE_ENERGY_FILE filename_for_printing_free_energies <rxns.fe>

FREE_ENERGY_FORMAT one_of_four_values:
	NONE for no free  energy printing, may use 0 instead of NONE
	NEG_LOG_LKLHD for printing the negative log likelihood ratios of the 
                      free energy, may use 1 instead of NEG_LOG_LKLHD

	KJ/MOL for printing the free energies in KJoules/mole 
	       may use 2 instead of KJ/MOL

        KCAL/MOL for printing the free energies in KCals/mole
	       may use 3 instead of KCAL/MOL
 

----------------------------------------------------------------------------

LOCAL state_struct components.
The indiviual local state structures are documented in the state_struct.h
file. It is convenient to view the state_struct as being composed of four 
logically partitions of which only the first 2 directly concern the user.

The first partition we call the meta_data partition and it primarily has 
information describing the size and location of the other parts of the state 
struct, and constants relevant to the boltzmann algorithm. It is set by the 
boltzmann_boot and boltzmann_init routines and its self referential 
pointer fields are set by the flatten_state routine. 


The meta_data partition has five fields used as a way of labeling the output 
from the boltzman_run contained in the state struct for 
future user reference. Four of those five fields may be set by the user.
  /* The agent_type should be an ordinal number reflecting which of        */
  /* several reaction files this state refers to. It is set by             */
  /* boltzmann_boot, and not modified by any of the other routines.        */
  int64_t agent_type;
  /*                                                                       */
  /* The thread_id is meant to hold information about the calling agent.   */
  /* It could be an mpi_rank or a pthreads thread id or a combination of   */
  /* those or anything the user wishes. It is not modified by any of the   */
  /* boltzmann routines. 
  int64_t thread_id;
  /*                                                                       */
  /* The x_coord, y_coord, and z_coord fields are also just label          */
  /* fields to augment the thread_id, and agent_type fields. They are      */
  /* included for adding spatial location information to the instance of   */
  /* the state structure. They are not modified by any of the boltzmann    */
  /* routines.                                                             */
  double  x_coord;
  double  y_coord;
  double  z_coord;

Another field in the meta_data partition of interest is the state_length
field 

  int64_t state_length;

which is the length in bytes of the "flattened" state_struct 
(including all four partitions(.

The second partition termed the two_way_data partition corresponds to 
data that is both used and modified or just set by the  
boltzmann_run routine. This is the only part of the state struct that the
Biocellion/other external user needs to interact with when calling 
boltzmann_run after calling the boltzmann_boot or boltzmann_init routines.

The third partition, the incoming_data partition, corresponds to data that is 
only used and not modified by the boltzmann_run routine. 
It is set by boltzmann_boot or boltzmann_init routines, and its 
corresponding fields should not be modified by the external user.

The fourth partition, the auxilliary_data parition, contains data 
useful for interpreting the ouput but not used or modified by the boltzmann_run
routine.  It is also initialized by boltzmann_boot or boltzmann_init calls 
and should not be modified by the external user.

The boltzmann_run routine also requires some workspace, and while the
size of that workspace is stored in the meta_data partition, its length
is not included in the "state_length" field as the boltzmann_run routine
will allocate that space if the "workspace_base" field is null.

FIELDS OF THE THE TWO_WAY_DATA PARTITION AND RELATED META_DATA FIELDS.
 The fields of the meta_data partition that pertain to the descripton of the 
 two_way_data_partition are as follows:
 
  /*                                                                       */
  /* The two_way_data_offset is the offset in eight_byte words from the    */
  /* begining of this state structure of the two_way (used and modified)   */
  /* data vectors.                                                         */
  int64_t two_way_data_offset;  	
  /*                                                                       */
  /* The two_way_data_length field is length in eight_byte words of the    */
  /* state vectors (enumerated below )containing data that is both used    */
  /* and modified by the boltzmann_run routine. This is set by the         */
  /* boltzmann_boot or boltzmann_init routine.                             */
  int64_t two_way_data_length;  	
  /*                                                                       */
  /* The nunique_molecules field is the number of different molecules      */
  /* in the reaction file.                                                 */
  int64_t nunique_molecules;     	
  /*                                                                       */
  /* The following 4 meta_data fields are set for an instance of the       */
  /* state structure by calling the flatten_state routine with the pointer */
  /* to the state structure as the first argument and the address of the   */
  /* pointer to the state structure as the second argument (see for        */
  /* example the boltzmann_boot_check routine)                             */
  /* These four fields are pointers to the data that is used and modified  */
  /* or just set by the boltzmann_run routine. After flatten_state is      */
  /* called as described above, they point to locations of the data        */
  /* within the instance of the state structure.                           */
  double  *dg_forward_p; 
  double  *entropy_p;
  double  *current_concentrations; 
  double  *bndry_flux_concs;       

The dg_forward_p field points to the location of the scalar containing the
change in Gibbs free energy for the forward reactions at the completion
of the boltzmann_run call.

The entropy_p field points to the location of the scalar containing the
system entropy after the simulation is run by boltzmann_run

The current_concentrations field points to a vector of initial concentrations
on input, and the resulting concentrations from boltzmann_run at completion.
This vector has "nunique_molecules" elements.

The bndry_flux_concs field points to a vector of the boundary fluxes after
the boltumann_run routine completes. It also has "nunique_molecules" 
elements.

If "state_p" is the address of a state_struct (struct *state_struct state_p;) 
and "state_d" is the cast of that address to a pointer a double (eight byte 
floating point element);

   state_d = (double *)state_p;

and if 
   int64_t twdo = state_p->two_way_data_offset;

Then state_d[twdo] contains the dg_forward value,
     state_d[twdo+1] contains the entropy value,
	
     state_d[twdo+2:twdo+nunique_molelcules+1] contains the current 
          concentrations values, and

     state_d[twdo+nunique_molecules+2:twdo+2*unique_molecules+1] contains
          the boundary_flux concentrations.

calling flatten_state as described above allows one instead to refer to 
     these quantities as *(state_p->dg_forward_p),
	                 *(state_p->entropy_p),
                 state_p->current_concentrations[0:state_p->nunique_molecules-1],
                         and 
                 state_p->bndry_flux_concs[0:state_p->nunique_molecules-1],
     

Actually there are two more components of the two_way data partition, 
the state structures for two distinct random number generators. 
These are initialized by boltzmann_boot or boltzmann_init and not meant for 
the users to modify.
They are part of the state that is preserved across calls and modified each
time so as not to be always restarting the random number generators used by
the boltzmann_run simulation.

The complete details of the state structure are given in the state_struct.h
file.

--------------------------------------------------------------------------


DETAILS OF THE SUPERSTATE FILE, OUTPUT FROM BOLTZMANN_BOOT.

The boltzmann_boot routine builds a state structure for each of the specified
reaction files and corresponding initial concentrations files and then
catenates them into a
superstate file containing a meta_data partition that has information 
describing the lengths and locations of the local state structures in the
file. Also in the meta_data partition is the global dictionary and 
map vectors that map local molecule types for each reaction_file state
to their global dictionary position. 

The boltzmann_boot routine takes as input the file name of the parameter file
as discussed above and returns a pointer to the memory mapped superstate file.
The memory mapping of the file is done by the boltzmann_mmap_superstate 
routine. It takes the name of the superstate file,
its length as arguments and memory maps the file returning a pointer to 
the memory mapped superstate file. When creating the superstate file in one
run for use in subsequent jobs, calling the boltzmann_mmap_superstate routine
with the name of the superstate file and its length would avoid having to
recompute it.


The layout of the super_state struct is described in the super_state_struct.h file.

After the length descriptors in the super_state_struct, memory is laid out as
follows as part of the super_state struct.

The following 8 byte intgeger vectors, that start at the offsets indicated
by fields of the super_state_struct.

state_offsets_sizes[0:2*number_of_reaction_files-1]
    state_offsets_sizes[2*i] = offset in bytes of the state_struct for 
                               the i'th reaction file listed in the
	                       RXN_FILE parameter specified in the
	                       parmeter file, 
	                       for i = 0:number_of_reaction_files-1.
    state_offsets_sizes[2*i+1] Length of the state_struct for the i'th
                               the i'th reaction file listed in the
	                       RXN_FILE parameter specified in the
	                       parmeter file, 
	                       for i = 0:number_of_reaction_files-1.
	                       
       offset in bytes, length pairs of the reaction states.

molecule_map_starts[0:number_of_reaction_files]
      molecule_map_starts[i] is an offset into the molecule_map and
      compartment_map arrays where the mapping for molecules
      found in the i'th reaction file starts, i=0:number_of_reaction_files-1.
      the last entry points to one beyond the last element of 
      molecule_map_starts, and is the global_number_of_molecules.

      The global molecule position for the j'th unique molecule in
      the i'th reaction file is given by

      molecule_map[molecule_map_starts[i]+j] for 
           j = 0:state->nunique_molecules-1 for the state structure of
	         the i'th reaction file.

      and the corresponding compartment number for that molecule is
      contained in

      compartment_map[molecule_map_starts[i]+j]
      

molecule_map[0:global_number_of_molecules-1] the translation of individual
      unique molecules within each reaction file to their 
      position in the global dictionary, see molecule_map_starts above.

compartment_map[0:global_number_of_molecules-1] the translation of compartment	      numbers for unique molecules within each reaction file to their
      compartment position in the global dictionary, see molecule_map_starts
      above.

molecule_names[0:unqiue_global_molecules-1]
      These will be character offsets relative to the beginning of the 
      molecules_text address for each unique molecule name.
    
compartment_names  [0:unique_global_compartments-1]
      These will be character offsets relative to the beginning of
      the compartments_text address for each unique compartment name.


The following two arrays are character arrays, stored in the metadata 
partition of the superstate, just after the 8byte integer arrays listed 
above.

molecule_text[0:molecule_text_length-1] character array containing the 
       aligned unique molecule names for the global dictionary.

comparmtent_text[0:compartment_text_length-1]
       aligned unique compartment names for the global dictionary.

After the meta_data partition, the flattened state structures for each reaction
file are stored in the superstate file on aligned on disk_page size boundarys,
with their offsets and lengths stored in the state_offsets_sizes array in the
super_state structure. 

The boltzmann_boot_check (in boltzmann_boot_check.c) routine is an example of 
using the state structures to echo the all of the reaction files and initial
concentrations using the molecule_map_start array in conjunction with the 
local state_structures. Note that the pointer returned from boltzmann_boot 
(or from boltzmann_mmap_superstate) is a pointer to read-only memory mapped 
data. So one needs to copy the local state parts to a local buffer  
(lstate in boltzmann_boot_check - see the memcpy call) and then call 
flatten_state on it them enable the use of the pointer fields such as 
lstate->current_concentrations. This is now done in the 
boltzmann_rep_state_i routine.

If one is only passing the local state to
boltzmann_run, flatten_state need not be called as boltzmann_run will do that,
but to conveniently access the output fields in the local_state via its own
pointer fields, the call to boltzmann_rep_state_i which calls
flatten_state is recommended.

--------------------------------------------------------------------------

Still to write, reactions file format, initial_concentrations_file formats

