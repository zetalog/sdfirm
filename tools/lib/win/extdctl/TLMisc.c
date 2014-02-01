//: TreeList memory allocation and miscellaneous routines
//
//	$Id: TLMisc.c,v 1.1 2007/03/12 14:54:21 zhenglv Exp $
//

#include "TLintern.h"
#include <malloc.h>
#include <memory.h>


//----------------------------------------------------|
// Private memory allocation routine

void* TLAlloc (long size) {
	void* p;
	p = malloc (size);
	if (!p) {
		MessageBox (NULL, "Memory allocation failure", "Error", MB_OK);
	}
	else {
		memset (p, 0, size);
	}
	return p;
}


//----------------------------------------------------|
// Private memory reallocation routine

void* TLReAlloc (void* p, long size) {
	if (p) {
		free (p);
		return (TLAlloc (size));
	}
	else return NULL;
}


//----------------------------------------------------|
// Private memory deallocation routine

void TLFree (void* p) {
	if (p) free (p);
}


//----------------------------------------------------|
// Return next item

TLTreeItem* NextItem (TLTreeItem* p) {
	if (p->nextItem) return p->nextItem;
	if (p->parentItem) return (NextItem (p->parentItem));
	return NULL;
}


//----------------------------------------------------|
// Return item below the specified one

TLTreeItem* DownItem (TLTreeItem* p, BOOL bShift) {
	TLTreeItem *p2;

	if (bShift) {
		if (p->nextItem) p = p->nextItem;
	}
	else {
		if (p->childItem && (p->state & TLIS_EXPANDED)) return p->childItem;
		if (p->nextItem) return p->nextItem;
		if (p->parentItem) {
			p2 = NextItem (p->parentItem);
			if (p2) return (p2);
		}
	}
	return p;
}


//----------------------------------------------------|
// Return last child item of the specified item

TLTreeItem* LastItem (TLTreeItem* p) {
	if (p->childItem && (p->state & TLIS_EXPANDED)) {
		p = p->childItem;
		while (p->nextItem) p = p->nextItem;
		return (LastItem (p));
	}
	return p;
}


//----------------------------------------------------|
// Return item above the specified one

TLTreeItem* UpItem (TLTreeItem* p, BOOL bShift) {
	if (bShift) {
		if (p->prevItem) p = p->prevItem;
	}
	else {
		if (p->prevItem) return LastItem (p->prevItem);
		if (p->parentItem) return p->parentItem;
	}
	return p;
}


//----------------------------------------------------|
// Process keyboard character

