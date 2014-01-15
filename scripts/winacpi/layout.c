/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2007
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
 * @(#)layout.c: window layout storing function
 * $Id: layout.c,v 1.7 2011-08-23 04:01:51 zhenglv Exp $
 */

#include "winacpi.h"

ULONG _fMenuFlags;
DWORD _dwRebarSide;

#define STATUSNUM	2
#define TOOLBARNUM	1
#define SPERATORNUM     1
#define CX_BITMAP       16
#define CY_BITMAP       16
#define CX_SPERATOR     20

#define IDX_EXITAPP		0

static BOOL ToolbarSeparator[TOOLBARNUM] = {0};

void MoveMainFrame(HWND hWnd, RECT *lpRect)
{
	RECT rcList;
	int x, y, cx, cy;
	
	GetWindowRect(GetDlgItem(hWnd, ID_MAINFRAME), &rcList);
	
	x = lpRect->left;
	y = lpRect->top;
	cx = lpRect->right - lpRect->left;
	cy = lpRect->bottom - lpRect->top;
	
	MoveWindow(GetDlgItem(hWnd, ID_MAINFRAME), x, y, cx, cy, TRUE);
}

void RegSaveString(UINT nKey, UINT nVal, LPCSTR pszVal)
{
	HKEY hkey;

	if (!RegCreateKeyEx(HKEY_LOCAL_MACHINE, StringFromIDS(nKey),
			    0, StringFromIDS(nKey), 0,
			    KEY_READ | KEY_WRITE, 0, &hkey, NULL)) {
		RegSetValueEx(hkey, StringFromIDS(nVal), 0, REG_SZ,
			      (LPBYTE)pszVal, pszVal ? strlen(pszVal) : 0);
		RegCloseKey(hkey);
	}
}

void RegDeleteString(UINT nKey, UINT nVal)
{
	HKEY hkey = NULL;
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, StringFromIDS(nKey),
			 0, KEY_ALL_ACCESS, &hkey)) {
		return;
	}
	
	RegDeleteValue(hkey, StringFromIDS(nVal));
	RegCloseKey(hkey);
}

INT RegLoadString(UINT nKey, UINT nVal, LPSTR pszVal, UINT nSize)
{
	DWORD cb;
	HKEY hkey = NULL;
	INT nRes = -1;
	
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, StringFromIDS(nKey),
			 0, KEY_READ, &hkey)) {
		return nRes;
	}
	
	cb = nSize;
	if (RegQueryValueEx(hkey, StringFromIDS(nVal), 0, 0,
			    (LPBYTE)pszVal, &cb) == 0) {
		nRes = 0;
	}
	
	RegCloseKey(hkey);
	return nRes;
}

INT RegLoadDword(UINT nKey, UINT nVal, INT *pdwValue)
{
	INT cb;
	HKEY hkey = NULL;
	INT nRes = -1;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, StringFromIDS(nKey),
			 0, KEY_READ, &hkey)) {
		return nRes;
	}
	
	cb = sizeof (INT);
	if (RegQueryValueEx(hkey, StringFromIDS(nVal), 0, 0,
			    (LPBYTE)pdwValue, &cb) == 0) {
		nRes = 0;
	}
	
	RegCloseKey(hkey);
	return nRes;
}

void RegSaveDword(UINT nKey, UINT nVal, INT dwVal)
{
	HKEY hkey;

	if (!RegCreateKeyEx(HKEY_LOCAL_MACHINE, StringFromIDS(nKey),
			    0, StringFromIDS(nKey), 0,
			    KEY_READ | KEY_WRITE, 0, &hkey, NULL)) {
		RegSetValueEx(hkey, StringFromIDS(nVal), 0, REG_DWORD,
			      (LPBYTE)&dwVal, sizeof (INT));
		RegCloseKey(hkey);
	}
}

TCHAR *StringFromIDS(UINT unID)
{
#define cchMaxIDS		200
	static TCHAR szBuf1[cchMaxIDS];

	szBuf1[0] = '\0';
	/*SideAssert(*/LoadString(_hInstance, unID, szBuf1, cchMaxIDS)/*)*/;
	return szBuf1;
}

