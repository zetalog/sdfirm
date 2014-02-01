//: TreeList Tree linked list management routines
//
//	$Id: TLTree.c,v 1.1 2007/03/12 14:54:21 zhenglv Exp $
//

#include "TLintern.h"


//----------------------------------------------------|
//  Remove the specified item from the tree linked list

static VOID 
sRemoveItemFromTree (
		TLWndData* pWD, 
		TLTreeItem* p) 
{
	if (p->prevItem) 
		p->prevItem->nextItem = p->nextItem;
	else if (p->parentItem) 
		p->parentItem->childItem = p->nextItem;
	else 
		pWD->rootItem = p->nextItem;

	if (p->nextItem) 
		p->nextItem->prevItem = p->prevItem;

	if (p->parentItem) 
		p->parentItem->cChildren--;

	p->nextItem = NULL;
	p->prevItem = NULL;
}


//----------------------------------------------------|
//  Reinsert Tree item into list

HTLITEM TLReInsertTreeItem (TLWndData* pWD, LPTL_INSERTSTRUCT lpIns) {
	TLTreeItem* lpNew;

	if (lpIns->item.hItem) {
		lpNew = (TLTreeItem*)(lpIns->item.hItem);
		sRemoveItemFromTree (pWD, lpNew);
		return TLInsertTreeItem (pWD, lpIns);
	}

	return NULL;
}


//----------------------------------------------------|
//  Insert Tree item into list

