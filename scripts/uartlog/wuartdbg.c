// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <assert.h>
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include "resource.h"
#include <host/uartdbg.h>

#define IDC_REBAR			199
#define IDM_TOOLBAR			200
#define IDM_EXITAPP			201
#define IDM_CONNECT			202
#define IDM_DISCONNECT			203
#define	IDT_WAIT_OVERLAP		204

/* following codes are dedicated to my dear ZuiZui */
#ifdef UNICODE
#define CML_TSTRTOUTF8(x, y)					\
	ULONG l##y = x ? _tcslen(x)+1 : 0;			\
	y = NULL;						\
	y = (LPSTR)malloc(sizeof (CHAR) * (l##y+1) * 8);	\
	WideCharToMultiByte(CP_UTF8, 0, x, _tcslen(x)+1,	\
			    y, l##y * 8,			\
			    NULL, NULL);
#define CML_TSTRTOMULTI(x, y)					\
	ULONG l##y = x ? _tcslen(x)+1 : 0;			\
	y = NULL;						\
	y = (LPSTR)malloc(sizeof (CHAR) * (l##y+1) * 8);	\
	WideCharToMultiByte(CP_ACP, 0, x, _tcslen(x)+1,		\
			    y, l##y * 8,			\
			    NULL, NULL);
#define CML_TSTRFROMUTF8(x, y)					\
	ULONG l##x = x ? strlen(x) : 0;				\
	y = NULL;						\
	y = (LPTSTR)malloc(sizeof (WCHAR) * (l##x+1));		\
	MultiByteToWideChar(CP_UTF8, 0, x, -1,			\
			    (LPWSTR)y, l##x+1);
#define CML_TSTRFROMMULTI(x, y)					\
	ULONG l##x = x ? strlen(x) : 0;				\
	y = NULL;						\
	y = (LPTSTR)malloc(sizeof (WCHAR) * (l##x+1));		\
	MultiByteToWideChar(CP_ACP, 0, x, -1,			\
			    (LPWSTR)y, l##x+1);
#else
#define CML_TSTRTOUTF8(x, y)					\
	ULONG l##y = x ? strlen(x)+1 : 0;			\
	y = NULL;						\
	y = (LPSTR)malloc(sizeof (CHAR) * (l##y+1));		\
	strcpy(y, x);
#define CML_TSTRTOMULTI(x, y)					\
	ULONG l##y = x ? strlen(x)+1 : 0;			\
	y = NULL;						\
	y = (LPSTR)malloc(sizeof (CHAR) * (l##y+1));		\
	strcpy(y, x);
#define CML_TSTRFROMUTF8(x, y)					\
	ULONG l##x = x ? strlen(x) : 0;				\
	y = NULL;						\
	y = (LPTSTR)malloc(sizeof (CHAR) * (l##x+1));		\
	strcpy(y, x);
#define CML_TSTRFROMMULTI(x, y)					\
	ULONG l##x = x ? strlen(x) : 0;				\
	y = NULL;						\
	y = (LPTSTR)malloc(sizeof (CHAR) * (l##x+1));		\
	strcpy(y, x);
#endif

struct CmbListItem {
	LPCTSTR text;
	DWORD val;
};

typedef struct { 
	TCHAR szComName[MAX_PATH];

	HANDLE hComFile;
	OVERLAPPED oOverlap;
	HANDLE hEvents;
	BOOL fPendingIO; 

	/* byte to read */
	BYTE bData;
	DWORD cbRead;

	DWORD dwState;
#define COM_CLOSED	0x00
#define COM_OPENED	0x01
#define COM_RUNNING	0x02

	DWORD dwFastBytes;
	DWORD dwBaudRate;
	BYTE bParity, bByteSize, bStopBits;
} COMINST, *LPCOMINST; 

VOID InitGlobalVals();
VOID DestroyGlobalVals();
HWND CreateToolbar(HWND hWndParent);
HWND CreateRebar(HWND hWndParent);
HWND CreateSetList(HWND hWndParent);
VOID CreateEventView(HWND hWnd);
VOID DestroyRebar(HWND hWndParent);
VOID DestroyEventView(HWND hWndParent);
HWND CreateBaudRateList(HWND hToolBar);	
HWND CreateByteSizeList(HWND hWndToolbar);
HWND CreateParityList(HWND hWndToolbar);
HWND CreateStopBitsList(HWND hWndToolbar);
HWND UpdateComList(HWND hWndToolbar);
VOID SetEventViewSize(LPRECT lpRect, HWND hWnd);
VOID SetRebarSize(LPRECT lpRect, HWND hCtrl);
VOID SetLastLogSize(LPRECT lpRect, HWND hWnd);
BOOL OnConnect(HWND hWnd);
BOOL OnDisConnect(HWND hWnd);
VOID WriteLogToFile(LPCTSTR szTask, LPCTSTR szType, LPCTSTR szDetail);
VOID ClearLastLog(VOID);
VOID ResetLog(VOID);
VOID EnumSerialPort(VOID);
BOOL SerialConnect(VOID);
VOID SerialDisconnect(VOID);
VOID SerialInit(VOID);
BOOL SerialConfig(HWND hWnd);
VOID SerialRead(HWND hWnd);
VOID AddToEventView(DWORD dwModule, LPCTSTR szType, LPCTSTR szDetail);
CHAR *CML_tstr2multi(LPCTSTR string);
TCHAR *CML_multi2tstr(LPCSTR string);
TCHAR *CML_utf82tstr(LPCSTR string);
CHAR *CML_tstr2utf8(LPCTSTR string);
TCHAR *GetModulePath(VOID);

COMINST hCOMInst;
HICON g_hIcon = NULL;

HINSTANCE g_hInst;
HANDLE g_hLogFile;
DWORD g_ListViewCnt;
HWND g_hListView;
HWND g_hEditView;
DWORD g_EditCharCnt = 0;
HWND g_hwndSetList = NULL;
LPTSTR *g_pComNames = NULL;
DWORD g_dwComCount = 0;
DWORD g_dwComSize = 0;

#define WM_SERIAL		WM_USER+1

struct CmbListItem BaudRate[] =
{
	{_T("4800"), 4800 },
	{_T("9600"), 9600 },
	{_T("19200"), 19200 },
	{_T("38400"), 38400 },
	{_T("57600"), 57600 },
	{_T("115200"), 115200 },
	{_T("230400"), 230400 },
#if 0
	{_T("14400"), 14400 },
	{_T("56000"), 56000 },
	{_T("128000"), 128000 },
	{_T("250000"), 250000 },
	{_T("256000"), 256000 },
	{_T("500000"), 500000 },
	{_T("1000000"), 1000000 },
	{_T("1500000"), 1500000 },
	{_T("2000000"), 2000000 },
#endif
};

struct CmbListItem ByteSize[] =
{
	{_T("5"), 5},
	{_T("6"), 6},
	{_T("7"), 7},
	{_T("8"), 8},
};

struct CmbListItem Parity[] =
{
	{_T("无"), 0},
	{_T("奇校验"), 1},
	{_T("偶校验"), 2},
};

struct CmbListItem StopBits[] =
{
	{_T("1"), 0},
	{_T("1.5"), 1},
	{_T("2"), 2},
};

VOID SaveSerialPort(BYTE *szPortName)
{
	if (!g_pComNames) {
		g_pComNames = malloc(sizeof (LPTSTR) * 10);
		if (g_pComNames)
			g_dwComSize = 10;
	}
	if (g_dwComSize <= g_dwComCount) {
		g_pComNames = realloc(g_pComNames, sizeof (LPTSTR) * (g_dwComSize + 10));
		if (g_pComNames)
			g_dwComSize += 10;
	}
	if (g_pComNames) {
		CML_TSTRFROMMULTI(szPortName, g_pComNames[g_dwComCount]);
		g_dwComCount++;
	}
}

VOID FreeSerialPort(VOID)
{
	DWORD i;

	if (g_pComNames)
	{
		for (i = 0; i < g_dwComCount; i++)
		{
			if (g_pComNames[i] != NULL)
				free(g_pComNames[i]);
		}
		free(g_pComNames);
		g_pComNames = NULL;
		g_dwComCount = 0;
		g_dwComSize = 0;
	}
}

VOID EnumSerialPort(VOID)  
{
	HKEY hKey;
	LPCTSTR data_Set = _T("HARDWARE\\DEVICEMAP\\SERIALCOMM\\");
	DWORD dwIndex = 0;
	LONG ret;
	
	FreeSerialPort();

	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_READ, &hKey);
	if (ret != ERROR_SUCCESS)
		return;
	
	while (TRUE)
	{
		LONG Status;   
		TCHAR Name[256] = {0};   
		BYTE szPortName[80] = {0};
		DWORD dwName;   
		DWORD dwSizeofPortName;   
		DWORD Type;
		
		dwName = sizeof(Name);   
		dwSizeofPortName = sizeof(szPortName);   
		Status = RegEnumValue(hKey, dwIndex++, Name, &dwName, 
				      NULL, &Type, szPortName,
				      &dwSizeofPortName);   
		
		if ((Status == ERROR_SUCCESS) || (Status == ERROR_MORE_DATA)) {
			SaveSerialPort(szPortName);
		} else {
			break;
		}
	}
	RegCloseKey(hKey);
}

BOOL SerialConnect(VOID)
{
	TCHAR szComName[MAX_PATH];

	SerialDisconnect();
	
	if (_tcslen(hCOMInst.szComName) > 4) {
		sprintf(szComName, "\\\\.\\%s", hCOMInst.szComName);
	} else {
		sprintf(szComName, "%s", hCOMInst.szComName);
	}
	hCOMInst.hComFile = CreateFile(szComName, GENERIC_READ,
				       0, 0, OPEN_EXISTING,
				       FILE_FLAG_OVERLAPPED, 0);
	if (hCOMInst.hComFile == INVALID_HANDLE_VALUE) {
		DWORD dwError = GetLastError();
		return FALSE;
	}
	hCOMInst.hEvents = CreateEvent(NULL, FALSE, TRUE, NULL);
	if (!hCOMInst.hEvents) {
		SerialDisconnect();
		return FALSE;
	}
	hCOMInst.fPendingIO = FALSE;
	hCOMInst.oOverlap.hEvent = hCOMInst.hEvents;
	hCOMInst.dwState = COM_OPENED;

	return TRUE;
}

VOID SerialDisconnect(VOID)
{
	if (!hCOMInst.hEvents) {
		CloseHandle(hCOMInst.hEvents);
		hCOMInst.hEvents = NULL;
	}
	if (hCOMInst.hComFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hCOMInst.hComFile);
		hCOMInst.hComFile = INVALID_HANDLE_VALUE;
	}
	hCOMInst.dwState = COM_CLOSED;
}

BOOL SerialConfig(HWND hWnd)
{
	DCB dcb;

	if (hCOMInst.hComFile == INVALID_HANDLE_VALUE)
		return FALSE;

	if (hCOMInst.dwState == COM_RUNNING) {
		return TRUE;
	}
	
	ZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	if (!GetCommState(hCOMInst.hComFile, &dcb))
		return FALSE;
	
	dcb.BaudRate = hCOMInst.dwBaudRate;   // set the baud rate
	dcb.ByteSize = hCOMInst.bByteSize;    // data size, xmit, and rcv
	dcb.Parity = hCOMInst.bParity;        // no parity bit
	dcb.StopBits = hCOMInst.bStopBits;    // one stop bit
	
	if (!SetCommState(hCOMInst.hComFile, &dcb))
		return FALSE;

	hCOMInst.dwState = COM_RUNNING;
	hCOMInst.dwFastBytes = 0;
	PostMessage(hWnd, WM_SERIAL, 0, 0);
	return TRUE;
}

VOID SerialInit(VOID)
{
	ZeroMemory(&hCOMInst, sizeof (COMINST));
	hCOMInst.dwState = COM_CLOSED;
}

#define MAX_FAST_BYTES		64

VOID ProcessLog(HWND hWnd)
{
	/* data arrived, go fast path */
	dbg_process_log(NULL, hCOMInst.bData);
	if (hCOMInst.dwFastBytes++ < MAX_FAST_BYTES)
		PostMessage(hWnd, WM_SERIAL, 0, 0);
	else
		hCOMInst.dwFastBytes = 0;
}

VOID SerialRead(HWND hWnd)
{
	DWORD dwRet, cbRet;
	BOOL fSuccess;

	dwRet = WaitForSingleObject(hCOMInst.hEvents, 0);
	if (hCOMInst.fPendingIO == TRUE && dwRet != WAIT_OBJECT_0) {
		/* no event, go slow path */
		return;
	}
	if (hCOMInst.fPendingIO == TRUE) {
		fSuccess = GetOverlappedResult(hCOMInst.hComFile,
					       &hCOMInst.oOverlap,
					       &cbRet, FALSE);
		if (!fSuccess) {
			SerialDisconnect();
			return;
		}
		hCOMInst.cbRead += cbRet;

		ProcessLog(hWnd);
		hCOMInst.fPendingIO = FALSE;
		return;
	} else {
		hCOMInst.cbRead = 0;
		fSuccess = ReadFile(hCOMInst.hComFile,
				    &hCOMInst.bData,
				    1,
				    &hCOMInst.cbRead,
				    &hCOMInst.oOverlap);
		if (fSuccess && hCOMInst.cbRead != 0) {
			ProcessLog(hWnd);
			return;
		} else {
			DWORD dwErr = GetLastError();
			if (dwErr == ERROR_HANDLE_EOF) {
				SerialDisconnect();
				return;
			} else if (dwErr == ERROR_IO_PENDING) {
				hCOMInst.fPendingIO = TRUE;
			} else {
				SerialDisconnect();
			}
		}
	}
}

void UpdateWindowSize(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	SetRebarSize(&rect, GetDlgItem(hWnd, IDC_REBAR));
	SetEventViewSize(&rect, hWnd);
	SetLastLogSize(&rect, hWnd);
}

static VOID UnsetDialogIcon(HWND hWnd)
{
	if (g_hIcon)
		DestroyIcon(g_hIcon);
}

static VOID SetDialogIcon(HWND hWnd)
{
	g_hIcon = LoadImage(g_hInst,
			    MAKEINTRESOURCE(IDI_UARTLOG),
			    IMAGE_ICON,
			    GetSystemMetrics(SM_CXSMICON),
			    GetSystemMetrics(SM_CYSMICON),
			    0);
	assert(g_hIcon);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)g_hIcon);
}

INT_PTR CALLBACK ConfDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) 
	{
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_COMBO_COM:
			if (HIWORD(wParam) == CBN_DROPDOWN) {
				UpdateComList(g_hwndSetList);
			}
			break;
		}
		break;
	}
	return 0;
}

INT_PTR CALLBACK MainDialogProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message) 
	{
	case WM_ERASEBKGND:
		return 1;

	case WM_INITDIALOG:
		InitGlobalVals();
		SetDialogIcon(hWnd);
		CreateRebar(hWnd);
		CreateEventView(hWnd);
		UpdateWindowSize(hWnd);
		return TRUE;

	case WM_SIZE:
		UpdateWindowSize(hWnd);
		break;

	case WM_SERIAL:
		/* faster path */
		SerialRead(hWnd);
		break;

	case WM_TIMER:
		if (wParam == IDT_WAIT_OVERLAP) {
			switch (hCOMInst.dwState) {
			case COM_CLOSED:
				SerialConnect();
				break;
			case COM_OPENED:
				SerialConfig(hWnd);
				break;
			default:
				/* slow path */
				SerialRead(hWnd);
				break;
			}
		}
		break;

	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_EXITAPP:
			DestroyWindow(hWnd);
			break;
		case IDM_CONNECT:
			OnConnect(hWnd);
			break;
		case IDM_DISCONNECT:
			OnDisConnect(hWnd);
			break;
		default:
			return 0;
		}
		break;

	case WM_CLOSE:
		KillTimer(hWnd, IDT_WAIT_OVERLAP);
		DestroyWindow(hWnd); 
		return TRUE;

	case WM_DESTROY:
		DestroyEventView(hWnd);
		DestroyRebar(hWnd);
		DestroyGlobalVals();
		UnsetDialogIcon(hWnd);
		PostQuitMessage(0);
		return TRUE;
	default:
		return 0;
	}
	return 0;
}

#define UART_CHARS_PER_LINE	16

static VOID Edit_AddText(HANDLE hEdit, LPCSTR szAdd)
{
	INT nLength;

	nLength = Edit_GetTextLength(hEdit);
	Edit_SetSel(hEdit, nLength, nLength);
	Edit_ReplaceSel(hEdit, szAdd);
}

static void WindowsLast(void *ctx, const unsigned char *szNew, size_t dwToStore)
{
	DWORD nIndex, nLine = 0;
	INT nSize = dwToStore*3 + 1;
	unsigned char *szAdd = malloc(nSize);

	if (szAdd) {
		memset(szAdd, 0, nSize);
		for (nIndex = 0; nIndex < dwToStore; nIndex++) {
			if (((g_EditCharCnt % UART_CHARS_PER_LINE) == 0) &&
			    (g_EditCharCnt != 0)) {
				nLine = nIndex;
				Edit_AddText(g_hEditView, "\r\n");
			}
			sprintf(szAdd, "%02x ", szNew[nIndex-nLine]);
			Edit_AddText(g_hEditView, szAdd);
			g_EditCharCnt++;
		}
		free(szAdd);
	}
}

static void WindowsDump(void *ctx, dbg_cmd_t bCmd, const char *szFormat, ...)
{
	FILE *pFile = (FILE *)ctx;
	va_list argList;
	unsigned char szBuf[1024];
	TCHAR *szDetail;
	TCHAR *szEvtName;
	
	va_start(argList, szFormat );
	vsprintf(szBuf, szFormat, argList);
	va_end(argList);

	szDetail = CML_multi2tstr(szBuf);
	szEvtName = CML_multi2tstr(dbg_event_name(bCmd));
	if (szDetail && szEvtName) {
		TCHAR *szTask = CML_multi2tstr(dbg_state_name(bCmd));
		AddToEventView(DBG_STATE_ID(bCmd), szEvtName, szDetail);
		ClearLastLog();
		if (szTask) {
			WriteLogToFile(szTask, szEvtName, szDetail);
			free(szTask);
		}
	}
	if (szDetail) free(szDetail);
	if (szEvtName) free(szEvtName);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HWND hDlg;

	g_hInst = hInstance;
	hDlg = CreateDialog(hInstance, (LPCTSTR)IDD_MAIN, NULL, (DLGPROC)MainDialogProc);
	if (hDlg == NULL)
		return FALSE;

	SerialInit();
	dbg_init();
	dbg_register_output(WindowsDump, WindowsLast);
	ShowWindow(hDlg, nCmdShow);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	dbg_exit();

	return msg.wParam;
}

void SetDefaultBaudRate(HWND hWndCombo, DWORD dwBaudRate)
{
	int idx;

	for (idx = 0; idx < sizeof(BaudRate)/sizeof(BaudRate[0]); idx++) {
		if (BaudRate[idx].val == dwBaudRate) {
			SendMessage(hWndCombo, CB_SETCURSEL, idx, 0);
			break;
		}
	}
}

HWND CreateBaudRateList(HWND hWndToolbar)
{
	HWND hWndCombo;
	int idx;

	hWndCombo = GetDlgItem(hWndToolbar, IDC_COMBO_BAUD);
	if (hWndCombo == NULL)
		return NULL;
	SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0);
	for (idx = 0; idx < sizeof(BaudRate)/sizeof(BaudRate[0]); idx++)
		SendMessage(hWndCombo, CB_ADDSTRING,
			    (WPARAM)0, (LPARAM)BaudRate[idx].text);
	SetDefaultBaudRate(hWndCombo, 115200);
	return hWndCombo;
}

HWND CreateParityList(HWND hWndToolbar)
{
	HWND hWndCombo;
	int idx;

	hWndCombo = GetDlgItem(hWndToolbar, IDC_COMBO_PARITY);
	if (hWndCombo == NULL)
		return NULL;
	SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0); 
	for (idx = 0; idx < sizeof(Parity)/sizeof(Parity[0]); idx++)
		SendMessage(hWndCombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)Parity[idx].text);
	SendMessage(hWndCombo, CB_SETCURSEL, 0, 0); 
	return hWndCombo;
}

HWND CreateByteSizeList(HWND hWndToolbar)
{
	HWND hWndCombo;
	int idx;

	hWndCombo = GetDlgItem(hWndToolbar, IDC_COMBO_BIT);
	if (hWndCombo == NULL)
		return NULL;
	SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0); 
	for (idx = 0; idx < sizeof(ByteSize)/sizeof(ByteSize[0]); idx++)
		SendMessage(hWndCombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)ByteSize[idx].text);
	SendMessage(hWndCombo, CB_SETCURSEL, 3, 0); 
	return hWndCombo;
}

HWND CreateStopBitsList(HWND hWndToolbar)
{
	HWND hWndCombo;
	int idx;

	hWndCombo = GetDlgItem(hWndToolbar, IDC_COMBO_STOPBIT);
	if (hWndCombo == NULL)
		return NULL;

	SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0); 
	for (idx = 0; idx < sizeof(StopBits)/sizeof(StopBits[0]); idx++)
		SendMessage(hWndCombo, CB_ADDSTRING, (WPARAM)0, (LPARAM)StopBits[idx].text);
	SendMessage(hWndCombo, CB_SETCURSEL, 0, 0); 
	return hWndCombo;
}

HWND UpdateComList(HWND hWndToolbar)
{
	HWND hWndCombo;
	DWORD i;

	EnumSerialPort();

	hWndCombo = GetDlgItem(hWndToolbar, IDC_COMBO_COM);
	if (hWndCombo == NULL)
		return NULL;

	SendMessage(hWndCombo, CB_RESETCONTENT, 0, 0); 
	for (i = 0; i < g_dwComCount; i++)
	{
		if (g_pComNames[i] != NULL)
			SendMessage(hWndCombo, CB_ADDSTRING,
				    (WPARAM)0, (LPARAM)g_pComNames[i]);
	}
	SendMessage(hWndCombo, CB_SETCURSEL, 0, 0); 
	return hWndCombo;
}

HWND CreateSetList(HWND hWndParent)
{
	g_hwndSetList = CreateDialog(g_hInst,
				     (LPCTSTR)IDD_COM_PARAM, 
				     hWndParent,
				     (DLGPROC)ConfDialogProc);
	if (g_hwndSetList == NULL)
		return NULL;
	
	CreateBaudRateList(g_hwndSetList);
	CreateParityList(g_hwndSetList);
	CreateByteSizeList(g_hwndSetList);
	CreateStopBitsList(g_hwndSetList);
	UpdateComList(g_hwndSetList);
	return g_hwndSetList;
}

HWND CreateToolbar(HWND hWndParent)
{
	HWND hwndToolBar;
	int ret;
	HIMAGELIST hImageList;
	HBITMAP hBitmap;
	TBBUTTON tbButtons[] = 
	{
		{ 0, IDM_EXITAPP, TBSTATE_ENABLED, TBSTYLE_AUTOSIZE, {0}, 0, (INT_PTR)_T("Exit UARTLog") },
		{ 1, IDM_CONNECT, TBSTATE_ENABLED, TBSTYLE_AUTOSIZE, {0}, 0, (INT_PTR)_T("Connect UART") },
		{ 2, IDM_DISCONNECT, TBSTATE_ENABLED, TBSTYLE_AUTOSIZE, {0}, 0, (INT_PTR)_T("Disconnect UART") }
	};
	
	hwndToolBar = CreateWindowEx(WS_EX_TOOLWINDOW, TOOLBARCLASSNAME, NULL, 
		WS_CHILD | CCS_NOPARENTALIGN | CCS_NODIVIDER |
		CCS_NORESIZE | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS,
		0, 0, 0, 0,
		hWndParent, (HMENU)IDM_TOOLBAR, g_hInst, NULL);
	
	if (hwndToolBar == NULL)
		return NULL;
	
        {
            hImageList = ImageList_Create(16, 16, ILC_COLOR|ILC_MASK,
                                          sizeof(tbButtons)/sizeof(tbButtons[0]), 0); 
            hBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_TOOLBAR4BIT));
            ImageList_AddMasked(hImageList, hBitmap, RGB(255, 0, 255));
            DeleteObject(hBitmap);
        }
	
	// Set the image list.
	SendMessage(hwndToolBar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImageList);
	
	// Add buttons.
	SendMessage(hwndToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof (TBBUTTON), 0);
	ret = SendMessage(hwndToolBar, TB_ADDBUTTONS, (WPARAM)(sizeof (tbButtons) / sizeof (tbButtons[0])), 
		(LPARAM)&tbButtons);
	SendMessage(hwndToolBar, TB_SETMAXTEXTROWS, (WPARAM)0, (LPARAM)0);
	SendMessage(hwndToolBar, TB_SETBUTTONSIZE, 0, MAKELONG(20, 20));
	
	// Tell the toolbar to resize itself, and show it.
	//SendMessage(hwndToolBar, TB_AUTOSIZE, 0, 0); 
	SendMessage(hwndToolBar, TB_ENABLEBUTTON, IDM_CONNECT, TRUE);
	SendMessage(hwndToolBar, TB_ENABLEBUTTON, IDM_DISCONNECT, FALSE);
	
	return hwndToolBar;
}

