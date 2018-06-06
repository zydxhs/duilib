#include "stdafx.h"
#include "deelx.h"

namespace DuiLib {

const int DOT_WIDTH = 5;

enum EMIPField
{
    EM_FIELD1 = 0,
    EM_FIELD2,
    EM_FIELD3,
    EM_FIELD4,
};

class CIPAddressWnd : public CWindowWnd
{
public:
    CIPAddressWnd();

    void Init(CIPAddressUI *pOwner, EMIPField eField);
    RECT CalPos();

    LPCTSTR GetWindowClassName() const;
    LPCTSTR GetSuperClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    void GetValidText(LPCTSTR pstrTxt, CDuiString &strValidTxt);

protected:
    bool IsValidChar(TCHAR ch);
protected:
    enum
    {
        DEFAULT_TIMERID = 20,
        // 2017-07-21 zhuyadong 添加 minmaxnumber 属性
        CHECK_TIMERID,          // 用于检测用户输入的值是否越界
    };

    CIPAddressUI   *m_pOwner;
    EMIPField       m_eField;
    HBRUSH m_hBkBrush;
    bool m_bInit;
    bool m_bDrawCaret;
#ifndef UNICODE
    BYTE m_byDChar; // 非0表示当前输入字符为双字节字符
#endif // UNICODE
};


CIPAddressWnd::CIPAddressWnd() : m_pOwner(NULL), m_eField(EM_FIELD1), m_hBkBrush(NULL), m_bInit(false),
    m_bDrawCaret(false)
{
}

void CIPAddressWnd::Init(CIPAddressUI *pOwner, EMIPField eField)
{
    m_pOwner = pOwner;
    m_eField = eField;
    RECT rcPos = CalPos();
    UINT uStyle = WS_CHILD | ES_AUTOHSCROLL | ES_NUMBER | DT_CENTER;
    Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);

    HFONT hFont = m_pOwner->GetManager()->GetFont(m_pOwner->GetFont());

    if (hFont == NULL) { hFont = m_pOwner->GetManager()->GetDefaultFontInfo()->hFont; }

    SetWindowFont(m_hWnd, hFont, TRUE);
    Edit_LimitText(m_hWnd, 3);
    Edit_SetText(m_hWnd, m_pOwner->GetFieldStr(m_eField));
    Edit_SetModify(m_hWnd, FALSE);
    SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
    Edit_Enable(m_hWnd, m_pOwner->IsEnabled());
    Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly());
    Edit_SetSel(m_hWnd, 0, GetWindowTextLength(m_hWnd));

    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SetFocus(m_hWnd);
    m_bInit = true;
}

RECT CIPAddressWnd::CalPos()
{
    CDuiRect rcPos = m_pOwner->GetFieldPos(m_eField);
    LONG lEditHeight = m_pOwner->GetManager()->GetFontInfo(m_pOwner->GetFont())->tm.tmHeight;

    if (lEditHeight < rcPos.GetHeight())
    {
        rcPos.top += (rcPos.GetHeight() - lEditHeight) / 2;
        rcPos.bottom = rcPos.top + lEditHeight;
    }

    CControlUI *pParent = m_pOwner;
    RECT rcParent;

    while (pParent = pParent->GetParent())
    {
        if (!pParent->IsVisible())
        {
            rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
            break;
        }

        rcParent = pParent->GetClientPos();

        if (!::IntersectRect(&rcPos, &rcPos, &rcParent))
        {
            rcPos.left = rcPos.top = rcPos.right = rcPos.bottom = 0;
            break;
        }
    }

    return rcPos;
}

LPCTSTR CIPAddressWnd::GetWindowClassName() const
{
    return _T("IPAddressWnd");
}

LPCTSTR CIPAddressWnd::GetSuperClassName() const
{
    return WC_EDIT;
}

void CIPAddressWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->Invalidate();

    // Clear reference and die
    if (m_hBkBrush != NULL) { ::DeleteObject(m_hBkBrush); }

    m_pOwner->GetManager()->RemoveNativeWindow(hWnd);
    m_pOwner->m_pWindow = NULL;
    delete this;
}