HTLITEM TLInsertTreeItem (TLWndData* pWD, LPTL_INSERTSTRUCT lpIns) {
	TLTreeItem* lpParent;
	TLTreeItem* lpPrev;
	TLTreeItem* lpNew;
	TLListItem* pli;
	int i;

	// allocate new linked list structure if needed
	if (!lpIns->item.hItem) {
		lpNew = TLAlloc (sizeof (TLTreeItem));
		if (!lpNew) return NULL;
		lpNew->state = 0;
		lpNew->childItem = NULL;
		lpNew->listItem = NULL;
		lpNew->cChildren = 0;
	}
	else {
		lpNew = (TLTreeItem*)(lpIns->item.hItem);
	}

	// get parent item
	lpParent = (TLTreeItem*)lpIns->hParent;
	lpNew->parentItem = lpParent;

	// Insert item into list at appropriate spot
	if (lpParent) {

	//not a root item
		if (lpIns->hInsertAfter == TLI_FIRST) {
			lpNew->nextItem = lpParent->childItem;
			if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
			lpParent->childItem = lpNew;
			lpNew->prevItem = NULL;
		}
		else if (lpIns->hInsertAfter == TLI_LAST) {
			lpPrev = lpParent->childItem;
			if (lpPrev) {
				while (lpPrev->nextItem) lpPrev = lpPrev->nextItem;
				lpPrev->nextItem = lpNew;
				lpNew->prevItem = lpPrev;
				lpNew->nextItem = NULL;
			}
			else {
				lpParent->childItem = lpNew;
				lpNew->nextItem = NULL;
				lpNew->prevItem = NULL;
			}
		}
		else if (lpIns->hInsertAfter == TLI_SORT) {
			lpPrev = lpParent->childItem;
			if (!lpPrev) {
				lpParent->childItem = lpNew;
				lpNew->nextItem = NULL;
				lpNew->prevItem = NULL;
			}
			else {
				if (lstrcmp (lpPrev->pszText, lpIns->item.pszText) >= 0) {
					lpNew->nextItem = lpParent->childItem;
					if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
					lpParent->childItem = lpNew;
					lpNew->prevItem = NULL;
				}
				else {
					while (lpPrev->nextItem && 
						(lstrcmp (lpPrev->nextItem->pszText, 
								  lpIns->item.pszText) < 0)) 
						lpPrev = lpPrev->nextItem;
					lpNew->nextItem = lpPrev->nextItem;
					if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
					lpPrev->nextItem = lpNew;
					lpNew->prevItem = lpPrev;
				}
			}
		}
		else if (lpIns->hInsertAfter == TLI_SORTREVERSE) {
			lpPrev = lpParent->childItem;
			if (!lpPrev) {
				lpParent->childItem = lpNew;
				lpNew->nextItem = NULL;
				lpNew->prevItem = NULL;
			}
			else {
				if (lstrcmp (lpPrev->pszText, lpIns->item.pszText) <= 0) {
					lpNew->nextItem = lpParent->childItem;
					if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
					lpParent->childItem = lpNew;
					lpNew->prevItem = NULL;
				}
				else {
					while (lpPrev->nextItem && 
						(lstrcmp (lpPrev->nextItem->pszText, 
								  lpIns->item.pszText) > 0)) 
						lpPrev = lpPrev->nextItem;
					lpNew->nextItem = lpPrev->nextItem;
					if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
					lpPrev->nextItem = lpNew;
					lpNew->prevItem = lpPrev;
				}
			}
		}
		else {
			lpPrev = (TLTreeItem*)lpIns->hInsertAfter;
			if (lpPrev) {
				lpNew->prevItem = lpPrev;
				lpNew->nextItem = lpPrev->nextItem;
				lpPrev->nextItem = lpNew;
				if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
			}
		}

		lpParent->cChildren++;
	}

	// this is a root item
	else {
		if (lpIns->hInsertAfter == TLI_FIRST) {
			lpNew->nextItem = pWD->rootItem;
			if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
			lpNew->prevItem = NULL;
			pWD->rootItem = lpNew;
		}
		else if (lpIns->hInsertAfter == TLI_LAST) {
			lpPrev = pWD->rootItem;
			if (lpPrev) {
				while (lpPrev->nextItem) lpPrev = lpPrev->nextItem;
				lpPrev->nextItem = lpNew;
				lpNew->prevItem = lpPrev;
				lpNew->nextItem = NULL;
			}
			else {
				pWD->rootItem = lpNew;
				lpNew->nextItem = NULL;
				lpNew->prevItem = NULL;
			}
		}
		else if (lpIns->hInsertAfter == TLI_SORT) {
			lpPrev = pWD->rootItem;
			if (!lpPrev) {
				pWD->rootItem = lpNew;
				lpNew->nextItem = NULL;
				lpNew->prevItem = NULL;
			}
			else {
				if (lstrcmp (lpPrev->pszText, lpIns->item.pszText) >= 0) {
					lpNew->nextItem = pWD->rootItem;
					if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
					pWD->rootItem = lpNew;
					lpNew->prevItem = NULL;
				}
				else {
					while (lpPrev->nextItem && 
						(lstrcmp (lpPrev->nextItem->pszText, 
								  lpIns->item.pszText) < 0)) 
						lpPrev = lpPrev->nextItem;
					lpNew->nextItem = lpPrev->nextItem;
					if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
					lpPrev->nextItem = lpNew;
					lpNew->prevItem = lpPrev;
				}
			}		
		}
		else if (lpIns->hInsertAfter == TLI_SORTREVERSE) {
			lpPrev = pWD->rootItem;
			if (!lpPrev) {
				pWD->rootItem = lpNew;
				lpNew->nextItem = NULL;
				lpNew->prevItem = NULL;
			}
			else {
				if (lstrcmp (lpPrev->pszText, lpIns->item.pszText) <= 0) {
					lpNew->nextItem = pWD->rootItem;
					if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
					pWD->rootItem = lpNew;
					lpNew->prevItem = NULL;
				}
				else {
					while (lpPrev->nextItem && 
						(lstrcmp (lpPrev->nextItem->pszText, 
								  lpIns->item.pszText) > 0)) 
						lpPrev = lpPrev->nextItem;
					lpNew->nextItem = lpPrev->nextItem;
					if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
					lpPrev->nextItem = lpNew;
					lpNew->prevItem = lpPrev;
				}
			}		
		}
		else {
			lpPrev = (TLTreeItem*)lpIns->hInsertAfter;
			if (lpPrev) {
				lpNew->prevItem = lpPrev;
				lpNew->nextItem = lpPrev->nextItem;
				lpPrev->nextItem = lpNew;
				if (lpNew->nextItem) lpNew->nextItem->prevItem = lpNew;
			}
		}

	}

	// only do the following if this is a new item
	if (!lpIns->item.hItem) {		
		if (lpIns->item.mask & TLIF_TEXT) {
			lpNew->pszText = TLAlloc (lstrlen (lpIns->item.pszText) +1);
			if (lpNew->pszText) lstrcpy (lpNew->pszText, lpIns->item.pszText);
		}
		else lpNew->pszText = NULL;
	
		if (lpIns->item.mask & TLIF_IMAGE) lpNew->iImage = lpIns->item.iImage;
		else lpNew->iImage = 0;
	
		if (lpIns->item.mask & TLIF_PARAM) lpNew->lParam = lpIns->item.lParam;
		else lpNew->lParam = 0;
	
		if (lpIns->item.mask & TLIF_STATE) {
			lpNew->state &= ~lpIns->item.stateMask;
			lpNew->state |= (lpIns->item.stateMask & lpIns->item.state);
		}
	
		if (pWD->iNumberColumns > 1) {
			lpNew->listItem = TLAlloc (sizeof (TLListItem));
			pli = lpNew->listItem;
			for (i=2; i<pWD->iNumberColumns; i++) {
				pli->nextItem = TLAlloc (sizeof (TLListItem));
				pli = pli->nextItem;
			}
		}
	}

	return (HTLITEM)lpNew;

}


