/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2014
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
 * @(#)winacpi.c: ACPI object viewer windows user interface
 * $Id: winacpi.c,v 1.87 2011-10-17 01:40:34 zhenglv Exp $
 */
#include <initguid.h>
#include "winacpi.h"

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
	IDD_WIZARDINTRO,
	IDD_WIZARDDONE,
	IDD_PROGRESS,
	IDC_WIZBITMAP,
	IDC_INTRODUCTION,
	IDC_MESSAGE,
	IDB_WIZARD,
	IDB_HEADER,
	IDS_ERROR,
	IDS_ERROR_FILE_BROWSE,
};
#define TOOLBARNUM		4
#define IDX_EXITAPP		0
#define IDX_ACPI_TABLELOAD	1
#define IDX_ACPI_TABLEUNLOAD	2
#define IDX_ACPI_TABLETEST	3

WINTOOLBARITEM _ToolbarItems[TOOLBARNUM] = {
	{ FALSE, IDX_EXITAPP, ID_APP_EXIT },
	{ FALSE, IDX_ACPI_TABLELOAD, ID_TABLE_LOAD },
	{ FALSE, IDX_ACPI_TABLEUNLOAD, ID_TABLE_UNLOAD },
	{ FALSE, IDX_ACPI_TABLETEST, ID_TEST_TABLE_UNLOAD },
};

static LRESULT WINAPI About_DlgProc(HWND hDlg, UINT uMsg,
				    WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg,
				    WPARAM wParam, LPARAM lParam);
static VOID ACPIAppendTable(LPACPIWNDDATA lpWD, acpi_ddb_t ddb);
static VOID ACPIRemoveTable(LPACPIWNDDATA lpWD, acpi_ddb_t ddb);
static acpi_ddb_t ACPIGetSelectedTable(LPACPIWNDDATA lpWD);

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

#define WM_LOADTABLE	WM_LAYOUTUSER+1
#define WM_UNLOADTABLE	WM_LAYOUTUSER+2

typedef unsigned __int64	llsize_t;

static LRESULT WINAPI About_DlgProc(HWND hDlg, UINT uMsg,
				    WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_INITDIALOG:
		CenterChild(hDlg, GetParent(hDlg));
		//SetDlgItemText(hDlg, IDC_DFU_LICENSE, _dfu_license);
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

static LRESULT WINAPI TableUnloadTest_DlgProc(HWND hDlg, UINT uMsg,
					      WPARAM wParam, LPARAM lParam)
{
	LPACPITESTTABLEUNLOAD pTableUnload;

	if (uMsg == WM_INITDIALOG) {
		pTableUnload = (LPACPITESTTABLEUNLOAD)lParam;
		SetWindowLong(hDlg, GWL_USERDATA, (LPARAM)pTableUnload);
	} else {
		pTableUnload = (LPACPITESTTABLEUNLOAD)GetWindowLong(hDlg, GWL_USERDATA);
	}

	switch (uMsg) {
	case WM_INITDIALOG:
		SetDlgItemText(hDlg, IDC_TABLEUNLOAD_PATH, pTableUnload->szPath);
		SetDlgItemInt(hDlg, IDC_TABLEUNLOAD_THREADS, pTableUnload->nThreads, FALSE);
		SetDlgItemInt(hDlg, IDC_TABLEUNLOAD_ITERS, pTableUnload->nIterations, FALSE);
		CenterChild(hDlg, GetParent(hDlg));
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, IDOK);
			break;
		case IDC_TABLEUNLOAD_PATH:
		case IDC_TABLEUNLOAD_THREADS:
		case IDC_TABLEUNLOAD_ITERS:
			break;
		case IDC_BROWSE:
			if (DlgBrowseDirectory(hDlg, pTableUnload->szPath,
					       sizeof (pTableUnload->szPath)))
				SetDlgItemText(hDlg, IDC_TABLEUNLOAD_PATH, pTableUnload->szPath);
			break;
		}
		break;
	}
	return 0L;
}