VOID CreateEventView(HWND hWnd)
{
	LVCOLUMN col;
	HIMAGELIST hImageList;
	HBITMAP hBitmap;

	g_hListView = GetDlgItem(hWnd, IDC_EVT_VIEW);
	if (g_hListView == NULL)
		return;
	g_hEditView = GetDlgItem(hWnd, IDC_LAST_LOG);
	if (g_hEditView == NULL)
		return;
	
	//SetWindowPos(hListView, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	col.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	col.fmt = LVCFMT_LEFT;
	col.cx = 20;
	col.pszText = _T("");
	col.iSubItem = 0;
	ListView_InsertColumn(g_hListView, 0, &col);
	
	col.fmt = LVCFMT_LEFT;
	col.cx = 60;
	col.pszText = _T("类别");
	col.iSubItem = 1;
	ListView_InsertColumn(g_hListView, 1, &col);
	
	col.fmt = LVCFMT_LEFT;
	col.cx = 800;
	col.pszText = _T("详细");
	col.iSubItem = 2;
	ListView_InsertColumn(g_hListView, 2, &col);

	hImageList = ImageList_Create(16, 16, ILC_COLOR16 | ILC_MASK,
				      16, 0);
	hBitmap = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_LOGLIST4BIT));
	ImageList_AddMasked(hImageList, hBitmap, RGB(255, 0, 255));
	DeleteObject(hBitmap);

	ListView_SetImageList(g_hListView, hImageList, LVSIL_SMALL); 
	ListView_SetExtendedListViewStyle(g_hListView, LVS_EX_FULLROWSELECT);
	SendMessage(g_hListView, LVM_SETITEMCOUNT, (WPARAM)500, 0);
	g_ListViewCnt = 0;
}