/*
 * RestoreWindowPosition
 * Purpose:
 *	Nostalgia
 * Arguments:
 *	hwnd to nostalge
 * Returns:
 *	zipola
 */
VOID RestoreWindowPosition(HWND hWnd)
{
	DWORD cb;
	WINDOWPLACEMENT	wndpl;
	HKEY hkey = NULL;
	INT bShow = FALSE;
	
	wndpl.showCmd = SW_SHOWNORMAL;
	if (RegOpenKeyEx(HKEY_CURRENT_USER, StringFromIDS(IDS_POSITION),
			 0, KEY_READ, &hkey)) {
		_fMenuFlags = VIEW_TOOLBAR | VIEW_STATUSBAR;
		goto err;
	}
	
	cb = sizeof (_fMenuFlags);
	if (RegQueryValueEx(hkey, StringFromIDS(IDS_MENUFLAGS), 0, 0,
			    (LPBYTE)&_fMenuFlags, &cb)) {
		_fMenuFlags = VIEW_TOOLBAR | VIEW_STATUSBAR;
	}
	
	cb = sizeof (wndpl);
	if (!RegQueryValueEx(hkey, StringFromIDS(IDS_PLACEMENT), 0, 0,
			     (LPBYTE)&wndpl, &cb)) {
		if (wndpl.length == sizeof (wndpl)) {
			int cmd;
			cmd = wndpl.showCmd;
			if (cmd == SW_SHOWMAXIMIZED)
				cmd = SW_MAXIMIZE;
			if (cmd == SW_SHOWMINIMIZED)
				cmd = SW_MINIMIZE;
			wndpl.showCmd = SW_SHOWNORMAL;
			SetWindowPlacement(hWnd, &wndpl);
			//ShowWindow(hWnd, cmd);
			bShow = TRUE;
		}
	}
	
	RegCloseKey(hkey);
	
err:
	if (!bShow)
		ShowWindow(hWnd, SW_SHOWNORMAL);
	CheckMenuItem(GetMenu(hWnd), ID_VIEW_TOOLBAR, _fMenuFlags & VIEW_TOOLBAR ?
		      (MF_CHECKED | MF_BYCOMMAND) : (MF_UNCHECKED | MF_BYCOMMAND));
	CheckMenuItem(GetMenu(hWnd), ID_VIEW_STATUSBAR, _fMenuFlags & VIEW_STATUSBAR ?
		      (MF_CHECKED | MF_BYCOMMAND) : (MF_UNCHECKED | MF_BYCOMMAND));
	ShowWindow(hWnd, wndpl.showCmd);
	ShowWindow(GetDlgItem(hWnd, ID_STATUS), _fMenuFlags & VIEW_STATUSBAR ? SW_SHOW : SW_HIDE);
	ShowWindow(GetDlgItem(hWnd, ID_REBAR), _fMenuFlags & VIEW_TOOLBAR ? SW_SHOW : SW_HIDE);
	RecalcLayout(hWnd);
}

void RecalcLayout(HWND hWnd)
{
	RECT rc;
	
	GetClientRect(hWnd, &rc);
	MoveRebar(hWnd, &rc);
	MoveStatus(hWnd, &rc);
	MoveMainFrame(hWnd, &rc);
	InvalidateRect(hWnd, NULL, FALSE);
}

/*
 * SaveWindowPosition
 * Purpose:
 *	Put all our eggs in one basket
 * Arguments:
 *	hWnd of basket
 * Returns:
 *	zipster
 */
VOID SaveWindowPosition(HWND hWnd)
{
	WINDOWPLACEMENT	wndpl;
	HKEY hkey;

	wndpl.length = sizeof (wndpl);
	GetWindowPlacement(hWnd, &wndpl);
	// for some reason, GetWindowPlacement resets the length part??
	wndpl.length = sizeof (wndpl);
	
	if (!RegCreateKeyEx(HKEY_CURRENT_USER, StringFromIDS(IDS_POSITION),
			    0, StringFromIDS(IDS_POSITION), 0,
			    KEY_READ | KEY_WRITE, 0, &hkey, NULL)) {
		RegSetValueEx(hkey, StringFromIDS(IDS_PLACEMENT), 0, REG_BINARY,
			      (LPBYTE)&wndpl, sizeof (wndpl));
		RegSetValueEx(hkey, StringFromIDS(IDS_MENUFLAGS), 0, REG_DWORD,
			     (LPBYTE)&_fMenuFlags, sizeof (_fMenuFlags));
		RegCloseKey(hkey);
	}
}

