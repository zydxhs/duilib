#include "StdAfx.h"

namespace DuiLib {

void PaintUpdatedCallback(wkeWebView webView, void *param, const HDC hdc, int x, int y, int cx, int cy)
{
    if (NULL != param)
    {
        CWkeBrowserUI *pView = (CWkeBrowserUI *)param;
        pView->Invalidate();
    }
}

void TitleChangedCallback(wkeWebView webView, void *param, const wkeString title)
{
    if (NULL != param)
    {
        CWkeBrowserUI *pView = (CWkeBrowserUI *)param;
        pView->SendNotify(webView, CWkeBrowserUI::ENTY_TITLE, CDuiString(wkeGetString(title)));
    }
}

void URLChangedCallback(wkeWebView webView, void *param, const wkeString url)
{
    if (NULL != param)
    {
        CWkeBrowserUI *pView = (CWkeBrowserUI *)param;
        CDuiString str1 = wkeGetString(url);
        pView->SendNotify(webView, CWkeBrowserUI::ENTY_URL, CDuiString(wkeGetString(url)));
    }
}

void AlertBoxCallback(wkeWebView webView, void *param, const wkeString msg)
{
    if (NULL != param)
    {
        CWkeBrowserUI *pView = (CWkeBrowserUI *)param;
        pView->SendNotify(webView, CWkeBrowserUI::ENTY_ALERTBOX, CDuiString(wkeGetString(msg)));
    }
}

bool ConfirmBoxCallback(wkeWebView webView, void *param, const wkeString msg)
{
    if (NULL != param)
    {
        CWkeBrowserUI *pView = (CWkeBrowserUI *)param;
        return pView->SendNotify(webView, CWkeBrowserUI::ENTY_CONFIRMBOX, CDuiString(wkeGetString(msg)));
    }

    return false;
}

bool PromptBoxCallback(wkeWebView webView, void *param, const wkeString msg,
                       const wkeString defaultResult, wkeString result)
{
    if (NULL != param)
    {
        CWkeBrowserUI *pView = (CWkeBrowserUI *)param;
        return pView->SendNotify(webView, CWkeBrowserUI::ENTY_PROMPTBOX, CDuiString(wkeGetString(msg)),
                                 CDuiString(wkeGetString(defaultResult)), CDuiString(wkeGetString(result)));
    }

    return false;
}

//////////////////////////////////////////////////////////////////////////
CWkeBrowserUI::CWkeBrowserUI(void) : m_pWeb(NULL)
{
    LoadMiniBlink(_T("node.dll"));
}

CWkeBrowserUI::~CWkeBrowserUI(void)
{
    FreeMiniBlink();
}

LPCTSTR CWkeBrowserUI::GetClass() const
{
    return DUI_CTR_BROWSER;
}

LPVOID CWkeBrowserUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_BROWSER) == 0) { return static_cast<CWkeBrowserUI *>(this); }

    return CControlUI::GetInterface(pstrName);
}