LRESULT CIPAddressWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;

    if (uMsg == WM_CREATE)
    {
        m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);

        if (m_pOwner->GetManager()->IsLayered())
        {
            ::SetTimer(m_hWnd, DEFAULT_TIMERID, ::GetCaretBlinkTime(), NULL);
        }

        bHandled = FALSE;
    }
    else if (uMsg == WM_KILLFOCUS) { lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); }
    else if (uMsg == OCM_COMMAND)
    {
        if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_CHANGE) { lRes = OnEditChanged(uMsg, wParam, lParam, bHandled); }
        else if (GET_WM_COMMAND_CMD(wParam, lParam) == EN_UPDATE)
        {
            RECT rcClient;
            ::GetClientRect(m_hWnd, &rcClient);
            ::InvalidateRect(m_hWnd, &rcClient, FALSE);
        }
    }
    else if (uMsg == WM_KEYDOWN)
    {
        if (VK_RETURN == wParam)
        {
            m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
        }
        else if (VK_RIGHT == wParam || VK_DOWN == wParam)
        {
            DWORD dwRet = ::SendMessage(m_hWnd, EM_GETSEL, 0, 0);
            lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
            // 光标位置
            WORD wIdx = LOWORD(dwRet);
            int cchLen = ::GetWindowTextLength(m_hWnd);

            if (wIdx == cchLen)
            {
                CIPAddressUI *pOwner = m_pOwner;
                EMIPField eField = m_eField;

                if (EM_FIELD4 != m_eField) { lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); }

                //切换焦点位置
                pOwner->MoveToNextField(eField);
            }
        }
        else if (VK_LEFT == wParam || VK_UP == wParam || VK_BACK == wParam)
        {
            DWORD dwRet = ::SendMessage(m_hWnd, EM_GETSEL, 0, 0);
            lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);

            // 光标位置
            if (0 == dwRet)
            {
                CIPAddressUI *pOwner = m_pOwner;
                EMIPField eField = m_eField;

                if (EM_FIELD1 != m_eField) { lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); }

                //切换焦点位置
                pOwner->MoveToBeforeField(eField);
            }
        }
    }
    else if (uMsg == OCM__BASE + WM_CTLCOLOREDIT  || uMsg == OCM__BASE + WM_CTLCOLORSTATIC)
    {
        if (m_pOwner->GetManager()->IsLayered() && !m_pOwner->GetManager()->IsPainting())
        {
            m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
        }

        DWORD clrColor = m_pOwner->GetNativeEditBkColor();

        if (clrColor == 0xFFFFFFFF) { return 0; }

        ::SetBkMode((HDC)wParam, TRANSPARENT);
        DWORD dwTextColor = m_pOwner->GetTextColor();
        ::SetTextColor((HDC)wParam, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

        if (clrColor < 0xFF000000)
        {
            if (m_hBkBrush != NULL) { ::DeleteObject(m_hBkBrush); }

            RECT rcWnd = m_pOwner->GetManager()->GetNativeWindowRect(m_hWnd);
            HBITMAP hBmpEditBk = CRenderEngine::GenerateBitmap(m_pOwner->GetManager(), rcWnd, m_pOwner, clrColor);
            m_hBkBrush = ::CreatePatternBrush(hBmpEditBk);
            ::DeleteObject(hBmpEditBk);
        }
        else
        {
            if (m_hBkBrush == NULL)
            {
                m_hBkBrush = ::CreateSolidBrush(RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
            }
        }

        return (LRESULT)m_hBkBrush;
    }
    else if (uMsg == WM_PAINT)
    {
        if (m_pOwner->GetManager()->IsLayered())
        {
            m_pOwner->GetManager()->AddNativeWindow(m_pOwner, m_hWnd);
        }

        bHandled = FALSE;
    }
    else if (uMsg == WM_PRINT)
    {
        if (m_pOwner->GetManager()->IsLayered())
        {
            lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);

            if (m_pOwner->IsEnabled() && m_bDrawCaret)    // todo:判断是否enabled
            {
                RECT rcClient;
                ::GetClientRect(m_hWnd, &rcClient);
                POINT ptCaret;
                ::GetCaretPos(&ptCaret);
                RECT rcCaret = { ptCaret.x, ptCaret.y, ptCaret.x, ptCaret.y + rcClient.bottom - rcClient.top };
                CRenderEngine::DrawLine((HDC)wParam, rcCaret, 1, 0xFF000000);
            }

            return lRes;
        }

        bHandled = FALSE;
    }
    else if (uMsg == WM_TIMER)
    {
        if (wParam == DEFAULT_TIMERID)
        {
            m_bDrawCaret = !m_bDrawCaret;
            RECT rcClient;
            ::GetClientRect(m_hWnd, &rcClient);
            ::InvalidateRect(m_hWnd, &rcClient, FALSE);
            return 0;
        }

        bHandled = FALSE;
    }
    else if (uMsg == WM_CHAR)
    {
        if (_T('.') == wParam)
        {
            CIPAddressUI *pOwner = m_pOwner;
            EMIPField eField = m_eField;

            if (EM_FIELD4 != m_eField) { lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); }

            //切换焦点位置
            pOwner->MoveToNextField(eField);
            return lRes;
        }
        else if ((wParam >= _T('0') && wParam <= _T('9')) || VK_BACK == wParam)
        {
            lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
            OnChar(uMsg, wParam, lParam, bHandled);
        }
        else if (VK_RETURN == wParam)
        {
        }
    }
    else if (uMsg == WM_PASTE)
    {
        lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        OnPaste(uMsg, wParam, lParam, bHandled);
    }