BOOL TLKeyboardChar (TLWndData* pWD, WPARAM ch, LPARAM lP) {
	TLTreeItem*		p;
	INT				iCurrentRow, iFinalRow;
	BOOL			bShift, bControl;

	if (pWD->style & TLS_SINGLESELECT) {
		bShift = FALSE;
		bControl = FALSE;
	}
	else {
		bControl = FALSE;
		bShift = FALSE;	
		if (GetKeyState (VK_SHIFT) & 0x8000) bShift = TRUE;
		else if (GetKeyState (VK_CONTROL) & 0x8000) bControl = TRUE;
	}

	pWD->nmTreeList.flags = 0;

	if (ch == VK_ESCAPE) {
		if (pWD->bDragging && (pWD->style & TLS_INTERNALDRAG)) {
			TLEndDrag (pWD, MAKELPARAM (-1, -1));
			pWD->bDragging = FALSE;
			InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
		}
	}

	if (pWD->focusItem) {

		switch (ch) {
		case VK_DOWN :
			p = DownItem (pWD->focusItem, bShift);
			if (p != pWD->focusItem) {
				pWD->focusItem->state &= ~TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				pWD->focusItem = p;
				if (!bControl) {
					TLDeselectTree (pWD, pWD->rootItem, NULL);
					if (!pWD->firstSelectItem) pWD->firstSelectItem = p;
					if (!bShift) {
						pWD->iNumberSelected = 1;
						pWD->firstSelectItem = p;
					}
					else {
						pWD->iNumberSelected = 0;
						TLSelectRange (pWD, pWD->rootItem, p);
					}
					pWD->focusItem->state |= TLIS_SELECTED;
					pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
				}
				pWD->focusItem->state |= TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				if (TLSelectAutoScroll (pWD, pWD->focusItem)) 
					InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
			}
			break;

		case VK_UP :
			p = UpItem (pWD->focusItem, bShift);
			if (p != pWD->focusItem) {
				pWD->focusItem->state &= ~TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				pWD->focusItem = p;
				if (!bControl) {
					TLDeselectTree (pWD, pWD->rootItem, NULL);
					if (!pWD->firstSelectItem) pWD->firstSelectItem = p;
					if (!bShift) {
						pWD->iNumberSelected = 1;
						pWD->firstSelectItem = p;
					}
					else {
						pWD->iNumberSelected = 0;
						TLSelectRange (pWD, pWD->rootItem, p);
					}
					pWD->focusItem->state |= TLIS_SELECTED;
					pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
				}
				pWD->focusItem->state |= TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				if (TLSelectAutoScroll (pWD, pWD->focusItem)) 
					InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
			}
			break;

		case VK_LEFT :
		case VK_SUBTRACT :
			if (!bShift) {
				if (pWD->focusItem->state & TLIS_EXPANDED) {
					p = pWD->focusItem;
					if (!bControl) {
						TLDeselectTree (pWD, pWD->rootItem, p);
						pWD->iNumberSelected = 1;
						p->state |= TLIS_SELECTED;
						pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
					}
					p->state &= ~TLIS_EXPANDED;
					InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
				}
				else {
					if (ch != VK_SUBTRACT) {
						if (pWD->focusItem->parentItem) {
							pWD->focusItem->state &= ~TLIS_FOCUSED;
							TLInvalidateItem (pWD, pWD->focusItem);
							p = pWD->focusItem->parentItem;
							pWD->focusItem = p;
							if (!bControl) {
								TLDeselectTree (pWD, pWD->rootItem, NULL);
								pWD->firstSelectItem = p;
								pWD->iNumberSelected = 1;
								p->state |= TLIS_SELECTED;
								pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
							}
							p->state |= TLIS_FOCUSED;
							TLInvalidateItem (pWD, p);
							if (TLSelectAutoScroll (pWD, p)) 
								InvalidateRect (pWD->hWnd, &pWD->tlInval,
								TRUE);
						}
					}
				}
			}
			break;

		case VK_RIGHT :
		case VK_RETURN :
		case VK_ADD :
			if (!bShift) {
				if (pWD->focusItem->childItem) {
					if (pWD->focusItem->state & TLIS_EXPANDED) {
						if (ch != VK_ADD) {
							pWD->focusItem->state &= ~TLIS_FOCUSED;
							TLInvalidateItem (pWD, pWD->focusItem);
							p = pWD->focusItem->childItem;
							pWD->focusItem = p;
							if (!bControl) {
								TLDeselectTree (pWD, pWD->rootItem, NULL);
								pWD->firstSelectItem = p;
								pWD->iNumberSelected = 1;
								p->state |= TLIS_SELECTED;
								pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
							}
							p->state |= TLIS_FOCUSED;
							TLInvalidateItem (pWD, p);
							if (TLSelectAutoScroll (pWD, p)) 
								InvalidateRect (pWD->hWnd, &pWD->tlInval,
								TRUE);
						}
					}
					else {
						p = pWD->focusItem;
						if (!bControl) {
							TLDeselectTree (pWD, pWD->rootItem, p);
							pWD->iNumberSelected = 1;
							p->state |= TLIS_SELECTED;
							pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
						}
						p->state |= TLIS_EXPANDED;
						InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
						TLExpandAutoScroll (pWD, pWD->focusItem);
					}
				}
			}
			break;

		case VK_NEXT :
			iCurrentRow = 0;
			TLGetRow (pWD->rootItem, pWD->focusItem, &iCurrentRow);
			if (iCurrentRow >= (pWD->iFirstRow + pWD->iMaxRows -1)) 
				iFinalRow = iCurrentRow + pWD->iMaxRows -1;
			else 
				iFinalRow = pWD->iFirstRow + pWD->iMaxRows -1;
			if (iFinalRow > pWD->iExpandedRows) 
				iFinalRow = pWD->iExpandedRows;
	
			p = pWD->focusItem;
			while (iCurrentRow < iFinalRow) {
				p = DownItem (p, bShift);
				iCurrentRow++;
			}
			if (p != pWD->focusItem) {
				pWD->focusItem->state &= ~TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				pWD->focusItem = p;
				if (bShift) {
					TLDeselectTree (pWD, pWD->rootItem, NULL);
					if (!pWD->firstSelectItem) pWD->firstSelectItem = p;
					pWD->iNumberSelected = 0;
					TLSelectRange (pWD, pWD->rootItem, p);
				}
				else if (!bControl) {
					TLDeselectTree (pWD, pWD->rootItem, NULL);
					pWD->firstSelectItem = p;
					pWD->iNumberSelected = 1;
					pWD->focusItem->state |= TLIS_SELECTED;
					pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
				}
				pWD->focusItem->state |= TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				if (TLSelectAutoScroll (pWD, pWD->focusItem)) 
					InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
			}
			break;

		case VK_PRIOR :
			iCurrentRow = 0;
			TLGetRow (pWD->rootItem, pWD->focusItem, &iCurrentRow);
			if (iCurrentRow == pWD->iFirstRow) 
				iFinalRow = iCurrentRow - pWD->iMaxRows +1;
			else 
				iFinalRow = pWD->iFirstRow;
			if (iFinalRow < 0) iFinalRow = 0;
	
			p = pWD->focusItem;
			while (iCurrentRow > iFinalRow) {
				p = UpItem (p, bShift);
				iCurrentRow--;
			}
			if (p != pWD->focusItem) {
				pWD->focusItem->state &= ~TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				pWD->focusItem = p;
				if (bShift) {
					TLDeselectTree (pWD, pWD->rootItem, NULL);
					if (!pWD->firstSelectItem) pWD->firstSelectItem = p;
					pWD->iNumberSelected = 0;
					TLSelectRange (pWD, pWD->rootItem, p);
				}
				else if (!bControl) {
					TLDeselectTree (pWD, pWD->rootItem, NULL);
					if (!pWD->firstSelectItem) pWD->firstSelectItem = p;
					if (!bShift) pWD->firstSelectItem = p;
					pWD->iNumberSelected = 1;
					pWD->focusItem->state |= TLIS_SELECTED;
					pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
				}
				pWD->focusItem->state |= TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				if (TLSelectAutoScroll (pWD, pWD->focusItem)) 
					InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
			}
			break;

		case VK_HOME :
			p = pWD->rootItem;
			if (p != pWD->focusItem) {
				pWD->focusItem->state &= ~TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				pWD->focusItem = p;
				if (!bControl) {
					TLDeselectTree (pWD, pWD->rootItem, NULL);
					if (!pWD->firstSelectItem) pWD->firstSelectItem = p;
					if (!bShift) {
						pWD->iNumberSelected = 1;
						pWD->firstSelectItem = p;
					}
					else {
						pWD->iNumberSelected = 0;
						TLSelectRange (pWD, pWD->rootItem, p);
					}
					pWD->focusItem->state |= TLIS_SELECTED;
					pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
				}
				pWD->focusItem->state |= TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				if (TLSelectAutoScroll (pWD, pWD->focusItem)) 
					InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
			}
			break;

		case VK_END :
			p = pWD->rootItem;
			while (p->nextItem) p = p->nextItem;
			if (p != pWD->focusItem) {
				pWD->focusItem->state &= ~TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				pWD->focusItem = p;
				if (!bControl) {
					TLDeselectTree (pWD, pWD->rootItem, NULL);
					if (!pWD->firstSelectItem) pWD->firstSelectItem = p;
					if (!bShift) {
						pWD->iNumberSelected = 1;
						pWD->firstSelectItem = p;
					}
					else {
						pWD->iNumberSelected = 0;
						TLSelectRange (pWD, pWD->rootItem, p);
					}
					pWD->focusItem->state |= TLIS_SELECTED;
					pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
				}
				pWD->focusItem->state |= TLIS_FOCUSED;
				TLInvalidateItem (pWD, pWD->focusItem);
				if (TLSelectAutoScroll (pWD, pWD->focusItem)) 
					InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
			}
			break;

		case VK_SPACE :
			if (bControl) {
				p = pWD->focusItem;
				if (pWD->focusItem->state & TLIS_SELECTED) {
					pWD->focusItem->state &= ~TLIS_SELECTED;
					pWD->iNumberSelected--;
				}
				else {
					pWD->focusItem->state |= TLIS_SELECTED;
					pWD->iNumberSelected++;
				}
				pWD->nmTreeList.flags |= TLC_BYKEYBOARD;
				TLInvalidateItem (pWD, pWD->focusItem);
			}
			break;
		}
	}	

	if (pWD->nmTreeList.flags) {
		if (pWD->iNumberSelected > 1) pWD->nmTreeList.flags |= TLC_MULTIPLE;
		pWD->nmTreeList.itemOld.hItem = NULL;
		if (pWD->iNumberSelected > 0) {
			pWD->nmTreeList.itemNew.hItem = (HTLITEM)p;
			pWD->nmTreeList.itemNew.state = p->state;
		}
		else {
			pWD->nmTreeList.itemNew.hItem = NULL;
			pWD->nmTreeList.itemNew.state = 0;
		}
		pWD->nmTreeList.hdr.code = TLN_SELCHANGED;
		SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
						(LPARAM)&(pWD->nmTreeList));
		return TRUE;
	}
	else {
		pWD->nmKeyDown.hdr.code = TLN_KEYDOWN;
		pWD->nmKeyDown.wVKey = ch;
		pWD->nmKeyDown.flags = lP;
		SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
						(LPARAM)&(pWD->nmKeyDown));
		return FALSE;
	}
}


