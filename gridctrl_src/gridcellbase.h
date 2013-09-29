/////////////////////////////////////////////////////////////////////////////
// GridCellBase.h : header file
//
// MFC Grid Control - Grid cell base class header file
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
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
#define AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CGridCtrl;

#define SELECTED_CELL_FONT_WEIGHT 600    // weight of text for selected items

// Each cell contains one of these. Fields "row" and "column" are not stored since we
// will usually have acces to them in other ways, and they are an extra 8 bytes per
// cell that is probably unnecessary.

class CGridCellBase : public CObject
{
    friend class CGridCtrl;
    DECLARE_DYNCREATE(CGridCellBase)

// Construction/Destruction
public:
    CGridCellBase();
    virtual ~CGridCellBase();

// Attributes
public:
    // can't do pure virtual because of DECLARE_DYNCREATE requirement
    //  use ASSERT() to require that programmer overrides all that should
    //  be pure virtuals

    virtual void SetText(LPCTSTR /* szText */)              { ASSERT( FALSE);    }
    virtual void SetImage(int /* nImage */)                 { ASSERT( FALSE);    }
    virtual void SetData(LPARAM /* lParam */)               { ASSERT( FALSE);    }
    virtual void SetState(DWORD nState)                     { m_nState = nState; }
    virtual void SetFormat(DWORD /* nFormat */)             { ASSERT( FALSE);    }
    virtual void SetTextClr(COLORREF /* clr */)             { ASSERT( FALSE);    }
    virtual void SetBackClr(COLORREF /* clr */)             { ASSERT( FALSE);    }
    virtual void SetFont(const LOGFONT* /* plf */)          { ASSERT( FALSE);    }
    virtual void SetMargin( UINT /* nMargin */)             { ASSERT( FALSE);    }
    virtual void SetGrid(CGridCtrl* /* pGrid */)            { ASSERT( FALSE);    }
    virtual void SetCoords( int /* nRow */, int /* nCol */) { ASSERT( FALSE);    }

    virtual LPCTSTR  GetText()                              { ASSERT( FALSE); return NULL;  }
    virtual LPCTSTR  GetTipText()                           { return GetText();             } // may override TitleTip return
    virtual int      GetImage()                             { ASSERT( FALSE); return -1;    }
    virtual LPARAM   GetData()                              { ASSERT( FALSE); return 0;     }
    virtual DWORD    GetState()                             { return m_nState;              }
    virtual DWORD    GetFormat()                            { ASSERT( FALSE); return 0;     }
    virtual COLORREF GetTextClr()                           { ASSERT( FALSE); return 0;     }
    virtual COLORREF GetBackClr()                           { ASSERT( FALSE); return 0;     }
    virtual const LOGFONT* GetFont()                        { ASSERT( FALSE); return NULL;  }
    virtual CGridCtrl* GetGrid()                            { ASSERT( FALSE); return NULL;  }
    virtual UINT GetMargin()                                { ASSERT( FALSE); return 0;     }

    virtual BOOL IsEditing()                                { ASSERT( FALSE); return FALSE; }

// Operators
public:
    virtual void operator=(CGridCellBase& cell);

// Operations
public:
    virtual void Reset();

    virtual BOOL Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual BOOL GetTextRect( LPRECT pRect);    // i/o:  i=dims of cell rect; o=dims of text rect
    virtual BOOL GetTipTextRect( LPRECT pRect) { return GetTextRect( pRect); }  // may override for btns, etc.
    virtual CSize GetTextExtent(LPCTSTR str);
    virtual CSize GetCellExtent(CDC* pDC);

    // override the following
    virtual BOOL Edit( int /* nRow */, int /* nCol */, CRect /* rect */, CPoint /* point */, 
                       UINT /* nID */, UINT /* nChar */) { ASSERT( FALSE); return FALSE;}
    virtual void EndEdit() {}

    // EFW - Added to print cells properly
    virtual BOOL PrintCell(CDC* pDC, int nRow, int nCol, CRect rect);

    // add additional protected grid members required of cells
    LRESULT SendMessageToParent(int nRow, int nCol, int nMessage);

protected:
    virtual void OnEndEdit() { ASSERT( FALSE); }
    virtual void OnMouseEnter();
    virtual void OnMouseOver();
    virtual void OnMouseLeave();
    virtual void OnClick( CPoint PointCellRelative);
    virtual void OnClickDown( CPoint PointCellRelative);
    virtual void OnRClick( CPoint PointCellRelative);
    virtual void OnDblClick( CPoint PointCellRelative);
    virtual BOOL OnSetCursor();

protected:
    DWORD    m_nState;      // Cell state (selected/focus etc)
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRIDCELLBASE_H__519FA702_722C_11D1_ABBA_00A0243D1382__INCLUDED_)
