#include "stdafx.h"
#include "UIShadow.h"

namespace DuiLib {

//////////////////////////////////////////////////////////////////////////
enum EMWndState
{
    ESTATE_UNKNOW,      //
    ESTATE_CREATE,      // 创建窗口
    ESTATE_SHOW,        // 显示窗口
    ESTATE_HIDE,        // 隐藏窗口
    ESTATE_CLOSE,       // 销毁窗口
};

// 功能选项控件关联的子窗口类型字符串或控件指针
struct TBudddyItem
{
    CDuiString          m_strDlg;   // 对话框类型字符串，用于工厂模式创建对话框
    CControlUI         *m_pCtrl;    // 控件指针

    TBudddyItem(void) { Reset(); }
    TBudddyItem(CDuiString strDlg, CControlUI *pCtrl) : m_strDlg(strDlg), m_pCtrl(pCtrl) { }
    void Reset(void)
    {
        m_strDlg.Empty();
        m_pCtrl = NULL;
    }
};


LPBYTE CWndImplBase::m_lpResourceZIPBuffer = NULL;

DUI_BEGIN_MESSAGE_MAP(CWndImplBase, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CWndImplBase::CWndImplBase()
    : m_pbtnMin(NULL)
    , m_pbtnMax(NULL)
    , m_pbtnRestore(NULL)
    , m_pbtnClose(NULL)
    , m_bModified(false)
    , m_nWndState(ESTATE_UNKNOW)
    , m_aryCtrlStatic(7)
    , m_pCtrlPlaceHolder(NULL)
{
}

void CWndImplBase::OnInitWindow(void)
{
    m_pbtnMin = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnmin")));
    m_pbtnMax = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnmax")));
    m_pbtnRestore = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnrestore")));
    m_pbtnClose = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnclose")));
}

DUI_INLINE void CWndImplBase::OnFinalMessage(HWND hWnd)
{
    m_pm.RemovePreMessageFilter(this);
    m_pm.RemoveNotifier(this);
    m_pm.ReapObjects(m_pm.GetRoot());
}

void CWndImplBase::OnDataSave(void)
{
    DestroyChildDlg();
    ResetBtnDlgItem();
    m_pCtrlPlaceHolder = NULL;
}

LRESULT CWndImplBase::ResponseDefaultKeyEvent(WPARAM wParam)
{
    if (wParam == VK_RETURN)
    {
        return FALSE;
    }
    else if (wParam == VK_ESCAPE)
    {
        Close();
        return TRUE;
    }

    return FALSE;
}

LRESULT CWndImplBase::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled)
{
    if (uMsg == WM_KEYDOWN)
    {
        switch (wParam)
        {
        case VK_RETURN:
        case VK_ESCAPE:
            return ResponseDefaultKeyEvent(wParam);

        default:
            break;
        }
    }

    return FALSE;
}

LRESULT CWndImplBase::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;

    // 2018-08-18 zhuyadong 添加特效
    if (lParam == TRIGGER_NONE)
    {
        if (ESTATE_SHOW == m_nWndState || ESTATE_CREATE == m_nWndState) { OnDataSave(); }

        if (ESTATE_SHOW == m_nWndState && m_pm.GetRoot()->HasEffect(TRIGGER_HIDE))
        {
            // 即将播放窗口关闭特效，隐藏窗口阴影
            CShadowUI *pShadow = m_pm.GetShadow();
            pShadow ? pShadow->SetShadowShow(false) : NULL; //lint !e62
            m_nWndState = ESTATE_CLOSE;
            bHandled = m_pm.GetRoot()->StartEffect(TRIGGER_HIDE);
        }
    }

    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

#if defined(WIN32) && !defined(UNDER_CE)
DUI_INLINE LRESULT CWndImplBase::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = (::GetWindowLong(*this, GWL_STYLE) & WS_CAPTION) ? FALSE : bHandled;
    bHandled = (::IsIconic(*this)) ? FALSE : bHandled;
    //if (::IsIconic(*this)) { bHandled = FALSE; }

    return (wParam == 0) ? TRUE : FALSE;
}

