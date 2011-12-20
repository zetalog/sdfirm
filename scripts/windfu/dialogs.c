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
 * @(#)dialogs.c: window common dialogs function
 * $Id: dialogs.c,v 1.5 2011-07-26 10:11:27 zhenglv Exp $
 */

#include "windfu.h"

static LRESULT WINAPI DlgWizardDoneProc(HWND hWnd, UINT uMsg,
					WPARAM wParam, LPARAM lParam);
static LRESULT WINAPI DlgWizardIntroProc(HWND hWnd, UINT uMsg,
					 WPARAM wParam, LPARAM lParam);
static void DlgDestroyWizard(LPWINWIZARDINFO wi);

#define MAX_MESSAGE     1024
#define MAX_CAPTION     64
#define MAX_IDS         200

int MessageIDBox(HWND hWnd, UINT uMessage, UINT uCaption, UINT uType)
{
	TCHAR szText[MAX_MESSAGE];
	TCHAR szCaption[MAX_CAPTION];
	
	LoadString(_hInstance, uMessage, szText, MAX_MESSAGE);
	LoadString(_hInstance, uCaption, szCaption, MAX_CAPTION);
	
	return MessageBox(hWnd, szText, szCaption, MB_ICONQUESTION | uType);
}

HPALETTE CreateDIBPalette(LPBITMAPINFO lpbmi, LPINT lpiNumColors)
{
	LPBITMAPINFOHEADER	lpbi;
	LPLOGPALETTE		lpPal;
	HANDLE			hLogPal;
	HPALETTE		hPal = NULL;
	INT			i;

	lpbi = (LPBITMAPINFOHEADER)lpbmi;
	if (lpbi->biBitCount <= 8)
		*lpiNumColors = (1 << lpbi->biBitCount);
	else
		*lpiNumColors = 0;  // No palette needed for 24 BPP DIB

	if (*lpiNumColors) {
		hLogPal = GlobalAlloc (GHND, sizeof (LOGPALETTE) +
				sizeof (PALETTEENTRY) * (*lpiNumColors));
		lpPal = (LPLOGPALETTE) GlobalLock (hLogPal);
		lpPal->palVersion = 0x300;
		lpPal->palNumEntries = *lpiNumColors;

		for (i = 0;  i < *lpiNumColors;  i++) {
			lpPal->palPalEntry[i].peRed   = lpbmi->bmiColors[i].rgbRed;
			lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
			lpPal->palPalEntry[i].peBlue  = lpbmi->bmiColors[i].rgbBlue;
			lpPal->palPalEntry[i].peFlags = 0;
		}
		hPal = CreatePalette (lpPal);
		GlobalUnlock (hLogPal);
		GlobalFree (hLogPal);
	}
	return hPal;
}

HBITMAP LoadResourceBitmap(HINSTANCE hInstance, LPSTR lpString,
                           HPALETTE FAR *lphPalette)
{
	HRSRC			hRsrc;
	HGLOBAL			hGlobal;
	HBITMAP			hBitmapFinal = NULL;
	LPBITMAPINFOHEADER	lpbi;
	HDC			hdc;
	INT			iNumColors;

	if (hRsrc = FindResource (hInstance, lpString, RT_BITMAP)) {
		hGlobal = LoadResource (hInstance, hRsrc);
		lpbi = (LPBITMAPINFOHEADER)LockResource (hGlobal);

		hdc = GetDC(NULL);
		*lphPalette = CreateDIBPalette((LPBITMAPINFO)lpbi, &iNumColors);
		if (*lphPalette) {
			SelectPalette (hdc,*lphPalette,FALSE);
			RealizePalette (hdc);
		}

		hBitmapFinal = CreateDIBitmap (hdc,
                   (LPBITMAPINFOHEADER)lpbi,
                   (LONG)CBM_INIT,
                   (LPSTR)lpbi + lpbi->biSize + iNumColors * sizeof(RGBQUAD),
                   (LPBITMAPINFO)lpbi,
                   DIB_RGB_COLORS );

		ReleaseDC (NULL,hdc);
		UnlockResource (hGlobal);
		FreeResource (hGlobal);
	}
	return (hBitmapFinal);
}

//  set font of heading text
HFONT LoadHeadingFont(HINSTANCE hInstance, BOOL interior, HDC hdc)
{
	LOGFONT	lf;
	
	memset(&lf, 0, sizeof(lf));
	
	if (interior) {
		lstrcpy(lf.lfFaceName, TEXT("MS San Serif"));
		//LoadString(hInstance, IDS_INTERIORWIZFONT, lf.lfFaceName, sizeof(lf.lfFaceName));
		lf.lfHeight = -MulDiv(9, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	} else {
		lstrcpy(lf.lfFaceName, TEXT("MS San Serif"));
		//LoadString(hInstance, IDS_INTROWIZFONT, lf.lfFaceName, sizeof(lf.lfFaceName));
		lf.lfHeight = -MulDiv(14, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		lf.lfWeight = FW_BOLD;
	}
	
	return (CreateFontIndirect(&lf));
}

LRESULT WINAPI DlgWizardDoneProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL result = FALSE;
	LPWINWIZARDINFO lpwi;
	LPNMHDR pnmh;

	if (uMsg == WM_INITDIALOG) {
		PROPSHEETPAGE *ppspMsgRec;
		ppspMsgRec = (PROPSHEETPAGE *)lParam;
		lpwi = (LPWINWIZARDINFO)ppspMsgRec->lParam;
		SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)lpwi);
	} else {
		lpwi = (LPWINWIZARDINFO)GetWindowLong(hWnd, GWL_USERDATA);
	}

	switch (uMsg) {
	case WM_INITDIALOG:
		result = TRUE;
		break;
	case WM_NOTIFY:
		pnmh = (LPNMHDR)lParam;
		switch (pnmh->code) {
		case PSN_SETACTIVE:
			SendMessage(lpwi->hwndWizard, PSM_SETWIZBUTTONS,
				    0, PSWIZB_BACK | PSWIZB_FINISH);
			SendDlgItemMessage(hWnd, IDC_WIZBITMAP, STM_SETIMAGE,
					   IMAGE_BITMAP, (LPARAM) lpwi->hBitmap);
			return 0;
		case PSN_WIZFINISH:
			SetWindowLong(hWnd, DWL_MSGRESULT, lpwi->nResult);
			return FALSE;
		case PSN_QUERYCANCEL:
			lpwi->nResult = FALSE;
			SetWindowLong(hWnd, DWL_MSGRESULT, lpwi->nResult);
			return FALSE;
		}
		break;
	}
	
	return result;
}