LRESULT CommandSelected(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	TCHAR  szBuffer[1024];
	UINT   nStringID = 0;
	UINT   fuFlags = GET_WM_MENUSELECT_FLAGS(wParam, lParam) & 0xffff;
	UINT   uCmd    = GET_WM_MENUSELECT_CMD(wParam, lParam);
	HMENU  hMenu   = GET_WM_MENUSELECT_HMENU(wParam, lParam);
	//int iPlugin;
	
	szBuffer[0] = 0;
	nStringID = 0;
	
	if (fuFlags == 0xffff && hMenu == NULL) {
		/* Menu has been closed */
		nStringID = IDS_READY;
	} else if (fuFlags & MFT_SEPARATOR) {
		/* Ignore separators */
		nStringID = 0;
	} else if (fuFlags & MF_POPUP) {
		/* Popup menu */
		/* System menu */
		if (fuFlags & MF_SYSMENU);
		nStringID = IDS_READY;
	} else {
		/* Must be a command item */
		nStringID = uCmd;   /* String ID == Command ID */
	}
	
	/* Load the string if we have an ID */
	if (nStringID != 0) {
		/*
		iPlugin = nStringID - IDM_PLUGIN_BASE;
		if (iPlugin >= 0 && iPlugin < qmcp_plugin_number)
		{
			strcpy(szBuffer, qmcp_plugin_list[iPlugin].hint);
		} else
		*/
		{
			LoadString(_hInstance, nStringID, szBuffer, CCHMAX(szBuffer));
		}
	}
	
	if (hWnd && GetDlgItem(hWnd, ID_STATUS)) {
		/* Finally... send the string to the status bar */
		DisplayStatus(GetDlgItem(hWnd, ID_STATUS), TEXT("%s"), szBuffer);
	}
	return 0;
}

void DestroyToolbar(HWND hwndParent)
{
	HIMAGELIST hImageList;
	HWND hwndToolbar;

	hwndToolbar = GetDlgItem(hwndParent, ID_TOOLBAR);
        hImageList = (HIMAGELIST)SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)NULL);
	if (hImageList) ImageList_Destroy(hImageList);
}

