
#ifndef __INC_EXTDCTRL_H
#define __INC_EXTDCTRL_H

#ifndef NOUSER

//
// Define API decoration for direct importing of DLL references.
//
#ifndef WINCOMMCTRLAPI
#if !defined(_COMCTL32_) && defined(_WIN32)
#define WINCOMMCTRLAPI DECLSPEC_IMPORT
#else
#define WINCOMMCTRLAPI
#endif
#endif // WINCOMMCTRLAPI

//
// For compilers that don't support nameless unions
//
#ifndef DUMMYUNIONNAME
#ifdef NONAMELESSUNION
#define DUMMYUNIONNAME   u
#define DUMMYUNIONNAME2  u2
#define DUMMYUNIONNAME3  u3
#else
#define DUMMYUNIONNAME
#define DUMMYUNIONNAME2
#define DUMMYUNIONNAME3
#endif
#endif // DUMMYUNIONNAME

#ifdef _WIN32
#include <pshpack1.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//===========================================================================
//
// Users of this header may define any number of these constants to avoid
// the definitions of each functional group.
//
// NOSPLITTER   Custom control which is to split panes
//
//===========================================================================

#include <prsht.h>
#include <limits.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#ifdef __cplusplus
#define SNDMSG ::SendMessage
#else
#define SNDMSG SendMessage
#endif


BOOL WINAPI InitSplitterControl(DWORD dwSize);
BOOL WINAPI InitTreeListControl(DWORD dwSize);

//====== Ranges for control message IDs =====================================

#define TLM_FIRST               0x1000      // TreeList messages


//====== WM_NOTIFY codes (NMHDR.code values) ================================

#define TLN_FIRST               (0U-100U)       
#define TLN_LAST                (0U-199U)


//====== TREELIST CONTROL ===================================================

#ifndef NOTREELIST

#ifdef _WIN32
#define WC_TREELISTA            "TreeListCtrl32"
#define WC_TREELISTW            L"TreeListCtrl32"

#ifdef UNICODE
#define  WC_TREELIST            WC_TREELISTW
#else
#define  WC_TREELIST            WC_TREELISTA
#endif

#else
#define WC_TREELIST             "TreeListCtrl"
#endif

#define TLS_HASBUTTONS          0x0001
#define TLS_HASLINES            0x0002
#define TLS_LINESATROOT         0x0004
#define TLS_AUTOSCROLL			0x0008
#define TLS_PROMISCUOUS			0x0010
#define TLS_SINGLESELECT		0x0020
#define TLS_INTERNALDRAG		0x0040
#define TLS_DRAGABLEHEADERS		0x0080
#define TLS_SHOWSELECTIONALWAYS	0x0100

typedef struct _TLITEM FAR* HTLITEM;

#define TLIF_TEXT               0x0001
#define TLIF_IMAGE              0x0002
#define TLIF_PARAM              0x0004
#define TLIF_STATE              0x0008
#define TLIF_HANDLE             0x0010
#define TLIF_DATAVALUE			0x0020
#define TLIF_NEXTHANDLE			0x0040
#define TLIF_PARENTHANDLE		0x0080
#define TLIF_PREVHANDLE			0x0100
#define TLIF_CHILDHANDLE		0x0200

#define TLIS_FOCUSED            0x0001
#define TLIS_SELECTED           0x0002
#define TLIS_EXPANDED           0x0004
#define TLIS_BOLD               0x0008
#define TLIS_ITALICS			0x0010
#define TLIS_CLICKED			0x0020
#define TLIS_VISIBLE			0x0080
#define TLIS_DRAGGEDOVER		0x0100
#define TLIS_TRUNCATED			0x0200

typedef struct _TL_TREEITEMA {
    UINT      mask;
    HTLITEM   hItem;
    UINT      state;
    UINT      stateMask;
    LPSTR     pszText;
    int       cchTextMax;
    int       iImage;
    int       iSelectedImage;
    int       cChildren;
    LPARAM    lParam;
} TL_TREEITEMA, FAR *LPTL_TREEITEMA;

typedef struct _TL_TREEITEMW {
    UINT      mask;
    HTLITEM   hItem;
    UINT      state;
    UINT      stateMask;
    LPWSTR    pszText;
    int       cchTextMax;
    int       iImage;
    int       iSelectedImage;
    int       cChildren;
    LPARAM    lParam;
} TL_TREEITEMW, FAR *LPTL_TREEITEMW;

