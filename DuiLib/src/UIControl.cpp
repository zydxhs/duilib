﻿#include "StdAfx.h"
#include <ShlGuid.h>
#include <ShObjIdl.h>
#include "DropSource.h"
#include "DataObject.h"
#include <string>

namespace DuiLib {

CControlUI::CControlUI()
    : m_pManager(NULL)
    , m_pParent(NULL)
    , m_pCover(NULL)
    , m_pEffect(NULL)
    , m_bUpdateNeeded(true)
    , m_bMenuUsed(false)
    , m_bAsyncNotify(false)
    , m_byEffectTrigger(TRIGGER_NONE)
    , m_bVisible(true)
    , m_bInternVisible(true)
    , m_bHot(false)
    , m_bFocused(false)
    , m_bEnabled(true)
    , m_bMouseEnabled(true)
    , m_bKeyboardEnabled(true)
    , m_bFloat(false)
    , m_bSetPos(false)
    , m_chShortcut('\0')
    , m_bNeedCtrl(false)
    , m_bNeedShift(false)
    , m_bNeedAlt(false)
    , m_pTag(NULL)
    , m_dwBackColor(0)
    , m_dwBackColor2(0)
    , m_dwBackColor3(0)
    , m_bDirection(true)
    , m_dwBorderColor(0)
    , m_dwHotBorderColor(0)
    , m_dwFocusedBorderColor(0)
    , m_dwSelectedBorderColor(0)
    , m_bColorHSL(false)
    , m_nBorderStyle(PS_SOLID)
    , m_nTooltipWidth(300)
    , m_nWeight(100)
    , m_bDropEnable(false)
    , m_bDragEnable(false)
    , m_bCapture(false)
    , m_bAutoWidth(false)
    , m_bAutoHeight(false)
{
    m_cXY.cx = m_cXY.cy = 0;
    m_cxyFixed.cx = m_cxyFixed.cy = 0;
    m_cxyMin.cx = m_cxyMin.cy = 0;
    m_cxyMax.cx = m_cxyMax.cy = MAX_CTRL_WIDTH;
    m_cxyBorderRound.cx = m_cxyBorderRound.cy = 0;

    ::ZeroMemory(&m_rcItem, sizeof(RECT));
    ::ZeroMemory(&m_rcMargin, sizeof(RECT));
    ::ZeroMemory(&m_rcPaint, sizeof(RECT));
    ::ZeroMemory(&m_rcBorderSize, sizeof(RECT));
    m_piFloatPercent.left = m_piFloatPercent.top = m_piFloatPercent.right = m_piFloatPercent.bottom = 0.0f;
}

CControlUI::~CControlUI()
{
    // 2018-08-18 zhuyadong 添加特效
    if (m_pEffect != NULL)
    {
        delete m_pEffect;
        m_pEffect = NULL;
    }

    if (m_pCover != NULL)
    {
        m_pCover->Delete();
        m_pCover = NULL;
    }

    RemoveAllCustomAttribute();

    if (OnDestroy) { OnDestroy(this); }

    if (m_pManager != NULL) { m_pManager->ReapObjects(this); }
}

void CControlUI::Delete()
{
    if (m_pManager) { m_pManager->RemoveMouseLeaveNeeded(this); }

    delete this;
}

CDuiString CControlUI::GetName() const
{
    return m_sName;
}

void CControlUI::SetName(LPCTSTR pstrName)
{
    if (m_sName != pstrName)
    {
        m_sName = pstrName;

        if (m_pManager != NULL) { m_pManager->RenameControl(this, pstrName); }
    }
}

LPVOID CControlUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_CONTROL) == 0) { return this; }

    return NULL;
}

LPCTSTR CControlUI::GetClass() const
{
    return DUI_CTR_CONTROL;
}

UINT CControlUI::GetControlFlags() const
{
    return 0;
}

HWND CControlUI::GetNativeWindow() const
{
    return NULL;
}

bool CControlUI::Activate()
{
    if (!IsVisible()) { return false; }

    if (!IsEnabled()) { return false; }

    return true;
}

CPaintManagerUI *CControlUI::GetManager() const
{
    return m_pManager;
}

void CControlUI::SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit)
{
    if (m_pCover != NULL) { m_pCover->SetManager(pManager, this, bInit); }

    m_pManager = pManager;
    m_pParent = pParent;

    if (bInit && m_pParent) { Init(); }
}

CControlUI *CControlUI::GetParent() const
{
    return m_pParent;
}

CControlUI *CControlUI::GetCover() const
{
    return m_pCover;
}

void CControlUI::SetCover(CControlUI *pControl)
{
    if (m_pCover == pControl) { return; }

    if (m_pCover != NULL) { m_pCover->Delete(); }

    m_pCover = pControl;

    if (m_pCover != NULL)
    {
        m_pManager->InitControls(m_pCover, this);

        if (IsVisible()) { NeedUpdate(); }
        else { pControl->SetInternVisible(false); }
    }
}

CDuiString CControlUI::GetText() const
{
    return m_sText;
}

void CControlUI::SetText(LPCTSTR pstrText)
{
    if (!pstrText && m_sText.IsEmpty()) { return; }

    if (pstrText && m_sText == pstrText) { return; }

    m_sText = pstrText;
    m_sTextOrig = m_sText;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sText);
    Invalidate();
}

void CControlUI::ReloadText(void)
{
    SetText(m_sTextOrig);
}

DWORD CControlUI::GetBkColor() const
{
    return m_dwBackColor;
}