HWND BuildToolbar(HWND hwndParent)
{
	HWND hwndToolbar;
	TBBUTTON tbArray[TOOLBARNUM];
	HDC hDC;
	INT i, num_bits, bitmap_index;
	HBITMAP hBitmap;
	HIMAGELIST hImageList;
	
	hwndToolbar = CreateWindowEx(WS_EX_TOOLWINDOW,
				     TOOLBARCLASSNAME,
				     NULL,
				     WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | 
				     WS_CLIPSIBLINGS | CCS_NODIVIDER |
				     CCS_NOPARENTALIGN | CCS_NORESIZE | 
				     TBSTYLE_FLAT | TBSTYLE_TOOLTIPS, 
				     0, 0,
				     (TOOLBARNUM)*(CX_BITMAP+4),
				     CY_BITMAP+8, 
				     hwndParent, (HMENU)ID_TOOLBAR,
				     _hInstance, NULL);
	
	if (hwndToolbar) {
		/* sets the size of the TBBUTTON structure. */
		SendMessage(hwndToolbar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
		
		/* set the bitmap size. */
		SendMessage(hwndToolbar, TB_SETBITMAPSIZE,
			    0, (LPARAM)MAKELONG(CX_BITMAP, CY_BITMAP));
		
		/* set the button size. */
		SendMessage(hwndToolbar, TB_SETBUTTONSIZE,
			    0, (LPARAM)MAKELONG(CX_BITMAP, CY_BITMAP));
		
		/* get hImageList on basis of display capabilities */
		hDC = GetDC(NULL);		/* DC for desktop */
		num_bits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
		ReleaseDC(NULL, hDC);
		
		/*if (num_bits <= 8)*/
		{
			hImageList = ImageList_Create(CX_BITMAP, CY_BITMAP,
						      ILC_COLOR|ILC_MASK,
						      TOOLBARNUM, 0); 
			hBitmap = LoadBitmap(_hInstance, MAKEINTRESOURCE(IDB_TOOLBAR4BIT));
			ImageList_AddMasked(hImageList, hBitmap, RGB(255, 0, 255));
			DeleteObject(hBitmap);
		}
		/*
		else {
			hImageList = ImageList_Create(CX_BITMAP, CY_BITMAP, ILC_COLOR24|ILC_MASK, 
			TOOLBARNUM, 0); 
			hBitmap = LoadBitmap (_hInstance, MAKEINTRESOURCE(IDB_TOOLBAR24BIT));
			ImageList_AddMasked(hImageList, hBitmap, RGB(255, 0, 255));
			DeleteObject(hBitmap);
		}
		*/
		hImageList = (HIMAGELIST)SendMessage(hwndToolbar, TB_SETIMAGELIST, 0, (LPARAM)hImageList);
		if (hImageList) ImageList_Destroy(hImageList);
		
		/* Loop to fill the array of TBBUTTON structures. */
		bitmap_index = 0;
		for (i = 0; i < TOOLBARNUM; i++) {
			tbArray[i].iBitmap   = bitmap_index;
			tbArray[i].idCommand = 0;
			tbArray[i].fsStyle   = TBSTYLE_BUTTON;
			tbArray[i].dwData    = 0;
			tbArray[i].iString   = bitmap_index;
			if (ToolbarSeparator[i]) {
				tbArray[i].fsState = 0;
				tbArray[i].fsStyle = TBSTYLE_SEP;
			} else {
				tbArray[i].fsState = TBSTATE_ENABLED;
				tbArray[i].fsStyle = TBSTYLE_BUTTON;
				bitmap_index++;
			}
		}
		
		tbArray[IDX_EXITAPP].idCommand = ID_APP_EXIT;
		
		/* add the buttons */
		SendMessage(hwndToolbar, TB_ADDBUTTONS,
			    (UINT)TOOLBARNUM, (LPARAM)tbArray);
	}
	
	return hwndToolbar;
}

VOID GetToolbarText(HWND hwndToolbar, LPTOOLTIPTEXT lpttt)
{
	static TCHAR szText[256];
	
	lpttt->hinst = _hInstance;
	/*
	 * specify the resource identifier of the descriptive 
	 * text for the given button. 
	 */
	LoadString(NULL, lpttt->hdr.idFrom, szText, 256);

	lpttt->lpszText = szText;
}

HWND BuildRebar(HWND hwndParent)
{
	HWND hwndRebar = NULL;
	LRESULT lResult;
	HWND hwndChild;
	
	_dwRebarSide = TOP;
	
	hwndRebar = CreateWindowEx(WS_EX_TOOLWINDOW, 
				   REBARCLASSNAME, 
				   NULL,
				   WS_VISIBLE |
				   WS_BORDER | 
				   WS_CHILD | 
				   WS_CLIPCHILDREN | 
				   WS_CLIPSIBLINGS | 
				   RBS_VARHEIGHT | 
				   RBS_BANDBORDERS | 
				   ((_dwRebarSide & 0x01) ? CCS_VERT : 0) | //_dwRebarSide is odd if this is a vertical bar
				   ((_dwRebarSide == BOTTOM) ? CCS_BOTTOM : 0) |
				   ((_dwRebarSide == RIGHT) ? CCS_RIGHT : 0) |
				   0,
				   0, 
				   0, 
				   200, 
				   32, 
				   hwndParent, 
				   (HMENU)ID_REBAR, 
				   _hInstance, 
				   NULL);
	
	if (hwndRebar) {
		REBARBANDINFO  rbbi;
		RECT           rc;
		
		hwndChild = BuildToolbar(hwndRebar);
		
		GetWindowRect(hwndChild, &rc);
		
		ZeroMemory(&rbbi, sizeof (rbbi));
		rbbi.cbSize       = sizeof (REBARBANDINFO);
		rbbi.fMask        = RBBIM_SIZE | 
				    RBBIM_CHILD | 
				    RBBIM_CHILDSIZE | 
				    RBBIM_ID | 
				    RBBIM_STYLE | 
				    RBBIM_TEXT |
				    RBBIM_BACKGROUND |
				    0;
		rbbi.cxMinChild   = rc.right - rc.left+4;
		rbbi.cyMinChild   = rc.bottom - rc.top;
		rbbi.cx           = 100;
		rbbi.fStyle       = RBBS_CHILDEDGE | 
				    RBBS_FIXEDBMP |
				    RBBS_GRIPPERALWAYS |
				    0;
		rbbi.wID          = ID_TOOLBAR;
		rbbi.hwndChild    = hwndChild;
		rbbi.lpText       = NULL;
		
		lResult = SendMessage(hwndRebar, RB_INSERTBAND,
				      (WPARAM)-1, (LPARAM)(LPREBARBANDINFO)&rbbi);
	}
	
	return hwndChild;
}

void MoveRebar(HWND hWnd, RECT *lpRect)
{
	RECT rcRebar;
	int x, y, cx, cy;
	
	//if (!_bShowRebar) return;
	
	if (!(_fMenuFlags & VIEW_TOOLBAR))
		return;
	
	GetWindowRect(GetDlgItem(hWnd, ID_REBAR), &rcRebar);
	
	switch (_dwRebarSide) {
	default:
	case TOP:
		//align the rebar along the top of the window
		x = lpRect->left;
		y = lpRect->top;
		cx = lpRect->right - lpRect->left;
		cy = rcRebar.bottom - rcRebar.top;
		
		lpRect->top = y + rcRebar.bottom - rcRebar.top;
		break;
		
	case LEFT:
		//align the rebar along the left side of the window
		x = lpRect->left;
		y = lpRect->top;
		cx = rcRebar.right - rcRebar.left;
		cy = lpRect->bottom - lpRect->top;
		
		lpRect->left = x + rcRebar.right - rcRebar.left;
		break;
		
	case BOTTOM:
		//align the rebar along the bottom of the window
		x = lpRect->left;
		y = lpRect->bottom - (rcRebar.bottom - rcRebar.top);
		cx = lpRect->right - lpRect->left;
		cy = rcRebar.bottom - rcRebar.top;
		
		lpRect->bottom = lpRect->bottom - (rcRebar.bottom - rcRebar.top);
		break;
		
	case RIGHT:
		//align the coolbar along the right side of the window
		x = lpRect->right - (rcRebar.right - rcRebar.left);
		y = lpRect->top;
		cx = rcRebar.right - rcRebar.left;
		cy = lpRect->bottom - lpRect->top;
		
		lpRect->right = lpRect->right - (rcRebar.right - rcRebar.left);
		break;
	}
	
	MoveWindow(GetDlgItem(hWnd, ID_REBAR), x, y, cx, cy, TRUE);
}

HWND BuildStatus(HWND hwndParent)
{
	HWND hwndStatus = NULL;
	RECT rc;
	DWORD dwStyle;
	HLOCAL hLocal;
	LPINT lpParts;
	
	dwStyle = WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | WS_GROUP;
	/* Create the status bar. */
	hwndStatus = CreateWindowEx(0,                      /* no extended styles */
				    STATUSCLASSNAME,        /* name of status bar class */
				    (LPCTSTR) NULL,         /* no text when first created */
				    WS_VISIBLE |            /* visible */
				    WS_CHILD,               /* creates a child window */
				    0, 0, 0, 0,             /* ignores size and position */
				    hwndParent,             /* handle to parent window */
				    (HMENU)ID_STATUS,       /* child window identifier */
				    _hInstance,             /* handle to application instance */
				    NULL);                  /* no window creation data */
	
	/* Get the coordinates of the parent window's client area. */
	GetClientRect(hwndParent, &rc); 
	
	/* Allocate an array for holding the right edge coordinates. */
	hLocal = LocalAlloc(LHND, sizeof (INT) * STATUSNUM); 
	lpParts = LocalLock(hLocal);
	
	/* Calculate the right edge coordinate for each part, and
	 * copy the coordinates to the array.
	 */
	lpParts[1] = rc.right - 16;
	lpParts[0] = rc.right - 46;
	
	/* Tell the status bar to create the window parts. */
	SendMessage(hwndStatus, SB_SETPARTS, (WPARAM)STATUSNUM, (LPARAM)lpParts);
	
	/* Free the array, and return. */
	LocalUnlock(hLocal);
	LocalFree(hLocal);
	
	return hwndStatus;
}

VOID DisplayStatus(HWND hwndStatus, LPCTSTR lpszFormat, ...)
{
	TCHAR szBuf[1024];
	unsigned int nSize;
	va_list ap;
	va_start(ap, lpszFormat);
	nSize = _vsntprintf(szBuf, sizeof (szBuf), lpszFormat, ap);
	va_end(ap);
	
	SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)szBuf);
}

