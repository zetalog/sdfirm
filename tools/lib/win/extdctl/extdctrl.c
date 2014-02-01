#include <windows.h>
#include <assert.h>
#include <host/extdctrl.h>

static int process_attched = 0;
HINSTANCE LIBINSTANCE = NULL;
COLORREF clrBtnFace;
COLORREF clrBtnShadow;
COLORREF clrBtnHilite;
COLORREF clrBtnText;
COLORREF clrWindowFrame;
// convenient system color
HBRUSH hbrWindowFrame;
HBRUSH hbrBtnFace;

BOOL APIENTRY DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            LIBINSTANCE = instance;
            if (!process_attched) {
	            clrBtnFace = GetSysColor(COLOR_BTNFACE);
	            clrBtnShadow = GetSysColor(COLOR_BTNSHADOW);
	            clrBtnHilite = GetSysColor(COLOR_BTNHIGHLIGHT);
	            clrBtnText = GetSysColor(COLOR_BTNTEXT);
	            clrWindowFrame = GetSysColor(COLOR_WINDOWFRAME);
				
				hbrBtnFace = GetSysColorBrush(COLOR_BTNFACE);
				assert(hbrBtnFace != NULL);
				hbrWindowFrame = GetSysColorBrush(COLOR_WINDOWFRAME);
				assert(hbrWindowFrame != NULL);
            }
            process_attched++;
            break;
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            process_attched--;
            if (!process_attched) {
            }
            break;
    }
    return TRUE;
}

void WINAPI InitExtendedControls(void)
{
	BOOL bInit;
	
	bInit = InitSplitterControl(0);
	assert(bInit);
	bInit = InitTreeListControl(0);
	assert(bInit);
}

BOOL WINAPI InitExtendedControlsEx(LPINITEXTENDEDCONTROLSEX lpInitCtrls)
{
    BOOL bSuccess;

    if (lpInitCtrls->dwICC & ICC_SPLITTER_CLASS)
        bSuccess = InitSplitterControl(lpInitCtrls->dwSize);

    return bSuccess;
}