void DestroyEventView(HWND hWnd)
{
	HIMAGELIST hImageList;

	hImageList = (HIMAGELIST)SendMessage(g_hListView,
					     LVM_GETIMAGELIST,
					     LVSIL_SMALL, 0);
	if (hImageList != NULL)
		ImageList_Destroy(hImageList);
}

void InitGlobalVals()
{
	TCHAR szFile[MAX_PATH + 12 * sizeof (TCHAR)];

	g_pComNames = NULL;
	g_dwComCount = 0;
	g_dwComSize = 0;

	/* open file to write into loginfo */
	g_hLogFile = INVALID_HANDLE_VALUE;
	memset(szFile, 0, sizeof (szFile));
	_tcscat(szFile, GetModulePath());
	_tcscat(szFile, _T("\\uartdbg.log"));
	g_hLogFile = CreateFile(szFile, GENERIC_READ | GENERIC_WRITE, 
				FILE_SHARE_READ, 0, CREATE_ALWAYS,
				FILE_FLAG_WRITE_THROUGH, 0);
}

void DestroyGlobalVals()
{
	DWORD i = 0;

	if (g_hLogFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(g_hLogFile);
		g_hLogFile = INVALID_HANDLE_VALUE;
	}
	FreeSerialPort();
}

BOOL OnConnect(HWND hWnd)
{
	HWND hToolBar, hSetList, hList;
	REBARBANDINFO rbBand;
	DWORD dwBaudRate;
	BYTE bParity, bByteSize, bStopBits;
	LRESULT lResult = CB_ERR;
	HWND hReBar;
	TCHAR szComName[MAX_PATH+1];

	ListView_DeleteAllItems(g_hListView);
	ClearLastLog();
	
	hReBar = GetDlgItem(hWnd, IDC_REBAR);
	assert(hReBar);
	hToolBar = GetDlgItem(hReBar, IDM_TOOLBAR);
	assert(hToolBar);

	/* Get Com Info */
	rbBand.fMask = RBBIM_CHILD;
	rbBand.cbSize = sizeof (REBARBANDINFO);

	if (!SendMessage(hReBar, RB_GETBANDINFO, (WPARAM)1, (LPARAM)&rbBand))
		return FALSE;

	hSetList = rbBand.hwndChild;
	assert(hSetList);

	hList = GetDlgItem(hSetList, IDC_COMBO_COM);
	assert(hList);

	GetWindowText(hList, szComName, MAX_PATH);

	/* Get BaudRate Info */
	hList = GetDlgItem(hSetList, IDC_COMBO_BAUD);
	assert(hList);

	lResult = SendMessage(hList, CB_GETCURSEL, 0, 0);
	if (lResult == CB_ERR)
		return FALSE;
	dwBaudRate = (DWORD)BaudRate[lResult].val;

	/* Get ByteSize Info */
	hList = GetDlgItem(hSetList, IDC_COMBO_BIT);
	if (hList == NULL)
		return FALSE;
	lResult = SendMessage(hList, CB_GETCURSEL, 0, 0);
	if (lResult == CB_ERR)
		return FALSE;
	bByteSize = (BYTE)ByteSize[lResult].val;

	/* Get Parity Info */
	hList = GetDlgItem(hSetList, IDC_COMBO_PARITY);
	assert(hList);

	lResult = SendMessage(hList, CB_GETCURSEL, 0, 0);
	if (lResult == CB_ERR)
		return FALSE;
	bParity = (BYTE)Parity[lResult].val;

	/* Get StopBits Info */
	hList = GetDlgItem(hSetList, IDC_COMBO_STOPBIT);
	assert(hList);

	lResult = SendMessage(hList, CB_GETCURSEL, 0, 0);
	if (lResult == CB_ERR)
		return FALSE;
	bStopBits = (BYTE)StopBits[lResult].val;

	lstrcpy(hCOMInst.szComName, szComName);
	hCOMInst.dwBaudRate = dwBaudRate;
	hCOMInst.bParity = bParity;
	hCOMInst.bByteSize = bByteSize;
	hCOMInst.bStopBits = bStopBits;

	ResetLog();

	SetTimer(hWnd, IDT_WAIT_OVERLAP, 1, NULL);
	/* Open Success */
	SendMessage(hToolBar, TB_ENABLEBUTTON, IDM_CONNECT, FALSE);
	SendMessage(hToolBar, TB_ENABLEBUTTON, IDM_DISCONNECT, TRUE);
	return TRUE;
}

