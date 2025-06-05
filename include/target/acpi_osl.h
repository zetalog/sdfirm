#ifndef __ACPI_OSL_H_INCLUDE__
#define __ACPI_OSL_H_INCLUDE__

#ifdef CONFIG_SBI
#define acpi_os_debug_print	sbi_printf
#else
#define acpi_os_debug_print	printf
#endif

#ifdef CONFIG_HEAP
#define acpi_os_allocate(size)		((void *)heap_alloc(size))
#define acpi_os_allocate_zeroed(size)	((void *)heap_calloc(size))
#define acpi_os_free(mem)		heap_free((caddr_t)mem)
#else
#define acpi_os_allocate(size)		((void *)page_alloc(ALIGN(size, PAGE_SIZE) / PAGE_SIZE))
#define acpi_os_allocate_zeroed(size)	((void *)page_alloc_zeroed(ALIGN(size, PAGE_SIZE) / PAGE_SIZE))
#define acpi_os_free(mem)		page_free((caddr_t)mem)
#endif

#ifdef CONFIG_TASK
#define acpi_os_sleep(msecs)		msleep(msecs)
#else
#define acpi_os_sleep(msecs)			\
	do {					\
		typeof(msecs) __m = (msecs);	\
		while (__m > 200) {		\
			mdelay(200);		\
			__m -= 200;		\
		}				\
		mdelay((uint8_t)__m);		\
	} while (0)
#endif

#ifdef CONFIG_MMU
#define acpi_os_map_memory(where, length)	((void *)mmap(where, length))
#define acpi_os_unmap_memory(where, length)	munmap(where)
#else
#define acpi_os_map_memory(where, length)	ACPI_TO_POINTER(where)
#define acpi_os_unmap_memory(where, length)	do { } while (0)
#endif

#endif /* __ACPI_OSL_H_INCLUDE__ */