void CControlUI::SetBkColor(DWORD dwBackColor)
{
    if (m_dwBackColor == dwBackColor) { return; }

    m_dwBackColor = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor2() const
{
    return m_dwBackColor2;
}

void CControlUI::SetBkColor2(DWORD dwBackColor)
{
    if (m_dwBackColor2 == dwBackColor) { return; }

    m_dwBackColor2 = dwBackColor;
    Invalidate();
}

DWORD CControlUI::GetBkColor3() const
{
    return m_dwBackColor3;
}

void CControlUI::SetBkColor3(DWORD dwBackColor)
{
    if (m_dwBackColor3 == dwBackColor) { return; }

    m_dwBackColor3 = dwBackColor;
    Invalidate();
}

void CControlUI::SetDirection(bool bHorizonal)
{
    if (m_bDirection == bHorizonal) { return; }

    m_bDirection = bHorizonal;
    Invalidate();
}

bool CControlUI::GetDirection(void)
{
    return m_bDirection;
}

LPCTSTR CControlUI::GetBkImage()
{
    return m_diBk.sDrawString;
}

void CControlUI::SetBkImage(LPCTSTR pStrImage)
{
    if (m_diBk.sDrawString == pStrImage && m_diBk.pImageInfo != NULL) { return; }

    m_diBk.Clear();
    m_diBk.sDrawString = pStrImage;
    DrawImage(NULL, m_diBk);

    if (m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0 && m_diBk.pImageInfo)
    {
        m_cxyFixed.cx = m_diBk.pImageInfo->nX;
        m_cxyFixed.cy = m_diBk.pImageInfo->nY;
    }

    Invalidate();
}

DWORD CControlUI::GetBorderColor() const
{
    return m_dwBorderColor;
}

void CControlUI::SetBorderColor(DWORD dwBorderColor)
{
    if (m_dwBorderColor == dwBorderColor) { return; }

    m_dwBorderColor = (dwBorderColor > 0x00ffffff) ? dwBorderColor : 0;
    Invalidate();
}

DWORD CControlUI::GetHotBorderColor() const
{
    return m_dwHotBorderColor;
}

void CControlUI::SetHotBorderColor(DWORD dwBorderColor)
{
    if (m_dwHotBorderColor == dwBorderColor) { return; }

    m_dwHotBorderColor = (dwBorderColor > 0x00ffffff) ? dwBorderColor : 0;
    Invalidate();
}

DWORD CControlUI::GetFocusBorderColor() const
{
    return m_dwFocusedBorderColor;
}

void CControlUI::SetFocusedBorderColor(DWORD dwBorderColor)
{
    if (m_dwFocusedBorderColor == dwBorderColor) { return; }

    m_dwFocusedBorderColor = (dwBorderColor > 0x00ffffff) ? dwBorderColor : 0;
    Invalidate();
}

DWORD CControlUI::GetSelectedBorderColor() const
{
    return m_dwSelectedBorderColor;
}

void CControlUI::SetSelectedBorderColor(DWORD dwSelectedColor)
{
    if (m_dwSelectedBorderColor == dwSelectedColor) { return; }

    m_dwSelectedBorderColor = (dwSelectedColor > 0x00ffffff) ? dwSelectedColor : 0;
    Invalidate();
}

bool CControlUI::IsColorHSL() const
{
    return m_bColorHSL;
}

void CControlUI::SetColorHSL(bool bColorHSL)
{
    if (m_bColorHSL == bColorHSL) { return; }

    m_bColorHSL = bColorHSL;
    Invalidate();
}

RECT CControlUI::GetBorderSize() const
{
    return m_rcBorderSize;
}

void CControlUI::SetBorderSize(RECT rc)
{
    m_rcBorderSize = rc;
    Invalidate();
}

void CControlUI::SetBorderSize(int iSize)
{
    m_rcBorderSize.left = m_rcBorderSize.top = m_rcBorderSize.right = m_rcBorderSize.bottom = iSize;
    Invalidate();
}

SIZE CControlUI::GetBorderRound() const
{
    return m_cxyBorderRound;
}

void CControlUI::SetBorderRound(SIZE cxyRound)
{
    m_cxyBorderRound = cxyRound;
    Invalidate();
}

bool CControlUI::DrawImage(HDC hDC, TDrawInfo &drawInfo)
{
    return CRenderEngine::DrawImage(hDC, m_pManager, m_rcItem, m_rcPaint, drawInfo);
}

const RECT &CControlUI::GetPos() const
{
    return m_rcItem;
}

RECT CControlUI::GetRelativePos() const
{
    CControlUI *pParent = GetParent();

    if (pParent != NULL)
    {
        RECT rcParentPos = pParent->GetPos();
        CDuiRect rcRelativePos(m_rcItem);
        rcRelativePos.Offset(-rcParentPos.left, -rcParentPos.top);
        return rcRelativePos;
    }
    else
    {
        return CDuiRect(0, 0, 0, 0);
    }
}

RECT CControlUI::GetClientPos() const
{
    return m_rcItem;
}

void CControlUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    if (rc.right < rc.left) { rc.right = rc.left; }

    if (rc.bottom < rc.top) { rc.bottom = rc.top; }

    CDuiRect invalidateRc = m_rcItem;

    if (::IsRectEmpty(&invalidateRc)) { invalidateRc = rc; }

    if (m_bFloat)
    {
        CControlUI *pParent = GetParent();

        if (pParent != NULL)
        {
            RECT rcParentPos = pParent->GetPos();
            RECT rcCtrl = {rcParentPos.left + rc.left, rcParentPos.top + rc.top,
                           rcParentPos.left + rc.right, rcParentPos.top + rc.bottom
                          };
            m_rcItem = rcCtrl;

            LONG width = rcParentPos.right - rcParentPos.left;
            LONG height = rcParentPos.bottom - rcParentPos.top;
            RECT rcPercent = {(LONG)(width * m_piFloatPercent.left), (LONG)(height * m_piFloatPercent.top),
                              (LONG)(width * m_piFloatPercent.right), (LONG)(height * m_piFloatPercent.bottom)
                             };
            m_cXY.cx = rc.left - rcPercent.left;
            m_cXY.cy = rc.top - rcPercent.top;
            m_cxyFixed.cx = rc.right - rcPercent.right - m_cXY.cx;
            m_cxyFixed.cy = rc.bottom - rcPercent.bottom - m_cXY.cy;
        }
    }
    else
    {
        m_rcItem = rc;
    }

    if (m_pManager == NULL) { return; }

    if (!m_bSetPos)
    {
        m_bSetPos = true;

        if (OnSize) { OnSize(this); }

        m_bSetPos = false;
    }

    m_bUpdateNeeded = false;

    if (bNeedInvalidate && IsVisible())
    {
        invalidateRc.Join(m_rcItem);
        CControlUI *pParent = this;
        RECT rcTemp;
        RECT rcParent;

        while (pParent = pParent->GetParent())
        {
            if (!pParent->IsVisible()) { return; }

            rcTemp = invalidateRc;
            rcParent = pParent->GetPos();

            if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent)) { return; }
        }

        m_pManager->Invalidate(invalidateRc);
    }

    if (m_pCover != NULL && m_pCover->IsVisible())
    {
        if (m_pCover->IsFloat())
        {
            SIZE szXY = m_pCover->GetFixedXY();
            SIZE sz = {m_pCover->GetFixedWidth(), m_pCover->GetFixedHeight()};
            TPercentInfo rcPercent = m_pCover->GetFloatPercent();
            LONG width = m_rcItem.right - m_rcItem.left;
            LONG height = m_rcItem.bottom - m_rcItem.top;
            RECT rcCtrl = { 0 };
            rcCtrl.left = (LONG)(width * rcPercent.left) + szXY.cx;
            rcCtrl.top = (LONG)(height * rcPercent.top) + szXY.cy;
            rcCtrl.right = (LONG)(width * rcPercent.right) + szXY.cx + sz.cx;
            rcCtrl.bottom = (LONG)(height * rcPercent.bottom) + szXY.cy + sz.cy;
            m_pCover->SetPos(rcCtrl, false);
        }
        else
        {
            SIZE sz = { rc.right - rc.left, rc.bottom - rc.top };

            if (sz.cx < m_pCover->GetMinWidth()) { sz.cx = m_pCover->GetMinWidth(); }

            if (sz.cx > m_pCover->GetMaxWidth()) { sz.cx = m_pCover->GetMaxWidth(); }

            if (sz.cy < m_pCover->GetMinHeight()) { sz.cy = m_pCover->GetMinHeight(); }

            if (sz.cy > m_pCover->GetMaxHeight()) { sz.cy = m_pCover->GetMaxHeight(); }

            RECT rcCtrl = { rc.left, rc.top, rc.left + sz.cx, rc.top + sz.cy };
            m_pCover->SetPos(rcCtrl, false);
        }
    }
}

void CControlUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
    CDuiRect invalidateRc = m_rcItem;
    m_rcItem.left += szOffset.cx;
    m_rcItem.top += szOffset.cy;
    m_rcItem.right += szOffset.cx;
    m_rcItem.bottom += szOffset.cy;

    if (bNeedInvalidate && m_pManager == NULL && IsVisible())
    {
        invalidateRc.Join(m_rcItem);
        CControlUI *pParent = this;
        RECT rcTemp;
        RECT rcParent;

        while (pParent = pParent->GetParent())
        {
            if (!pParent->IsVisible()) { return; }

            rcTemp = invalidateRc;
            rcParent = pParent->GetPos();

            if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent)) { return; }
        }

        m_pManager->Invalidate(invalidateRc);
    }

    if (m_pCover != NULL && m_pCover->IsVisible()) { m_pCover->Move(szOffset, false); }
}

int CControlUI::GetWidth() const
{
    return m_rcItem.right - m_rcItem.left;
}

int CControlUI::GetHeight() const
{
    return m_rcItem.bottom - m_rcItem.top;
}

int CControlUI::GetX() const
{
    return m_rcItem.left;
}

int CControlUI::GetY() const
{
    return m_rcItem.top;
}

SIZE CControlUI::GetFixedXY() const
{
    return m_cXY;
}

void CControlUI::SetFixedXY(SIZE szXY)
{
    m_cXY.cx = szXY.cx;
    m_cXY.cy = szXY.cy;
    NeedParentUpdate();
}

