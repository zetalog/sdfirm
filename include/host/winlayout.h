#ifndef __WIN_LAYOUT_H_INCLUDE__
#define __WIN_LAYOUT_H_INCLUDE__

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <prsht.h>

#define MAX_LOADSTRING	100
#define TOP		0x00
#define LEFT		0x01
#define BOTTOM		0x02
#define RIGHT		0x03
#define VIEW_TOOLBAR	0x00000001
#define VIEW_STATUSBAR	0x00000002

#define WM_PROGRESS	WM_USER+1
#define WM_LAYOUTUSER	WM_USER+1

#define CCHMAX(sz)      (sizeof (sz) / sizeof (TCHAR))

typedef struct tagWINWIZARDINFO {
	HWND hwndWizard;
	HWND hwndParent;
	HICON hIcon;
	HFONT hfontIntro;
	HPALETTE hPalette;
	HBITMAP hBitmap;
	LPTSTR lpszIntro;
	ULONG lParam;
	INT nResult;
} WINWIZARDINFO, FAR *LPWINWIZARDINFO;

typedef struct tagWINPROPERTYPAGE {
	UINT nPageID;
	DLGPROC lpfnPageProc;
} WINPROPERTYPAGE, *LPWINPROPERTYPAGE;

typedef struct tagWINTOOLBARITEM {
	BOOL bSeperator;
	INT nIndex;
	INT nID;
} WINTOOLBARITEM, *LPWINTOOLBARITEM;

#define DLG_WIZARD_TITLE_NONE		-1
VOID WINAPI DlgPaintWizard(HWND hWnd, LPWINWIZARDINFO wi);
INT WINAPI DlgLaunchWizard(HWND hwndParent, UINT nIntro, UINT nIcon, 
			   int iModPages, LPWINPROPERTYPAGE lpPages, ULONG lParam);

// progress
typedef struct tagWINPROGRESSDLG *WINPROGRESSHANDLE;
typedef BOOL (*WINPROGRESSCB)(WINPROGRESSHANDLE hHandle, HWND hWnd, UINT nStep, LPVOID pData);
BOOL WINAPI DlgShowProgress(HWND hWnd, UINT nSteps, WINPROGRESSCB pfnProgress, LPVOID pData);
VOID WINAPI DlgShowStatus(WINPROGRESSHANDLE hHandle, LPTSTR szMessage);

BOOL WINAPI DlgBrowseFile(HWND hwndParent, LPSTR pszFile, UINT nSize,
			  LPSTR *pszName, LPSTR *pszSufix, UINT nSufix,
			  BOOL bSave);
VOID WINAPI DlgDisplayError(HWND hwndParent, UINT uCaption, DWORD dwError);

#define LAYOUT_MAINFRAME	0
#define LAYOUT_POSITION		1
#define LAYOUT_MENUFLAGS	2
#define LAYOUT_PLACEMENT	3
#define LAYOUT_VIEW_TOOLBAR	4
#define LAYOUT_VIEW_STATUSBAR	5
#define LAYOUT_STATUS		6
#define LAYOUT_REBAR		7
#define LAYOUT_TOOLBAR		8
#define LAYOUT_TOOLBAR4BIT	9
#define LAYOUT_READY_STRING	10
#define LAYOUT_WIZARDINTRO	11
#define LAYOUT_WIZARDDONE	12
#define LAYOUT_PROGRESS		13
#define LAYOUT_WIZBITMAP	14
#define LAYOUT_INTRODUCTION	15
#define LAYOUT_MESSAGE		16
#define LAYOUT_WIZARD		17
#define LAYOUT_HEADER		18
#define LAYOUT_ERROR_STRING	19
#define LAYOUT_BROWSE_STRING	20
#define MAX_LAYOUT_IDS		21

extern HINSTANCE _hInstance;
extern TCHAR _szTitle[MAX_LOADSTRING];
extern TCHAR _szWindowClass[MAX_LOADSTRING];
extern ULONG _fMenuFlags;
extern INT _nIdMappings[MAX_LAYOUT_IDS];

/* Maxmum string length for MessageIDBox */
#define MAX_MESSAGE     1024
#define MAX_CAPTION     64

void MoveMainFrame(HWND hWnd, RECT *lpRect);
void RegSaveString(UINT nKey, UINT nVal, LPCSTR pszVal);
void RegDeleteString(UINT nKey, UINT nVal);
INT RegLoadString(UINT nKey, UINT nVal, LPSTR pszVal, UINT nSize);
INT RegLoadDword(UINT nKey, UINT nVal, INT *pdwValue);
void RegSaveDword(UINT nKey, UINT nVal, INT dwVal);
TCHAR *StringFromIDS(UINT unID);
VOID RestoreWindowPosition(HWND hWnd);
void RecalcLayout(HWND hWnd);
VOID SaveWindowPosition(HWND hWnd);
LRESULT CommandSelected(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
void DestroyToolbar(HWND hwndParent);
HWND BuildToolbar(HWND hwndParent, INT nTools, LPWINTOOLBARITEM pTools);
VOID GetToolbarText(HWND hwndToolbar, LPTOOLTIPTEXT lpttt);
HWND BuildRebar(HWND hwndParent, INT nTools, LPWINTOOLBARITEM pTools);
void MoveRebar(HWND hWnd, RECT *lpRect);
HWND BuildStatus(HWND hwndParent);
void MoveStatus(HWND hWnd, RECT *lpRect);
VOID DisplayStatus(HWND hwndStatus, LPCTSTR lpszFormat, ...);
int MessageIDBox(HWND hWnd, UINT uMessage, UINT uCaption, UINT uType);
void WINAPI CenterChild(HWND hwndChild, HWND hwndParent);

#endif /* __WIN_LAYOUT_H_INCLUDE__ */