#ifndef UNICODE
    else if (uMsg == WM_IME_CHAR)
    {
        lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        // 2018-03-14
        // 1. 操作系统会把一个 WM_IME_CHAR 消息转换为两个 WM_CHAR 消息
        // 2. 编辑框只有处理了两个 WM_CHAR 消息后，该字符才会出现在编辑框中
        // 3. 为了支持在编辑框中插入、选择字符后替换输入等功能
        // 该消息不作字符过滤、正则匹配工作，而是设置标识，在 WM_CHARE 消息中处理
        m_byDChar = 0 != HIBYTE(wParam) ? 1 : 0;
    }

#endif // UNICODE
    else { bHandled = FALSE; }

    if (!bHandled) { return CWindowWnd::HandleMessage(uMsg, wParam, lParam); }

    return lRes;
}

LRESULT CIPAddressWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

    if ((HWND)wParam != m_pOwner->GetManager()->GetPaintWindow())
    {
        ::SendMessage(m_pOwner->GetManager()->GetPaintWindow(), WM_KILLFOCUS, wParam, lParam);
    }

    SendMessage(WM_CLOSE);
    return lRes;
}

LRESULT CIPAddressWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    if (!m_bInit) { return 0; }

    if (m_pOwner == NULL) { return 0; }

    // Copy text back
    int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
    ::GetWindowText(m_hWnd, m_pOwner->m_sText.GetData(cchLen), cchLen);

    // 过滤左侧字符 0
    if (m_pOwner->m_sText.GetLength() > 1 && _T('0') == m_pOwner->m_sText[0])
    {
        m_pOwner->SetText(m_pOwner->m_sText.Mid(1));
        ::SendMessage(m_hWnd, EM_SETSEL, LOWORD(-2), HIWORD(-1));
    }

    // 如果值超出上限，强制改为 255
    int n = _ttoi(m_pOwner->m_sText.GetData());

    if (n > 255)
    {
        n = 255;
        m_pOwner->SetText(_T("255"));
        ::SendMessage(m_hWnd, EM_SETSEL, LOWORD(-2), HIWORD(-1));
    }

    m_pOwner->SetField(m_eField, n);
    m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);

    if (m_pOwner->GetManager()->IsLayered()) { m_pOwner->Invalidate(); }

    return 0;
}

// bHandled 必须为TRUE
LRESULT CIPAddressWnd::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
#ifndef UNICODE

    if (0 != m_byDChar)
    {
        m_byDChar -= 1;
        return 0;
    }