LRESULT CWndImplBase::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = (::GetWindowLong(*this, GWL_STYLE) & WS_CAPTION) ? FALSE : bHandled;

    if (!bHandled) { return 0; }

    LPRECT pRect = NULL;

    if (wParam == TRUE)
    {
        LPNCCALCSIZE_PARAMS pParam = (LPNCCALCSIZE_PARAMS)lParam;
        pRect = &pParam->rgrc[0];
    }
    else
    {
        pRect = (LPRECT)lParam;
    }

    if (::IsZoomed(m_hWnd))
    {
        // 最大化时，计算当前显示器最适合宽高度
        MONITORINFO oMonitor = {};
        oMonitor.cbSize = sizeof(oMonitor);
        ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
        CDuiRect rcWork = oMonitor.rcWork;
        CDuiRect rcMonitor = oMonitor.rcMonitor;
        rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

        pRect->right = pRect->left + rcWork.GetWidth();
        pRect->bottom = pRect->top + rcWork.GetHeight();
        return WVR_REDRAW;
    }

    return 0;
}

LRESULT CWndImplBase::OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;

    if (IsZoomed(m_hWnd))
    {
        // 2018-09-06 zhuyadong 修复代码来自 redrain
        LPWINDOWPOS lpPos = (LPWINDOWPOS)lParam;

        if (lpPos->flags & SWP_FRAMECHANGED) // 第一次最大化，而不是最大化之后所触发的WINDOWPOSCHANGE
        {
            POINT pt = { 0, 0 };
            HMONITOR hMontorPrimary = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
            HMONITOR hMonitorTo = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);

            if (hMonitorTo != hMontorPrimary)
            {
                // 解决无边框窗口在双屏下面（副屏分辨率大于主屏）时，最大化不正确的问题
                MONITORINFO  miTo = { sizeof(miTo), 0 };
                GetMonitorInfo(hMonitorTo, &miTo);
                lpPos->x = miTo.rcWork.left;
                lpPos->y = miTo.rcWork.top;
                lpPos->cx = miTo.rcWork.right - miTo.rcWork.left;
                lpPos->cy = miTo.rcWork.bottom - miTo.rcWork.top;
            }
        }
    }

    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = (::GetWindowLong(*this, GWL_STYLE) & WS_CAPTION) ? FALSE : bHandled;
    return 0;
}

LRESULT CWndImplBase::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
    ::ScreenToClient(*this, &pt);

    RECT rcClient;
    ::GetClientRect(*this, &rcClient);

    if (!::IsZoomed(*this))
    {
        RECT rcSizeBox = m_pm.GetSizeBox();

        if (pt.y < rcClient.top + rcSizeBox.top)
        {
            if (pt.x < rcClient.left + rcSizeBox.left) { return HTTOPLEFT; }

            if (pt.x > rcClient.right - rcSizeBox.right) { return HTTOPRIGHT; }

            return HTTOP;
        }
        else if (pt.y > rcClient.bottom - rcSizeBox.bottom)
        {
            if (pt.x < rcClient.left + rcSizeBox.left) { return HTBOTTOMLEFT; }

            if (pt.x > rcClient.right - rcSizeBox.right) { return HTBOTTOMRIGHT; }

            return HTBOTTOM;
        }

        if (pt.x < rcClient.left + rcSizeBox.left) { return HTLEFT; }

        if (pt.x > rcClient.right - rcSizeBox.right) { return HTRIGHT; }
    }

    RECT rcCaption = m_pm.GetCaptionRect();

    if ((pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right) &&
        ((pt.y >= rcCaption.top && pt.y < rcCaption.bottom) || -1 == rcCaption.bottom))
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(pt));

        if (pControl && IsCaptionCtrl(pControl)) { return HTCAPTION; }
    }

    // if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right
    //          && pt.y >= rcCaption.top && pt.y < rcCaption.bottom)
    // {
    //     CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(pt));
    //
    //     if (pControl && _tcsicmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
    //         _tcsicmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
    //         _tcsicmp(pControl->GetClass(), DUI_CTR_TEXT) != 0)
    //     {
    //         return HTCAPTION;
    //     }
    // }

    return HTCLIENT;
}

