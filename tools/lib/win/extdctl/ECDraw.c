#include "ECIntern.h"
#include <assert.h>

static BOOL _ModifyStyle(HWND hWnd, int nStyleOffset, DWORD dwRemove, DWORD dwAdd, UINT nFlags);

BOOL ModifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	return _ModifyStyle(hWnd, GWL_STYLE, dwRemove, dwAdd, nFlags);
}

BOOL ModifyStyleEx(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	return _ModifyStyle(hWnd, GWL_EXSTYLE, dwRemove, dwAdd, nFlags);
}

BOOL _ModifyStyle(HWND hWnd, int nStyleOffset, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
	DWORD dwStyle;
	DWORD dwNewStyle;
	
	if (hWnd == NULL) return FALSE;
	dwStyle = GetWindowLong(hWnd, nStyleOffset);
	dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
	if (dwStyle == dwNewStyle)
		return FALSE;
	
	SetWindowLong(hWnd, nStyleOffset, dwNewStyle);
	if (nFlags != 0)
	{
		SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
			SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
	}
	return TRUE;
}

BOOL IsRuntimeClass(HWND hWnd, LPSTR lpszClass)
{
	CHAR szClass[128+1];

	if (RealGetWindowClass(hWnd, szClass, 128))
	{
		if (strcmp(lpszClass, szClass) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}

void RepositionWindow(SIZEPARENTPARAMS* lpLayout, HWND hWnd, LPCRECT lpRect)
{
	RECT rectOld;
	HWND hWndParent = GetParent(hWnd);
	POINT point;

	assert(hWnd != NULL);
	assert(lpRect != NULL);
	assert(hWndParent != NULL);

	if (lpLayout != NULL && lpLayout->hDWP == NULL)
		return;

	// first check if the new rectangle is the same as the current
	GetWindowRect(hWnd, &rectOld);
	point.x = rectOld.top;
	point.y = rectOld.left;
	ScreenToClient(hWndParent, &point);
	point.x = rectOld.bottom;
	point.y = rectOld.right;
	ScreenToClient(hWndParent, &point);
	if (EqualRect(&rectOld, lpRect))
		return;     // nothing to do

	// try to use DeferWindowPos for speed, otherwise use SetWindowPos
	if (lpLayout != NULL)
	{
		lpLayout->hDWP = DeferWindowPos(lpLayout->hDWP, hWnd, NULL,
			lpRect->left, lpRect->top,  lpRect->right - lpRect->left,
			lpRect->bottom - lpRect->top, SWP_NOACTIVATE|SWP_NOZORDER);
	}
	else
	{
		SetWindowPos(hWnd, NULL, lpRect->left, lpRect->top,
			lpRect->right - lpRect->left, lpRect->bottom - lpRect->top,
			SWP_NOACTIVATE|SWP_NOZORDER);
	}
}

void SwapLeftRight(LPRECT lpRect)
{
    LONG temp = lpRect->left;
    lpRect->left = lpRect->right;
    lpRect->right = temp;
}

void _FillSolidRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clr)
{
	RECT rect;
	
	assert(hDC != NULL);
	
	SetBkColor(hDC, clr);
	SetRect(&rect, x, y, x + cx, y + cy);
	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}

void Draw3dRect(HDC hDC, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	_Draw3dRect(hDC, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
		    lpRect->bottom - lpRect->top, clrTopLeft, clrBottomRight);
}

void _Draw3dRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
	_FillSolidRect(hDC, x, y, cx - 1, 1, clrTopLeft);
	_FillSolidRect(hDC, x, y, 1, cy - 1, clrTopLeft);
	_FillSolidRect(hDC, x + cx, y, -1, cy, clrBottomRight);
	_FillSolidRect(hDC, x, y + cy, cx, -1, clrBottomRight);
}

void FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr)
{
	assert(hDC != NULL);

	SetBkColor(hDC, clr);
	ExtTextOut(hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
}

void CalcWindowRect(HWND hWnd, LPRECT lpClientRect, UINT nAdjustType)
{
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	DWORD dwExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	if (nAdjustType == 0)
		dwExStyle &= ~WS_EX_CLIENTEDGE;
	AdjustWindowRectEx(lpClientRect, dwStyle, FALSE, dwExStyle);
}

HBRUSH _HalftoneBrush = 0;

HBRUSH GetHalftoneBrush()
{
	if (_HalftoneBrush == NULL) {
		int i;
		HBITMAP grayBitmap;
		WORD grayPattern[8];
		for (i = 0; i < 8; i++)
			grayPattern[i] = (WORD)(0x5555 << (i & 1));
		grayBitmap = CreateBitmap(8, 8, 1, 1, &grayPattern);
		if (grayBitmap != NULL) {
			_HalftoneBrush = CreatePatternBrush(grayBitmap);
			DeleteObject(grayBitmap);
		}
	}
	return _HalftoneBrush;
}
