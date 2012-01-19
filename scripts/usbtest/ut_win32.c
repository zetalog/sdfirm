#include "ut_win32.h"

/* http://msdn.microsoft.com/en-us/library/aa379790(VS.85).aspx */
/* http://msdn.microsoft.com/en-us/library/ms801364.aspx */
/* http://www.diybl.com/course/3_program/c++/cppsl/200866/122899.html */
/* http://www.di.ubi.pt/~desousa/ocamlpcsc/doc/windows/pcscML.html */
/* http://www.osronline.com/showthread.cfm?link=122754        baud */
/* http://www.microsoft.com/whdc/device/input/smartcard/USB_CCID.mspx */

SCARDCONTEXT g_hSC = 0;
SCARDHANDLE  g_hCardHandle = 0;

static int win_init(void);
static void win_exit(void);
static void win_err(const char *str);

/* call PCSC API */
static t_list_rdrs(void);
static int t_list_rdr_groups(void);
static int t_card_conn(LPCTSTR rdr);

/* may call CCID command */
static void t_get_atr(SCARDHANDLE hCard);
static LONG t_ioctl_Power(IN SCARDHANDLE hCard);
static LONG t_ioctl_GetState(IN SCARDHANDLE hCard);
static LONG t_ioctl_Present(IN SCARDHANDLE hCard);

static LONG t_transmit(IN SCARDHANDLE hCard,
		       IN LPCSCARD_IO_REQUEST pioSendPci,
		       IN LPCBYTE pbSendBuffer,
		       IN DWORD cbSendLength,
		       IN OUT LPSCARD_IO_REQUEST pioRecvPci,
		       OUT LPBYTE pbRecvBuffer,
		       IN OUT LPDWORD pcbRecvLength);

static LONG t_SelectFile(IN SCARDHANDLE hCard,
                         IN DWORD dwClassCode,
                         IN WORD lpbFileName,
                         OUT LPDWORD lpdwExtraBytes);

static LONG t_SCGetResponse(IN SCARDHANDLE hCard,
			    IN DWORD dwLength,
			    OUT LPBYTE * lplpbResponse);

static const PBYTE t_card_state(DWORD state);

#define SC_RDR_GROUP		"SCard$AllReaders"
#define SC_RDR_DEF_GROUP	"SCard$DefaultReaders"

static int test_main(void)
{
	DWORD   newle = 0;
	LPBYTE	resp;

	t_SelectFile(g_hCardHandle, 0x00, 0x3F00, &newle);
	if (newle != 0) {
		t_SCGetResponse(g_hCardHandle, newle, &resp);
		if (resp)
			free(resp);
	} else {
		printf("SelectFile failure\r\n");
		return 0;
	}
#if 0
	t_ioctl_Present(g_hCardHandle);
	t_ioctl_GetState(g_hCardHandle);
	t_ioctl_Power(g_hCardHandle);
	t_ioctl_GetState(g_hCardHandle);
#endif
	return 0;
}

int main(void)
{
	atexit(win_exit);

	if (win_init())
		return 1;

	test_main();
	return 0;
}

static int win_init(void)
{
	LONG	lReturn;

	// Establish the context.
	lReturn = SCardEstablishContext(SCARD_SCOPE_USER,
					NULL, NULL, &g_hSC);

	if ( SCARD_S_SUCCESS != lReturn ) {
		win_err("Failed SCardEstablishContext");
		return -1;
	} else {
		return t_list_rdrs();
	}	
}

static void win_exit(void)
{
	if (g_hCardHandle)
		SCardDisconnect(g_hCardHandle, SCARD_LEAVE_CARD);
	if (g_hSC)
		SCardReleaseContext(g_hSC);
}


