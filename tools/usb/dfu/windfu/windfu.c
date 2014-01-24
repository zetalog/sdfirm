/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2011
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)windfu.c: device firmware upgrade windows user interface
 * $Id: windfu.c,v 1.87 2011-10-17 01:40:34 zhenglv Exp $
 */
#include <initguid.h>
#include <host/winlayout.h>
#include "windfu.h"

TCHAR _szTitle[MAX_LOADSTRING];
TCHAR _szWindowClass[MAX_LOADSTRING];
TCHAR _szFirmware[MAX_PATH+1];
USHORT _usVendor = 0;
USHORT _usProduct = 0;
USHORT _usDevice = 0;
HINSTANCE _hInstance;

INT _nIdMappings[MAX_LAYOUT_IDS] = {
	ID_MAINFRAME,
	IDS_POSITION,
	IDS_MENUFLAGS,
	IDS_PLACEMENT,
	ID_VIEW_TOOLBAR,
	ID_VIEW_STATUSBAR,
	ID_STATUS,
	ID_REBAR,
	ID_TOOLBAR,
	IDB_TOOLBAR4BIT,
	IDS_READY,
};

#define IDX_EXITAPP		0
#define IDX_DFUSUFFIX		1
#define IDX_DFUDETACH		2
#define IDX_DFUABORT		3
#define IDX_DFUDNLOAD		4
#define IDX_DFUUPLOAD		5
#define IDX_DFUCYCLE		6
#define TOOLBARNUM		7

WINTOOLBARITEM _ToolbarItems[TOOLBARNUM] = {
	{ FALSE, IDX_EXITAPP, ID_APP_EXIT },
	{ FALSE, IDX_DFUSUFFIX, ID_DFU_SUFFIX },
	{ FALSE, IDX_DFUDETACH, ID_DFU_DETACH },
	{ FALSE, IDX_DFUABORT, ID_DFU_ABORT },
	{ FALSE, IDX_DFUDNLOAD, ID_DFU_DNLOAD },
	{ FALSE, IDX_DFUUPLOAD, ID_DFU_UPLOAD },
	{ FALSE, IDX_DFUCYCLE, ID_DFU_CYCLE },
};

static LRESULT WINAPI DlgWizardBrowseProc(HWND hWnd, UINT uMsg,
					  WPARAM wParam, LPARAM lParam);
static LRESULT WINAPI DlgWizardProgramProc(HWND hWnd, UINT uMsg,
					   WPARAM wParam, LPARAM lParam);
static LRESULT WINAPI DlgWizardSuffixProc(HWND hWnd, UINT uMsg,
					  WPARAM wParam, LPARAM lParam);
static LRESULT WINAPI DFUSuffixDlgProc(HWND hDlg, UINT uMsg,
				       WPARAM wParam, LPARAM lParam);
static LRESULT WINAPI About_DlgProc(HWND hDlg, UINT uMsg,
				    WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg,
				    WPARAM wParam, LPARAM lParam);
static DWORD WINAPI DFUShowProgressThread(LPWINWIZARDINFO lpwi);

LPCSTR dfu_seq_name(int seq)
{
#define DFU_SEQ_STRING(str)	\
	case DFU_SEQ_##str:	\
		return #str;
	switch (seq) {
	DFU_SEQ_STRING(IDLE);
	DFU_SEQ_STRING(DNLOAD);
	DFU_SEQ_STRING(UPLOAD);
	DFU_SEQ_STRING(DETACH);
	DFU_SEQ_STRING(ABORT);
	}
#undef DFU_SEQ_STRING
	return NULL;
}

LPCSTR dfu_state_name(UCHAR state)
{
#define DFU_STATE_STRING(str)	\
	case DFU_STATE_##str:	\
		return #str;
	switch (state) {
	DFU_STATE_STRING(appIDLE);
	DFU_STATE_STRING(appDETACH);
	DFU_STATE_STRING(dfuIDLE);
	DFU_STATE_STRING(dfuDNLOAD_SYNC);
	DFU_STATE_STRING(dfuDNBUSY);
	DFU_STATE_STRING(dfuDNLOAD_IDLE);
	DFU_STATE_STRING(dfuMANIFEST_SYNC);
	DFU_STATE_STRING(dfuMANIFEST);
	DFU_STATE_STRING(dfuMANIFEST_WAIT_RESET);
	DFU_STATE_STRING(dfuUPLOAD_IDLE);
	DFU_STATE_STRING(dfuERROR);
	}
#undef DFU_STATE_STRING
	return NULL;
}

LPCSTR dfu_status_name(UCHAR status)
{
#define DFU_STATUS_STRING(str)	\
	case DFU_STATUS_##str:	\
		return #str;
	switch (status) {
	DFU_STATUS_STRING(OK);
	DFU_STATUS_STRING(errTARGET);
	DFU_STATUS_STRING(errFILE);
	DFU_STATUS_STRING(errWRITE);
	DFU_STATUS_STRING(errERASE);
	DFU_STATUS_STRING(errCHECK_ERASED);
	DFU_STATUS_STRING(errPROG);
	DFU_STATUS_STRING(errVERIFY);
	DFU_STATUS_STRING(errADDRESS);
	DFU_STATUS_STRING(errNOTDONE);
	DFU_STATUS_STRING(errFIRMWARE);
	DFU_STATUS_STRING(errVENDOR);
	DFU_STATUS_STRING(errUSBR);
	DFU_STATUS_STRING(errPOR);
	DFU_STATUS_STRING(errUNKNOWN);
	DFU_STATUS_STRING(errSTALLEDPKT);
	}
	return NULL;
}

VOID DFUInitDevices(LPDFUWNDDATA lpWD)
{
	lpWD->dfu_devs = NULL;
	lpWD->dfu_nr_devs = 0;
}

VOID DFUFreeDevices(LPDFUWNDDATA lpWD)
{
	if (lpWD->dfu_devs) {
		free(lpWD->dfu_devs);
	}
	DFUInitDevices(lpWD);
}

void DFULoadFirmware(struct dfu_firm *firm)
{
	firm->idVendor = _usVendor;
	firm->idProduct = _usProduct;
	firm->bcdDevice = _usDevice;
}

void DFUSaveFirmware(struct dfu_firm *firm)
{
	_usVendor = firm->idVendor;
	_usProduct = firm->idProduct;
	_usDevice = firm->bcdDevice;
}

VOID DFUAppendDevice(LPDFUWNDDATA lpWD, LPDFUDEVICE dev)
{
	LVITEM lvi = { 0 };
	int nIndex;
	HWND hwndList = lpWD->hwndListView;
	CHAR tmpstring[10];
	CHAR *name = dev->info.ucInterfaceName;

	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
	lvi.iImage = dev->info.bInterfaceProtocol;
	lvi.lParam = (LPARAM)dev;
	lvi.pszText = name;
	lvi.cchTextMax = _tcslen(name) ? _tcslen(name) : 0;
	nIndex = ListView_InsertItem(hwndList, &lvi);

	if (nIndex != -1) {
		sprintf(tmpstring, "%d", dev->info.wTransferSize);
		ListView_SetItemText(hwndList, nIndex, 1, tmpstring);
		ListView_SetItemText(hwndList, nIndex, 2,
				     (LPSTR)dfu_state_name(dev->info.bState));
		ListView_SetItemText(hwndList, nIndex, 3,
				     (LPSTR)dfu_status_name(dev->info.bStatus));
		sprintf(tmpstring, "0x%04X", dev->info.idVendor);
		ListView_SetItemText(hwndList, nIndex, 4, tmpstring);
		sprintf(tmpstring, "0x%04X", dev->info.idProduct);
		ListView_SetItemText(hwndList, nIndex, 5, tmpstring);
	}
}

VOID DFURemoveDevice(LPDFUWNDDATA lpWD, LPDFUDEVICE dev)
{
	LV_FINDINFO lvfi;
	int nIndex;
	HWND hwndList = lpWD->hwndListView;

	lvfi.flags = LVFI_PARAM;
	lvfi.lParam = (LPARAM)dev;
	nIndex = ListView_FindItem(hwndList, -1, &lvfi);
	if (nIndex != -1) {
		ListView_DeleteItem(hwndList, nIndex);
	}
}

