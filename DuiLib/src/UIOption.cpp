#include "stdafx.h"
#include <map>

namespace DuiLib {
COptionUI::COptionUI()
    : m_bSelected(false)
    , m_dwSelNormalBkColor(0)
    , m_dwSelHotBkColor(0)
    , m_dwSelFocusedBkColor(0)
    , m_dwSelPushedBkColor(0)
    , m_dwSelDisabledBkColor(0)
    , m_nBindTabIndex(0)
{
}

COptionUI::~COptionUI()
{
    if (!m_sGroupName.IsEmpty() && m_pManager) { m_pManager->RemoveOptionGroup(m_sGroupName, this); }
}

LPCTSTR COptionUI::GetClass() const
{
    return DUI_CTR_OPTION;
}

LPVOID COptionUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_OPTION) == 0) { return static_cast<COptionUI *>(this); }

    return CButtonUI::GetInterface(pstrName);
}

void COptionUI::SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit)
{
    CControlUI::SetManager(pManager, pParent, bInit);

    if (bInit && !m_sGroupName.IsEmpty())
    {
        if (m_pManager) { m_pManager->AddOptionGroup(m_sGroupName, this); }
    }
}

DUI_INLINE LPCTSTR COptionUI::GetGroup() const
{
    return m_sGroupName;
}

void COptionUI::SetGroup(LPCTSTR pStrGroupName)
{
    if (pStrGroupName == NULL)
    {
        if (m_sGroupName.IsEmpty()) { return; }

        m_sGroupName.Empty();
    }
    else
    {
        if (m_sGroupName == pStrGroupName) { return; }

        if (!m_sGroupName.IsEmpty() && m_pManager) { m_pManager->RemoveOptionGroup(m_sGroupName, this); }

        m_sGroupName = pStrGroupName;
    }

    if (!m_sGroupName.IsEmpty())
    {
        if (m_pManager) { m_pManager->AddOptionGroup(m_sGroupName, this); }
    }
    else
    {
        if (m_pManager) { m_pManager->RemoveOptionGroup(m_sGroupName, this); }
    }

    Selected(m_bSelected);
}

DUI_INLINE bool COptionUI::IsSelected() const
{
    return m_bSelected;
}

void COptionUI::Selected(bool bSelected, bool bTriggerEvent)
{
    if (m_bSelected == bSelected) { return; }

    m_bSelected = bSelected;

    if (m_bSelected) { m_uButtonState |= UISTATE_SELECTED; }
    else { m_uButtonState &= ~UISTATE_SELECTED; }

    if (m_pManager != NULL)
    {
        if (!m_sGroupName.IsEmpty())
        {
            if (m_bSelected)
            {
                CDuiPtrArray *aOptionGroup = m_pManager->GetOptionGroup(m_sGroupName);

                for (int i = 0; i < aOptionGroup->GetSize(); i++)
                {
                    COptionUI *pControl = static_cast<COptionUI *>(aOptionGroup->GetAt(i));

                    if (pControl != this)
                    {
                        pControl->Selected(false, bTriggerEvent);
                    }
                }

                if (bTriggerEvent) { m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED); }
            }
        }
        else
        {
            if (bTriggerEvent) { m_pManager->SendNotify(this, DUI_MSGTYPE_SELECTCHANGED); }
        }
    }

    Invalidate();
}

bool COptionUI::Activate()
{
    if (!CButtonUI::Activate()) { return false; }

    if (!m_sGroupName.IsEmpty()) { Selected(true); SwitchTabLayoutPage(); }
    else                         { Selected(!m_bSelected); }

    return true;
}

void COptionUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled())
    {
        if (m_bSelected) { m_uButtonState = UISTATE_SELECTED; }
        else { m_uButtonState = 0; }
    }
}

DUI_INLINE LPCTSTR COptionUI::GetSelNormalImage()
{
    return m_diSelNormal.sDrawString;
}