#endif // UNICODE

    if (VK_BACK == wParam) { return 0; }

    WORD wIdx = LOWORD(::SendMessage(m_hWnd, EM_GETSEL, 0, 0));
    CDuiString sTxt = m_pOwner->m_sText;

    if (!IsValidChar(wParam))
    {
        ::MessageBeep(MB_ICONWARNING);
        // 删除字符
        wIdx -= 1;
#ifndef UNICODE
        LPCTSTR start = (LPCTSTR)sTxt.GetData();
        LPCTSTR end = (LPCTSTR)_mbsninc((const unsigned char *)sTxt.GetData(), wIdx);
        int n = end - start;
        sTxt = m_pOwner->m_sText.Left(n);
        sTxt += m_pOwner->m_sText.Mid(n + 1);
#else
        sTxt = m_pOwner->m_sText.Left(wIdx);
        sTxt += m_pOwner->m_sText.Mid(wIdx + 1);
#endif // UNICODE
        m_pOwner->SetText(sTxt);
        ::SendMessage(m_hWnd, EM_SETSEL, wIdx, wIdx);
    }

    return 0;
}

// bHandled 必须为TRUE
LRESULT CIPAddressWnd::OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    CDuiString strValidTxt;
    TCHAR buf[MAX_PATH] = { 0 };
    GetWindowText(m_hWnd, buf, MAX_PATH);

    GetValidText(buf, strValidTxt);

    if (strValidTxt != buf)
    {
        // 增加粘贴
        SetWindowText(m_hWnd, strValidTxt);
        ::SendMessage(m_hWnd, EM_SETSEL, LOWORD(-2), HIWORD(-1));
    }

    return 0L;
}

void CIPAddressWnd::GetValidText(LPCTSTR pstrTxt, CDuiString &strValidTxt)
{
    strValidTxt = _T("");

    for (const TCHAR *pch = pstrTxt; NULL != *pch && 0 != *pch; pch += 1)
    {
        if (IsValidChar(*pch))
        {
            strValidTxt += *pch;
        }
    }
}

bool CIPAddressWnd::IsValidChar(TCHAR ch)
{
    return (ch >= _T('0') && ch <= _T('9')) ? true : false;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CIPAddressUI::CIPAddressUI() : m_pWindow(NULL), m_dwIp(0), m_bReadOnly(false),
    m_uEditState(0), m_eField(EM_FIELD1)
{
    SetTextPadding(CDuiRect(4, 3, 4, 3));
    SetBkColor(0xFFFFFFFF);
}

CIPAddressUI::~CIPAddressUI()
{
}

LPCTSTR CIPAddressUI::GetClass() const
{
    return DUI_CTR_IPADDRESS;
}

LPVOID CIPAddressUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_IPADDRESS) == 0) { return static_cast<CIPAddressUI *>(this); }

    return CLabelUI::GetInterface(pstrName);
}