//----------------------------------------------------|
//  Delete all children of item

BOOL TLDeleteChildren (TLWndData* pWD, TL_TREEITEM* lpti) {
	TLTreeItem* p;

	p = (TLTreeItem*) lpti->hItem;
	if (!p) return FALSE;

	if (p->childItem) {
		TLDeleteTreeList (pWD, p->childItem);
		p->childItem = NULL;
		p->cChildren = 0;
		return TRUE;
	}
	else
		return FALSE;
}


//----------------------------------------------------|
//  Delete Tree item

BOOL TLDeleteTreeItem (TLWndData* pWD, TL_TREEITEM* lpti) {
	TLTreeItem* p;
	TLListItem* pli;
	TLListItem* opli;

	p = (TLTreeItem*) lpti->hItem;
	if (!p) return FALSE;

	sRemoveItemFromTree (pWD, p);

	if (p->childItem) TLDeleteTreeList (pWD, p->childItem);

	if (pWD->focusItem == p) pWD->focusItem = NULL;
	if (pWD->firstSelectItem == p) pWD->firstSelectItem = NULL;

	pli = p->listItem;
	while (pli) {
		TLFree (pli->pszText);
		opli = pli;
		pli = pli->nextItem;
		TLFree (opli);
	}

	TLFree (p->pszText);
	TLFree (p);

	return TRUE;
}


//----------------------------------------------------|
//  Get Tree item information

BOOL TLGetTreeItem (TLWndData* pWD, TL_TREEITEM* lpti) {
	TLTreeItem* p;

	p = (TLTreeItem*) lpti->hItem;
	if (!p) return FALSE;

	if (lpti->mask & TLIF_TEXT) {
		if (lpti->cchTextMax > lstrlen (p->pszText)) 
			lstrcpy (lpti->pszText, p->pszText);
		else {
			lpti->pszText = NULL;
			return FALSE;
		}
	}

	if (lpti->mask & TLIF_IMAGE) lpti->iImage = p->iImage;

	if (lpti->mask & TLIF_PARAM) lpti->lParam = p->lParam;

	if (lpti->mask & TLIF_STATE) lpti->state = p->state;

	if (lpti->mask & TLIF_NEXTHANDLE) lpti->hItem = (HTLITEM)p->nextItem;
	else if (lpti->mask & TLIF_PREVHANDLE) lpti->hItem = (HTLITEM)p->prevItem;
	else if (lpti->mask & TLIF_PARENTHANDLE) lpti->hItem = 
												(HTLITEM)p->parentItem;
	else if (lpti->mask & TLIF_CHILDHANDLE) lpti->hItem = 
												(HTLITEM)p->childItem;

	return TRUE;
}


//----------------------------------------------------|
//  Set Tree item information