LPDFUDEVICE DFUGetSelected(LPDFUWNDDATA lpWD)
{
	int nIndex;
	HWND hwndList = lpWD->hwndListView;
	LVITEM lvi = { 0 };

	nIndex = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
	if (nIndex == -1)
		return NULL;

	lvi.iItem = nIndex;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;
	if (!ListView_GetItem(hwndList, &lvi))
		return NULL;

	return (LPDFUDEVICE)lvi.lParam;
}

LPDFUDEVICE DFUDupDevice(LPDFUDEVICE dev)
{
	LPDFUDEVICE ndev = malloc(sizeof (DFUDEVICE));
	if (ndev)
		memcpy(ndev, dev, sizeof (DFUDEVICE));
	return ndev;
}

VOID DFUUpdateDevice(LPDFUWNDDATA lpWD, LPDFUDEVICE odev, LPDFUDEVICE ndev)
{
	LV_FINDINFO lvfi;
	int nIndex;
	HWND hwndList = lpWD->hwndListView;
	LVITEM lvi = { 0 };
	CHAR tmpstring[10];
	CHAR *name = ndev->info.ucInterfaceName;

	lvfi.flags = LVFI_PARAM;
	lvfi.lParam = (LPARAM)odev;
	nIndex = ListView_FindItem(hwndList, -1, &lvfi);
	if (nIndex != -1) {
		lvi.iItem = nIndex;
		lvi.iSubItem = 0;
		lvi.mask = LVIF_PARAM | LVIF_IMAGE | LVIF_TEXT;
		lvi.iImage = ndev->info.bInterfaceProtocol;
		lvi.lParam = (LPARAM)ndev;
		lvi.pszText = name;
		lvi.cchTextMax = _tcslen(name) ? _tcslen(name) : 0;
		if (ListView_SetItem(hwndList, &lvi)) {
			sprintf(tmpstring, "%d", ndev->info.wTransferSize);
			ListView_SetItemText(hwndList, nIndex, 1, tmpstring);
			ListView_SetItemText(hwndList, nIndex, 2,
					     (LPSTR)dfu_state_name(ndev->info.bState));
			ListView_SetItemText(hwndList, nIndex, 3,
					     (LPSTR)dfu_status_name(ndev->info.bStatus));
			sprintf(tmpstring, "0x%04X", ndev->info.idVendor);
			ListView_SetItemText(hwndList, nIndex, 4, tmpstring);
			sprintf(tmpstring, "0x%04X", ndev->info.idProduct);
			ListView_SetItemText(hwndList, nIndex, 5, tmpstring);
		}
	}
}

VOID DFUUpdateDevices(LPDFUWNDDATA lpWD, LPDFUDEVICE devs, UINT nr_devs)
{
	UINT i, j;
	BOOL found;

	for (i = 0; i < nr_devs; i++) {
		found = FALSE;
		for (j = 0; j < lpWD->dfu_nr_devs; j++) {
			if (strcmp(devs[i].path, lpWD->dfu_devs[j].path) == 0) {
				DFUUpdateDevice(lpWD, &lpWD->dfu_devs[j], &devs[i]);
				found = TRUE;
				break;
			}
		}
		if (!found) {
			DFUAppendDevice(lpWD, &devs[i]);
		}
	}
	for (j = 0; j < lpWD->dfu_nr_devs; j++) {
		found = FALSE;
		for (i = 0; i < nr_devs; i++) {
			if (strcmp(devs[i].path, lpWD->dfu_devs[j].path) == 0) {
				/* DFUUpdateDevice(lpWD, &lpWD->dfu_devs[j], &devs[i]); */
				found = TRUE;
				break;
			}
		}
		if (!found) {
			DFURemoveDevice(lpWD, &lpWD->dfu_devs[j]);
		}
	}
	DFUFreeDevices(lpWD);
	lpWD->dfu_nr_devs = nr_devs;
	lpWD->dfu_devs = devs;
}

VOID DFUControlDevice(LPDFUWNDDATA lpWD, ULONG code)
{
	HANDLE hdev = INVALID_HANDLE_VALUE;
	LPDFUDEVICE dev = NULL;
	DWORD bytes;
	BOOL res = FALSE;

	DFUStopEnum(lpWD->hWnd);
	dev = DFUGetSelected(lpWD);
	if (!dev) goto end;
	hdev = CreateFile(dev->path,
			  GENERIC_READ | GENERIC_WRITE,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  NULL,
			  OPEN_EXISTING,
			  0,
			  NULL);
	if (hdev == INVALID_HANDLE_VALUE) goto end;
	res = DeviceIoControl(hdev, code,
			      NULL, 0, NULL, 0,
			      &bytes, NULL);
end:
	if (hdev != INVALID_HANDLE_VALUE) CloseHandle(hdev);
	DFUStartEnum(lpWD->hWnd);
}

VOID DFUCycleDevice(LPDFUWNDDATA lpWD)
{
	DFUControlDevice(lpWD, IOCTL_USBD_CYCLEIF);
}

VOID DFUDetachDevice(LPDFUWNDDATA lpWD)
{
	DFUControlDevice(lpWD, IOCTL_USB_DFU_DETACH);
}

VOID DFUAbortDevice(LPDFUWNDDATA lpWD)
{
	DFUControlDevice(lpWD, IOCTL_USB_DFU_ABORT);
}

LPCSTR DFUIdentifierToString(USHORT id, UCHAR *str)
{
	if (!str) return NULL;
	return itoa(id, str, 16);
}

static BYTE hex2byte(const char *hex)
{
	BYTE Hi;		/* holds high byte */
	BYTE Lo;		/* holds low byte */
	BYTE Result;	/* holds result */
	
	/* Get the value of the first byte to Hi */
	Hi = hex[0];
	if ('0' <= Hi && Hi <= '9') {
		Hi -= '0';
	} else if ('a' <= Hi && Hi <= 'f') {
		Hi -= ('a'-10);
	} else if ('A' <= Hi && Hi <= 'F') {
		Hi -= ('A'-10);
	}
	
	/* Get the value of the second byte to Lo */
 	Lo = hex[1];
	if ('0' <= Lo && Lo <= '9') {
		Lo -= '0';
	} else if ('a' <= Lo && Lo <= 'f') {
		Lo -= ('a'-10);
	} else if ('A' <= Lo && Lo <= 'F') {
		Lo -= ('A'-10);
	}
	Result = Lo + (16 * Hi);
	return (Result);
}

static WORD hex2word(const char *hex)
{
	WORD Lo, Hi;
	char newhex[5];
	int fill_len = 4-strlen(hex);

	memset(newhex, '0', fill_len);
	memcpy(newhex+fill_len, hex, strlen(hex));
	newhex[4] = '\0';

	Hi = hex2byte(newhex);
	Lo = hex2byte(newhex+2);
	return (Hi<<8) | Lo; 
}

USHORT DFUStringToIdentifier(const CHAR *str)
{
	if (!str) return 0;
	return hex2word(str);
}

static VOID DFUSuffixLoadAttributes(HWND hDlg, struct dfu_firm *firm)
{
	CHAR tmp[5] = {0};

	SetDlgItemText(hDlg, IDC_DEVICE,
		       DFUIdentifierToString(firm->bcdDevice, tmp));
	SetDlgItemText(hDlg, IDC_VENDOR,
		       DFUIdentifierToString(firm->idVendor, tmp));
	SetDlgItemText(hDlg, IDC_PRODUCT,
		       DFUIdentifierToString(firm->idProduct, tmp));
}

static VOID DFUSuffixSaveAttributes(HWND hDlg, struct dfu_firm *firm)
{
	CHAR tmp[5];

	GetDlgItemText(hDlg, IDC_DEVICE, tmp, sizeof(tmp));
	firm->bcdDevice =  DFUStringToIdentifier(tmp);
	GetDlgItemText(hDlg, IDC_VENDOR, tmp, sizeof(tmp));
	firm->idVendor =  DFUStringToIdentifier(tmp);
	GetDlgItemText(hDlg, IDC_PRODUCT, tmp, sizeof(tmp));
	firm->idProduct =  DFUStringToIdentifier(tmp);
}

static VOID DFUSuffixInitDialog(HWND hDlg)
{
	SendDlgItemMessage(hDlg, IDC_VENDOR, EM_SETLIMITTEXT, (WPARAM)4, 0);
	SendDlgItemMessage(hDlg, IDC_PRODUCT, EM_SETLIMITTEXT, (WPARAM)4, 0);
	SendDlgItemMessage(hDlg, IDC_DEVICE, EM_SETLIMITTEXT, (WPARAM)4, 0);
}