LRESULT WINAPI DlgWizardIntroProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL result = FALSE;
	LPWINWIZARDINFO lpwi;
	LPNMHDR pnmh;

	if (uMsg == WM_INITDIALOG) {
		PROPSHEETPAGE *ppspMsgRec;
		ppspMsgRec = (PROPSHEETPAGE *)lParam;
		lpwi = (LPWINWIZARDINFO)ppspMsgRec->lParam;
		SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)lpwi);
	} else {
		lpwi = (LPWINWIZARDINFO)GetWindowLong(hWnd, GWL_USERDATA);
	}
	
	switch (uMsg) {
	case WM_INITDIALOG:
		lpwi->hwndWizard = GetParent(hWnd);
		// center dialog on screen
		CenterChild(lpwi->hwndWizard, lpwi->hwndParent);
		lpwi->nResult = FALSE;
		SetWindowLong(hWnd, DWL_MSGRESULT, lpwi->nResult);
		SetDlgItemText(hWnd, IDC_INTRODUCTION, lpwi->lpszIntro);
		result = TRUE;
		break;
	case WM_NOTIFY:
		pnmh = (LPNMHDR) lParam;
		switch (pnmh->code) {
		case PSN_SETACTIVE:
			SendMessage(lpwi->hwndWizard, PSM_SETWIZBUTTONS, 0, PSWIZB_NEXT);
			SendDlgItemMessage(hWnd, IDC_WIZBITMAP, STM_SETIMAGE,
					   IMAGE_BITMAP, (LPARAM) lpwi->hBitmap);
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

void WINAPI DlgPaintWizard(HWND hWnd, LPWINWIZARDINFO wi)
{
	if (wi->hPalette) {
		PAINTSTRUCT ps;
		HDC hDC = BeginPaint(hWnd, &ps);
		SelectPalette(hDC, wi->hPalette, FALSE);
		RealizePalette(hDC);
		EndPaint(hWnd, &ps);
	}
}

void DlgDestroyWizard(LPWINWIZARDINFO wi)
{
	if (wi) {
		DeleteObject(wi->hBitmap);
		DeleteObject(wi->hfontIntro);
		DestroyIcon(wi->hIcon);
	}
}

#define DLG_WIZARD_INTRO	0
#define DLG_WIZARD_CUSTOM	1

int WINAPI DlgLaunchWizard(HWND hwndParent, UINT nIntro, UINT nIcon, 
			   int iModPages, LPWINPROPERTYPAGE lpPages, ULONG lParam)
{
	WINWIZARDINFO	wi;
	LPPROPSHEETPAGE	pspWiz;
	PROPSHEETHEADER	pshWiz;
	INT		index;
	INT		iNumBits, iBitmap;
	HDC		hDC;
	TCHAR		szIntro[2048];
	INT		iWizardPages = 0;
	int		nResult;
	
	iWizardPages = iModPages+2;
	
	pspWiz = (LPPROPSHEETPAGE)malloc(iWizardPages * sizeof (PROPSHEETPAGE));
	if (!pspWiz) return FALSE;
	
	memset(&wi, 0, sizeof (WINWIZARDINFO));
	wi.hIcon = (HICON)LoadImage(_hInstance, MAKEINTRESOURCE(nIcon),
		IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
	LoadString(_hInstance, nIntro, szIntro, 2048);
	wi.lpszIntro = szIntro;
	wi.hwndParent = hwndParent;
	
	// Determine which bitmap will be displayed in the wizard
	hDC = GetDC(NULL);	 // DC for desktop
	iNumBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	if (iNumBits <= 1)
		iBitmap = IDB_WIZARD;
	else if (iNumBits <= 4)
		iBitmap = IDB_WIZARD;
	else
		iBitmap = IDB_WIZARD;
	
	wi.hBitmap = LoadResourceBitmap(_hInstance, MAKEINTRESOURCE(iBitmap), &wi.hPalette);
	wi.hfontIntro = LoadHeadingFont(_hInstance, FALSE, hDC);
	ReleaseDC(NULL, hDC);
	
	wi.lParam = lParam;
	
	/* Set the values common to all pages */
	for (index = 0; index < iWizardPages; index++) {
		pspWiz[index].dwSize      = sizeof (PROPSHEETPAGE);
		pspWiz[index].dwFlags     = PSP_DEFAULT;
		pspWiz[index].hInstance   = _hInstance;
		pspWiz[index].pszTemplate = NULL;
		pspWiz[index].hIcon       = NULL;
		pspWiz[index].pfnDlgProc  = NULL;
		pspWiz[index].lParam      = (LPARAM)&wi;
		pspWiz[index].pfnCallback = NULL;
		pspWiz[index].pcRefParent = NULL;
	}
	// Set up the intro page
	pspWiz[0].pszTemplate = MAKEINTRESOURCE(IDD_WIZARDINTRO);
	pspWiz[0].pfnDlgProc = (DLGPROC)DlgWizardIntroProc;
	
	// Set module dependent pages here
	for (index = 0; index < iModPages; index++) {
		LPCDLGTEMPLATE pResource;
		HRSRC hResInfo;
		
		pspWiz[index+DLG_WIZARD_CUSTOM].dwFlags |= PSP_DLGINDIRECT;
		hResInfo = FindResource(_hInstance,
			MAKEINTRESOURCE(lpPages[index].nPageID),
			RT_DIALOG);
		assert(hResInfo);
		pResource = (LPCDLGTEMPLATE)LoadResource(_hInstance, hResInfo);
		assert(pResource);
		pspWiz[index+DLG_WIZARD_CUSTOM].pResource = pResource;
		pspWiz[index+DLG_WIZARD_CUSTOM].pfnDlgProc = lpPages[index].lpfnPageProc;
	}
	
	// Set up the done page
	pspWiz[iModPages+DLG_WIZARD_CUSTOM].pszTemplate = MAKEINTRESOURCE(IDD_WIZARDDONE);
	pspWiz[iModPages+DLG_WIZARD_CUSTOM].pfnDlgProc = (DLGPROC)DlgWizardDoneProc;
	
	// Create the header
	pshWiz.dwSize		= sizeof (PROPSHEETHEADER);
	pshWiz.dwFlags		= PSH_WIZARD | PSH_PROPSHEETPAGE | PSH_USEHICON;
	pshWiz.hwndParent	= hwndParent;
	pshWiz.hInstance	= _hInstance;
	pshWiz.hIcon		= wi.hIcon;
	pshWiz.pszCaption	= NULL;
	pshWiz.nPages		= iWizardPages;
	pshWiz.nStartPage	= DLG_WIZARD_INTRO;
	pshWiz.ppsp		= pspWiz;
	pshWiz.pfnCallback	= NULL;
	pshWiz.pszbmWatermark   = MAKEINTRESOURCE(IDB_WIZARD);
	pshWiz.pszbmHeader      = MAKEINTRESOURCE(IDB_HEADER);
	
	nResult = PropertySheet(&pshWiz);
	
	DlgDestroyWizard(&wi);
	free(pspWiz);
	return nResult;
}

LPSTR DlgRefineFile(LPSTR pszFile, UINT nSize)
{
	char *s, *d;
	LPSTR pszRes = NULL;
	int skip = 0;

	if (pszFile) {
		pszRes = calloc(nSize, 1);
		for (s = pszFile, d = pszRes; s && *s; s++) {
			if (skip) {
				if (*s != '\\') {
					skip = 0;
					*d++ = *s;
				}
			} else if (*s == '\\') {
				if (!skip) {
					*d++ = *s;
					skip = 1;
				}
			} else {
				*d++ = *s;
			}
		}
	}
	return pszRes;
}

VOID WINAPI DlgDisplayError(HWND hwndParent, UINT uCaption, DWORD dwError)
{
	TCHAR msg[MAX_MESSAGE+1] = "";
	TCHAR cap[MAX_CAPTION+1];

	LoadString(_hInstance, uCaption, cap, MAX_CAPTION);
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
		       NULL, dwError,
		       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		       msg, MAX_MESSAGE, NULL);
	MessageBox(hwndParent, msg, cap, MB_ICONERROR | MB_OK);
}

BOOL WINAPI DlgBrowseFile(HWND hwndParent, LPSTR pszFile, UINT nSize,
			  LPSTR *pszName, LPSTR *pszSufix, UINT nSufix,
			  BOOL bSave)
{
	OPENFILENAME ofn;
	UINT nFilters = 5+nSufix;
	UINT nIndex;
	LPSTR pszFilter = NULL;
	LPSTR pszFileTemp = NULL;
	LPSTR pszTmp;
	BOOL bResult;
	
	for (nIndex = 0; nIndex < nSufix; nIndex++) {
		nFilters += (strlen(pszName[nIndex])+1);
		nFilters += (strlen(pszSufix[nIndex])+1);
	}
	
	pszFilter = (LPTSTR)malloc(nFilters*sizeof (CHAR));
	memset(pszFilter, 0, nFilters*sizeof (CHAR));
	pszTmp = pszFilter;
	
	for (nIndex = 0; nIndex < nSufix; nIndex++) {
		strcpy(pszTmp, pszName[nIndex]);
		pszTmp += strlen(pszName[nIndex])+1;
		strcpy(pszTmp, pszSufix[nIndex]);
		pszTmp += strlen(pszSufix[nIndex])+1;
	}
	
	pszFileTemp = DlgRefineFile(pszFile, nSize);

	ofn.lStructSize         = sizeof (OPENFILENAME);
	ofn.hwndOwner           = hwndParent;
	ofn.hInstance           = _hInstance;
	ofn.lpstrFilter         = pszFilter;
	ofn.lpstrCustomFilter   = NULL;
	ofn.nMaxCustFilter      = 1;
	ofn.nFilterIndex        = 1;
	ofn.lpstrFile           = pszFileTemp;
	ofn.nMaxFile            = nSize;
	ofn.lpstrFileTitle      = NULL;
	ofn.nMaxFileTitle       = 0;
	ofn.lpstrInitialDir     = NULL;
	ofn.lpstrTitle          = "Browse File";
	ofn.nFileOffset         = 0;
	ofn.nFileExtension      = 0;
	ofn.lpstrDefExt         = NULL;
	ofn.lCustData           = 0;
	ofn.lpfnHook            = NULL;
	ofn.lpTemplateName      = NULL;
	ofn.Flags               = OFN_SHOWHELP | OFN_EXPLORER;
	
	/* call the common dialog function. */
	if (bSave)
		bResult = GetSaveFileName(&ofn);
	else
		bResult = GetOpenFileName(&ofn);

	if (bResult) {
		strcpy(pszFile, ofn.lpstrFile);
	} else {
		DWORD dwError = CommDlgExtendedError();

		if (dwError)
			MessageIDBox(hwndParent, IDS_ERROR_FILE_BROWSE, IDS_ERROR, MB_OK);
	}

	free(pszFilter);
	if (pszFileTemp) free(pszFileTemp);
	return bResult;
}

typedef struct tagWINPROGRESSDLG {
	HWND hwndParent;
	HWND hwndDialog;
	HWND hwndProgress;
	HWND hwndStatus;

	WINPROGRESSCB pfnProgress;
	VOID *pData;

	UINT nSteps;
	BOOL bCancel;

	HANDLE hThread;
	ULONG ulThreadID;
} WINPROGRESSDLG, *LPWINPROGRESSDLG;

#define WM_PROGRESS	WM_USER+1

DWORD WINAPI DlgShowProgressThread(LPWINPROGRESSDLG lppd)
{
	UINT nStep;
	
	assert(lppd);
	for (nStep = 0; nStep < lppd->nSteps; nStep++) {
		if (lppd->bCancel)
			break;
		
		if (lppd->pfnProgress(lppd, lppd->hwndParent, nStep, lppd->pData) == FALSE)
			break;
		
		/* trick: slow down the progress bar */
		PostMessage(lppd->hwndDialog, WM_PROGRESS, nStep, FALSE);
		Sleep(20);
	}
	
	PostMessage(lppd->hwndDialog, WM_PROGRESS, nStep, TRUE);
	free(lppd);
	return 0L;
}

VOID WINAPI DlgShowStatus(LPWINPROGRESSDLG lppd, LPTSTR szMessage)
{
	SetWindowText(GetDlgItem(lppd->hwndDialog, IDC_MESSAGE), szMessage);
}

LRESULT WINAPI DlgProgressDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL result = FALSE;
	LPWINPROGRESSDLG lppd = (LPWINPROGRESSDLG)GetWindowLong(hWnd, GWL_USERDATA);
	BOOL bEnd;
	UINT nStep;
	
	switch (uMsg) {
	case WM_INITDIALOG:
		lppd = (LPWINPROGRESSDLG)lParam;
		SetWindowLong(hWnd, GWL_USERDATA, (LPARAM)lppd);
		
		lppd->hwndStatus = GetDlgItem(hWnd, IDC_MESSAGE);
		lppd->hwndProgress = GetDlgItem(hWnd, IDC_PROGRESS);
		
		lppd->hwndDialog = hWnd;
		SendMessage(lppd->hwndProgress, PBM_SETRANGE, 0, MAKELPARAM(0, lppd->nSteps));
		SendMessage(lppd->hwndProgress, PBM_SETPOS, 0, 0);
		
		// center dialog on screen
		CenterChild(lppd->hwndDialog, lppd->hwndParent);
		lppd->hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DlgShowProgressThread,
					     lppd, 0, &(lppd->ulThreadID));
		return TRUE;
		
	case WM_PROGRESS:
		bEnd = (BOOL)lParam;
		nStep = (UINT)wParam;
		
		SendMessage(lppd->hwndProgress, PBM_SETPOS, nStep, 0);
		if (bEnd) {
			if (nStep < lppd->nSteps)
				EndDialog(hWnd, IDCANCEL);
			else
				EndDialog(hWnd, IDOK);
			break;
		}
		break;
		
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDCANCEL:
			lppd->bCancel = TRUE;
			EndDialog(hWnd, IDCANCEL);
			break;
		}
		break;
	}
	return result;
}

BOOL WINAPI DlgShowProgress(HWND hWnd, UINT nSteps, WINPROGRESSCB pfnProgress, LPVOID pData)
{
	UINT nID;
	WINPROGRESSDLG *pDlg;
	
	pDlg = malloc(sizeof (WINPROGRESSDLG));
	if (!pDlg) return FALSE;
	
	memset(pDlg, 0, sizeof (WINPROGRESSDLG));
	pDlg->hwndParent = hWnd;
	pDlg->nSteps = nSteps;
	pDlg->pfnProgress = pfnProgress;
	pDlg->pData = pData;
	pDlg->bCancel = FALSE;
	
	nID = DialogBoxParam(_hInstance, MAKEINTRESOURCE(IDD_PROGRESS), hWnd,
			     (DLGPROC)DlgProgressDialogProc, (LPARAM)pDlg);
	
	if (nID == IDCANCEL)
		return FALSE;
	return TRUE;
}