BOOL TLSetTreeItem (TLWndData* pWD, TL_TREEITEM* lpti) {
	TLTreeItem* p;

	p = (TLTreeItem*) lpti->hItem;
	if (!p) return FALSE;

	if (lpti->mask & TLIF_TEXT) {
		p->pszText = TLReAlloc (p->pszText, lstrlen (lpti->pszText) +1);
		lstrcpy (p->pszText, lpti->pszText);
	}

	if (lpti->mask & TLIF_IMAGE) p->iImage = lpti->iImage;

	if (lpti->mask & TLIF_PARAM)  p->lParam = lpti->lParam;

	if (lpti->mask & TLIF_STATE) {
		p->state &= ~(lpti->stateMask);
		p->state |= (lpti->state & lpti->stateMask);
	}

	TLInvalidateItem (pWD, p);
	return TRUE;
}


//----------------------------------------------------|
//  Deselect Tree

BOOL TLDeselectTree (TLWndData* pWD, TLTreeItem* p, TLTreeItem* pExcept) {
	BOOL bDeselected = FALSE;

	while (p) {
		if (p->childItem) 
			bDeselected = TLDeselectTree (pWD, p->childItem, pExcept);
		if (p != pExcept) {
			if (p->state & TLIS_SELECTED) {
				p->state &= ~TLIS_SELECTED;
				pWD->iNumberSelected--;
				TLInvalidateItem (pWD, p);
				bDeselected = TRUE;
			}
			if (p->state & TLIS_FOCUSED) {
				p->state &= ~TLIS_FOCUSED;
				pWD->nmTreeList.flags = 0;
				pWD->nmTreeList.itemOld.hItem = NULL;
				pWD->nmTreeList.itemNew.hItem = NULL;
				pWD->nmTreeList.itemNew.state = 0;
				pWD->nmTreeList.hdr.code = TLN_SELCHANGED;
				SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
								(LPARAM)&(pWD->nmTreeList));
			}
		}
		p = p->nextItem;
	}
	return bDeselected;
}


//----------------------------------------------------|
//  Expand Tree

void TLExpandTree (TLTreeItem* p) {
	while (p) {
		if (p->childItem) {
			p->state |= TLIS_EXPANDED;
			TLExpandTree (p->childItem);
		}
		p = p->nextItem;
	}
}


//----------------------------------------------------|
//  Expand entire Tree

void ExpandTree (TLTreeItem* p) {
	while (p) {
		if (p->childItem) ExpandTree (p->childItem);
		p->state |= TLIS_EXPANDED;
		p = p->nextItem;
	}
}


//----------------------------------------------------|
//  Expand/Collapse Tree item 

BOOL TLExpandTreeItem (TLWndData* pWD, TL_TREEITEM* lpti, int flags) {
	TLTreeItem* p;


	switch (flags) {
	case TLE_COLLAPSE :
		p = (TLTreeItem*) lpti->hItem;
		if (!p) return FALSE;
		if (p->state & TLIS_EXPANDED) {
			p->state &= ~TLIS_EXPANDED;
			TLDeselectTree (pWD, p->childItem, NULL);
			TLSelectAutoScroll (pWD, p);
			return TRUE;
		}
		else return FALSE;

	case TLE_EXPAND :
		p = (TLTreeItem*) lpti->hItem;
		if (!p) return FALSE;
		if (p->state & TLIS_EXPANDED) return FALSE;
		else {
			p->state |= TLIS_EXPANDED;
			TLExpandAutoScroll (pWD, p);
			return TRUE;
		}
		break;

	case TLE_TOGGLE :
		p = (TLTreeItem*) lpti->hItem;
		if (!p) return FALSE;
		if (p->state & TLIS_EXPANDED) {
			p->state &= ~TLIS_EXPANDED;
			TLDeselectTree (pWD, p->childItem, NULL);
			TLSelectAutoScroll (pWD, p);
			return TRUE;
		}
		else p->state |= TLIS_EXPANDED;
		return TRUE;

	case TLE_COLLAPSEALL :
		p = NULL;
		if (lpti) p = (TLTreeItem*) lpti->hItem;
		if (p) {
			p->state &= ~TLIS_EXPANDED;
			TLDeselectTree (pWD, p->childItem, NULL);
			TLSelectAutoScroll (pWD, p);
		}
		else {
			p = pWD->rootItem;
			while (p) {
				p->state &= ~TLIS_EXPANDED;
				TLDeselectTree (pWD, p->childItem, NULL);
				p = p->nextItem;
			}
			TLSelectAutoScroll (pWD, pWD->rootItem);
		}
		return TRUE;

	case TLE_EXPANDALL :
		p = NULL;
		if (lpti) p = (TLTreeItem*) lpti->hItem;
		if (p) {
			p->state |= TLIS_EXPANDED;
			TLExpandTree (p->childItem);
			TLExpandAutoScroll (pWD, p);
		}
		else {
			TLExpandTree (pWD->rootItem);
			TLExpandAutoScroll (pWD, pWD->rootItem);
		}
		return TRUE;
	}

	return FALSE;
}