static VOID DFUValidateInput(HWND hDlg, int nIDDlgItem)
{
	HWND hEdit = GetDlgItem(hDlg, nIDDlgItem);
	size_t i, j, curchar;
	CHAR tmp[2][5] = {0};
	POINT curpos = {0};
	BOOL update = FALSE;
	ULONG coord;

	switch (nIDDlgItem) {
	case IDC_DEVICE:
	case IDC_VENDOR:
	case IDC_PRODUCT:
		GetCaretPos(&curpos);
		coord = MAKELPARAM(curpos.x, curpos.y);
		curchar = SendMessage(hEdit, EM_CHARFROMPOS, 0, coord);
		GetWindowText(hEdit, tmp[0], sizeof(tmp[0]));
		for (i = 0, j = 0; i < strlen(tmp[0]); i++) {
			if (i == curchar)
				curchar = j;
			if (isxdigit(tmp[0][i]))
				tmp[1][j++] = tmp[0][i];
			else
				curchar--;
		}
		tmp[1][j] = '\0';
		if (strcmp(tmp[0], tmp[1])) {
			coord = SendMessage(hEdit, EM_POSFROMCHAR,
					    curchar, 0);
			SetCaretPos(LOWORD(coord), HIWORD(coord));
			SetWindowText(hEdit, tmp[1]);
		} else {
			SetCaretPos(curpos.x, curpos.y);
		}
		break;
	}
}

void DFUSyncBrowseButtons(HWND hwndWizard, HWND hwndPage,
			  LPDFUPROGRAM prog, BOOL update)
{
	if (update)
		SetWindowText(GetDlgItem(hwndPage, IDC_FIRMWARE), prog->firmware);
	if (strlen(prog->firmware)) {
		SendMessage(hwndWizard, PSM_SETWIZBUTTONS,
			    0, PSWIZB_BACK | PSWIZB_NEXT);
	} else {
		SendMessage(hwndWizard, PSM_SETWIZBUTTONS,
			    0, PSWIZB_BACK);
	}
}

static LRESULT WINAPI DlgWizardBrowseProc(HWND hWnd, UINT uMsg,
					  WPARAM wParam, LPARAM lParam)
{
	BOOL result = FALSE;
	LPWINWIZARDINFO lpwi;
	LPDFUPROGRAM prog;
	LPNMHDR pnmh;
	int wmId, wmEvent;
	char szSuffix[] = "*.*";
	char szName[] = "All Files (*.*)";
	char *pSuffix[1] = { szSuffix };
	char *pName[1] = { szName };

	if (uMsg == WM_INITDIALOG) {
		PROPSHEETPAGE *ppspMsgRec;
		ppspMsgRec = (PROPSHEETPAGE *)lParam;
		lpwi = (LPWINWIZARDINFO)ppspMsgRec->lParam;
		SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)lpwi);
	} else {
		lpwi = (LPWINWIZARDINFO)GetWindowLong(hWnd, GWL_USERDATA);
	}
	if (lpwi)
		prog = (LPDFUPROGRAM)lpwi->lParam;
	else
		prog = NULL;
	
	switch (uMsg) {
	case WM_INITDIALOG:
		result = TRUE;
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDC_BROWSE:
			if (DlgBrowseFile(lpwi->hwndWizard,
					  prog->firmware, MAX_PATH,
					  pName, pSuffix, 1,
					  prog->type == DFU_UPLOAD)) {
				DFUSyncBrowseButtons(lpwi->hwndWizard, hWnd, prog, TRUE);
			}
			break;
		case IDC_FIRMWARE:
			if (wmEvent == EN_CHANGE) {
				GetWindowText(GetDlgItem(hWnd, IDC_FIRMWARE),
					      prog->firmware, MAX_PATH+1);
				DFUSyncBrowseButtons(lpwi->hwndWizard, hWnd, prog, FALSE);
			}
			break;
		}
		break;
	case WM_NOTIFY:
		pnmh = (LPNMHDR)lParam;
		switch (pnmh->code) {
		case PSN_SETACTIVE:
			SendMessage(lpwi->hwndWizard, PSM_SETWIZBUTTONS,
				    0, PSWIZB_BACK);
			SendDlgItemMessage(hWnd, IDC_WIZBITMAP, STM_SETIMAGE,
					   IMAGE_BITMAP, (LPARAM) lpwi->hBitmap);
			DFUSyncBrowseButtons(lpwi->hwndWizard, hWnd, prog, TRUE);
			return 0;
		case PSN_KILLACTIVE:
			result = FALSE;
			SetWindowLong(hWnd, DWL_MSGRESULT, result);
			break;
		case PSN_QUERYCANCEL:
			lpwi->nResult = FALSE;
			SetWindowLong(hWnd, DWL_MSGRESULT, lpwi->nResult);
			return FALSE;
		}
		break;
	}
	
	return result;
}

#define WM_PROGRAM	WM_USER+1

static DWORD WINAPI DFUShowProgressThread(LPWINWIZARDINFO lpwi)
{
	LPDFUPROGRAM prog;
	UINT pos = 0, range;

	prog = (LPDFUPROGRAM)lpwi->lParam;
	assert(prog && prog->program && prog->abort);
	do {
		if (prog->cancelled) {
			prog->error = ERROR_CANCELLED;
			prog->abort(prog);
			PostMessage(prog->hwnd_prog, WM_PROGRAM, 0, 0);
			break;
		}
		if (prog->program(prog, &pos, &range) == FALSE) {
			PostMessage(prog->hwnd_prog, WM_PROGRAM, 0, 0);
			break;
		}
		PostMessage(prog->hwnd_prog, WM_PROGRAM, pos, range);
	} while (pos < range);
	return 0L;
}

BOOL DFUSeqHasSucceeded(LPDFUPROGRAM prog)
{
	return prog->seq_ended && (prog->seq_error == DFU_STATUS_OK);
}

void DFUSyncPorgramButtons(HWND hwndWizard, HWND hwndPage, LPDFUPROGRAM prog)
{
	if (prog->thread != NULL || !prog->seq_ended) {
		SendMessage(hwndWizard, PSM_SETWIZBUTTONS, 0, 0);
		EnableWindow(GetDlgItem(hwndPage, IDC_START), FALSE);
		if (prog->cancelled) {
			EnableWindow(GetDlgItem(hwndPage, IDC_STOP), FALSE);
		} else {
			EnableWindow(GetDlgItem(hwndPage, IDC_STOP), TRUE);
		}
	} else {
		EnableWindow(GetDlgItem(hwndPage, IDC_START), TRUE);
		EnableWindow(GetDlgItem(hwndPage, IDC_STOP), FALSE);
		if (prog->completed && DFUSeqHasSucceeded(prog)) {
			SendMessage(hwndWizard, PSM_SETWIZBUTTONS,
				    0, PSWIZB_BACK | PSWIZB_NEXT);
		} else {
			SendMessage(hwndWizard, PSM_SETWIZBUTTONS,
				    0, PSWIZB_BACK);
		}
	}
}

VOID DFUStopProgram(LPDFUPROGRAM prog)
{
	if (prog->file != INVALID_HANDLE_VALUE) {
		CloseHandle(prog->file);
		prog->file = INVALID_HANDLE_VALUE;
	}
	if (prog->dev != INVALID_HANDLE_VALUE) {
		DFUCloseDevice(prog->dev);
		prog->dev = INVALID_HANDLE_VALUE;
	}
}

BOOL DFUStartProgram(LPDFUPROGRAM prog)
{
	DWORD bytes;
	BOOL res;

	assert(prog->dev == INVALID_HANDLE_VALUE);
	assert(prog->file == INVALID_HANDLE_VALUE);
	prog->dev = DFUOpenDevice(prog->info);
	if (!prog->dev)
		return FALSE;
	if (prog->type == DFU_DNLOAD) {
		res = DeviceIoControl(prog->dev, IOCTL_USB_DFU_DNLOAD,
				      &prog->firm, sizeof(struct dfu_firm),
				      NULL, 0, &bytes, NULL);
	} else {
		res = DeviceIoControl(prog->dev, IOCTL_USB_DFU_UPLOAD,
				      NULL, 0, NULL, 0, &bytes, NULL);
	}
	if (res) {
		prog->seq_begun = TRUE;
		DFUSynchronizeSeq(prog);
	}
	return res;
}

