#include "mach.h"

caddr_t sim_stack;

int main(int argc, char **argv)
{
	uint8_t stack_top = 0;

	sim_stack = (caddr_t)&stack_top;

	sim_init();
	sim_run();	
}
