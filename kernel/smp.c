#include <target/smp.h>

void smp_init(void)
{
	printf("SMP initializing %d\n", smp_processor_id());
}