//----------------------------------------------------|
//  Select Tree item 

BOOL TLSelectTreeItem (TLWndData* pWD, TL_TREEITEM* lpti, BOOL bDeselect) {
	TLTreeItem* p;

	if (pWD->focusItem) pWD->focusItem->state &= ~TLIS_FOCUSED;
	if (bDeselect) {
		TLDeselectTree (pWD, pWD->rootItem, NULL);
		pWD->iNumberSelected = 0;
	}
	if (lpti) {
		p = (TLTreeItem*) lpti->hItem;
		if (!p) return FALSE;
		p->state |= TLIS_SELECTED | TLIS_FOCUSED;
		pWD->focusItem = p;
		pWD->firstSelectItem = p;
		(pWD->iNumberSelected)++;
		if (TLSelectAutoScroll (pWD, p)) 
			InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
		else TLInvalidateItem (pWD, p);
		if (pWD->iNumberSelected > 1) pWD->nmTreeList.flags = TLC_MULTIPLE;
		else pWD->nmTreeList.flags = 0;
		pWD->nmTreeList.itemOld.hItem = NULL;
		pWD->nmTreeList.itemNew.hItem = (HTLITEM)p;
		pWD->nmTreeList.itemNew.state = p->state;
		pWD->nmTreeList.hdr.code = TLN_SELCHANGED;
		SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
							(LPARAM)&(pWD->nmTreeList));
	}
	return TRUE;
}


//----------------------------------------------------|
//  Select all the children of a Tree item 

BOOL TLSelectChildren (TLWndData* pWD, TL_TREEITEM* lpti) {
	TLTreeItem* p;

	TLDeselectTree (pWD, pWD->rootItem, NULL);
	pWD->iNumberSelected = 0;

	if (lpti == NULL) {
		p = pWD->rootItem;
		pWD->focusItem = p;
	}
	else {
		p = (TLTreeItem*) lpti->hItem;
		if (p) p = p->childItem;
	}

	if (!p) return FALSE;

	while (p) {
		p->state |= TLIS_SELECTED;
		pWD->iNumberSelected++;
		TLInvalidateItem (pWD, p);
		p = p->nextItem;
	}
	return TRUE;
}


//----------------------------------------------------|
//  Delete Tree List linked list

void TLDeleteTreeList (TLWndData* pWD, TLTreeItem* p) {
	TLTreeItem* op;
	TLListItem* pli;
	TLListItem* opli;

	while (p) {
		if (pWD->focusItem == p) pWD->focusItem = NULL;
		if (pWD->firstSelectItem == p) pWD->firstSelectItem = NULL;

		if (p->childItem) TLDeleteTreeList (pWD, p->childItem);

		pli = p->listItem;
		while (pli) {
			TLFree (pli->pszText);
			opli = pli;
			pli = pli->nextItem;
			TLFree (opli);
		}

		TLFree (p->pszText);
		op = p;
		p = p->nextItem;
		TLFree (op);
	}
}


//----------------------------------------------------|
//  Delete Column linked list