LRESULT CWndImplBase::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

    MONITORINFO oMonitor = {};
    oMonitor.cbSize = sizeof(oMonitor);
    ::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
    CDuiRect rcWork = oMonitor.rcWork;
    CDuiRect rcMonitor = oMonitor.rcMonitor;
    rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

    // 计算最大化时，正确的原点坐标
    lpMMI->ptMaxPosition.x  = rcWork.left;
    lpMMI->ptMaxPosition.y  = rcWork.top;

    lpMMI->ptMaxTrackSize.x = rcWork.GetWidth();
    lpMMI->ptMaxTrackSize.y = rcWork.GetHeight();

    lpMMI->ptMinTrackSize.x = m_pm.GetMinInfo().cx;
    lpMMI->ptMinTrackSize.y = m_pm.GetMinInfo().cy;

    bHandled = FALSE;
    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT CWndImplBase::OnNcLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if ((m_pbtnMax && m_pbtnMax->IsVisible()) || (m_pbtnRestore && m_pbtnRestore->IsVisible()))
    {
        bHandled = FALSE;
    }

    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}
#endif

LRESULT CWndImplBase::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    SIZE szRoundCorner = m_pm.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)

    if (!::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0))
    {
        CDuiRect rcWnd;
        ::GetWindowRect(*this, &rcWnd);
        rcWnd.Offset(-rcWnd.left, -rcWnd.top);
        rcWnd.right++; rcWnd.bottom++;
        HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom,
                                         szRoundCorner.cx, szRoundCorner.cy);
        ::SetWindowRgn(*this, hRgn, TRUE);
        ::DeleteObject(hRgn);
    }

#endif
    bHandled = FALSE;
    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT CWndImplBase::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (wParam == SC_CLOSE)
    {
        bHandled = TRUE;
        SendMessage(WM_CLOSE);
        return 0;
    }

#if defined(WIN32) && !defined(UNDER_CE)
    BOOL bZoomed = ::IsZoomed(*this);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);

    if (::IsZoomed(*this) != bZoomed)
    {
        // toggle status of max and restore button
        if (NULL != m_pbtnMax && NULL != m_pbtnRestore)
        {
            m_pbtnMax->SetVisible(TRUE == bZoomed);
            m_pbtnRestore->SetVisible(FALSE == bZoomed);
        }
    }

#else
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
#endif
    return lRes;
}

