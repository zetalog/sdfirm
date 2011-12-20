#include <winsock.h>
#include "winsim.h"
#include "resource.h"
#include <host/usbip.h>

#define WM_SOCKET	WM_USER+1

VOID IHCIDlgCtrlUpdate(HWND hDlg)
{
}

LRESULT CALLBACK IHCIDlgProc(HWND hDlg, UINT message,
			     WPARAM wParam, LPARAM lParam)
{
	char szTemp[16];
	uint32_t addr;
	uint16_t port;

	switch (message) {
	case WM_INITDIALOG:
		itoa(USBIP_PORT, szTemp, 10);
		SetWindowText(GetDlgItem(hDlg, IDC_IHCI_ADDR), "127.0.0.1");
		SetWindowText(GetDlgItem(hDlg, IDC_IHCI_PORT), szTemp);
		SetTimer(hDlg, IDT_IHCI_CTRL, 1, (TIMERPROC)IHCIDlgProc);
		return TRUE;
	case WM_TIMER:
		switch (wParam) {
		case IDT_IHCI_CTRL:
			IHCIDlgCtrlUpdate(hDlg);
			break;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_IHCI_RESET:
			GetWindowText(GetDlgItem(hDlg, IDC_IHCI_ADDR),
				     (LPSTR)szTemp, sizeof(szTemp));
			addr = inet_addr(szTemp);
			GetWindowText(GetDlgItem(hDlg, IDC_IHCI_PORT), 
				     (LPSTR)szTemp, sizeof(szTemp));
			port = htons((uint16_t)(atoi(szTemp)));
			break;
		case IDC_IHCI_ENUM:
			break;
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	case WM_SOCKET:
		break;
	case WM_CLOSE:
		break;
	}
	return FALSE;
}

int InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	DialogBox(hInstance, (LPCTSTR)IDD_IHCI_HOST,
		  NULL, (DLGPROC)IHCIDlgProc);
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	DWORD dwRes;
	WSADATA wsaData;
	
	dwRes = WSAStartup(MAKEWORD(2,2), &wsaData);

	eloop_init();

	dwRes = InitInstance(hInstance, nCmdShow);
	WSACleanup();
	return dwRes;
}
