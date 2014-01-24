#ifndef __WINACPI_H_INCLUDE__
#define __WINACPI_H_INCLUDE__

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <prsht.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
#include <devioctl.h>
#include <target/acpi.h>
#include <host/winlayout.h>

#include "resource.h"

typedef struct tagACPIWNDDATA {
	HWND hWnd;
	HWND hwndToolbar;
	HWND hwndStatusbar;
	HWND hwndListView;
	HIMAGELIST himgTableTypes;
} ACPIWNDDATA, *LPACPIWNDDATA;

#define WinACPI_GetPtr(hWnd)			\
	(LPACPIWNDDATA)GetWindowLong((hWnd), 0)
#define WinACPI_SetPtr(hWnd, pTL)		\
	(LPACPIWNDDATA)SetWindowLong((hWnd), 0, (LONG)(pTL))

#define TIMER_REFRESH		1
#define TIMER_REFRESH_TIMEOUT	500	/* Milliseconds */

extern void acpi_emu_init(void);
extern void acpi_ospm_init(void);
extern void acpi_ospm_exit(void);

extern acpi_status_t acpi_emu_load_table(const char *file);

#endif /* __WINACPI_H_INCLUDE__ */
