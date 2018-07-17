#include "stdafx.h"

namespace DuiLib {
CHorizontalLayoutUI::CHorizontalLayoutUI() : m_iSepWidth(0), m_uButtonState(0), m_bImmMode(false)
{
    m_ptLastMouse.x = m_ptLastMouse.y = 0;
    ::ZeroMemory(&m_rcNewPos, sizeof(m_rcNewPos));
}

LPCTSTR CHorizontalLayoutUI::GetClass() const
{
    return DUI_CTR_HORIZONTALLAYOUT;
}

LPVOID CHorizontalLayoutUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_HORIZONTALLAYOUT) == 0) { return static_cast<CHorizontalLayoutUI *>(this); }

    return CContainerUI::GetInterface(pstrName);
}

UINT CHorizontalLayoutUI::GetControlFlags() const
{
    if (IsEnabled() && m_iSepWidth != 0) { return UIFLAG_SETCURSOR; }
    else { return 0; }
}

void CHorizontalLayoutUI::SetPos(RECT rc, bool bNeedInvalidate)
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

    int cyNeeded = 0;
    int nAdjustables = 0;
    int cxFixed = 0;
    int nEstimateNum = 0;
    SIZE szControlAvailable;
    int iControlMaxWidth = 0;
    int iControlMaxHeight = 0;
    CControlUI *pCtrl = NULL;

    for (int i = 0; i < m_items.GetSize(); i++)
    {
        pCtrl = static_cast<CControlUI *>(m_items[i]);

        if (!pCtrl->IsVisible()) { continue; }

        if (pCtrl->IsFloat()) { continue; }

        szControlAvailable = szAvailable;
        RECT rcMargin = pCtrl->GetMargin();
        szControlAvailable.cy -= rcMargin.top + rcMargin.bottom;
        iControlMaxWidth = pCtrl->GetFixedWidth();
        iControlMaxHeight = pCtrl->GetFixedHeight();

        if (iControlMaxWidth <= 0) { iControlMaxWidth = pCtrl->GetMaxWidth(); }

        if (iControlMaxHeight <= 0) { iControlMaxHeight = pCtrl->GetMaxHeight(); }

        if (szControlAvailable.cx > iControlMaxWidth) { szControlAvailable.cx = iControlMaxWidth; }

        if (szControlAvailable.cy > iControlMaxHeight) { szControlAvailable.cy = iControlMaxHeight; }

        //SIZE sz = { 0 };
        SIZE sz = pCtrl->EstimateSize(szControlAvailable);

        if (pCtrl->GetFixedWidth() == 0)
        {
            nAdjustables++;
            sz.cx = 0;
            sz.cy = pCtrl->GetFixedHeight();
        }
        else
        {
            //sz = pCtrl->EstimateSize(szControlAvailable);

            if (sz.cx == 0)
            {
                nAdjustables++;
            }
            else
            {
                if (sz.cx < pCtrl->GetMinWidth()) { sz.cx = pCtrl->GetMinWidth(); }

                if (sz.cx > pCtrl->GetMaxWidth()) { sz.cx = pCtrl->GetMaxWidth(); }
            }
        }

        cxFixed += sz.cx + pCtrl->GetMargin().left + pCtrl->GetMargin().right;

        sz.cy = std::max<int>(sz.cy, 0);

        if (sz.cy < pCtrl->GetMinHeight()) { sz.cy = pCtrl->GetMinHeight(); }

        if (sz.cy > pCtrl->GetMaxHeight()) { sz.cy = pCtrl->GetMaxHeight(); }

        cyNeeded = std::max<int>(cyNeeded, sz.cy + rcMargin.top + rcMargin.bottom);
        nEstimateNum++;
    }

    cxFixed += (nEstimateNum - 1) * m_iChildMargin;

    // Place elements
    int cxNeeded = 0;
    int cxExpand = 0;

    if (nAdjustables > 0) { cxExpand = std::max<int>(0, (szAvailable.cx - cxFixed) / nAdjustables); }

    // Position the elements
    SIZE szRemaining = szAvailable;
    int iPosX = rc.left;

    if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible())
    {
        iPosX -= m_pHorizontalScrollBar->GetScrollPos();
    }

    int iEstimate = 0;
    int iAdjustable = 0;
    int cxFixedRemaining = cxFixed;

    for (int i = 0; i < m_items.GetSize(); i++)
    {
        pCtrl = static_cast<CControlUI *>(m_items[i]);

        if (!pCtrl->IsVisible()) { continue; }

        if (pCtrl->IsFloat())
        {
            SetFloatPos(i);
            continue;
        }

        iEstimate += 1;
        RECT rcMargin = pCtrl->GetMargin();
        szRemaining.cx -= rcMargin.left;

        szControlAvailable = szRemaining;
        szControlAvailable.cy -= rcMargin.top + rcMargin.bottom;
        iControlMaxWidth = pCtrl->GetFixedWidth();
        iControlMaxHeight = pCtrl->GetFixedHeight();

        if (iControlMaxWidth <= 0) { iControlMaxWidth = pCtrl->GetMaxWidth(); }

        if (iControlMaxHeight <= 0) { iControlMaxHeight = pCtrl->GetMaxHeight(); }

        if (szControlAvailable.cx > iControlMaxWidth) { szControlAvailable.cx = iControlMaxWidth; }

        if (szControlAvailable.cy > iControlMaxHeight) { szControlAvailable.cy = iControlMaxHeight; }

        cxFixedRemaining = cxFixedRemaining - (rcMargin.left + rcMargin.right);

        if (iEstimate > 1) { cxFixedRemaining = cxFixedRemaining - m_iChildMargin; }

        SIZE sz = pCtrl->EstimateSize(szControlAvailable);

        if (pCtrl->GetFixedWidth() == 0 || sz.cx == 0)
        {
            iAdjustable++;
            sz.cx = cxExpand;

            // Distribute remaining to last element (usually round-off left-overs)
            if (iAdjustable == nAdjustables)
            {
                sz.cx = std::max<int>(0, szRemaining.cx - rcMargin.right - cxFixedRemaining);
            }

            if (sz.cx < pCtrl->GetMinWidth()) { sz.cx = pCtrl->GetMinWidth(); }

            if (sz.cx > pCtrl->GetMaxWidth()) { sz.cx = pCtrl->GetMaxWidth(); }
        }
        else
        {
            if (sz.cx < pCtrl->GetMinWidth()) { sz.cx = pCtrl->GetMinWidth(); }

            if (sz.cx > pCtrl->GetMaxWidth()) { sz.cx = pCtrl->GetMaxWidth(); }

            cxFixedRemaining -= sz.cx;
        }

        sz.cy = pCtrl->GetMaxHeight();

        if (sz.cy == 0) { sz.cy = szAvailable.cy - rcMargin.top - rcMargin.bottom; }

        if (sz.cy < 0) { sz.cy = 0; }

        if (sz.cy > szControlAvailable.cy) { sz.cy = szControlAvailable.cy; }

        if (sz.cy < pCtrl->GetMinHeight()) { sz.cy = pCtrl->GetMinHeight(); }

        UINT iChildAlign = GetChildVAlign();

        if (iChildAlign == DT_VCENTER)
        {
            int iPosY = (rc.bottom + rc.top) / 2;

            if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
            {
                iPosY += m_pVerticalScrollBar->GetScrollRange() / 2;
                iPosY -= m_pVerticalScrollBar->GetScrollPos();
            }

            RECT rcCtrl = { iPosX + rcMargin.left, iPosY - sz.cy / 2, iPosX + sz.cx + rcMargin.left, iPosY + sz.cy - sz.cy / 2 };
            pCtrl->SetPos(rcCtrl, false);
        }
        else if (iChildAlign == DT_BOTTOM)
        {
            int iPosY = rc.bottom;

            if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
            {
                iPosY += m_pVerticalScrollBar->GetScrollRange();
                iPosY -= m_pVerticalScrollBar->GetScrollPos();
            }

            RECT rcCtrl = { iPosX + rcMargin.left, iPosY - rcMargin.bottom - sz.cy, iPosX + sz.cx + rcMargin.left, iPosY - rcMargin.bottom };
            pCtrl->SetPos(rcCtrl, false);
        }
        else
        {
            int iPosY = rc.top;

            if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible())
            {
                iPosY -= m_pVerticalScrollBar->GetScrollPos();
            }

            RECT rcCtrl = { iPosX + rcMargin.left, iPosY + rcMargin.top, iPosX + sz.cx + rcMargin.left, iPosY + sz.cy + rcMargin.top };
            pCtrl->SetPos(rcCtrl, false);
        }

        iPosX += sz.cx + m_iChildMargin + rcMargin.left + rcMargin.right;
        cxNeeded += sz.cx + rcMargin.left + rcMargin.right;
        szRemaining.cx -= sz.cx + m_iChildMargin + rcMargin.right;
    }

    cxNeeded += (nEstimateNum - 1) * m_iChildMargin;

    // Process the scrollbar
    ProcessScrollBar(rc, cxNeeded, cyNeeded);
}

