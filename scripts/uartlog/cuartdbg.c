#include <host/uartdbg.h>

HANDLE g_hComFile = NULL;
DWORD g_dwLastError = 0;

static void DumpError(void)
{
	DWORD dwCurrError = GetLastError();

	if (dwCurrError != g_dwLastError) {
		g_dwLastError = dwCurrError;
		printf("<raw> ERROR: ");
		printf("%d", dwCurrError);
		printf("\r\n");
	}
}

void cleanup(void)
{
	dbg_exit();
	if (g_hComFile)
		CloseHandle(g_hComFile);

}

static void ConsoleLast(void *ctx, const unsigned char *szNew, size_t dwToStore)
{
	DWORD nIndex;
	FILE *pFile = (FILE *)ctx;

	for (nIndex = 0; nIndex < dwToStore; nIndex++) {
		fprintf(pFile, "%02x ", szNew[nIndex]);
	}
}

static void ConsoleDump(void *ctx, dbg_cmd_t bCmd, const char *szFormat, ...)
{
	FILE *pFile = (FILE *)ctx;
	va_list argList;
	unsigned char szBuf[1024];
	
	va_start(argList, szFormat );
	vsprintf(szBuf, szFormat, argList);
	va_end(argList);

	fprintf(pFile, "<%s> %s: ",
		dbg_state_name(bCmd), dbg_event_name(bCmd));
	fprintf(pFile, "%s", szBuf);
	fprintf(pFile, "\r\n");
}

int main(int argc, char **argv)
{
	DCB dcb;

	atexit(cleanup);
	
	dbg_init();
	dbg_register_output(ConsoleDump, ConsoleLast);

opencom:
	while (1) {
		g_hComFile = CreateFile(argv[1], GENERIC_READ, 0,
					0, OPEN_EXISTING, 0, 0);
		if (g_hComFile == INVALID_HANDLE_VALUE) {
			DumpError();
			Sleep(1000 * 1);
		} else {
			ZeroMemory(&dcb, sizeof(DCB));
			dcb.DCBlength = sizeof(DCB);
			if (!GetCommState(g_hComFile, &dcb)) {
				DumpError();
				Sleep(1000 * 1);
				CloseHandle(g_hComFile);
				g_hComFile = NULL;
				goto opencom;
			} else {
				dcb.BaudRate = CBR_38400;
				dcb.ByteSize = 8;
				dcb.Parity = 0;
				dcb.StopBits = 0;
				if (!SetCommState(g_hComFile, &dcb)) {
					DumpError();
					Sleep(1000 * 1);
					CloseHandle(g_hComFile);
					g_hComFile = NULL;
					goto opencom;
				} else {
					goto readcom;
				}
			}
		}
	}

readcom:
	while (1) {
		BYTE bData;
		DWORD dwRes;
		if (ReadFile(g_hComFile, &bData, 1, &dwRes, NULL) <= 0) {
			DumpError();
			Sleep(1000);
			CloseHandle(g_hComFile);
			g_hComFile = NULL;
			goto opencom;
		}
		dbg_process_log(stdout, bData);
	}

	return 0;
}
