/*
 * ZETALOG's Personal COPYRIGHT
 *
 * Copyright (c) 2003
 *    ZETALOG - "Lv ZHENG".  All rights reserved.
 *    Author: Lv "Zetalog" Zheng
 *    Internet: zetalog@gmail.com
 *
 * This COPYRIGHT used to protect Personal Intelligence Rights.
 * Redistribution and use in source and binary forms with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Lv "Zetalog" ZHENG.
 * 3. Neither the name of this software nor the names of its developers may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 4. Permission of redistribution and/or reuse of souce code partially only
 *    granted to the developer(s) in the companies ZETALOG worked.
 * 5. Any modification of this software should be published to ZETALOG unless
 *    the above copyright notice is no longer declaimed.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ZETALOG AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ZETALOG OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)SPLIntern.h: internal header of splitter window class
 * $Id: SPLIntern.h,v 1.1 2007/03/12 14:54:21 zhenglv Exp $
 */

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0400
#include <limits.h>
#include <windows.h>
#include <commctrl.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include <tchar.h>

#include "ECIntern.h"

typedef struct tagROWCOLINFO
{
	int nMinSize;       // below that try not to show
	int nIdealSize;     // user set size
	// variable depending on the available size layout
	int nCurSize;       // 0 => invisible, -1 => nonexistant
} ROWCOLINFO, FAR *LPROWCOLINFO;

typedef enum tagESPLITTYPE
{
	splitBox,
	splitBar,
	splitIntersection,
	splitBorder
} ESPLITTYPE;

typedef struct tagSPLWNDDATA
{
	HWND hWnd;
	HWND hWndParent;
	UINT nID;
	DWORD dwStyle;
	
	INT nMaxRows;
	INT nMaxCols;
	
	// implementation attributes which control layout of the splitter
	// size of splitter bar
	INT cxSplitter;
	INT cySplitter;
	
	// current state information
	INT nRows;
	INT nCols;
	LPROWCOLINFO lpColInfo;
	LPROWCOLINFO lpRowInfo;
	
	// Tracking info - only valid when 'bTracking' is set
	BOOL bTracking;
	POINT ptTrackOffset;
	RECT rcLimit;
	RECT rcTracker;
	INT htTrack;
	
	HCURSOR hcurArrow;
	
	// for dynamic creation
	LPTSTR lpDynamicViewClass;
	HINSTANCE hInstance;
	LPVOID lParam;
} SPLWNDDATA, FAR *LPSPLWNDDATA;

typedef enum tagHITTESTVALUE
{
	noHit                   = 0,
	vSplitterBox            = 1,
	hSplitterBox            = 2,
	vSplitterBar1           = 101,
	vSplitterBar15          = 115,
	hSplitterBar1           = 201,
	hSplitterBar15          = 215,
	splitterIntersection1   = 301,
	splitterIntersection225 = 525
} HITTESTVALUE;

#define Splitter_GetPtr(hWnd)   (LPSPLWNDDATA)GetWindowLong((hWnd), 0)
#define Splitter_SetPtr(hWnd, pTL)  \
			(LPSPLWNDDATA)SetWindowLong((hWnd), 0, (LONG)(pTL))

// SPLMsgProc.c
LRESULT WINAPI SplitterMsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// SPLCreate.c
BOOL SPLCreateStatic(LPSPLWNDDATA lpWD, LPSPLCREATESTRUCT lpcs);
BOOL SPLCreateCommon(LPSPLWNDDATA lpWD, LPCTSTR lpszClassName, HWND hwndParentWnd, HINSTANCE hInstance, UINT cx, UINT cy, DWORD dwStyle, UINT nID);
BOOL SPLCreateView(LPSPLWNDDATA lpWD, int row, int col, SIZE sizeInit,
                   LPCTSTR lpszClassName, LPCTSTR lpszWindowName, HINSTANCE hInstance,
                   DWORD dwStyle, DWORD dwExStyle, LPVOID lpParam);
BOOL SPLSwitchView(LPSPLWNDDATA lpWD, int row, int col, SIZE sizeInit,
                   LPCTSTR lpszClassName, LPCTSTR lpszWindowName, HINSTANCE hInstance,
                   DWORD dwStyle, DWORD dwExStyle, LPVOID lpParam);
int SPLIdFromRowCol(LPSPLWNDDATA lpWD, int row, int col);

// SPLDelete.c
void SPLDeleteView(LPSPLWNDDATA lpWD, int row, int col);

// SPLMisc.c
void* SPLAlloc(long size);
void* SPLReAlloc(void* p, long size);
void SPLFree(void* p);

// SPLAttrib.c
HWND SPLGetPane(LPSPLWNDDATA lpWD, int row, int col);
BOOL SPLIsChildPane(LPSPLWNDDATA lpWD, HWND hwndView, LPSPLPANEINFO lppi);
void SPLGetRowInfo(LPSPLWNDDATA lpWD, int row, int *cyCur, int *cyMin);
void SPLSetRowInfo(LPSPLWNDDATA lpWD, int row, int cyIdeal, int cyMin);
void SPLGetColumnInfo(LPSPLWNDDATA lpWD, int col, int *cxCur, int *cxMin);
void SPLSetColumnInfo(LPSPLWNDDATA lpWD, int col, int cxIdeal, int cxMin);
HWND SPLGetActivePane(LPSPLWNDDATA lpWD, LPSPLPANEINFO lppi);
void SPLSetActivePane(LPSPLWNDDATA lpWD, int row, int col, HWND hWnd);
void SPLActivateNext(LPSPLWNDDATA lpWD, BOOL bPrev);

// SPLTrack.c
int SPLHitTest(LPSPLWNDDATA lpWD, POINT pt);
void SPLGetHitRect(LPSPLWNDDATA lpWD, int ht, RECT *lpRectHit);
void SPLStartTracking(LPSPLWNDDATA lpWD, int ht);
void SPLStopTracking(LPSPLWNDDATA lpWD, BOOL bAccept);
void SPLInvertTracker(LPSPLWNDDATA lpWD, RECT *lpRect);
void SPLMouseMove(LPSPLWNDDATA lpWD, UINT nFlags, POINT pt);
void SPLTrackRowSize(LPSPLWNDDATA lpWD, int y, int row);
void SPLTrackColumnSize(LPSPLWNDDATA lpWD, int x, int col);

// SPLDraw.c
void SPLDrawAllSplitBars(LPSPLWNDDATA lpWD, HDC hDC, int cxInside, int cyInside);
void SPLDrawSplitter(LPSPLWNDDATA lpWD, HDC hDC, ESPLITTYPE nType, const RECT *lpRectArg);
void SPLPaint(LPSPLWNDDATA lpWD);
void SPLRecalcLayout(LPSPLWNDDATA lpWD);