#ifdef UNICODE
#define  TL_TREEITEM                TL_TREEITEMW
#define  LPTL_TREEITEM              LPTL_TREEITEMW
#else
#define  TL_TREEITEM                TL_TREEITEMA
#define  LPTL_TREEITEM              LPTL_TREEITEMA
#endif


#define TLI_ROOT                ((HTLITEM)0xFFFF0000)
#define TLI_FIRST               ((HTLITEM)0xFFFF0001)
#define TLI_LAST                ((HTLITEM)0xFFFF0002)
#define TLI_SORT                ((HTLITEM)0xFFFF0003)
#define TLI_SORTREVERSE         ((HTLITEM)0xFFFF0004)

typedef struct _TL_INSERTSTRUCTA {
    HTLITEM hParent;
    HTLITEM hInsertAfter;
    TL_TREEITEMA item;
} TL_INSERTSTRUCTA, FAR *LPTL_INSERTSTRUCTA;

typedef struct _TL_INSERTSTRUCTW {
    HTLITEM hParent;
    HTLITEM hInsertAfter;
    TL_TREEITEMW item;
} TL_INSERTSTRUCTW, FAR *LPTL_INSERTSTRUCTW;

#ifdef UNICODE
#define  TL_INSERTSTRUCT        TL_INSERTSTRUCTW
#define  LPTL_INSERTSTRUCT      LPTL_INSERTSTRUCTW
#else
#define  TL_INSERTSTRUCT        TL_INSERTSTRUCTA
#define  LPTL_INSERTSTRUCT      LPTL_INSERTSTRUCTA
#endif

#define TLM_INSERTITEMA         (TLM_FIRST + 0)
#define TLM_INSERTITEMW         (TLM_FIRST + 50)
#ifdef UNICODE
#define  TLM_INSERTITEM         TLM_INSERTITEMW
#else
#define  TLM_INSERTITEM         TLM_INSERTITEMA
#endif

#define TreeList_InsertItem(hwnd, lpis) \
    (HTLITEM)SNDMSG((hwnd), TLM_INSERTITEM, 0, \
	(LPARAM)(LPTL_INSERTSTRUCT)(lpis))

#define TLM_REINSERTITEMA       (TLM_FIRST + 4)
#define TLM_REINSERTITEMW       (TLM_FIRST + 54)
#ifdef UNICODE
#define  TLM_REINSERTITEM       TLM_REINSERTITEMW
#else
#define  TLM_REINSERTITEM       TLM_REINSERTITEMA
#endif

#define TreeList_ReInsertItem(hwnd, lpis) \
    (HTLITEM)SNDMSG((hwnd), TLM_REINSERTITEM, 0, \
	(LPARAM)(LPTL_INSERTSTRUCT)(lpis))


#define TLM_DELETETREE          (TLM_FIRST + 3)
#define TreeList_DeleteTree(hwnd, bFree) \
    (BOOL)SNDMSG((hwnd), TLM_DELETETREE, bFree, 0)


#define TLM_DELETEITEM          (TLM_FIRST + 1)
#define TreeList_DeleteItem(hwnd, pitem) \
    (BOOL)SNDMSG((hwnd), TLM_DELETEITEM, 0, (LPARAM)(TL_TREEITEM FAR*)(pitem))


#define TLM_DELETECHILDREN      (TLM_FIRST + 10)
#define TreeList_DeleteChildren(hwnd, pitem) \
    (BOOL)SNDMSG((hwnd), TLM_DELETECHILDREN, 0, \
	(LPARAM)(TL_TREEITEM FAR*)(pitem))


#define TLM_EXPAND              (TLM_FIRST + 2)
#define TreeList_Expand(hwnd, hitem, code) \
    (BOOL)SNDMSG((hwnd), TLM_EXPAND, (WPARAM)code, \
	(LPARAM)(HTLITEM)(hitem))


#define TLE_COLLAPSE            0x0001
#define TLE_EXPAND              0x0002
#define TLE_TOGGLE              0x0003
#define TLE_COLLAPSEALL			0x0004
#define TLE_EXPANDALL			0x0005

#define TLM_SETIMAGELIST        (TLM_FIRST + 9)
#define TreeList_SetImageList(hwnd, himl) \
    (HIMAGELIST)SNDMSG((hwnd), TLM_SETIMAGELIST, 0, \
	(LPARAM)(UINT)(HIMAGELIST)(himl))

