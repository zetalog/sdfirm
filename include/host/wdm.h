#ifndef __WDM_H_INCLUDE__
#define __WDM_H_INCLUDE__

#define IOCTL_INPUT				0x00	/* User -> Kernel */
#define IOCTL_OUTPUT				0x01	/* Kernel -> User */
#define IOCTL_CODE(code, dir, method)		\
	CTL_CODE(FILE_DEVICE_UNKNOWN,		\
		 ((code)<<1 | (dir)),		\
		 method, FILE_ANY_ACCESS)
#define MAX_IOCTL_CODE				127
#define IOCTL_GET_DIR(code)			((code) >> 2 & 0x01)
#define IOCTL_GET_CODE(code)			((code) >> 3 & 0xFFF)

#endif /* __WDM_H_INCLUDE__ */
