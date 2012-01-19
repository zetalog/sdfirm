#ifndef __COS_SCOS_H_INCLUDE__
#define __COS_SCOS_H_INCLUDE__

#ifndef ARCH_HAVE_ICC
#define ARCH_HAVE_ICC			1
#else
#error "Multiple COS defined"
#endif

scs_err_t cos_vs_activate(void);
void cos_vs_deactivate(void);
void cos_vs_init(void);

#endif /* __COS_SCOS_H_INCLUDE__ */
