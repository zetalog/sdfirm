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
 * @(#)SPLTrack.c: handle splitter tracking
 * $Id: SPLTrack.c,v 1.1 2007/03/12 14:54:21 zhenglv Exp $
 */

#include "SPLIntern.h"

void SPLGetHitRect(LPSPLWNDDATA lpWD, int ht, RECT *lpRectHit)
{
	RECT rectClient;
	int cx, cy, x, y;
	
	assert(lpWD && lpWD->hWnd);
	
	GetClientRect(lpWD->hWnd, &rectClient);
	cx = rectClient.right-rectClient.left;
	cy = rectClient.bottom-rectClient.top;
	x = rectClient.top;
	y = rectClient.left;
	
	// hit rectangle does not include border
	// lpWD->rcLimit will be limited to valid tracking rect
	// lpWD->ptTrackOffset will be set to appropriate tracking offset
	lpWD->ptTrackOffset.x = 0;
	lpWD->ptTrackOffset.y = 0;
	
	if (ht == vSplitterBox)
	{
		cy = lpWD->cySplitter;
		lpWD->ptTrackOffset.y = -(cy / 2);
		assert(lpWD->lpRowInfo[0].nCurSize > 0);
		lpWD->rcLimit.bottom -= cy;
	}
	else if (ht == hSplitterBox)
	{
		cx = lpWD->cxSplitter;
		lpWD->ptTrackOffset.x = -(cx / 2);
		assert(lpWD->lpColInfo[0].nCurSize > 0);
		lpWD->rcLimit.right -= cx;
	}
	else if (ht >= vSplitterBar1 && ht <= vSplitterBar15)
	{
		int row;
		cy = lpWD->cySplitter;
		lpWD->ptTrackOffset.y = -(cy / 2);
		for (row = 0; row < ht - vSplitterBar1; row++)
			y += lpWD->lpRowInfo[row].nCurSize + lpWD->cySplitter;
		lpWD->rcLimit.top = y;
		y += lpWD->lpRowInfo[row].nCurSize;
		lpWD->rcLimit.bottom -= cy;
	}
	else if (ht >= hSplitterBar1 && ht <= hSplitterBar15)
	{
		int col;
		cx = lpWD->cxSplitter;
		lpWD->ptTrackOffset.x = -(cx / 2);
		for (col = 0; col < ht - hSplitterBar1; col++)
			x += lpWD->lpColInfo[col].nCurSize + lpWD->cxSplitter;
		lpWD->rcLimit.left = x;
		x += lpWD->lpColInfo[col].nCurSize;
		lpWD->rcLimit.right -= cx;
	}
	else
	{
		DebugPrint("Error: SPLGetHitRect(%d): Not Found!", ht);
		assert(FALSE);
	}
	
	lpRectHit->right = (lpRectHit->left = x) + cx;
	lpRectHit->bottom = (lpRectHit->top = y) + cy;
}

int SPLHitTest(LPSPLWNDDATA lpWD, POINT pt)
{
	RECT rectClient, rect;
	int row, col;
	
	assert(lpWD && lpWD->hWnd);
	
	GetClientRect(lpWD->hWnd, &rectClient);
	
	// for hit detect, include the border of splitters
	CopyRect(&rect , &rectClient);
	for (col = 0; col < lpWD->nCols - 1; col++)
	{
		rect.left += lpWD->lpColInfo[col].nCurSize;
		rect.right = rect.left + lpWD->cxSplitter;
		if (PtInRect(&rect, pt))
			break;
		rect.left = rect.right;
	}
	
	CopyRect(&rect , &rectClient);
	for (row = 0; row < lpWD->nRows - 1; row++)
	{
		rect.top += lpWD->lpRowInfo[row].nCurSize;
		rect.bottom = rect.top + lpWD->cySplitter;
		if (PtInRect(&rect, pt))
			break;
		rect.top = rect.bottom;
	}
	
	// row and col set for hit splitter (if not hit will be past end)
	if (col != lpWD->nCols - 1)
	{
		if (row != lpWD->nRows - 1)
			return splitterIntersection1 + row * 15 + col;
		return hSplitterBar1 + col;
	}
	
	if (row != lpWD->nRows - 1)
		return vSplitterBar1 + row;
	
	return noHit;
}

// cache of last needed cursor
static HCURSOR _hcurLast = NULL;
static HCURSOR _hcurDestroy = NULL;
static UINT _idcPrimaryLast = 0; // store the primary IDC

void SPLSetSplitCursor(LPSPLWNDDATA lpWD, int ht)
{
	UINT idcPrimary;        // app supplied cursor
	LPCTSTR idcSecondary;    // system supplied cursor (MAKEINTRESOURCE)

	if (ht == vSplitterBox ||
	    ht >= vSplitterBar1 && ht <= vSplitterBar15)
	{
		idcPrimary = EC_IDC_VSPLITBAR;
		idcSecondary = IDC_SIZENS;
	}
	else if (ht == hSplitterBox ||
		 ht >= hSplitterBar1 && ht <= hSplitterBar15)
	{
		idcPrimary = EC_IDC_HSPLITBAR;
		idcSecondary = IDC_SIZEWE;
	}
	else if (ht >= splitterIntersection1 && ht <= splitterIntersection225)
	{
		idcPrimary = EC_IDC_SMALLARROWS;
		idcSecondary = IDC_SIZEALL;
	}
	else
	{
		SetCursor(lpWD->hcurArrow);
		idcPrimary = 0;     // don't use it
		idcSecondary = 0;   // don't use it
	}

	if (idcPrimary != 0)
	{
		HCURSOR hcurToDestroy = NULL;
		if (idcPrimary != _idcPrimaryLast)
		{
			// load in another cursor
			hcurToDestroy = _hcurDestroy;

			if ((_hcurDestroy = _hcurLast =
			    LoadCursor(LIBINSTANCE, MAKEINTRESOURCE(idcPrimary))) == NULL)
			{
				// will not look as good
				assert(_hcurDestroy == NULL);    // will not get destroyed
				_hcurLast = LoadCursor(NULL, idcSecondary);
				assert(_hcurLast != NULL);
			}
			_idcPrimaryLast = idcPrimary;
		}
		assert(_hcurLast != NULL);
		SetCursor(_hcurLast);
		assert(_hcurLast != hcurToDestroy);
		if (hcurToDestroy != NULL)
			DestroyCursor(hcurToDestroy); // destroy after being set
	}
}

void SPLStartTracking(LPSPLWNDDATA lpWD, int ht)
{
	HWND hwndView;
	
	assert(lpWD && lpWD->hWnd);
	if (ht == noHit)
		return;
	
	// GetClientRect will restrict 'lpWD->rcLimit' as appropriate
	GetClientRect(lpWD->hWnd, &(lpWD->rcLimit));
	
	if (ht >= splitterIntersection1 && ht <= splitterIntersection225)
	{
		// split two directions (two tracking rectangles)
		int row = (ht - splitterIntersection1) / 15;
		int col = (ht - splitterIntersection1) % 15;
		int yTrackOffset;
		
		SPLGetHitRect(lpWD, row + vSplitterBar1, &(lpWD->rcTracker));
		yTrackOffset = lpWD->ptTrackOffset.y;
		lpWD->ptTrackOffset.y = yTrackOffset;
	}
	else
	{
		// only hit one bar
		SPLGetHitRect(lpWD, ht, &(lpWD->rcTracker));
	}
	
	// allow active view to preserve focus before taking it away
	hwndView = SPLGetActivePane(lpWD, NULL);
	if (hwndView != NULL)
	{
		/*
		HWND hwndFrame;
		assert(hwndView);
		hwndFrame = GetParentFrame(lpWD->hWnd);
		assert(hwndFrame);
		hwndView->OnActivateFrame(WA_INACTIVE, hwndFrame);
		*/
	}

	// steal focus and capture
	SetCapture(lpWD->hWnd);
	SetFocus(lpWD->hWnd);

	// make sure no updates are pending
	RedrawWindow(lpWD->hWnd, NULL, NULL, RDW_ALLCHILDREN | RDW_UPDATENOW);

	// set tracking state and appropriate cursor
	lpWD->bTracking = TRUE;
	SPLInvertTracker(lpWD, &(lpWD->rcTracker));
	lpWD->htTrack = ht;
	SPLSetSplitCursor(lpWD, ht);
}

void SPLInvertTracker(LPSPLWNDDATA lpWD, RECT *lpRect)
{
	HDC hDC;
	HBRUSH hBrush, hOldBrush = NULL;
	
	assert(lpWD && lpWD->hWnd);
	assert(!IsRectEmpty(lpRect));
	assert((GetWindowLong(lpWD->hWnd, GWL_STYLE) & WS_CLIPCHILDREN) == 0);
	
	// pat-blt without clip children on
	hDC = GetDC(lpWD->hWnd);
	// invert the brush pattern (looks just like frame window sizing)
	hBrush = GetHalftoneBrush();
	if (hBrush != NULL)
		hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);
	PatBlt(hDC, lpRect->left, lpRect->top, lpRect->right-lpRect->left,
		lpRect->bottom-lpRect->top, PATINVERT);
	if (hOldBrush != NULL)
		SelectObject(hDC, hOldBrush);
	ReleaseDC(lpWD->hWnd, hDC);
}