void MoveStatus(HWND hWnd, RECT *lpRect)
{
	RECT rcStatus;
	int x, y, cx, cy;
	HLOCAL hloc;
	LPINT lpParts;
	
	if (!(_fMenuFlags & VIEW_STATUSBAR))
		return;
	
	GetWindowRect(GetDlgItem(hWnd, ID_STATUS), &rcStatus);
	
	//align the rebar along the top of the window
	x = lpRect->left;
	y = lpRect->bottom - (rcStatus.bottom-rcStatus.top);
	cx = lpRect->right - lpRect->left;
	cy = rcStatus.bottom-rcStatus.top;
	
	MoveWindow(GetDlgItem(hWnd, ID_STATUS), x, y, cx, cy, TRUE);
	
	lpRect->bottom = (y>lpRect->top?y:lpRect->top);
	/* Allocate an array for holding the right edge coordinates. */
	hloc = LocalAlloc(LHND, sizeof (int) * STATUSNUM); 
	lpParts = LocalLock(hloc); 
	
	/* Calculate the right edge coordinate for each part, and
	* copy the coordinates to the array.
	*/
	lpParts[1] = lpRect->right - 16;
	lpParts[0] = lpRect->right - 46;
	
	/* Tell the status bar to create the window parts. */
	SendMessage(GetDlgItem(hWnd, ID_STATUS), SB_SETPARTS,
                    (WPARAM)STATUSNUM, (LPARAM)lpParts);
	
	//    XPStatu_Paint(status->handle, QMCP_STATUS_NUM);
	/* Free the array, and return. */
	LocalUnlock(hloc);
	LocalFree(hloc);
}

