/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2003
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
 * @(#)SPLAttrib.c: handle splitter attributes
 * $Id: SPLAttrib.c,v 1.2 2007-07-03 01:48:33 zhenglv Exp $
 */

#include "SPLIntern.h"
#include <malloc.h>
#include <memory.h>

// Private memory allocation routine
void* SPLAlloc(long size) 
{
	void* p;

	p = malloc(size);
	if (!p)
		MessageBox(NULL, TEXT("Memory allocation failure"), TEXT("Splitter Error"), MB_OK);
	else
		memset(p, 0, size);

	return p;
}

// Private memory reallocation routine
void* SPLReAlloc(void *p, long size) 
{
	if (p) free(p);
	
	return (SPLAlloc(size));
}


// Private memory deallocation routine
void SPLFree(void *p) 
{
	if (p) free(p);
}

void SPLGetRowInfo(LPSPLWNDDATA lpWD, int row, int *cyCur, int *cyMin)
{
	assert(lpWD && lpWD->hWnd);
	assert(row >= 0 && row < lpWD->nMaxRows);

	*cyCur = lpWD->lpRowInfo[row].nCurSize;
	*cyMin = lpWD->lpRowInfo[row].nMinSize;
}

void SPLSetRowInfo(LPSPLWNDDATA lpWD, int row, int cyIdeal, int cyMin)
{
	assert(lpWD && lpWD->hWnd);
	assert(row >= 0 && row < lpWD->nMaxRows);
	assert(cyIdeal >= 0);
	assert(cyMin >= 0);

	lpWD->lpRowInfo[row].nIdealSize = cyIdeal;
	lpWD->lpRowInfo[row].nMinSize = cyMin;
}

void SPLGetColumnInfo(LPSPLWNDDATA lpWD, int col, int *cxCur, int *cxMin)
{
	assert(lpWD && lpWD->hWnd);
	assert(col >= 0 && col < lpWD->nMaxCols);

	*cxCur = lpWD->lpColInfo[col].nCurSize;
	*cxMin = lpWD->lpColInfo[col].nMinSize;
}

void SPLSetColumnInfo(LPSPLWNDDATA lpWD, int col, int cxIdeal, int cxMin)
{
	assert(lpWD && lpWD->hWnd);
	assert(col >= 0 && col < lpWD->nMaxCols);
	assert(cxIdeal >= 0);
	assert(cxMin >= 0);

	lpWD->lpColInfo[col].nIdealSize = cxIdeal;
	lpWD->lpColInfo[col].nMinSize = cxMin;
}

HWND SPLGetPane(LPSPLWNDDATA lpWD, int row, int col)
{
	HWND hwndView;
	
	assert(lpWD && lpWD->hWnd);
	
	hwndView = GetDlgItem(lpWD->hWnd, SPLIdFromRowCol(lpWD, row, col));
	return hwndView;
}

BOOL SPLIsChildPane(LPSPLWNDDATA lpWD, HWND hwndView, LPSPLPANEINFO lppi)
{
	UINT nID;
	
	assert(lpWD && lpWD->hWnd && hwndView);
	
	nID = GetDlgCtrlID(hwndView);
	if (IsChild(lpWD->hWnd, hwndView) && nID >= EC_IDW_PANE_FIRST && nID <= EC_IDW_PANE_LAST)
	{
		if (lppi)
		{
			lppi->row = (nID - EC_IDW_PANE_FIRST) / 16;
			lppi->col = (nID - EC_IDW_PANE_FIRST) % 16;
			assert(lppi->row >= lpWD->nRows);
			assert(lppi->col >= lpWD->nCols);
		}
		return TRUE;
	}
	else
	{
		if (lppi)
		{
			lppi->row = -1;
			lppi->col = -1;
		}
		return FALSE;
	}
}

void SPLSetActivePane(LPSPLWNDDATA lpWD, int row, int col, HWND hWnd)
{
	// set the focus to the pane
	HWND hwndPane = hWnd == NULL ? SPLGetPane(lpWD, row, col) : hWnd;
	DebugPrint("Warning: Next pane is not a view - calling SetFocus.");
	SetFocus(hwndPane);
}

HWND SPLGetActivePane(LPSPLWNDDATA lpWD, LPSPLPANEINFO lppi)
{
	HWND hwndView = NULL;
	
	assert(lpWD && lpWD->hWnd);
	
	// failing that, use the current focus
	if (hwndView == NULL)
		hwndView = GetFocus();
	
	// make sure the pane is a child pane of the splitter
	if (hwndView != NULL && !SPLIsChildPane(lpWD, hwndView, lppi))
		hwndView = NULL;
	
	return hwndView;
}

void SPLActivateNext(LPSPLWNDDATA lpWD, BOOL bPrev)
{
	int row, col;
	SPLPANEINFO sppi;
	assert(lpWD && lpWD->hWnd);
	
	// find the coordinate of the current pane
	if (SPLGetActivePane(lpWD, &sppi) == NULL)
	{
		DebugPrint("Warning: Cannot go to next pane - there is no current view.");
		return;
	}
	row = sppi.row;
	col = sppi.col;
	assert(row >= 0 && row < lpWD->nRows);
	assert(col >= 0 && col < lpWD->nCols);
	
	// determine next pane
	if (bPrev)
	{
		// prev
		if (--col < 0)
		{
			col = lpWD->nCols - 1;
			if (--row < 0)
				row = lpWD->nRows - 1;
		}
	}
	else
	{
		// next
		if (++col >= lpWD->nCols)
		{
			col = 0;
			if (++row >= lpWD->nRows)
				row = 0;
		}
	}
	
	// set newly active pane
	SPLSetActivePane(lpWD, row, col, NULL);
}

BOOL SPLCreateStatic(LPSPLWNDDATA lpWD, LPSPLCREATESTRUCT lpcs)
{
	assert(lpcs->hwndParent != NULL);
	assert(lpcs->nRows >= 1 && lpcs->nRows <= 16);
	assert(lpcs->nCols >= 1 || lpcs->nCols <= 16);
	// 1x1 is not permitted
	assert(lpcs->nCols > 1 || lpcs->nRows > 1);
	assert(lpcs->dwStyle & WS_CHILD);
	
	// none yet
	assert(lpWD->nRows == 0 || lpWD->nCols == 0);
	lpWD->nRows = lpWD->nMaxRows = lpcs->nRows;
	lpWD->nCols = lpWD->nMaxCols = lpcs->nCols;
	
	// create with zero minimum pane size
	if (!SPLCreateCommon(lpWD, lpcs->lpszClass, lpcs->hwndParent,
			     lpcs->hInstance, 0, 0, lpcs->dwStyle, lpcs->nID))
		return FALSE;
	
	// all panes must be created with explicit calls to CreateView
	return TRUE;
}

BOOL SPLCreateCommon(LPSPLWNDDATA lpWD, LPCTSTR lpszClassName,
		     HWND hwndParentWnd, HINSTANCE hInstance,
		     UINT cx, UINT cy, DWORD dwStyle, UINT nID)
{
	INT col, row;
	
	assert(hwndParentWnd != NULL);
	assert(cx >= 0 || cy >= 0);
	assert(dwStyle & WS_CHILD);
	assert(nID != 0);
	
	// only do once
	assert(lpWD->lpColInfo == NULL && lpWD->lpRowInfo == NULL);
	assert(lpWD->nMaxCols > 0 || lpWD->nMaxRows > 0);
	
	if (!dwStyle)
		dwStyle |= WS_CHILD | WS_VISIBLE;
	
	// the Windows scroll bar styles bits turn on the smart scrollbars
	ModifyStyle(lpWD->hWnd, WS_HSCROLL|WS_VSCROLL|WS_BORDER, dwStyle, 0);
	// create with the same wnd-class as MDI-Frame (no erase bkgnd)
	
	// attach the initial splitter parts
	lpWD->lpColInfo = SPLAlloc(sizeof (ROWCOLINFO) * lpWD->nMaxCols);
	if (!lpWD->lpColInfo)
	{
		DestroyWindow(lpWD->hWnd);
		return FALSE;
	}
	for (col = 0; col < lpWD->nMaxCols; col++)
	{
		lpWD->lpColInfo[col].nMinSize = lpWD->lpColInfo[col].nIdealSize = cx;
		lpWD->lpColInfo[col].nCurSize = -1; // will be set in RecalcLayout
	}
	lpWD->lpRowInfo = SPLAlloc(sizeof (ROWCOLINFO) * lpWD->nMaxRows);
	if (!lpWD->lpRowInfo)
	{
		DestroyWindow(lpWD->hWnd);
		return FALSE;
	}
	for (row = 0; row < lpWD->nMaxRows; row++)
	{
		lpWD->lpRowInfo[row].nMinSize = lpWD->lpRowInfo[row].nIdealSize = cy;
		lpWD->lpRowInfo[row].nCurSize = -1; // will be set in RecalcLayout
	}
	return TRUE;
}

// You must create ALL panes unless DYNAMIC_SPLIT is defined!
//  Usually the splitter window is invisible when creating a pane
BOOL SPLCreateView(LPSPLWNDDATA lpWD, int row, int col, SIZE sizeInit,
                   LPCTSTR lpszClassName, LPCTSTR lpszWindowName, HINSTANCE hInstance,
                   DWORD dwStyle, DWORD dwExStyle, LPVOID lpParam)
{
	RECT rect;
	HWND hWnd;
	
#ifdef _DEBUG
	assert(lpWD && lpWD->hWnd);
	assert(row >= 0 && row < lpWD->nRows);
	assert(col >= 0 && col < lpWD->nCols);
	
	if (!lpWD->hWnd || GetDlgItem(lpWD->hWnd, SPLIdFromRowCol(lpWD, row, col)) != NULL)
	{
		return FALSE;
	}
#endif
	
	// set the initial size for that pane
	lpWD->lpColInfo[col].nIdealSize = sizeInit.cx;
	lpWD->lpRowInfo[row].nIdealSize = sizeInit.cy;
	
	dwStyle = (dwStyle | WS_CHILD) & ~(WS_BORDER);
	dwExStyle &= ~ WS_EX_CLIENTEDGE;
	
	// Create with the right size (wrong position)
	SetRect(&rect, 0, 0, sizeInit.cx, sizeInit.cy);
	if (dwStyle & WS_POPUP) return FALSE;
	
	hWnd = CreateWindowEx(dwExStyle, lpszClassName, lpszWindowName,
			      dwStyle, rect.left, rect.top,
			      rect.right - rect.left, rect.bottom - rect.top,
			      lpWD->hWnd, (HMENU)SPLIdFromRowCol(lpWD, row, col),
			      hInstance, lpParam);
	{
		BringWindowToTop(lpWD->hWnd);
		// show it as specified
		if (dwStyle & WS_MINIMIZE)
			ShowWindow(hWnd, SW_SHOWMINIMIZED);
		else if (dwStyle & WS_MAXIMIZE)
			ShowWindow(hWnd, SW_SHOWMAXIMIZED);
		else
			ShowWindow(hWnd, SW_SHOWNORMAL);
	}
	return hWnd?TRUE:FALSE;
}

// You must create ALL panes unless DYNAMIC_SPLIT is defined!
//  Usually the splitter window is invisible when creating a pane
BOOL SPLSwitchView(LPSPLWNDDATA lpWD, int row, int col, SIZE sizeInit,
                   LPCTSTR lpszClassName, LPCTSTR lpszWindowName, HINSTANCE hInstance,
                   DWORD dwStyle, DWORD dwExStyle, LPVOID lpParam)
{
	RECT rect;
	HWND hWnd;
	
	assert(lpWD && lpWD->hWnd);
	assert(row >= 0 && row < lpWD->nRows);
	assert(col >= 0 && col < lpWD->nCols);
	
	if (dwStyle & WS_POPUP) return FALSE;

	hWnd = SPLGetPane(lpWD, row, col);
	assert(hWnd);

	GetWindowRect(hWnd, &rect);

	SPLDeleteView(lpWD, row, col);
	
	dwStyle = (dwStyle | WS_CHILD) & ~(WS_BORDER);
	dwExStyle &= ~ WS_EX_CLIENTEDGE;
	
	hWnd = CreateWindowEx(dwExStyle, lpszClassName, lpszWindowName,
				 dwStyle, rect.left, rect.top,
				 rect.right - rect.left, rect.bottom - rect.top,
				 lpWD->hWnd, (HMENU)SPLIdFromRowCol(lpWD, row, col),
				 hInstance, lpParam);
	if (hWnd) {
		BringWindowToTop(lpWD->hWnd);
		ShowWindow(hWnd, SW_SHOWNORMAL);
		SPLRecalcLayout(lpWD);
	} else {
		DWORD dwError = GetLastError();
	}
	return hWnd?TRUE:FALSE;
}

int SPLIdFromRowCol(LPSPLWNDDATA lpWD, int row, int col)
{
	assert(lpWD && lpWD->hWnd);
	assert(row >= 0);
	assert(row < lpWD->nRows);
	assert(col >= 0);
	assert(col < lpWD->nCols);
	return EC_IDW_PANE_FIRST + row * 16 + col;
}

void SPLDeleteView(LPSPLWNDDATA lpWD, int row, int col)
{
	HWND hwndPane;
	
	assert(lpWD && lpWD->hWnd);
	
	// if active child is being deleted - activate next
	hwndPane = SPLGetPane(lpWD, row, col);
	//ASSERT_KINDOF(CView, pPane);
	if (SPLGetActivePane(lpWD, NULL) == hwndPane)
		SPLActivateNext(lpWD, FALSE);
	
	// default implementation assumes view will auto delete in PostNcDestroy
	DestroyWindow(hwndPane);
}
