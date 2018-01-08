#include <target/kbuild.h>
#include <target/compiler.h>
#include <target/types.h>
#include <target/context.h>

/*
 * GCC 3.0, 3.1: general bad code generation.
 * GCC 3.2.0: incorrect function argument offset calculation.
 * GCC 3.2.x: miscompiles NEW_AUX_ENT in fs/binfmt_elf.c
 *            (http://gcc.gnu.org/PR8896) and incorrect structure
 *	      initialisation in fs/jffs2/erase.c
 */
#if (__GNUC__ == 3 && __GNUC_MINOR__ < 3)
#error Your compiler is too buggy; it is known to miscompile kernels.
#error    Known good compilers: 3.3
#endif

int main(void)
{
	DEFINE(SMC_SP_USR,	offsetof(struct smc_context, sp_usr));
	DEFINE(SMC_SP_SVC,	offsetof(struct smc_context, sp_svc));
	DEFINE(SMC_SP_MON,	offsetof(struct smc_context, sp_mon));
	DEFINE(SMC_SP_ABT,	offsetof(struct smc_context, sp_abt));
	DEFINE(SMC_SP_UND,	offsetof(struct smc_context, sp_und));
	DEFINE(SMC_SP_IRQ,	offsetof(struct smc_context, sp_irq));
	DEFINE(SMC_SP_FIQ,	offsetof(struct smc_context, sp_fiq));
	DEFINE(SMC_LR_USR,	offsetof(struct smc_context, lr_usr));
	DEFINE(SMC_LR_SVC,	offsetof(struct smc_context, lr_svc));
	DEFINE(SMC_LR_MON,	offsetof(struct smc_context, lr_mon));
	DEFINE(SMC_LR_ABT,	offsetof(struct smc_context, lr_abt));
	DEFINE(SMC_LR_UND,	offsetof(struct smc_context, lr_und));
	DEFINE(SMC_LR_IRQ,	offsetof(struct smc_context, lr_irq));
	DEFINE(SMC_LR_FIQ,	offsetof(struct smc_context, lr_fiq));
	DEFINE(SMC_SPSR_SVC,	offsetof(struct smc_context, spsr_svc));
	DEFINE(SMC_SPSR_MON,	offsetof(struct smc_context, spsr_mon));
	DEFINE(SMC_SPSR_ABT,	offsetof(struct smc_context, spsr_abt));
	DEFINE(SMC_SPSR_UND,	offsetof(struct smc_context, spsr_und));
	DEFINE(SMC_SPSR_IRQ,	offsetof(struct smc_context, spsr_irq));
	DEFINE(SMC_SPSR_FIQ,	offsetof(struct smc_context, spsr_fiq));
	return 0;
}