void StartTableUnloadTest(LPACPIWNDDATA lpWD)
{
	LPACPITESTTABLEUNLOAD pTableUnload;

	pTableUnload = (LPACPITESTTABLEUNLOAD)&lpWD->utTableUnload;
	if (IDOK == DialogBoxParam(_hInstance,
				   MAKEINTRESOURCE(IDD_TABLEUNLOAD_TEST),
				   lpWD->hWnd, (DLGPROC)TableUnloadTest_DlgProc,
				   (LPARAM)pTableUnload)) {
		acpi_test_TableUnload_start(pTableUnload->szPath,
					    pTableUnload->nThreads,
					    pTableUnload->nIterations);
		//"E:\\workspace\\acpica\\kernel-bugs\\36932", 10, 100);
	}
}

void MainUpdateCommands(LPACPIWNDDATA lpWD)
{
	acpi_ddb_t ddb = ACPIGetSelectedTable(lpWD);
	BOOL valid_ddb = (ddb != ACPI_DDB_HANDLE_INVALID);

	SendMessage(lpWD->hwndToolbar, TB_ENABLEBUTTON, ID_TABLE_UNLOAD,
		    MAKELONG(valid_ddb, 0));

	EnableMenuItem(GetMenu(lpWD->hWnd), ID_TABLE_UNLOAD,
		       valid_ddb ? MF_ENABLED : MF_GRAYED);
}

VOID DispatchNotifications(LPACPIWNDDATA lpWD, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR pnmh = (LPNMHDR)lParam;

	if (pnmh->code == TTN_NEEDTEXT) {
		GetToolbarText(lpWD->hWnd, (LPTOOLTIPTEXT)lParam);
		CommandSelected(lpWD->hWnd, WM_NOTIFY, 0, 0);
	}
}

acpi_status_t AcpiHandleTableEvents(struct acpi_table_desc *table,
				    acpi_ddb_t ddb, uint32_t event,
				    void *context)
{
	LPACPIWNDDATA lpWD = (LPACPIWNDDATA)context;

	switch (event) {
	case ACPI_EVENT_TABLE_INSTALL:
		SendNotifyMessage(lpWD->hWnd, WM_LOADTABLE, ddb, 0);
		break;
	case ACPI_EVENT_TABLE_UNINSTALL:
		SendNotifyMessage(lpWD->hWnd, WM_UNLOADTABLE, ddb, 0);
		break;
	case ACPI_EVENT_TABLE_LOAD:
		break;
	case ACPI_EVENT_TABLE_UNLOAD:
		break;
	}

	return AE_OK;
}

VOID ACPIInitApplication(LPACPIWNDDATA lpWD)
{
	acpi_emu_init();
	acpi_ospm_init();
	acpi_test_init();
	acpi_event_register_table_handler(AcpiHandleTableEvents, lpWD);
}

VOID ACPIExitApplication(LPACPIWNDDATA lpWD)
{
	acpi_test_exit();
	acpi_ospm_exit();
	acpi_event_unregister_table_handler(AcpiHandleTableEvents);
}

static VOID ACPIAppendTable(LPACPIWNDDATA lpWD, acpi_ddb_t ddb)
{
	LVITEM lvi = { 0 };
	int nIndex;
	HWND hwndList = lpWD->hwndTableList;
	CHAR tmpstring[10];
	char name[ACPI_NAME_SIZE+1];
	struct acpi_table_header *table;

	if (ACPI_FAILURE(acpi_get_table(ddb, &table)))
		return;

	memset(name, 0, sizeof (name));
	ACPI_NAMECPY(ACPI_NAME2TAG(table->signature), name);

	lvi.iItem = 0;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_TEXT;
	lvi.iImage = (acpi_table_contains_aml(table) ? 0x00 : 0x01) |
		     (acpi_table_is_loaded(ddb) ? 0x00: 0x02);
	lvi.lParam = (LPARAM)ddb;
	lvi.pszText = name;
	lvi.cchTextMax = _tcslen(name) ? _tcslen(name) : 0;
	nIndex = ListView_InsertItem(hwndList, &lvi);

	if (nIndex != -1) {
		if (acpi_table_has_header(name)) {
			sprintf(tmpstring, "%6.6s", table->oem_id);
			ListView_SetItemText(hwndList, nIndex, 1, tmpstring);
			sprintf(tmpstring, "%8.8s", table->oem_table_id);
			ListView_SetItemText(hwndList, nIndex, 2, tmpstring);
			sprintf(tmpstring, "0x%02X", table->revision);
			ListView_SetItemText(hwndList, nIndex, 3, tmpstring);
			sprintf(tmpstring, "0x%08X", table->oem_revision);
			ListView_SetItemText(hwndList, nIndex, 4, tmpstring);
		}
	}

	acpi_put_table(ddb, table);
}