LRESULT CWndImplBase::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
    styleValue &= ~WS_CAPTION;
    ::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
    RECT rcClient;
    ::GetClientRect(*this, &rcClient);
    ::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
                   rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

    m_pm.Init(m_hWnd);
    m_pm.AddPreMessageFilter(this);

    CDialogBuilder builder;
    CDuiString strResourcePath = m_pm.GetResourcePath();

    if (strResourcePath.IsEmpty())
    {
        strResourcePath = m_pm.GetInstancePath();
        strResourcePath += GetSkinFolder().GetData();
        m_pm.SetResourcePath(strResourcePath.GetData());
    }

    switch (GetResourceType())
    {
    case UILIB_ZIP:
        m_pm.SetResourceZip(GetZIPFileName().GetData(), true);
        break;

    case UILIB_ZIPRESOURCE:
        {
            HRSRC hResource = ::FindResource(m_pm.GetResourceDll(), GetResourceID(), _T("ZIPRES"));

            if (hResource == NULL) { return 0L; }

            DWORD dwSize = 0;
            HGLOBAL hGlobal = ::LoadResource(m_pm.GetResourceDll(), hResource);

            if (hGlobal == NULL)
            {
#if defined(WIN32) && !defined(UNDER_CE)
                ::FreeResource(hResource);
#endif
                return 0L;
            }

            dwSize = ::SizeofResource(m_pm.GetResourceDll(), hResource);

            if (dwSize == 0) { return 0L; }

            m_lpResourceZIPBuffer = new BYTE[ dwSize ];

            if (m_lpResourceZIPBuffer != NULL)
            {
                ::CopyMemory(m_lpResourceZIPBuffer, (LPBYTE)::LockResource(hGlobal), dwSize);
            }

#if defined(WIN32) && !defined(UNDER_CE)
            ::FreeResource(hResource);
#endif
            m_pm.SetResourceZip(m_lpResourceZIPBuffer, dwSize);
        }
        break;
    }

    CControlUI *pRoot = NULL;

    if (GetResourceType() == UILIB_RESOURCE)
    {
        STRINGorID xml(_ttoi(GetSkinFile().GetData()));
        pRoot = builder.Create(xml, _T("xml"), this, &m_pm);
    }
    else
    { pRoot = builder.Create(GetSkinFile().GetData(), (UINT)0, this, &m_pm); }

    ASSERT(pRoot);

    if (pRoot == NULL)
    {
        MessageBox(NULL, _T("加载资源文件失败"), _T("Duilib"), MB_OK | MB_ICONERROR);
        ExitProcess(1);
        return 0;
    }

    if (::GetWindowLong(*this, GWL_STYLE) & WS_CAPTION)
    {
        // 有标题栏
        RECT rt = { 0 };
        m_pm.SetSizeBox(rt);
        m_pm.SetCaptionRect(rt);
        m_pm.SetRoundCorner(0, 0);
    }

    if (!(::GetWindowLong(*this, GWL_STYLE) & WS_CHILD))
    {
        AddIncludeCtrlForCaption(DUI_CTR_CONTROL);
        AddIncludeCtrlForCaption(DUI_CTR_LABEL);
        AddIncludeCtrlForCaption(DUI_CTR_TEXT);
        AddIncludeCtrlForCaption(DUI_CTR_GIFANIM);
        AddIncludeCtrlForCaption(DUI_CTR_PWDCHECK);
        AddIncludeCtrlForCaption(DUI_CTR_PROGRESS);
        AddIncludeCtrlForCaption(DUI_CTR_CONTAINER);
        AddIncludeCtrlForCaption(DUI_CTR_CHILDLAYOUT);
        AddIncludeCtrlForCaption(DUI_CTR_VERTICALLAYOUT);
        AddIncludeCtrlForCaption(DUI_CTR_VBOX);
        AddIncludeCtrlForCaption(DUI_CTR_HORIZONTALLAYOUT);
        AddIncludeCtrlForCaption(DUI_CTR_HBOX);
        AddIncludeCtrlForCaption(DUI_CTR_HWEIGHTLAYOUT);
        AddIncludeCtrlForCaption(DUI_CTR_TILELAYOUT);
        AddIncludeCtrlForCaption(DUI_CTR_TABLAYOUT);
    }

    m_pm.AttachDialog(pRoot);
    m_pm.AddNotifier(this);
    OnInitWindow();

    if (((CREATESTRUCT *)lParam)->style & WS_VISIBLE)
    {
        m_nWndState = ESTATE_SHOW;
        OnDataInit();
    }
    else
    {
        m_nWndState = ESTATE_CREATE;
    }

    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

DUI_INLINE LRESULT CWndImplBase::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT CWndImplBase::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

LRESULT CWndImplBase::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    switch (uMsg)
    {
    case WM_CREATE:         lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;

    case WM_CLOSE:          lRes = OnClose(uMsg, wParam, lParam, bHandled); break;

    case WM_DESTROY:        lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
#if defined(WIN32) && !defined(UNDER_CE)

    case WM_NCACTIVATE:     lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;

    case WM_NCCALCSIZE:     lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;

    case WM_WINDOWPOSCHANGING: lRes = OnWindowPosChanging(uMsg, wParam, lParam, bHandled); break;

    case WM_NCPAINT:        lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;

    case WM_NCHITTEST:      lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;

    case WM_GETMINMAXINFO:  lRes = OnGetMinMaxInfo(uMsg, wParam, lParam, bHandled); break;

    case WM_MOUSEWHEEL:     lRes = OnMouseWheel(uMsg, wParam, lParam, bHandled); break;

    case WM_NCLBUTTONDBLCLK: lRes = OnNcLButtonDblClk(uMsg, wParam, lParam, bHandled); break;
#endif

    case WM_SIZE:           lRes = OnSize(uMsg, wParam, lParam, bHandled); break;

    case WM_CHAR:           lRes = OnChar(uMsg, wParam, lParam, bHandled); break;

    case WM_SYSCOMMAND:     lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;

    case WM_KEYDOWN:        lRes = OnKeyDown(uMsg, wParam, lParam, bHandled); break;

    case WM_KILLFOCUS:      lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;

    case WM_SETFOCUS:       lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;

    case WM_LBUTTONUP:      lRes = OnLButtonUp(uMsg, wParam, lParam, bHandled); break;

    case WM_LBUTTONDOWN:    lRes = OnLButtonDown(uMsg, wParam, lParam, bHandled); break;

    case WM_MOUSEMOVE:      lRes = OnMouseMove(uMsg, wParam, lParam, bHandled); break;

    case WM_MOUSEHOVER:     lRes = OnMouseHover(uMsg, wParam, lParam, bHandled); break;

    case WM_TIMER:          lRes = OnTimer(uMsg, wParam, lParam, bHandled); break;

    default:                bHandled = FALSE; break;
    }

    if (bHandled) { return lRes; }

    lRes = HandleCustomMessage(uMsg, wParam, lParam, bHandled);

    if (bHandled) { return lRes; }

    if (m_pm.MessageHandler(uMsg, wParam, lParam, lRes)) { return lRes; }

    return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CWndImplBase::AddChild(CDuiString strType, CWndImplBase *pWnd)
{
    m_mapChild.Set(strType, pWnd);
}