void COptionUI::SetSelNormalImage(LPCTSTR pStrImage)
{
    if (m_diSelNormal.sDrawString == pStrImage && m_diSelNormal.pImageInfo != NULL) { return; }

    m_diSelNormal.Clear();
    m_diSelNormal.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR COptionUI::GetSelHotImage()
{
    return m_diSelHot.sDrawString;
}

void COptionUI::SetSelHotImage(LPCTSTR pStrImage)
{
    if (m_diSelHot.sDrawString == pStrImage && m_diSelHot.pImageInfo != NULL) { return; }

    m_diSelHot.Clear();
    m_diSelHot.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR COptionUI::GetSelFocusedImage()
{
    return m_diSelFocused.sDrawString;
}

void COptionUI::SetSelFocusedImage(LPCTSTR pStrImage)
{
    if (m_diSelFocused.sDrawString == pStrImage && m_diSelFocused.pImageInfo != NULL) { return; }

    m_diSelFocused.Clear();
    m_diSelFocused.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR COptionUI::GetSelPushedImage()
{
    return m_diSelPushed.sDrawString;
}

void COptionUI::SetSelPushedImage(LPCTSTR pStrImage)
{
    if (m_diSelPushed.sDrawString == pStrImage && m_diSelPushed.pImageInfo != NULL) { return; }

    m_diSelPushed.Clear();
    m_diSelPushed.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR COptionUI::GetSelDisabledImage()
{
    return m_diSelDisabled.sDrawString;
}

void COptionUI::SetSelDisabledImage(LPCTSTR pStrImage)
{
    if (m_diSelDisabled.sDrawString == pStrImage && m_diSelDisabled.pImageInfo != NULL) { return; }

    m_diSelDisabled.Clear();
    m_diSelDisabled.sDrawString = pStrImage;
    Invalidate();
}

DUI_INLINE LPCTSTR COptionUI::GetUnselNormalImage()
{
    return m_diNormal.sDrawString;
}

DUI_INLINE void COptionUI::SetUnselNormalImage(LPCTSTR pStrImage)
{
    SetNormalImage(pStrImage);
}

DUI_INLINE LPCTSTR COptionUI::GetUnselHotImage()
{
    return m_diHot.sDrawString;
}

DUI_INLINE void COptionUI::SetUnselHotImage(LPCTSTR pStrImage)
{
    SetHotImage(pStrImage);
}

DUI_INLINE LPCTSTR COptionUI::GetUnselFocusedImage()
{
    return m_diFocused.sDrawString;
}

DUI_INLINE void COptionUI::SetUnselFocusedImage(LPCTSTR pStrImage)
{
    SetFocusedImage(pStrImage);
}

DUI_INLINE LPCTSTR COptionUI::GetUnselPushedImage()
{
    return m_diPushed.sDrawString;
}

DUI_INLINE void COptionUI::SetUnselPushedImage(LPCTSTR pStrImage)
{
    SetPushedImage(pStrImage);
}

DUI_INLINE LPCTSTR COptionUI::GetUnselDisabledImage()
{
    return m_diDisabled.sDrawString;
}

DUI_INLINE void COptionUI::SetUnselDisabledImage(LPCTSTR pStrImage)
{
    SetDisabledImage(pStrImage);
}

DUI_INLINE void COptionUI::SetSelTextColor(DWORD dwTextColor)
{
    if (m_dwPushedTextColor == dwTextColor) { return; }

    m_dwPushedTextColor = dwTextColor;
    Invalidate();
}

DUI_INLINE DWORD COptionUI::GetSelTextColor()
{
    return m_dwPushedTextColor;
}

DUI_INLINE void COptionUI::SetSelNormalBkColor(DWORD dwColor)
{
    if (m_dwSelNormalBkColor == dwColor) { return; }

    m_dwSelNormalBkColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD COptionUI::GetSelNormalBkColor()
{
    return m_dwSelNormalBkColor;
}

DUI_INLINE void COptionUI::SetSelHotBkColor(DWORD dwkColor)
{
    if (m_dwSelHotBkColor == dwkColor) { return; }

    m_dwSelHotBkColor = dwkColor;
    Invalidate();
}

DUI_INLINE DWORD COptionUI::GetSelHotBkColor()
{
    return m_dwSelHotBkColor;
}

void COptionUI::SetSelFocusedBkColor(DWORD dwColor)
{
    if (m_dwSelFocusedBkColor == dwColor) { return; }

    m_dwSelFocusedBkColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD COptionUI::GetSelFocusedBkColor()
{
    return m_dwSelFocusedBkColor;
}

void COptionUI::SetSelPushedBkColor(DWORD dwColor)
{
    if (m_dwSelPushedBkColor == dwColor) { return; }

    m_dwSelPushedBkColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD COptionUI::GetSelPushedBkColor()
{
    return m_dwSelPushedBkColor;
}

void COptionUI::SetSelDisabledBkColor(DWORD dwColor)
{
    if (m_dwSelDisabledBkColor = dwColor) { return; }

    m_dwSelDisabledBkColor = dwColor;
    Invalidate();
}

DUI_INLINE DWORD COptionUI::GetSelDisabledBkColor()
{
    return m_dwSelDisabledBkColor;
}

DUI_INLINE LPCTSTR COptionUI::GetForeImage()
{
    return m_diFore.sDrawString;
}

void COptionUI::SetForeImage(LPCTSTR pStrImage)
{
    if (m_diFore.sDrawString == pStrImage && m_diFore.pImageInfo != NULL) { return; }

    m_diFore.Clear();
    m_diFore.sDrawString = pStrImage;
    Invalidate();
}

SIZE COptionUI::EstimateSize(SIZE szAvailable)
{
    //if (m_cxyFixed.cy == 0) { return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8); }
    //return CControlUI::EstimateSize(szAvailable);
    return CButtonUI::EstimateSize(szAvailable);
}

void COptionUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("group")) == 0) { SetGroup(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("selected")) == 0) { Selected(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("foreimage")) == 0) { SetForeImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("selnormalimage")) == 0 || _tcscmp(pstrName, _T("selectedimage")) == 0)
    { SetSelNormalImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("selhotimage")) == 0 || _tcscmp(pstrName, _T("selectedhotimage")) == 0)
    { SetSelHotImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("selfocusedimage")) == 0) { SetSelFocusedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("selpushedimage")) == 0) { SetSelPushedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("seldisabledimage")) == 0) { SetSelDisabledImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("unselnormalimage")) == 0 || _tcscmp(pstrName, _T("normalimage")) == 0)
    { SetUnselNormalImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("unselhotimage")) == 0 || _tcscmp(pstrName, _T("hotimage")) == 0)
    { SetUnselHotImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("unselfocusedimage")) == 0 || _tcscmp(pstrName, _T("focusedimage")) == 0)
    { SetUnselFocusedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("unselpushedimage")) == 0 || _tcscmp(pstrName, _T("pushedimage")) == 0)
    { SetUnselPushedImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("unseldisabledimage")) == 0 || _tcscmp(pstrName, _T("disabledimage")) == 0)
    { SetUnselDisabledImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("selnormalbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelNormalBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("selhotbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelHotBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("selfocusedbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelFocusedBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("selpushedbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelPushedBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("seldisabledbkcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelDisabledBkColor(clr);
    }
    else if (_tcscmp(pstrName, _T("seltextcolor")) == 0 || _tcscmp(pstrName, _T("pushedtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("bindtablayout")) == 0)
    {
        m_sBindTabLayout = ParseString(pstrValue);
    }
    else if (_tcscmp(pstrName, _T("bindtabindex")) == 0)
    {
        m_nBindTabIndex = ParseInt(pstrValue);
        m_nBindTabIndex = m_nBindTabIndex >= 0 ? m_nBindTabIndex : 0;
    }
    else if (_tcscmp(pstrName, _T("fivestatusimage")) == 0) { DUITRACE(_T("不支持属性:fivestatusimage")); }
    else { CButtonUI::SetAttribute(pstrName, pstrValue); }
}

void COptionUI::PaintBkColor(HDC hDC)
{
    if (m_uButtonState & UISTATE_SELECTED)
    {
        if (!IsEnabled() && 0 != m_dwSelDisabledBkColor)
        {
            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelDisabledBkColor));
        }
        else if (0 != (m_uButtonState & UISTATE_PUSHED) && 0 != m_dwSelPushedBkColor)
        {
            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelPushedBkColor));
        }
        else if (IsFocused() && 0 != m_dwSelFocusedBkColor)
        {
            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelFocusedBkColor));
        }
        else if (0 != (m_uButtonState & UISTATE_HOT) && 0 != m_dwSelHotBkColor)
        {
            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelHotBkColor));
        }
        else if (0 != m_dwSelNormalBkColor)
        {
            CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelNormalBkColor));
        }
        else
        {
            CButtonUI::PaintBkColor(hDC);
        }
    }
    else
    {
        CButtonUI::PaintBkColor(hDC);
    }
}

