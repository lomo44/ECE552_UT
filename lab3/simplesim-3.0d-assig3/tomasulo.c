
#include <limits.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "host.h"
#include "misc.h"
#include "machine.h"
#include "regs.h"
#include "memory.h"
#include "loader.h"
#include "syscall.h"
#include "dlite.h"
#include "options.h"
#include "stats.h"
#include "sim.h"
#include "decode.def"

#include "instr.h"

/* PARAMETERS OF THE TOMASULO'S ALGORITHM */

#define INSTR_QUEUE_SIZE         10

#define RESERV_INT_SIZE    4
#define RESERV_FP_SIZE     2
#define FU_INT_SIZE        2
#define FU_FP_SIZE         1

#define FU_INT_LATENCY     4
#define FU_FP_LATENCY      9

/* IDENTIFYING INSTRUCTIONS */

//unconditional branch, jump or call
#define IS_UNCOND_CTRL(op) (MD_OP_FLAGS(op) & F_CALL || \
                         MD_OP_FLAGS(op) & F_UNCOND)

//conditional branch instruction
#define IS_COND_CTRL(op) (MD_OP_FLAGS(op) & F_COND)

//floating-point computation
#define IS_FCOMP(op) (MD_OP_FLAGS(op) & F_FCOMP)

//integer computation
#define IS_ICOMP(op) (MD_OP_FLAGS(op) & F_ICOMP)

//load instruction
#define IS_LOAD(op)  (MD_OP_FLAGS(op) & F_LOAD)

//store instruction
#define IS_STORE(op) (MD_OP_FLAGS(op) & F_STORE)

//trap instruction
#define IS_TRAP(op) (MD_OP_FLAGS(op) & F_TRAP)

#define USES_INT_FU(op) (IS_ICOMP(op) || IS_LOAD(op) || IS_STORE(op))
#define USES_FP_FU(op) (IS_FCOMP(op))

#define WRITES_CDB(op) (IS_ICOMP(op) || IS_LOAD(op) || IS_FCOMP(op))

/* FOR DEBUGGING */

//prints info about an instruction
#define PRINT_INST(out, instr, str, cycle)    \
  myfprintf(out, "%d: %s", cycle, str);        \
  md_print_insn(instr->inst, instr->pc, out); \
  myfprintf(stdout, "(%d)\n",instr->index);

#define PRINT_REG(out, reg, str, instr) \
  myfprintf(out, "reg#%d %s ", reg, str);    \
  md_print_insn(instr->inst, instr->pc, out); \
  myfprintf(stdout, "(%d)\n",instr->index);

/* VARIABLES */

//instruction queue for tomasulo
static instruction_t *instr_queue[INSTR_QUEUE_SIZE];
//number of instructions in the instruction queue
static int instr_queue_size = 0;

//reservation stations (each reservation station entry contains a pointer to an instruction)
static instruction_t *reservINT[RESERV_INT_SIZE];
static instruction_t *reservFP[RESERV_FP_SIZE];

//functional units
static instruction_t *fuINT[FU_INT_SIZE];
static instruction_t *fuFP[FU_FP_SIZE];

//common data bus
static instruction_t *commonDataBus = NULL;

//The map table keeps track of which instruction produces the value for each register
static instruction_t *map_table[MD_TOTAL_REGS];

//the index of the last instruction fetched
static int fetch_index = 0;


void tmPushInsQueue(instruction_t *in_pInstuction) {
    if (instr_queue_size < INSTR_QUEUE_SIZE) {
        // Queue has emptry slots in it, shift down the queue
        instr_queue[instr_queue_size] = in_pInstuction;
        instr_queue_size++;
    }
}

instruction_t* tmPopInsQueue(){
    instruction_t* ret = instr_queue[0];
    int i;
    for (i = 0; i < INSTR_QUEUE_SIZE-1; i++) {
        instr_queue[i] = instr_queue[i + 1];
    }
    instr_queue_size--;
    return ret;
}

instruction_t* tmTopQueue(){
    return instr_queue[0];
}

/* FUNCTIONAL UNITS */


/* RESERVATION STATIONS */


/* 
 * Description: 
 * 	Checks if simulation is done by finishing the very last instruction
 *      Remember that simulation is done only if the entire pipeline is empty
 * Inputs:
 * 	sim_insn: the total number of instructions simulated
 * Returns:
 * 	True: if simulation is finished
 */