static int t_list_rdrs(void)
{
	LPCTSTR		pGroup = NULL;
	LPTSTR          pmszReaders = NULL;
	LPTSTR          pReader;
	LONG            lReturn, lReturn2;
	DWORD           cch = SCARD_AUTOALLOCATE;
	SCARDCONTEXT	hSC = g_hSC;
	INT		ret = -1;

	printf("List smartcard readers in system:\n");
	// Retrieve the list the readers.
	// hSC was set by a previous call to SCardEstablishContext.
	lReturn = SCardListReaders(hSC, pGroup, (LPTSTR)&pmszReaders, &cch);

	if (lReturn != SCARD_S_SUCCESS) {
		win_err("SCardListReaders Fail");
		return -1;
	}

	switch( lReturn )
	{
	case SCARD_E_NO_READERS_AVAILABLE:
		win_err("Reader is not in groups");
		// Take appropriate action.
		// ...
		break;
		
	case SCARD_S_SUCCESS:
		// Do something with the multi string of readers.
		// Output the values.
		// A double-null terminates the list of values.

		pReader = pmszReaders;
		while ( '\0' != *pReader )
		{
			// Display the value.
			printf("Reader: %s\n", pReader );
			ret = t_card_conn(pReader);
			if (ret == 0)
				break;
			// Advance to the next value.
			pReader = pReader + strlen((BYTE *)pReader) + 1;
		}

		// Free the memory.
		lReturn2 = SCardFreeMemory(hSC, pmszReaders);
		if (SCARD_S_SUCCESS != lReturn2)
			win_err("Failed SCardFreeMemory");
		break;
	default:
		win_err("Failed SCardListReaders");
		break;
	}
	return ret;
}

static int t_card_conn(LPCTSTR rdr)
{
	LONG            lReturn;
	DWORD           dwAP;
	SCARDCONTEXT	hContext = g_hSC;

	printf("Connect card and show attr...\r\n");
	lReturn = SCardConnect( hContext, rdr, SCARD_SHARE_SHARED, 
				SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
				&g_hCardHandle, &dwAP );
	if ( SCARD_S_SUCCESS != lReturn )
	{
		win_err("Failed SCardConnect");
		return -1;
	}
	
	// Use the connection.
	// Display the active protocol.
	switch ( dwAP )
	{
	case SCARD_PROTOCOL_T0:
		printf("Active protocol T0\n"); 
		break;
		
	case SCARD_PROTOCOL_T1:
		printf("Active protocol T1\n"); 
		break;
		
	case SCARD_PROTOCOL_UNDEFINED:
	default:
		printf("Active protocol unnegotiated or unknown\n"); 
		break;
	}
	
	// Remember to disconnect (by calling SCardDisconnect).
	// ...
	t_get_atr(g_hCardHandle);

	return 0;
}

static void t_get_atr(SCARDHANDLE hCard)
{
	DWORD dwReaderLen = 0;
	DWORD dwState;
	DWORD dwProtocol;
	BYTE  atr[MAXIMUM_ATTR_STRING_LENGTH];
	DWORD dwAtrLen;
	DWORD lResult, dwIndex = 0;
	DWORD i;
	//
	// Get status of card
	//
	lResult = SCardStatus(hCard,
		NULL,
		&dwReaderLen,
		&dwState,
		&dwProtocol,
		atr,
		&dwAtrLen);

	if (lResult != SCARD_S_SUCCESS)
	{
		win_err("Failed SCardStatus");
		return;
	}
	
	//
	// Print out ATR bytes of card
	//
	printf(("\tATR = "), dwIndex + 1);
	for (i = 0; i < dwAtrLen; i++) {
		printf(("%02x "), (DWORD) atr[i]);
	}
	printf("\n\tSTATE = %s\n", t_card_state(dwState));
}

static const PBYTE t_card_state(DWORD state)
{
	switch (state) {
	case SCARD_UNKNOWN: return "unknown";
	case SCARD_ABSENT: return "absent";
	case SCARD_PRESENT: return "present";
	case SCARD_SWALLOWED: return "swallowed";
	case SCARD_POWERED: return "pwred";
	case SCARD_NEGOTIABLE: return "nego";
	case SCARD_SPECIFIC: return "specific";
	default: return "abnormal";
	}
}

