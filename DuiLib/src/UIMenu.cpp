//2017-02-23 zhuyadong 添加菜单控件
#include "stdafx.h"
#include "UIShadow.h"

namespace DuiLib {

enum
{
    EMENU_CLOSE_ALL         = 1,    //关闭所有菜单
    EMENU_CLOSE_SUB         = 2,    //关闭子菜单
    //------------------------------------
    EMENU_ICON_WIDTH        = 26,   //菜单图标占用的最大宽度
    EMENU_EXPAND_WIDTH      = 20,   //下级扩展菜单图标占用的最大宽度
    EMENU_LINE_HEIGHT       = 6,    //菜单分割线默认高度
    EMENU_ITEM_MINHEIGHT    = 22,   // 菜单项最小高度
};

class CSubjectMenu : public CSubjectBase
{
public:
    CSubjectMenu(void) : m_pm(NULL), m_pMenuRoot(NULL) { SetNotifyDirection(false); }

    void SetManger(CPaintManagerUI *pm)
    {
        if (pm != NULL) { m_pm = pm; }
    }
    CPaintManagerUI *GetManager() const { return m_pm; }

protected:
private:
    CPaintManagerUI    *m_pm;
    CMenuUI            *m_pMenuRoot;
};

CSubjectMenu &GetGlobalMenuSubject()
{
    static CSubjectMenu s_cGlobalContextMenuSubject;
    return s_cGlobalContextMenuSubject;
}

/////////////////////////////////////////////////////////////////////////////////////
//
CDuiString CMenuWnd::s_strName;
CDuiString CMenuWnd::s_strUserData;
UINT_PTR   CMenuWnd::s_ptrTag;

CMenuWnd *CMenuWnd::CreateMenu(CMenuElementUI *pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT pt,
                               CPaintManagerUI *pParent, DWORD dwAlign)
{
    ASSERT(NULL != pOwner || NULL != pParent);
    CMenuWnd *pMenu = new CMenuWnd;
    pMenu->Init(pOwner, xml, pSkinType, pt, pParent, dwAlign);
    return pMenu;
}

CMenuWnd::CMenuWnd(void): m_pOwner(NULL), m_xml(_T(""))
{
    m_dwAlign = EMENU_ALIGN_LEFT | EMENU_ALIGN_TOP;
}

CControlUI *CMenuWnd::CreateControl(LPCTSTR pstrClassName)
{
    if (_tcsicmp(pstrClassName, DUI_CTR_MENU) == 0)
    {
        return new CMenuUI();
    }
    else if (_tcsicmp(pstrClassName, DUI_CTR_MENUELEMENT) == 0)
    {
        return new CMenuElementUI();
    }

    return NULL;
}

void CMenuWnd::OnSubjectUpdate(WPARAM p1, WPARAM p2, LPARAM p3, CSubjectBase *pSub)
{
    switch (p1)
    {
    case EMENU_CLOSE_ALL:
        Close();
        break;

    case EMENU_CLOSE_SUB:
        {
            HWND hParent = GetParent(m_hWnd);

            while (hParent != NULL)
            {
                if (hParent == (HWND)p2)
                {
                    Close();
                    break;
                }

                hParent = GetParent(hParent);
            }
        }
        break;

    default:
        break;
    }
}

CPaintManagerUI *CMenuWnd::GetManager(void)
{
    return &m_pm;
}

CMenuUI *CMenuWnd::GetMenuUI(void)
{
    return static_cast<CMenuUI *>(m_pm.GetRoot());
}

void CMenuWnd::ResizeMenu(void)
{
    CControlUI *pRoot = m_pm.GetRoot();
#if defined(WIN32) && !defined(UNDER_CE)
    MONITORINFO oMonitor = { };
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
#else
    CDuiRect rcWork;
    GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
    SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };
    szAvailable = pRoot->EstimateSize(szAvailable);
    RECT rtInset = GetMenuUI()->GetInset();
    szAvailable.cx += (rtInset.left + rtInset.right);
    szAvailable.cy += (rtInset.top + rtInset.bottom);
    m_pm.SetInitSize(szAvailable.cx, szAvailable.cy);

    DWORD dwAlignment = EMENU_ALIGN_LEFT | EMENU_ALIGN_TOP;

    SIZE szInit = m_pm.GetInitSize();
    CDuiRect rc;
    CDuiPoint point = m_ptBase;
    rc.left = point.x;
    rc.top = point.y;
    rc.right = rc.left + szInit.cx;
    rc.bottom = rc.top + szInit.cy;

    int nWidth = rc.GetWidth();
    int nHeight = rc.GetHeight();

    if (dwAlignment & EMENU_ALIGN_RIGHT)
    {
        rc.right = point.x;
        rc.left = rc.right - nWidth;
    }

    if (dwAlignment & EMENU_ALIGN_BOTTOM)
    {
        rc.bottom = point.y;
        rc.top = rc.bottom - nHeight;
    }

    SetForegroundWindow(m_hWnd);
    MoveWindow(m_hWnd, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), FALSE);
    SetWindowPos(m_hWnd, HWND_TOPMOST, rc.left, rc.top, rc.GetWidth(), rc.GetHeight(), SWP_SHOWWINDOW);
}

void CMenuWnd::ResizeSubMenu(void)
{
    // Position the popup window in absolute space
    if (NULL == m_pOwner) { return; }

    RECT rcOwner = m_pOwner->GetPos();
    RECT rc = rcOwner;

    int cxFixed = 0;
    int cyFixed = 0;

#if defined(WIN32) && !defined(UNDER_CE)
    MONITORINFO oMonitor = { };
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
#else
    CDuiRect rcWork;
    GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWork);