//----------------------------------------------------|
// Process selected objects by calling callback function

BOOL IterateSelected (TLTreeItem* pStart, LPARAM lParam) {
	TL_TREEITEM tli;
	TLTreeItem* p;
	FARPROC lpfnCallback;

	lpfnCallback = *(FARPROC*)lParam;

	if (!pStart) return FALSE;
	p = pStart;

	while (p->nextItem) p = p->nextItem;

	while (p) {
		if (p->state & TLIS_EXPANDED) 
			IterateSelected (p->childItem, lParam);
		if (p->state & TLIS_SELECTED) {
			tli.hItem = (HTLITEM)p;
			tli.pszText = p->pszText;
			tli.iImage = p->iImage;
			tli.lParam = p->lParam;

			p = p->prevItem;
			if (!(lpfnCallback)(&tli, lParam)) return FALSE;
		}
		else p = p->prevItem;
	}
	return TRUE;
}


BOOL TLIterateSelected (TLWndData* pWD, LPARAM lParam) {
	return (IterateSelected (pWD->rootItem, lParam));
}


//----------------------------------------------------|
// Scroll to keyboard-selected key
//
//	c =				character just typed by user
//	bStillTyping =	TRUE => this character is part of a previous
//					string of characters => append to end.

BOOL TLSelectByKeyboard (TLWndData* pWD, LONG c, BOOL bStillTyping) {

	INT i;
	TLTreeItem* p;
	TLTreeItem* pfirst;

	// return if non-printable character
	if (!isprint (c)) return FALSE;

	// escape key deselects selection and clears buffer
	if (c == VK_ESCAPE) {
		pWD->pTypeBufPtr = &(pWD->szTypeBuf[0]);
		pWD->iNumCharInTypeBuf = 0;
		if (pWD->focusItem) {
			pWD->focusItem->state &= ~TLIS_FOCUSED;
			TLInvalidateItem (pWD, pWD->focusItem);
			TLDeselectTree (pWD, pWD->rootItem, NULL);
		}
		pWD->focusItem = NULL;
		pWD->firstSelectItem = NULL;
		pWD->iNumberSelected = 0;
		pWD->nmTreeList.flags = TLC_BYKEYBOARD;
		pWD->nmTreeList.itemOld.hItem = NULL;
		pWD->nmTreeList.itemNew.hItem = NULL;
		pWD->nmTreeList.itemNew.state = 0;
		pWD->nmTreeList.hdr.code = TLN_SELCHANGED;
		SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
						(LPARAM)&(pWD->nmTreeList));
		return FALSE;
	}

	// convert to lower case
	(CHAR)c = tolower ((CHAR)c);

	// get starting point for traversing linked list
	if (pWD->focusItem) {
		p = pWD->focusItem;
		while (p->parentItem) p = p->parentItem;
	}
	else p = pWD->rootItem;
	if (!p) return FALSE;
	pfirst = p;

	// if only one char in buffer and incoming is same character
	// then step to next item which begins with this character
	if ((pWD->iNumCharInTypeBuf == 1) && 
		(pWD->szTypeBuf[0] == (CHAR)c)) 
	{
		if (p->nextItem) p = p->nextItem;
		else p = pWD->rootItem;
		while (tolower (p->pszText[0]) != pWD->szTypeBuf[0]) {
			if (p->nextItem) p = p->nextItem;
			else p = pWD->rootItem;
			if (p == pfirst) break;
		}
	}

	// otherwise do standard matching
	else {
		if (!bStillTyping) {
			pWD->iNumCharInTypeBuf = 0;
			pWD->pTypeBufPtr = &(pWD->szTypeBuf[0]);
		}
		if (pWD->iNumCharInTypeBuf < TYPEBUFFERSIZE-1) {
			*(pWD->pTypeBufPtr) = (CHAR)c;
			(pWD->pTypeBufPtr)++;
			*(pWD->pTypeBufPtr) = '\0';
			pWD->iNumCharInTypeBuf++;

			if (p->nextItem) p = p->nextItem;

			i = lstrlen (pWD->szTypeBuf);
			while (strnicmp (p->pszText, pWD->szTypeBuf, i) != 0) {
				if (p->nextItem) p = p->nextItem;
				else p = pWD->rootItem;
				if (p == pfirst) break;
			}
		}
	}

	// if we have selected new item then handle selection/deselection and
	// notify parent of selection change
	if (p != pfirst) {
		if (pWD->focusItem) {
			pWD->focusItem->state &= ~TLIS_FOCUSED;
			TLInvalidateItem (pWD, pWD->focusItem);
			TLDeselectTree (pWD, pWD->rootItem, NULL);
		}
		pWD->focusItem = p;
		pWD->iNumberSelected = 1;
		pWD->firstSelectItem = p;
		pWD->focusItem->state |= TLIS_FOCUSED | TLIS_SELECTED;
		TLInvalidateItem (pWD, pWD->focusItem);
		if (TLSelectAutoScroll (pWD, pWD->focusItem)) 
			InvalidateRect (pWD->hWnd, &pWD->tlInval, TRUE);
		pWD->nmTreeList.flags = TLC_BYKEYBOARD;
		pWD->nmTreeList.itemOld.hItem = NULL;
		pWD->nmTreeList.itemNew.hItem = (HTLITEM)p;
		pWD->nmTreeList.itemNew.state = p->state;
		pWD->nmTreeList.hdr.code = TLN_SELCHANGED;
		SendMessage (pWD->hWndParent, WM_NOTIFY, pWD->id, 
						(LPARAM)&(pWD->nmTreeList));
	}

	return TRUE;
}