void WINAPI CenterChild(HWND hwndChild, HWND hwndParent)
{
	int   xNew, yNew;
	int   cxChild, cyChild;
	int   cxParent, cyParent;
	int   cxScreen, cyScreen;
	RECT  rcChild, rcParent;
	HDC   hdc;
	/* Get the Height and Width of the child window */
	GetWindowRect(hwndChild, &rcChild);
	cxChild = rcChild.right - rcChild.left;
	cyChild = rcChild.bottom - rcChild.top;
	/* Get the Height and Width of the parent window */
	GetWindowRect(hwndParent, &rcParent);
	cxParent = rcParent.right - rcParent.left;
	cyParent = rcParent.bottom - rcParent.top;
	/* Get the display limits */
	hdc = GetDC(hwndChild);
	if (hdc == NULL) {
		/* major problems - move window to 0,0 */
		xNew = yNew = 0;
	} else {
		cxScreen = GetDeviceCaps(hdc, HORZRES);
		cyScreen = GetDeviceCaps(hdc, VERTRES);
		ReleaseDC(hwndChild, hdc);
		if (hwndParent == NULL) {
			cxParent = cxScreen;
			cyParent = cyScreen;
			SetRect(&rcParent, 0, 0, cxScreen, cyScreen);
		}
		/* Calculate new X position, then adjust for screen */
		xNew = rcParent.left + ((cxParent - cxChild) / 2);
		if (xNew < 0) {
			xNew = 0;
		} else if ((xNew + cxChild) > cxScreen) {
			xNew = cxScreen - cxChild; 
		}
		/* Calculate new Y position, then adjust for screen */
		yNew = rcParent.top  + ((cyParent - cyChild) / 2);
		if (yNew < 0) {
			yNew = 0;
		} else if ((yNew + cyChild) > cyScreen) {
			yNew = cyScreen - cyChild;
		}  
	}
	SetWindowPos(hwndChild, NULL, xNew, yNew, 0, 0,
		     SWP_NOSIZE | SWP_NOZORDER); 
}
