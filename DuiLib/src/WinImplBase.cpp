#include "stdafx.h"

namespace DuiLib {

//////////////////////////////////////////////////////////////////////////
enum EMWndState
{
    ESTATE_UNKNOW,      //
    ESTATE_CREATE,      // 创建窗体
    ESTATE_SHOW,        // 显示窗体
    ESTATE_HIDE,        // 隐藏窗体
    ESTATE_CLOSE,       // 销毁窗体
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
    , m_nWndState(ESTATE_UNKNOW)
{
}

void CWndImplBase::OnInitWindow(void)
{
    m_pbtnMin = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnmin")));
    m_pbtnMax = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnmax")));
    m_pbtnRestore = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnrestore")));
    m_pbtnClose = static_cast<CButtonUI *>(m_pm.FindControl(_T("btnclose")));
}

INLINE void CWndImplBase::OnFinalMessage(HWND hWnd)
{
    m_pm.RemovePreMessageFilter(this);
    m_pm.RemoveNotifier(this);
    m_pm.ReapObjects(m_pm.GetRoot());
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
    if (ESTATE_SHOW == m_nWndState) { OnDataSave(); }

    bHandled = FALSE;
    return 0;
}

INLINE LRESULT CWndImplBase::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

#if defined(WIN32) && !defined(UNDER_CE)
INLINE LRESULT CWndImplBase::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
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

INLINE LRESULT CWndImplBase::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
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

    if (pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
        && pt.y >= rcCaption.top && pt.y < rcCaption.bottom)
    {
        CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(pt));

        if (pControl && _tcsicmp(pControl->GetClass(), DUI_CTR_BUTTON) != 0 &&
            _tcsicmp(pControl->GetClass(), DUI_CTR_OPTION) != 0 &&
            _tcsicmp(pControl->GetClass(), DUI_CTR_TEXT) != 0)
        {
            return HTCAPTION;
        }
    }

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

INLINE LRESULT CWndImplBase::OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
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

INLINE LRESULT CWndImplBase::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
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

INLINE LRESULT CWndImplBase::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
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

INLINE LRESULT CWndImplBase::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

INLINE LRESULT CWndImplBase::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

INLINE LRESULT CWndImplBase::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

INLINE LRESULT CWndImplBase::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

INLINE LRESULT CWndImplBase::OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = FALSE;
    return 0;
}

INLINE LRESULT CWndImplBase::OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
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

LRESULT CWndImplBase::OnWndDataUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (EPARAM_INIT == wParam) { OnDataInit(); }
    else                       { OnDataSave(); }

    return 0;
}

INLINE LRESULT CWndImplBase::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LRESULT lRes = 0;

    switch (uMsg)
    {
    //2017-02-25 zhuyadong 完善多语言切换
    case WM_LANGUAGE_UPDATE:    lRes = OnLanguageUpdate(uMsg, wParam, lParam);  break;

    case WM_WNDDATA_UPDATE:     lRes = OnWndDataUpdate(uMsg, wParam, lParam);   break;

    default:                    bHandled = FALSE;                               break;
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

void CWndImplBase::ShowWindow(bool bShow /*= true*/, bool bTakeFocus /*= true*/)
{
    if (ESTATE_SHOW == m_nWndState && !bShow)
    {
        m_nWndState = ESTATE_HIDE;
        SendMessage(WM_WNDDATA_UPDATE, EPARAM_SAVE);
    }

    if ((ESTATE_HIDE == m_nWndState || ESTATE_CREATE == m_nWndState) && bShow)
    {
        m_nWndState = ESTATE_SHOW;
        PostMessage(WM_WNDDATA_UPDATE, EPARAM_INIT);
    }

    CWindowWnd::ShowWindow(bShow, bTakeFocus);
}

INLINE void CWndImplBase::Notify(TNotifyUI &msg)
{
    if (msg.sType == DUI_MSGTYPE_WINDOWINIT)
    {
        OnPrepare();
        return;
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

}