void DFUStartSeqSync(LPDFUPROGRAM prog)
{
	SetTimer(prog->hwnd_prog, DFU_ENUM_TIMER, DFU_ENUM_TIMEOUT,
		 (TIMERPROC)DlgWizardProgramProc);
}

void DFUStopSeqSync(LPDFUPROGRAM prog)
{
	KillTimer(prog->hwnd_prog, DFU_ENUM_TIMER);
}

void DFUSynchronizeSeq(LPDFUPROGRAM prog)
{
	HANDLE hdev = INVALID_HANDLE_VALUE;
	LPDFUDEVICE dev = NULL;
	DWORD bytes;
	BOOL res = FALSE;
	struct dfu_seq seq;

	dev = prog->info;
	if (!dev) goto end;
	hdev = CreateFile(dev->path,
			  GENERIC_READ | GENERIC_WRITE,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  NULL,
			  OPEN_EXISTING,
			  0,
			  NULL);
	if (hdev == INVALID_HANDLE_VALUE) goto end;
	res = DeviceIoControl(hdev, IOCTL_USB_DFU_LASTSEQ,
			      NULL, 0, &seq, sizeof (struct dfu_seq),
			      &bytes, NULL);
	if (!res) goto end;
	if (prog->seq_begun) {
		/* The last sequence information is useless if the current
		 * sequence has not begun.
		 */
		prog->seq = seq.seq;
		prog->seq_ended = seq.seq_ended;
		prog->seq_error = seq.seq_error;
	}
end:
	if (prog->error != ERROR_SUCCESS && !prog->seq_ended) {
		/* The sequence is aborted by the driver, possible causes
		 * may include the device unplugging.
		 * As UI logic is depending on this function's result,
		 * here we must be careful to take the responsibility of
		 * a correct UI logic manifestation.
		 */
		prog->seq_ended = seq.seq_ended;
		prog->seq_error = DFU_STATUS_errNOTDONE;
	}
	SetDlgItemText(prog->hwnd_prog, IDC_SEQUENCE,
		       dfu_seq_name(prog->seq));
	if (prog->seq_ended) {
		SetDlgItemText(prog->hwnd_prog, IDC_ERROR,
			       dfu_status_name(prog->seq_error));
	}
	if (hdev != INVALID_HANDLE_VALUE) CloseHandle(hdev);
}

static LRESULT WINAPI DlgWizardProgramProc(HWND hWnd, UINT uMsg,
					   WPARAM wParam, LPARAM lParam)
{
	BOOL result = FALSE;
	LPWINWIZARDINFO lpwi;
	LPDFUPROGRAM prog;
	LPNMHDR pnmh;
	int wmId, wmEvent;
	UINT pos, range;

	if (uMsg == WM_INITDIALOG) {
		PROPSHEETPAGE *ppspMsgRec;
		ppspMsgRec = (PROPSHEETPAGE *)lParam;
		lpwi = (LPWINWIZARDINFO)ppspMsgRec->lParam;
		SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)lpwi);
	} else {
		lpwi = (LPWINWIZARDINFO)GetWindowLong(hWnd, GWL_USERDATA);
	}
	if (lpwi)
		prog = (LPDFUPROGRAM)lpwi->lParam;
	else
		prog = NULL;
	
	switch (uMsg) {
	case WM_PROGRAM:
		pos = wParam;
		range = lParam;
		SendMessage(GetDlgItem(hWnd, IDC_PROGRAM), PBM_SETRANGE,
				       0, MAKELPARAM(0, range));
		SendMessage(GetDlgItem(hWnd, IDC_PROGRAM), PBM_SETPOS, pos, 0);
		if (pos == range) {
			if (range == 0) {
				DlgDisplayError(lpwi->hwndWizard, IDS_ERROR, prog->error);
			}
			if (prog->thread != NULL) {
				CloseHandle(prog->thread);
				prog->thread = NULL;
			}
			DFUStopProgram(prog);
			DFUSyncPorgramButtons(lpwi->hwndWizard, hWnd, prog);
		}
		break;
	case WM_INITDIALOG:
		result = TRUE;
		break;
	case WM_TIMER:
		switch (wParam) {
		case DFU_ENUM_TIMER:
			DFUSynchronizeSeq(prog);
			DFUSyncPorgramButtons(lpwi->hwndWizard, hWnd, prog);
			break;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDC_START:
			assert(prog->program);
			if (IDOK == MessageIDBox(lpwi->hwndWizard,
						 IDS_INFO_START_PROGRAM,
						 IDS_INFORMATION,
						 MB_OKCANCEL | MB_ICONQUESTION)) {
				if (DFUStartProgram(prog)) {
					assert(prog->thread == NULL);
					prog->completed = FALSE;
					prog->cancelled = FALSE;
					prog->error = ERROR_SUCCESS;
					prog->thread = CreateThread(0, 0,
								    (LPTHREAD_START_ROUTINE)DFUShowProgressThread,
								    lpwi, 0, &(prog->tid));
					DFUSyncPorgramButtons(lpwi->hwndWizard, hWnd, prog);
				} else {
					MessageIDBox(lpwi->hwndWizard,
						     IDS_ERROR_DEVICE_CONTROL,
						     IDS_ERROR,
						     MB_OK | MB_ICONERROR);
					DFUStopProgram(prog);
				}
			}
			break;
		case IDC_STOP:
			if (IDOK == MessageIDBox(lpwi->hwndWizard,
						 IDS_INFO_STOP_PROGRAM,
						 IDS_WARNING,
						 MB_OKCANCEL | MB_ICONWARNING)) {
				prog->cancelled = TRUE;
				DFUSyncPorgramButtons(lpwi->hwndWizard, hWnd, prog);
			}
			break;
		}
		break;
	case WM_NOTIFY:
		pnmh = (LPNMHDR)lParam;
		switch (pnmh->code) {
		case PSN_SETACTIVE:
			prog->hwnd_prog = hWnd;
			DFUStartSeqSync(prog);
			SendMessage(lpwi->hwndWizard, PSM_SETWIZBUTTONS,
				    0, PSWIZB_BACK);
			SetWindowText(GetDlgItem(hWnd, IDC_DEVICE),
				      prog->info->info.ucInterfaceName);
			SetWindowText(GetDlgItem(hWnd, IDC_FIRMWARE),
				      prog->firmware);
			SendDlgItemMessage(hWnd, IDC_WIZBITMAP, STM_SETIMAGE,
					   IMAGE_BITMAP, (LPARAM) lpwi->hBitmap);
			DFUSyncPorgramButtons(lpwi->hwndWizard, hWnd, prog);
			return 0;
		case PSN_KILLACTIVE:
			result = FALSE;
			SetWindowLong(hWnd, DWL_MSGRESULT, result);
			DFUStopSeqSync(prog);
			break;
		case PSN_QUERYCANCEL:
			if (prog->thread != NULL || !prog->seq_ended) {
				MessageIDBox(lpwi->hwndWizard,
					     IDS_INFO_START_PROGRAM,
					     IDS_ERROR,
					     MB_OK | MB_ICONERROR);
				result = TRUE;
			} else {
				result = FALSE;
			}
			SetWindowLong(hWnd, DWL_MSGRESULT, result);
			break;
		}
		break;
	}
	
	return result;
}

void DFUSyncSuffixButtons(HWND hwndWizard, HWND hwndPage, LPDFUPROGRAM prog)
{
	if (Edit_GetTextLength(GetDlgItem(hwndPage, IDC_DEVICE)) == 0 ||
	    Edit_GetTextLength(GetDlgItem(hwndPage, IDC_PRODUCT)) == 0 ||
	    Edit_GetTextLength(GetDlgItem(hwndPage, IDC_VENDOR)) == 0) {
		SendMessage(hwndWizard, PSM_SETWIZBUTTONS,
			    0, PSWIZB_BACK);
	} else {
		SendMessage(hwndWizard, PSM_SETWIZBUTTONS,
			    0, PSWIZB_BACK | PSWIZB_NEXT);
	}
}

