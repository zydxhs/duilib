#include "stdafx.h"

namespace DuiLib {
enum
{
    // 定时器超时时间
    ELLAPSE_FADE = 40,
    ELLAPSE_DISABLE = 500,
};


CButtonUI::CButtonUI()
    : m_uButtonState(0)
    , m_dwHotBkColor(0)
    , m_dwFocusedBkColor(0)
    , m_dwPushedBkColor(0)
    , m_dwDisabledBkColor(0)
    , m_dwHotTextColor(0)
    , m_dwFocusedTextColor(0)
    , m_dwPushedTextColor(0)
    , m_byFadeAlpha(255)
    , m_byFadeAlphaDelta(0)
    , m_byDisableSeconds(0)
    , m_byEllapseSeconds(0)
{
    m_uTextStyle = DT_SINGLELINE | DT_VCENTER | DT_CENTER;
}

LPCTSTR CButtonUI::GetClass() const
{
    return DUI_CTR_BUTTON;
}

LPVOID CButtonUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_BUTTON) == 0) { return static_cast<CButtonUI *>(this); }

    return CLabelUI::GetInterface(pstrName);
}

UINT CButtonUI::GetControlFlags() const
{
    return (IsKeyboardEnabled() ? UIFLAG_TABSTOP : 0) | (IsEnabled() ? UIFLAG_SETCURSOR : 0);
}

void CButtonUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }
        else { CLabelUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_SETFOCUS)
    {
        Invalidate();
    }

    if (event.Type == UIEVENT_KILLFOCUS)
    {
        Invalidate();
    }

    if (event.Type == UIEVENT_KEYDOWN)
    {
        if (IsKeyboardEnabled() && IsEnabled())
        {
            if (event.chKey == VK_SPACE || event.chKey == VK_RETURN)
            {
                Activate();

                // 2018-07-15 如果启用了响应频率控制，并且处理于可用状态
                // 有可能用户响应事件后，设置按钮为禁用，此时就不能启动该特性，避免超时后置按钮为可用状态
                if (0 != m_byDisableSeconds && IsEnabled() && NULL != m_pManager)
                {
                    m_byEllapseSeconds = 0;
                    m_pManager->SetTimer(this, TIMERID_DISABLE, ELLAPSE_DISABLE);
                    SetEnabled(false);
                }

                return;
            }
        }
    }

    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_LBUTTONDBLDOWN)
    {
        if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())
        {
            SetCapture();
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
            Invalidate();
        }

        return;
    }

    if (event.Type == UIEVENT_MOUSEMOVE)
    {
        if ((m_uButtonState & UISTATE_CAPTURED) != 0)
        {
            if (::PtInRect(&m_rcItem, event.ptMouse)) { m_uButtonState |= UISTATE_PUSHED; }
            else { m_uButtonState &= ~UISTATE_PUSHED; }

            Invalidate();
        }

        if ((event.wParam & MK_LBUTTON) && m_bDragEnable)
        {
            OnDoDragDrop(event);
        }

        return;
    }

    if (event.Type == UIEVENT_BUTTONUP && IsEnabled())
    {
        ReleaseCapture();

        if ((m_uButtonState & UISTATE_CAPTURED) != 0)
        {
            m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
            Invalidate();
        }

        return;
    }

    if ((event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_CLICK) && IsEnabled())
    {
        if (NULL != m_pManager && ::PtInRect(&m_rcItem, event.ptMouse))
        {
            // // 2018-08-18 zhuyadong 添加特效。单击特效
            if (event.Type == UIEVENT_CLICK)         { StartEffect(TRIGGER_CLICK); Activate(); }
            else if (event.Type == UIEVENT_DBLCLICK) { m_pManager->SendNotify(this, DUI_MSGTYPE_DBLCLICK); }

            // 2018-07-15 如果启用了响应频率控制，并且处于可用状态
            // 有可能用户响应事件后，设置按钮为禁用，此时就不能启动该特性，避免超时后置按钮为可用状态
            if (0 != m_byDisableSeconds && IsEnabled())
            {
                m_byEllapseSeconds = 0;
                m_pManager->SetTimer(this, TIMERID_DISABLE, ELLAPSE_DISABLE);
                m_sText.Format(_T("%s %d ..."), m_sTextOrig.GetData(), m_byDisableSeconds);
                SetEnabled(false);
            }

            return;
        }
    }

    if (event.Type == UIEVENT_CONTEXTMENU)
    {
        if (NULL != m_pManager && IsContextMenuUsed() && IsEnabled())
        {
            ReleaseCapture();
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam, true);
        }
        // 2017-02-25 zhuyadong 消息传递给父控件，用于复杂组合控件的上下文菜单响应。
        else if (m_pParent != NULL) { m_pParent->DoEvent(event); }

        return;
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

        if (NULL != m_pManager && GetFadeAlphaDelta() > 0)
        {
            m_pManager->SetTimer(this, TIMERID_FADE, ELLAPSE_FADE);
        }
    }

    if (event.Type == UIEVENT_MOUSELEAVE && NULL != m_pManager)
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

            if (GetFadeAlphaDelta() > 0)
            {
                m_pManager->SetTimer(this, TIMERID_FADE, ELLAPSE_FADE);
            }
        }
        else
        {
            if (m_pManager) { m_pManager->AddMouseLeaveNeeded(this); }

            return;
        }
    }

    if (event.Type == UIEVENT_SETCURSOR)
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_HAND)));
        return;
    }

    if (event.Type == UIEVENT_TIMER && NULL != m_pManager)
    {
        if (event.wParam == TIMERID_FADE)
        {
            if ((m_uButtonState & UISTATE_HOT) != 0)
            {
                if (m_byFadeAlpha > m_byFadeAlphaDelta) { m_byFadeAlpha -= m_byFadeAlphaDelta; }
                else
                {
                    m_byFadeAlpha = 0;
                    m_pManager->KillTimer(this, TIMERID_FADE);
                }
            }
            else
            {
                if (m_byFadeAlpha < 255 - m_byFadeAlphaDelta) { m_byFadeAlpha += m_byFadeAlphaDelta; }
                else
                {
                    m_byFadeAlpha = 255;
                    m_pManager->KillTimer(this, TIMERID_FADE);
                }
            }

            Invalidate();
            return;
        }
        else if (event.wParam == TIMERID_DISABLE)
        {
            m_byEllapseSeconds += 1;
            BYTE byEllapseSec = m_byEllapseSeconds / 2;

            if (byEllapseSec == m_byDisableSeconds)
            {
                m_pManager->KillTimer(this, TIMERID_DISABLE);
                m_sText = m_sTextOrig;
                SetEnabled(true);
            }
            else
            {
                m_sText.Format(_T("%s %d ..."), m_sTextOrig.GetData(), m_byDisableSeconds - byEllapseSec);
                Invalidate();
            }

            return;
        }
    }

    CLabelUI::DoEvent(event);
}

bool CButtonUI::Activate()
{
    if (!CControlUI::Activate()) { return false; }

    if (m_pManager != NULL) { m_pManager->SendNotify(this, DUI_MSGTYPE_CLICK); }

    return true;
}

void CButtonUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