BOOL TLDeleteAllColumns (TLWndData* pWD) {
	TLColumnItem* p;
	TLColumnItem* opci;
	INT i;

	for (i=pWD->iNumberColumns-1; i>=0; i--) 
		Header_DeleteItem (pWD->hWndHeader, i);

	p = pWD->columnItem;
	while (p) {
		opci = p;
		p = p->nextItem;
		TLFree (opci);
	}
		
	pWD->columnItem = NULL;		
	pWD->iNumberColumns = 0;

	return TRUE;
}


//----------------------------------------------------|
//  Insert column into TreeList

LONG TLInsertColumn (TLWndData* pWD, LONG index, TL_COLUMN* tlc) {

	HD_ITEM hditem;
	TLColumnItem* pci;

	hditem.mask = 0;

	if (index == 0) {
		if (tlc->mask & TLCF_WIDTH) pWD->iFirstColumnWidth = tlc->cx;
		hditem.mask = HDI_FORMAT | HDI_WIDTH |HDI_TEXT;
		hditem.fmt = tlc->fmt;
		hditem.cxy = tlc->cx;
		hditem.pszText = tlc->pszText;
		pWD->iNumberColumns++;
		pWD->iFirstColumnWidth = tlc->cx;
		pWD->iTotalWidth = tlc->cx;
		return (Header_InsertItem (pWD->hWndHeader, index, &hditem)); 
	}

	pWD->iTotalWidth += tlc->cx;

	pci = pWD->columnItem;
	if (pci) {
		while (pci->nextItem) pci = pci->nextItem;
		pci->nextItem = TLAlloc (sizeof (TLColumnItem));
		pci = pci->nextItem;
	}
	else {
		pWD->columnItem = TLAlloc (sizeof (TLColumnItem));
		pci = pWD->columnItem;
	}
	pci->iSubItem = index;

	if (tlc->mask & TLCF_DATATYPE) {
		pci->iDataType = tlc->iDataType;
	}
	if (tlc->mask & TLCF_DATAMAX) {
		pci->lMaxValue = tlc->cchTextMax;
	}
	if (tlc->mask & TLCF_FMT) {
		hditem.mask |= HDI_FORMAT;
		hditem.fmt = tlc->fmt;
		if ((hditem.fmt == TLCFMT_LOGBAR) || 
			(hditem.fmt == TLCFMT_LINBAR) ||
			(hditem.fmt == TLCFMT_IMAGE)) 
				hditem.fmt = HDF_CENTER;
		else if (hditem.fmt == TLCFMT_IMAGELIST)
				hditem.fmt = HDF_LEFT;
		pci->iDataFmt = tlc->fmt;
	}
	if (tlc->mask & TLCF_WIDTH) {
		hditem.mask |= HDI_WIDTH;
		hditem.cxy = tlc->cx;
		pci->cx = tlc->cx;
		if (pci->cx >= BITMAPWIDTH) 
			pci->cxImageOffset = (pci->cx - BITMAPWIDTH) / 2;
		else 
			pci->cxImageOffset = 0;
	}
	if (tlc->mask & TLCF_TEXT) {
		hditem.mask |= HDI_TEXT;
		hditem.pszText = tlc->pszText;
	}
	if (tlc->mask & TLCF_SUBITEM) {
		hditem.mask |= HDI_LPARAM;
		hditem.lParam = tlc->iSubItem;
	}
	if (tlc->mask & TLCF_MOUSENOTIFY) {
		pci->bMouseNotify = tlc->bMouseNotify;
	}
	pWD->iNumberColumns++;
	
	return (Header_InsertItem (pWD->hWndHeader, index, &hditem)); 
}


//----------------------------------------------------|
//  Change column attributes