#endif
    SIZE szAvailable = { rcWork.right - rcWork.left, rcWork.bottom - rcWork.top };

    for (int it = 0; it < m_pOwner->GetCount(); it++)
    {
        if (m_pOwner->GetItemAt(it)->GetInterface(DUI_CTR_MENUELEMENT) != NULL)
        {
            CControlUI *pControl = static_cast<CControlUI *>(m_pOwner->GetItemAt(it));
            SIZE sz = pControl->EstimateSize(szAvailable);
            cyFixed += sz.cy;

            if (cxFixed < sz.cx)
            {
                cxFixed = sz.cx;
            }
        }
    }

    RECT rtInset = GetMenuUI()->GetInset();
    cxFixed += (rtInset.left + rtInset.right);
    cyFixed += (rtInset.top + rtInset.bottom);

    RECT rcWindow;
    GetWindowRect(m_pOwner->GetManager()->GetPaintWindow(), &rcWindow);

    rc.top = rcOwner.top;
    rc.bottom = rc.top + cyFixed;
    ::MapWindowRect(m_pOwner->GetManager()->GetPaintWindow(), HWND_DESKTOP, &rc);
    rc.left = rcWindow.right;
    rc.right = rc.left + cxFixed;
    // rc.right += 2;

    bool bReachBottom = false;
    bool bReachRight = false;
    LONG chRightAlgin = 0;
    LONG chBottomAlgin = 0;

    RECT rcPreWindow = { 0 };
    CSubjectMenu &cSub = GetGlobalMenuSubject();
    CMenuWnd *pMenu = dynamic_cast<CMenuWnd *>(cSub.GetFirst());

    for (; NULL != pMenu; pMenu = dynamic_cast<CMenuWnd *>(cSub.GetNext()))
    {
        GetWindowRect(pMenu->GetHWND(), &rcPreWindow);

        bReachRight = rcPreWindow.left >= rcWindow.right;
        bReachBottom = rcPreWindow.top >= rcWindow.bottom;

        if (pMenu->GetHWND() == m_pOwner->GetManager()->GetPaintWindow() || bReachRight || bReachBottom) { break; }
    }

    if (bReachBottom)
    {
        rc.bottom = rcWindow.top;
        rc.top = rc.bottom - cyFixed;
    }

    if (bReachRight)
    {
        rc.right = rcWindow.left;
        rc.left = rc.right - cxFixed;
    }

    if (rc.bottom > rcWork.bottom)
    {
        rc.bottom = rc.top;
        rc.top = rc.bottom - cyFixed;
    }

    if (rc.right > rcWork.right)
    {
        rc.right = rcWindow.left;
        rc.left = rc.right - cxFixed;

        //rc.top += 5;
        //rc.top = rcWindow.bottom;
        //rc.bottom = rc.top + cyFixed;
    }

    if (rc.top < rcWork.top)
    {
        rc.top = rcOwner.top;
        rc.bottom = rc.top + cyFixed;
    }

    if (rc.left < rcWork.left)
    {
        rc.left = rcWindow.right;
        rc.right = rc.left + cxFixed;
    }

    MoveWindow(m_hWnd, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, FALSE);
}

LPCTSTR CMenuWnd::GetWindowClassName(void) const
{
    return _T("MenuWnd");
}

void CMenuWnd::OnFinalMessage(HWND hWnd)
{
    GetGlobalMenuSubject().RemoveObserver(this);

    if (m_pOwner != NULL)
    {
        CMenuElementUI *pMenuItem = NULL;

        for (int i = 0; i < m_pOwner->GetCount(); i++)
        {
            pMenuItem = static_cast<CMenuElementUI *>(m_pOwner->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT));

            if (pMenuItem != NULL)
            {
                pMenuItem->SetOwner(m_pOwner->GetParent());
                pMenuItem->SetVisible(false);
                pMenuItem->SetInternVisible(false);
            }
        }

        m_pOwner->m_pSubMenuWnd = NULL;
        m_pOwner->m_uButtonState &= ~UISTATE_PUSHED;
        m_pOwner->Invalidate();
    }

    delete this;
}