void COptionUI::PaintStatusImage(HDC hDC)
{
    if (IsFocused()) { m_uButtonState |= UISTATE_FOCUSED; }
    else { m_uButtonState &= ~UISTATE_FOCUSED; }

    if (!IsEnabled()) { m_uButtonState |= UISTATE_DISABLED; }
    else { m_uButtonState &= ~UISTATE_DISABLED; }

    // 绘制顺序：5态 背景图->前景图
    if (m_uButtonState & UISTATE_SELECTED)
    {
        // 选中状态
        if ((m_uButtonState & UISTATE_DISABLED) != 0)
        {
            if (!DrawImage(hDC, m_diSelDisabled)) { DrawNormalBkImage(hDC, m_diSelNormal); }

            DrawImage(hDC, m_diFore);
        }
        else if ((m_uButtonState & UISTATE_PUSHED) != 0)
        {
            if (!DrawImage(hDC, m_diSelPushed)) { DrawNormalBkImage(hDC, m_diSelNormal); }

            DrawImage(hDC, m_diFore);
        }
        else if ((m_uButtonState & UISTATE_HOT) != 0)
        {
            DrawNormalBkImage(hDC, m_diSelNormal, &m_diSelHot, true);

            if (!DrawImage(hDC, m_diHotFore)) { DrawImage(hDC, m_diFore); }
        }
        else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
        {
            if (!DrawImage(hDC, m_diSelFocused)) { DrawNormalBkImage(hDC, m_diSelNormal); }

            DrawImage(hDC, m_diFore);
        }
        else
        {
            DrawNormalBkImage(hDC, m_diSelNormal);
            DrawImage(hDC, m_diFore);
        }
    }
    else
    {
        // 未选中状态
        CButtonUI::PaintStatusImage(hDC);
    }

    //if ((m_uButtonState & UISTATE_SELECTED) != 0)
    //{
    //    if ((m_uButtonState & UISTATE_HOT) != 0)
    //    {
    //        if (DrawImage(hDC, m_diSelectedHot)) { goto Label_ForeImage; }
    //    }
    //
    //    if (DrawImage(hDC, m_diSelected)) { goto Label_ForeImage; }
    //    else if (m_dwSelectedBkColor != 0)
    //    {
    //        CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwSelectedBkColor));
    //        goto Label_ForeImage;
    //    }
    //}
    //UINT uSavedState = m_uButtonState;
    //m_uButtonState &= ~UISTATE_PUSHED;
    //CButtonUI::PaintStatusImage(hDC);
    //m_uButtonState = uSavedState;
    //Label_ForeImage:
    //DrawImage(hDC, m_diFore);
}

