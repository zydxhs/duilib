#include "StdAfx.h"

namespace DuiLib {
CSliderUI::CSliderUI() : m_nStep(1), m_bImmMode(false)
{
    m_uTextStyle = DT_SINGLELINE | DT_CENTER;
    m_szThumb.cx = m_szThumb.cy = 10;
}

LPCTSTR CSliderUI::GetClass() const
{
    return DUI_CTR_SLIDER;
}

UINT CSliderUI::GetControlFlags() const
{
    if (IsEnabled()) { return UIFLAG_SETCURSOR | UIFLAG_TABSTOP; }
    else { return 0; }
}

LPVOID CSliderUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_SLIDER) == 0) { return static_cast<CSliderUI *>(this); }

    return CProgressUI::GetInterface(pstrName);
}

void CSliderUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

int CSliderUI::GetChangeStep()
{
    return m_nStep;
}

void CSliderUI::SetChangeStep(int step)
{
    m_nStep = step;
}

void CSliderUI::SetThumbSize(SIZE szXY)
{
    m_szThumb = szXY;
}

RECT CSliderUI::GetThumbRect() const
{
    if (m_bHorizontal)
    {
        int left = m_rcItem.left + (m_rcItem.right - m_rcItem.left - m_szThumb.cx) * (m_nValue - m_nMin) /
                   (m_nMax - m_nMin);
        int top = (m_rcItem.bottom + m_rcItem.top - m_szThumb.cy) / 2;
        return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy);
    }
    else
    {
        int left = (m_rcItem.right + m_rcItem.left - m_szThumb.cx) / 2;
        int top = m_rcItem.bottom - m_szThumb.cy - (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy) *
                  (m_nValue - m_nMin) / (m_nMax - m_nMin);
        return CDuiRect(left, top, left + m_szThumb.cx, top + m_szThumb.cy);
    }
}

bool CSliderUI::IsImmMode() const
{
    return m_bImmMode;
}

void CSliderUI::SetImmMode(bool bImmMode)
{
    m_bImmMode = bImmMode;
}

LPCTSTR CSliderUI::GetThumbImage() const
{
    return m_diThumb.sDrawString;
}