void CMenuWnd::Init(CMenuElementUI *pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT ptClient,
                    CPaintManagerUI *pParent, DWORD dwAlign)
{
    m_ptBase  = ptClient;
    m_pOwner  = pOwner;
    m_xml     = xml;
    m_dwAlign = dwAlign;

    if (pSkinType != NULL) { m_sSkinType = pSkinType; }

    // 如果是一级菜单的创建
    if (pOwner == NULL)
    {
        ASSERT(pParent != NULL);
        GetGlobalMenuSubject().SetManger(pParent);
    }

    GetGlobalMenuSubject().AddObserver(this);
    Create((m_pOwner == NULL) ? pParent->GetPaintWindow() : m_pOwner->GetManager()->GetPaintWindow(),
           NULL, WS_POPUP, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, CDuiRect());
    // HACK: Don't deselect the parent's caption
    HWND hWndParent = m_hWnd;

    while (::GetParent(hWndParent) != NULL) { hWndParent = ::GetParent(hWndParent); }

    ::ShowWindow(m_hWnd, SW_SHOW);
#if defined(WIN32) && !defined(UNDER_CE)
    ::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
#endif
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch (uMsg)
    {
    case WM_CREATE:             lRes = OnCreate(uMsg, wParam, lParam, bHandled);    break;

    case WM_CLOSE:              lRes = OnClose(uMsg, wParam, lParam, bHandled);     break;

    case WM_KILLFOCUS:          lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;

    case WM_SIZE:               lRes = OnSize(uMsg, wParam, lParam, bHandled);      break;

    case WM_KEYDOWN:            lRes = OnKeyDown(uMsg, wParam, lParam, bHandled);   break;

    case WM_RBUTTONDOWN:
    case WM_CONTEXTMENU:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
        break;

    default:
        bHandled = FALSE;
        break;
    }

    if (bHandled) { return lRes; }

    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) { return lRes; }

    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CMenuWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bool bShowShadow = false;

    if (m_pOwner != NULL)
    {
        LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
        styleValue &= ~WS_CAPTION;
        ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        RECT rcClient;
        ::GetClientRect(*this, &rcClient);
        ::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
                       rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

        m_pm.Init(m_hWnd);
        m_pm.SetForceUseSharedRes(true);
        // The trick is to add the items to the new container. Their owner gets
        // reassigned by this operation - which is why it is important to reassign
        // the items back to the righfull owner/manager when the window closes.
        CMenuUI *pLayout = new CMenuUI();
        pLayout->SetManager(&m_pm, NULL, true);
        pLayout->SetAutoDestroy(false);
        pLayout->EnableScrollBar();
        {
            LPCTSTR pDefAttrMenu = m_pOwner->GetManager()->GetDefaultAttributeList(DUI_CTR_MENU);
            LPCTSTR pDefAttrItem = m_pOwner->GetManager()->GetDefaultAttributeList(DUI_CTR_MENUELEMENT);
            CMenuElementUI *pItem = NULL;

            if (pDefAttrMenu) { pLayout->SetAttributeList(pDefAttrMenu); }

            for (int i = 0; i < m_pOwner->GetCount(); i++)
            {
                pItem = static_cast<CMenuElementUI *>(m_pOwner->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT));

                if (NULL != pItem)
                {
                    if (pDefAttrItem) { pItem->SetAttributeList(pDefAttrItem); }

                    pItem->SetOwner(pLayout);
                    pLayout->Add(static_cast<CControlUI *>(pItem));
                }
            }
        }

        CShadowUI *pShadow = m_pm.GetShadow();
        m_pOwner->GetManager()->GetShadow()->CopyShadow(pShadow);
        bShowShadow = pShadow->IsShow();
        pShadow->SetShow(false);

        m_pm.AttachDialog(pLayout);
        m_pm.SetLayered(m_pOwner->GetManager()->IsLayered());
        m_pm.AddNotifier(this);
        ResizeSubMenu();
    }
    else
    {
        m_pm.Init(m_hWnd);
        m_pm.SetForceUseSharedRes(true);
        CDialogBuilder builder;
        CControlUI *pRoot = builder.Create(m_xml, m_sSkinType.GetData(), this, &m_pm);
        ASSERT(pRoot);

        CShadowUI *pShadow = m_pm.GetShadow();
        bShowShadow = pShadow->IsShow();
        pShadow->SetShow(false);

        m_pm.AttachDialog(pRoot);
        m_pm.AddNotifier(this);
        ResizeMenu();
    }

    m_pm.GetShadow()->SetShow(bShowShadow);
    m_pm.GetShadow()->Create(&m_pm);
    GetManager()->SetFocus(GetMenuUI());
    return 0;
}

LRESULT CMenuWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (m_pOwner != NULL)
    {
        m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent(), false);
        m_pOwner->SetPos(m_pOwner->GetPos(), false);
        m_pOwner->SetFocus();
    }

    bHandled = FALSE;
    return 0;
}

LRESULT CMenuWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    HWND hFocusWnd = (HWND)wParam;
    BOOL bInMenuWindowList = FALSE;

    CSubjectMenu &cSub = GetGlobalMenuSubject();
    CMenuWnd *pMenu = dynamic_cast<CMenuWnd *>(cSub.GetFirst());

    for (; NULL != pMenu; pMenu = dynamic_cast<CMenuWnd *>(cSub.GetNext()))
    {
        if (pMenu->GetHWND() == hFocusWnd)
        {
            bInMenuWindowList = TRUE;
            break;
        }
    }

    if (!bInMenuWindowList)
    {
        GetGlobalMenuSubject().NotifyObserver(EMENU_CLOSE_ALL, (WPARAM)GetHWND());
    }
    else
    {
        bHandled = FALSE;
    }

    return 0;
}

LRESULT CMenuWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    SIZE szRoundCorner = m_pm.GetRoundCorner();

    if (!::IsIconic(*this))
    {
        CDuiRect rcWnd;
        ::GetWindowRect(*this, &rcWnd);
        rcWnd.Offset(-rcWnd.left, -rcWnd.top);
        rcWnd.right++;
        rcWnd.bottom++;
        HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom,
                                         szRoundCorner.cx, szRoundCorner.cy);
        ::SetWindowRgn(*this, hRgn, TRUE);
        ::DeleteObject(hRgn);
    }

    bHandled = FALSE;
    return 0;
}

LRESULT CMenuWnd::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (wParam == VK_ESCAPE || wParam == VK_LEFT) { Close(); }

    bHandled = FALSE;
    return 0;
}

/////////////////////////////////////////////////////////////////////////////////////
//
CMenuUI::CMenuUI(void)
{
    if (GetHeader() != NULL) { GetHeader()->SetVisible(false); }
}

CMenuUI::~CMenuUI(void)
{
}

CMenuElementUI *CMenuUI::NewMenuItem(void)
{
    CMenuElementUI *pItem = new CMenuElementUI();
    pItem->SetAttributeList(m_pManager->GetDefaultAttributeList(DUI_CTR_MENUELEMENT));
    return pItem;
}