static LONG t_SelectFile (IN SCARDHANDLE hCard,
                          IN DWORD dwClassCode,
                          IN WORD lpbFileName,
                          OUT LPDWORD lpdwExtraBytes)
{
	LONG lResult;
	BYTE  status[2];
	DWORD dwStatusLength = sizeof(status);
	BYTE apdu[7] = {(BYTE) dwClassCode, 0xa4, 0x00, 0x00, 0x02,
		(lpbFileName & 0xFF00) >> 8, (lpbFileName & 0x00FF)};
	
	printf("SelectFile Test:\r\n");
	//
	// Initialize returned info.
	//
	*lpdwExtraBytes = 0;
	
	
	//
	// Send APDU to card
	//
	lResult = t_transmit(hCard,
		SCARD_PCI_T0,
		apdu,
		sizeof(apdu),
		NULL,
		status,
		&dwStatusLength);
	
	// If API successful but card operation failed, then
	// return SW1 and SW2 as error code
	//
	if (lResult == SCARD_S_SUCCESS)
	{
		if (!(status[0] == 0x90 && status[1] == 0x00))
		{
			if (status[0] == 0x61)
			{
				//
				// Successful, but there might be extra data from the card
				//
				*lpdwExtraBytes = (DWORD) status[1];
			}
			else
			{
				//
				// Card error, so return SW1 and SW2 as error
				//
				lResult = MAKELONG(MAKEWORD(status[1], status[0]), 0x0000);
			}
		}
	}
	
	return lResult;
}

static LONG t_SCGetResponse (IN SCARDHANDLE hCard,
			     IN DWORD dwLength,
			     OUT LPBYTE * lplpbResponse)
{
	LONG lResult;
	LPBYTE lpbResponse;
	DWORD dwStatusLen;
	BYTE apdu[5] = {0x00, 0xc0, 0x00, 0x00, (BYTE) dwLength};
	
	assert(lplpbResponse != NULL);
	
	printf("GetResponse:\r\n");
	//
	// APDU response data length cannot be larger than 256
	//
	if (dwLength > 256)
	{
		return SCARD_E_INVALID_PARAMETER;
	}
	
	//
	// Initialize returned info.
	//
	*lplpbResponse = NULL;
	
	//
	// Allocate memory.
	//
	lpbResponse = (LPBYTE) malloc(dwLength + 2);
	
	if (lpbResponse == NULL)
	{
		return ERROR_OUTOFMEMORY;
	}
	
	//
	// Construct the Get Response APDU.
	// Note that when dwLength is casted to BYTE, 256 would be converted to 0,
	// which will be the correct value to indicate 256 bytes.
	//
	dwStatusLen = dwLength + 2;
	
	//
	// Send APDU to card.
	//
	lResult = t_transmit(hCard,
		SCARD_PCI_T0,
		apdu,
		sizeof(apdu),
		NULL,
		lpbResponse,
		&dwStatusLen);
	if (lResult == SCARD_S_SUCCESS)
	{
		//
		// Did we get all the data?
		//
		if (dwStatusLen == dwLength + 2)
		{
			//
			// Is the data good?
			//
			if (lpbResponse[dwLength] == 0x90 && lpbResponse[dwLength + 1] == 0x00)
			{
				//
				// Successful, so return pointer to response data.
				//
				*lplpbResponse = (LPBYTE) lpbResponse;
				printf("SelectFile success\r\n");

			}
			else
			{
				//
				// Error occurred, so free memory
				//
				free(lpbResponse);
				printf("SelectFile failure\r\n");
				//
				// and return SW1 and SW2 as return code
				//
				lResult = MAKELONG(MAKEWORD(lpbResponse[dwLength + 1],
					lpbResponse[dwLength]), 0x000);
			}
		}
		else
		{
			//
			// Error occurred, so free memory
			//
			free(lpbResponse);
			
			//
			// and return SW1 and SW2 as return code
			//
			lResult = MAKELONG(MAKEWORD(lpbResponse[1], lpbResponse[0]), 0x000);
		}
	}
	else
	{
		//
		// Error occurred, so free memory before returning.
		//
		free(lpbResponse);
	}
	
	return lResult;
}