static VOID ACPIRemoveTable(LPACPIWNDDATA lpWD, acpi_ddb_t ddb)
{
	LV_FINDINFO lvfi;
	int nIndex;
	HWND hwndList = lpWD->hwndTableList;

	lvfi.flags = LVFI_PARAM;
	lvfi.lParam = (LPARAM)ddb;
	nIndex = ListView_FindItem(hwndList, -1, &lvfi);
	if (nIndex != -1)
		ListView_DeleteItem(hwndList, nIndex);
}

acpi_ddb_t ACPIGetSelectedTable(LPACPIWNDDATA lpWD)
{
	int nIndex;
	HWND hwndList = lpWD->hwndTableList;
	LVITEM lvi = { 0 };

	nIndex = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);
	if (nIndex == -1)
		return ACPI_DDB_HANDLE_INVALID;

	lvi.iItem = nIndex;
	lvi.iSubItem = 0;
	lvi.mask = LVIF_PARAM;
	if (!ListView_GetItem(hwndList, &lvi))
		return ACPI_DDB_HANDLE_INVALID;

	return (acpi_ddb_t)lvi.lParam;
}

void ACPIBuildTableTitles(LPACPIWNDDATA lpWD)
{
	LVCOLUMN lvc;
	TCHAR tsz[64] = TEXT("");
	HBITMAP hBitmap;
	HIMAGELIST hImageList;

#define BITMAP_NUM_TABLETYPES	4
#define SIGNATUREWIDTH		100
#define OEMIDWIDTH		120
#define OEMTABLEIDWIDTH		160
#define REVISIONWIDTH		50
#define OEMREVISIONWIDTH	100
	
	hImageList = ImageList_Create(16, 16, ILC_COLOR|ILC_MASK, BITMAP_NUM_TABLETYPES, 0); 
	hBitmap = LoadBitmap(_hInstance, MAKEINTRESOURCE(IDB_TABLETYPE4BIT));
	ImageList_AddMasked(hImageList, hBitmap, RGB(255, 0, 255));
	DeleteObject(hBitmap);
	lpWD->himgTableTypes = hImageList;
	
	// some stuff will not be changed
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvc.pszText = tsz;
	lvc.fmt = LVCFMT_LEFT;
	
	lvc.iSubItem = 0;
	
	lvc.cx = SIGNATUREWIDTH;
	LoadString(_hInstance, IDS_TABLE_SIGNATURE, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndTableList, 2, &lvc);
	
	lvc.cx = OEMIDWIDTH;
	LoadString(_hInstance, IDS_TABLE_OEMID, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndTableList, 3, &lvc);
	
	lvc.cx = OEMTABLEIDWIDTH;
	LoadString(_hInstance, IDS_TABLE_OEMTABLEID, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndTableList, 4, &lvc);
	
	lvc.cx = REVISIONWIDTH;
	LoadString(_hInstance, IDS_TABLE_REVISION, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndTableList, 5, &lvc);
	
	lvc.cx = OEMREVISIONWIDTH;
	LoadString(_hInstance, IDS_TABLE_OEMREVISION, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndTableList, 6, &lvc);
	
	ListView_SetImageList(lpWD->hwndTableList, hImageList, LVSIL_SMALL);
}

void ACPIBuildObjectTitles(LPACPIWNDDATA lpWD)
{
	LVCOLUMN lvc;
	TCHAR tsz[64] = TEXT("");

#define ATTRIBWIDTH		50
#define VALUEWIDTH		60
	
	// some stuff will not be changed
	lvc.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvc.pszText = tsz;
	lvc.fmt = LVCFMT_LEFT;
	
	lvc.iSubItem = 0;
	
	lvc.cx = ATTRIBWIDTH;
	LoadString(_hInstance, IDS_OBJECT_ATTRIBUTE, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndNamespaceList, 2, &lvc);
	
	lvc.cx = VALUEWIDTH;
	LoadString(_hInstance, IDS_OBJECT_VALUE, tsz, sizeof (tsz));
	lvc.cchTextMax = _tcslen(tsz);
	ListView_InsertColumn(lpWD->hwndNamespaceList, 3, &lvc);
}