BOOL OnDisConnect(HWND hWnd)
{
	HWND hReBar;
	HWND hToolBar;

	KillTimer(hWnd, IDT_WAIT_OVERLAP);

	/* Close Comm port */
	SerialDisconnect();

	hReBar = GetDlgItem(hWnd, IDC_REBAR);
	assert(hReBar);
	hToolBar = GetDlgItem(hReBar, IDM_TOOLBAR);
	assert(hToolBar);

	/* Close Success */
	SendMessage(hToolBar, TB_ENABLEBUTTON, IDM_CONNECT, TRUE);
	SendMessage(hToolBar, TB_ENABLEBUTTON, IDM_DISCONNECT, FALSE);
	return TRUE;
}

#define TOP    0x00
#define LEFT   0x01
#define BOTTOM 0x02
#define RIGHT  0x03

DWORD _dwRebarSide = TOP;

HWND CreateRebar(HWND hWndParent)
{
	HWND hwndRebar, hwndToolBar, hwndSetList;
	DWORD dwBtnSize;
	RECT rc;
	INITCOMMONCONTROLSEX icex;
	REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };
	
	// Initialize common controls.
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC   = ICC_COOL_CLASSES | ICC_BAR_CLASSES;
	InitCommonControlsEx(&icex);
	
	// Create the rebar.
	hwndRebar = CreateWindowEx(WS_EX_TOOLWINDOW,
				   REBARCLASSNAME, 
				   NULL,
				   WS_VISIBLE |
				   WS_BORDER | 
				   WS_CHILD | 
				   WS_CLIPCHILDREN | 
				   WS_CLIPSIBLINGS | 
				   RBS_VARHEIGHT | 
				   RBS_BANDBORDERS | 
				   //_dwRebarSide is odd if this is a vertical bar
				   ((_dwRebarSide & 0x01) ? CCS_VERT : 0) |
				   ((_dwRebarSide == BOTTOM) ? CCS_BOTTOM : 0) |
				   ((_dwRebarSide == RIGHT) ? CCS_RIGHT : 0) |
				   0,
				   0, 
				   0, 
				   200, 
				   32, 
				   hWndParent, 
				   (HMENU)IDC_REBAR, 
				   g_hInst, 
				   NULL);
	
	if (!hwndRebar)
	{
		return NULL;
	}
	
	hwndToolBar = CreateToolbar(hwndRebar);
	hwndSetList = CreateSetList(hwndRebar);
	// Initialize band info used by both bands.
	rbBand.fMask  = 
		RBBIM_STYLE       // fStyle is valid.
		| RBBIM_TEXT        // lpText is valid.
		| RBBIM_CHILD       // hwndChild is valid.
		| RBBIM_CHILDSIZE   // child size members are valid.
		| RBBIM_SIZE       // cx is valid
		| RBBIM_ID;
	rbBand.fStyle = RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS | RBBS_BREAK; //RBBS_FIXEDSIZE
	
	// Get the height of the toolbar.
	dwBtnSize = (DWORD)SendMessage(hwndToolBar, TB_GETBUTTONSIZE, 0, 0);
	
	// Set values unique to the band with the toolbar.
	rbBand.lpText = TEXT("");
	rbBand.hwndChild = hwndToolBar;
	rbBand.cyChild = HIWORD(dwBtnSize);
	rbBand.cxMinChild = LOWORD(dwBtnSize);
	rbBand.cyMinChild = HIWORD(dwBtnSize);
	// The default width is the width of the buttons.
	rbBand.cx = LOWORD(dwBtnSize) * 2 + 15;
	rbBand.wID = IDM_TOOLBAR;
	
	// Add the band that has the toolbar.
	SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
	
	// Set values unique to the band with the combo box.
	GetWindowRect(hwndSetList, &rc);
	rbBand.lpText = TEXT("");
	rbBand.hwndChild = hwndSetList;
	rbBand.cyChild = rc.bottom - rc.top;
	rbBand.cxMinChild = 100;
	rbBand.cyMinChild = rc.bottom - rc.top;
	// The default width should be set to some value wider than the text. The combo 
	// box itself will expand to fill the band.
	rbBand.cx = rc.right - rc.left;
	rbBand.wID = IDD_COM_PARAM;
	
	// Add the band that has the combo box.
	SendMessage(hwndRebar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
	return (hwndRebar);
}