void SPLMouseMove(LPSPLWNDDATA lpWD, UINT nFlags, POINT pt)
{
	if (GetCapture() != lpWD->hWnd)
		SPLStopTracking(lpWD, FALSE);
	
	if (lpWD->bTracking)
	{
		// move tracker to current cursor position
		pt.x += lpWD->ptTrackOffset.x;
		pt.y += lpWD->ptTrackOffset.y;
		// pt is the upper right of hit detect
		// limit the point to the valid split range
		if (pt.y < lpWD->rcLimit.top)
			pt.y = lpWD->rcLimit.top;
		else if (pt.y > lpWD->rcLimit.bottom)
			pt.y = lpWD->rcLimit.bottom;
		if (pt.x < lpWD->rcLimit.left)
			pt.x = lpWD->rcLimit.left;
		else if (pt.x > lpWD->rcLimit.right)
			pt.x = lpWD->rcLimit.right;

		if (lpWD->htTrack == vSplitterBox ||
		    lpWD->htTrack >= vSplitterBar1 && lpWD->htTrack <= vSplitterBar15)
		{
			if (lpWD->rcTracker.top != pt.y)
			{
				SPLInvertTracker(lpWD, &(lpWD->rcTracker));
				OffsetRect(&(lpWD->rcTracker), 0, pt.y - lpWD->rcTracker.top);
				SPLInvertTracker(lpWD, &(lpWD->rcTracker));
			}
		}
		else if (lpWD->htTrack == hSplitterBox ||
			 lpWD->htTrack >= hSplitterBar1 && lpWD->htTrack <= hSplitterBar15)
		{
			if (lpWD->rcTracker.left != pt.x)
			{
				SPLInvertTracker(lpWD, &(lpWD->rcTracker));
				OffsetRect(&(lpWD->rcTracker), pt.x - lpWD->rcTracker.left, 0);
				SPLInvertTracker(lpWD, &(lpWD->rcTracker));
			}
		}
		else if (lpWD->htTrack >= splitterIntersection1 &&
			 lpWD->htTrack <= splitterIntersection225)
		{
			if (lpWD->rcTracker.top != pt.y)
			{
				SPLInvertTracker(lpWD, &(lpWD->rcTracker));
				OffsetRect(&(lpWD->rcTracker), 0, pt.y - lpWD->rcTracker.top);
				SPLInvertTracker(lpWD, &(lpWD->rcTracker));
			}
		}
	}
	else
	{
		// simply hit-test and set appropriate cursor
		int ht = SPLHitTest(lpWD, pt);
		SPLSetSplitCursor(lpWD, ht);
	}
}