void CHorizontalLayoutUI::DoPostPaint(HDC hDC, const RECT &rcPaint)
{
    if ((m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode)
    {
        RECT rcSeparator = GetThumbRect(true);
        CRenderEngine::DrawColor(hDC, rcSeparator, 0xAA000000);
    }
}

SIZE CHorizontalLayoutUI::EstimateSize(SIZE szAvailable)
{
    SIZE sz = CControlUI::EstimateSize(szAvailable);

    if (m_bAutoWidth)
    {
        sz.cx = m_iChildMargin * (GetCount() - 1);

        for (int i = 0; i < GetCount(); ++i)
        {
            SIZE sz2 = GetItemAt(i)->EstimateSize(szAvailable);
            sz.cx += sz2.cx;
        }

        m_cxyFixed.cx = sz.cx;
    }

    return sz;
}

void CHorizontalLayoutUI::SetSepWidth(int iWidth)
{
    m_iSepWidth = iWidth;
}

int CHorizontalLayoutUI::GetSepWidth() const
{
    return m_iSepWidth;
}

void CHorizontalLayoutUI::SetSepImmMode(bool bImmediately)
{
    if (m_bImmMode == bImmediately) { return; }

    if ((m_uButtonState & UISTATE_CAPTURED) != 0 && !m_bImmMode && m_pManager != NULL)
    {
        m_pManager->RemovePostPaint(this);
    }

    m_bImmMode = bImmediately;
}

bool CHorizontalLayoutUI::IsSepImmMode() const
{
    return m_bImmMode;
}

void CHorizontalLayoutUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("sepwidth")) == 0) { SetSepWidth(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("sepimm")) == 0) { SetSepImmMode(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("autoheight")) == 0) { DUITRACE(_T("不支持属性:autoheight")); }
    else { CContainerUI::SetAttribute(pstrName, pstrValue); }
}