void CWkeBrowserUI::DoEvent(TEventUI &event)
{
    if (!m_pWeb) { return CControlUI::DoEvent(event); }

    switch (event.Type)
    {
    case UIEVENT_TIMER:
        if (event.wParam == TIMERID_WEB_REFRESH)
        {
            wkeRepaintIfNeeded(m_pWeb);
            Invalidate();
        }

        break;

    //case UIEVENT_WINDOWSIZE:
    //    wkeResize(m_pWeb, GET_X_LPARAM(event.lParam), GET_Y_LPARAM(event.lParam));
    //    Invalidate();
    //    break;

    case UIEVENT_BUTTONDOWN:
        {
            wkeSetFocus(m_pWeb);
            SetCapture();
            int x = GET_X_LPARAM(event.lParam);
            int y = GET_Y_LPARAM(event.lParam);
            x -= m_rcPaint.left;
            y -= m_rcPaint.top;

            unsigned int flags = 0;

            if (event.wParam & MK_CONTROL) { flags |= WKE_CONTROL; }

            if (event.wParam & MK_SHIFT) { flags |= WKE_SHIFT; }

            if (event.wParam & MK_LBUTTON) { flags |= WKE_LBUTTON; }

            if (event.wParam & MK_MBUTTON) { flags |= WKE_MBUTTON; }

            if (event.wParam & MK_RBUTTON) { flags |= WKE_RBUTTON; }

            bool handled = wkeFireMouseEvent(m_pWeb, WM_LBUTTONDOWN, x, y, flags);

            if (handled) { return; }
        }
        break;

    case UIEVENT_BUTTONUP:
        {
            ReleaseCapture();
            int x = GET_X_LPARAM(event.lParam);
            int y = GET_Y_LPARAM(event.lParam);
            x -= m_rcPaint.left;
            y -= m_rcPaint.top;

            unsigned int flags = 0;

            if (event.wParam & MK_CONTROL) { flags |= WKE_CONTROL; }

            if (event.wParam & MK_SHIFT) { flags |= WKE_SHIFT; }

            if (event.wParam & MK_LBUTTON) { flags |= WKE_LBUTTON; }

            if (event.wParam & MK_MBUTTON) { flags |= WKE_MBUTTON; }

            if (event.wParam & MK_RBUTTON) { flags |= WKE_RBUTTON; }

            bool handled = wkeFireMouseEvent(m_pWeb, WM_LBUTTONUP, x, y, flags);

            if (handled) { return; }
        }
        break;

    case UIEVENT_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(event.lParam);
            int y = GET_Y_LPARAM(event.lParam);
            x -= m_rcPaint.left;
            y -= m_rcPaint.top;

            unsigned int flags = 0;

            if (event.wParam & MK_CONTROL) { flags |= WKE_CONTROL; }

            if (event.wParam & MK_SHIFT) { flags |= WKE_SHIFT; }

            if (event.wParam & MK_LBUTTON) { flags |= WKE_LBUTTON; }

            if (event.wParam & MK_MBUTTON) { flags |= WKE_MBUTTON; }

            if (event.wParam & MK_RBUTTON) { flags |= WKE_RBUTTON; }

            bool handled = wkeFireMouseEvent(m_pWeb, WM_MOUSEMOVE, x, y, flags);

            if (handled) { return; }
        }
        break;

    case UIEVENT_RBUTTONDOWN:
        {
            wkeSetFocus(m_pWeb);
            int x = GET_X_LPARAM(event.lParam);
            int y = GET_Y_LPARAM(event.lParam);
            x -= m_rcPaint.left;
            y -= m_rcPaint.top;

            unsigned int flags = 0;

            if (event.wParam & MK_CONTROL) { flags |= WKE_CONTROL; }

            if (event.wParam & MK_SHIFT) { flags |= WKE_SHIFT; }

            if (event.wParam & MK_LBUTTON) { flags |= WKE_LBUTTON; }

            if (event.wParam & MK_MBUTTON) { flags |= WKE_MBUTTON; }

            if (event.wParam & MK_RBUTTON) { flags |= WKE_RBUTTON; }

            bool handled = wkeFireMouseEvent(m_pWeb, WM_RBUTTONDOWN, x, y, flags);

            if (handled) { return; }
        }
        break;

    case UIEVENT_RBUTTONUP:
        {
            int x = GET_X_LPARAM(event.lParam);
            int y = GET_Y_LPARAM(event.lParam);
            x -= m_rcPaint.left;
            y -= m_rcPaint.top;

            unsigned int flags = 0;

            if (event.wParam & MK_CONTROL) { flags |= WKE_CONTROL; }

            if (event.wParam & MK_SHIFT) { flags |= WKE_SHIFT; }

            if (event.wParam & MK_LBUTTON) { flags |= WKE_LBUTTON; }

            if (event.wParam & MK_MBUTTON) { flags |= WKE_MBUTTON; }

            if (event.wParam & MK_RBUTTON) { flags |= WKE_RBUTTON; }

            bool handled = wkeFireMouseEvent(m_pWeb, WM_RBUTTONUP, x, y, flags);

            if (handled) { return; }
        }
        break;

    case UIEVENT_LBUTTONDBLDOWN:
        {
            int x = GET_X_LPARAM(event.lParam);
            int y = GET_Y_LPARAM(event.lParam);
            x -= m_rcPaint.left;
            y -= m_rcPaint.top;

            unsigned int flags = 0;

            if (event.wParam & MK_CONTROL) { flags |= WKE_CONTROL; }

            if (event.wParam & MK_SHIFT) { flags |= WKE_SHIFT; }

            if (event.wParam & MK_LBUTTON) { flags |= WKE_LBUTTON; }

            if (event.wParam & MK_MBUTTON) { flags |= WKE_MBUTTON; }

            if (event.wParam & MK_RBUTTON) { flags |= WKE_RBUTTON; }

            bool handled = wkeFireMouseEvent(m_pWeb, WM_LBUTTONDBLCLK, x, y, flags);

            if (handled) { return; }
        }
        break;

    // case UIEVENT_CONTEXTMENU:
    //     {
    //         unsigned int flags = 0;
    //
    //         if (event.wParam & MK_CONTROL) { flags |= WKE_CONTROL; }
    //
    //         if (event.wParam & MK_SHIFT) { flags |= WKE_SHIFT; }
    //
    //         if (event.wParam & MK_LBUTTON) { flags |= WKE_LBUTTON; }
    //
    //         if (event.wParam & MK_MBUTTON) { flags |= WKE_MBUTTON; }
    //
    //         if (event.wParam & MK_RBUTTON) { flags |= WKE_RBUTTON; }
    //
    //         bool handled = wkeFireContextMenuEvent(m_pWeb, event.ptMouse.x, event.ptMouse.y, flags);
    //
    //         if (handled) { return; }
    //     }
    //     break;

    case UIEVENT_SCROLLWHEEL:
        {
            POINT pt;
            pt.x = event.ptMouse.x;
            pt.y = event.ptMouse.y;
            pt.x -= m_rcPaint.left;
            pt.y -= m_rcPaint.top;
            ScreenToClient(m_pManager->GetPaintWindow(), &pt);

            //int delta = GET_WHEEL_DELTA_WPARAM(event.wParam);
            int nFlag = GET_X_LPARAM(event.wParam);
            int delta = (nFlag == SB_LINEDOWN) ? -120 : 120;
            unsigned int flags = 0;

            if (event.wParam & MK_CONTROL) { flags |= WKE_CONTROL; }

            if (event.wParam & MK_SHIFT) { flags |= WKE_SHIFT; }

            if (event.wParam & MK_LBUTTON) { flags |= WKE_LBUTTON; }

            if (event.wParam & MK_MBUTTON) { flags |= WKE_MBUTTON; }

            if (event.wParam & MK_RBUTTON) { flags |= WKE_RBUTTON; }

            bool handled = wkeFireMouseWheelEvent(m_pWeb, pt.x, pt.y, delta, flags);

            if (handled) { return; }
        }
        break;

    case UIEVENT_KEYDOWN:
        {
            unsigned int virtualKeyCode = event.wParam;
            unsigned int flags = 0;

            if (HIWORD(event.lParam) & KF_REPEAT) { flags |= WKE_REPEAT; }

            if (HIWORD(event.lParam) & KF_EXTENDED) { flags |= WKE_EXTENDED; }

            bool handled = wkeFireKeyDownEvent(m_pWeb, virtualKeyCode, flags, false);

            if (event.wParam == VK_F5) { Reload(); }

            if (handled) { return; }
        }
        break;

    case UIEVENT_KEYUP:
        {
            unsigned int virtualKeyCode = event.wParam;
            unsigned int flags = 0;

            if (HIWORD(event.lParam) & KF_REPEAT) { flags |= WKE_REPEAT; }

            if (HIWORD(event.lParam) & KF_EXTENDED) { flags |= WKE_EXTENDED; }

            bool handled = wkeFireKeyUpEvent(m_pWeb, virtualKeyCode, flags, false);

            if (handled) { return; }
        }
        break;

    case UIEVENT_CHAR:
        {
            unsigned int charCode = event.wParam;
            unsigned int flags = 0;

            if (HIWORD(event.lParam) & KF_REPEAT) { flags |= WKE_REPEAT; }

            if (HIWORD(event.lParam) & KF_EXTENDED) { flags |= WKE_EXTENDED; }

            bool handled = wkeFireKeyPressEvent(m_pWeb, charCode, flags, false);

            if (handled) { return; }
        }
        break;

    case UIEVENT_IME_STARTCOMPOSITION:
        {
            wkeRect caret = wkeGetCaretRect(m_pWeb);

            CANDIDATEFORM form;
            form.dwIndex = 0;
            form.dwStyle = CFS_EXCLUDE;
            form.ptCurrentPos.x = caret.x;
            form.ptCurrentPos.y = caret.y + caret.h;
            form.rcArea.top = caret.y + m_rcPaint.top;
            form.rcArea.bottom = caret.y + caret.h + m_rcPaint.top;
            form.rcArea.left = caret.x + m_rcPaint.left;
            form.rcArea.right = caret.x + caret.w + m_rcPaint.left;

            HIMC hIMC = ImmGetContext(m_pManager->GetPaintWindow());
            ImmSetCandidateWindow(hIMC, &form);
            ImmReleaseContext(m_pManager->GetPaintWindow(), hIMC);
        }
        break;

    case UIEVENT_SETFOCUS:
        wkeSetFocus(m_pWeb);
        break;

    case UIEVENT_KILLFOCUS:
        wkeKillFocus(m_pWeb);
        break;

    default:
        CControlUI::DoEvent(event);
        break;
    }
}