TPercentInfo CControlUI::GetFloatPercent() const
{
    return m_piFloatPercent;
}

void CControlUI::SetFloatPercent(TPercentInfo piFloatPercent)
{
    m_piFloatPercent = piFloatPercent;
    NeedParentUpdate();
}

int CControlUI::GetFixedWidth() const
{
    return m_cxyFixed.cx;
}

void CControlUI::SetFixedWidth(int cx)
{
    if (cx < 0) { return; }

    m_cxyFixed.cx = cx;
    NeedParentUpdate();
}

int CControlUI::GetFixedHeight() const
{
    return m_cxyFixed.cy;
}

void CControlUI::SetFixedHeight(int cy)
{
    if (cy < 0) { return; }

    m_cxyFixed.cy = cy;
    NeedParentUpdate();
}

int CControlUI::GetMinWidth() const
{
    return m_cxyMin.cx;
}

void CControlUI::SetMinWidth(int cx)
{
    if (m_cxyMin.cx == cx) { return; }

    if (cx < 0) { return; }

    m_cxyMin.cx = cx;
    NeedParentUpdate();
}

int CControlUI::GetMaxWidth() const
{
    if (m_cxyMax.cx < m_cxyMin.cx) { return m_cxyMin.cx; }

    return m_cxyMax.cx;
}

void CControlUI::SetMaxWidth(int cx)
{
    if (m_cxyMax.cx == cx) { return; }

    if (cx < 0) { return; }

    m_cxyMax.cx = cx;
    NeedParentUpdate();
}

int CControlUI::GetMinHeight() const
{
    return m_cxyMin.cy;
}

void CControlUI::SetMinHeight(int cy)
{
    if (m_cxyMin.cy == cy) { return; }

    if (cy < 0) { return; }

    m_cxyMin.cy = cy;
    NeedParentUpdate();
}

int CControlUI::GetMaxHeight() const
{
    if (m_cxyMax.cy < m_cxyMin.cy) { return m_cxyMin.cy; }

    return m_cxyMax.cy;
}

void CControlUI::SetMaxHeight(int cy)
{
    if (m_cxyMax.cy == cy) { return; }

    if (cy < 0) { return; }

    m_cxyMax.cy = cy;
    NeedParentUpdate();
}

CDuiString CControlUI::GetToolTip() const
{
    return m_sToolTip;
}

void CControlUI::SetToolTip(LPCTSTR pstrText)
{
    m_sToolTip = pstrText;
    m_sToolTip.Replace(_T("<n>"), _T("\r\n"));
    m_sToolTipOrig = m_sToolTip;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sToolTip);
}

void CControlUI::ReloadToolTip(void)
{
    m_sToolTip = m_sToolTipOrig;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sToolTip);
    Invalidate();
}

void CControlUI::SetToolTipWidth(int nWidth)
{
    m_nTooltipWidth = nWidth;
}

int CControlUI::GetToolTipWidth(void)
{
    return m_nTooltipWidth;
}

TCHAR CControlUI::GetShortcut() const
{
    return m_chShortcut;
}

void CControlUI::SetShortcut(LPCTSTR pstrText)
{
#if defined(UNICODE) || defined(_UNICODE)
    typedef std::wstring tstring;
#else
    typedef std::string  tstring;
#endif
    LPTSTR pText = (LPTSTR)pstrText;
    pText = _tcsupr(pText);
    tstring sTxt(pText);

    size_t nPos;
    nPos = sTxt.find(_T("CTRL"));

    if (tstring::npos != nPos)
    {
        m_bNeedCtrl = true;
        sTxt.erase(nPos, 4);
    }

    nPos = sTxt.find(_T("SHIFT"));

    if (tstring::npos != nPos)
    {
        m_bNeedShift = true;
        sTxt.erase(nPos, 5);
    }

    nPos = sTxt.find(_T("ALT"));

    if (tstring::npos != nPos)
    {
        m_bNeedAlt = true;
        sTxt.erase(nPos, 3);
    }

    nPos = sTxt.find_first_of(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));

    if (tstring::npos != nPos)
    {
        m_chShortcut = sTxt[nPos];

        // 2018-08-23 zhuyadong 快捷键设置：不支持 SHIFT + 字母，只有字母时，自动添加 ALT
        // 如果快捷键为 SHIFT + 字母，则自动去掉 SHIFT
        if (!m_bNeedCtrl && !m_bNeedAlt) { m_bNeedShift = false; }

        // 如果快捷键只有一个字母，则自动添加 ALT
        if (!m_bNeedCtrl && !m_bNeedShift) { m_bNeedAlt = true; }
    }
    else
    {
        m_bNeedCtrl = m_bNeedShift = m_bNeedAlt = false;
    }
}

bool CControlUI::IsNeedCtrl(void) const
{
    return m_bNeedCtrl;
}

bool CControlUI::IsNeedShift(void) const
{
    return m_bNeedShift;
}

bool CControlUI::IsNeedAlt(void) const
{
    return m_bNeedAlt;
}

bool CControlUI::IsContextMenuUsed() const
{
    return m_bMenuUsed;
}

void CControlUI::SetContextMenuUsed(bool bMenuUsed)
{
    m_bMenuUsed = bMenuUsed;
}

const CDuiString &CControlUI::GetUserData()
{
    return m_sUserData;
}

void CControlUI::SetUserData(LPCTSTR pstrText)
{
    m_sUserData = pstrText;
}

UINT_PTR CControlUI::GetTag() const
{
    return m_pTag;
}

void CControlUI::SetTag(UINT_PTR pTag)
{
    m_pTag = pTag;
}

bool CControlUI::IsVisible() const
{
    return m_bVisible && m_bInternVisible;
}

bool CControlUI::SetVisible(bool bVisible /*= true*/)
{
    if (m_bVisible == bVisible) { return true; }

    // 2018-08-18 zhuyadong 修复控件首次显示特效问题
    if (m_pEffect && TRIGGER_NONE == m_byEffectTrigger)
    {
        if (!bVisible && StartEffect(TRIGGER_HIDE)) { return false; }
    }

    bool v = IsVisible();
    m_bVisible = bVisible;

    if (m_bFocused) { m_bFocused = false; }

    if (!bVisible && m_pManager && m_pManager->GetFocus() == this)
    {
        m_pManager->SetFocus(NULL);
    }

    if (IsVisible() != v)
    {
        NeedParentUpdate();
    }

    if (m_pCover != NULL) { m_pCover->SetInternVisible(IsVisible()); }

    // 2018-08-18 zhuyadong 修复控件首次显示特效问题
    if (m_pEffect && TRIGGER_NONE == m_byEffectTrigger && bVisible)
    {
        if (m_rcItem.left == m_rcItem.right || m_rcItem.top == m_rcItem.bottom && m_pParent)
        {
            RECT rc = m_pParent->GetPos();
            m_pParent->SetPos(rc, true);
        }

        StartEffect(TRIGGER_SHOW);
    }

    return true;
}

void CControlUI::SetInternVisible(bool bVisible)
{
    m_bInternVisible = bVisible;

    if (!bVisible && m_pManager && m_pManager->GetFocus() == this)
    {
        m_pManager->SetFocus(NULL);
    }

    if (m_pCover != NULL) { m_pCover->SetInternVisible(IsVisible()); }
}

bool CControlUI::IsEnabled() const
{
    return m_bEnabled;
}

void CControlUI::SetEnabled(bool bEnabled)
{
    if (m_bEnabled == bEnabled) { return; }

    m_bEnabled = bEnabled;
    Invalidate();
}

bool CControlUI::IsMouseEnabled() const
{
    return m_bMouseEnabled;
}

void CControlUI::SetMouseEnabled(bool bEnabled)
{
    m_bMouseEnabled = bEnabled;
}

bool CControlUI::IsKeyboardEnabled() const
{
    return m_bKeyboardEnabled;
}
void CControlUI::SetKeyboardEnabled(bool bEnabled)
{
    m_bKeyboardEnabled = bEnabled;
}

