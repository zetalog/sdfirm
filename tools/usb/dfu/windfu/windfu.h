#ifndef __WINDFU_H_INCLUDE__
#define __WINDFU_H_INCLUDE__

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
#include <host/winusb.h>
#include <host/usbdfu.h>
#include <host/winlayout.h>

#include "resource.h"

typedef struct tagDFUDEVICE {
	struct dfu_info info;
	char path[MAX_PATH];
	BOOL sync_info;
} DFUDEVICE, *LPDFUDEVICE;

typedef struct tagDFUWNDDATA {
	HWND hWnd;
	HWND hwndToolbar;
	HWND hwndStatusbar;
	HWND hwndListView;
	UINT nRefresh;
	HIMAGELIST himgDFUModes;
	UINT dfu_nr_devs;
	LPDFUDEVICE dfu_devs;
} DFUWNDDATA, *LPDFUWNDDATA;

typedef struct tagDFUPROGRAM DFUPROGRAM, *LPDFUPROGRAM;
struct tagDFUPROGRAM {
	LPDFUDEVICE info;
	CHAR firmware[MAX_PATH+1];
	HANDLE dev;
	HANDLE file;
	BOOL cancelled;
	BOOL completed;
	DWORD error;
	BYTE *buffer;
	BOOL read_eof;
	struct dfu_firm firm;
	int read_pos;
	int write_pos;
	int blkno;
	int type;
	int seq;
	BOOL seq_ended;
	BOOL seq_begun;
	UCHAR seq_error;
	HWND hwnd_prog;
	HWND hwnd_main;
	VOID (*abort)(LPDFUPROGRAM prog);
	BOOL (*program)(LPDFUPROGRAM prog, UINT *pos, UINT *range);
	HANDLE thread;
	DWORD tid;
};

#define WinDFU_GetPtr(hWnd)			\
	(LPDFUWNDDATA)GetWindowLong((hWnd), 0)
#define WinDFU_SetPtr(hWnd, pTL)		\
	(LPDFUWNDDATA)SetWindowLong((hWnd), 0, (LONG)(pTL))

#define DFU_ENUM_TIMER          1
#define DFU_CMND_TIMER          2
#define DFU_ENUM_TIMEOUT	5*1000	/* Milliseconds */
#define DFU_CMND_TIMEOUT	500	/* Milliseconds */

VOID DFUEnumDevices(LPDFUWNDDATA lpWD);
VOID DFUFreeDevices(LPDFUWNDDATA lpWD);
HANDLE DFUOpenDevice(LPDFUDEVICE dev);
VOID DFUCloseDevice(HANDLE hdev);
LPDFUDEVICE DFUDupDevice(LPDFUDEVICE dev);

void DFUStartEnum(HWND hWnd);
void DFUStopEnum(HWND hWnd);
void DFUSynchronizeSeq(LPDFUPROGRAM prog);

extern CHAR _dfu_license[];
extern HINSTANCE _hInstance;

#endif /* __WINDFU_H_INCLUDE__ */