static LRESULT WINAPI DlgWizardSuffixProc(HWND hWnd, UINT uMsg,
					  WPARAM wParam, LPARAM lParam)
{
	BOOL result = FALSE;
	LPWINWIZARDINFO lpwi;
	LPDFUPROGRAM prog;
	LPNMHDR pnmh;
	int wmId, wmEvent;

	if (uMsg == WM_INITDIALOG) {
		PROPSHEETPAGE *ppspMsgRec;
		ppspMsgRec = (PROPSHEETPAGE *)lParam;
		lpwi = (LPWINWIZARDINFO)ppspMsgRec->lParam;
		SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)lpwi);
	} else {
		lpwi = (LPWINWIZARDINFO)GetWindowLong(hWnd, GWL_USERDATA);
	}
	if (lpwi)
		prog = (LPDFUPROGRAM)lpwi->lParam;
	else
		prog = NULL;
	
	switch (uMsg) {
	case WM_INITDIALOG:
		result = TRUE;
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDC_DEVICE:
		case IDC_VENDOR:
		case IDC_PRODUCT:
			if (wmEvent == EN_CHANGE) {
				DFUValidateInput(hWnd, wmId);
				DFUSyncSuffixButtons(lpwi->hwndWizard, hWnd, prog);
			}
			break;
		}
		break;
	case WM_NOTIFY:
		pnmh = (LPNMHDR)lParam;
		switch (pnmh->code) {
		case PSN_SETACTIVE:
			SendDlgItemMessage(hWnd, IDC_WIZBITMAP, STM_SETIMAGE,
					   IMAGE_BITMAP, (LPARAM) lpwi->hBitmap);
			DFUSuffixLoadAttributes(hWnd, &prog->firm);
			DFUSuffixInitDialog(hWnd);
			DFUSyncSuffixButtons(lpwi->hwndWizard, hWnd, prog);
			return 0;
		case PSN_KILLACTIVE:
			DFUSuffixSaveAttributes(hWnd, &prog->firm);
			result = FALSE;
			SetWindowLong(hWnd, DWL_MSGRESULT, result);
			break;
		case PSN_QUERYCANCEL:
			lpwi->nResult = FALSE;
			SetWindowLong(hWnd, DWL_MSGRESULT, lpwi->nResult);
			break;
		}
		break;
	}
	
	return result;
}

#define DFU_DNLOAD_PAGES	3
#define DFU_UPLOAD_PAGES	2

WINPROPERTYPAGE _dnload_pages[DFU_DNLOAD_PAGES] = {
	{ IDD_WIZARDFIRM, DlgWizardBrowseProc, },
	{ IDD_WIZARDSIGN, DlgWizardSuffixProc, },
	{ IDD_WIZARDPROG, DlgWizardProgramProc, },
};

WINPROPERTYPAGE _upload_pages[DFU_UPLOAD_PAGES] = {
	{ IDD_WIZARDFIRM, DlgWizardBrowseProc, },
	{ IDD_WIZARDPROG, DlgWizardProgramProc, },
};

typedef unsigned __int64	llsize_t;
#define MAKELLONG(a, b)		((llsize_t)((a) | ((llsize_t)(b)) << 32)) 

void dfu_abort_firmware(LPDFUPROGRAM prog)
{
	DWORD bytes;
	BOOL res;
	res = DeviceIoControl(prog->dev,
			      IOCTL_USB_DFU_ABORT,
			      NULL, 0, NULL, 0,
			      &bytes, NULL);
	if (!res) {
		DWORD ecode = GetLastError();
	}
}

DWORD dfu_get_error(void)
{
	DWORD err = GetLastError();
	if (err == ERROR_SUCCESS)
		err = ERROR_UNKNOWN_EXCEPTION;
	return err;
}

BOOL dfu_dnload_firmware(LPDFUPROGRAM prog, UINT *pos, UINT *range)
{
	BOOL res;
	DWORD bytes;
	unsigned short blksz = prog->info->info.wTransferSize;

	if (*pos == 0) {
		llsize_t imgsz;
		DWORD low, high;
		prog->file = CreateFile(prog->firmware,
					GENERIC_READ,
					0,
					NULL,
					OPEN_EXISTING,
					0,/* FILE_FLAG_OVERLAPPED */
					NULL);
		if (prog->file == INVALID_HANDLE_VALUE) {
			prog->error = dfu_get_error();
			return FALSE;
		}
		low = GetFileSize(prog->file, &high);
		if (low == INVALID_FILE_SIZE) {
			prog->error = dfu_get_error();
			return FALSE;
		}
		imgsz = MAKELLONG(low, high);
		*pos = 1;
		/* open & blocks & the EOF block */
		*range = 2 + (UINT)((imgsz+(llsize_t)blksz-1) / blksz);
		prog->read_eof = FALSE;
		prog->read_pos = 0;
		prog->write_pos = 0;
		prog->blkno = 0;
	} else {
		if (!prog->read_eof && prog->read_pos < blksz) {
			res = ReadFile(prog->file,
				       prog->buffer+prog->read_pos,
				       blksz-prog->read_pos,
				       &bytes,
				       NULL);
			if (!res) {
				prog->error = dfu_get_error();
				if (prog->error != ERROR_HANDLE_EOF)
					return FALSE;
				prog->error = ERROR_SUCCESS;
				prog->read_eof = TRUE;
			}
			if (bytes == 0) {
				prog->read_eof = TRUE;
			}
			prog->read_pos += bytes;
		}
		if (prog->read_pos > prog->write_pos) {
			res = WriteFile(prog->dev,
					prog->buffer+prog->write_pos,
					prog->read_pos - prog->write_pos,
					&bytes,
					NULL);
			if (!res) {
				prog->error = dfu_get_error();
				return FALSE;
			}
			prog->write_pos += bytes;
			if (prog->write_pos == blksz) {
				prog->blkno++;
				assert(prog->read_pos == blksz);
				prog->read_pos = 0;
				prog->write_pos = 0;
				*pos = *pos + 1;
				if (!prog->read_eof && *pos == *range) {
					*range = *pos + 1;
				}
			}
		}
		if (prog->read_eof && prog->read_pos == prog->write_pos) {
			*pos = *pos + 1;
			*range = *pos;
			prog->completed = TRUE;
		}
	}
	return TRUE;
}

BOOL dfu_upload_firmware(LPDFUPROGRAM prog, UINT *pos, UINT *range)
{
	BOOL res;
	DWORD bytes;
	unsigned short blksz = prog->info->info.wTransferSize;

	if (*pos == 0) {
		prog->file = CreateFile(prog->firmware,
					GENERIC_WRITE,
					FILE_SHARE_WRITE,
					NULL,
					OPEN_ALWAYS,
					0,// FILE_FLAG_OVERLAPPED,
					NULL);
		if (prog->file == INVALID_HANDLE_VALUE) {
			prog->error = dfu_get_error();
			return FALSE;
		}
		/* wait for the next block */
		*pos = 1;
		*range = 1 + 1;
		prog->read_eof = FALSE;
		prog->read_pos = 0;
		prog->write_pos = 0;
		prog->blkno = 0;
	} else {
		if (!prog->read_eof && prog->read_pos < blksz) {
			res = ReadFile(prog->dev,
				       prog->buffer+prog->read_pos,
				       blksz-prog->read_pos,
				       &bytes,
				       NULL);
			if (!res) {
				prog->error = dfu_get_error();
				if (prog->error != ERROR_HANDLE_EOF)
					return FALSE;
				prog->error = ERROR_SUCCESS;
				prog->read_eof = TRUE;
			}
			if (bytes == 0)
				prog->read_eof = TRUE;
			prog->read_pos += bytes;
		}
		if (prog->read_pos > prog->write_pos) {
			res = WriteFile(prog->file,
					prog->buffer+prog->write_pos,
					prog->read_pos - prog->write_pos,
					&bytes,
					NULL);
			if (!res) {
				prog->error = dfu_get_error();
				return FALSE;
			}
			prog->write_pos += bytes;
			if (prog->write_pos == blksz) {
				prog->blkno++;
				assert(prog->read_pos == blksz);
				prog->read_pos = 0;
				prog->write_pos = 0;
				*pos = *pos + 1;
				*range = *pos + 1;
			}
		}
		if (prog->read_eof && prog->read_pos == prog->write_pos) {
			*pos = *pos + 1;
			*range = *pos;
			prog->completed = TRUE;
		}
	}
	return TRUE;
}

VOID DFUCloseProgram(LPDFUPROGRAM prog)
{
	if (prog) {
		assert(prog->thread == NULL);
		assert(prog->file == INVALID_HANDLE_VALUE);
		assert(prog->dev == INVALID_HANDLE_VALUE);
		if (prog->info) {
			free(prog->info);
			prog->info = NULL;
		}
		if (prog->buffer) {
			free(prog->buffer);
			prog->buffer = NULL;
		}
	}
	DFUStartEnum(prog->hwnd_main);
}