bool CControlUI::IsHot() const
{
    return m_bHot;
}

bool CControlUI::IsFocused() const
{
    return m_bFocused;
}

void CControlUI::SetFocus()
{
    if (m_pManager != NULL) { m_pManager->SetFocus(this, false); }
}

bool CControlUI::IsFloat() const
{
    return m_bFloat;
}

void CControlUI::SetFloat(bool bFloat)
{
    if (m_bFloat == bFloat) { return; }

    m_bFloat = bFloat;
    NeedParentUpdate();
}

void CControlUI::SetWeight(int nWeight)
{
    m_nWeight = nWeight;
    NeedParentUpdate();
}

int CControlUI::GetWeight(void)
{
    return m_nWeight;
}

void CControlUI::AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr)
{
    if (pstrName == NULL || pstrName[0] == _T('\0') || pstrAttr == NULL || pstrAttr[0] == _T('\0')) { return; }

    CDuiString *pCostomAttr = new CDuiString(pstrAttr);

    if (pCostomAttr != NULL)
    {
        if (m_mCustomAttrHash.Find(pstrName) == NULL)
        { m_mCustomAttrHash.Set(pstrName, (LPVOID)pCostomAttr); }
        else
        { delete pCostomAttr; }
    }
}

LPCTSTR CControlUI::GetCustomAttribute(LPCTSTR pstrName) const
{
    if (pstrName == NULL || pstrName[0] == _T('\0')) { return NULL; }

    CDuiString *pCostomAttr = static_cast<CDuiString *>(m_mCustomAttrHash.Find(pstrName));

    if (pCostomAttr) { return pCostomAttr->GetData(); }

    return NULL;
}

bool CControlUI::RemoveCustomAttribute(LPCTSTR pstrName)
{
    if (pstrName == NULL || pstrName[0] == _T('\0')) { return NULL; }

    CDuiString *pCostomAttr = static_cast<CDuiString *>(m_mCustomAttrHash.Find(pstrName));

    if (!pCostomAttr) { return false; }

    delete pCostomAttr;
    return m_mCustomAttrHash.Remove(pstrName);
}

void CControlUI::RemoveAllCustomAttribute()
{
    CDuiString *pCostomAttr;

    for (int i = 0; i < m_mCustomAttrHash.GetSize(); i++)
    {
        if (LPCTSTR key = m_mCustomAttrHash.GetAt(i))
        {
            pCostomAttr = static_cast<CDuiString *>(m_mCustomAttrHash.Find(key));
            delete pCostomAttr;
        }
    }

    m_mCustomAttrHash.Resize();
}

CControlUI *CControlUI::FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags)
{
    if ((uFlags & UIFIND_VISIBLE) != 0 && !IsVisible()) { return NULL; }

    if ((uFlags & UIFIND_ENABLED) != 0 && !IsEnabled()) { return NULL; }

    if ((uFlags & UIFIND_HITTEST) != 0 && (!::PtInRect(&m_rcItem, * static_cast<LPPOINT>(pData)))) { return NULL; }

    if ((uFlags & UIFIND_UPDATETEST) != 0 && Proc(this, pData) != NULL) { return NULL; }

    CControlUI *pResult = NULL;

    if ((uFlags & UIFIND_ME_FIRST) != 0)
    {
        if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) { pResult = Proc(this, pData); }
    }

    if (pResult == NULL && m_pCover != NULL)
    {
        /*if( (uFlags & UIFIND_HITTEST) == 0 || true)*/ pResult = m_pCover->FindControl(Proc, pData, uFlags);
    }

    if (pResult == NULL && (uFlags & UIFIND_ME_FIRST) == 0)
    {
        if ((uFlags & UIFIND_HITTEST) == 0 || IsMouseEnabled()) { pResult = Proc(this, pData); }
    }

    return pResult;
}

void CControlUI::Invalidate()
{
    if (!IsVisible()) { return; }

    RECT invalidateRc = m_rcItem;

    CControlUI *pParent = this;
    RECT rcTemp;
    RECT rcParent;

    while (pParent = pParent->GetParent())
    {
        rcTemp = invalidateRc;
        rcParent = pParent->GetPos();

        if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent))
        {
            return;
        }
    }

    if (m_pManager != NULL) { m_pManager->Invalidate(invalidateRc); }
}

bool CControlUI::IsUpdateNeeded() const
{
    return m_bUpdateNeeded;
}

void CControlUI::NeedUpdate()
{
    if (!IsVisible()) { return; }

    m_bUpdateNeeded = true;
    Invalidate();

    if (m_pManager != NULL) { m_pManager->NeedUpdate(); }
}

void CControlUI::NeedParentUpdate()
{
    if (GetParent())
    {
        GetParent()->NeedUpdate();
        GetParent()->Invalidate();
    }
    else
    {
        NeedUpdate();
    }

    if (m_pManager != NULL) { m_pManager->NeedUpdate(); }
}

DWORD CControlUI::GetAdjustColor(DWORD dwColor)
{
    // 2018-03-29 zyd 颜色透明显示。保留 alpha 通道值，绘制颜色时使用
    if (!m_bColorHSL) { return dwColor; }

    short H, S, L;
    CPaintManagerUI::GetHSL(&H, &S, &L);
    return CRenderEngine::AdjustColor(dwColor, H, S, L);
}

void CControlUI::Init()
{
    DoInit();

    if (OnInit) { OnInit(this); }
}

void CControlUI::DoInit()
{

}

void CControlUI::Event(TEventUI &event)
{
    if (OnEvent(&event)) { DoEvent(event); }
}

void CControlUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_SETCURSOR)
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
        return;
    }

    if (event.Type == UIEVENT_SETFOCUS)
    {
        m_bFocused = true;
        Invalidate();
        return;
    }

    if (event.Type == UIEVENT_KILLFOCUS)
    {
        m_bFocused = false;
        Invalidate();
        return;
    }

    if (event.Type == UIEVENT_TIMER)
    {
        m_pManager->SendNotify(this, DUI_MSGTYPE_TIMER, event.wParam, event.lParam);

        // 2018-08-18 zhuyadong 添加特效
        if (event.wParam <= TIMERID_TRIGGER_MAX && m_pEffect)
        {
            m_pEffect->OnElapse(event.wParam);
        }

        return;
    }

    if (event.Type == UIEVENT_CONTEXTMENU && IsEnabled())
    {
        ReleaseCapture();

        if (IsContextMenuUsed())
        {
            m_pManager->SendNotify(this, DUI_MSGTYPE_MENU, event.wParam, event.lParam, true);
            return;
        }
    }

    // 2017-02-25 zhuyadong 添加双击事件
    if (event.Type == UIEVENT_DBLCLICK)
    {
        m_pManager->SendNotify(this, DUI_MSGTYPE_DBLCLICK);
        return;
    }

    // 拖拽
    if (event.Type == UIEVENT_MOUSEMOVE && (event.wParam & MK_LBUTTON) && m_bDragEnable)
    {
        OnDoDragDrop(event);
        return;
    }

    if (event.Type == UIEVENT_MOUSEENTER)
    {
        if (::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())
        {
            m_bHot = true;
        }

        // 2018-08-18 zhuyadong 添加特效。鼠标移入特效
        StartEffect(TRIGGER_ENTER);
    }

    if (event.Type == UIEVENT_MOUSELEAVE)
    {
        if (!::PtInRect(&m_rcItem, event.ptMouse) && IsEnabled())
        {
            m_bHot = false;
        }

        // 2018-08-18 zhuyadong 添加特效。鼠标移出特效
        StartEffect(TRIGGER_LEAVE);
    }

    if (m_pParent != NULL) { m_pParent->DoEvent(event); }
}


void CControlUI::SetVirtualWnd(LPCTSTR pstrValue)
{
    m_sVirtualWnd = pstrValue;
    m_pManager->UsedVirtualWnd(true);
}