bool CWkeBrowserUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    // 2018-08-18 zhuyadong 添加特效
    if (IsEffectRunning())
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
        PaintWebContent(hDC, rcPaint);
        PaintText(hDC);
        PaintBorder(hDC);
    }
    else
    {
        PaintBkColor(hDC);
        PaintBkImage(hDC);
        PaintStatusImage(hDC);
        PaintWebContent(hDC, rcPaint);
        PaintText(hDC);
        PaintBorder(hDC);
    }

    return true;
}

void CWkeBrowserUI::DoInit()
{
    CControlUI::DoInit();
    InitBrowser();
}

void CWkeBrowserUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);

    if (m_pWeb)
    {
        wkeResize(m_pWeb, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
    }
}

void CWkeBrowserUI::InitBrowser(void)
{
    m_pWeb = wkeCreateWebView();
    wkeSetTransparent(m_pWeb, false);

    wkeOnPaintUpdated(m_pWeb, PaintUpdatedCallback, this);
    wkeOnTitleChanged(m_pWeb, TitleChangedCallback, this);
    wkeOnURLChanged(m_pWeb, URLChangedCallback, this);
    wkeOnAlertBox(m_pWeb, AlertBoxCallback, this);
    wkeOnConfirmBox(m_pWeb, ConfirmBoxCallback, this);
    wkeOnPromptBox(m_pWeb, PromptBoxCallback, this);

    wkeResize(m_pWeb, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top);
    m_pManager->SetTimer(this, TIMERID_WEB_REFRESH, 15);
}