BOOL DFUOpenProgram(LPDFUWNDDATA lpWD, LPDFUPROGRAM prog, int type)
{
	LPDFUDEVICE dev = NULL;
	BOOL res = FALSE;

	memset(prog, 0, sizeof (DFUPROGRAM));
	prog->hwnd_main = lpWD->hWnd;
	prog->thread = NULL;
	prog->file = INVALID_HANDLE_VALUE;
	prog->dev = INVALID_HANDLE_VALUE;
	prog->seq = DFU_SEQ_IDLE;
	prog->seq_error = DFU_STATUS_OK;
	prog->seq_ended = TRUE;
	prog->seq_begun = FALSE;

	DFUStopEnum(prog->hwnd_main);

	dev = DFUGetSelected(lpWD);
	if (!dev) return FALSE;

	prog->info = DFUDupDevice(dev);
	if (!prog->info) goto fail;
	prog->buffer = malloc(prog->info->info.wTransferSize);
	if (!prog->buffer) goto fail;

	strncpy(prog->firmware, _szFirmware, MAX_PATH);
	res = TRUE;

	if (type == DFU_DNLOAD) {
		/* set device as download mode */
		DFULoadFirmware(&prog->firm);
		prog->type = DFU_DNLOAD;
		prog->program = dfu_dnload_firmware;
		prog->abort = dfu_abort_firmware;
	} else {
		/* set device as upload mode */
		prog->type = DFU_UPLOAD;
		prog->abort = dfu_abort_firmware;
		prog->program = dfu_upload_firmware;
	}
fail:
	if (res == FALSE)
		DFUCloseProgram(prog);
	return res;
}

VOID DFUDnloadFirmware(LPDFUWNDDATA lpWD)
{
	DFUPROGRAM prog;
	LPDFUDEVICE dev = NULL;

	if (!DFUOpenProgram(lpWD, &prog, DFU_DNLOAD)) {
		MessageIDBox(lpWD->hWnd,
			     IDS_ERROR_FILE_OPEN, IDS_ERROR,
			     MB_OK | MB_ICONERROR);
		goto end;
	}
	if (DlgLaunchWizard(lpWD->hWnd, ID_DFU_DNLOAD_INTRO, IDI_DNLOAD,
			    DFU_DNLOAD_PAGES, _dnload_pages, (ULONG)&prog)) {
		strncpy(_szFirmware, prog.firmware, MAX_PATH);
	}
end:
	DFUCloseProgram(&prog);
}

VOID DFUUploadFirmware(LPDFUWNDDATA lpWD)
{
	DFUPROGRAM prog;

	if (!DFUOpenProgram(lpWD, &prog, DFU_UPLOAD)) {
		MessageIDBox(lpWD->hWnd,
			     IDS_ERROR_FILE_OPEN, IDS_ERROR,
			     MB_OK | MB_ICONERROR);
		goto end;
	}
	if (DlgLaunchWizard(lpWD->hWnd, ID_DFU_UPLOAD_INTRO, IDI_UPLOAD,
			    DFU_UPLOAD_PAGES, _upload_pages, (ULONG)&prog)) {
		strncpy(_szFirmware, prog.firmware, MAX_PATH);
	}
end:
	DFUCloseProgram(&prog);
}

static LRESULT WINAPI DFUSuffixDlgProc(HWND hDlg, UINT uMsg,
				       WPARAM wParam, LPARAM lParam)
{
	struct dfu_firm *firm;
	int wmId, wmEvent;

	if (uMsg == WM_INITDIALOG) {
		firm = (struct dfu_firm *)lParam;
		SetWindowLong(hDlg, GWL_USERDATA, (LPARAM)firm);
	} else {
		firm = (struct dfu_firm *)GetWindowLong(hDlg, GWL_USERDATA);
	}

	switch (uMsg) {
	case WM_INITDIALOG:
		DFUSuffixLoadAttributes(hDlg, firm);
		DFUSuffixInitDialog(hDlg);
		CenterChild(hDlg, GetParent(hDlg));
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId) {
		case IDOK:
			DFUSuffixSaveAttributes(hDlg, firm);
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			break;
		case IDC_DEVICE:
		case IDC_VENDOR:
		case IDC_PRODUCT:
			if (wmEvent == EN_CHANGE) {
				DFUValidateInput(hDlg, wmId);
			}
			break;
		}
		break;
	}
	return 0L;
}

void DFUConfigureSuffix(LPDFUWNDDATA lpWD)
{
	struct dfu_firm firm;

	DFULoadFirmware(&firm);
	if (IDOK == DialogBoxParam(_hInstance,
				   MAKEINTRESOURCE(IDD_SUFFIX),
				   lpWD->hWnd, (DLGPROC)DFUSuffixDlgProc,
				   (LPARAM)&firm)) {
		DFUSaveFirmware(&firm);
	}
}

HANDLE DFUOpenDevice(LPDFUDEVICE dev)
{
	return CreateFile(dev->path,
			  GENERIC_READ | GENERIC_WRITE,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  NULL,
			  OPEN_EXISTING,
			  0,// FILE_FLAG_OVERLAPPED,
			  NULL);
}

VOID DFUCloseDevice(HANDLE hdev)
{
	if (hdev != INVALID_HANDLE_VALUE) CloseHandle(hdev);
}

BOOL DFUValidateDevice(CHAR *path, LPDFUDEVICE dev)
{
	HANDLE hdev = INVALID_HANDLE_VALUE;
	BOOL res = FALSE;
	struct dfu_info info;
	DWORD bytes;

	memset(&info, 0, sizeof (struct dfu_info));
	info.bcdDFUVersion = 0xFF;

	hdev = CreateFile(path,
			  GENERIC_READ | GENERIC_WRITE,
			  FILE_SHARE_READ | FILE_SHARE_WRITE,
			  NULL,
			  OPEN_EXISTING,
			  0,
			  NULL);
	if (hdev == INVALID_HANDLE_VALUE) goto end;
	res = DeviceIoControl(hdev, IOCTL_USB_DFU_GETINFO,
			      NULL, 0, &info, sizeof (struct dfu_info),
			      &bytes, NULL);
	if (!res) goto end;
	if (info.bcdDFUVersion > DFU_VERSION) {
		res = FALSE;
		goto end;
	}
	if (info.bInterfaceProtocol != USBDFU_PROTO_RUNTIME &&
	    info.bInterfaceProtocol != USBDFU_PROTO_DFUMODE) {
		res = FALSE;
		goto end;
	}
	strncpy(dev->path, path, MAX_PATH);
	memcpy(&dev->info, &info, sizeof (struct dfu_info));
	dev->sync_info = TRUE;

end:
	if (hdev != INVALID_HANDLE_VALUE) CloseHandle(hdev);
	return res;
}

BOOL DFUGetDeviceName(HDEVINFO hdi,
		      PSP_DEVICE_INTERFACE_DATA di_data,
		      CHAR *path)
{
	PSP_DEVICE_INTERFACE_DETAIL_DATA di_detail = NULL;
	ULONG length = 0;
	ULONG req_len = 0;
	BOOL res = FALSE;
	
	if (SetupDiGetDeviceInterfaceDetail(hdi, di_data, NULL, 0,
					    &req_len, NULL)) {
		goto end;
	}
	length = req_len;
	di_detail = malloc(length);
	if (NULL == di_detail) {
		goto end;
	}
	di_detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
	if (!SetupDiGetDeviceInterfaceDetail(hdi, di_data,
					     di_detail, length,
					     &req_len, NULL)) {
		goto end;
	}
	strcpy(path, di_detail->DevicePath);
	res = TRUE;
	
end:
	if (di_detail) free(di_detail);
	return res;
}