CDuiString CControlUI::GetVirtualWnd() const
{
    CDuiString str;

    if (!m_sVirtualWnd.IsEmpty())
    {
        str = m_sVirtualWnd;
    }
    else
    {
        CControlUI *pParent = GetParent();

        if (pParent != NULL)
        {
            str = pParent->GetVirtualWnd();
        }
        else
        {
            str = _T("");
        }
    }

    return str;
}

DUI_INLINE void CControlUI::SetDropEnable(bool bDropEnable)
{
    m_bDropEnable = bDropEnable;
}

DUI_INLINE bool CControlUI::GetDropEnable(void)
{
    return m_bDropEnable;
}

DUI_INLINE void CControlUI::SetDragEnable(bool bDragEnable)
{
    m_bDragEnable = bDragEnable;
}

DUI_INLINE bool CControlUI::GetDragEnable(void)
{
    return m_bDragEnable;
}

void CControlUI::OnDragEnter(COleDataHelper *pDataHelper)
{
    if (m_bDropEnable)
    {
        pDataHelper->SetEffect(DROPEFFECT_COPY);
        m_pManager->SendNotify(this, DUI_MSGTYPE_DRAGENTER, (WPARAM)pDataHelper);
    }
    else
    {
        if (m_pParent != NULL) { m_pParent->OnDragEnter(pDataHelper); }
        else                   { pDataHelper->SetEffect(DROPEFFECT_NONE); }
    }
}

void CControlUI::OnDragOver(COleDataHelper *pDataHelper)
{
    if (m_bDropEnable)
    {
        pDataHelper->SetEffect(DROPEFFECT_COPY);
        m_pManager->SendNotify(this, DUI_MSGTYPE_DRAGOVER, (WPARAM)pDataHelper);
    }
    else
    {
        pDataHelper->SetEffect(DROPEFFECT_NONE);

        if (m_pParent != NULL) { m_pParent->OnDragOver(pDataHelper); }
    }
}

void CControlUI::OnDragLeave(void)
{
    if (m_bDropEnable) { m_pManager->SendNotify(this, DUI_MSGTYPE_DRAGLEAVE); }
    else if (m_pParent != NULL) { m_pParent->OnDragLeave(); }
}

void CControlUI::OnDragDrop(COleDataHelper *pDataHelper)
{
    if (m_bDropEnable)
    {
        pDataHelper->SetEffect(DROPEFFECT_COPY);
        m_pManager->SendNotify(this, DUI_MSGTYPE_DRAGDROP, (WPARAM)pDataHelper);
    }
    else
    {
        pDataHelper->SetEffect(DROPEFFECT_NONE);

        if (m_pParent != NULL) { m_pParent->OnDragDrop(pDataHelper); }
    }
}

void CControlUI::SetCapture(void)
{
    if (m_pManager && !m_bCapture)
    {
        m_pManager->SetCapture(this);
        m_bCapture = true;
    }
}

void CControlUI::ReleaseCapture(void)
{
    if (m_pManager && m_bCapture)
    {
        m_pManager->ReleaseCapture(this);
        m_bCapture = false;
    }
}

void CControlUI::OnDoDragDrop(TEventUI &evt)
{
    SetCapture();
    // 调用接口，由用户填充数据
    COleDataHelper cDataObjHelper;
    IDataObject *pDataObject = cDataObjHelper.CreateDataObject();
    TNotifyUI msg;
    msg.sType = DUI_MSGTYPE_DRAGDATA;
    msg.pSender = this;
    msg.dwTimestamp = 0;
    msg.ptMouse = evt.ptMouse;
    msg.wParam = (WPARAM)&cDataObjHelper;
    msg.lParam = 0;
    m_pManager->SendNotify(msg);

    IDropSource *pDropSource = NULL;
    IDragSourceHelper *pDragHelper = NULL;
    DWORD        dwEffect;
    DWORD        dwResult;

    // Create IDropSource COM objects
    CreateDropSource(&pDropSource);
    CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_ALL, IID_IDragSourceHelper, (LPVOID *)&pDragHelper);

    if (!m_diDrag.bLoaded && !m_diDrag.sDrawString.IsEmpty())
    {
        CDuiString sResType;
        DWORD dwMask = 0;
        bool bHSL = false;
        CRenderEngine::ParseDrawInfo(m_diDrag, sResType, dwMask, bHSL);
        m_diDrag.pImageInfo = m_pManager->AddImage(m_diDrag.sImageName, sResType, dwMask, bHSL);
    }

    if (NULL == m_diDrag.pImageInfo)
    {
        pDragHelper->InitializeFromWindow(m_pManager->GetPaintWindow(), &evt.ptMouse, pDataObject);
    }
    else
    {
        SHDRAGIMAGE sdi;
        sdi.sizeDragImage.cx = m_diDrag.pImageInfo->nX;
        sdi.sizeDragImage.cy = m_diDrag.pImageInfo->nY;
        sdi.ptOffset.x = m_diDrag.pImageInfo->nX / 2;
        sdi.ptOffset.y = m_diDrag.pImageInfo->nY - 10;
        sdi.hbmpDragImage = m_diDrag.pImageInfo->hBitmap;
        sdi.crColorKey = 0xFFFFFFFF;
        HRESULT hr = pDragHelper->InitializeFromBitmap(&sdi, pDataObject);
        // sdi.sizeDragImage.cx = 40;
        // sdi.sizeDragImage.cy = 40;
        // sdi.ptOffset.x = sdi.ptOffset.y = 0;
        // sdi.crColorKey = 0xFFFFFFFF;
        // HBITMAP hBMP = CreateCompatibleBitmap(m_pManager->GetPaintDC(), 40, 40);
        // HDC hCloneDC = CreateCompatibleDC(m_pManager->GetPaintDC());
        // HBITMAP hOld = (HBITMAP)SelectObject(hCloneDC, hBMP);
        // SendMessage(m_pManager->GetPaintWindow(), WM_PRINT, (WPARAM)hCloneDC,
        //             (LPARAM)PRF_ERASEBKGND | PRF_NONCLIENT | PRF_CHILDREN | PRF_CLIENT);
        // ::SelectObject(hCloneDC, hOld);
        // sdi.hbmpDragImage = hBMP;
        // HRESULT hr = pDragHelper->InitializeFromBitmap(&sdi, pDataObject);
        // if (S_OK != hr) { DeleteObject(hBMP); }
    }

    // 暂时只支持复制
    DWORD dwEffects = DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK | DROPEFFECT_SCROLL;
    dwResult = DoDragDrop(pDataObject, pDropSource, dwEffects, &dwEffect);

    // success!
    // if (dwResult == DRAGDROP_S_DROP)
    // {
    //     if (dwEffect & DROPEFFECT_MOVE)
    //     {
    //         // remove selectionHello World from edit control
    //     }
    // }
    // // cancelled
    // else if (dwResult == DRAGDROP_S_CANCEL)
    // {
    // }
    pDragHelper->Release();
    pDropSource->Release();

    ReleaseCapture();
}

// 2018-08-18 zhuyadong 添加特效
void CControlUI::ParseEffectInfo(LPCTSTR pstrValue, BYTE &byEffect, WORD &wElapse, bool &bDirection,
                                 bool &bLoop)
{
    LPTSTR pstr = NULL;
    byEffect = (BYTE)_tcstol(pstrValue, &pstr, 10); ASSERT(pstr);
    wElapse = (WORD)_tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
    pstrValue = pstr + 1;
    LPCTSTR pstr2 = _tcschr(pstrValue, _T(',')); ASSERT(pstr2);
    bDirection = (_tcsncmp(pstrValue, _T("true"), 4) == 0) ? true : false;
    bLoop = (_tcsncmp(pstr2 + 1, _T("true"), 4) == 0) ? true : false;
}

CDuiString CControlUI::GetAttribute(LPCTSTR pstrName)
{
    return _T("");
}

void CControlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("pos")) == 0)
    {
        RECT rcPos = { 0 };
        LPTSTR pstr = NULL;
        rcPos.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
        rcPos.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
        rcPos.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
        rcPos.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
        SIZE szXY = {rcPos.left, rcPos.top};
        SetFixedXY(szXY);
        //ASSERT(rcPos.right - rcPos.left >= 0);
        //ASSERT(rcPos.bottom - rcPos.top >= 0);
        SetFixedWidth(rcPos.right - rcPos.left);
        SetFixedHeight(rcPos.bottom - rcPos.top);
    }
    else if (_tcscmp(pstrName, _T("bkcolor")) == 0 || _tcscmp(pstrName, _T("bkcolor1")) == 0)
    {
        while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) { pstrValue = ::CharNext(pstrValue); }

        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("bkcolor2")) == 0)
    {
        while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) { pstrValue = ::CharNext(pstrValue); }

        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor2(clrColor);
    }
    else if (_tcscmp(pstrName, _T("bkcolor3")) == 0)
    {
        while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) { pstrValue = ::CharNext(pstrValue); }

        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBkColor3(clrColor);
    }
    else if (_tcscmp(pstrName, _T("direction")) == 0) { m_bDirection = _tcscmp(pstrValue, _T("true")) == 0; }
    else if (_tcscmp(pstrName, _T("bordercolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetBorderColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("hotbordercolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetHotBorderColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("focusedbordercolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetFocusedBorderColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("selectedbordercolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetSelectedBorderColor(clrColor);
    }
    else if (_tcscmp(pstrName, _T("colorhsl")) == 0) { SetColorHSL(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("bordersize")) == 0)
    {
        CDuiString nValue = pstrValue;

        if (nValue.Find(',') < 0)
        {
            SetBorderSize(_ttoi(pstrValue));
        }
        else
        {
            RECT rcBorder = { 0 };
            LPTSTR pstr = NULL;
            rcBorder.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
            rcBorder.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
            rcBorder.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
            rcBorder.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
            SetBorderSize(rcBorder);
        }
    }
    else if (_tcscmp(pstrName, _T("borderstyle")) == 0) { SetBorderStyle(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("borderround")) == 0)
    {
        SIZE cxyRound = { 0 };
        LPTSTR pstr = NULL;
        cxyRound.cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
        cxyRound.cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
        SetBorderRound(cxyRound);
    }
    else if (_tcscmp(pstrName, _T("bkimage")) == 0) { SetBkImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("width")) == 0) { SetFixedWidth(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("height")) == 0) { SetFixedHeight(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("minwidth")) == 0) { SetMinWidth(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("minheight")) == 0) { SetMinHeight(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("maxwidth")) == 0) { SetMaxWidth(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("maxheight")) == 0) { SetMaxHeight(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("name")) == 0) { SetName(pstrValue); }
    else if (_tcscmp(pstrName, _T("text")) == 0) { SetText(pstrValue); }
    else if (_tcscmp(pstrName, _T("tooltip")) == 0) { SetToolTip(pstrValue); }
    else if (_tcscmp(pstrName, _T("userdata")) == 0) { SetUserData(pstrValue); }
    else if (_tcscmp(pstrName, _T("tag")) == 0)
    {
        if (*pstrValue == _T('0'))
        {
            pstrValue = ::CharNext(pstrValue);

            if (*pstrValue == _T('x') || *pstrValue == _T('X'))
            {
                pstrValue = ::CharNext(pstrValue);
                SetTag(_tcstoul(pstrValue, NULL, 16));
            }
            else
            {
                SetTag((UINT_PTR)_ttoi64(pstrValue));
            }
        }
        else { SetTag((UINT_PTR)_ttoi64(pstrValue)); }
    }
    else if (_tcscmp(pstrName, _T("enabled")) == 0) { SetEnabled(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("mouse")) == 0) { SetMouseEnabled(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("keyboard")) == 0) { SetKeyboardEnabled(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("visible")) == 0) { SetVisible(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("float")) == 0)
    {
        CDuiString nValue = pstrValue;

        if (nValue.Find(',') < 0)
        {
            SetFloat(_tcscmp(pstrValue, _T("true")) == 0);
        }
        else
        {
            TPercentInfo piFloatPercent = { 0 };
            LPTSTR pstr = NULL;
            piFloatPercent.left = _tcstod(pstrValue, &pstr);  ASSERT(pstr);
            piFloatPercent.top = _tcstod(pstr + 1, &pstr);    ASSERT(pstr);
            piFloatPercent.right = _tcstod(pstr + 1, &pstr);  ASSERT(pstr);
            piFloatPercent.bottom = _tcstod(pstr + 1, &pstr); ASSERT(pstr);
            SetFloatPercent(piFloatPercent);
            SetFloat(true);
        }
    }
    else if (_tcscmp(pstrName, _T("shortcut")) == 0) { SetShortcut(pstrValue); }
    else if (_tcscmp(pstrName, _T("menu")) == 0) { SetContextMenuUsed(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("virtualwnd")) == 0) { SetVirtualWnd(pstrValue); }
    else if (_tcscmp(pstrName, _T("weight")) == 0) { SetWeight(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("dropenable")) == 0) { m_bDropEnable = _tcscmp(pstrValue, _T("true")) == 0; }
    else if (_tcscmp(pstrName, _T("dragenable")) == 0) { m_bDragEnable = _tcscmp(pstrValue, _T("true")) == 0; }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { m_diDrag = pstrValue; }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { SetAutoWidth(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("autoheight")) == 0) { SetAutoHeight(_tcscmp(pstrValue, _T("true")) == 0); }
    // 2018-08-18 zhuyadong 添加特效
    else if (_tcscmp(pstrName, _T("triggerenter")) == 0)
    {
        BYTE byEffect = EFFECT_FLIPLEFT;
        WORD wFrequency = 150;
        bool bDirection = true, bLoop = false;
        ParseEffectInfo(pstrValue, byEffect, wFrequency, bDirection, bLoop);
        AddEffect(TRIGGER_ENTER, byEffect, wFrequency, bDirection, bLoop);
    }
    else if (_tcscmp(pstrName, _T("triggerleave")) == 0)
    {
        BYTE byEffect = EFFECT_FLIPLEFT;
        WORD wFrequency = 150;
        bool bDirection = true, bLoop = false;
        ParseEffectInfo(pstrValue, byEffect, wFrequency, bDirection, bLoop);
        AddEffect(TRIGGER_LEAVE, byEffect, wFrequency, bDirection, bLoop);
    }
    else if (_tcscmp(pstrName, _T("triggerclick")) == 0)
    {
        BYTE byEffect = EFFECT_FLIPLEFT;
        WORD wFrequency = 150;
        bool bDirection = true, bLoop = false;
        ParseEffectInfo(pstrValue, byEffect, wFrequency, bDirection, bLoop);
        AddEffect(TRIGGER_CLICK, byEffect, wFrequency, bDirection, bLoop);
    }
    else if (_tcscmp(pstrName, _T("triggershow")) == 0)
    {
        BYTE byEffect = EFFECT_FLIPLEFT;
        WORD wFrequency = 150;
        bool bDirection = true, bLoop = false;
        ParseEffectInfo(pstrValue, byEffect, wFrequency, bDirection, bLoop);
        AddEffect(TRIGGER_SHOW, byEffect, wFrequency, bDirection, bLoop);
    }
    else if (_tcscmp(pstrName, _T("triggerhide")) == 0)
    {
        BYTE byEffect = EFFECT_FLIPLEFT;
        WORD wFrequency = 150;
        bool bDirection = true, bLoop = false;
        ParseEffectInfo(pstrValue, byEffect, wFrequency, bDirection, bLoop);
        AddEffect(TRIGGER_HIDE, byEffect, wFrequency, bDirection, bLoop);
    }
    else if (_tcscmp(pstrName, _T("margin")) == 0)
    {
        RECT rcMargin = { 0 };
        LPTSTR pstr = NULL;
        rcMargin.left = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);
        rcMargin.top = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr);
        rcMargin.right = _tcstol(pstr + 1, &pstr, 10);  ASSERT(pstr);
        rcMargin.bottom = _tcstol(pstr + 1, &pstr, 10); ASSERT(pstr);
        SetMargin(rcMargin);
    }
    else { AddCustomAttribute(pstrName, pstrValue); }
}

