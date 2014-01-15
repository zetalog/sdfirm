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
#include "winacpi.h"

TCHAR _szTitle[MAX_LOADSTRING];
TCHAR _szWindowClass[MAX_LOADSTRING];
TCHAR _szFirmware[MAX_PATH+1];
USHORT _usVendor = 0;
USHORT _usProduct = 0;
USHORT _usDevice = 0;
HINSTANCE _hInstance;

static LRESULT WINAPI About_DlgProc(HWND hDlg, UINT uMsg,
				    WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg,
				    WPARAM wParam, LPARAM lParam);

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

#define WM_PROGRAM	WM_USER+1

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

void MainUpdateCommands(LPACPIWNDDATA lpWD)
{
}

VOID DispatchNotifications(LPACPIWNDDATA lpWD, WPARAM wParam, LPARAM lParam)
{
	LPNMHDR pnmh = (LPNMHDR)lParam;

	if (pnmh->code == TTN_NEEDTEXT) {
		GetToolbarText(lpWD->hWnd, (LPTOOLTIPTEXT)lParam);
		CommandSelected(lpWD->hWnd, WM_NOTIFY, 0, 0);
	}
}

BOOL ACPICreateWindow(LPACPIWNDDATA lpWD)
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
	return TRUE;
}

VOID ACPIDestroyWindow(LPACPIWNDDATA lpWD)
{
	DestroyWindow(lpWD->hwndListView);
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
	LPACPIWNDDATA lpWD = WinACPI_GetPtr(hWnd);
	
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
		lpWD->hwndToolbar = BuildRebar(hWnd);
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
			acpi_emu_init();
			acpi_ospm_init();
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
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
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