BOOL ACPICreateWindow(LPACPIWNDDATA lpWD)
{
	SPLCREATESTRUCT splcs;
	SPLPANEINFO sppi;
	DWORD dwStyle;

	memset(&splcs, 0, sizeof (splcs));
	splcs.hInstance = _hInstance;
	splcs.lpszClass = _szWindowClass;
	splcs.hwndParent = lpWD->hWnd;
	splcs.dwStyle = WS_CHILD | WS_VISIBLE;
	splcs.nID = EC_IDW_PANE_FIRST;
	splcs.nRows = 1;
	splcs.nCols = 3;
	splcs.sizeMin.cx = 16;
	splcs.sizeMin.cy = 16;
	splcs.nMaxCols = 0;
	splcs.nMaxRows = 0;
	
	dwStyle = WS_VISIBLE | WS_CHILD | WS_BORDER |
		  WS_TABSTOP | WS_GROUP;
	lpWD->hwndSplitter = CreateWindowEx(WS_EX_CLIENTEDGE, WC_SPLITTER,
					    TEXT("ACPIMainFrame"), dwStyle,
					    0, 0, 0, 0, lpWD->hWnd,
					    (HMENU)ID_MAINFRAME,
					    _hInstance, &splcs);
	
	splcs.lpszClass = WC_LISTVIEW;
	splcs.lpszName = TEXT("ACPITables");
	splcs.dwStyle = LVS_REPORT | LVS_SHOWSELALWAYS;
	splcs.nRows = 0;
	splcs.nCols = 0;
	splcs.sizeMin.cx = 60;
	splcs.sizeMin.cy = 0;
	if (!Splitter_CreateView(lpWD->hwndSplitter, &splcs))
		return FALSE;
	
	splcs.lpszClass = WC_TREEVIEW;
	splcs.lpszName = TEXT("ACPINamespace");
	splcs.dwStyle = TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	splcs.nRows = 0;
	splcs.nCols = 1;
	splcs.sizeMin.cx = 500;
	splcs.sizeMin.cy = 0;
	if (!Splitter_CreateView(lpWD->hwndSplitter, &splcs))
		return FALSE;
	
	splcs.lpszClass = WC_LISTVIEW;
	splcs.lpszName = TEXT("ACPIObjects");
	splcs.dwStyle = LVS_REPORT | LVS_SHOWSELALWAYS;
	splcs.nRows = 0;
	splcs.nCols = 2;
	splcs.sizeMin.cx = 120;
	splcs.sizeMin.cy = 0;
	if (!Splitter_CreateView(lpWD->hwndSplitter, &splcs))
		return FALSE;
	
	sppi.col = 0;
	sppi.row = 0;
	lpWD->hwndTableList = Splitter_GetPane(lpWD->hwndSplitter, &sppi);
	ListView_SetExtendedListViewStyle(lpWD->hwndTableList, 
					  LVS_EX_FULLROWSELECT/* |
					  LVS_EX_GRIDLINES*/);
	ACPIBuildTableTitles(lpWD);

	sppi.col = 1;
	sppi.row = 0;
	lpWD->hwndNamespaceTree = Splitter_GetPane(lpWD->hwndSplitter, &sppi);

	sppi.col = 2;
	sppi.row = 0;
	lpWD->hwndNamespaceList = Splitter_GetPane(lpWD->hwndSplitter, &sppi);
	ListView_SetExtendedListViewStyle(lpWD->hwndNamespaceList, 
					  LVS_EX_FULLROWSELECT/* |
					  LVS_EX_GRIDLINES*/);
	ACPIBuildObjectTitles(lpWD);

	ShowWindow(lpWD->hwndSplitter, SW_SHOW);
	memset(&lpWD->utTableUnload, 0, sizeof (ACPITESTTABLEUNLOAD));
	lpWD->utTableUnload.nThreads = 10;
	lpWD->utTableUnload.nIterations = 10;

	ACPIInitApplication(lpWD);
	return TRUE;
}