LPCTSTR CMenuUI::GetClass(void) const
{
    return DUI_CTR_MENU;
}

LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_MENU) == 0) { return static_cast<CMenuUI *>(this); }

    return CListUI::GetInterface(pstrName);
}

void CMenuUI::DoEvent(TEventUI &event)
{
    return CListUI::DoEvent(event);
}

bool CMenuUI::Add(CControlUI *pControl)
{
    CMenuElementUI *pItemList = static_cast<CMenuElementUI *>(pControl->GetInterface(DUI_CTR_MENUELEMENT));
    CMenuElementUI *pItem = NULL;

    if (pItemList == NULL) { return false; }

    for (int i = 0; i < pItemList->GetCount(); ++i)
    {
        pItem = static_cast<CMenuElementUI *>(pItemList->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT));

        if (pItem != NULL) { pItem->SetInternVisible(false); }
    }

    return CListUI::Add(pControl);
}

bool CMenuUI::AddAt(CControlUI *pControl, int iIndex)
{
    CMenuElementUI *pItemList = static_cast<CMenuElementUI *>(pControl->GetInterface(DUI_CTR_MENUELEMENT));
    CMenuElementUI *pItem = NULL;

    if (pItemList == NULL) { return false; }

    for (int i = 0; i < pItemList->GetCount(); ++i)
    {
        pItem = static_cast<CMenuElementUI *>(pItemList->GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT));

        if (pItem != NULL) { pItem->SetInternVisible(false); }
    }

    return CListUI::AddAt(pControl, iIndex);
}

bool CMenuUI::Remove(CControlUI *pControl, bool bDoNotDestroy)
{
    return (NULL != pControl->GetInterface(DUI_CTR_MENUELEMENT)) ? CListUI::Remove(pControl,
            bDoNotDestroy) : false;
}

int CMenuUI::GetItemIndex(CControlUI *pControl) const
{
    return (NULL != pControl->GetInterface(DUI_CTR_MENUELEMENT)) ? CListUI::GetItemIndex(pControl) : -1;
}

bool CMenuUI::SetItemIndex(CControlUI *pControl, int iIndex)
{
    return (NULL != pControl->GetInterface(DUI_CTR_MENUELEMENT)) ? CListUI::SetItemIndex(pControl,
            iIndex) : false;
}

CMenuElementUI *CMenuUI::FindMenuItem(LPCTSTR pstrName)
{
    return dynamic_cast<CMenuElementUI *>(GetManager()->FindControl(pstrName));
}

SIZE CMenuUI::EstimateSize(SIZE szAvailable)
{
    int cxFixed = 0;
    int cyFixed = 0;

    for (int it = 0; it < GetCount(); it++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(GetItemAt(it));

        if (!pControl->IsVisible()) { continue; }

        SIZE sz = pControl->EstimateSize(szAvailable);
        cyFixed += sz.cy;

        if (cxFixed < sz.cx) { cxFixed = sz.cx; }
    }

    return CDuiSize(cxFixed, cyFixed);
}

void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    CListUI::SetAttribute(pstrName, pstrValue);
}

/////////////////////////////////////////////////////////////////////////////////////
//
static CMenuElementUI *s_pHotItem = NULL;   // 当前鼠标所在的菜单项的指针

CMenuElementUI::CMenuElementUI(void) : m_pSubMenuWnd(NULL), m_bCheckItem(false), m_bChecked(false),
    m_wIconWidth(EMENU_ICON_WIDTH), m_wExpandWidth(EMENU_EXPAND_WIDTH), m_bLine(false), m_dwLineColor(0XFFBCBFC4)
{
    m_cxyFixed.cy = EMENU_ITEM_MINHEIGHT;
    m_bMouseChildEnabled = true;

    m_diIconNormal.Clear();
    m_diIconChecked.Clear();
    m_diExpandIcon.Clear();
}

CMenuElementUI::~CMenuElementUI(void)
{
}

CMenuElementUI *CMenuElementUI::NewMenuItem(void)
{
    CMenuUI *pMenu = dynamic_cast<CMenuUI *>(GetOwner());

    if (NULL != pMenu)
    {
        CMenuElementUI *pItem = pMenu->NewMenuItem();
        ASSERT(NULL != pItem);
        pItem->SetInternVisible(false);
        return pItem;
    }

    return NULL;
}

LPCTSTR CMenuElementUI::GetClass(void) const
{
    return DUI_CTR_MENUELEMENT;
}

LPVOID CMenuElementUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcsicmp(pstrName, DUI_CTR_MENUELEMENT) == 0) { return static_cast<CMenuElementUI *>(this); }

    return CListContainerElementUI::GetInterface(pstrName);
}