#define TLM_SELECTITEM          (TLM_FIRST + 11)
#define TreeList_Select(hwnd, hitem, bdeselect) \
    (HTLITEM)SNDMSG((hwnd), TLM_SELECTITEM, (WPARAM)bdeselect, \
	(LPARAM)(HTLITEM)(hitem))

#define TLM_DRAGOVER			(TLM_FIRST + 14)
#define TreeList_DragOver(hwnd, coords) \
	(BOOL)SNDMSG((hwnd), TLM_DRAGOVER, 0, (LPARAM)(coords))

#define TLRB_TOPLEVELONLY		0x0001

typedef struct _TL_DRAGBITMAP {
	HBITMAP		hbmDrag;
	SIZE		sizeDrag;
	POINT		ptCursorPos;
	POINT		ptHotSpot;
	ULONG		ulFlags;
} TL_DRAGBITMAP, FAR *LPTL_DRAGBITMAP;

#define TLM_RENDERDRAGBITMAP	(TLM_FIRST + 15)
#define TreeList_RenderDragBitmap(hwnd, lpdb) \
	(BOOL)SNDMSG((hwnd), TLM_RENDERDRAGBITMAP, 0, \
	(LPARAM)(LPTL_DRAGBITMAP)(lpdb))

#define TLM_DRAGOVERPAINT		(TLM_FIRST + 16)
#define TreeList_DragOverPaint(hwnd) \
	(BOOL)SNDMSG((hwnd), TLM_DRAGOVERPAINT, 0, 0)

#define TLM_SELECTCHILDREN      (TLM_FIRST + 20)
#define TreeList_SelectChildren(hwnd, hitem) \
    (HTLITEM)SNDMSG((hwnd), TLM_SELECTCHILDREN, 0, \
	(LPARAM)(HTLITEM)(hitem))

#define TLM_ITERATESELECTED     (TLM_FIRST + 21)
#define TreeList_IterateSelected(hwnd, userstruct) \
    (HTLITEM)SNDMSG((hwnd), TLM_ITERATESELECTED, 0, (LPARAM)(userstruct))


#define TLM_GETITEMA            (TLM_FIRST + 12)
#define TLM_GETITEMW            (TLM_FIRST + 62)

#ifdef UNICODE
#define  TLM_GETITEM            TLM_GETITEMW
#else
#define  TLM_GETITEM            TLM_GETITEMA
#endif

#define TreeList_GetItem(hwnd, pitem) \
    (BOOL)SNDMSG((hwnd), TLM_GETITEM, 0, (LPARAM)(TL_TREEITEM FAR*)(pitem))


#define TLM_SETITEMA            (TLM_FIRST + 13)
#define TLM_SETITEMW            (TLM_FIRST + 63)

#ifdef UNICODE
#define  TLM_SETITEM            TLM_SETITEMW
#else
#define  TLM_SETITEM            TLM_SETITEMA
#endif
#define TreeList_SetItem(hwnd, pitem) \
    (BOOL)SNDMSG((hwnd), TLM_SETITEM, 0, \
	(LPARAM)(const TL_TREEITEM FAR*)(pitem))


#define TLM_GETFIRSTITEM             (TLM_FIRST + 18)
#define TreeList_GetFirstItem(hwnd) \
    (HTLITEM)SNDMSG((hwnd), TLM_GETFIRSTITEM, 0, 0)

typedef struct _TL_COLUMNA
{
    UINT mask;
    int fmt;
    int cx;
    LPSTR pszText;
    int cchTextMax;
    int iSubItem;
	UINT iDataType;
	BOOL bMouseNotify;
} TL_COLUMNA;

typedef struct _TL_COLUMNW
{
    UINT mask;
    int fmt;
    int cx;
    LPWSTR pszText;
    int cchTextMax;
    int iSubItem;
	UINT iDataType;
	BOOL bMouseNotify;
} TL_COLUMNW;

#ifdef UNICODE
#define  TL_COLUMN              TL_COLUMNW
#else
#define  TL_COLUMN              TL_COLUMNA
#endif

#define TLC_DATANONE			0x0000
#define TLC_DATASTRING			0x0001
#define TLC_DATALONG			0x0002

#define TLCF_FMT                0x0001
#define TLCF_WIDTH              0x0002
#define TLCF_TEXT               0x0004
#define TLCF_SUBITEM            0x0008
#define TLCF_DATATYPE			0x0010
#define TLCF_DATAMAX			0x0020
#define TLCF_DATAVALUE			0x0040
#define TLCF_MOUSENOTIFY		0x0080