LONG TLSetColumn (TLWndData* pWD, LONG index, TL_COLUMN* tlc) {

	TLColumnItem* pci;
	HD_ITEM hdi;
	int i;

	if (index < 1) return -1;
	if (index >= pWD->iNumberColumns) return -1;

	hdi.mask = 0;

	pci = pWD->columnItem;
	for (i=1; i<index; i++) {
		if (pci) pci = pci->nextItem;
	}
	pci->iSubItem = index;

	if (tlc->mask & TLCF_DATATYPE) {
		pci->iDataType = tlc->iDataType;
	}
	if (tlc->mask & TLCF_FMT) {
		pci->iDataFmt = tlc->fmt;
	}
	if (tlc->mask & TLCF_DATAMAX) {
		pci->lMaxValue = tlc->cchTextMax;
	}
	if (tlc->mask & TLCF_WIDTH) {
		pci->cx = tlc->cx;
		if (pci->cx >= BITMAPWIDTH) 
			pci->cxImageOffset = (pci->cx - BITMAPWIDTH) / 2;
		else 
			pci->cxImageOffset = 0;
		hdi.mask = HDI_WIDTH;
		hdi.cxy = tlc->cx;
		pWD->iTotalWidth += tlc->cx;
	}

	if (hdi.mask) Header_SetItem (pWD->hWndHeader, index, &hdi);

	InvalidateRect (pWD->hWnd, NULL, TRUE);
	return index;
}


//----------------------------------------------------|
//  Get column width

LONG TLGetColumnWidth (TLWndData* pWD, LONG index) {

	TLColumnItem* pci;
	int i;

	if (index < 0) return -1;
	if (index >= pWD->iNumberColumns) return -1;

	if (index == 0) return (pWD->iFirstColumnWidth);

	pci = pWD->columnItem;
	for (i=1; i<index; i++) {
		if (pci) pci = pci->nextItem;
	}
	return pci->cx;
}


//----------------------------------------------------|
//  Set data in list item

LONG TLSetListItem (TLWndData* pWD, TL_LISTITEM* lpli, BOOL bInvalidate) {
	TLListItem* pli;
	int i;

	if (!lpli) return -1;
	if (lpli->iSubItem < 1) return -1;
	if (lpli->iSubItem >= pWD->iNumberColumns) return -1;

	pli = ((TLTreeItem*)(lpli->hItem))->listItem;
	for (i=1; i<lpli->iSubItem; i++) {
		pli = pli->nextItem;
	}

	if (lpli->mask & TLIF_STATE) {
		pli->state &= ~(lpli->stateMask);
		pli->state |= (lpli->stateMask & lpli->state);
	}
	if (lpli->mask & TLIF_TEXT) {
		TLFree (pli->pszText);
		if (lpli->pszText) {
			pli->pszText = TLAlloc (lstrlen (lpli->pszText) +1);
			lstrcpy (pli->pszText, lpli->pszText);
		}
		else
			pli->pszText = NULL;
	}
	if (lpli->mask & TLIF_DATAVALUE) {
		pli->lDataValue = lpli->lDataValue;
	}

	if (bInvalidate) TLInvalidateItem (pWD, (TLTreeItem*)lpli->hItem);
	return (lpli->iSubItem);
} 


//----------------------------------------------------|
//  Get data in list item

LONG TLGetListItem (TLWndData* pWD, TL_LISTITEM* lpli) {
	TLListItem* pli;
	int i;

	if (!lpli) return -1;
	if (lpli->iSubItem < 1) return -1;
	if (lpli->iSubItem >= pWD->iNumberColumns) return -1;

	pli = ((TLTreeItem*)(lpli->hItem))->listItem;
	for (i=1; i<lpli->iSubItem; i++) {
		pli = pli->nextItem;
	}

	if (lpli->mask & TLIF_TEXT) {
		if (lpli->pszText) {
			if (lstrlen (pli->pszText) < lpli->cchTextMax)
				lstrcpy (lpli->pszText, pli->pszText);
		}
	}

	if (lpli->mask & TLIF_DATAVALUE) {
		lpli->lDataValue = pli->lDataValue;
	}

	return (lpli->iSubItem);
} 


//----------------------------------------------------|
//  Ensure that specified item is visible 

BOOL TLEnsureVisible (TLWndData* pWD, TL_TREEITEM* lpti) {

	if (!lpti->hItem) return FALSE;
	if (!pWD->rootItem) return FALSE;

	return (TLSelectAutoScroll (pWD, (TLTreeItem*)lpti->hItem));
}