void CButtonUI::SetHotBkColor(DWORD dwColor)
{
    if (m_dwHotBkColor == dwColor) { return; }

    m_dwHotBkColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD CButtonUI::GetHotBkColor() const
{
    return m_dwHotBkColor;
}

void CButtonUI::SetFocusedBkColor(DWORD dwColor)
{
    if (m_dwFocusedBkColor == dwColor) { return; }

    m_dwFocusedBkColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD CButtonUI::GetFocusedBkColor() const
{
    return m_dwFocusedBkColor;
}

void CButtonUI::SetPushedBkColor(DWORD dwColor)
{
    if (m_dwPushedBkColor == dwColor) { return; }

    m_dwPushedBkColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD CButtonUI::GetPushedBkColor() const
{
    return m_dwPushedBkColor;
}

void CButtonUI::SetDisabledBkColor(DWORD dwColor)
{
    if (m_dwDisabledBkColor == dwColor) { return; }

    m_dwDisabledBkColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD CButtonUI::GetDisabledBkColor() const
{
    return m_dwDisabledBkColor;
}

void CButtonUI::SetHotTextColor(DWORD dwColor)
{
    if (m_dwHotTextColor == dwColor) { return; }

    m_dwHotTextColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD CButtonUI::GetHotTextColor() const
{
    return m_dwHotTextColor;
}

void CButtonUI::SetFocusedTextColor(DWORD dwColor)
{
    if (m_dwFocusedTextColor == dwColor) { return; }

    m_dwFocusedTextColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD CButtonUI::GetFocusedTextColor() const
{
    return m_dwFocusedTextColor;
}

void CButtonUI::SetPushedTextColor(DWORD dwColor)
{
    if (m_dwPushedBkColor == dwColor) { return; }

    m_dwPushedTextColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD CButtonUI::GetPushedTextColor() const
{
    return m_dwPushedTextColor;
}

DUI_INLINE LPCTSTR CButtonUI::GetNormalImage()
{
    return m_diNormal.sDrawString;
}

void CButtonUI::SetNormalImage(LPCTSTR pStrImage)
{
    if (m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL) { return; }

    m_diNormal.Clear();
    m_diNormal.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR CButtonUI::GetHotImage()
{
    return m_diHot.sDrawString;
}

void CButtonUI::SetHotImage(LPCTSTR pStrImage)
{
    if (m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL) { return; }

    m_diHot.Clear();
    m_diHot.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR CButtonUI::GetPushedImage()
{
    return m_diPushed.sDrawString;
}

void CButtonUI::SetPushedImage(LPCTSTR pStrImage)
{
    if (m_diPushed.sDrawString == pStrImage && m_diPushed.pImageInfo != NULL) { return; }

    m_diPushed.Clear();
    m_diPushed.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR CButtonUI::GetFocusedImage()
{
    return m_diFocused.sDrawString;
}

void CButtonUI::SetFocusedImage(LPCTSTR pStrImage)
{
    if (m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL) { return; }

    m_diFocused.Clear();
    m_diFocused.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR CButtonUI::GetDisabledImage()
{
    return m_diDisabled.sDrawString;
}

void CButtonUI::SetDisabledImage(LPCTSTR pStrImage)
{
    if (m_diDisabled.sDrawString == pStrImage && m_diDisabled.pImageInfo != NULL) { return; }

    m_diDisabled.Clear();
    m_diDisabled.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR CButtonUI::GetForeImage()
{
    return m_diFore.sDrawString;
}

void CButtonUI::SetForeImage(LPCTSTR pStrImage)
{
    if (m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL) { return; }

    m_diFore.Clear();
    m_diFore.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR CButtonUI::GetHotForeImage()
{
    return m_diHotFore.sDrawString;
}

void CButtonUI::SetHotForeImage(LPCTSTR pStrImage)
{
    if (m_diHotFore.sDrawString == pStrImage && m_diHotFore.pImageInfo != NULL) { return; }

    m_diHotFore.Clear();
    m_diHotFore.sDrawString = pStrImage;
    Invalidate();
}

void CButtonUI::SetFiveStatusImage(LPCTSTR pStrImage)
{
    m_diNormal.Clear();
    m_diPushed.Clear();
    m_diHot.Clear();
    m_diFocused.Clear();
    m_diDisabled.Clear();
    m_diNormal.sDrawString = pStrImage;
    int nPos = m_diNormal.sDrawString.Find(_T(','));
    // 'H' 水平排列，'V' 垂直排列。默认 'H'
    TCHAR ch = _T('H');

    if (-1 != nPos)
    {
        ch = m_diNormal.sDrawString[0];
        m_diNormal.sDrawString = m_diNormal.sDrawString.Right(m_diNormal.sDrawString.GetLength() - 2);
        ch = (_T('V') == ch || _T('v') == ch) ? _T('V') : _T('H');
    }

    m_diPushed.sDrawString = pStrImage;
    m_diHot.sDrawString = pStrImage;
    m_diFocused.sDrawString = pStrImage;
    m_diDisabled.sDrawString = pStrImage;

    // Load 图片
    CDuiString sResType;
    DWORD dwMask            = 0;
    bool bHSL               = false;
    CRenderEngine::ParseDrawInfo(m_diNormal, sResType, dwMask, bHSL);
    m_diNormal.pImageInfo   = m_pManager->AddImage((LPCTSTR)m_diNormal.sImageName,
                                                   (LPCTSTR)sResType, dwMask, bHSL, true);
    m_diPushed.pImageInfo   = m_diNormal.pImageInfo;
    m_diHot.pImageInfo      = m_diNormal.pImageInfo;
    m_diFocused.pImageInfo  = m_diNormal.pImageInfo;
    m_diDisabled.pImageInfo = m_diNormal.pImageInfo;

    if (!m_diNormal.pImageInfo) { return; }

    // 分割图片
    if (_T('H') == ch)
    {
        LONG nW                = m_diNormal.pImageInfo->nX / 5;
        LONG nH                = m_diNormal.pImageInfo->nY;
        m_diNormal.rcBmpPart   = CDuiRect(nW * 0, 0, nW * 1, nH);
        m_diPushed.rcBmpPart   = CDuiRect(nW * 1, 0, nW * 2, nH);
        m_diHot.rcBmpPart      = CDuiRect(nW * 2, 0, nW * 3, nH);
        m_diFocused.rcBmpPart  = CDuiRect(nW * 3, 0, nW * 4, nH);
        m_diDisabled.rcBmpPart = CDuiRect(nW * 4, 0, nW * 5, nH);
    }
    else
    {
        LONG nW                 = m_diNormal.pImageInfo->nX;
        LONG nH                 = m_diNormal.pImageInfo->nY / 5;
        m_diNormal.rcBmpPart    = CDuiRect(0, nH * 0, nW, nH * 1);
        m_diPushed.rcBmpPart    = CDuiRect(0, nH * 1, nW, nH * 2);
        m_diHot.rcBmpPart       = CDuiRect(0, nH * 2, nW, nH * 3);
        m_diFocused.rcBmpPart   = CDuiRect(0, nH * 3, nW, nH * 4);
        m_diDisabled.rcBmpPart  = CDuiRect(0, nH * 4, nW, nH * 5);
    }

    if (m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0)
    {
        m_cxyFixed.cx = m_diNormal.rcBmpPart.right;
        m_cxyFixed.cy = m_diNormal.rcBmpPart.bottom;
    }

    Invalidate();
}

DUI_INLINE void CButtonUI::SetFadeAlphaDelta(BYTE uDelta)
{
    m_byFadeAlphaDelta = uDelta;
}

DUI_INLINE BYTE CButtonUI::GetFadeAlphaDelta()
{
    return m_byFadeAlphaDelta;
}

DUI_INLINE void CButtonUI::SetDisabledSeconds(BYTE bySecs)
{
    m_byDisableSeconds = (bySecs <= 100) ? bySecs : 100;
}

DUI_INLINE BYTE CButtonUI::GetDisabledSeconds()
{
    return m_byDisableSeconds;
}

SIZE CButtonUI::EstimateSize(SIZE szAvailable)
{
    //if (m_cxyFixed.cy == 0) { return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8); }
    //return CControlUI::EstimateSize(szAvailable);
    return CLabelUI::EstimateSize(szAvailable);
}

void CButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("normalimage")) == 0) { SetNormalImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("hotimage")) == 0) { SetHotImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("pushedimage")) == 0) { SetPushedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("focusedimage")) == 0) { SetFocusedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("disabledimage")) == 0) { SetDisabledImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("foreimage")) == 0) { SetForeImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("hotforeimage")) == 0) { SetHotForeImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("fivestatusimage")) == 0) { SetFiveStatusImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("fadedelta")) == 0) { SetFadeAlphaDelta(ParseByte(pstrValue)); }
    else if (_tcscmp(pstrName, _T("hotbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetHotBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("focusedbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetFocusedBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("pushedbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetPushedBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("disabledbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetDisabledBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("hottextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetHotTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("focusedtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetFocusedTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("pushedtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetPushedTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("disableseconds")) == 0)
    {
        SetDisabledSeconds(ParseByte(pstrValue));
    }
    else { CLabelUI::SetAttribute(pstrName, pstrValue); }
}

void CButtonUI::PaintBkColor(HDC hDC)
{
    if (!IsEnabled() && 0 != m_dwDisabledBkColor)
    {
        CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwDisabledBkColor));
    }
    else if (0 != (m_uButtonState & UISTATE_PUSHED) && 0 != m_dwPushedBkColor)
    {
        CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwPushedBkColor));
    }
    else if (IsFocused() && 0 != m_dwFocusedBkColor)
    {
        CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwFocusedBkColor));
    }
    else if (0 != (m_uButtonState & UISTATE_HOT) && 0 != m_dwHotBkColor)
    {
        CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
    }
    else { CLabelUI::PaintBkColor(hDC); }
}

void CButtonUI::PaintText(HDC hDC)
{
    if (IsFocused()) { m_uButtonState |= UISTATE_FOCUSED; }
    else { m_uButtonState &= ~ UISTATE_FOCUSED; }

    if (!IsEnabled()) { m_uButtonState |= UISTATE_DISABLED; }
    else { m_uButtonState &= ~ UISTATE_DISABLED; }

    if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }

    if (m_dwDisabledTextColor == 0) { m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor(); }

    if (m_sText.IsEmpty()) { return; }

    RECT rc = m_rcItem;
    rc.left += (m_rcBorderSize.left + m_rcPadding.left);
    rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
    rc.top += (m_rcBorderSize.top + m_rcPadding.top);
    rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

    if (GetEnabledEffect())
    {
        PaintTextEffect(hDC, rc);
    }
    else
    {
        DWORD clrColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;

        if (((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0))
        {
            clrColor = GetPushedTextColor();
        }
        else if (((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0))
        {
            clrColor = GetHotTextColor();
        }
        else if (((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0))
        {
            clrColor = GetFocusedTextColor();
        }

        if (m_bShowHtml)
            CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, clrColor,
                                        NULL, NULL, NULL, m_iFont, m_uTextStyle);
        else
            CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, clrColor,
                                    m_iFont, m_uTextStyle);
    }
}

void CButtonUI::PaintStatusImage(HDC hDC)
{
    if (IsFocused()) { m_uButtonState |= UISTATE_FOCUSED; }
    else { m_uButtonState &= ~ UISTATE_FOCUSED; }

    if (!IsEnabled()) { m_uButtonState |= UISTATE_DISABLED; }
    else { m_uButtonState &= ~ UISTATE_DISABLED; }

    // 绘制顺序：5态 背景图->前景图
    if ((m_uButtonState & UISTATE_DISABLED) != 0)
    {
        if (!DrawImage(hDC, m_diDisabled)) { DrawNormalBkImage(hDC, m_diNormal); }

        DrawImage(hDC, m_diFore);
    }
    else if ((m_uButtonState & UISTATE_PUSHED) != 0)
    {
        if (!DrawImage(hDC, m_diPushed)) { DrawNormalBkImage(hDC, m_diNormal); }

        DrawImage(hDC, m_diFore);
    }
    else if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        DrawNormalBkImage(hDC, m_diNormal, &m_diHot, true);

        if (!DrawImage(hDC, m_diHotFore)) { DrawImage(hDC, m_diFore); }
    }
    else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
    {
        if (!DrawImage(hDC, m_diFocused)) { DrawNormalBkImage(hDC, m_diNormal); }

        DrawImage(hDC, m_diFore);
    }
    else
    {
        DrawNormalBkImage(hDC, m_diNormal);
        DrawImage(hDC, m_diFore);
    }

    //if ((m_uButtonState & UISTATE_DISABLED) != 0)
    //{
    //    if (DrawImage(hDC, m_diDisabled)) { goto Label_ForeImage; }
    //}
    //else if ((m_uButtonState & UISTATE_PUSHED) != 0)
    //{
    //    if (!DrawImage(hDC, m_diPushed))
    //    { DrawImage(hDC, m_diNormal); }
    //
    //    if (DrawImage(hDC, m_diPushedFore)) { return; }
    //    else { goto Label_ForeImage; }
    //}
    //else if ((m_uButtonState & UISTATE_HOT) != 0)
    //{
    //    if (GetFadeAlphaDelta() > 0)
    //    {
    //        if (m_uFadeAlpha == 0)
    //        {
    //            m_diHot.uFade = 255;
    //            DrawImage(hDC, m_diHot);
    //        }
    //        else
    //        {
    //            m_diNormal.uFade = m_uFadeAlpha;
    //            DrawImage(hDC, m_diNormal);
    //            m_diHot.uFade = 255 - m_uFadeAlpha;
    //            DrawImage(hDC, m_diHot);
    //        }
    //    }
    //    else
    //    {
    //        if (!DrawImage(hDC, m_diHot))
    //        { DrawImage(hDC, m_diNormal); }
    //    }
    //
    //    if (DrawImage(hDC, m_diHotFore)) { return; }
    //    else if (m_dwHotBkColor != 0)
    //    {
    //        CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwHotBkColor));
    //        return;
    //    }
    //    else { goto Label_ForeImage; }
    //}
    //else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
    //{
    //    if (DrawImage(hDC, m_diFocused)) { goto Label_ForeImage; };
    //}
    //
    //if (GetFadeAlphaDelta() > 0)
    //{
    //    if (m_uFadeAlpha == 255)
    //    {
    //        m_diNormal.uFade = 255;
    //        DrawImage(hDC, m_diNormal);
    //    }
    //    else
    //    {
    //        m_diHot.uFade = 255 - m_uFadeAlpha;
    //        DrawImage(hDC, m_diHot);
    //        m_diNormal.uFade = m_uFadeAlpha;
    //        DrawImage(hDC, m_diNormal);
    //    }
    //}
    //else
    //{
    //    DrawImage(hDC, m_diNormal);
    //}
    //Label_ForeImage:
    //DrawImage(hDC, m_diFore);
}

void CButtonUI::DrawNormalBkImage(HDC hDC, TDrawInfo &diNormal, TDrawInfo *pdiHot, bool bHot)
{
    // bHot 为 false 时，pdiHot 可以为NULL
    // bHot 为 true  时，pdiHot 不能为NULL
    // 因此 diNormal/pdiNormal 不会为NULL，pdiHot 可能为NULL
    if (bHot && NULL == pdiHot) { return; }

    TDrawInfo *pdiNormal = &diNormal;

    if (bHot)
    {
        pdiNormal = pdiHot;
        pdiHot = &diNormal;
    }

    if (GetFadeAlphaDelta() > 0)
    {
        if (m_byFadeAlpha == 255)
        {
            pdiNormal->byFade = 255;
            DrawImage(hDC, *pdiNormal);
        }
        else
        {
            if (NULL != pdiHot)
            {
                pdiHot->byFade = 255 - m_byFadeAlpha;
                DrawImage(hDC, *pdiHot);
            }

            pdiNormal->byFade = m_byFadeAlpha;
            DrawImage(hDC, *pdiNormal);
        }
    }
    else
    {
        if (!DrawImage(hDC, *pdiNormal) && bHot)
        {
            DrawImage(hDC, *pdiHot);
        }
    }
}

}