void CSliderUI::SetThumbImage(LPCTSTR pStrImage)
{
    if (m_diThumb.sDrawString == pStrImage && m_diThumb.pImageInfo != NULL) { return; }

    m_diThumb.Clear();
    m_diThumb.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CSliderUI::GetThumbHotImage() const
{
    return m_diThumbHot.sDrawString;
}

void CSliderUI::SetThumbHotImage(LPCTSTR pStrImage)
{
    if (m_diThumbHot.sDrawString == pStrImage && m_diThumbHot.pImageInfo != NULL) { return; }

    m_diThumbHot.Clear();
    m_diThumbHot.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CSliderUI::GetThumbPushedImage() const
{
    return m_diThumbPushed.sDrawString;
}

void CSliderUI::SetThumbPushedImage(LPCTSTR pStrImage)
{
    if (m_diThumbPushed.sDrawString == pStrImage && m_diThumbPushed.pImageInfo != NULL) { return; }

    m_diThumbPushed.Clear();
    m_diThumbPushed.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CSliderUI::GetThumbDisabledImage() const
{
    return m_diThumbDisabled.sDrawString;
}

void CSliderUI::SetThumbDisabledImage(LPCTSTR pStrImage)
{
    if (m_diThumbDisabled.sDrawString == pStrImage && m_diThumbDisabled.pImageInfo != NULL) { return; }

    m_diThumbDisabled.Clear();
    m_diThumbDisabled.sDrawString = pStrImage;
    Invalidate();
}

void CSliderUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }
        else { CProgressUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_LBUTTONDBLDOWN)
    {
        if (IsEnabled())
        {
            /*RECT rcThumb = GetThumbRect();
            if( ::PtInRect(&rcThumb, event.ptMouse) ) {
            m_uButtonState |= UISTATE_CAPTURED;
            }
            }
            return;*/
            SetCapture();
            m_uButtonState |= UISTATE_CAPTURED;

            int nValue;

            if (m_bHorizontal)
            {
                if (event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2) { nValue = m_nMax; }
                else if (event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2) { nValue = m_nMin; }
                else { nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx); }
            }
            else
            {
                if (event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2) { nValue = m_nMin; }
                else if (event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2) { nValue = m_nMax; }
                else { nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy); }
            }

            if (m_nValue != nValue && nValue >= m_nMin && nValue <= m_nMax)
            {
                m_nValue = nValue;

                if (NULL != m_pManager) { m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGING); }

                Invalidate();
            }
        }

        return;
    }

    if (event.Type == UIEVENT_BUTTONUP && IsEnabled())
    {
        ReleaseCapture();

        if ((m_uButtonState & UISTATE_CAPTURED) != 0)
        {
            if (m_bHorizontal)
            {
                if (event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2) { m_nValue = m_nMax; }
                else if (event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2) { m_nValue = m_nMin; }
                else { m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx); }
            }
            else
            {
                if (event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2) { m_nValue = m_nMin; }
                else if (event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2) { m_nValue = m_nMax; }
                else { m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy); }
            }

            if (NULL != m_pManager) { m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED); }

            m_uButtonState &= ~UISTATE_CAPTURED;
            Invalidate();
        }

        return;
    }

    if (event.Type == UIEVENT_CONTEXTMENU)
    {
        return;
    }

    if (event.Type == UIEVENT_SCROLLWHEEL && IsEnabled() && NULL != m_pManager && IsFocused())
    {
        switch (LOWORD(event.wParam))
        {
        case SB_LINEUP:
            SetValue(GetValue() + GetChangeStep());
            // 2018-08-17 zhuyadong 滚轮事件，立即发送 CHANGING 消息，延时发送 CHANGED 消息
            //m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
            m_pManager->SetTimer(this, TIMERID_DELAY_NTY, 300);
            m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGING);
            return;

        case SB_LINEDOWN:
            SetValue(GetValue() - GetChangeStep());
            // 2018-08-17 zhuyadong 滚轮事件，立即发送 CHANGING 消息，延时发送 CHANGED 消息
            //m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
            m_pManager->SetTimer(this, TIMERID_DELAY_NTY, 300);
            m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGING);
            return;
        }
    }

    if (event.Type == UIEVENT_MOUSEMOVE)
    {
        if ((m_uButtonState & UISTATE_CAPTURED) != 0)
        {
            if (m_bHorizontal)
            {
                if (event.ptMouse.x >= m_rcItem.right - m_szThumb.cx / 2) { m_nValue = m_nMax; }
                else if (event.ptMouse.x <= m_rcItem.left + m_szThumb.cx / 2) { m_nValue = m_nMin; }
                else { m_nValue = m_nMin + (m_nMax - m_nMin) * (event.ptMouse.x - m_rcItem.left - m_szThumb.cx / 2) / (m_rcItem.right - m_rcItem.left - m_szThumb.cx); }
            }
            else
            {
                if (event.ptMouse.y >= m_rcItem.bottom - m_szThumb.cy / 2) { m_nValue = m_nMin; }
                else if (event.ptMouse.y <= m_rcItem.top + m_szThumb.cy / 2) { m_nValue = m_nMax; }
                else { m_nValue = m_nMin + (m_nMax - m_nMin) * (m_rcItem.bottom - event.ptMouse.y - m_szThumb.cy / 2) / (m_rcItem.bottom - m_rcItem.top - m_szThumb.cy); }
            }

            if (m_bImmMode && NULL != m_pManager) { m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGING); }

            Invalidate();
        }

        return;
    }

    if (event.Type == UIEVENT_SETCURSOR)
    {
        RECT rcThumb = GetThumbRect();

        if (IsEnabled() && ::PtInRect(&rcThumb, event.ptMouse))
        {
            ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
            return;
        }
    }

    if (event.Type == UIEVENT_MOUSEENTER)
    {
        if (::PtInRect(&m_rcItem, event.ptMouse))
        {
            if (IsEnabled())
            {
                if ((m_uButtonState & UISTATE_HOT) == 0)
                {
                    m_uButtonState |= UISTATE_HOT;
                    Invalidate();
                }
            }
        }
    }

    if (event.Type == UIEVENT_MOUSELEAVE)
    {
        if (!::PtInRect(&m_rcItem, event.ptMouse))
        {
            if (IsEnabled())
            {
                if ((m_uButtonState & UISTATE_HOT) != 0)
                {
                    m_uButtonState &= ~UISTATE_HOT;
                    Invalidate();
                }
            }

            if (m_pManager) { m_pManager->RemoveMouseLeaveNeeded(this); }
        }
        else
        {
            if (m_pManager) { m_pManager->AddMouseLeaveNeeded(this); }

            return;
        }
    }

    if (event.Type == UIEVENT_TIMER)
    {
        if (event.wParam == TIMERID_DELAY_NTY && NULL != m_pManager)
        {
            // 2018-08-17 zhuyadong 滚轮事件，延时发送 CHANGED 消息
            m_pManager->KillTimer(this, TIMERID_DELAY_NTY);
            m_pManager->SendNotify(this, DUI_MSGTYPE_VALUECHANGED);
            return;
        }
    }

    CControlUI::DoEvent(event);
}


void CSliderUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("thumbimage")) == 0) { SetThumbImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("thumbhotimage")) == 0) { SetThumbHotImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("thumbpushedimage")) == 0) { SetThumbPushedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("thumbdisabledimage")) == 0) { SetThumbDisabledImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("thumbsize")) == 0)
    {
        SIZE sz = ParseSize(pstrValue);
        SetThumbSize(sz);
    }
    else if (_tcscmp(pstrName, _T("step")) == 0)
    {
        SetChangeStep(ParseInt(pstrValue));
    }
    else if (_tcscmp(pstrName, _T("imm")) == 0) { SetImmMode(ParseBool(pstrValue)); }
    else { CProgressUI::SetAttribute(pstrName, pstrValue); }
}

void CSliderUI::PaintStatusImage(HDC hDC)
{
    CProgressUI::PaintStatusImage(hDC);

    RECT rcThumb = GetThumbRect();
    rcThumb.left -= m_rcItem.left;
    rcThumb.top -= m_rcItem.top;
    rcThumb.right -= m_rcItem.left;
    rcThumb.bottom -= m_rcItem.top;

    if ((m_uButtonState & UISTATE_CAPTURED) != 0)
    {
        m_diThumbPushed.rcDestOffset = rcThumb;

        if (DrawImage(hDC, m_diThumbPushed)) { return; }
    }
    else if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        m_diThumbHot.rcDestOffset = rcThumb;

        if (DrawImage(hDC, m_diThumbHot)) { return; }
    }
    else if ((m_uButtonState & UISTATE_DISABLED) != 0)
    {
        m_diThumbDisabled.rcDestOffset = rcThumb;

        if (DrawImage(hDC, m_diThumbDisabled)) { return; }
    }

    m_diThumb.rcDestOffset = rcThumb;

    if (DrawImage(hDC, m_diThumb)) { return; }
}
}