void CHorizontalLayoutUI::DoEvent(TEventUI &event)
{
    if (m_iSepWidth != 0)
    {
        if (event.Type == UIEVENT_BUTTONDOWN && IsEnabled())
        {
            SetCapture();
            RECT rcSeparator = GetThumbRect(false);

            if (::PtInRect(&rcSeparator, event.ptMouse))
            {
                m_uButtonState |= UISTATE_CAPTURED;
                m_ptLastMouse = event.ptMouse;
                m_rcNewPos = m_rcItem;

                if (!m_bImmMode && m_pManager) { m_pManager->AddPostPaint(this); }

                return;
            }
        }

        if (event.Type == UIEVENT_BUTTONUP)
        {
            ReleaseCapture();

            if ((m_uButtonState & UISTATE_CAPTURED) != 0)
            {
                m_uButtonState &= ~UISTATE_CAPTURED;
                m_rcItem = m_rcNewPos;

                if (!m_bImmMode && m_pManager) { m_pManager->RemovePostPaint(this); }

                NeedParentUpdate();
                return;
            }
        }

        if (event.Type == UIEVENT_MOUSEMOVE)
        {
            if ((m_uButtonState & UISTATE_CAPTURED) != 0)
            {
                LONG cx = event.ptMouse.x - m_ptLastMouse.x;
                m_ptLastMouse = event.ptMouse;
                RECT rc = m_rcNewPos;

                if (m_iSepWidth >= 0)
                {
                    if (cx > 0 && event.ptMouse.x < m_rcNewPos.right - m_iSepWidth) { return; }

                    if (cx < 0 && event.ptMouse.x > m_rcNewPos.right) { return; }

                    rc.right += cx;

                    if (rc.right - rc.left <= GetMinWidth())
                    {
                        if (m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth()) { return; }

                        rc.right = rc.left + GetMinWidth();
                    }

                    if (rc.right - rc.left >= GetMaxWidth())
                    {
                        if (m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth()) { return; }

                        rc.right = rc.left + GetMaxWidth();
                    }
                }
                else
                {
                    if (cx > 0 && event.ptMouse.x < m_rcNewPos.left) { return; }

                    if (cx < 0 && event.ptMouse.x > m_rcNewPos.left - m_iSepWidth) { return; }

                    rc.left += cx;

                    if (rc.right - rc.left <= GetMinWidth())
                    {
                        if (m_rcNewPos.right - m_rcNewPos.left <= GetMinWidth()) { return; }

                        rc.left = rc.right - GetMinWidth();
                    }

                    if (rc.right - rc.left >= GetMaxWidth())
                    {
                        if (m_rcNewPos.right - m_rcNewPos.left >= GetMaxWidth()) { return; }

                        rc.left = rc.right - GetMaxWidth();
                    }
                }

                CDuiRect rcInvalidate = GetThumbRect(true);
                m_rcNewPos = rc;
                m_cxyFixed.cx = m_rcNewPos.right - m_rcNewPos.left;

                if (m_bImmMode)
                {
                    m_rcItem = m_rcNewPos;
                    NeedParentUpdate();
                }
                else
                {
                    rcInvalidate.Join(GetThumbRect(true));
                    rcInvalidate.Join(GetThumbRect(false));

                    if (m_pManager) { m_pManager->Invalidate(rcInvalidate); }
                }

                return;
            }

            if ((event.wParam & MK_LBUTTON) && m_bDragEnable)
            {
                OnDoDragDrop(event);
                return;
            }
        }

        if (event.Type == UIEVENT_SETCURSOR)
        {
            RECT rcSeparator = GetThumbRect(false);

            if (IsEnabled() && ::PtInRect(&rcSeparator, event.ptMouse))
            {
                ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEWE)));
                return;
            }
        }
    }

    CContainerUI::DoEvent(event);
}

RECT CHorizontalLayoutUI::GetThumbRect(bool bUseNew) const
{
    if ((m_uButtonState & UISTATE_CAPTURED) != 0 && bUseNew)
    {
        if (m_iSepWidth >= 0) { return CDuiRect(m_rcNewPos.right - m_iSepWidth, m_rcNewPos.top, m_rcNewPos.right, m_rcNewPos.bottom); }
        else { return CDuiRect(m_rcNewPos.left, m_rcNewPos.top, m_rcNewPos.left - m_iSepWidth, m_rcNewPos.bottom); }
    }
    else
    {
        if (m_iSepWidth >= 0) { return CDuiRect(m_rcItem.right - m_iSepWidth, m_rcItem.top, m_rcItem.right, m_rcItem.bottom); }
        else { return CDuiRect(m_rcItem.left, m_rcItem.top, m_rcItem.left - m_iSepWidth, m_rcItem.bottom); }
    }
}
}
