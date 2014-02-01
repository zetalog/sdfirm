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
 * @(#)SPLDraw.c: handle splitter painting
 * $Id: SPLDraw.c,v 1.1 2007/03/12 14:54:21 zhenglv Exp $
 */

#include "SPLIntern.h"

void SPLPaint(LPSPLWNDDATA lpWD)
{
	PAINTSTRUCT ps;
	RECT rectClient;
	HDC hDC;
	
	assert(lpWD && lpWD->hWnd);
	
	hDC = BeginPaint(lpWD->hWnd, &ps);
	GetClientRect(lpWD->hWnd, &rectClient);
	
	// extend split bars to window border (past margins)
	DebugPrint("SPLDrawAllSplitBars: called from SPLPaint.");
	SPLDrawAllSplitBars(lpWD, hDC, rectClient.right, rectClient.bottom);
	
	EndPaint(lpWD->hWnd, &ps);
}

// Generic routine:
//  for X direction: pInfo = lpWD->pColInfo, nMax = lpWD->nMaxCols, nSize = cx
//  for Y direction: pInfo = lpWD->pRowInfo, nMax = lpWD->nMaxRows, nSize = cy
static void _LayoutRowCol(LPROWCOLINFO pInfoArray,
	int nMax, int nSize, int nSizeSplitter)
{
	LPROWCOLINFO pInfo;
	int i;

	assert(pInfoArray != NULL);
	assert(nMax > 0);
	assert(nSizeSplitter > 0);

	if (nSize < 0)
		nSize = 0;  // if really too small, layout as zero size

	// start with ideal sizes
	for (i = 0, pInfo = pInfoArray; i < nMax-1; i++, pInfo++)
	{
		if (pInfo->nIdealSize < pInfo->nMinSize)
			pInfo->nIdealSize = 0;      // too small to see
		pInfo->nCurSize = pInfo->nIdealSize;
	}
	pInfo->nCurSize = INT_MAX;  // last row/column takes the rest

	for (i = 0, pInfo = pInfoArray; i < nMax; i++, pInfo++)
	{
		assert(nSize >= 0);
		if (nSize == 0)
		{
			// no more room (set pane to be invisible)
			pInfo->nCurSize = 0;
			continue;       // don't worry about splitters
		}
		else if (nSize < pInfo->nMinSize && i != 0)
		{
			// additional panes below the recommended minimum size
			//   aren't shown and the size goes to the previous pane
			pInfo->nCurSize = 0;

			// previous pane already has room for splitter + border
			//   add remaining size and remove the extra border
			assert(CX_BORDER2 == CY_BORDER2);
			(pInfo-1)->nCurSize += nSize + CX_BORDER2;
			nSize = 0;
		}
		else
		{
			// otherwise we can add the second pane
			assert(nSize > 0);
			if (pInfo->nCurSize == 0)
			{
				// too small to see
				if (i != 0)
					pInfo->nCurSize = 0;
			}
			else if (nSize < pInfo->nCurSize)
			{
				// this row/col won't fit completely - make as small as possible
				pInfo->nCurSize = nSize;
				nSize = 0;
			}
			else
			{
				// can fit everything
				nSize -= pInfo->nCurSize;
			}
		}

		// see if we should add a splitter
		assert(nSize >= 0);
		if (i != nMax - 1)
		{
			// should have a splitter
			if (nSize > nSizeSplitter)
			{
				nSize -= nSizeSplitter; // leave room for splitter + border
				assert(nSize > 0);
			}
			else
			{
				// not enough room - add left over less splitter size
				assert(CX_BORDER2 == CY_BORDER2);
				pInfo->nCurSize += nSize;
				if (pInfo->nCurSize > (nSizeSplitter - CX_BORDER2))
					pInfo->nCurSize -= (nSizeSplitter - CY_BORDER2);
				nSize = 0;
			}
		}
	}
	assert(nSize == 0); // all space should be allocated
}

// repositions client area of specified window
// assumes everything has WS_BORDER or is inset like it does
//  (includes scroll bars)
static void _DeferClientPos(SIZEPARENTPARAMS *lpLayout,
	HWND hWnd, int x, int y, int cx, int cy)
{
	RECT rect, rectOld;
	HWND hWndParent = GetParent(hWnd);
	POINT point;
	
	assert(hWnd != NULL);
	
	rect.left = x;
	rect.top = y;
	rect.right = x+cx;
	rect.bottom = y+cy;
	
	// adjust for border size (even if zero client size)
	CalcWindowRect(hWnd, &rect, adjustBorder);
	
	// adjust for 3d border (splitter windows have implied border)
	if (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE &&
		IsRuntimeClass(hWndParent, WC_SPLITTER))
		InflateRect(&rect, CX_BORDER2, CY_BORDER2);
	
	// first check if the new rectangle is the same as the current
	GetWindowRect(hWnd, &rectOld);
	point.x = rectOld.top;
	point.y = rectOld.left;
	ScreenToClient(hWndParent, &point);
	point.x = rectOld.bottom;
	point.y = rectOld.right;
	ScreenToClient(hWndParent, &point);
	if (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
		LONG temp = rectOld.left; rectOld.left = rectOld.right; rectOld.right = temp;
	}

	if (!EqualRect(&rect, &rectOld))
		RepositionWindow(lpLayout, hWnd, &rect);
}

void SPLRecalcLayout(LPSPLWNDDATA lpWD)
{
	RECT rectClient;
	SIZEPARENTPARAMS layout;
	
	assert(lpWD && lpWD->hWnd);
	assert(lpWD->nRows > 0 && lpWD->nCols > 0); // must have at least one pane
	
	DebugPrint("Recalc Layout of Splitter panes & scroll bars %d - %d",
		   lpWD->cxSplitter, lpWD->cySplitter);
	
	GetClientRect(lpWD->hWnd, &rectClient);
	
	// layout columns (restrict to possible sizes)
	_LayoutRowCol(lpWD->lpColInfo, lpWD->nCols, rectClient.right-rectClient.left, lpWD->cxSplitter);
	_LayoutRowCol(lpWD->lpRowInfo, lpWD->nRows, rectClient.bottom-rectClient.top, lpWD->cySplitter);
	
	// adjust the panes (and optionally scroll bars)
	
	// give the hint for the maximum number of HWNDs
	layout.hDWP = BeginDeferWindowPos((lpWD->nCols + 1) * (lpWD->nRows + 1) + 1);
	
	//BLOCK: Reposition all the panes
	{
		int x = rectClient.left;
		int col;
		for (col = 0; col < lpWD->nCols; col++)
		{
			int cx = lpWD->lpColInfo[col].nCurSize;
			int y = rectClient.top;
			int row;
			for (row = 0; row < lpWD->nRows; row++)
			{
				int cy = lpWD->lpRowInfo[row].nCurSize;
				HWND hWnd = SPLGetPane(lpWD, row, col);
				DebugPrint("Defer pane(%d, %d)", row, col);
				_DeferClientPos(&layout, hWnd, x, y, cx, cy);
				y += cy + lpWD->cySplitter;
			}
			x += cx + lpWD->cxSplitter;
		}
	}
	
	// move and resize all the windows at once!
	if (layout.hDWP == NULL || !EndDeferWindowPos(layout.hDWP))
		DebugPrint("DeferWindowPos: failed - low system resources.");
	
	// invalidate all the splitter bars (with NULL pDC)
	DebugPrint("SPLDrawAllSplitBars: called from SPLRecalcLayout.");
	SPLDrawAllSplitBars(lpWD, NULL, rectClient.right, rectClient.bottom);
}

char *SPLDrawType(ESPLITTYPE nType)
{
	switch (nType)
	{
	case splitBox:
		return "Box";
	case splitBar:
		return "Bar";
	case splitIntersection:
		return "Intersection";
	case splitBorder:
		return "Border";
	default:
		return "Unkonw";
	}
}

void SPLDrawSplitter(LPSPLWNDDATA lpWD, HDC hDC, ESPLITTYPE nType,
		     const RECT *lpRectArg)
{
	RECT rect;
	COLORREF clr = clrBtnFace;

	// if pDC == NULL, then just invalidate
	if (hDC == NULL)
	{
		RedrawWindow(lpWD->hWnd, lpRectArg, NULL, RDW_INVALIDATE|RDW_NOCHILDREN);
		return;
	}

	// otherwise, actually draw
	CopyRect(&rect, lpRectArg);
	DebugPrint("Type=%s, TopLeft=(%d, %d), BottomRight=(%d, %d)",
		   SPLDrawType(nType), rect.left, rect.top, rect.right, rect.bottom);
	switch (nType)
	{
	case splitBorder:
		Draw3dRect(hDC, &rect, clrBtnShadow, clrBtnHilite);
		InflateRect(&rect, -CX_BORDER, -CY_BORDER);
		Draw3dRect(hDC, &rect, clrWindowFrame, clrBtnFace);
		return;

	case splitIntersection:
		break;

	case splitBox:
		//Draw3dRect(hDC, &rect, clrBtnFace, clrWindowFrame);
		//InflateRect(&rect, -CX_BORDER, -CY_BORDER);
		//Draw3dRect(hDC, &rect, clrBtnHilite, clrBtnShadow);
		//InflateRect(&rect, -CX_BORDER, -CY_BORDER);
		break;

	case splitBar:
		Draw3dRect(hDC, &rect, clrBtnFace, clrWindowFrame);
		InflateRect(&rect, -CX_BORDER, -CY_BORDER);
		Draw3dRect(hDC, &rect, clrBtnHilite, clrBtnShadow);
		InflateRect(&rect, -CX_BORDER, -CY_BORDER);
		break;

	default:
		assert(FALSE);  // unknown splitter type
	}
	// fill the middle
	FillSolidRect(hDC, &rect, clrBtnFace);
}

void SPLDrawAllSplitBars(LPSPLWNDDATA lpWD, HDC hDC, int cxInside, int cyInside)
{
	RECT rect;
	int col, row;

	assert(lpWD && lpWD->hWnd);

	// draw column split bars
	GetClientRect(lpWD->hWnd, &rect);
	for (col = 0; col < lpWD->nCols - 1; col++)
	{
		rect.left += lpWD->lpColInfo[col].nCurSize;
		rect.right = rect.left + lpWD->cxSplitter;
		if (rect.left > cxInside)
			break;      // stop if not fully visible
		SPLDrawSplitter(lpWD, hDC, splitBar, &rect);
		rect.left = rect.right;
	}

	// draw row split bars
	GetClientRect(lpWD->hWnd, &rect);
	for (row = 0; row < lpWD->nRows - 1; row++)
	{
		rect.top += lpWD->lpRowInfo[row].nCurSize;
		rect.bottom = rect.top + lpWD->cySplitter;
		if (rect.top > cyInside)
			break;      // stop if not fully visible
		SPLDrawSplitter(lpWD, hDC, splitBar, &rect);
		rect.top = rect.bottom;
	}
}