UINT CIPAddressUI::GetControlFlags() const
{
    if (!IsEnabled()) { return CControlUI::GetControlFlags(); }

    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

HWND CIPAddressUI::GetNativeWindow() const
{
    if (m_pWindow) { return m_pWindow->GetHWND(); }

    return NULL;
}

void CIPAddressUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pParent != NULL) { m_pParent->DoEvent(event); }
        else { CLabelUI::DoEvent(event); }

        return;
    }

    if (event.Type == UIEVENT_SETCURSOR && IsEnabled())
    {
        ::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_IBEAM)));
        return;
    }

    if (event.Type == UIEVENT_WINDOWSIZE)
    {
        if (m_pWindow != NULL) { m_pManager->SetFocusNeeded(this); }
    }

    if (event.Type == UIEVENT_SCROLLWHEEL)
    {
        if (m_pWindow != NULL) { return; }
    }

    if (event.Type == UIEVENT_SETFOCUS && IsEnabled())
    {
        if (m_pWindow) { return; }

        // 获取当前鼠标位置
        POINT pt;
        ::GetCursorPos(&pt);
        ::ScreenToClient(GetManager()->GetPaintWindow(), &pt);

        if (PtInRect(&m_rcField[0], pt))        { m_eField = EM_FIELD1; }
        else if (PtInRect(&m_rcField[1], pt))   { m_eField = EM_FIELD2; }
        else if (PtInRect(&m_rcField[2], pt))   { m_eField = EM_FIELD3; }
        else if (PtInRect(&m_rcField[3], pt))   { m_eField = EM_FIELD4; }
        else                                    { m_eField = EM_FIELD1; }

        m_pWindow = new CIPAddressWnd();
        ASSERT(m_pWindow);
        m_pWindow->Init(this, m_eField);
        Invalidate();
    }

    if (event.Type == UIEVENT_KILLFOCUS && IsEnabled())
    {
        Invalidate();
    }

    if (event.Type == UIEVENT_BUTTONDOWN || event.Type == UIEVENT_DBLCLICK || event.Type == UIEVENT_RBUTTONDOWN)
    {
        if (IsEnabled())
        {
            // 2018-05-28 原来代码在manager中捕获，在此释放。修改Capture机制后，由控件自己决定是否捕获
            // ReleaseCapture();
            EMIPField eField = EM_FIELD1;

            if (PtInRect(&m_rcField[0], event.ptMouse))      { eField = EM_FIELD1; }
            else if (PtInRect(&m_rcField[1], event.ptMouse)) { eField = EM_FIELD2; }
            else if (PtInRect(&m_rcField[2], event.ptMouse)) { eField = EM_FIELD3; }
            else if (PtInRect(&m_rcField[3], event.ptMouse)) { eField = EM_FIELD4; }

            if (eField != m_eField)
            {
                m_eField = eField;

                if (NULL != m_pWindow)
                {
                    m_pWindow->Close();
                    m_pWindow = NULL;
                }
            }

            if (IsFocused() && m_pWindow == NULL)
            {
                m_pWindow = new CIPAddressWnd();
                ASSERT(m_pWindow);
                m_pWindow->Init(this, m_eField);
            }
        }

        return;
    }

    if (event.Type == UIEVENT_MOUSEMOVE) { return; }

    if (event.Type == UIEVENT_BUTTONUP) { return; }

    if (event.Type == UIEVENT_CONTEXTMENU) { return; }

    if (event.Type == UIEVENT_MOUSEENTER)
    {
        if (::PtInRect(&m_rcItem, event.ptMouse))
        {
            if (IsEnabled())
            {
                if ((m_uEditState & UISTATE_HOT) == 0)
                {
                    m_uEditState |= UISTATE_HOT;
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
                if ((m_uEditState & UISTATE_HOT) != 0)
                {
                    m_uEditState &= ~UISTATE_HOT;
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

    if (event.Type == UIEVENT_CHAR)
    {
        //if (_T('.') == event.chKey)
        //{
        //    m_eField = (EM_FIELD4 == m_eField) ? EM_FIELD1 : (EMIPField)(m_eField + 1);
        //}
        //else if (VK_BACK == event.chKey)
        //{
        //    m_eField = (EM_FIELD1 == m_eField) ? EM_FIELD4 : (EMIPField)(m_eField - 1);
        //}
    }

    CLabelUI::DoEvent(event);
}

void CIPAddressUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled())
    {
        m_uEditState = 0;
    }
}

void CIPAddressUI::SetText(LPCTSTR pstrText)
{
    m_sText = pstrText;

    if (m_pWindow != NULL) { Edit_SetText(*m_pWindow, m_sText); }

    Invalidate();
}

void CIPAddressUI::SetReadOnly(bool bReadOnly)
{
    if (m_bReadOnly == bReadOnly) { return; }

    m_bReadOnly = bReadOnly;

    if (m_pWindow != NULL) { Edit_SetReadOnly(*m_pWindow, m_bReadOnly); }

    Invalidate();
}

bool CIPAddressUI::IsReadOnly() const
{
    return m_bReadOnly;
}

HWND CIPAddressUI::GetNativeEditHWND() const
{
    if (m_pWindow == NULL) { return NULL; }

    return m_pWindow->GetHWND();
}

void CIPAddressUI::SetIp(DWORD dwIp)
{
    m_dwIp = dwIp;
    Invalidate();
}

DWORD CIPAddressUI::GetIp(void)
{
    return m_dwIp;
}

void CIPAddressUI::SetIpStr(CDuiString strIp)
{
    int n1 = 0, n2 = 0, n3 = 0, n4 = 0;
    _stscanf(strIp.GetData(), _T("%d.%d.%d.%d"), &n1, &n2, &n3, &n4);
    m_dwIp = MAKEIPADDRESS(n1, n2, n3, n4);
    Invalidate();
}

DuiLib::CDuiString CIPAddressUI::GetIpStr(void)
{
    int n1 = FIRST_IPADDRESS(m_dwIp);
    int n2 = SECOND_IPADDRESS(m_dwIp);
    int n3 = THIRD_IPADDRESS(m_dwIp);
    int n4 = FOURTH_IPADDRESS(m_dwIp);
    CDuiString strIp;
    strIp.Format(_T("%d.%d.%d.%d"), n1, n2, n3, n4);
    return strIp;
}

DWORD CIPAddressUI::GetNativeEditBkColor() const
{
    return GetBkColor();
}

void CIPAddressUI::SetField(EMIPField eField, int n)
{
    int n1 = FIRST_IPADDRESS(m_dwIp);
    int n2 = SECOND_IPADDRESS(m_dwIp);
    int n3 = THIRD_IPADDRESS(m_dwIp);
    int n4 = FOURTH_IPADDRESS(m_dwIp);

    switch (eField)
    {
    case EM_FIELD1: n1 = n; break;

    case EM_FIELD2: n2 = n; break;

    case EM_FIELD3: n3 = n; break;

    case EM_FIELD4: n4 = n; break;
    }

    m_dwIp = MAKEIPADDRESS(n1, n2, n3, n4);
}

CDuiString CIPAddressUI::GetFieldStr(EMIPField eField)
{
    CDuiString strField;

    if (0 == m_dwIp) { strField = _T("0"); }
    else
    {
        switch (eField)
        {
        case EM_FIELD1: strField.Format(_T("%d"), FIRST_IPADDRESS(m_dwIp));     break;

        case EM_FIELD2: strField.Format(_T("%d"), SECOND_IPADDRESS(m_dwIp));    break;

        case EM_FIELD3: strField.Format(_T("%d"), THIRD_IPADDRESS(m_dwIp));     break;

        case EM_FIELD4: strField.Format(_T("%d"), FOURTH_IPADDRESS(m_dwIp));    break;
        }
    }

    return strField;
}

CDuiRect CIPAddressUI::GetFieldPos(EMIPField eField)
{
    CDuiRect rt;

    switch (eField)
    {
    case EM_FIELD1: rt = m_rcField[0]; break;

    case EM_FIELD2: rt = m_rcField[1]; break;

    case EM_FIELD3: rt = m_rcField[2]; break;

    case EM_FIELD4: rt = m_rcField[3]; break;

    default:        rt = m_rcField[0]; break;
    }

    return rt;
}

void CIPAddressUI::MoveToNextField(EMIPField eField)
{
    if (EM_FIELD4 == eField)
    {
        // 当前焦点在第 4 个字段
        //MessageBeep(MB_ICONWARNING);
        return;
    }

    m_eField = (EMIPField)(eField + 1);
    m_pWindow = new CIPAddressWnd();
    ASSERT(NULL != m_pWindow);
    m_pWindow->Init(this, m_eField);
    Invalidate();
}

void CIPAddressUI::MoveToBeforeField(EMIPField eField)
{
    if (EM_FIELD1 == eField)
    {
        // 当前焦点在第 1 个字段
        //MessageBeep(MB_ICONWARNING);
        return;
    }

    m_eField = (EMIPField)(eField - 1);
    m_pWindow = new CIPAddressWnd();
    ASSERT(NULL != m_pWindow);
    m_pWindow->Init(this, m_eField);
    Invalidate();
}

void CIPAddressUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);

    int nWidth = (rc.right - rc.left - m_rcTextPadding.left - m_rcTextPadding.right - DOT_WIDTH * 3) / 4;

    m_rcField[0].left = rc.left + m_rcTextPadding.left;
    m_rcField[0].right = m_rcField[0].left + nWidth;
    m_rcField[0].top = rc.top + m_rcTextPadding.top;
    m_rcField[0].bottom = rc.bottom - m_rcTextPadding.bottom;

    m_rcField[1].left = m_rcField[0].right + DOT_WIDTH;
    m_rcField[1].right = m_rcField[1].left + nWidth;
    m_rcField[1].top = m_rcField[0].top;
    m_rcField[1].bottom = m_rcField[0].bottom;

    m_rcField[2].left = m_rcField[1].right + DOT_WIDTH;
    m_rcField[2].right = m_rcField[2].left + nWidth;
    m_rcField[2].top = m_rcField[1].top;
    m_rcField[2].bottom = m_rcField[1].bottom;

    m_rcField[3].left = m_rcField[2].right + DOT_WIDTH;
    m_rcField[3].right = rc.right - m_rcTextPadding.right;
    m_rcField[3].top = m_rcField[2].top;
    m_rcField[3].bottom = m_rcField[2].bottom;
}