static bool is_simulation_done(counter_t sim_insn) {

    /* ECE552: YOUR CODE GOES HERE */
    if(fetch_index >= sim_insn && instr_queue_size == 0)
        return true; //ECE552: you can change this as needed; we've added this so the code provided to you compiles
    return false;
}

/* 
 * Description: 
 * 	Retires the instruction from writing to the Common Data Bus
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void CDB_To_retire(int current_cycle) {
    /* ECE552: YOUR CODE GOES HERE */
    int i,j;
    if(commonDataBus!=NULL){
        if(current_cycle - commonDataBus->tom_cdb_cycle >= 1){
            // Broadcasting the result back to CDB
            for(i = 0; i < RESERV_FP_SIZE; i++){
                if(reservFP[i]!=NULL){
                    for(j = 0 ; j < 3 ;j++){
                        if(reservFP[i]->Q[j]==commonDataBus){
                            reservFP[i]->Q[j] = NULL;
                        }
                    }
                }
            }
            for(i = 0; i < RESERV_INT_SIZE; i++){
                if(reservINT[i]!=NULL){
                    for(j = 0 ; j < 3 ;j++){
                        if(reservINT[i]->Q[j]==commonDataBus){
                            reservINT[i]->Q[j] = NULL;
                        }
                    }
                }
            }
            for(i = 0; i < MD_TOTAL_REGS;i++){
                if(map_table[i]==commonDataBus){
                    map_table[i] = NULL;
                }
            }
            commonDataBus = NULL;
        }
    }
}


/* 
 * Description: 
 * 	Moves an instruction from the execution stage to common data bus (if possible)
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void execute_To_CDB(int current_cycle) {
    /* ECE552: YOUR CODE GOES HERE */
    int i;
    instruction_t* ins_toCDB = NULL;
    // Getting the oldest FP instruction in the function unit
    for(i = 0; i < FU_FP_SIZE; i++){
        instruction_t* current_instruction = fuFP[i];
        if(current_instruction!=NULL){
            if(current_cycle - current_instruction->tom_execute_cycle >= FU_FP_LATENCY){
                    if(ins_toCDB == NULL){
                        ins_toCDB = current_instruction;
                    }
                    else{
                        if(current_instruction->index < ins_toCDB->index){
                            ins_toCDB = current_instruction;
                        }
                    }
            }
        }
    }
    // Getting the oldest Int instruction in the function unit
    for(i = 0; i < FU_INT_SIZE; i++){
        instruction_t* current_instruction = fuINT[i];
        if(current_instruction!=NULL){
            if(current_cycle - current_instruction->tom_execute_cycle >= FU_INT_LATENCY){
                if(WRITES_CDB(current_instruction->op)){
                    if(ins_toCDB == NULL){
                        ins_toCDB = current_instruction;
                    }
                    else{
                        if(current_instruction->index < ins_toCDB->index){
                            ins_toCDB = current_instruction;
                        }
                    }
                }
                else{
                    int j;
                    for(j = 0; j < RESERV_INT_SIZE; j++){
                        if(reservINT[j]==current_instruction)
                            reservINT[j] = NULL;
                    }
                    fuINT[i] = NULL;
                }
            }
        }
    }
    // Push the selected instruction to CDB
    if(ins_toCDB!=NULL){
        if(commonDataBus==NULL){
            ins_toCDB->tom_cdb_cycle = current_cycle;
            commonDataBus = ins_toCDB;
            for(i = 0; i < FU_INT_SIZE;i++){
                if(fuINT[i]==commonDataBus)
                    fuINT[i]=NULL;
            }
            for(i = 0; i < FU_FP_SIZE;i++){
                if(fuFP[i]==commonDataBus)
                    fuFP[i]=NULL;
            }
            for(i = 0; i < RESERV_FP_SIZE; i++){
                if(reservFP[i]==ins_toCDB)
                    reservFP[i] = NULL;
            }
            for(i = 0; i < RESERV_INT_SIZE; i++){
                if(reservINT[i]==ins_toCDB)
                    reservINT[i] = NULL;
            }
        }
    }
}