#define TLCFMT_LEFT             0x0000
#define TLCFMT_RIGHT            0x0001
#define TLCFMT_CENTER           0x0002
#define TLCFMT_LINBAR			0x0004
#define TLCFMT_LOGBAR			0x0005
#define TLCFMT_IMAGE			0x0006
#define TLCFMT_IMAGELIST		0x0007


#define TLM_SETCOLUMNA          (TLM_FIRST + 26)
#define TLM_SETCOLUMNW          (TLM_FIRST + 96)
#ifdef UNICODE
#define  TLM_SETCOLUMN          TLM_SETCOLUMNW
#else
#define  TLM_SETCOLUMN          TLM_SETCOLUMNA
#endif

#define TreeList_SetColumn(hwnd, iCol, pcol) \
    (BOOL)SNDMSG((hwnd), TLM_SETCOLUMN, (WPARAM)(int)(iCol), \
	(LPARAM)(const TL_COLUMN FAR*)(pcol))


#define TLM_INSERTCOLUMNA       (TLM_FIRST + 27)
#define TLM_INSERTCOLUMNW       (TLM_FIRST + 97)
#ifdef UNICODE
#   define  TLM_INSERTCOLUMN    TLM_INSERTCOLUMNW
#else
#   define  TLM_INSERTCOLUMN    TLM_INSERTCOLUMNA
#endif

#define TreeList_InsertColumn(hwnd, iCol, pcol) \
    (int)SNDMSG((hwnd), TLM_INSERTCOLUMN, (WPARAM)(int)(iCol), \
	(LPARAM)(const TL_COLUMN FAR*)(pcol))

#define TLM_DELETEALLCOLUMNS	(TLM_FIRST + 25)
#define TreeList_DeleteAllColumns(hwnd) \
    (int)SNDMSG((hwnd), TLM_DELETEALLCOLUMNS, 0, 0)

#define TLM_ENSUREVISIBLE       (TLM_FIRST + 28)
#define TreeList_EnsureVisible(hwnd, hitem) \
    (BOOL)SNDMSG((hwnd), TLM_ENSUREVISIBLE, 0, (LPARAM)(HTLITEM)(hitem))

#define TLM_GETCOLUMNWIDTH      (TLM_FIRST + 29)
#define TreeList_GetColumnWidth(hwnd, iCol) \
    (int)SNDMSG((hwnd), TLM_GETCOLUMNWIDTH, (WPARAM)(int)(iCol), 0)

#define TLM_GETORDERARRAY       (TLM_FIRST + 30)
#define TreeList_GetOrderArray(hwnd, isize, iarray) \
    (BOOL)SNDMSG((hwnd), TLM_GETORDERARRAY, \
	(WPARAM)(int)(isize), (LPARAM)(HTLITEM)(iarray))

#define TLM_SETWHEELSCROLLLINES	(TLM_FIRST + 31)
#define TreeList_SetWheelScrollLines(hwnd, ilines) \
    (BOOL)SNDMSG((hwnd), TLM_SETWHEELSCROLLLINES, 0, (LPARAM)(ilines))


typedef struct _NM_TREELIST {
    NMHDR       hdr;
    UINT        flags;
	UINT		index;
    TL_TREEITEMA    itemOld;
    TL_TREEITEMA    itemNew;
    POINT       ptDrag;
} NM_TREELIST, FAR *LPNM_TREELIST;

#define TLN_SELCHANGED          (TLN_FIRST-2)

#define TLC_UNKNOWN             0x0000
#define TLC_BYMOUSE             0x0001
#define TLC_BYKEYBOARD          0x0002
#define TLC_MULTIPLE			0x0080

#define TLN_HEADERCLICKED       (TLN_FIRST-3)
#define TLN_LISTITEMCLICKED		(TLN_FIRST-4)
#define TLN_ITEMDBLCLICKED		(TLN_FIRST-5)
#define TLN_BEGINDRAG           (TLN_FIRST-7)
#define TLN_DROP                (TLN_FIRST-8)
#define TLN_CONTEXTMENU			(TLN_FIRST-9)
#define TLN_KEYDOWN             (TLN_FIRST-12)
#define TLN_CHAR                (TLN_FIRST-13)
#define TLN_DRAGGEDOVER			(TLN_FIRST-14)
#define TLN_HEADERREORDERED		(TLN_FIRST-15)
#define TLN_AUTOSCROLLED		(TLN_FIRST-16)