CDuiString CControlUI::GetAttributeList(bool bIgnoreDefault)
{
    return _T("");
}

// 解决 TreeeNode 控件不能通过Default设置的问题
// horizattr, dotlineattr, folderattr, checkboxattr, itemattr 等复合属性的值，需要用 ? 包围起来
// 示例：   <Default name="TreeNode" value="text=&quot;abc&quot; folderattr=?width=&quot;16&quot; height=&quot;16&quot; ? " />
void CControlUI::SetAttributeList(LPCTSTR pstrList)
{
    if (NULL == pstrList) { return; }

    CDuiString sItem;
    CDuiString sValue;

    while (*pstrList != _T('\0'))
    {
        sItem.Empty();
        sValue.Empty();

        while (*pstrList != _T('\0') && *pstrList != _T('='))
        {
            LPTSTR pstrTemp = ::CharNext(pstrList);

            while (pstrList < pstrTemp)
            {
                sItem += *pstrList++;
            }
        }

        ASSERT(*pstrList == _T('='));

        if (*pstrList++ != _T('=')) { return; }

        ASSERT(*pstrList == _T('\"') || *pstrList == _T('?'));
        TCHAR ch = *pstrList++;

        if (ch != _T('\"') && ch != _T('?')) { return; }

        while (*pstrList != _T('\0') && *pstrList != ch)
        {
            LPTSTR pstrTemp = ::CharNext(pstrList);

            while (pstrList < pstrTemp)
            {
                sValue += *pstrList++;
            }
        }

        ASSERT(*pstrList == ch);

        if (*pstrList++ != ch) { return; }

        SetAttribute(sItem, sValue);

        if (*pstrList++ != _T(' ')) { return; }

        while (*pstrList == _T(' ')) { ++pstrList; }
    }
}

SIZE CControlUI::EstimateSize(SIZE szAvailable)
{
    return m_cxyFixed;
}

bool CControlUI::Paint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    if (pStopControl == this) { return false; }

    if (!::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem)) { return true; }

    if (OnPaint)
    {
        if (!OnPaint(this)) { return true; }
    }

    if (!DoPaint(hDC, rcPaint, pStopControl)) { return false; }

    if (m_pCover != NULL) { return m_pCover->Paint(hDC, rcPaint); }

    return true;
}

bool CControlUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    // 2018-08-18 zhuyadong 添加特效
    if (NULL != m_pEffect && m_pEffect->IsRunning(m_byEffectTrigger))
    {
        // 窗体显示特效：第一次走到这里，并非是特效，而是系统触发的绘制。应该过滤掉
        if (TRIGGER_SHOW == m_byEffectTrigger && 0 == m_pEffect->GetCurFrame(m_byEffectTrigger)) { return true; }

        BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        static PFunAlphaBlend spfAlphaBlend = GetAlphaBlend();
        spfAlphaBlend(hDC, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left,
                      m_rcItem.bottom - m_rcItem.top, m_pEffect->GetMemHDC(m_byEffectTrigger),
                      0, 0, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, bf);
        return true;
    }

    // 绘制循序：背景颜色->背景图->状态图->文本->边框
    if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0)
    {
        CRenderClip roundClip;
        CRenderClip::GenerateRoundClip(hDC, m_rcPaint, m_rcItem,
                                       m_cxyBorderRound.cx, m_cxyBorderRound.cy, roundClip);
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    else
    {
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintText(hDC);
        PaintBorder(hDC);
    }

    return true;
}

void CControlUI::PaintBkColor(HDC hDC)
{
    if (m_dwBackColor != 0)
    {
        if (m_dwBackColor2 != 0)
        {
            if (m_dwBackColor3 != 0)
            {
                RECT rc = m_rcItem;
                rc.left += m_rcBorderSize.left;
                rc.top += m_rcBorderSize.top;
                rc.right -= m_rcBorderSize.right;
                rc.bottom -= m_rcBorderSize.bottom;

                if (m_bDirection)
                {
                    // 水平渐变
                    LONG tmp = rc.right;
                    rc.right = (rc.right + rc.left) / 2;
                    CRenderEngine::DrawGradient(hDC, rc,
                                                GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2),
                                                false, 8);
                    rc.left = rc.right;
                    rc.right = tmp;
                    CRenderEngine::DrawGradient(hDC, rc,
                                                GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3),
                                                false, 8);
                }
                else
                {
                    // 垂直渐变
                    LONG tmp = rc.bottom = rc.bottom;
                    rc.bottom = (rc.bottom + rc.top) / 2;
                    CRenderEngine::DrawGradient(hDC, rc,
                                                GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2),
                                                true, 8);
                    rc.top = rc.bottom;
                    rc.bottom = tmp;
                    CRenderEngine::DrawGradient(hDC, rc,
                                                GetAdjustColor(m_dwBackColor2), GetAdjustColor(m_dwBackColor3),
                                                true, 8);
                }

            }
            else
            {
                CRenderEngine::DrawGradient(hDC, m_rcItem,
                                            GetAdjustColor(m_dwBackColor), GetAdjustColor(m_dwBackColor2),
                                            m_bDirection, 16);
            }
        }
        else if (m_dwBackColor >= 0xFF000000) { CRenderEngine::DrawColor(hDC, m_rcPaint, GetAdjustColor(m_dwBackColor)); }
        else { CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(m_dwBackColor)); }
    }
}

void CControlUI::PaintBkImage(HDC hDC)
{
    DrawImage(hDC, m_diBk);
}

void CControlUI::PaintStatusImage(HDC hDC)
{
    return;
}

void CControlUI::PaintText(HDC hDC)
{
    return;
}

