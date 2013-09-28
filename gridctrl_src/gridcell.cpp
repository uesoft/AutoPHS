// GridCell.cpp : implementation file
//
// MFC Grid Control - Main grid cell class
//
// Provides the implementation for the "default" cell type of the
// grid control. Adds in cell editing.
//
// Written by Chris Maunder <cmaunder@mail.com>
// Copyright (c) 1998-2000. All Rights Reserved.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name and all copyright 
// notices remains intact. 
//
// An email letting me know how you are using it would be nice as well. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// For use with CGridCtrl v2.10
//
// History:
// Eric Woodruff - 20 Feb 2000 - Added PrintCell() plus other minor changes
// Ken Bertelson - 12 Apr 2000 - Split CGridCell into CGridCell and CGridCellBase
// <kenbertelson@hotmail.com>
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GridCell.h"
#include "InPlaceEdit.h"
#include "GridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CGridCell, CGridCellBase)

/////////////////////////////////////////////////////////////////////////////
// GridCell

CGridCell::CGridCell()
{
    Reset();
}

CGridCell::~CGridCell()
{
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Attributes

void CGridCell::Reset()
{
    CGridCellBase::Reset();

    m_nFormat = DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX;
#ifndef _WIN32_WCE
    m_nFormat |= DT_END_ELLIPSIS;
#endif
    m_strText.Empty();
    m_nImage  = -1;
    m_crBkClr = CLR_DEFAULT;
    m_crFgClr = CLR_DEFAULT;
    m_lParam  = 0;
    m_nMargin = 3;
    m_pGrid   = NULL;

    m_bEditing = FALSE;
    m_pEditWnd = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// GridCell Operations

BOOL CGridCell::Edit(int nRow, int nCol, CRect rect, CPoint /* point */, UINT nID, UINT nChar)
{
    DWORD dwStyle = ES_LEFT;
    if (GetFormat() & DT_RIGHT) 
        dwStyle = ES_RIGHT;
    else if (GetFormat() & DT_CENTER) 
        dwStyle = ES_CENTER;

    m_bEditing = TRUE;
    
    // InPlaceEdit auto-deletes itself
    CGridCtrl* pGrid = GetGrid();
    m_pEditWnd = new CInPlaceEdit(pGrid, rect, dwStyle, nID, nRow, nCol, GetText(), nChar);
    
    return TRUE;
}

void CGridCell::EndEdit()
{
    if (m_pEditWnd)
        ((CInPlaceEdit*)m_pEditWnd)->EndEdit();
}

void CGridCell::OnEndEdit()
{
    m_bEditing = FALSE;
    m_pEditWnd = NULL;
}