void DestroyRebar(HWND hWndParent)
{
	HWND hReBar = GetDlgItem(hWndParent, IDC_REBAR);
	HIMAGELIST hImageList;
	HWND hToolBar;

	if (hReBar == NULL)
	{
		return;
	}
	hToolBar = GetDlgItem(hReBar, IDM_TOOLBAR);
	if (hToolBar == NULL)
	{
		return;
	}
	hImageList = (HIMAGELIST)SendMessage(hToolBar, TB_GETIMAGELIST, 0, 0);
	ImageList_Destroy(hImageList);
}

VOID ResetLog(VOID)
{
	SetFilePointer(g_hLogFile, 0, 0, FILE_BEGIN);
	SetEndOfFile(g_hLogFile);
}

VOID ClearLastLog(VOID)
{
	SetWindowText(g_hEditView, _T(""));
	g_EditCharCnt = 0;
}

VOID AddToEventView(DWORD dwModule, LPCTSTR szType, LPCTSTR szDetail)
{
	LVITEM lvI;
	DWORD dwItemIdx;

	if (g_hListView == NULL)
	{
		return;
	}
	dwItemIdx = g_ListViewCnt++;

	lvI.mask = LVIF_IMAGE; 
	lvI.iItem = 0;
	lvI.iSubItem = 0;
	lvI.iImage = dwModule;
	ListView_InsertItem(g_hListView, &lvI);

	lvI.mask = LVIF_TEXT; 
	lvI.iItem = 0;
	lvI.iSubItem = 1;
	lvI.pszText = (LPTSTR)szType;
	SendMessage(g_hListView, LVM_SETITEM, 0, (LPARAM)&lvI);

	lvI.mask = LVIF_TEXT; 
	lvI.iItem = 0;
	lvI.iSubItem = 2;
	lvI.pszText = (LPTSTR)szDetail;
	SendMessage(g_hListView, LVM_SETITEM, 0, (LPARAM)&lvI);
}