void COptionUI::PaintText(HDC hDC)
{
    if ((m_uButtonState & UISTATE_SELECTED) != 0)
    {
        DWORD dwSavedColor = m_dwTextColor;
        m_dwTextColor = m_dwPushedTextColor;
        CButtonUI::PaintText(hDC);
        m_dwTextColor = dwSavedColor;
    }
    else
    {
        UINT uSavedState = m_uButtonState;
        m_uButtonState &= ~UISTATE_PUSHED;
        CButtonUI::PaintText(hDC);
        m_uButtonState = uSavedState;
    }

    //if ((m_uButtonState & UISTATE_SELECTED) != 0)
    //{
    //    if (m_sText.IsEmpty()) { return; }
    //
    //    if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }
    //
    //    if (m_dwDisabledTextColor == 0) { m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor(); }
    //
    //    int nLinks = 0;
    //    RECT rc = m_rcItem;
    //    rc.left += m_rcTextPadding.left;
    //    rc.right -= m_rcTextPadding.right;
    //    rc.top += m_rcTextPadding.top;
    //    rc.bottom -= m_rcTextPadding.bottom;
    //    DWORD dwTxtClr = (0 != m_dwPushedTextColor) ? m_dwPushedTextColor : m_dwTextColor;
    //    dwTxtClr = IsEnabled() ? dwTxtClr : m_dwDisabledTextColor;
    //
    //    if (m_bShowHtml)
    //        CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, dwTxtClr,
    //                                    NULL, NULL, nLinks, m_iFont, m_uTextStyle);
    //    else
    //        CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, dwTxtClr,
    //                                m_iFont, m_uTextStyle);
    //}
    //else
    //{
    //    UINT uSavedState = m_uButtonState;
    //    m_uButtonState &= ~UISTATE_PUSHED;
    //    CButtonUI::PaintText(hDC);
    //    m_uButtonState = uSavedState;
    //}
}

