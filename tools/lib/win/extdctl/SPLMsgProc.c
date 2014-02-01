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
 * @(#)SPLMsgProc.c: handle splitter main entrance
 * $Id: SPLMsgProc.c,v 1.1 2007/03/12 14:54:21 zhenglv Exp $
 */

#include "SPLIntern.h"

/*
 * Function: SplitterMsgProc
 *
 * Purpose:
 *
 * Window procedure for splitter
 */
long FAR PASCAL SplitterMsgProc(HWND hWnd, UINT uMsg, UINT wParam, long lParam)
{
	LPSPLWNDDATA lpWD = Splitter_GetPtr(hWnd);
	
	if (lpWD == NULL) 
	{
		if (uMsg == WM_NCCREATE) 
		{
			HWND hwndParent;
			
			lpWD = (SPLWNDDATA*)SPLAlloc(sizeof (SPLWNDDATA));
			if (lpWD == NULL)
				return 0L;
			
			Splitter_SetPtr(hWnd, lpWD);
			hwndParent = GetParent(hWnd);
			assert(hwndParent != NULL);
			ModifyStyleEx(hwndParent, WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);
		}
		else 
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	if (uMsg == WM_NCDESTROY) 
	{
		SPLFree(lpWD->lpRowInfo);
		SPLFree(lpWD->lpColInfo);
		SPLFree(lpWD);
		lpWD = NULL;
		Splitter_SetPtr(hWnd, NULL);
	}
	
	switch (uMsg)
	{
	case WM_CREATE:
		{
			LPCREATESTRUCT lpcs = (LPCREATESTRUCT)lParam;
			LPSPLCREATESTRUCT lpscs = (LPSPLCREATESTRUCT)lpcs->lpCreateParams;
			
			ZeroMemory(lpWD, sizeof (SPLWNDDATA));
			lpWD->hWnd = hWnd;
			lpWD->hWndParent = ((LPCREATESTRUCT)lParam)->hwndParent;
			lpWD->nID = (LONG)((LPCREATESTRUCT)lParam)->hMenu;
			lpWD->dwStyle = (LONG)((LPCREATESTRUCT)lParam)->style;
			lpWD->cxSplitter = lpWD->cySplitter = 3 + 2 + 2;

			lpWD->hcurArrow = LoadCursor(LIBINSTANCE, IDC_ARROW);
			
#ifdef _DEBUG
			assert(GetSystemMetrics(SM_CXBORDER) == 1 && GetSystemMetrics(SM_CYBORDER) == 1);
#endif
			SPLCreateStatic(lpWD, lpscs);
		}
		break;
		
	case WM_DESTROY:
		if (lpWD->lpDynamicViewClass)
			free(lpWD->lpDynamicViewClass);
		break;
		
	case WM_NOTIFY:
	case WM_COMMAND:
		{
			return SendMessage(lpWD->hWndParent, WM_NOTIFY, wParam, lParam);
		}
		break;
		
	case WM_SIZE:
		{
			UINT nType = wParam;
			int cx = LOWORD(lParam);
			int cy = HIWORD(lParam);
			DebugPrint("SPLRecalcLayout: WM_SIZE");
			if (nType != SIZE_MINIMIZED && cx > 0 && cy > 0)
				SPLRecalcLayout(lpWD);
		}
		return (LRESULT)0;
		
	case WM_PAINT:
		DebugPrint("SPLPaint: WM_PAINT");
		SPLPaint(lpWD);
		return (LRESULT)TRUE;
		
	case WM_LBUTTONDOWN:
		{
			POINT pt;
			if (lpWD->bTracking)
				break;
			
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			
			SPLStartTracking(lpWD, SPLHitTest(lpWD, pt));
		}
		break;
		
	case WM_LBUTTONDBLCLK:
		break;
		
	case WM_LBUTTONUP:
		SPLStopTracking(lpWD, TRUE);
		// GetDlgItem(lpWD->hWnd, EC_IDW_SIZE_BOX);
		InvalidateRect(hWnd, NULL, TRUE);
		return (LRESULT)TRUE;
		
	case WM_KEYDOWN:
		break;
		
	case WM_HSCROLL:
		break;
		
	case WM_VSCROLL:
		break;
		
	case WM_CANCELMODE:
		break;
		
	case WM_SETCURSOR:
		break;
		
	case WM_MOUSEMOVE:
		{
			POINT pt;
			pt.x = GET_X_LPARAM(lParam);
			pt.y = GET_Y_LPARAM(lParam);
			
			SPLMouseMove(lpWD, wParam, pt);
		}
		break;

	case SPLM_SWITCHVIEW:
		{
			LPSPLCREATESTRUCT lpcs = (LPSPLCREATESTRUCT)lParam;
			
			return SPLSwitchView(lpWD, lpcs->nRows, lpcs->nCols, lpcs->sizeMin,
					     lpcs->lpszClass, lpcs->lpszName, lpcs->hInstance,
					     lpcs->dwStyle, lpcs->dwExStyle, lpcs->lpParam);
		}
		break;
		
	case SPLM_CREATEVIEW:
		{
			LPSPLCREATESTRUCT lpcs = (LPSPLCREATESTRUCT)lParam;
			
			return SPLCreateView(lpWD, lpcs->nRows, lpcs->nCols, lpcs->sizeMin,
					     lpcs->lpszClass, lpcs->lpszName, lpcs->hInstance,
					     lpcs->dwStyle, lpcs->dwExStyle, lpcs->lpParam);
		}
		break;
		
	case SPLM_GETROWCOUNT:
		return lpWD->nRows;
		
	case SPLM_GETCOLCOUNT:
		return lpWD->nCols;
		
	case SPLM_GETROWINFO:
		{
			LPSPLROWINFO lpri = (LPSPLROWINFO)lParam;
			
			SPLGetRowInfo(lpWD, lpri->row, &(lpri->cyCur), &(lpri->cyMin));
		}
		break;
		
	case SPLM_SETROWINFO:
		{
			LPSPLROWINFO lpri = (LPSPLROWINFO)lParam;
			
			SPLSetRowInfo(lpWD, lpri->row, lpri->cyCur, lpri->cyMin);
		}
		break;
		
	case SPLM_GETCOLINFO:
		{
			LPSPLCOLINFO lpci = (LPSPLCOLINFO)lParam;
			
			SPLGetColumnInfo(lpWD, lpci->col, &(lpci->cxCur), &(lpci->cxMin));
		}
		break;
		
	case SPLM_SETCOLINFO:
		{
			LPSPLCOLINFO lpci = (LPSPLCOLINFO)lParam;
			
			SPLSetColumnInfo(lpWD, lpci->col, lpci->cxCur, lpci->cxMin);
		}
		break;
		
	case SPLM_GETPANE:
		{
			LPSPLPANEINFO lppi = (LPSPLPANEINFO)lParam;
			
			assert(lpWD && hWnd && lppi);
			return (LRESULT)SPLGetPane(lpWD, lppi->row, lppi->col);
		}
		break;
		
	case SPLM_ISCHILDPANE:
		{
			HWND hwndView = (HWND)wParam;
			LPSPLPANEINFO lppi = (LPSPLPANEINFO)lParam;
			
			return SPLIsChildPane(lpWD, hwndView, lppi);
		}
		break;
	}
	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
}
