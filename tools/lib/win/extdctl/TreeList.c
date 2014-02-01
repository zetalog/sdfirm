//:TreeList.c - handle "TreeList" window class
//
//	$Id: TreeList.c,v 1.1 2007/03/12 14:54:21 zhenglv Exp $
//

#include "TLintern.h"

BOOL WINAPI InitTreeListControl (DWORD dwSize)
{
	WNDCLASS  wc;
	ATOM atom;
	
	InitCommonControls ();

	// register new window classes for menus
	wc.style = CS_DBLCLKS | CS_GLOBALCLASS | CS_PARENTDC; // Class style(s).
	wc.lpfnWndProc = (WNDPROC) TreeListMsgProc; 
	wc.cbClsExtra = 0;	                        // No per-class extra data.
	wc.cbWndExtra = sizeof (TLWndData*);		// pointer to extra data structure
	wc.hInstance = LIBINSTANCE;	
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor (NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) (COLOR_WINDOW+1); // Background color
	wc.lpszMenuName = NULL;						// No menu
	wc.lpszClassName = "TreeListCtrl32";		// Name used in CreateWindow
	atom = RegisterClass (&wc);
	return atom != 0;
}