VOID ACPIDestroyWindow(LPACPIWNDDATA lpWD)
{
	ACPIExitApplication(lpWD);
	DestroyWindow(lpWD->hwndNamespaceList);
	DestroyWindow(lpWD->hwndNamespaceTree);
	DestroyWindow(lpWD->hwndTableList);
	DestroyWindow(lpWD->hwndSplitter);
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

static INT ACPIDisplayStatus(HWND hWnd, UINT uMessage, UINT uCaption, UINT uType,
			     acpi_status_t status)
{
	TCHAR szMessage[MAX_MESSAGE];
	TCHAR szCaption[MAX_CAPTION];
	TCHAR szText[MAX_MESSAGE];

	LoadString(_hInstance, uMessage, szMessage, MAX_MESSAGE);
	LoadString(_hInstance, uCaption, szCaption, MAX_CAPTION);

	sprintf(szText, szMessage, acpi_error_string(status, true));
	
	return MessageBox(hWnd, szText, szCaption, MB_ICONQUESTION | uType);
}

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg,
				    WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	TCHAR szBuffer[1024];
	char szFile[MAX_PATH] = "";
	LPCREATESTRUCT lpcs;
	LPACPIWNDDATA lpWD = WinACPI_GetPtr(hWnd);
	char szSuffix[] = "*.*";
	char szName[] = "All Files (*.*)";
	char *pSuffix[1] = { szSuffix };
	char *pName[1] = { szName };
	acpi_status_t status;
	
	if (lpWD == NULL) {
		if (uMsg == WM_NCCREATE) {
			lpWD = (LPACPIWNDDATA)malloc(sizeof (ACPIWNDDATA));
			if (lpWD == NULL)
				return 0L;
			WinACPI_SetPtr(hWnd, lpWD);
		} else {
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	if (uMsg == WM_NCDESTROY)  {
		free(lpWD);
		lpWD = NULL;
		WinACPI_SetPtr(hWnd, NULL);
	}
	
	switch (uMsg) {
	case WM_CREATE:
		lpcs = (LPCREATESTRUCT)lParam;
		lpWD->hWnd = hWnd;
		if (!ACPICreateWindow(lpWD))
			return FALSE;
		lpWD->hwndToolbar = BuildRebar(hWnd, TOOLBARNUM, _ToolbarItems);
		lpWD->hwndStatusbar = BuildStatus(hWnd);
		LoadString(_hInstance, IDS_READY, szBuffer, CCHMAX(szBuffer));
		DisplayStatus(GetDlgItem(hWnd, ID_STATUS), TEXT("%s"), szBuffer);
		RestoreWindowPosition(hWnd);
		RestoreWindowContext(hWnd);
		SetTimer(hWnd, TIMER_REFRESH, TIMER_REFRESH_TIMEOUT,
			 (TIMERPROC)MainWndProc);
		MainUpdateCommands(lpWD);
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
		case ID_TABLE_LOAD:
			if (DlgBrowseFile(hWnd,
					  szFile, MAX_PATH,
					  pName, pSuffix, 1,
					  FALSE)) {
				status = acpi_emu_load_table(szFile, NULL);
				if (ACPI_FAILURE(status))
					ACPIDisplayStatus(hWnd, IDS_ERROR_LOAD_TABLE,
							  IDS_ERROR, MB_OK, status);
			}
			break;
		case ID_TABLE_UNLOAD:
			acpi_uninstall_table(ACPIGetSelectedTable(lpWD));
			break;
		case ID_TEST_TABLE_UNLOAD:
			StartTableUnloadTest(lpWD);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
		break;
	case WM_LOADTABLE:
		ACPIAppendTable(lpWD, (acpi_ddb_t)wParam);
		break;
	case WM_UNLOADTABLE:
		ACPIRemoveTable(lpWD, (acpi_ddb_t)wParam);
		break;
	case WM_TIMER:
		switch (wParam) {
		case TIMER_REFRESH:
			MainUpdateCommands(lpWD);
			break;
		}
		break;
	case WM_DESTROY:
		ACPIDestroyWindow(lpWD);
		KillTimer(hWnd, TIMER_REFRESH);
		SaveWindowContext(hWnd);
		SaveWindowPosition(hWnd);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

ATOM RegisterAppClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;
	ATOM atom;
	
	wcex.cbSize = sizeof (WNDCLASSEX); 
	
	wcex.style	   = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = (WNDPROC)MainWndProc;
	wcex.cbClsExtra	   = 0;
	wcex.cbWndExtra	   = sizeof (ACPIWNDDATA);
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
	
	InitExtendedControls();
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