void COptionUI::PaintBorder(HDC hDC)
{
    if ((0 == m_dwBorderColor && 0 == m_dwHotBorderColor && 0 == m_dwFocusedBorderColor) ||
        (0 == m_rcBorderSize.left && 0 == m_rcBorderSize.right &&
         0 == m_rcBorderSize.top && 0 == m_rcBorderSize.bottom))
    {
        return;
    }

    DWORD clrBorder = GetAdjustColor(m_dwBorderColor);

    if (IsFocused() && m_dwFocusedBorderColor != 0)
    {
        clrBorder = GetAdjustColor(m_dwFocusedBorderColor);
    }
    else if (m_bHot && m_dwHotBorderColor != 0)
    {
        clrBorder = GetAdjustColor(m_dwHotBorderColor);
    }
    else if (IsSelected() && m_dwSelectedBorderColor)
    {
        clrBorder = GetAdjustColor(m_dwSelectedBorderColor);
    }

    if (0 == clrBorder) { return; }

    if (m_rcBorderSize.left > 0 && (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0))
    {
        //画圆角边框
        CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_rcBorderSize.left,
                                     m_cxyBorderRound.cx, m_cxyBorderRound.cy, clrBorder, m_nBorderStyle);
        return;
    }

    if (m_rcBorderSize.left == m_rcBorderSize.right && m_rcBorderSize.top == m_rcBorderSize.bottom &&
        m_rcBorderSize.left == m_rcBorderSize.top)
    {
        //画直角边框
        CRenderEngine::DrawRect(hDC, m_rcItem, m_rcBorderSize.left, clrBorder, m_nBorderStyle);
        return;
    }

    RECT rcBorder;
    int nBorderStyle = PS_INSIDEFRAME | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL | PS_GEOMETRIC;

    if (m_rcBorderSize.left > 0)
    {
        rcBorder = m_rcItem;
        rcBorder.left += m_rcBorderSize.left / 2;
        rcBorder.right = rcBorder.left;

        CRenderEngine::DrawRect(hDC, rcBorder, m_rcBorderSize.left, clrBorder, m_nBorderStyle);
    }

    if (m_rcBorderSize.top > 0)
    {
        rcBorder = m_rcItem;
        rcBorder.top += m_rcBorderSize.top / 2;
        rcBorder.bottom = rcBorder.top;
        rcBorder.left += m_rcBorderSize.top / 2;
        rcBorder.right -= m_rcBorderSize.right;

        CRenderEngine::DrawRect(hDC, rcBorder, m_rcBorderSize.top, clrBorder, m_nBorderStyle);
    }

    if (m_rcBorderSize.right > 0)
    {
        rcBorder = m_rcItem;
        rcBorder.left = m_rcItem.right - m_rcBorderSize.right / 2 - m_rcBorderSize.right % 2;
        rcBorder.right = rcBorder.left;

        CRenderEngine::DrawRect(hDC, rcBorder, m_rcBorderSize.right, clrBorder, m_nBorderStyle);
    }

    if (m_rcBorderSize.bottom > 0)
    {
        rcBorder = m_rcItem;
        rcBorder.top = m_rcItem.bottom - m_rcBorderSize.bottom / 2 - m_rcBorderSize.bottom % 2;
        rcBorder.bottom = rcBorder.top;
        rcBorder.left += m_rcBorderSize.bottom / 2;
        rcBorder.right -= m_rcBorderSize.right;

        CRenderEngine::DrawRect(hDC, rcBorder, m_rcBorderSize.bottom, clrBorder, m_nBorderStyle);
    }
}

void COptionUI::SwitchTabLayoutPage(void)
{
    if (m_sBindTabLayout.IsEmpty() || m_nBindTabIndex < 0) { return; }

    CTabLayoutUI *pLayout = static_cast<CTabLayoutUI *>(GetManager()->FindControl(m_sBindTabLayout));

    if (pLayout) { pLayout->SelectItem(m_nBindTabIndex); }
}

//////////////////////////////////////////////////////////////////////////
class CButtonGroupImpl
{
public:
    CButtonGroupImpl(void) { }
    ~CButtonGroupImpl(void) { }
    void AddButton(COptionUI *pBtn, int nId = -1);
    void DelButton(COptionUI *pBtn);

    void SetButtonId(COptionUI *pBtn, int nId);
    int GetButtonId(COptionUI *pBtn);
    COptionUI *GetButton(int nId);

    COptionUI *GetSelectedButton(void);
    int GetSelectedId(void);

    void SetSelectedButton(int nId);
    void SetSelectedButton(COptionUI *pBtn);

private:
    typedef std::map<COptionUI *, int>     CMapBtnId;
    CMapBtnId   m_mapBtnIds;
};

