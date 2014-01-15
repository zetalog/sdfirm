#ifndef __LAYOUT_H_INCLUDE__
#define __LAYOUT_H_INCLUDE__

#define MAX_LOADSTRING	100
#define TOP		0x00
#define LEFT		0x01
#define BOTTOM		0x02
#define RIGHT		0x03
#define VIEW_TOOLBAR	0x00000001
#define VIEW_STATUSBAR	0x00000002

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

extern HINSTANCE _hInstance;
extern TCHAR _szTitle[MAX_LOADSTRING];
extern TCHAR _szWindowClass[MAX_LOADSTRING];
extern ULONG _fMenuFlags;

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
HWND BuildToolbar(HWND hwndParent);
VOID GetToolbarText(HWND hwndToolbar, LPTOOLTIPTEXT lpttt);
HWND BuildRebar(HWND hwndParent);
void MoveRebar(HWND hWnd, RECT *lpRect);
HWND BuildStatus(HWND hwndParent);
void MoveStatus(HWND hWnd, RECT *lpRect);
VOID DisplayStatus(HWND hwndStatus, LPCTSTR lpszFormat, ...);
int MessageIDBox(HWND hWnd, UINT uMessage, UINT uCaption, UINT uType);
void WINAPI CenterChild(HWND hwndChild, HWND hwndParent);

#endif /* __LAYOUT_H_INCLUDE__ */
