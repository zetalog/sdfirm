#ifndef __LINKAGE_ARM64_H_INCLUDE__
#define __LINKAGE_ARM64_H_INCLUDE__

#define __ALIGN .align 2
#define __ALIGN_STR ".align 2"

#define ENDPROC(name) \
  .type name, %function; \
  END(name)

#endif /* __LINKAGE_ARM64_H_INCLUDE__ */
