#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0400
#include <limits.h>
#include <windows.h>
#include <commctrl.h>
#include "ECRes.h"
#include <host/extdctrl.h>

// special struct for WM_SIZEPARENT
typedef struct tagSIZEPARENTPARAMS
{
	HDWP hDWP;       // handle for DeferWindowPos
	RECT rect;       // parent client rectangle (trim as appropriate)
	SIZE sizeTotal;  // total size on each side as layout proceeds
	BOOL bStretch;   // should stretch to fill all space
} SIZEPARENTPARAMS;

enum AdjustType { adjustBorder = 0, adjustOutside = 1 };

VOID _cdecl DebugPrint(const CHAR *lpFmt, ...);

BOOL ModifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags);
BOOL ModifyStyleEx(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags);
void RepositionWindow(SIZEPARENTPARAMS* lpLayout, HWND hWnd, LPCRECT lpRect);
void FillSolidRect(HDC hDC, LPCRECT lpRect, COLORREF clr);
void _FillSolidRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clr);
void Draw3dRect(HDC hDC, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);
void _Draw3dRect(HDC hDC, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);
void CalcWindowRect(HWND hWnd, LPRECT lpClientRect, UINT nAdjustType);
HBRUSH GetHalftoneBrush();
void SwapLeftRight(LPRECT lpRect);
BOOL IsRuntimeClass(HWND hWnd, LPSTR lpszClass);

extern COLORREF clrBtnFace;
extern COLORREF clrBtnShadow;
extern COLORREF clrBtnHilite;
extern COLORREF clrBtnText;
extern COLORREF clrWindowFrame;
extern HBRUSH hbrWindowFrame;
extern HBRUSH hbrBtnFace;

HINSTANCE LIBINSTANCE;