/* 
 * Description: 
 * 	Moves instruction(s) from the issue to the execute stage (if possible). We prioritize old instructions
 *      (in program order) over new ones, if they both contend for the same functional unit.
 *      All RAW dependences need to have been resolved with stalls before an instruction enters execute.
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void issue_To_execute(int current_cycle) {

  /* ECE552: YOUR CODE GOES HERE */
    instruction_t* fp_next = NULL;
    instruction_t* int_next[2];
    int_next[0]=NULL;
    int_next[1]=NULL;
    instruction_t* inst_disp;
    for (int i = 0; i < 2; ++i) {
        if (reservFP[i] != NULL) {
            inst_disp = reservFP[i];
            if (inst_disp->Q[0]==NULL && inst_disp->Q[1]==NULL && inst_disp->Q[2]==NULL) {
                if (fp_next == NULL)
                    fp_next = inst_disp;
                else if (fp_next->index > inst_disp->index)
                    fp_next = inst_disp;
            }
        }
    }
    if (fp_next != NULL && fuFP[0] == NULL){
        fuFP[0] = fp_next;
        fp_next->tom_execute_cycle = current_cycle;
    }

    for (int j = 0; j < 4; ++j) {
        if (reservINT[j] != NULL) {
            inst_disp = reservINT[j];
            if (inst_disp->Q[0]==NULL && inst_disp->Q[1]==NULL && inst_disp->Q[2]==NULL && inst_disp->tom_execute_cycle == -1) {
                if (int_next[0] == NULL)
                    int_next[0] = inst_disp;
                else if (int_next[0]->index > inst_disp->index){
                    int_next[1] = int_next[0];
                    int_next[0] = inst_disp;
                } else if (int_next[1] == NULL)
                    int_next[1] = inst_disp;
                 else if (int_next[1]->index > inst_disp->index)
                    int_next[1] = inst_disp;
            }
        }
    }
    for (int k = 0; k< FU_INT_SIZE;k++){
        if (fuINT[k] == NULL) {
	        if (int_next[0] != NULL)
	        {
	            fuINT[k] = int_next[0] ;
	            int_next[0]  = NULL;
	            fuINT[k]->tom_execute_cycle = current_cycle;
	        } else if(int_next[1]  != NULL) {
                fuINT[k] = int_next[1];
                int_next[1]  = NULL;
	            fuINT[k]->tom_execute_cycle = current_cycle;
            }
        }
    }
    // if (fuINT[0]==NULL && fuINT[1]==NULL) {
    //     if (int_next[0] != NULL) {
    //         fuINT[0] = int_next[0];
    //         int_next[0]->tom_execute_cycle = current_cycle;
    //         if (int_next[1]!= NULL) {
    //             fuINT[1] = int_next[1];
    //             int_next[1]->tom_execute_cycle = current_cycle;
    //         }
    //     }
    // }
    // if (fuINT[0]==NULL && fuINT[1]!=NULL && int_next[0] != NULL) {
    //     fuINT[0] = int_next[0];
    //     int_next[0]->tom_execute_cycle = current_cycle;
    // }
    // if (fuINT[0]!=NULL && fuINT[1]==NULL && int_next[0] != NULL) {
    //     fuINT[1] = int_next[0];
    //     int_next[0]->tom_execute_cycle = current_cycle;
    // }
}

/* 
 * Description: 
 * 	Moves instruction(s) from the dispatch stage to the issue stage
 * Inputs:
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void dispatch_To_issue(int current_cycle) {

  /* ECE552: YOUR CODE GOES HERE */
    instruction_t* inst_disp;
    inst_disp = instr_queue[0];
    if(inst_disp != NULL){
        if (IS_UNCOND_CTRL(inst_disp->op)||IS_COND_CTRL(inst_disp->op)) {
            inst_disp->tom_issue_cycle = current_cycle;
            tmPopInsQueue();
        }
        if (USES_INT_FU(inst_disp->op)) {
            for (int j = 0; j < 4; ++j) {
                if (reservINT[j] == NULL) {
                    reservINT[j] = inst_disp;
                    inst_disp->tom_issue_cycle = current_cycle;
                    for (int k = 0;k<3;k++){
                        if (inst_disp->r_in[k] >=0 && inst_disp->r_in[k]< MD_TOTAL_REGS) {
                            inst_disp->Q[k] = map_table[inst_disp->r_in[k]];
                        }
                    }
                    if(WRITES_CDB(inst_disp->op)){
                        map_table[inst_disp->r_out[0]] = inst_disp;
                        map_table[inst_disp->r_out[1]] = inst_disp;
                    }
                    tmPopInsQueue();
                    break;
                }
            }
        }
        if (USES_FP_FU(inst_disp->op)) {
            for (int j = 0; j < 2; ++j) {
                if (reservFP[j] == NULL) {
                    reservFP[j] = inst_disp;
                    inst_disp->tom_issue_cycle = current_cycle;
                    for (int k = 0;k<3;k++){
                        if (inst_disp->r_in[k] >=0 && inst_disp->r_in[k]< MD_TOTAL_REGS) {
                            inst_disp->Q[k] = map_table[inst_disp->r_in[k]];
                        }
                    }
                    map_table[inst_disp->r_out[0]] = inst_disp;
                    map_table[inst_disp->r_out[1]] = inst_disp;
                    tmPopInsQueue();
                    break;
                }
            }
        }
    }
}