VOID DFUEnumDevices(LPDFUWNDDATA lpWD)
{
	LPGUID guid = (LPGUID)&GUID_DEVCLASS_FIRMWARE;
	HDEVINFO hdi = INVALID_HANDLE_VALUE;
	SP_DEVICE_INTERFACE_DATA di_data;
	UINT nr_devs = 0, index = 0;
	LPDFUDEVICE devs = NULL;
	ULONG i;
	BOOL res;
	CHAR path[MAX_PATH];
	DFUDEVICE dev;
	
	hdi = SetupDiGetClassDevs(guid, NULL, NULL,
				  DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hdi == INVALID_HANDLE_VALUE)
		return;

	for (i = 0; ; i++) {
		di_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		res = SetupDiEnumDeviceInterfaces(hdi, 0, guid, i, &di_data);
		if (!res) {
			if (ERROR_NO_MORE_ITEMS == GetLastError()) break;
		} else {
			if (!DFUGetDeviceName(hdi, &di_data, path)) continue;
			if (!DFUValidateDevice(path, &dev)) continue;

			nr_devs++;
			if (!devs) {
				devs = malloc(sizeof (DFUDEVICE));
			} else {
				devs = realloc(devs, nr_devs * sizeof (DFUDEVICE));
			}
			if (!devs) break;
			memcpy(&devs[index], &dev, sizeof (DFUDEVICE));
			index++;
		}
	}

	if (hdi != INVALID_HANDLE_VALUE)
		SetupDiDestroyDeviceInfoList(hdi);

	DFUUpdateDevices(lpWD, devs, nr_devs);
}

static LRESULT WINAPI About_DlgProc(HWND hDlg, UINT uMsg,
				    WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		CenterChild(hDlg, GetParent(hDlg));
		SetDlgItemText(hDlg, IDC_DFU_LICENSE, _dfu_license);
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, IDOK);
			break;
		}
		break;
	}
	return 0L;
}

void DisplayVersion(HWND hwndParent)
{
	DialogBox(_hInstance,
		  MAKEINTRESOURCE(IDD_ABOUT),
		  hwndParent, (DLGPROC)About_DlgProc);
}