void WriteLogToFile(LPCTSTR szTask, LPCTSTR szType, LPCTSTR szDetail)
{
	TCHAR *szBuf = NULL;
	DWORD dwWrite = 0;
	DWORD dwLen = _tcslen(szTask) + _tcslen(szType) +
		      _tcslen(szDetail) + sizeof (TCHAR) * 4;

	szBuf = (TCHAR *)malloc(sizeof (TCHAR) * (dwLen + 1));
	if (szBuf == NULL)
		return;

	memset(szBuf, 0x00, dwLen * sizeof (TCHAR));
	_tcscat(szBuf, szTask);
	_tcscat(szBuf, TEXT(" "));
	_tcscat(szBuf, szType);
	_tcscat(szBuf, TEXT(" "));
	_tcscat(szBuf, szDetail);
	_tcscat(szBuf, TEXT("\r\n"));
	WriteFile(g_hLogFile, szBuf, dwLen, &dwWrite, NULL);
}

VOID SetEventViewSize(LPRECT lpRect, HWND hWnd)
{
	RECT rcBar;
	int Height, Width, Margin;

	GetWindowRect(GetDlgItem(hWnd, IDC_REBAR), &rcBar);
	Margin = rcBar.bottom - rcBar.top - 4;
	Height = 2*(lpRect->bottom - lpRect->top - Margin)/3;
	Width = (lpRect->right - lpRect->left);

	SetWindowPos(GetDlgItem(hWnd, IDC_EVT_VIEW), HWND_TOP,
		     0, Margin,
		     Width, Height, SWP_NOZORDER);
}