static LONG t_transmit(IN SCARDHANDLE hCard,
		       IN LPCSCARD_IO_REQUEST pioSendPci,
		       IN LPCBYTE pbSendBuffer,
		       IN DWORD cbSendLength,
		       IN OUT LPSCARD_IO_REQUEST pioRecvPci,
		       OUT LPBYTE pbRecvBuffer,
		       IN OUT LPDWORD pcbRecvLength)
{
	LONG rv;

	rv = SCardTransmit(hCard, pioSendPci, pbSendBuffer, cbSendLength,
		pioRecvPci, pbRecvBuffer, pcbRecvLength);

	if ( SCARD_S_SUCCESS != rv ) {
		win_err("Failed SCardTransmit");
		return rv;   // or other appropriate error action
	}
	return rv;
}

static LONG t_ioctl_Present(IN SCARDHANDLE hCard)
{
	
	INT	ret = 0;
	LONG	lReturn;
	
	lReturn = SCardControl(hCard, IOCTL_SMARTCARD_IS_PRESENT,
			       NULL, 0, NULL, 0, &ret);
	if (STATUS_PENDING == lReturn) {
		printf("lReturn=%d\r\n", lReturn);
	}
	return lReturn;
}

static LONG t_ioctl_Power(IN SCARDHANDLE hCard)
{
	DWORD dwControlCode = IOCTL_SMARTCARD_POWER;
	LONG	lReturn;
	BYTE	sbuf[10] = {0};
	BYTE	rbuf[33] = {0};
	INT	ret = 0;

	memset(sbuf, 0, 10);
#if 0
	sbuf[0] = 0x62;		/* PowerOn */
	sbuf[1] = 0x00;
	sbuf[2] = 0x00;
	sbuf[3] = 0x00;
	sbuf[4] = 0x00;
	sbuf[5] = 0;
	sbuf[6] = 0;
	sbuf[7] = 0x00;
	sbuf[8] = 0x00;
	sbuf[9] = 0x00;	
#endif
	sbuf[0] = SCARD_POWER_DOWN;

	lReturn = SCardControl(hCard, dwControlCode,
			       sbuf, 1, rbuf, 33, &ret);

	if (SCARD_S_SUCCESS == lReturn && ret > 0) {
		int i;
		char buff[33 * 3];

		memset(buff, 0x00, sizeof (buff));

		for (i = 0; i < ret; ) {
			sprintf(buff + 3 * i, "%02X ", rbuf[i]);
			i++;
		}
		printf("ret=%s\r\n", buff);
	} else {
		printf("Failed SCardControl\n");
	}
	return lReturn;
}

static LONG t_ioctl_GetState(IN SCARDHANDLE hCard)
{
	LONG	lReturn;
	LONG	rbuf = 0;
	INT	ret = 0;

	lReturn = SCardControl(hCard, IOCTL_SMARTCARD_GET_STATE,
			       NULL, 0, (LPVOID)rbuf, sizeof (LONG), &ret);

	if (SCARD_S_SUCCESS != lReturn)
		printf("Failed SCardControl\n");
	else
		printf("STATE=%s\r\n", t_card_state(rbuf));

	return lReturn;
}

static void win_err(const char *str)
{
	char szError[256];
	DWORD dwError = GetLastError();
	
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
		      NULL, 0, 0, szError,
		      sizeof(szError), NULL);
	if (str)
		printf("%s, ", str);
	printf("<err>=%s\r\n", szError);
}