void CIPAddressUI::Move(SIZE szOffset, bool bNeedInvalidate)
{
    CControlUI::Move(szOffset, bNeedInvalidate);

    if (m_pWindow != NULL)
    {
        RECT rcPos = m_pWindow->CalPos();

        if (::IsRectEmpty(&rcPos)) { ::ShowWindow(m_pWindow->GetHWND(), SW_HIDE); }
        else
        {
            ::SetWindowPos(m_pWindow->GetHWND(), NULL, rcPos.left, rcPos.top, rcPos.right - rcPos.left,
                           rcPos.bottom - rcPos.top, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        }
    }
}

void CIPAddressUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);

    if (!IsVisible() && m_pWindow != NULL) { m_pManager->SetFocus(NULL); }
}

void CIPAddressUI::SetInternVisible(bool bVisible)
{
    if (!IsVisible() && m_pWindow != NULL) { m_pManager->SetFocus(NULL); }
}

SIZE CIPAddressUI::EstimateSize(SIZE szAvailable)
{
    if (m_cxyFixed.cy == 0) { return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8); }

    return CControlUI::EstimateSize(szAvailable);
}

void CIPAddressUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("readonly")) == 0) { SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("dragenable")) == 0) { DUITRACE(_T("不支持属性:dragenable")); }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { DUITRACE(_T("不支持属性:drageimage")); }
    else if (_tcscmp(pstrName, _T("dropenable")) == 0) { DUITRACE(_T("不支持属性:dropenable")); }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CLabelUI::SetAttribute(pstrName, pstrValue); }
}

