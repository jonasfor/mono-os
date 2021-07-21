#include "arch-sim.h"

struct s_bcp
{
	unsigned pid;
	struct s_regs regs;
};

void init_os();
void halt_os();
int create_process();
void terminate_process();
void schedule();