void CWndImplBase::DelChild(CDuiString strType)
{
    m_mapChild.Remove(strType);
}

CWndImplBase *CWndImplBase::GetChild(CDuiString strType)
{
    LPVOID p = m_mapChild.Find(strType);
    return (CWndImplBase *)p;
}

void CWndImplBase::DestroyChildDlg()
{
    for (int i = 0; i < m_mapChild.GetSize(); ++i)
    {
        LPCTSTR pKey = m_mapChild.GetAt(i);

        if (pKey)
        {
            CWndImplBase *pWnd = (CWndImplBase *)m_mapChild.Find(pKey);

            if (NULL != pWnd && ::IsWindow(pWnd->GetHWND())) { pWnd->Close(IDCANCEL, true); }
        }
    }

    m_mapChild.RemoveAll();
}

void CWndImplBase::DestroyChildDlg(CDuiString strType)
{
    CWndImplBase *pWnd = (CWndImplBase *)m_mapChild.Find(strType);

    if (NULL != pWnd && ::IsWindow(pWnd->GetHWND())) { pWnd->Close(IDOK, true); }

    m_mapChild.Remove(strType);
}

bool CWndImplBase::MakeCtrlSizeNty(CContainerUI *pCtrl)
{
    ASSERT(NULL != pCtrl);

    if (NULL == pCtrl) { return false; }

    m_pCtrlPlaceHolder = pCtrl;
    pCtrl->OnSize += MakeDelegate(this, &CWndImplBase::Relayout);
    return true;
}

void CWndImplBase::AddBtnDlgItem(CDuiString strBtnName, CDuiString strDlgType, CControlUI *pCtrl)
{
    m_mapBtnItem.Set(strBtnName, new TBudddyItem(strDlgType, pCtrl));
}

void CWndImplBase::ResetBtnDlgItem(void)
{
    for (int i = 0; i < m_mapBtnItem.GetSize(); ++i)
    {
        LPCTSTR pKey = m_mapBtnItem.GetAt(i);

        if (pKey)
        {
            TBudddyItem *pItem = (TBudddyItem *)m_mapBtnItem.Find(pKey);

            if (NULL != pItem) { delete pItem; }
        }
    }

    m_mapBtnItem.RemoveAll();
}

void CWndImplBase::SwitchChildDlg(CDuiString strBtnName)
{
    TBudddyItem *pItem = (TBudddyItem *)m_mapBtnItem.Find(strBtnName);
    ASSERT(pItem);

    if (NULL == pItem || m_strChildDlgType == pItem->m_strDlg) { return; }

    CWndImplBase *pWnd = (CWndImplBase *)m_mapChild.Find(m_strChildDlgType);

    if (NULL != pWnd)
    {
        // 销毁子窗口，保存数据
        DestroyChildDlg(m_strChildDlgType);
        m_strChildDlgType.Empty();
    }

    pWnd = CreateWnd(pItem->m_strDlg);

    if (NULL != pWnd)
    {
        m_strChildDlgType = pItem->m_strDlg;
        AddChild(m_strChildDlgType, pWnd);
        Relayout(NULL);
        pWnd->ShowWindow(true);
    }
}