bool CWkeBrowserUI::SendNotify(void *pWebView, int nFlag, LPCTSTR sMsg, LPCTSTR sDefRet, LPCTSTR sRet)
{
    if (ENTY_TITLE == nFlag) { m_sTitle = sMsg; }
    else if (ENTY_URL == nFlag) { m_sURL = sMsg; }
    else if (ENTY_ALERTBOX == nFlag) { m_sMsg = sMsg; }
    else if (ENTY_CONFIRMBOX == nFlag) { m_sMsg = sMsg; }
    else if (ENTY_PROMPTBOX == nFlag)
    {
        m_sMsg = sMsg;
        m_sDefRet = sDefRet;
        m_sRet = sRet;
    }

    TNotifyUI msg;
    msg.pSender = this;
    msg.sType = DUI_MSGTYPE_WEBNOTIFY;
    msg.dwTimestamp = 0;
    msg.ptMouse.x = msg.ptMouse.y = 0;
    msg.wParam = nFlag;
    msg.lParam = (LPARAM)pWebView;
    m_pManager->SendNotify(msg);
    return (0 != msg.dwTimestamp);
}

void CWkeBrowserUI::LoadUrl(LPCTSTR szUrl)
{
    wkeLoadURL(m_pWeb, szUrl);
}

void CWkeBrowserUI::LoadFile(LPCTSTR szFile)
{
    wkeLoadFile(m_pWeb, szFile);
}

void CWkeBrowserUI::Reload(void)
{
    if (NULL != m_pWeb) { wkeReload(m_pWeb); }
}

CDuiString CWkeBrowserUI::RunJS(LPCTSTR szJS)
{
    jsValue jsRet = wkeRunJS(m_pWeb, szJS);
    jsExecState jsState = wkeGlobalExec(m_pWeb);
    CDuiString strRet = jsToTempString(jsState, jsRet);
    return strRet;
}

bool CWkeBrowserUI::CanGoBack()
{
    return wkeCanGoBack(m_pWeb);
}

void CWkeBrowserUI::GoBack()
{
    wkeGoBack(m_pWeb);
}

bool CWkeBrowserUI::CanGoForward()
{
    return wkeCanGoForward(m_pWeb);
}

void CWkeBrowserUI::GoForward()
{
    wkeGoForward(m_pWeb);
}

void CWkeBrowserUI::PaintWebContent(HDC hDC, const RECT &rcPaint)
{
    HDC hDCWeb = wkeGetViewDC(m_pWeb);
    RECT rc = GetClientPos();
    BitBlt(hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hDCWeb, 0, 0, SRCCOPY);
}

}
