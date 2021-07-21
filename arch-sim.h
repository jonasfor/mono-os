#ifndef ARCH_SIM_H_INCLUDED
#define ARCH_SIM_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

#define INSTALLED_MEMORY 512 /* 2^10 = 1024 bytes */
#define N_REGISTERS 16

/* LO and HI registers number */
#define LO_NUMBER 10
#define HI_NUMBER 11

#define SYS_MODE 0
#define USER_MODE 1

struct s_regs
{
	int32_t r[N_REGISTERS];
	uint32_t ir;
	uint32_t pc;
	uint32_t base;
	uint32_t limit;
	uint8_t mode; /* Dual mode operation: SYS_MODE and USER_MODE */
};

extern uint8_t MEMORY[INSTALLED_MEMORY];
extern struct s_regs REGS;
extern short STEP_BY_STEP;

extern void (*translate_addr_handler)(uint32_t* addr);
extern void (*clock_int_handler)();
extern void (*bad_instruction_handler)();
extern void (*div_by_zero_exception_handler)();
extern void (*system_call_handler)();

void init_arch();
void program_clock_int(unsigned nticks);
uint32_t execute_next_instruction();
int load_program(const char* filename);
void start_execution();
void stop_execution();
void print_regs();

#endif /* ARCH_SIM_H_INCLUDED */