void CIPAddressUI::PaintText(HDC hDC)
{
    if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }

    if (m_dwDisabledTextColor == 0) { m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor(); }

    DWORD dwTextColor = IsEnabled() ? m_dwTextColor : m_dwDisabledTextColor;
    CDuiString sText;

    // Field 1
    RECT rc = m_rcField[0];
    sText.Format(_T("%d"), FIRST_IPADDRESS(m_dwIp));
    CRenderEngine::DrawText(hDC, m_pManager, rc, sText, dwTextColor, m_iFont,
                            DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    rc.left = m_rcField[0].right;
    rc.right = m_rcField[1].left;
    CRenderEngine::DrawText(hDC, m_pManager, rc, _T("."), dwTextColor, m_iFont,
                            DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Field 2
    rc.left = m_rcField[1].left;
    rc.right = m_rcField[1].right;
    sText.Format(_T("%d"), SECOND_IPADDRESS(m_dwIp));
    CRenderEngine::DrawText(hDC, m_pManager, rc, sText, dwTextColor, m_iFont,
                            DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    rc.left = m_rcField[1].right;
    rc.right = m_rcField[2].left;
    CRenderEngine::DrawText(hDC, m_pManager, rc, _T("."), dwTextColor, m_iFont,
                            DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Field 3
    rc.left = m_rcField[2].left;
    rc.right = m_rcField[2].right;
    sText.Format(_T("%d"), THIRD_IPADDRESS(m_dwIp));
    CRenderEngine::DrawText(hDC, m_pManager, rc, sText, dwTextColor, m_iFont,
                            DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    rc.left = m_rcField[2].right;
    rc.right = m_rcField[3].left;
    CRenderEngine::DrawText(hDC, m_pManager, rc, _T("."), dwTextColor, m_iFont,
                            DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    // Field 4
    rc.left = m_rcField[3].left;
    rc.right = m_rcField[3].right;
    sText.Format(_T("%d"), FOURTH_IPADDRESS(m_dwIp));
    CRenderEngine::DrawText(hDC, m_pManager, rc, sText, dwTextColor, m_iFont,
                            DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

}