typedef struct _TL_KEYDOWN {
    NMHDR hdr;
    WORD wVKey;
    UINT flags;
} TL_KEYDOWN;

typedef struct _TL_LISTITEMA
{
    UINT mask;
	UINT state;
	UINT stateMask;
    HTLITEM hItem;
    int iSubItem;
    LPSTR pszText;
    int cchTextMax;
    LONG lDataValue;
} TL_LISTITEMA;

typedef struct _TL_LISTITEMW
{
    UINT mask;
	UINT state;
	UINT stateMask;
    HTLITEM hItem;
    int iSubItem;
    LPWSTR pszText;
    int cchTextMax;
    LONG lDataValue;
} TL_LISTITEMW;

#ifdef UNICODE
#define TL_LISTITEM    TL_LISTITEMW
#else
#define TL_LISTITEM    TL_LISTITEMA
#endif

#define TLM_SETLISTITEMA         (TLM_FIRST + 7)
#define TLM_SETLISTITEMW         (TLM_FIRST + 77)
#ifdef UNICODE
#define TLM_SETLISTITEM          TLM_SETLISTITEMW
#else
#define TLM_SETLISTITEM          TLM_SETLISTITEMA
#endif
#define TreeList_SetListItem(hwnd, pitem, binval)   \
	(int)SNDMSG((hwnd), TLM_SETLISTITEM, binval, \
	(LPARAM)(const TL_LISTITEM FAR*)(pitem))


#define TLM_GETLISTITEMA         (TLM_FIRST + 8)
#define TLM_GETLISTITEMW         (TLM_FIRST + 78)
#ifdef UNICODE
#define TLM_GETLISTITEM          TLM_GETLISTITEMW
#else
#define TLM_GETLISTITEM          TLM_GETLISTITEMA
#endif
#define TreeList_GetListItem(hwnd, pitem)   \
	(int)SNDMSG((hwnd), TLM_GETLISTITEM, 0, \
	(LPARAM)(const TL_LISTITEM FAR*)(pitem))

#endif

//====== Ranges for control message IDs =====================================

#define SPLM_FIRST              0x3000      // Splitter messages


//====== WM_NOTIFY codes (NMHDR.code values) ================================

#define SPLN_FIRST              (0U-300U)       
#define SPLN_LAST               (0U-399U)


//====== SPLITTER CONTROL ===================================================
#ifndef NOSPLITTER

#ifdef _WIN32
#define WC_SPLITTERA            "SplitterFrame32"
#define WC_SPLITTERW            L"SplitterFrame32"

#ifdef UNICODE
#define  WC_SPLITTER            WC_SPLITTERW
#else
#define  WC_SPLITTER            WC_SPLITTERA
#endif

#else
#define WC_SPLITTER             "SplitterFrame"
#endif

// used for CreateWindowEx & Splitter_CreateView
typedef struct tagSPLCREATESTRUCT {
	HWND hwndParent;
	DWORD dwStyle;
	UINT nID;
	
	// for creating new views
	HANDLE hInstance;
	LPVOID lpParam;     // window-creation data
	LPCTSTR lpszName;
	LPCTSTR lpszClass;
	DWORD dwExStyle;
	
	INT nMaxRows;
	INT nMaxCols;
	
	INT nRows;
	INT nCols;
	
	SIZE sizeMin;
} SPLCREATESTRUCT, FAR *LPSPLCREATESTRUCT;

#define SPLM_CREATEVIEW         (SPLM_FIRST + 0)
#define Splitter_CreateView(hwnd, lpsi) \
    (BOOL)SNDMSG((hwnd), SPLM_CREATEVIEW, 0, (LPARAM)(LPSPLCREATESTRUCT)(lpsi))

#define SPLM_SWITCHVIEW         (SPLM_FIRST + 1)
#define Splitter_SwitchView(hwnd, lpsi) \
    (BOOL)SNDMSG((hwnd), SPLM_SWITCHVIEW, 0, (LPARAM)(LPSPLCREATESTRUCT)(lpsi))

#define SPLM_GETROWCOUNT        (SPLM_FIRST + 2)
#define Splitter_GetRowCount(hwnd) \
    (BOOL)SNDMSG((hwnd), SPLM_GETROWCOUNT, 0, 0)