VOID SetLastLogSize(LPRECT lpRect, HWND hWnd)
{
	RECT rcBar;
	RECT rcEvt;
	int Height, Width, Margin;

	GetWindowRect(GetDlgItem(hWnd, IDC_REBAR), &rcBar);
	GetWindowRect(GetDlgItem(hWnd, IDC_EVT_VIEW), &rcEvt);
	Margin = (rcBar.bottom - rcBar.top-4) + (rcEvt.bottom - rcEvt.top);
	Height = (lpRect->bottom - lpRect->top) - Margin;
	Width = (lpRect->right - lpRect->left);

	SetWindowPos(GetDlgItem(hWnd, IDC_LAST_LOG), HWND_TOP,
		     0, Margin,
		     Width, Height, SWP_NOZORDER);
}

VOID SetRebarSize(LPRECT lpRect, HWND hCtrl)
{
	int iWidth = lpRect->right - lpRect->left;
	REBARBANDINFO rbBand = { sizeof(REBARBANDINFO) };

	rbBand.fMask  = RBBIM_SIZE;       // cx is valid
	rbBand.cx = iWidth;

	SendMessage(hCtrl, RB_SETBANDINFO, 0, (LPARAM)&rbBand);
	SendMessage(hCtrl, RB_SETBANDINFO, 1, (LPARAM)&rbBand);
}