LRESULT CWndImplBase::OnWndDataUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (EPARAM_INIT == wParam) { OnDataInit(); }
    else                       { OnDataSave(); }

    return 0;
}

LRESULT CWndImplBase::OnWndEffectShowEndNty(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ESTATE_SHOW == m_nWndState && m_pm.GetRoot()->HasEffect(TRIGGER_SHOW))
    {
        // 窗口显示特效播放完毕，显示窗口阴影
        CShadowUI *pShadow = m_pm.GetShadow();
        pShadow ? pShadow->SetShadowShow(true) : NULL; //lint !e62
        m_pm.SetFocusDefault();
    }

    if (ESTATE_HIDE == m_nWndState)
    {
        // 隐藏特效：无需处理窗口阴影
        // 隐藏特效播放完毕，隐藏窗口
        CWindowWnd::ShowWindow(false, false);
    }

    if (ESTATE_CLOSE == m_nWndState)
    {
        // 这里不能直接 DestroyWindow(),会导致异常崩溃。
        PostMessage(WM_CLOSE, 0, TRIGGER_HIDE);
    }

    return 0;
}

bool CWndImplBase::Relayout(void *pParam)
{
    if (NULL == m_pm.GetRoot() || NULL == m_pCtrlPlaceHolder) { return false; }

    CWndImplBase *pWndChild = (CWndImplBase *)m_mapChild.Find(m_strChildDlgType);

    if (NULL == pWndChild) { return false; }

    CDuiRect rt = m_pCtrlPlaceHolder->GetClientPos();
    ::SetWindowPos(pWndChild->GetHWND(), NULL, rt.left, rt.top, rt.GetWidth(), rt.GetHeight(), SWP_SHOWWINDOW);
    return true;
}

void CWndImplBase::SetModified(bool bModified)
{
    m_bModified = bModified;
}

bool CWndImplBase::IsModified(void)
{
    return m_bModified;
}

DUI_INLINE LRESULT CWndImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LRESULT lRes = 0;

    switch (uMsg)
    {
    //2017-02-25 zhuyadong 完善多语言切换
    case WM_LANGUAGE_UPDATE:            lRes = OnLanguageUpdate(uMsg, wParam, lParam);      break;

    case WM_WNDDATA_UPDATE:             lRes = OnWndDataUpdate(uMsg, wParam, lParam);       break;

    case WM_WNDEFFECT_SHOWEND_NOTIFY:   lRes = OnWndEffectShowEndNty(uMsg, wParam, lParam); break;

    default:                            bHandled = FALSE;                                   break;
    }

    return lRes;
}

void CWndImplBase::OnClick(TNotifyUI &msg)
{
    if (m_pbtnClose == msg.pSender)         { Close(); }
    else if (m_pbtnMin == msg.pSender)      { SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); }
    else if (m_pbtnMax == msg.pSender)      { SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); }
    else if (m_pbtnRestore == msg.pSender)  { SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); }
}

void CWndImplBase::AddIncludeCtrlForCaption(LPCTSTR szCtrlName)
{
    if (-1 == m_aryCtrlStatic.Find((LPVOID)szCtrlName))
    {
        m_aryCtrlStatic.Add((LPVOID)szCtrlName);
    }
}

bool CWndImplBase::IsCaptionCtrl(CControlUI *pCtrl)
{
    LPCTSTR szCtrlName = pCtrl->GetClass();

    for (int i = 0; i < m_aryCtrlStatic.GetSize(); ++i)
    {
        LPCTSTR szName = (LPCTSTR)m_aryCtrlStatic.GetAt(i);

        if (0 == _tcscmp(szCtrlName, szName)) { return true; }
    }

    return false;
}