void CButtonGroupImpl::AddButton(COptionUI *pBtn, int nId /*= -1*/)
{
    m_mapBtnIds[pBtn] = nId;
}

void CButtonGroupImpl::DelButton(COptionUI *pBtn)
{
    CMapBtnId::iterator it(m_mapBtnIds.find(pBtn));

    if (it != m_mapBtnIds.end()) { m_mapBtnIds.erase(it); }
}

void CButtonGroupImpl::SetButtonId(COptionUI *pBtn, int nId)
{
    m_mapBtnIds[pBtn] = nId;
}

int CButtonGroupImpl::GetButtonId(COptionUI *pBtn)
{
    return m_mapBtnIds[pBtn];
}

COptionUI *CButtonGroupImpl::GetButton(int nId)
{
    for (CMapBtnId::iterator it(m_mapBtnIds.begin()); it != m_mapBtnIds.end(); ++it)
    {
        if (nId == it->second) { return it->first; }
    }

    return NULL;
}

COptionUI *CButtonGroupImpl::GetSelectedButton(void)
{
    for (CMapBtnId::iterator it(m_mapBtnIds.begin()); it != m_mapBtnIds.end(); ++it)
    {
        if (it->first->IsSelected()) { return it->first; }
    }

    return NULL;
}

int CButtonGroupImpl::GetSelectedId(void)
{
    for (CMapBtnId::iterator it(m_mapBtnIds.begin()); it != m_mapBtnIds.end(); ++it)
    {
        if (it->first->IsSelected())
        { return it->second; }
    }

    return -1;
}

void CButtonGroupImpl::SetSelectedButton(int nId)
{
    for (CMapBtnId::iterator it(m_mapBtnIds.begin()); it != m_mapBtnIds.end(); ++it)
    {
        if (it->second == nId)
        {
            it->first->Selected(true);
            break;
        }
    }
}

void CButtonGroupImpl::SetSelectedButton(COptionUI *pBtn)
{
    CMapBtnId::iterator it(m_mapBtnIds.find(pBtn));

    if (it != m_mapBtnIds.end())
    {
        it->first->Selected(true);
    }
}

CButtonGroup::CButtonGroup(void) : m_pBtnGroupImpl(NULL)
{
    m_pBtnGroupImpl = new CButtonGroupImpl();
}

CButtonGroup::~CButtonGroup(void)
{
    if (NULL != m_pBtnGroupImpl)
    {
        delete m_pBtnGroupImpl;
        m_pBtnGroupImpl = NULL;
    }
}

void CButtonGroup::AddButton(COptionUI *pBtn, int nId /*= -1*/)
{
    if (NULL != m_pBtnGroupImpl) { m_pBtnGroupImpl->AddButton(pBtn, nId); }
}

void CButtonGroup::DelButton(COptionUI *pBtn)
{
    if (NULL != m_pBtnGroupImpl) { m_pBtnGroupImpl->DelButton(pBtn); }
}

void CButtonGroup::SetButtonId(COptionUI *pBtn, int nId)
{
    if (NULL != m_pBtnGroupImpl) { m_pBtnGroupImpl->SetButtonId(pBtn, nId); }
}

int CButtonGroup::GetButtonId(COptionUI *pBtn)
{
    if (NULL != m_pBtnGroupImpl) { return m_pBtnGroupImpl->GetButtonId(pBtn); }

    return -1;
}

COptionUI *CButtonGroup::GetButton(int nId)
{
    if (NULL != m_pBtnGroupImpl) { return m_pBtnGroupImpl->GetButton(nId); }

    return NULL;
}

COptionUI *CButtonGroup::GetSelectedButton(void)
{
    if (NULL != m_pBtnGroupImpl) { return m_pBtnGroupImpl->GetSelectedButton(); }

    return NULL;
}

int CButtonGroup::GetSelectedId(void)
{
    if (NULL != m_pBtnGroupImpl) { return m_pBtnGroupImpl->GetSelectedId(); }

    return -1;
}

void CButtonGroup::SetSelectedButton(int nId)
{
    if (NULL != m_pBtnGroupImpl) { m_pBtnGroupImpl->SetSelectedButton(nId); }
}

void CButtonGroup::SetSelectedButton(COptionUI *pBtn)
{
    if (NULL != m_pBtnGroupImpl) { m_pBtnGroupImpl->SetSelectedButton(pBtn); }
}

}