void CControlUI::PaintBorder(HDC hDC)
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
    // int nBorderStyle = PS_INSIDEFRAME | PS_ENDCAP_SQUARE | PS_JOIN_BEVEL | PS_GEOMETRIC | m_nBorderStyle;

    if (m_rcBorderSize.left > 0)
    {
        rcBorder = m_rcItem;
        rcBorder.left += m_rcBorderSize.left / 2;
        rcBorder.right = rcBorder.left;

        CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.left, clrBorder, m_nBorderStyle);
    }

    if (m_rcBorderSize.top > 0)
    {
        rcBorder = m_rcItem;
        rcBorder.top += m_rcBorderSize.top / 2;
        rcBorder.bottom = rcBorder.top;
        rcBorder.left += m_rcBorderSize.top / 2;
        rcBorder.right -= m_rcBorderSize.right;

        CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.top, clrBorder, m_nBorderStyle);
    }

    if (m_rcBorderSize.right > 0)
    {
        rcBorder = m_rcItem;
        rcBorder.left = m_rcItem.right - m_rcBorderSize.right / 2 - m_rcBorderSize.right % 2;
        rcBorder.right = rcBorder.left;

        CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.right, clrBorder, m_nBorderStyle);
    }

    if (m_rcBorderSize.bottom > 0)
    {
        rcBorder = m_rcItem;
        rcBorder.top = m_rcItem.bottom - m_rcBorderSize.bottom / 2 - m_rcBorderSize.bottom % 2;
        rcBorder.bottom = rcBorder.top;
        rcBorder.left += m_rcBorderSize.bottom / 2;
        rcBorder.right -= m_rcBorderSize.right;

        CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.bottom, clrBorder, m_nBorderStyle);
    }

    //
    //if (m_rcBorderSize.left > 0 && (m_dwBorderColor != 0 || m_dwFocusBorderColor != 0))
    //{
    //    if (m_cxyBorderRound.cx > 0 || m_cxyBorderRound.cy > 0) //画圆角边框
    //    {
    //        if (IsFocused() && m_dwFocusBorderColor != 0)
    //        { CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_rcBorderSize.left, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle); }
    //        else
    //        { CRenderEngine::DrawRoundRect(hDC, m_rcItem, m_rcBorderSize.left, m_cxyBorderRound.cx, m_cxyBorderRound.cy, GetAdjustColor(m_dwBorderColor), m_nBorderStyle); }
    //    }
    //    else
    //    {
    //        if (m_rcBorderSize.right == m_rcBorderSize.left && m_rcBorderSize.top == m_rcBorderSize.left &&
    //            m_rcBorderSize.bottom == m_rcBorderSize.left)
    //        {
    //            if (IsFocused() && m_dwFocusBorderColor != 0)
    //            { CRenderEngine::DrawRect(hDC, m_rcItem, m_rcBorderSize.left, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle); }
    //            else
    //            { CRenderEngine::DrawRect(hDC, m_rcItem, m_rcBorderSize.left, GetAdjustColor(m_dwBorderColor), m_nBorderStyle); }
    //        }
    //        else
    //        {
    //            RECT rcBorder;
    //
    //            if (m_rcBorderSize.left > 0)
    //            {
    //                rcBorder        = m_rcItem;
    //                rcBorder.left  += m_rcBorderSize.left / 2;
    //                rcBorder.right  = rcBorder.left;
    //
    //                if (IsFocused() && m_dwFocusBorderColor != 0)
    //                { CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.left, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle); }
    //                else
    //                { CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.left, GetAdjustColor(m_dwBorderColor), m_nBorderStyle); }
    //            }
    //
    //            if (m_rcBorderSize.top > 0)
    //            {
    //                rcBorder        = m_rcItem;
    //                rcBorder.top   += m_rcBorderSize.top / 2;
    //                rcBorder.bottom = rcBorder.top;
    //                rcBorder.left  += m_rcBorderSize.left;
    //                rcBorder.right -= m_rcBorderSize.right;
    //
    //                if (IsFocused() && m_dwFocusBorderColor != 0)
    //                { CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.top, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle); }
    //                else
    //                { CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.top, GetAdjustColor(m_dwBorderColor), m_nBorderStyle); }
    //            }
    //
    //            if (m_rcBorderSize.right > 0)
    //            {
    //                rcBorder        = m_rcItem;
    //                rcBorder.left   = m_rcItem.right - m_rcBorderSize.right / 2;
    //                rcBorder.right  = rcBorder.left;
    //
    //                if (IsFocused() && m_dwFocusBorderColor != 0)
    //                { CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.right, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle); }
    //                else
    //                { CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.right, GetAdjustColor(m_dwBorderColor), m_nBorderStyle); }
    //            }
    //
    //            if (m_rcBorderSize.bottom > 0)
    //            {
    //                rcBorder        = m_rcItem;
    //                rcBorder.top    = m_rcItem.bottom - m_rcBorderSize.bottom / 2;
    //                rcBorder.bottom = rcBorder.top;
    //                rcBorder.left  += m_rcBorderSize.left;
    //                rcBorder.right -= m_rcBorderSize.right;
    //
    //                if (IsFocused() && m_dwFocusBorderColor != 0)
    //                { CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.bottom, GetAdjustColor(m_dwFocusBorderColor), m_nBorderStyle); }
    //                else
    //                { CRenderEngine::DrawLine(hDC, rcBorder, m_rcBorderSize.bottom, GetAdjustColor(m_dwBorderColor), m_nBorderStyle); }
    //            }
    //        }
    //    }
    //}
}

void CControlUI::DoPostPaint(HDC hDC, const RECT &rcPaint)
{
    if (OnPostPaint) { OnPostPaint(this); }
}

int CControlUI::GetBorderStyle() const
{
    return m_nBorderStyle;
}

void CControlUI::SetBorderStyle(int nStyle)
{
    m_nBorderStyle = nStyle;
    Invalidate();
}

RECT CControlUI::GetMargin() const
{
    return m_rcMargin;
}

void CControlUI::SetMargin(RECT rcMargin)
{
    m_rcMargin = rcMargin;
    NeedParentUpdate();
}

void CControlUI::SetAutoWidth(bool bAutoWidth)
{
    m_bAutoWidth = bAutoWidth;
}

bool CControlUI::GetAutoWidth(void)
{
    return m_bAutoWidth;
}

void CControlUI::SetAutoHeight(bool bAutoHeight)
{
    m_bAutoHeight = bAutoHeight;
}

bool CControlUI::GetAutoHeight(void)
{
    return m_bAutoHeight;
}

// 2018-08-18 zhuyadong 添加特效
bool CControlUI::AddEffect(BYTE byTrigger, BYTE byEffect, WORD wElapse, bool bDirection, bool bLoop)
{
    if (NULL == m_pEffect) { m_pEffect = new CEffectUI(this); }

    if (NULL == m_pEffect) { return false; }

    return m_pEffect->Add(byTrigger, byEffect, wElapse, bDirection, bLoop);
}

void CControlUI::DelEffect(BYTE byTrigger)
{
    if (NULL != m_pEffect)
    {
        m_pEffect->Del(byTrigger);
    }
}

bool CControlUI::StartEffect(BYTE byTrigger)
{
    if (NULL == m_pEffect || !m_pEffect->HasEffectTrigger(byTrigger)) { return false; }

    if (TRIGGER_NONE != m_byEffectTrigger) { StopEffect(); }

    bool bRet = m_pEffect->Start(byTrigger);

    if (bRet) { m_byEffectTrigger = byTrigger; }

    return bRet;
}

void CControlUI::StopEffect(void)
{
    if (NULL != m_pEffect)
    {
        m_pEffect->Stop(m_byEffectTrigger);
        m_byEffectTrigger = TRIGGER_NONE;
    }
}

DUI_INLINE BYTE CControlUI::GetEffectTrigger(void)
{
    return m_byEffectTrigger;
}

void CControlUI::OnEffectBegin(TAniParam &data)
{
    if (m_pParent) { m_pParent->Invalidate(); }
    else
    {
        HWND hParent = ::GetParent(GetManager()->GetPaintWindow());
        ::RedrawWindow(hParent, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
    }
}

void CControlUI::OnEffectEnd(TAniParam &data)
{
    // 触发控件隐藏
    if (TRIGGER_HIDE == m_byEffectTrigger)
    {
        if (m_pParent) { SetVisible(false);}
        else
        {
            // 窗体关闭特效，重新发送 WM_CLOSE 消息，带参数特效触发器
            ::PostMessage(m_pManager->GetPaintWindow(), WM_CLOSE, 0, m_byEffectTrigger);
        }
    }

    m_byEffectTrigger = TRIGGER_NONE;
    Invalidate();
}

void CControlUI::OnEffectDraw(TAniParam &data)
{
    ::RedrawWindow(GetManager()->GetPaintWindow(), &m_rcItem, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void CControlUI::SetCoverSize(LONG nPersent, bool bHorizontal, bool bTopLeft)
{
    RECT rt = m_rcItem;

    if (bHorizontal)
    {
        LONG nLen = m_rcItem.right - m_rcItem.left;
        nLen = nPersent * nLen / 100;   // Cover 新的大小

        if (bTopLeft) { rt.right = rt.left + nLen; }
        else          { rt.left = rt.right - nLen; }
    }
    else
    {
        LONG nLen = m_rcItem.bottom - m_rcItem.top;
        nLen = nPersent * nLen / 100;   // Cover 新的大小

        if (bTopLeft) { rt.bottom = rt.top + nLen; }
        else          { rt.top = rt.bottom - nLen; }
    }

    m_pCover->SetPos(rt);
}

} // namespace DuiLib