bool CMenuElementUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    RECT rcTemp = { 0 };

    if (!::IntersectRect(&rcTemp, &rcPaint, &m_rcItem)) { return true; }

    if (m_bLine)
    {
        RECT rcLine = { m_rcItem.left + m_wIconWidth, m_rcItem.top + m_cxyFixed.cy / 2,
                        m_rcItem.right, m_rcItem.top + m_cxyFixed.cy / 2
                      };
        CRenderEngine::DrawLine(hDC, rcLine, 1, m_dwLineColor);
        return true;
    }

    CRenderClip clip;
    CRenderClip::GenerateClip(hDC, m_rcItem, clip);
    CMenuElementUI::DrawItemBk(hDC, m_rcItem);
    DrawItemText(hDC, m_rcItem);
    DrawItemIcon(hDC, m_rcItem);
    DrawItemExpand(hDC, m_rcItem);

    if (m_items.GetSize() > 0)
    {
        RECT rc = m_rcItem;
        rc.left += m_rcInset.left;
        rc.top += m_rcInset.top;
        rc.right -= m_rcInset.right;
        rc.bottom -= m_rcInset.bottom;

        if (m_pVerticalScrollBar && m_pVerticalScrollBar->IsVisible()) { rc.right -= m_pVerticalScrollBar->GetFixedWidth(); }

        if (m_pHorizontalScrollBar && m_pHorizontalScrollBar->IsVisible()) { rc.bottom -= m_pHorizontalScrollBar->GetFixedHeight(); }

        if (!::IntersectRect(&rcTemp, &rcPaint, &rc))
        {
            for (int it = 0; it < m_items.GetSize(); it++)
            {
                CControlUI *pControl = static_cast<CControlUI *>(m_items[it]);

                if (pControl == pStopControl) { return false; }

                if (!pControl->IsVisible()) { continue; }

                if (pControl->GetInterface(DUI_CTR_MENUELEMENT) != NULL) { continue; }

                if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) { continue; }

                if (pControl->IsFloat())
                {
                    if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) { continue; }

                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) { return false; }
                }
            }
        }
        else
        {
            CRenderClip childClip;
            CRenderClip::GenerateClip(hDC, rcTemp, childClip);

            for (int it = 0; it < m_items.GetSize(); it++)
            {
                CControlUI *pControl = static_cast<CControlUI *>(m_items[it]);

                if (pControl == pStopControl) { return false; }

                if (!pControl->IsVisible()) { continue; }

                if (pControl->GetInterface(DUI_CTR_MENUELEMENT) != NULL) { continue; }

                if (!::IntersectRect(&rcTemp, &rcPaint, &pControl->GetPos())) { continue; }

                if (pControl->IsFloat())
                {
                    if (!::IntersectRect(&rcTemp, &m_rcItem, &pControl->GetPos())) { continue; }

                    CRenderClip::UseOldClipBegin(hDC, childClip);

                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) { return false; }

                    CRenderClip::UseOldClipEnd(hDC, childClip);
                }
                else
                {
                    if (!::IntersectRect(&rcTemp, &rc, &pControl->GetPos())) { continue; }

                    if (!pControl->Paint(hDC, rcPaint, pStopControl)) { return false; }
                }
            }
        }
    }

    if (m_pVerticalScrollBar != NULL)
    {
        if (m_pVerticalScrollBar == pStopControl) { return false; }

        if (m_pVerticalScrollBar->IsVisible())
        {
            if (::IntersectRect(&rcTemp, &rcPaint, &m_pVerticalScrollBar->GetPos()))
            {
                if (!m_pVerticalScrollBar->Paint(hDC, rcPaint, pStopControl)) { return false; }
            }
        }
    }

    if (m_pHorizontalScrollBar != NULL)
    {
        if (m_pHorizontalScrollBar == pStopControl) { return false; }

        if (m_pHorizontalScrollBar->IsVisible())
        {
            if (::IntersectRect(&rcTemp, &rcPaint, &m_pHorizontalScrollBar->GetPos()))
            {
                if (!m_pHorizontalScrollBar->Paint(hDC, rcPaint, pStopControl)) { return false; }
            }
        }
    }

    return true;
}

SIZE CMenuElementUI::EstimateSize(SIZE szAvailable)
{
    SIZE cXY = { 0 };

    for (int it = 0; it < GetCount(); it++)
    {
        CControlUI *pControl = static_cast<CControlUI *>(GetItemAt(it));

        if (!pControl->IsVisible()) { continue; }

        SIZE sz = pControl->EstimateSize(szAvailable);
        cXY.cy += sz.cy;

        if (cXY.cx < sz.cx) { cXY.cx = sz.cx; }
    }

    if (cXY.cy == 0)
    {
        TListInfoUI *pInfo = m_pOwner->GetListInfo();

        DWORD iTextColor = pInfo->dwTextColor;

        if ((m_uButtonState & UISTATE_HOT) != 0)
        {
            iTextColor = pInfo->dwHotTextColor;
        }

        if (IsChecked())
        {
            iTextColor = pInfo->dwSelectedTextColor;
        }

        if (!IsEnabled())
        {
            iTextColor = pInfo->dwDisabledTextColor;
        }

        RECT rcText = { 0, 0, std::max<int>(szAvailable.cx, m_cxyFixed.cx), 9999 };
        rcText.left += pInfo->rcTextPadding.left;
        rcText.right -= pInfo->rcTextPadding.right;

        if (pInfo->bShowHtml)
        {
            int nLinks = 0;
            CRenderEngine::DrawHtmlText(GetManager()->GetPaintDC(), GetManager(), rcText, m_sText, iTextColor,
                                        NULL, NULL, nLinks, pInfo->nFont,
                                        DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
        }
        else
        {
            CRenderEngine::DrawText(GetManager()->GetPaintDC(), GetManager(), rcText, m_sText, iTextColor,
                                    pInfo->nFont, DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
        }

        cXY.cy = rcText.bottom - rcText.top + pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
        cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right + 20;
    }

    if (m_cxyFixed.cy != 0) { cXY.cy = m_cxyFixed.cy; }

    return cXY;
}

void CMenuElementUI::DoEvent(TEventUI &event)
{
    switch (event.Type)
    {
    case UIEVENT_TIMER:             OnTimer(event);                             break;

    case UIEVENT_MOUSEENTER:        OnMouseEnter(event);                        break;

    case UIEVENT_MOUSELEAVE:        OnMouseLeave(event);                        break;

    case UIEVENT_BUTTONUP:          OnLButtonUp(event);                         break;

    case UIEVENT_KEYDOWN:           OnKeyDown(event);                           break;

    default:                        CListContainerElementUI::DoEvent(event);    break;
    }
}

void CMenuElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("iconwidth")) == 0)
    {
        SetIconWidth(_ttoi(pstrValue));
    }
    else if (_tcscmp(pstrName, _T("iconnormal")) == 0)
    {
        SetIconNormal(pstrValue);
    }
    else if (_tcscmp(pstrName, _T("iconchecked")) == 0)
    {
        SetIconChecked(pstrValue);
    }
    else if (_tcscmp(pstrName, _T("checkitem")) == 0)
    {
        SetCheckItem(_tcscmp(pstrValue, _T("true")) == 0 ? true : false);
    }
    else if (_tcscmp(pstrName, _T("checked")) == 0)
    {
        SetCheck(_tcscmp(pstrValue, _T("true")) == 0 ? true : false);
    }
    else if (_tcscmp(pstrName, _T("expandwidth")) == 0)
    {
        SetExpandWidth(_ttoi(pstrValue));
    }
    else if (_tcscmp(pstrName, _T("expandicon")) == 0)
    {
        SetExpandIcon(pstrValue);
    }
    else if (_tcscmp(pstrName, _T("line")) == 0)
    {
        if (_tcscmp(pstrValue, _T("true")) == 0) { SetLine(); }
    }
    else if (_tcscmp(pstrName, _T("linecolor")) == 0)
    {
        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        SetLineColor(_tcstoul(pstrValue, &pstr, 16));
    }
    else if (_tcscmp(pstrName, _T("height")) == 0)
    {
        int nHeight = _ttoi(pstrValue);
        nHeight = (nHeight < EMENU_ITEM_MINHEIGHT) ? EMENU_ITEM_MINHEIGHT : nHeight;
        SetFixedHeight(nHeight);
    }
    else
    {
        CListContainerElementUI::SetAttribute(pstrName, pstrValue);
    }
}