void SPLStopTracking(LPSPLWNDDATA lpWD, BOOL bAccept)
{
	HWND hwndOldActiveView;
	
	assert(lpWD && lpWD->hWnd);
	
	if (!lpWD->bTracking)
		return;
	
	ReleaseCapture();
	
	// erase tracker rectangle
	SPLInvertTracker(lpWD, &(lpWD->rcTracker));
	lpWD->bTracking = FALSE;
	
	// save old active view
	hwndOldActiveView = SPLGetActivePane(lpWD, NULL);

	if (bAccept)
	{
		if (lpWD->htTrack >= vSplitterBar1 && lpWD->htTrack <= vSplitterBar15)
		{
			// set row height
			SPLTrackRowSize(lpWD, lpWD->rcTracker.top, lpWD->htTrack - vSplitterBar1);
			SPLRecalcLayout(lpWD);
		}
		else if (lpWD->htTrack >= hSplitterBar1 && lpWD->htTrack <= hSplitterBar15)
		{
			// set columnw width
			SPLTrackColumnSize(lpWD, lpWD->rcTracker.left, lpWD->htTrack - hSplitterBar1);
			SPLRecalcLayout(lpWD);
		}
		else if (lpWD->htTrack >= splitterIntersection1 &&
			lpWD->htTrack <= splitterIntersection225)
		{
			// set row height and column width
			int row = (lpWD->htTrack - splitterIntersection1) / 15;
			int col = (lpWD->htTrack - splitterIntersection1) % 15;

			SPLTrackRowSize(lpWD, lpWD->rcTracker.top, row);
			SPLRecalcLayout(lpWD);
		}
	}

	if (hwndOldActiveView == SPLGetActivePane(lpWD, NULL))
	{
		if (hwndOldActiveView != NULL)
		{
			SPLSetActivePane(lpWD, -1, -1, hwndOldActiveView); // re-activate
			SetFocus(hwndOldActiveView); // make sure focus is restored
		}
	}
}

void SPLTrackRowSize(LPSPLWNDDATA lpWD, int y, int row)
{
	POINT pt;
	
	assert(lpWD && lpWD->hWnd);
	assert(lpWD->nRows > 1);
	
	pt.x = 0, pt.y = y;
	ClientToScreen(lpWD->hWnd, &pt);
	ScreenToClient(SPLGetPane(lpWD, row, 0), &pt);
	lpWD->lpRowInfo[row].nIdealSize = pt.y;      // new size
	if (pt.y < lpWD->lpRowInfo[row].nMinSize)
	{
		// resized too small
		lpWD->lpRowInfo[row].nIdealSize = 0; // make it go away
	}
}

void SPLTrackColumnSize(LPSPLWNDDATA lpWD, int x, int col)
{
	POINT pt;
	HWND hwndPane;
	
	assert(lpWD && lpWD->hWnd);
	assert(lpWD->nCols > 1);
	
	pt.x = x, pt.y = 0;
	ClientToScreen(lpWD->hWnd, &pt);
	hwndPane = SPLGetPane(lpWD, 0, col);
	ScreenToClient(hwndPane, &pt);
	lpWD->lpColInfo[col].nIdealSize = pt.x;      // new size
	if (pt.x < lpWD->lpColInfo[col].nMinSize)
	{
		// resized too small
		lpWD->lpColInfo[col].nIdealSize = 0; // make it go away
	}
}
