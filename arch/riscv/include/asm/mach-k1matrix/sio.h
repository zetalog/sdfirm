#ifndef __K1MATRIX_SIO_H_INCLUDE__
#define __K1MATRIX_SIO_H_INCLUDE__

#ifdef CONFIG_K1MATRIX_AST2600
#define ARCH_HAVE_SIO
#include <driver/ast2600_sio.h>

#define SIO_HW_ENTER		AST_SIO_ENTER
#define SIO_HW_EXIT		AST_SIO_EXIT
#endif

#endif /* __K1MATRIX_SIO_H_INCLUDE__ */
