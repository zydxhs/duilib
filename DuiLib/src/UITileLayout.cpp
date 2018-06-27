#include "stdafx.h"

namespace DuiLib {
CTileLayoutUI::CTileLayoutUI() : m_nColumns(1), m_nRows(0)
    , m_nColumnsFixed(0), m_iChildVPadding(0)
    , m_nRowsFixed(0), m_bChildRounded(false)
{
    m_szItem.cx = m_szItem.cy = 80;
}

LPCTSTR CTileLayoutUI::GetClass() const
{
    return DUI_CTR_TILELAYOUT;
}

LPVOID CTileLayoutUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_TILELAYOUT) == 0) { return static_cast<CTileLayoutUI *>(this); }

    return CContainerUI::GetInterface(pstrName);
}

int CTileLayoutUI::GetFixedColumns() const
{
    return m_nColumnsFixed;
}

void CTileLayoutUI::SetFixedColumns(int iColums)
{
    if (iColums >= 0)
    {
        m_nColumnsFixed = iColums;
        NeedUpdate();
    }
}

int CTileLayoutUI::GetFixedRows() const
{
    return m_nRowsFixed;
}

void CTileLayoutUI::SetFixedRows(int iRows)
{
    if (iRows >= 0 && m_bChildRounded)
    {
        m_nRowsFixed = iRows;
        NeedUpdate();
    }
}

int CTileLayoutUI::GetChildVPadding() const
{
    return m_iChildVPadding;
}

void CTileLayoutUI::SetChildVPadding(int iPadding)
{
    m_iChildVPadding = (iPadding < 0) ? 0 : iPadding;
    NeedUpdate();
}

bool CTileLayoutUI::GetChildRounded() const
{
    return m_bChildRounded;
}

void CTileLayoutUI::SetChildRounded(bool bRound)
{
    if (m_bChildRounded != bRound)
    {
        m_bChildRounded = bRound;
        NeedUpdate();
    }
}

SIZE CTileLayoutUI::GetItemSize() const
{
    return m_szItem;
}

void CTileLayoutUI::SetItemSize(SIZE szSize)
{
    if (m_szItem.cx != szSize.cx || m_szItem.cy != szSize.cy)
    {
        m_szItem = szSize;
        NeedUpdate();
    }
}

int CTileLayoutUI::GetColumns() const
{
    return m_nColumns;
}

int CTileLayoutUI::GetRows() const
{
    return m_nRows;
}

void CTileLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("itemsize")) == 0)
    {
        SIZE szItem = { 0 };
        LPTSTR pstr = NULL;
        m_szItem.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
        m_szItem.cy = _tcstol(pstr + 1, &pstr, 10);   ASSERT(pstr);
    }
    else if (_tcscmp(pstrName, _T("columns")) == 0)
    {
        m_nColumnsFixed = _ttoi(pstrValue);
        m_nColumnsFixed = m_nColumnsFixed > 0 ? m_nColumnsFixed : 1;
    }
    else if (_tcscmp(pstrName, _T("childvpadding")) == 0)
    {
        m_iChildVPadding = _ttoi(pstrValue);
        m_iChildVPadding = m_iChildVPadding >= 0 ? m_iChildVPadding : 0;
    }
    else if (_tcscmp(pstrName, _T("rows")) == 0)
    {
        m_nRowsFixed = _ttoi(pstrValue);
        m_nRowsFixed = m_nRowsFixed > 0 ? m_nRowsFixed : 1;
    }
    else if (_tcscmp(pstrName, _T("childrounded")) == 0)
    {
        m_bChildRounded = (_tcscmp(pstrValue, _T("true")) == 0) ? true : false;
    }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else if (_tcscmp(pstrName, _T("autoheight")) == 0) { DUITRACE(_T("不支持属性:autoheight")); }
    else { CContainerUI::SetAttribute(pstrName, pstrValue); }
}

void CTileLayoutUI::ResetInternVisible(void)
{
    for (int i = 0; i < m_items.GetSize(); ++i)
    {
        CControlUI *pCtrl = static_cast<CControlUI *>(m_items[i]);

        if (pCtrl->IsFloat()) { continue; }

        pCtrl->SetInternVisible(true);
    }
}

void CTileLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);
    rc = m_rcItem;

    // Adjust for inset
    rc.left += m_rcInset.left;
    rc.top += m_rcInset.top;
    rc.right -= m_rcInset.right;
    rc.bottom -= m_rcInset.bottom;

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { rc.right -= m_pVerticalScrollBar->GetFixedWidth(); }

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight(); }

    if (m_items.GetSize() == 0)
    {
        ProcessScrollBar(rc, 0, 0);
        return;
    }

    // Determine the minimum size
    SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
    { szAvailable.cx += m_pHorizontalScrollBar->GetScrollRange(); }

    if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
    { szAvailable.cy += m_pVerticalScrollBar->GetScrollRange(); }

    int nEstimateNum = 0;
    int nMaxVisibleCtrl = m_nRowsFixed * m_nColumnsFixed;   // 子控件铺满时，最大可显示控件数
    ResetInternVisible();                                   // 重置子控件内部显示属性

    for (int it = 0; it < m_items.GetSize(); it++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[it]);

        if (!pControl->IsVisible()) { continue; }

        if (pControl->IsFloat()) { continue; }

        //nEstimateNum++;
        // 隐藏多余的子控件
        if (!m_bChildRounded)                                        { ++nEstimateNum; }
        else if (m_bChildRounded && nMaxVisibleCtrl <= nEstimateNum) { pControl->SetInternVisible(false); }
    }

    int cxNeeded = 0;
    int cyNeeded = 0;
    int iChildPadding = m_iChildPadding;
    int nXRemainder = 0; // 水平方向 等分后剩余像素数
    int nYRemainder = 0; // 水平方向 等分后剩余像素数
    int nCoord = 0;      // 子控件的索引位置（从左到右，从上到下，从0开即递增），用于计算子控件的行列号

    if (m_nColumnsFixed == 0)
    {
        if (rc.right - rc.left >= m_szItem.cx)
        {
            m_nColumns = (rc.right - rc.left) / m_szItem.cx;
            cxNeeded = rc.right - rc.left;

            if (m_nColumns > 1)
            {
                if (iChildPadding <= 0)
                {
                    iChildPadding = (cxNeeded - m_nColumns * m_szItem.cx) / (m_nColumns - 1);
                }

                if (iChildPadding < 0) { iChildPadding = 0; }
            }
            else
            {
                iChildPadding = 0;
            }
        }
        else
        {
            m_nColumns = 1;
            cxNeeded = m_szItem.cx;
        }

        m_nRows = (nEstimateNum - 1) / m_nColumns + 1;
        cyNeeded = m_nRows * m_szItem.cy + (m_nRows - 1) * m_iChildVPadding;
    }
    else
    {
        if (!m_bChildRounded)
        {
            m_nColumns = m_nColumnsFixed;

            if (m_nColumns > 1)
            {
                if (iChildPadding <= 0)
                {
                    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() &&
                        rc.right - rc.left >= m_nColumns * m_szItem.cx)
                    {
                        iChildPadding = (rc.right - rc.left - m_nColumns * m_szItem.cx) / (m_nColumns - 1);
                    }
                    else
                    {
                        iChildPadding = (szAvailable.cx - m_nColumns * m_szItem.cx) / (m_nColumns - 1);
                    }
                }

                if (iChildPadding < 0) { iChildPadding = 0; }
            }
            else { iChildPadding = 0; }

            if (nEstimateNum >= m_nColumns) { cxNeeded = m_nColumns * m_szItem.cx + (m_nColumns - 1) * iChildPadding; }
            else { cxNeeded = nEstimateNum * m_szItem.cx + (nEstimateNum - 1) * iChildPadding; }

            m_nRows = (nEstimateNum - 1) / m_nColumns + 1;
            cyNeeded = m_nRows * m_szItem.cy + (m_nRows - 1) * m_iChildVPadding;
        }
        else
        {
            // 子控件铺满
            m_nColumns = m_nColumnsFixed;
            m_szItem.cx = (rc.right - rc.left - iChildPadding * (m_nColumnsFixed - 1)) / m_nColumnsFixed;
            nXRemainder = (rc.right - rc.left - iChildPadding * (m_nColumnsFixed - 1)) % m_nColumnsFixed;
            m_szItem.cy = (rc.bottom - rc.top - m_iChildVPadding * (m_nRowsFixed - 1)) / m_nRowsFixed;
            nYRemainder = (rc.bottom - rc.top - m_iChildVPadding * (m_nRowsFixed - 1)) % m_nRowsFixed;
        }
    }

    for (int it1 = 0; it1 < m_items.GetSize(); it1++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_items[it1]);

        if (!pControl->IsVisible()) { continue; }

        if (pControl->IsFloat())
        {
            SetFloatPos(it1);
            continue;
        }

        RECT rcPadding = pControl->GetPadding();
        SIZE sz = m_szItem;
        sz.cx -= rcPadding.left + rcPadding.right;
        sz.cy -= rcPadding.top + rcPadding.bottom;

        if (sz.cx > pControl->GetMaxWidth()) { sz.cx = pControl->GetMaxWidth(); }

        if (sz.cy > pControl->GetMaxHeight()) { sz.cy = pControl->GetMaxHeight(); }

        if (sz.cx < 0) { sz.cx = 0; }

        if (sz.cy < 0) { sz.cy = 0; }

        UINT iChildAlign = GetChildAlign();
        UINT iChildVAlign = GetChildVAlign();
        int iColumnIndex = it1 % m_nColumns;
        int iRowIndex = it1 / m_nColumns;
        int iPosX = rc.left + iColumnIndex * (m_szItem.cx + iChildPadding);

        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
        {
            iPosX -= m_pHorizontalScrollBar->GetScrollPos();
        }

        int iPosY = rc.top + iRowIndex * (m_szItem.cy + m_iChildVPadding);

        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
        {
            iPosY -= m_pVerticalScrollBar->GetScrollPos();
        }

        SIZE szTmp = m_szItem;
        RECT rcCtrl;

        if (m_bChildRounded)
        {
            // 修正子控件的大小、位置
            szTmp.cx = (iColumnIndex < nXRemainder) ? (szTmp.cx + 1) : szTmp.cx;
            szTmp.cy = (iRowIndex < nYRemainder) ? (szTmp.cy + 1) : szTmp.cy;
            iPosX += (iColumnIndex < nXRemainder) ? iColumnIndex : nXRemainder;
            iPosY += (iRowIndex < nYRemainder) ? iRowIndex : nYRemainder;

            if (iChildAlign == DT_CENTER)
            {
                if (iChildVAlign == DT_VCENTER)
                {
                    rcCtrl.left = iPosX + (szTmp.cx - sz.cx) / 2 + rcPadding.left;
                    rcCtrl.top = iPosY + (szTmp.cy - sz.cy) / 2 + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else if (iChildVAlign == DT_BOTTOM)
                {
                    rcCtrl.left = iPosX + (szTmp.cx - sz.cx) / 2 + rcPadding.left;
                    rcCtrl.top = iPosY + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else
                {
                    rcCtrl.left = iPosX + (szTmp.cx - sz.cx) / 2 + rcPadding.left;
                    rcCtrl.top = iPosY + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
            }
            else if (iChildAlign == DT_RIGHT)
            {
                if (iChildVAlign == DT_VCENTER)
                {
                    rcCtrl.left = iPosX + szTmp.cx - sz.cx + rcPadding.left;
                    rcCtrl.top = iPosY + (szTmp.cy - sz.cy) / 2 + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + (szTmp.cy - sz.cy) / 2 + sz.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else if (iChildVAlign == DT_BOTTOM)
                {
                    rcCtrl.left = iPosX + szTmp.cx - sz.cx + rcPadding.left;
                    rcCtrl.top = iPosY + szTmp.cy - sz.cy + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else
                {
                    rcCtrl.left = iPosX + szTmp.cx - sz.cx + rcPadding.left;
                    rcCtrl.top = iPosY + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
            }
            else
            {
                if (iChildVAlign == DT_VCENTER)
                {
                    rcCtrl.left = iPosX + rcPadding.left;
                    rcCtrl.top = iPosY + (szTmp.cy - sz.cy) / 2 + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else if (iChildVAlign == DT_BOTTOM)
                {
                    rcCtrl.left = iPosX + rcPadding.left;
                    rcCtrl.top = iPosY + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else
                {
                    rcCtrl.left = iPosX + rcPadding.left;
                    rcCtrl.top = iPosY + rcPadding.top;
                    rcCtrl.right = iPosX + szTmp.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
            }
        }
        else
        {
            if (iChildAlign == DT_CENTER)
            {
                if (iChildVAlign == DT_VCENTER)
                {
                    rcCtrl.left = iPosX + (m_szItem.cx - sz.cx) / 2 + rcPadding.left;
                    rcCtrl.top = iPosY + (m_szItem.cy - sz.cy) / 2 + rcPadding.top;
                    rcCtrl.right = iPosX + (m_szItem.cx - sz.cx) / 2 + sz.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + (m_szItem.cy - sz.cy) / 2 + sz.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else if (iChildVAlign == DT_BOTTOM)
                {
                    rcCtrl.left = iPosX + (m_szItem.cx - sz.cx) / 2 + rcPadding.left;
                    rcCtrl.top = iPosY + m_szItem.cy - sz.cy + rcPadding.top;
                    rcCtrl.right = iPosX + (m_szItem.cx - sz.cx) / 2 + sz.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + m_szItem.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else
                {
                    rcCtrl.left = iPosX + (m_szItem.cx - sz.cx) / 2 + rcPadding.left;
                    rcCtrl.top = iPosY + rcPadding.top;
                    rcCtrl.right = iPosX + (m_szItem.cx - sz.cx) / 2 + sz.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + sz.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
            }
            else if (iChildAlign == DT_RIGHT)
            {
                if (iChildVAlign == DT_VCENTER)
                {
                    rcCtrl.left = iPosX + m_szItem.cx - sz.cx + rcPadding.left;
                    rcCtrl.top = iPosY + (m_szItem.cy - sz.cy) / 2 + rcPadding.top;
                    rcCtrl.right = iPosX + m_szItem.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + (m_szItem.cy - sz.cy) / 2 + sz.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else if (iChildVAlign == DT_BOTTOM)
                {
                    rcCtrl.left = iPosX + m_szItem.cx - sz.cx + rcPadding.left;
                    rcCtrl.top = iPosY + m_szItem.cy - sz.cy + rcPadding.top;
                    rcCtrl.right = iPosX + m_szItem.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + m_szItem.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else
                {
                    rcCtrl.left = iPosX + m_szItem.cx - sz.cx + rcPadding.left;
                    rcCtrl.top = iPosY + rcPadding.top;
                    rcCtrl.right = iPosX + m_szItem.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + sz.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
            }
            else
            {
                if (iChildVAlign == DT_VCENTER)
                {
                    rcCtrl.left = iPosX + rcPadding.left;
                    rcCtrl.top = iPosY + (m_szItem.cy - sz.cy) / 2 + rcPadding.top;
                    rcCtrl.right = iPosX + sz.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + (m_szItem.cy - sz.cy) / 2 + sz.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else if (iChildVAlign == DT_BOTTOM)
                {
                    rcCtrl.left = iPosX + rcPadding.left;
                    rcCtrl.top = iPosY + szTmp.cy - sz.cy + rcPadding.top;
                    rcCtrl.right = iPosX + sz.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + szTmp.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
                else
                {
                    rcCtrl.left = iPosX + rcPadding.left;
                    rcCtrl.top = iPosY + rcPadding.top;
                    rcCtrl.right = iPosX + sz.cx - rcPadding.right;
                    rcCtrl.bottom = iPosY + sz.cy - rcPadding.bottom;
                    pControl->SetPos(rcCtrl, false);
                }
            }
        }

    }

    // Process the scrollbar
    ProcessScrollBar(rc, cxNeeded, cyNeeded);



    //  // Position the elements
    //  if( m_szItem.cx > 0 ) {
    //      m_nColumns = (rc.right - rc.left) / m_szItem.cx;
    //      if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
    //          m_nColumns = (rc.right - rc.left + m_pHorizontalScrollBar->GetScrollRange() ) / m_szItem.cx;
    //  }

    //  if( m_nColumns == 0 ) m_nColumns = 1;

    //  int cyNeeded = 0;
    //  int cxWidth = (rc.right - rc.left) / m_nColumns;
    //  if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() )
    //      cxWidth = (rc.right - rc.left + m_pHorizontalScrollBar->GetScrollRange() ) / m_nColumns;

    //  int cyHeight = 0;
    //  int iCount = 0;
    //  POINT ptTile = { rc.left, rc.top };
    //  if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) {
    //      ptTile.y -= m_pVerticalScrollBar->GetScrollPos();
    //  }
    //  int iPosX = rc.left;
    //  if( m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible() ) {
    //      iPosX -= m_pHorizontalScrollBar->GetScrollPos();
    //      ptTile.x = iPosX;
    //  }
    //  for( int it1 = 0; it1 < m_items.GetSize(); it1++ ) {
    //      CControlUI* pControl = static_cast<CControlUI*>(m_items[it1]);
    //      if( !pControl->IsVisible() ) continue;
    //      if( pControl->IsFloat() ) {
    //          SetFloatPos(it1);
    //          continue;
    //      }

    //      // Determine size
    //      RECT rcTile = { ptTile.x, ptTile.y, ptTile.x + cxWidth, ptTile.y };
    //      if( (iCount % m_nColumns) == 0 )
    //      {
    //          int iIndex = iCount;
    //          for( int it2 = it1; it2 < m_items.GetSize(); it2++ ) {
    //              CControlUI* pLineControl = static_cast<CControlUI*>(m_items[it2]);
    //              if( !pLineControl->IsVisible() ) continue;
    //              if( pLineControl->IsFloat() ) continue;

    //              RECT rcPadding = pLineControl->GetPadding();
    //              SIZE szAvailable = { rcTile.right - rcTile.left - rcPadding.left - rcPadding.right, 9999 };

    //              if( szAvailable.cx < pControl->GetMinWidth() ) szAvailable.cx = pControl->GetMinWidth();
    //              if( szAvailable.cx > pControl->GetMaxWidth() ) szAvailable.cx = pControl->GetMaxWidth();

    //              SIZE szTile = pLineControl->EstimateSize(szAvailable);
    //              if( szTile.cx == 0 ) szTile.cx = m_szItem.cx;
    //              if( szTile.cy == 0 ) szTile.cy = m_szItem.cy;
    //              if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
    //              if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
    //              if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
    //              if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();

    //              cyHeight = MAX(cyHeight, szTile.cy + rcPadding.top + rcPadding.bottom);
    //              if( (++iIndex % m_nColumns) == 0) break;
    //          }
    //      }

    //      RECT rcPadding = pControl->GetPadding();

    //      rcTile.left += rcPadding.left;
    //      rcTile.right -= rcPadding.right;

    //      // Set position
    //      rcTile.top = ptTile.y + rcPadding.top;
    //      rcTile.bottom = ptTile.y + cyHeight;

    //      SIZE szAvailable = { rcTile.right - rcTile.left, rcTile.bottom - rcTile.top };
    //      SIZE szTile = pControl->EstimateSize(szAvailable);
    //      if( szTile.cx == 0 ) szTile.cx = m_szItem.cx;
    //      if( szTile.cy == 0 ) szTile.cy = m_szItem.cy;
    //      if( szTile.cx == 0 ) szTile.cx = szAvailable.cx;
    //      if( szTile.cy == 0 ) szTile.cy = szAvailable.cy;
    //      if( szTile.cx < pControl->GetMinWidth() ) szTile.cx = pControl->GetMinWidth();
    //      if( szTile.cx > pControl->GetMaxWidth() ) szTile.cx = pControl->GetMaxWidth();
    //      if( szTile.cy < pControl->GetMinHeight() ) szTile.cy = pControl->GetMinHeight();
    //      if( szTile.cy > pControl->GetMaxHeight() ) szTile.cy = pControl->GetMaxHeight();
    //      RECT rcPos = {(rcTile.left + rcTile.right - szTile.cx) / 2, (rcTile.top + rcTile.bottom - szTile.cy) / 2,
    //          (rcTile.left + rcTile.right - szTile.cx) / 2 + szTile.cx, (rcTile.top + rcTile.bottom - szTile.cy) / 2 + szTile.cy};
    //      pControl->SetPos(rcPos, false);

    //      if( (++iCount % m_nColumns) == 0 ) {
    //          ptTile.x = iPosX;
    //          ptTile.y += cyHeight + iChildPadding;
    //          cyHeight = 0;
    //      }
    //      else {
    //          ptTile.x += cxWidth;
    //      }
    //      cyNeeded = rcTile.bottom - rc.top;
    //      if( m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible() ) cyNeeded += m_pVerticalScrollBar->GetScrollPos();
    //  }

    //  // Process the scrollbar
    //  ProcessScrollBar(rc, 0, cyNeeded);
}
}