/* 
 * Description: 
 * 	Grabs an instruction from the instruction trace (if possible)
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * Returns:
 * 	None
 */
void fetch(instruction_trace_t *trace) {
    /* ECE552: YOUR CODE GOES HERE */
    if(fetch_index < sim_num_insn && instr_queue_size < INSTR_QUEUE_SIZE){
        instruction_trace_t* curretn_trace = trace;
        instruction_t* current_instruction = NULL;
        // Getting into the correct trace page and index
        int page_index = (fetch_index+1) / INSTR_TRACE_SIZE;
        int instruction_index = (fetch_index+1) % INSTR_TRACE_SIZE;
        int pass_index = 0;
        while(pass_index!=page_index){
            trace = trace->next;
            pass_index++;
        }
        // getting the correct trace
        current_instruction = &trace->table[instruction_index];
        // if the trace is a trap, move to the next trace
        while(IS_TRAP(current_instruction->op)){
            instruction_index++;
            if(instruction_index >= INSTR_TRACE_SIZE){
                instruction_index = instruction_index%INSTR_TRACE_SIZE;
                trace = trace->next;
                page_index++;
            }
            current_instruction = &trace->table[instruction_index];
        }
        // Initialize its cycle count
        current_instruction->tom_execute_cycle = -1;
        current_instruction->tom_cdb_cycle = -1;
        current_instruction->tom_dispatch_cycle = -1;
        current_instruction->tom_issue_cycle = -1;
        tmPushInsQueue(current_instruction);
        fetch_index = current_instruction->index;
    }
}

/* 
 * Description: 
 * 	Calls fetch and dispatches an instruction at the same cycle (if possible)
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * 	current_cycle: the cycle we are at
 * Returns:
 * 	None
 */
void fetch_To_dispatch(instruction_trace_t *trace, int current_cycle) {

    fetch(trace);
    /* ECE552: YOUR CODE GOES HERE */
    for (int i= 0; i < INSTR_QUEUE_SIZE; i++) {
        if (instr_queue[i]!= NULL){
            if (instr_queue[i]->tom_dispatch_cycle == -1) {
                instr_queue[i]->tom_dispatch_cycle = current_cycle;
            }
        }
    }
}

/* 
 * Description: 
 * 	Performs a cycle-by-cycle simulation of the 4-stage pipeline
 * Inputs:
 *      trace: instruction trace with all the instructions executed
 * Returns:
 * 	The total number of cycles it takes to execute the instructions.
 * Extra Notes:
 * 	sim_num_insn: the number of instructions in the trace
 */
counter_t runTomasulo(instruction_trace_t *trace) {
    //initialize instruction queue
    int i;
    for (i = 0; i < INSTR_QUEUE_SIZE; i++) {
        instr_queue[i] = NULL;
    }

    //initialize reservation stations
    for (i = 0; i < RESERV_INT_SIZE; i++) {
        reservINT[i] = NULL;
    }

    for (i = 0; i < RESERV_FP_SIZE; i++) {
        reservFP[i] = NULL;
    }

    //initialize functional units
    for (i = 0; i < FU_INT_SIZE; i++) {
        fuINT[i] = NULL;
    }

    for (i = 0; i < FU_FP_SIZE; i++) {
        fuFP[i] = NULL;
    }

    //initialize map_table to no producers
    int reg;
    for (reg = 0; reg < MD_TOTAL_REGS; reg++) {
        map_table[reg] = NULL;
    }

    int cycle = 1;
    while (true) {

        /* ECE552: YOUR CODE GOES HERE */
        CDB_To_retire(cycle);
        execute_To_CDB(cycle);
        issue_To_execute(cycle);
        fetch_To_dispatch(trace,cycle);
        dispatch_To_issue(cycle);
        cycle++;

        if (is_simulation_done(sim_num_insn))
            break;
    }
    print_all_instr (trace,20);
    return cycle;
}