void CMenuElementUI::SetAttributeList(LPCTSTR pstrList)
{
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

        ASSERT(*pstrList == _T('\"'));

        if (*pstrList++ != _T('\"')) { return; }

        while (*pstrList != _T('\0') && *pstrList != _T('\"'))
        {
            LPTSTR pstrTemp = ::CharNext(pstrList);

            while (pstrList < pstrTemp)
            {
                sValue += *pstrList++;
            }
        }

        ASSERT(*pstrList == _T('\"'));

        if (*pstrList++ != _T('\"')) { return; }

        SetAttribute(sItem, sValue);

        if (*pstrList++ != _T(' ')) { return; }
    }
}

void CMenuElementUI::DrawItemIcon(HDC hDC, const RECT &rcItem)
{
    RECT rtIcon;
    rtIcon.left = rcItem.left;
    rtIcon.right = std::min<int>(m_wIconWidth, m_cxyFixed.cy);
    rtIcon.top = rcItem.top + (m_cxyFixed.cy - rtIcon.right) / 2;
    rtIcon.bottom = rtIcon.top + rtIcon.right;
    rtIcon.right += rtIcon.left;

    if (!m_bCheckItem)
    {
        if (!m_diIconNormal.sDrawString.IsEmpty())
        {
            CRenderEngine::DrawImage(hDC, m_pManager, rtIcon, m_rcPaint, m_diIconNormal);
        }
    }
    else
    {
        if (IsChecked())
        {
            CRenderEngine::DrawImage(hDC, m_pManager, rtIcon, m_rcPaint, m_diIconChecked);
        }
        else
        {
            CRenderEngine::DrawImage(hDC, m_pManager, rtIcon, rtIcon, m_diIconNormal);
        }
    }
}

void CMenuElementUI::DrawItemText(HDC hDC, const RECT &rcItem)
{
    if (m_sText.IsEmpty()) { return; }

    if (m_pOwner == NULL) { return; }

    TListInfoUI *pInfo = m_pOwner->GetListInfo();
    DWORD iTextColor = pInfo->dwTextColor;

    if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        iTextColor = pInfo->dwHotTextColor;
    }

    if (IsSelected())
    {
        iTextColor = pInfo->dwSelectedTextColor;
    }

    if (!IsEnabled())
    {
        iTextColor = pInfo->dwDisabledTextColor;
    }

    int nLinks = 0;
    RECT rcText = rcItem;
    rcText.left += pInfo->rcTextPadding.left;
    rcText.right -= pInfo->rcTextPadding.right;
    rcText.top += pInfo->rcTextPadding.top;
    rcText.bottom -= pInfo->rcTextPadding.bottom;

    if (pInfo->bShowHtml)
    {
        CRenderEngine::DrawHtmlText(hDC, GetManager(), rcText, m_sText, iTextColor,
                                    NULL, NULL, nLinks, pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
    }
    else
    {
        CRenderEngine::DrawText(hDC, GetManager(), rcText, m_sText, iTextColor,
                                pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);
    }
}

void CMenuElementUI::DrawItemExpand(HDC hDC, const RECT &rcItem)
{
    if (IsExpandable())
    {
        RECT rtDest = { rcItem.right - m_wExpandWidth, rcItem.top, rcItem.right, rcItem.bottom };
        CRenderEngine::DrawImage(hDC, GetManager(), rtDest, m_rcPaint, m_diExpandIcon);
    }
}

void CMenuElementUI::SetIconWidth(WORD wWidth)
{
    m_wIconWidth = wWidth;
    Invalidate();
}