void CWndImplBase::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= true*/)
{
    // 正在播放窗口隐藏特效，立即重新播放隐藏特效
    if (ESTATE_HIDE == m_nWndState && !bShow && m_pm.GetRoot()->IsEffectRunning())
    {
        if (m_pm.GetRoot()->StartEffect(TRIGGER_HIDE)) { return; }
    }

    // 窗口从显示到隐藏，如果有特效则播放特效
    if (ESTATE_SHOW == m_nWndState && !bShow)
    {
        m_nWndState = ESTATE_HIDE;
        SendMessage(WM_WNDDATA_UPDATE, EPARAM_SAVE);

        if (m_pm.GetRoot()->HasEffect(TRIGGER_HIDE))
        {
            // 即将播放隐藏特效，隐藏窗口阴影
            CShadowUI *pShadow = m_pm.GetShadow();
            pShadow ? pShadow->SetShadowShow(false) : NULL; //lint !e62

            // 播放特效成功，阻止窗口隐藏；待到特效播放完毕，再隐藏。执行成功后，阻止隐藏窗口
            if (m_pm.GetRoot()->StartEffect(TRIGGER_HIDE)) { return; }
        }
    }

    // 正在播放窗口显示特效，立即重新播放显示特效
    if (ESTATE_SHOW == m_nWndState && bShow && m_pm.GetRoot()->IsEffectRunning())
    {
        m_pm.GetRoot()->StartEffect(TRIGGER_SHOW);
        return;
    }

    // 窗口从隐藏到显示，如果有特效则播放特效
    if ((ESTATE_HIDE == m_nWndState || ESTATE_CREATE == m_nWndState) && bShow)
    {
        if (m_pm.GetRoot()->HasEffect(TRIGGER_SHOW))
        {
            // 即将播放显示特效，必需隐藏窗口阴影；特效播放完毕，再显示窗口阴影
            if (ESTATE_CREATE == m_nWndState)
            {
                CShadowUI *pShadow = m_pm.GetShadow();
                pShadow ? pShadow->SetShadowShow(false) : NULL; //lint !e62
            }

            m_pm.GetRoot()->StartEffect(TRIGGER_SHOW);
        }

        m_nWndState = ESTATE_SHOW;
        PostMessage(WM_WNDDATA_UPDATE, EPARAM_INIT);
    }

    CWindowWnd::ShowWindow(bShow, bTakeFocus);
}

DUI_INLINE void CWndImplBase::Notify(TNotifyUI &msg)
{
    if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
    {
        // 2019-10-11 优化显示/隐藏窗口特效
        // 只在创建窗口时执行一次。创建隐藏窗口，也会走到这里
        OnPrepare();

        // m_pm.SetFocusDefault() 必须在特效播放完毕后执行，否则由于编辑框控件的 Native 窗口，会破坏特效
        // 1. 没有特效时，直接调用
        // 2. 播放特效失败时，直接调用
        // 3. 播放特效成功时，在特效播放完毕后调用
        if (m_pm.GetRoot()->HasEffect(TRIGGER_SHOW))
        {
            if (ESTATE_SHOW == m_nWndState)
            {
                // 即将播放窗口显示特效，隐藏窗口阴影。播放完毕，显示窗口阴影
                CShadowUI *pShadow = m_pm.GetShadow();
                pShadow ? pShadow->SetShadowShow(false) : NULL; //lint !e62
            }

            if (!m_pm.GetRoot()->StartEffect(TRIGGER_SHOW)) { m_pm.SetFocusDefault(); }
        }
        else
        { m_pm.SetFocusDefault(); }

        return;
    }
    else if (msg.sType == DUI_MSGTYPE_SCROLL)
    {
        if (m_pCtrlPlaceHolder)
        {
            CContainerUI *pScrll = dynamic_cast<CScrollBarUI *>(msg.pSender)->GetOwner();
            CControlUI *pParent = m_pCtrlPlaceHolder->GetParent();

            while (pParent)
            {
                if (pParent == pScrll) { Relayout(&msg);  break; }
                else { pParent = pParent->GetParent(); }
            }
        }
    }

    if (NULL == msg.pSender) { return; }

    return CNotifyPump::NotifyPump(msg);
}

LRESULT CWndImplBase::OnLanguageUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ReloadTitle();
    m_pm.UpdateLanguage();
    return 0L;
}

void CWndImplBase::SetCurDlgType(CDuiString strDlgType)
{
    m_strChildDlgType = strDlgType;
}

}