#define SPLM_GETCOLCOUNT        (SPLM_FIRST + 3)
#define Splitter_GetColumnCount(hwnd) \
    (BOOL)SNDMSG((hwnd), SPLM_GETCOLCOUNT, 0, 0)

typedef struct tagSPLROWINFO {
	INT row;
	INT cyCur;
	INT cyMin;
} SPLROWINFO, FAR *LPSPLROWINFO;

#define SPLM_GETROWINFO         (SPLM_FIRST + 4)
#define Splitter_GetRowInfo(hwnd, lpri) \
    (BOOL)SNDMSG((hwnd), SPLM_GETROWINFO, 0, (LPARAM)(LPSPLROWINFO)(lpri))

#define SPLM_SETROWINFO         (SPLM_FIRST + 5)
#define Splitter_SetRowInfo(hwnd, lpri) \
    (BOOL)SNDMSG((hwnd), SPLM_SETROWINFO, 0, (LPARAM)(LPSPLROWINFO)(lpri))

typedef struct tagSPLCOLINFO {
	INT col;
	INT cxCur;
	INT cxMin;
} SPLCOLINFO, FAR *LPSPLCOLINFO;

#define SPLM_GETCOLINFO         (SPLM_FIRST + 6)
#define Splitter_GetColumnInfo(hwnd, lpci) \
    (BOOL)SNDMSG((hwnd), SPLM_GETCOLINFO, 0, (LPARAM)(LPSPLCOLINFO)(lpci))

#define SPLM_SETCOLINFO         (SPLM_FIRST + 7)
#define Splitter_SetColumnInfo(hwnd, lpci) \
    (BOOL)SNDMSG((hwnd), SPLM_SETCOLINFO, 0, (LPARAM)(LPSPLCOLINFO)(lpci))

typedef struct tagSPLPANEINFO {
	INT row;
	INT col;
} SPLPANEINFO, FAR *LPSPLPANEINFO;

#define SPLM_GETPANE            (SPLM_FIRST + 8)
#define Splitter_GetPane(hwnd, lppi) \
    (HWND)SNDMSG((hwnd), SPLM_GETPANE, 0, (LPARAM)(LPSPLPANEINFO)(lppi))

#define SPLM_ISCHILDPANE        (SPLM_FIRST + 9)
#define Splitter_IsChildPane(hwnd, child, lppi) \
    (BOOL)SNDMSG((hwnd), SPLM_ISCHILDPANE, (WPARAM)child, (LPARAM)(LPSPLPANEINFO)(lppi))

#define SPLM_GETACTIVEPANE      (SPLM_FIRST + 10)
#define Splitter_GetActivePane(hwnd, lppi) \
    (HWND)SNDMSG((hwnd), SPLM_GETACTIVEPANE, 0, (LPARAM)(LPSPLPANEINFO)(lppi))

#define SPLM_SETACTIVEPANE      (SPLM_FIRST + 11)
#define Splitter_SetActivePane(hwnd, child, lppi) \
    (BOOL)SNDMSG((hwnd), SPLM_SETACTIVEPANE, (WPARAM)child, (LPARAM)(LPSPLPANEINFO)(lppi))

#endif

#define ICC_SPLITTER_CLASS	0x00000001

#define EC_IDW_PANE_FIRST	0xE900  // first pane (256 max)
#define EC_IDW_PANE_LAST	0xE9ff
#define EC_IDW_HSCROLL_FIRST	0xEA00  // first Horz scrollbar (16 max)
#define EC_IDW_VSCROLL_FIRST	0xEA10  // first Vert scrollbar (16 max)
#define EC_IDW_SIZE_BOX		0xEA20  // size box for splitters

#define CX_BORDER2		2
#define CY_BORDER2		2
#define CX_BORDER		1
#define CY_BORDER		1
#define CX_VSCROLL		17
#define CY_HSCROLL		17

typedef struct tagINITEXTENDEDCONTROLSEX {
    DWORD dwSize;
    DWORD dwICC;
} INITEXTENDEDCONTROLSEX, *LPINITEXTENDEDCONTROLSEX;

VOID WINAPI InitExtendedControls(VOID);
BOOL WINAPI InitExtendedControlsEx(LPINITEXTENDEDCONTROLSEX lpInitCtrls);

#ifdef __cplusplus
}
#endif

#ifdef _WIN32
#include <poppack.h>
#endif

#endif	  //NOUSER

#endif // __INC_EXTDCTRL_H