WORD CMenuElementUI::GetIconWidth(void) const
{
    return m_wIconWidth;
}

void CMenuElementUI::SetIconNormal(LPCTSTR pstrIcon)
{
    if (m_diIconNormal.sDrawString == pstrIcon && m_diIconNormal.pImageInfo != NULL) { return; }

    m_diIconNormal.Clear();
    m_diIconNormal.sDrawString = pstrIcon;
    Invalidate();
}

void CMenuElementUI::SetIconChecked(LPCTSTR pstrIcon)
{
    if (m_diIconChecked.sDrawString == pstrIcon && m_diIconChecked.pImageInfo != NULL) { return; }

    m_diIconChecked.Clear();
    m_diIconChecked.sDrawString = pstrIcon;
    Invalidate();
}

void CMenuElementUI::SetCheckItem(bool bCheckItem)
{
    m_bCheckItem = bCheckItem;
}

bool CMenuElementUI::IsCheckItem(void) const
{
    return m_bCheckItem;
}

void CMenuElementUI::SetCheck(bool bChecked)
{
    if (!m_bCheckItem) { return; }

    m_bChecked = bChecked;
}

bool CMenuElementUI::IsChecked(void) const
{
    return m_bChecked;
}

void CMenuElementUI::SetExpandWidth(WORD wWidth)
{
    m_wExpandWidth = wWidth;
    Invalidate();
}

WORD CMenuElementUI::GetExpandWidth(void) const
{
    return m_wExpandWidth;
}

void CMenuElementUI::SetExpandIcon(LPCTSTR pstrIcon)
{
    if (m_diExpandIcon.sDrawString == pstrIcon && m_diExpandIcon.pImageInfo != NULL) { return; }

    m_diExpandIcon.Clear();
    m_diExpandIcon.sDrawString = pstrIcon;
    Invalidate();
}

void CMenuElementUI::SetLine(void)
{
    m_bLine = true;
    SetMouseChildEnabled(false);
    SetMouseEnabled(false);
    SetEnabled(false);

    if (GetFixedHeight() != EMENU_LINE_HEIGHT) { SetFixedHeight(EMENU_LINE_HEIGHT); }
}

bool CMenuElementUI::IsLine(void) const
{
    return m_bLine;
}

void CMenuElementUI::SetLineColor(DWORD color)
{
    m_dwLineColor = color;
}

DWORD CMenuElementUI::GetLineColor(void) const
{
    return m_dwLineColor;
}

void CMenuElementUI::OnTimer(TEventUI &event)
{
    if (TIMERID_MOUSEENTER == event.wParam)
    {
        GetManager()->KillTimer(this, TIMERID_MOUSEENTER);

        if (m_pSubMenuWnd || s_pHotItem != this) { return; }

        // 判断是否有子菜单
        bool bHasSubMenu = false;

        for (int nIdx = 0; nIdx < GetCount(); ++nIdx)
        {
            // 查找是否：有子菜单，并且没有展开
            CMenuElementUI *pItem = static_cast<CMenuElementUI *>(GetItemAt(nIdx)->GetInterface(DUI_CTR_MENUELEMENT));

            if (NULL != pItem)
            {
                pItem->SetVisible(true);
                pItem->SetInternVisible(true);
                bHasSubMenu = true;
            }
        }

        // 关闭当前菜单窗口的子菜单
        GetGlobalMenuSubject().NotifyObserver(EMENU_CLOSE_SUB, (WPARAM)GetManager()->GetPaintWindow());

        // 满足这两个条件时创建子菜单窗口：1.鼠标在当前菜单项上 2.当前菜单项有子窗口
        if (bHasSubMenu && ::PtInRect(&m_rcItem, GetManager()->GetMousePos()))
        {
            m_pSubMenuWnd = CMenuWnd::CreateMenu(this, _T(""), NULL, CDuiPoint(), NULL);
            ASSERT(m_pSubMenuWnd);
        }
    }
    else if (TIMERID_MOUSELEAVE == event.wParam)
    {
        // 鼠标不在子菜单窗口内时，关闭子菜单窗口
        GetManager()->KillTimer(this, TIMERID_MOUSELEAVE);

        POINT ptMouse = GetManager()->GetMousePos();
        RECT rtSubMenuWnd;
        ::GetClientRect(GetManager()->GetPaintWindow(), &rtSubMenuWnd);

        if (::PtInRect(&rtSubMenuWnd, ptMouse))
        {
            // 鼠标在当前菜单窗口，关闭当前菜单可能存在的子菜单窗口
            GetGlobalMenuSubject().NotifyObserver(EMENU_CLOSE_SUB, (WPARAM)GetManager()->GetPaintWindow());
            return;
        }

        if (NULL == m_pSubMenuWnd)
        {
            // 鼠标不在当前菜单窗口，并且没有子菜单窗口，关闭当前菜单可能存在的子菜单窗口，取消选择菜单项
            if (GetOwner()) { GetOwner()->SelectItem(-1, false); }

            GetGlobalMenuSubject().NotifyObserver(EMENU_CLOSE_SUB, (WPARAM)GetManager()->GetPaintWindow());
            return;
        }

        ptMouse = m_pSubMenuWnd->GetManager()->GetMousePos();
        ::GetClientRect(*m_pSubMenuWnd, &rtSubMenuWnd);

        if (::PtInRect(&rtSubMenuWnd, ptMouse))
        {
            // 鼠标处于子菜单窗口，设置当前菜单项为选择状态，不关闭子菜单窗口
            if (GetOwner()) { GetOwner()->SelectItem(GetIndex(), false); }
        }
        else
        {
            // 鼠标不在任何菜单窗口上
            GetGlobalMenuSubject().NotifyObserver(EMENU_CLOSE_SUB, (WPARAM)GetManager()->GetPaintWindow());

            if (GetOwner()) { GetOwner()->SelectItem(-1, false); }
        }
    }
}

