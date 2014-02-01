//:TLMsgProc.c - TreeList message processing routine
//
//	$Id: TLMsgPrc.c,v 1.1 2007/03/12 14:54:21 zhenglv Exp $
//

#include "TLintern.h"
#include <math.h>


//----------------------------------------------------|
//  Message processing function for TreeList class

LRESULT WINAPI TreeListMsgProc (HWND hWnd, UINT msg, WPARAM wParam, 
									LPARAM lParam) {

	PAINTSTRUCT ps;
	HDC hdc;
	DWORD dwStyle;

	TLWndData* pWD = TreeList_GetPtr(hWnd);

    if (pWD == NULL) {
        if (msg == WM_NCCREATE) {
        	pWD = (TLWndData*)TLAlloc (sizeof(TLWndData));
        	if (pWD == NULL) return 0L;
        	TreeList_SetPtr (hWnd, pWD);
        }
        else {
        	return DefWindowProc (hWnd, msg, wParam, lParam);
        }
    }
	
    if (msg == WM_NCDESTROY) {
		TLFree (pWD);
        pWD = NULL;
        TreeList_SetPtr (hWnd, NULL);
    }

	switch (msg) {

	case WM_CREATE :
		pWD->hWnd = hWnd;
		pWD->hWndParent = ((LPCREATESTRUCT)lParam)->hwndParent;
		pWD->hdcMem = NULL;
		pWD->id = (LONG)((LPCREATESTRUCT)lParam)->hMenu; 
		pWD->style = (LONG)((LPCREATESTRUCT)lParam)->style;
		pWD->bTreeFocused = FALSE;
		pWD->uDragScrollState = 0;
		pWD->rootItem = NULL;
		pWD->columnItem = NULL;
		pWD->hImageList = NULL;
		pWD->hCursorNormal = GetCursor ();
		pWD->hCursorDragging = 
			LoadCursor (GetModuleHandle (NULL), "TreeListDraggingCursor");
		if (!(pWD->hCursorDragging)) pWD->hCursorDragging = GetCursor ();
		pWD->hCursorNoDrag = LoadCursor (NULL, IDC_NO);
		if (!(pWD->hCursorNoDrag)) pWD->hCursorNoDrag = GetCursor ();
		pWD->firstSelectItem = NULL;
		pWD->focusItem = NULL;
		pWD->iNumberColumns = 0;
		pWD->iHorizontalPos = 0;
		pWD->iFirstRow = 0;
		pWD->iWheelDelta = 0;
		pWD->iWheelScroll = 3;
		pWD->iNumberSelected = 0;
		pWD->clickedTreeItem = NULL;
		pWD->bDragging = FALSE;
		pWD->nmTreeList.hdr.hwndFrom = hWnd;
		pWD->bStillTyping = FALSE;
		pWD->szTypeBuf[0] = '\0';
		pWD->pTypeBufPtr = &(pWD->szTypeBuf[0]);
		pWD->iNumCharInTypeBuf = 0;

		TLInitPaint (pWD);

		// create a vertical scroll bar
		GetClientRect (pWD->hWnd, &pWD->tlRect);

		// create header control
		dwStyle = WS_VISIBLE | WS_CHILD | HDS_HORZ | HDS_BUTTONS;
		if (pWD->style & TLS_DRAGABLEHEADERS) 
			dwStyle |= HDS_DRAGDROP;
		pWD->hWndHeader = CreateWindowEx (
			0L, WC_HEADER, (LPCTSTR) NULL, dwStyle, 
			0, 0, 30000, pWD->iRowHeight+1,
			hWnd, (HMENU)IDC_TREELISTHEADER, 
			((LPCREATESTRUCT)lParam)->hInstance, NULL);
		SendMessage (pWD->hWndHeader, WM_SETFONT, (WPARAM)pWD->hFont, FALSE);

		// create tooltip control
		dwStyle = 0;
		pWD->hWndToolTip = CreateWindowEx (
			0L, TOOLTIPS_CLASS, (LPCTSTR) NULL, dwStyle, 
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
			hWnd, (HMENU)NULL, ((LPCREATESTRUCT)lParam)->hInstance, NULL); 
		if (pWD->hWndToolTip) {
			TOOLINFO ti;

			ti.cbSize = sizeof(TOOLINFO);
			ti.uFlags = TTF_ABSOLUTE|TTF_TRACK|TTF_IDISHWND|TTF_TRANSPARENT;
			ti.hwnd = pWD->hWndParent;
			ti.uId = (LPARAM)pWD->hWnd;
			ti.hinst = ((LPCREATESTRUCT)lParam)->hInstance;
			ti.lpszText = "Test is the time";
			ti.lParam = 0;

			SendMessage (pWD->hWndToolTip, TTM_ADDTOOL, 0, (LPARAM)&ti);
			SendMessage (pWD->hWndToolTip, TTM_TRACKACTIVATE, FALSE, 
								(LPARAM)&ti);
		}
		break;

	case WM_ERASEBKGND :
		return 1;

	case WM_KILLFOCUS :
		if (!pWD->bDragging) {
			pWD->bTreeFocused = FALSE;
			InvalidateRect (hWnd, NULL, FALSE);
		}
		return 0;

	case WM_SETFOCUS :
		pWD->bTreeFocused = TRUE;
		if (!pWD->focusItem) {
			if (pWD->rootItem) {
				pWD->focusItem = pWD->rootItem;
				pWD->focusItem->state |= TLIS_FOCUSED;
			}
		}
		else 
			pWD->focusItem->state |= TLIS_FOCUSED;
		InvalidateRect (hWnd, NULL, FALSE);
		return 0;

	case WM_GETDLGCODE :
		return DLGC_WANTARROWS | DLGC_WANTCHARS;

	case WM_PAINT :
		TLPrePaint (pWD);
		hdc = BeginPaint (pWD->hWnd, &ps);
		TLMemPaint (pWD, hdc, &ps);
		EndPaint (pWD->hWnd, &ps);
		return 0;

	case TLM_DRAGOVERPAINT :
		TLPrePaint (pWD);
		hdc = GetDCEx (pWD->hWnd, NULL, DCX_LOCKWINDOWUPDATE);
		TLMemPaint (pWD, hdc, NULL);
		ReleaseDC (pWD->hWnd, hdc);
		return 0;

	case TLM_RENDERDRAGBITMAP :
		TLPrePaint (pWD);
		return ((BOOL)TLMemPaintDragBitmap (pWD, (LPTL_DRAGBITMAP)lParam));

	case TLM_DRAGOVER :
		pWD->bTreeFocused = TRUE;
		return ((LRESULT)TLDragOver (pWD, lParam));

	case WM_LBUTTONDOWN :
		SetFocus (hWnd);
		TLEndDrag (pWD, lParam);
		pWD->bDragging = FALSE;
		TLLeftButtonDown (pWD, wParam, lParam);
		pWD->ptButtonDown.x = LOWORD (lParam);
		pWD->ptButtonDown.y = HIWORD (lParam);
		return 0;

	case WM_LBUTTONUP :
		if (pWD->bDragging) TLEndDrag (pWD, lParam);
		else TLLeftButtonUp (pWD, wParam, lParam);
		pWD->bDragging = FALSE;
		break;

	case WM_RBUTTONDOWN :
		SetFocus (hWnd);
		TLEndDrag (pWD, lParam);
		pWD->bDragging = FALSE;
		TLRightButtonDown (pWD, wParam, lParam);
		return 0;

	case WM_LBUTTONDBLCLK :
		SetFocus (hWnd);
		TLEndDrag (pWD, lParam);
		pWD->bDragging = FALSE;
		TLLeftButtonDblClk (pWD, wParam, lParam);
		return 0;

	case WM_RBUTTONDBLCLK :
		SetFocus (hWnd);
		break;

	case WM_MOUSEMOVE :
		TLMouseMove (pWD, wParam, lParam);
		return 0;

	case TLM_INSERTITEM :
		return ((LRESULT)TLInsertTreeItem (pWD, (LPTL_INSERTSTRUCT)lParam));

	case TLM_REINSERTITEM :
		return ((LRESULT)TLReInsertTreeItem (pWD, (LPTL_INSERTSTRUCT)lParam));

	case TLM_DELETEITEM :
		return ((LRESULT)TLDeleteTreeItem (pWD, (TL_TREEITEM*)lParam));

	case TLM_GETITEM :
		return ((LRESULT)TLGetTreeItem (pWD, (TL_TREEITEM*)lParam));

	case TLM_SETITEM :
		return ((LRESULT)TLSetTreeItem (pWD, (TL_TREEITEM*)lParam));

	case TLM_GETFIRSTITEM :
		return ((LRESULT)pWD->rootItem);

	case TLM_INSERTCOLUMN :
		return ((LRESULT)TLInsertColumn (pWD, wParam, (TL_COLUMN*)lParam));

	case TLM_SETCOLUMN :
		return ((LRESULT)TLSetColumn (pWD, wParam, (TL_COLUMN*)lParam));

	case TLM_DELETEALLCOLUMNS :
		return (TLDeleteAllColumns (pWD));

	case TLM_GETCOLUMNWIDTH :
		return ((LRESULT)TLGetColumnWidth (pWD, wParam));
		
	case TLM_SETIMAGELIST :
		pWD->hImageList = (HIMAGELIST)lParam;
		return 0;

	case TLM_SETLISTITEM :
		return ((LRESULT)TLSetListItem (pWD, (TL_LISTITEM*)lParam,
										(BOOL)wParam));

	case TLM_GETLISTITEM :
		return ((LRESULT)TLGetListItem (pWD, (TL_LISTITEM*)lParam));

	case TLM_EXPAND :
		return ((LRESULT)TLExpandTreeItem (pWD, (TL_TREEITEM*)lParam, 
												wParam));

	case TLM_SELECTITEM :
		return ((LRESULT)TLSelectTreeItem (pWD, (TL_TREEITEM*)lParam, 
												wParam));

	case TLM_SELECTCHILDREN :
		return ((LRESULT)TLSelectChildren (pWD, (TL_TREEITEM*)lParam));

	case TLM_ENSUREVISIBLE :
		return ((LRESULT)TLEnsureVisible (pWD, (TL_TREEITEM*)lParam));

	case TLM_ITERATESELECTED :
		return ((LRESULT)TLIterateSelected (pWD, lParam));

	case TLM_DELETECHILDREN :
		return ((LRESULT)TLDeleteChildren (pWD, (TL_TREEITEM*)lParam));

	case TLM_DELETETREE :
		if (wParam) TLDeleteTreeList (pWD, pWD->rootItem);
		pWD->rootItem = NULL;
		pWD->focusItem = NULL;
		pWD->firstSelectItem = NULL;
		pWD->iNumberSelected = 0;
		pWD->iFirstRow = 0;
		return TRUE;

	case TLM_GETORDERARRAY :
		return (SendMessage (pWD->hWndHeader, 
								HDM_GETORDERARRAY, wParam, lParam));

	case TLM_SETWHEELSCROLLLINES :
		pWD->iWheelScroll = lParam;
		break;

	case WM_TIMER :
		switch (wParam) {
		case KEYBOARDTIMERID :
			KillTimer (hWnd, wParam);
			pWD->bStillTyping = FALSE;
			break;
		case REORDERTIMERID :
			KillTimer (hWnd, wParam);
			pWD->nmTreeList.hdr.code = TLN_HEADERREORDERED;
			SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
							(LPARAM)&(pWD->nmTreeList));
			break;
		case DRAGSCROLLTIMERID :
			KillTimer (hWnd, wParam);
			TLDragScroll (pWD);
			break;
		case TOOLTIPTIMERID :
			if (TLKillToolTip (pWD))
				KillTimer (hWnd, wParam);
			break;
		}
		break;

	case WM_NOTIFY :
		switch (wParam) {
		case IDC_TREELISTHEADER :
			return TLHeaderNotification (pWD, (NMHEADER*)lParam);
		}
		break;
		
	case WM_HSCROLL :
		TLHScroll (pWD, LOWORD (wParam), HIWORD (wParam), (HWND)lParam);
		return 0;

	case WM_VSCROLL :
		TLVScroll (pWD, LOWORD (wParam), HIWORD (wParam), (HWND)lParam);
		return 0;

	case WM_MOUSEWHEEL :
		TLMouseWheel (pWD, LOWORD (wParam), HIWORD (wParam));
		return 0;

	case WM_WINDOWPOSCHANGED :
		TLResize (pWD, (WINDOWPOS*)lParam);
		return 0;

	case WM_SIZE :
	case WM_MOVE :
		return 0;

	case WM_KEYDOWN :
		TLKeyboardChar (pWD, wParam, lParam);
		break;

	case WM_SYSCOLORCHANGE :
		TLClosePaint (pWD);
		TLInitPaint (pWD);
		SetWindowPos (pWD->hWndHeader, 0, 0, 0, 30000, pWD->iRowHeight+1, 
						SWP_NOZORDER|SWP_NOMOVE);
		SendMessage (pWD->hWndHeader, WM_SETFONT, (WPARAM)pWD->hFont, FALSE);
		break;

	case WM_CONTEXTMENU :
		pWD->nmTreeList.hdr.code = TLN_CONTEXTMENU;
		pWD->nmTreeList.ptDrag.x = LOWORD (lParam);
		pWD->nmTreeList.ptDrag.y = HIWORD (lParam);
		SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
							(LPARAM)&(pWD->nmTreeList));
		return 0;

	case WM_CHAR :
		if ((pWD->style & TLS_AUTOSCROLL) && ~(lParam & 0x80000000)) {
			TLSelectByKeyboard (pWD, wParam, pWD->bStillTyping);
			pWD->bStillTyping = TRUE;
			SetTimer (hWnd, KEYBOARDTIMERID, KEYTIMEOUT, NULL);
		}
		pWD->nmKeyDown.hdr.code = TLN_CHAR;
		pWD->nmKeyDown.wVKey = wParam;
		pWD->nmKeyDown.flags = lParam;
		SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
							(LPARAM)&(pWD->nmKeyDown));
		break;

	case WM_CLOSE :
		TLDeleteMemDC (pWD);
		TLClosePaint (pWD);
		TLDeleteTreeList (pWD, pWD->rootItem);
		pWD->rootItem = NULL;
		TLDeleteAllColumns (pWD);
		pWD->columnItem = NULL;
		break;

	} 
	return (DefWindowProc (hWnd, msg, wParam, lParam));

}
