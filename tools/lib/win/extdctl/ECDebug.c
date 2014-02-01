#include "ECIntern.h"

/*
 *	DbgPrint
 *	
 *	Purpose:
 *		Debug only variable outputdebugstring
 *	
 *	Parameters:
 *		Standard wsprintf type stuff
 *
 *	Returns:
 *		nada
 */
#ifdef _DEBUG
VOID _cdecl DebugPrint(const CHAR *lpFmt, ...)
{
	va_list	arglist;
	CHAR	lpOutput[400] = "NW: ";

	va_start(arglist, lpFmt);
	wvsprintfA(&lpOutput[4], lpFmt, arglist);
	va_end(arglist);

	lstrcatA(lpOutput, "\r\n");
	OutputDebugStringA(lpOutput);
}

UINT AssertStuff(LPSTR szMsg, LPSTR szFile, INT nLine)
{
	CHAR	rgch[1024];
	CHAR	rgchUnk[] = "Unknown file\n";
	INT		nID;

	if (szFile)
		wsprintfA(rgch, "File %s, line %d\n", szFile, nLine);
	else
		lstrcpyA(rgch, rgchUnk);

	if (szMsg)
		lstrcatA(rgch, szMsg);

	MessageBeep(MB_ICONHAND);
	nID = MessageBoxA(NULL, rgch, "Assert Failure",
			MB_ABORTRETRYIGNORE | MB_DEFBUTTON3 | MB_ICONHAND | 
			MB_SETFOREGROUND | MB_TASKMODAL);

	if (nID == IDRETRY)
		DebugBreak();

	// if cancelling, force a hard exit w/ a GP-fault so that Dr. Watson
	// generates a nice stack trace log.
	if (nID == IDABORT)
		*(BYTE *)0 = 1;	// write to address 0 causes GP-fault

	return nID;
}
#else
VOID _cdecl DebugPrint(const CHAR *lpFmt, ...)
{
}
#endif
