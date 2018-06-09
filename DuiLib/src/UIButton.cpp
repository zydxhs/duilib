﻿#include "stdafx.h"

namespace DuiLib {
CButtonUI::CButtonUI()
    : m_uButtonState(0)
    , m_dwHotTextColor(0)
    , m_dwPushedTextColor(0)
    , m_dwFocusedTextColor(0)
    , m_dwHotBkColor(0)
    , m_uFadeAlphaDelta(0)
    , m_uFadeAlpha(255)
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
    static bool bDblClick = false;

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
                return;
            }
        }
    }

    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK)
    {
        if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())
        {
            SetCapture();
            m_uButtonState |= UISTATE_PUSHED | UISTATE_CAPTURED;
            Invalidate();

            if (event.Type == UIEVENT_DBLCLICK)
            {
                bDblClick = true;
                m_pManager->SendNotify(this, DUI_MSGTYPE_DBLCLICK);
            }
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

        return;
    }

    if (event.Type == UIEVENT_BUTTONUP && IsEnabled())
    {
        ReleaseCapture();

        m_uButtonState &= ~(UISTATE_PUSHED | UISTATE_CAPTURED);
        Invalidate();

        if ((m_uButtonState & UISTATE_CAPTURED) != 0)
        {
            if (bDblClick) { bDblClick = false; }
            else           { if (::PtInRect(&m_rcItem, event.ptMouse)) { Activate(); } }

        }

        return;
    }

    if (event.Type == UIEVENT_CONTEXTMENU)
    {
        if (IsContextMenuUsed() && IsEnabled())
        {
            ReleaseCapture();
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam);
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

        if (GetFadeAlphaDelta() > 0)
        {
            m_pManager->SetTimer(this, FADE_TIMERID, FADE_ELLAPSE);
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

            if (GetFadeAlphaDelta() > 0)
            {
                m_pManager->SetTimer(this, FADE_TIMERID, FADE_ELLAPSE);
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

    if (event.Type == UIEVENT_TIMER  && event.wParam == FADE_TIMERID)
    {
        if ((m_uButtonState & UISTATE_HOT) != 0)
        {
            if (m_uFadeAlpha > m_uFadeAlphaDelta) { m_uFadeAlpha -= m_uFadeAlphaDelta; }
            else
            {
                m_uFadeAlpha = 0;
                m_pManager->KillTimer(this, FADE_TIMERID);
            }
        }
        else
        {
            if (m_uFadeAlpha < 255 - m_uFadeAlphaDelta) { m_uFadeAlpha += m_uFadeAlphaDelta; }
            else
            {
                m_uFadeAlpha = 255;
                m_pManager->KillTimer(this, FADE_TIMERID);
            }
        }

        Invalidate();
        return;
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
    m_dwHotBkColor = dwColor;
}

DWORD CButtonUI::GetHotBkColor() const
{
    return m_dwHotBkColor;
}

void CButtonUI::SetHotTextColor(DWORD dwColor)
{
    m_dwHotTextColor = dwColor;
}

DWORD CButtonUI::GetHotTextColor() const
{
    return m_dwHotTextColor;
}

void CButtonUI::SetPushedTextColor(DWORD dwColor)
{
    m_dwPushedTextColor = dwColor;
}

DWORD CButtonUI::GetPushedTextColor() const
{
    return m_dwPushedTextColor;
}

void CButtonUI::SetFocusedTextColor(DWORD dwColor)
{
    m_dwFocusedTextColor = dwColor;
}

DWORD CButtonUI::GetFocusedTextColor() const
{
    return m_dwFocusedTextColor;
}

LPCTSTR CButtonUI::GetNormalImage()
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

LPCTSTR CButtonUI::GetHotImage()
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

LPCTSTR CButtonUI::GetPushedImage()
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

LPCTSTR CButtonUI::GetFocusedImage()
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

LPCTSTR CButtonUI::GetDisabledImage()
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

LPCTSTR CButtonUI::GetForeImage()
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

LPCTSTR CButtonUI::GetHotForeImage()
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
    m_diNormal.sDrawString = pStrImage;
    DrawImage(NULL, m_diNormal);

    if (m_diNormal.pImageInfo)
    {
        LONG width = m_diNormal.pImageInfo->nX / 5;
        LONG height = m_diNormal.pImageInfo->nY;
        m_diNormal.rcBmpPart = CDuiRect(0, 0, width, height);

        if (m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0)
        {
            m_cxyFixed.cx = width;
            m_cxyFixed.cy = height;
        }
    }

    m_diPushed.Clear();
    m_diPushed.sDrawString = pStrImage;
    DrawImage(NULL, m_diPushed);

    if (m_diPushed.pImageInfo)
    {
        LONG width = m_diPushed.pImageInfo->nX / 5;
        LONG height = m_diPushed.pImageInfo->nY;
        m_diPushed.rcBmpPart = CDuiRect(width, 0, width * 2, height);
    }

    m_diHot.Clear();
    m_diHot.sDrawString = pStrImage;
    DrawImage(NULL, m_diHot);

    if (m_diHot.pImageInfo)
    {
        LONG width = m_diHot.pImageInfo->nX / 5;
        LONG height = m_diHot.pImageInfo->nY;
        m_diHot.rcBmpPart = CDuiRect(width * 2, 0, width * 3, height);
    }

    m_diFocused.Clear();
    m_diFocused.sDrawString = pStrImage;
    DrawImage(NULL, m_diFocused);

    if (m_diFocused.pImageInfo)
    {
        LONG width = m_diFocused.pImageInfo->nX / 5;
        LONG height = m_diFocused.pImageInfo->nY;
        m_diFocused.rcBmpPart = CDuiRect(width * 3, 0, width * 4, height);
    }

    m_diDisabled.Clear();
    m_diDisabled.sDrawString = pStrImage;
    DrawImage(NULL, m_diDisabled);

    if (m_diDisabled.pImageInfo)
    {
        LONG width = m_diDisabled.pImageInfo->nX / 5;
        LONG height = m_diDisabled.pImageInfo->nY;
        m_diDisabled.rcBmpPart = CDuiRect(width * 4, 0, width * 5, height);
    }

    Invalidate();
}

void CButtonUI::SetFadeAlphaDelta(BYTE uDelta)
{
    m_uFadeAlphaDelta = uDelta;
}

BYTE CButtonUI::GetFadeAlphaDelta()
{
    return m_uFadeAlphaDelta;
}

SIZE CButtonUI::EstimateSize(SIZE szAvailable)
{
    //if (m_cxyFixed.cy == 0) { return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8); }
    //return CControlUI::EstimateSize(szAvailable);
    return CLabelUI::EstimateSize(szAvailable);
}

void CButtonUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("normalimage")) == 0) { SetNormalImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("hotimage")) == 0) { SetHotImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("pushedimage")) == 0) { SetPushedImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("focusedimage")) == 0) { SetFocusedImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("disabledimage")) == 0) { SetDisabledImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("foreimage")) == 0) { SetForeImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("hotforeimage")) == 0) { SetHotForeImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("fivestatusimage")) == 0) { SetFiveStatusImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("fadedelta")) == 0) { SetFadeAlphaDelta((BYTE)_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("hotbkcolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotBkColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("hottextcolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotTextColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("pushedtextcolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetPushedTextColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("focusedtextcolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetFocusedTextColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("dragenable")) == 0) { DUITRACE(_T("不支持属性:dragenable")); }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { DUITRACE(_T("不支持属性:drageimage")); }
    else if (_tcscmp(pstrName, _T("dropenable")) == 0) { DUITRACE(_T("不支持属性:dropenable")); }
    else { CLabelUI::SetAttribute(pstrName, pstrValue); }
}

void CButtonUI::PaintBkColor(HDC hDC)
{
    if (0 != (m_uButtonState & UISTATE_HOT) && 0 != m_dwHotBkColor)
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

    int nLinks = 0;
    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;

    DWORD clrColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;

    if (((m_uButtonState & UISTATE_PUSHED) != 0) && (GetPushedTextColor() != 0))
    { clrColor = GetPushedTextColor(); }
    else if (((m_uButtonState & UISTATE_HOT) != 0) && (GetHotTextColor() != 0))
    { clrColor = GetHotTextColor(); }
    else if (((m_uButtonState & UISTATE_FOCUSED) != 0) && (GetFocusedTextColor() != 0))
    { clrColor = GetFocusedTextColor(); }

    if (m_bShowHtml)
        CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, clrColor,
                                    NULL, NULL, nLinks, m_iFont, m_uTextStyle);
    else
        CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, clrColor,
                                m_iFont, m_uTextStyle);
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
        if (!DrawImage(hDC, m_diDisabled)) { DrawNormalBkImg(hDC, m_diNormal); }

        DrawImage(hDC, m_diFore);
    }
    else if ((m_uButtonState & UISTATE_PUSHED) != 0)
    {
        if (!DrawImage(hDC, m_diPushed)) { DrawNormalBkImg(hDC, m_diNormal); }

        DrawImage(hDC, m_diFore);
    }
    else if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        DrawNormalBkImg(hDC, m_diNormal, &m_diHot, true);

        if (!DrawImage(hDC, m_diHotFore)) { DrawImage(hDC, m_diFore); }
    }
    else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
    {
        if (!DrawImage(hDC, m_diFocused)) { DrawNormalBkImg(hDC, m_diNormal); }

        DrawImage(hDC, m_diFore);
    }
    else
    {
        DrawNormalBkImg(hDC, m_diNormal);
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

void CButtonUI::DrawNormalBkImg(HDC hDC, TDrawInfo &diNormal, TDrawInfo *pdiHot, bool bHot)
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
        if (m_uFadeAlpha == 255)
        {
            pdiNormal->uFade = 255;
            DrawImage(hDC, *pdiNormal);
        }
        else
        {
            if (NULL != pdiHot)
            {
                pdiHot->uFade = 255 - m_uFadeAlpha;
                DrawImage(hDC, *pdiHot);
            }

            pdiNormal->uFade = m_uFadeAlpha;
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