void CMenuElementUI::OnMouseEnter(TEventUI &event)
{
    s_pHotItem = this;
    m_pOwner->SelectItem(GetIndex(), true);
    GetManager()->SetTimer(this, TIMERID_MOUSEENTER, 500);

    CListContainerElementUI::DoEvent(event);
}

void CMenuElementUI::OnMouseLeave(TEventUI &event)
{
    if (NULL != s_pHotItem)
    {
        s_pHotItem->GetManager()->KillTimer(s_pHotItem, TIMERID_MOUSEENTER);
        s_pHotItem = NULL;
    }

    // 子菜单窗口保持条件：鼠标移动到子菜单窗口
    GetManager()->SetTimer(this, TIMERID_MOUSELEAVE, 100);
    CListContainerElementUI::DoEvent(event);
}

void CMenuElementUI::OnLButtonUp(TEventUI &event)
{
    CListContainerElementUI::DoEvent(event);

    if (m_pSubMenuWnd || !IsEnabled()) { return; }

    bool hasSubMenu = false;
    CMenuElementUI *pItem = NULL;

    for (int i = 0; i < GetCount(); ++i)
    {
        pItem = static_cast<CMenuElementUI *>(GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT));

        if (pItem != NULL)
        {
            pItem->SetVisible(true);
            pItem->SetInternVisible(true);
            hasSubMenu = true;
        }
    }

    if (hasSubMenu)
    {
        m_pSubMenuWnd = CMenuWnd::CreateMenu(this, _T(""), NULL, CDuiPoint(), NULL);
        ASSERT(m_pSubMenuWnd);
    }
    else
    {
        SetCheck(!IsChecked());
        CMenuWnd::s_strName = GetName();
        CMenuWnd::s_strUserData = GetUserData();
        CMenuWnd::s_ptrTag = GetTag();
        CPaintManagerUI *pManager = GetGlobalMenuSubject().GetManager();

        if (NULL != pManager)
        {
            HWND hWnd = pManager->GetPaintWindow();
            PostMessage(hWnd, WM_MENUITEM_CLICK, NULL, (LPARAM)(IsChecked() ? TRUE : FALSE));
            TNotifyUI msg;
            msg.pSender = NULL;
            msg.dwTimestamp = 0;
            msg.ptMouse.x = msg.ptMouse.y = 0;
            msg.sType = DUI_MSGTYPE_MENUITEM_CLICK;
            msg.wParam = (WPARAM)&CMenuWnd::s_strName;
            msg.lParam = (LPARAM)CMenuWnd::s_ptrTag;
            pManager->SendNotify(msg, true);
        }

        GetGlobalMenuSubject().NotifyObserver(EMENU_CLOSE_ALL, (WPARAM)GetManager()->GetPaintWindow());
    }
}

//支  持：左箭头收起子菜单，右箭头展开子菜单
//不支持：上/下箭头切换菜单项焦点
void CMenuElementUI::OnKeyDown(TEventUI &event)
{
    CListContainerElementUI::DoEvent(event);

    if (!m_pSubMenuWnd)
    {
        bool hasSubMenu = false;
        CMenuElementUI *pItem = NULL;

        for (int i = 0; i < GetCount(); ++i)
        {
            pItem = static_cast<CMenuElementUI *>(GetItemAt(i)->GetInterface(DUI_CTR_MENUELEMENT));

            if (pItem != NULL)
            {
                pItem->SetVisible(true);
                pItem->SetInternVisible(true);
                hasSubMenu = true;
            }
        }

        if (hasSubMenu && (event.chKey == VK_RIGHT || event.chKey == VK_RETURN))
        {
            m_pOwner->SelectItem(GetIndex(), true);
            m_pSubMenuWnd = CMenuWnd::CreateMenu(this, _T(""), NULL, CDuiPoint(), NULL);
            ASSERT(m_pSubMenuWnd);
            ::Sleep(1);
            m_pSubMenuWnd->PostMessage(WM_KEYDOWN, VK_DOWN, event.lParam);
        }
        else if (!hasSubMenu && event.chKey == VK_RETURN)
        {
            SetCheck(!IsChecked());
            CMenuWnd::s_strName = GetName();
            CMenuWnd::s_strUserData = GetUserData();
            CMenuWnd::s_ptrTag = GetTag();
            CPaintManagerUI *pManager = GetGlobalMenuSubject().GetManager();

            if (NULL != pManager)
            {
                HWND hWnd = pManager->GetPaintWindow();
                PostMessage(hWnd, WM_MENUITEM_CLICK, NULL, (LPARAM)(IsChecked() ? TRUE : FALSE));
                TNotifyUI msg;
                msg.pSender = NULL;
                msg.dwTimestamp = 0;
                msg.ptMouse.x = msg.ptMouse.y = 0;
                msg.sType = DUI_MSGTYPE_MENUITEM_CLICK;
                msg.wParam = (WPARAM)&CMenuWnd::s_strName;
                msg.lParam = (LPARAM)CMenuWnd::s_ptrTag;
                pManager->SendNotify(msg, true);
            }

            GetGlobalMenuSubject().NotifyObserver(EMENU_CLOSE_ALL, (WPARAM)GetManager()->GetPaintWindow());
        }
    }
}

} // namespace DuiLib