void DFUUpdateCommands(LPDFUWNDDATA lpWD)
{
	LPDFUDEVICE dfu = DFUGetSelected(lpWD);
	BOOL dfu_mode = (dfu && dfu->info.bInterfaceProtocol == USBDFU_PROTO_DFUMODE);
	BOOL run_time = (dfu && dfu->info.bInterfaceProtocol == USBDFU_PROTO_RUNTIME);
	
	SendMessage(lpWD->hwndToolbar, TB_ENABLEBUTTON, ID_DFU_DETACH,
		    MAKELONG(run_time, 0));
	SendMessage(lpWD->hwndToolbar, TB_ENABLEBUTTON, ID_DFU_ABORT,
		    MAKELONG(dfu_mode, 0));
	SendMessage(lpWD->hwndToolbar, TB_ENABLEBUTTON, ID_DFU_DNLOAD,
		    MAKELONG(dfu_mode, 0));
	SendMessage(lpWD->hwndToolbar, TB_ENABLEBUTTON, ID_DFU_UPLOAD,
		    MAKELONG(dfu_mode, 0));
	SendMessage(lpWD->hwndToolbar, TB_ENABLEBUTTON, ID_DFU_CYCLE,
		    MAKELONG(dfu_mode || run_time, 0));

	EnableMenuItem(GetMenu(lpWD->hWnd), ID_DFU_DETACH,
		       run_time ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(GetMenu(lpWD->hWnd), ID_DFU_ABORT,
		       dfu_mode ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(GetMenu(lpWD->hWnd), ID_DFU_DNLOAD,
		       dfu_mode ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(GetMenu(lpWD->hWnd), ID_DFU_UPLOAD,
		       dfu_mode ? MF_ENABLED : MF_GRAYED);
	EnableMenuItem(GetMenu(lpWD->hWnd), ID_DFU_CYCLE,
		       (dfu_mode || run_time) ? MF_ENABLED : MF_GRAYED);
}

VOID DispatchNotifications(LPDFUWNDDATA lpWD, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR pnmh = (LPNMHDR)lParam;

	if (pnmh->code == TTN_NEEDTEXT) {
		GetToolbarText(lpWD->hWnd, (LPTOOLTIPTEXT)lParam);
		CommandSelected(lpWD->hWnd, WM_NOTIFY, 0, 0);
	}
}

void DFUBuildTitles(LPDFUWNDDATA lpWD)
{
	LVCOLUMN lvc;
	TCHAR tsz[64] = TEXT("");
	HBITMAP hBitmap;
	HIMAGELIST hImageList;

#define BITMAP_NUM_DFUMODES	3
#define MODEWIDTH		20
#define NAMEWIDTH		280
#define XFERWIDTH		100
#define STATEWIDTH		100
#define STATUSWIDTH		100
#define VIDWIDTH		50
#define PIDWIDTH		50
	
	/*
	hImageList = ImageList_Create(16, 16, ILC_COLOR|ILC_MASK, BITMAP_NUM, 0); 
	hBitmap = LoadBitmap(_DllInstance, MAKEINTRESOURCE(IDB_FACILITY));
	ImageList_AddMasked(hImageList, hBitmap, RGB(255, 0, 255));
	DeleteObject(hBitmap);
	lpWD->hFacility = hImageList;
	*/
	
	hImageList = ImageList_Create(16, 16, ILC_COLOR|ILC_MASK, BITMAP_NUM_DFUMODES, 0); 
	hBitmap = LoadBitmap(_hInstance, MAKEINTRESOURCE(IDB_DFUMODE4BIT));
	ImageList_AddMasked(hImageList, hBitmap, RGB(255, 0, 255));
	DeleteObject(hBitmap);
	lpWD->himgDFUModes = hImageList;
	
	// some stuff will not be changed
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvc.pszText = tsz;
	lvc.fmt = LVCFMT_LEFT;
	
	lvc.iSubItem = 0;
	
	lvc.cx = NAMEWIDTH;
	LoadString(_hInstance, ID_DFU_NAME, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndListView, 2, &lvc);
	
	lvc.cx = XFERWIDTH;
	LoadString(_hInstance, ID_DFU_XFER, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndListView, 3, &lvc);
	
	lvc.cx = STATEWIDTH;
	LoadString(_hInstance, ID_DFU_STATE, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndListView, 4, &lvc);
	
	lvc.cx = STATUSWIDTH;
	LoadString(_hInstance, ID_DFU_STATUS, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndListView, 5, &lvc);
	
	lvc.cx = VIDWIDTH;
	LoadString(_hInstance, ID_DFU_VENDOR, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndListView, 6, &lvc);
	
	lvc.cx = PIDWIDTH;
	LoadString(_hInstance, ID_DFU_PRODUCT, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndListView, 7, &lvc);
	
	ListView_SetImageList(lpWD->hwndListView, hImageList, LVSIL_SMALL);
}

BOOL DFUCreateWindow(LPDFUWNDDATA lpWD)
{
	HWND hWnd;

	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, 
			      "DfuListView",
			      WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS,
			      0, 0, 0, 0,
			      lpWD->hWnd,
			      (HMENU)ID_MAINFRAME, _hInstance, NULL);
	if (!hWnd) {
		MessageIDBox(NULL, IDS_ERROR_INIT_APPLICATION,
			     IDS_ERROR, MB_OK | MB_ICONERROR);
		return FALSE;
	}
	lpWD->hwndListView = hWnd;
	ListView_SetExtendedListViewStyle(lpWD->hwndListView, 
					  LVS_EX_FULLROWSELECT/* |
					  LVS_EX_GRIDLINES*/);
	DFUBuildTitles(lpWD);
	DFUInitDevices(lpWD);
	return TRUE;
}

VOID DFUDestroyWindow(LPDFUWNDDATA lpWD)
{
	HIMAGELIST hImageList;
	
	hImageList = ListView_SetImageList(lpWD->hwndListView, NULL, LVSIL_SMALL);
	//ASSERT(hImageList == lpWD->himgDFUModes);
	ImageList_Destroy(lpWD->himgDFUModes);
	DestroyWindow(lpWD->hwndListView);
	DFUFreeDevices(lpWD);
}

VOID SaveWindowContext(HWND hWnd)
{
	HKEY hkey;

	if (!RegCreateKeyEx(HKEY_CURRENT_USER, StringFromIDS(IDS_CONTEXT),
			    0, StringFromIDS(IDS_CONTEXT), 0,
			    KEY_READ | KEY_WRITE, 0, &hkey, NULL)) {
		RegSetValueEx(hkey, StringFromIDS(IDS_FIRMWARE), 0, REG_SZ,
			      (LPBYTE)&_szFirmware, strlen(_szFirmware)+1);
		RegSetValueEx(hkey, StringFromIDS(IDS_VENDOR), 0, REG_DWORD,
			      (LPBYTE)&_usVendor, sizeof(USHORT));
		RegSetValueEx(hkey, StringFromIDS(IDS_PRODUCT), 0, REG_DWORD,
			      (LPBYTE)&_usProduct, sizeof(USHORT));
		RegSetValueEx(hkey, StringFromIDS(IDS_DEVICE), 0, REG_DWORD,
			      (LPBYTE)&_usDevice, sizeof(USHORT));
		RegCloseKey(hkey);
	}
}

VOID RestoreWindowContext(HWND hWnd)
{
	DWORD cb;
	HKEY hkey = NULL;
	
	if (RegOpenKeyEx(HKEY_CURRENT_USER, StringFromIDS(IDS_CONTEXT),
			 0, KEY_READ, &hkey)) {
		return;
	}
	
	cb = MAX_PATH+1;
	if (RegQueryValueEx(hkey, StringFromIDS(IDS_FIRMWARE), 0, 0,
			    (LPBYTE)&_szFirmware, &cb)) {
	}
	cb = sizeof(USHORT);
	if (RegQueryValueEx(hkey, StringFromIDS(IDS_VENDOR), 0, 0,
			    (LPBYTE)&_usVendor, &cb)) {
	}
	cb = sizeof(USHORT);
	if (RegQueryValueEx(hkey, StringFromIDS(IDS_PRODUCT), 0, 0,
		(LPBYTE)&_usProduct, &cb)) {
	}
	cb = sizeof(USHORT);
	if (RegQueryValueEx(hkey, StringFromIDS(IDS_DEVICE), 0, 0,
		(LPBYTE)&_usDevice, &cb)) {
	}

	
	RegCloseKey(hkey);
}

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg,
				    WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	TCHAR szBuffer[1024];
	LPCREATESTRUCT lpcs;
	LPDFUWNDDATA lpWD = WinDFU_GetPtr(hWnd);
	
	if (lpWD == NULL) {
		if (uMsg == WM_NCCREATE) {
			lpWD = (LPDFUWNDDATA)malloc(sizeof (DFUWNDDATA));
			if (lpWD == NULL)
				return 0L;
			WinDFU_SetPtr(hWnd, lpWD);
		} else {
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	if (uMsg == WM_NCDESTROY)  {
		free(lpWD);
		lpWD = NULL;
		WinDFU_SetPtr(hWnd, NULL);
	}
	
	switch (uMsg) {
	case WM_CREATE:
		lpcs = (LPCREATESTRUCT)lParam;
		lpWD->hWnd = hWnd;
		if (!DFUCreateWindow(lpWD))
			return FALSE;
		lpWD->hwndToolbar = BuildRebar(hWnd, TOOLBARNUM, _ToolbarItems);
		lpWD->hwndStatusbar = BuildStatus(hWnd);
		LoadString(_hInstance, IDS_READY, szBuffer, CCHMAX(szBuffer));
		DisplayStatus(GetDlgItem(hWnd, ID_STATUS), TEXT("%s"), szBuffer);
		RestoreWindowPosition(hWnd);
		RestoreWindowContext(hWnd);
		DFUStartEnum(hWnd);
		SetTimer(hWnd, DFU_CMND_TIMER, DFU_CMND_TIMEOUT,
			 (TIMERPROC)MainWndProc);
		DFUUpdateCommands(lpWD);
		break;
	case WM_SIZE:
		RecalcLayout(hWnd);
		break;
	case WM_MENUSELECT:
		CommandSelected(hWnd, uMsg, wParam, lParam);
		break;
	case WM_ERASEBKGND:
		return 1;
	case WM_NOTIFY:
		DispatchNotifications(lpWD, wParam, lParam);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId) {
		case ID_APP_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_APP_ABOUT:
			DisplayVersion(hWnd);
			break;
		case ID_VIEW_STATUSBAR:
			if (_fMenuFlags & VIEW_STATUSBAR)
				_fMenuFlags &= ~VIEW_STATUSBAR;
			else
				_fMenuFlags |= VIEW_STATUSBAR;
			ShowWindow(GetDlgItem(hWnd, ID_STATUS),
				   _fMenuFlags & VIEW_STATUSBAR ? SW_SHOW : SW_HIDE);
			CheckMenuItem(GetMenu(hWnd), ID_VIEW_STATUSBAR,
				      _fMenuFlags & VIEW_STATUSBAR ?
				      (MF_CHECKED | MF_BYCOMMAND) : (MF_UNCHECKED | MF_BYCOMMAND));
			RecalcLayout(hWnd);
			break;
		case ID_VIEW_TOOLBAR:
			if (_fMenuFlags & VIEW_TOOLBAR)
				_fMenuFlags &= ~VIEW_TOOLBAR;
			else
				_fMenuFlags |= VIEW_TOOLBAR;
			ShowWindow(GetDlgItem(hWnd, ID_REBAR), _fMenuFlags & VIEW_TOOLBAR ? SW_SHOW : SW_HIDE);
			CheckMenuItem(GetMenu(hWnd), ID_VIEW_TOOLBAR, _fMenuFlags & VIEW_TOOLBAR ?
				      (MF_CHECKED | MF_BYCOMMAND) : (MF_UNCHECKED | MF_BYCOMMAND));
			RecalcLayout(hWnd);
			break;
		case ID_DFU_DETACH:
			DFUDetachDevice(lpWD);
			break;
		case ID_DFU_ABORT:
			DFUAbortDevice(lpWD);
			break;
		case ID_DFU_DNLOAD:
			DFUDnloadFirmware(lpWD);
			break;
		case ID_DFU_UPLOAD:
			DFUUploadFirmware(lpWD);
			break;
		case ID_DFU_CYCLE:
			DFUCycleDevice(lpWD);
			break;
		case ID_DFU_SUFFIX:
			DFUConfigureSuffix(lpWD);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;
	case WM_TIMER:
		switch (wParam) {
		case DFU_ENUM_TIMER:
			DFUEnumDevices(lpWD);
			break;
		case DFU_CMND_TIMER:
			DFUUpdateCommands(lpWD);
			break;
		}
		break;
	case WM_DESTROY:
		DFUDestroyWindow(lpWD);
		KillTimer(hWnd, DFU_CMND_TIMER);
		DFUStopEnum(hWnd);
		SaveWindowContext(hWnd);
		SaveWindowPosition(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

void DFUStartEnum(HWND hWnd)
{
	SetTimer(hWnd, DFU_ENUM_TIMER, DFU_ENUM_TIMEOUT,
		 (TIMERPROC)MainWndProc);
}

void DFUStopEnum(HWND hWnd)
{
	KillTimer(hWnd, DFU_ENUM_TIMER);
}

ATOM RegisterAppClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ATOM atom;
	
	wcex.cbSize = sizeof (WNDCLASSEX); 
	
	wcex.style	   = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = (WNDPROC)MainWndProc;
	wcex.cbClsExtra	   = 0;
	wcex.cbWndExtra	   = sizeof (DFUWNDDATA);
	wcex.hInstance	   = hInstance;
	wcex.hIcon	   = NULL;//LoadIcon(hInstance, MAKEINTRESOURCE(IDI_INSTANCE));
	wcex.hCursor	   = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);//(HBRUSH)COLOR_WINDOWFRAME;
	wcex.lpszMenuName  = (LPCTSTR)IDM_MAINFRAME;
	wcex.lpszClassName = _szWindowClass;
	wcex.hIconSm	   = LoadImage(wcex.hInstance, MAKEINTRESOURCE(IDI_MAINFRAME),
				       IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	
	atom = RegisterClassEx(&wcex);
	return atom;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;
	
	_hInstance = hInstance;
	
	hWnd = CreateWindow(_szWindowClass, _szTitle, WS_OVERLAPPEDWINDOW,
			    CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL,
			    hInstance, NULL);
	
	if (!hWnd) return FALSE;
	
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		   LPSTR lpCmdLine, int nShowCmd)
{
	MSG msg;
	HACCEL hAccelTable;
	INITCOMMONCONTROLSEX iccex;
	
	LoadString(hInstance, IDS_APP_TITLE, _szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_APP_CLASS, _szWindowClass, MAX_LOADSTRING);
	
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccex.dwICC = ICC_COOL_CLASSES;
	InitCommonControlsEx(&iccex);
	
	RegisterAppClass(hInstance);
	
	if (!InitInstance(hInstance, nShowCmd)) 
		return FALSE;

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDA_MAINFRAME);
	
	while (GetMessage(&msg, NULL, 0, 0))  {
		if (msg.hwnd && !TranslateAccelerator(msg.hwnd, hAccelTable, &msg))  {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}