LPSTR CML_tstr2multi(LPCTSTR string)
{
	char *result = NULL;
	CML_TSTRTOMULTI(string, result);
	return result;
}

LPTSTR CML_multi2tstr(LPCSTR string)
{
	TCHAR *result = NULL;
	CML_TSTRFROMMULTI(string, result);
	return result;
}

LPTSTR CML_utf82tstr(LPCSTR string)
{
	TCHAR *result = NULL;
	CML_TSTRFROMUTF8(string, result);
	return result;
}

LPSTR CML_tstr2utf8(LPCTSTR string)
{
	char *result = NULL;
	CML_TSTRTOUTF8(string, result);
	return result;
}

/* following codes are used to initialize OpenLDAP */
void chomp(char *line)
{
	char *last = line+strlen(line);
	
	for (last; last >= line; last--) {
		if (strchr(" \r\n\t", *last))
			*last = '\0';
		else
			break;
	}
}

char *strcat_alloc(char *old_str, const char *str)
{
	int old_size;
	int new_size;
	char *text;
	int size = strlen(str);
	
	if (old_str) {
		old_size = strlen(old_str);
		new_size = old_size + size + 1;
		text = realloc(old_str, new_size);
		memcpy(text + old_size, str, size);
		text[new_size-1] = 0;
	} else {
		text = strdup(str);
	}
	return text;
}

TCHAR *GetModulePath(VOID)
{
	int done = FALSE;
	static TCHAR res[MAX_PATH], *temp;
	
	if (done) return res;
	if (GetModuleFileName(g_hInst, res, MAX_PATH)) {
		temp = _tcsrchr(res, _T('\\'));
		if (temp != NULL)
			*temp = _T('\0');
		done = TRUE;
	}
	return res;
}
