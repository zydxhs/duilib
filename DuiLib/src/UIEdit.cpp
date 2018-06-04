#include "stdafx.h"
#include "deelx.h"

namespace DuiLib {
const TCHAR *const ALPHABET_LOWER = _T("abcdefghijklmnopqrstuvwxyz");
const TCHAR *const ALPHABET_UPPER = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const TCHAR *const BLANK = _T(" ");
const TCHAR *const NUMBERS_BIN = _T("01");
const TCHAR *const NUMBERS_DEC = _T("0123456789");
const TCHAR *const NUMBERS_HEX = _T("0123456789abcdefABCDEF");
const TCHAR *const LINUX_NOTSUPPORT =
    _T("@#$&()*/������������������������:����������������������������������~������\\|\"'[]{}<>!?; \t");
const TCHAR *const SPECIAL_SYMBOL = _T("`~!@#$%^&*()-_=+[]{};:,.<>/?'\"\\|");


class CEditWnd : public CWindowWnd
{
public:
    CEditWnd();

    void Init(CEditUI *pOwner);
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
    void GetRegExpMatch(LPCTSTR pstrTxt, CDuiString &strValidTxt);
    bool IsValidNumber(LPTSTR &pstr);
protected:
    enum
    {
        DEFAULT_TIMERID = 20,
        // 2017-07-21 zhuyadong ���� minmaxnumber ����
        CHECK_TIMERID,          // ���ڼ���û������ֵ�Ƿ�Խ��
    };

    CEditUI *m_pOwner;
    HBRUSH m_hBkBrush;
    bool m_bInit;
    bool m_bDrawCaret;
#ifndef UNICODE
    BYTE m_byDChar; // ��0��ʾ��ǰ�����ַ�Ϊ˫�ֽ��ַ�
#endif // UNICODE
};


CEditWnd::CEditWnd() : m_pOwner(NULL), m_hBkBrush(NULL), m_bInit(false), m_bDrawCaret(false)
{
#ifndef UNICODE
    m_byDChar = 0;
#endif // UNICODE
}

void CEditWnd::Init(CEditUI *pOwner)
{
    m_pOwner = pOwner;
    RECT rcPos = CalPos();
    UINT uStyle = WS_CHILD | ES_AUTOHSCROLL | pOwner->GetWindowStyls();
    UINT uTextStyle = m_pOwner->GetTextStyle();

    if (uTextStyle & DT_LEFT) { uStyle |= ES_LEFT; }
    else if (uTextStyle & DT_CENTER) { uStyle |= ES_CENTER; }
    else if (uTextStyle & DT_RIGHT) { uStyle |= ES_RIGHT; }

    if (m_pOwner->IsPasswordMode()) { uStyle |= ES_PASSWORD; }

    Create(m_pOwner->GetManager()->GetPaintWindow(), NULL, uStyle, 0, rcPos);

    HFONT hFont = NULL;
    int iFontIndex = m_pOwner->GetFont();

    if (iFontIndex != -1)
    { hFont = m_pOwner->GetManager()->GetFont(iFontIndex); }

    if (hFont == NULL)
    { hFont = m_pOwner->GetManager()->GetDefaultFontInfo()->hFont; }

    SetWindowFont(m_hWnd, hFont, TRUE);
    Edit_LimitText(m_hWnd, m_pOwner->GetMaxChar());

    if (m_pOwner->IsPasswordMode()) { Edit_SetPasswordChar(m_hWnd, m_pOwner->GetPasswordChar()); }

    Edit_SetText(m_hWnd, m_pOwner->GetText());
    Edit_SetModify(m_hWnd, FALSE);
    SendMessage(EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELPARAM(0, 0));
    Edit_Enable(m_hWnd, m_pOwner->IsEnabled() == true);
    Edit_SetReadOnly(m_hWnd, m_pOwner->IsReadOnly() == true);

    //Styls
    ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
    ::SetFocus(m_hWnd);

    if (m_pOwner->IsAutoSelAll())
    {
        int nSize = GetWindowTextLength(m_hWnd);

        if (nSize == 0) { nSize = 1; }

        Edit_SetSel(m_hWnd, 0, nSize);
    }
    else
    {
        int nSize = GetWindowTextLength(m_hWnd);
        Edit_SetSel(m_hWnd, nSize, nSize);
    }

    m_bInit = true;
}

RECT CEditWnd::CalPos()
{
    CDuiRect rcPos = m_pOwner->GetPos();
    RECT rcInset = m_pOwner->GetTextPadding();
    rcPos.left += rcInset.left;
    rcPos.top += rcInset.top;
    rcPos.right -= rcInset.right;
    rcPos.bottom -= rcInset.bottom;
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

LPCTSTR CEditWnd::GetWindowClassName() const
{
    return _T("EditWnd");
}

LPCTSTR CEditWnd::GetSuperClassName() const
{
    return WC_EDIT;
}

void CEditWnd::OnFinalMessage(HWND hWnd)
{
    m_pOwner->Invalidate();

    // Clear reference and die
    if (m_hBkBrush != NULL) { ::DeleteObject(m_hBkBrush); }

    m_pOwner->GetManager()->RemoveNativeWindow(hWnd);
    m_pOwner->m_pWindow = NULL;
    delete this;
}

LRESULT CEditWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes = 0;
    BOOL bHandled = TRUE;
    bool bTrack = false;

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
    else if (uMsg == WM_KEYDOWN && TCHAR(wParam) == VK_RETURN)
    {
        m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_RETURN);
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

            if (m_pOwner->IsEnabled() && m_bDrawCaret)    // todo:�ж��Ƿ�enabled
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
        lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        OnChar(uMsg, wParam, lParam, bHandled);
    }
    else if (uMsg == WM_PASTE)
    {
        lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        OnPaste(uMsg, wParam, lParam, bHandled);
    }
    // 2018-06-02 �޸��༭���ȡ�������ʾTooltip������
    else if (uMsg == WM_MOUSEMOVE)
    {
        if (!bTrack && !(wParam & MK_LBUTTON))
        {
            int iHoverTime = m_pOwner->GetManager()->GetHoverTime();
            TRACKMOUSEEVENT tme = { 0 };
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_HOVER | TME_LEAVE;
            tme.hwndTrack = m_hWnd;
            tme.dwHoverTime = iHoverTime;
            _TrackMouseEvent(&tme);
            bTrack = true;
        }
        else { bHandled = FALSE; }
    }
    else if (uMsg == WM_MOUSELEAVE)
    {
        bTrack = false;
    }
    else if (uMsg == WM_MOUSEHOVER)
    {
        bTrack = false;
        m_pOwner->GetManager()->MessageHandler(WM_MOUSEHOVER, wParam, lParam, lRes);
    }

#ifndef UNICODE
    else if (uMsg == WM_IME_CHAR)
    {
        lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
        // 2018-03-14
        // 1. ����ϵͳ���һ�� WM_IME_CHAR ��Ϣת��Ϊ���� WM_CHAR ��Ϣ
        // 2. �༭��ֻ�д��������� WM_CHAR ��Ϣ�󣬸��ַ��Ż�����ڱ༭����
        // 3. Ϊ��֧���ڱ༭���в��롢ѡ���ַ����滻����ȹ���
        // ����Ϣ�����ַ����ˡ�����ƥ�乤�����������ñ�ʶ���� WM_CHARE ��Ϣ�д���
        m_byDChar = 0 != HIBYTE(wParam) ? 1 : 0;
    }

#endif // UNICODE
    else { bHandled = FALSE; }

    if (!bHandled) { return CWindowWnd::HandleMessage(uMsg, wParam, lParam); }

    return lRes;
}

LRESULT CEditWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    LRESULT lRes = ::DefWindowProc(m_hWnd, uMsg, wParam, lParam);

    if ((HWND)wParam != m_pOwner->GetManager()->GetPaintWindow())
    {
        ::SendMessage(m_pOwner->GetManager()->GetPaintWindow(), WM_KILLFOCUS, wParam, lParam);
    }

    SendMessage(WM_CLOSE);
    return lRes;
}

LRESULT CEditWnd::OnEditChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL & /*bHandled*/)
{
    if (!m_bInit) { return 0; }

    if (m_pOwner == NULL) { return 0; }

    // Copy text back
    int cchLen = ::GetWindowTextLength(m_hWnd) + 1;
    LPTSTR pstr = static_cast<LPTSTR>(_alloca(cchLen * sizeof(TCHAR)));
    ASSERT(pstr);

    if (pstr == NULL) { return 0; }

    ::GetWindowText(m_hWnd, pstr, cchLen);

    if (m_pOwner->IsNumberOnly() && !IsValidNumber(pstr)) { MessageBeep(MB_ICONWARNING); }

    m_pOwner->m_sText = pstr;
    m_pOwner->GetManager()->SendNotify(m_pOwner, DUI_MSGTYPE_TEXTCHANGED);

    if (m_pOwner->GetManager()->IsLayered()) { m_pOwner->Invalidate(); }


    return 0;
}

// bHandled ����ΪTRUE
LRESULT CEditWnd::OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
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

    if ((m_pOwner->IsCharFilter() && !m_pOwner->IsValidChar(wParam)) ||
        (m_pOwner->IsRegExpFilter() && !m_pOwner->IsRegExpMatch(sTxt.GetData())))
    {
        ::MessageBeep(MB_ICONWARNING);
        // ɾ���ַ�
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

// bHandled ����ΪTRUE
LRESULT CEditWnd::OnPaste(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (!m_pOwner->IsCharFilter() && !m_pOwner->IsRegExpFilter()) { return 0L; }

    CDuiString strValidTxt;
    TCHAR buf[MAX_PATH] = { 0 };
    GetWindowText(m_hWnd, buf, MAX_PATH);

    if (m_pOwner->IsCharFilter()) { GetValidText(buf, strValidTxt); }
    else if (m_pOwner->IsRegExpFilter()) { GetRegExpMatch(buf, strValidTxt); }

    if (strValidTxt != buf)
    {
        // ����ճ��
        SetWindowText(m_hWnd, strValidTxt);
        ::SendMessage(m_hWnd, EM_SETSEL, LOWORD(-2), HIWORD(-1));
    }

    return 0L;
}

void CEditWnd::GetValidText(LPCTSTR pstrTxt, CDuiString &strValidTxt)
{
    strValidTxt = _T("");

    for (const TCHAR *pch = pstrTxt; NULL != *pch && 0 != *pch; pch += 1)
    {
        if (m_pOwner->IsValidChar(*pch))
        {
            strValidTxt += *pch;
        }
    }
}

void CEditWnd::GetRegExpMatch(LPCTSTR pstrTxt, CDuiString &strValidTxt)
{
    strValidTxt = m_pOwner->IsRegExpMatch(pstrTxt) ? pstrTxt : _T("");
}

bool CEditWnd::IsValidNumber(LPTSTR &pstr)
{
    // 2018-03-07 zhuyadong ɾ����������0
    int nLen = (int)_tcslen(pstr);
    WORD wIdx = LOWORD(::SendMessage(m_hWnd, EM_GETSEL, 0, 0));

    if (nLen > 1 && 0 != wIdx)
    {
        int i = 0;

        for (; i < nLen; ++i)
        {
            if (_T('0') != pstr[i]) { break; }
        }

        if (0 != i)
        {
            i = (i == nLen) ? --i : i;
            pstr += i;
            SetWindowText(m_hWnd, pstr);
            ::SendMessage(m_hWnd, WM_KEYDOWN, VK_END, 0);
        }
    }

    int nValue = _ttoi(pstr);

    if (nValue < m_pOwner->GetMinNumber())
    {
        _stprintf(pstr, _T("%d"), m_pOwner->GetMinNumber());
        SetWindowText(m_hWnd, pstr);
        ::SendMessage(m_hWnd, WM_KEYDOWN, VK_END, 0);
        return false;
    }

    if (nValue > m_pOwner->GetMaxNumber())
    {
        _stprintf(pstr, _T("%d"), m_pOwner->GetMaxNumber());
        SetWindowText(m_hWnd, pstr);
        ::SendMessage(m_hWnd, WM_KEYDOWN, VK_END, 0);
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

CEditUI::CEditUI() : m_pWindow(NULL), m_uMaxChar(255), m_bReadOnly(false),
    m_bPasswordMode(false), m_cPasswordChar(_T('*')), m_bAutoSelAll(false), m_uButtonState(0),
    m_iWindowStyls(0), m_dwTipColor(0xFFBAC0C5), m_nMinNumber(0), m_nMaxNumber(0),
    m_bCharFilter(false), m_bWiteList(true), m_bRegExp(false), m_pRegExp(NULL)
{
    SetTextPadding(CDuiRect(4, 3, 4, 3));
    SetBkColor(0xFFFFFFFF);
}

CEditUI::~CEditUI()
{
    if (NULL != m_pRegExp)
    {
        delete m_pRegExp;
        m_pRegExp = NULL;
    }
}

LPCTSTR CEditUI::GetClass() const
{
    return DUI_CTR_EDIT;
}

LPVOID CEditUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_EDIT) == 0) { return static_cast<CEditUI *>(this); }

    return CLabelUI::GetInterface(pstrName);
}

UINT CEditUI::GetControlFlags() const
{
    if (!IsEnabled()) { return CControlUI::GetControlFlags(); }

    return UIFLAG_SETCURSOR | UIFLAG_TABSTOP;
}

HWND CEditUI::GetNativeWindow() const
{
    if (m_pWindow) { return m_pWindow->GetHWND(); }

    return NULL;
}

void CEditUI::DoEvent(TEventUI &event)
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

        m_pWindow = new CEditWnd();
        ASSERT(m_pWindow);
        m_pWindow->Init(this);
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
            // 2018-05-28 ԭ��������manager�в����ڴ��ͷš��޸�Capture���ƺ��ɿؼ��Լ������Ƿ񲶻�
            // ReleaseCapture();

            if (IsFocused() && m_pWindow == NULL)
            {
                m_pWindow = new CEditWnd();
                ASSERT(m_pWindow);
                m_pWindow->Init(this);
            }
            else if (m_pWindow != NULL)
            {
                if (!m_bAutoSelAll)
                {
                    POINT pt = event.ptMouse;
                    pt.x -= m_rcItem.left + m_rcTextPadding.left;
                    pt.y -= m_rcItem.top + m_rcTextPadding.top;
                    Edit_SetSel(*m_pWindow, 0, 0);
                    ::SendMessage(*m_pWindow, WM_LBUTTONDOWN, event.wParam, MAKELPARAM(pt.x, pt.y));
                }
            }
        }

        return;
    }

    if (event.Type == UIEVENT_MOUSEMOVE)
    {
        if ((event.wParam & MK_LBUTTON) && m_bDragEnable) { OnDoDragDrop(event); }

        return;
    }

    if (event.Type == UIEVENT_BUTTONUP)
    {
        return;
    }

    if (event.Type == UIEVENT_CONTEXTMENU)
    {
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

    CLabelUI::DoEvent(event);
}

void CEditUI::SetEnabled(bool bEnable)
{
    CControlUI::SetEnabled(bEnable);

    if (!IsEnabled())
    {
        m_uButtonState = 0;
    }
}

void CEditUI::SetText(LPCTSTR pstrText)
{
    m_sText = pstrText;

    if (m_pWindow != NULL) { Edit_SetText(*m_pWindow, m_sText); }

    Invalidate();
}

void CEditUI::SetMaxChar(UINT uMax)
{
    m_uMaxChar = uMax;

    if (m_pWindow != NULL) { Edit_LimitText(*m_pWindow, m_uMaxChar); }
}

UINT CEditUI::GetMaxChar()
{
    return m_uMaxChar;
}

void CEditUI::SetReadOnly(bool bReadOnly)
{
    if (m_bReadOnly == bReadOnly) { return; }

    m_bReadOnly = bReadOnly;

    if (m_pWindow != NULL) { Edit_SetReadOnly(*m_pWindow, m_bReadOnly); }

    Invalidate();
}

bool CEditUI::IsReadOnly() const
{
    return m_bReadOnly;
}

void CEditUI::SetNumberOnly(bool bNumberOnly)
{
    if (bNumberOnly)
    {
        m_iWindowStyls |= ES_NUMBER;
    }
    else
    {
        m_iWindowStyls &= ~ES_NUMBER;
    }
}

bool CEditUI::IsNumberOnly() const
{
    return m_iWindowStyls & ES_NUMBER ? true : false;
}

int CEditUI::GetWindowStyls() const
{
    return m_iWindowStyls;
}

HWND CEditUI::GetNativeEditHWND() const
{
    if (m_pWindow == NULL) { return NULL; }

    return m_pWindow->GetHWND();
}

void CEditUI::SetPasswordMode(bool bPasswordMode)
{
    if (m_bPasswordMode == bPasswordMode) { return; }

    m_bPasswordMode = bPasswordMode;
    Invalidate();
}

bool CEditUI::IsPasswordMode() const
{
    return m_bPasswordMode;
}

void CEditUI::SetPasswordChar(TCHAR cPasswordChar)
{
    if (m_cPasswordChar == cPasswordChar) { return; }

    m_cPasswordChar = cPasswordChar;

    if (m_pWindow != NULL) { Edit_SetPasswordChar(*m_pWindow, m_cPasswordChar); }

    Invalidate();
}

TCHAR CEditUI::GetPasswordChar() const
{
    return m_cPasswordChar;
}

bool CEditUI::IsAutoSelAll()
{
    return m_bAutoSelAll;
}

void CEditUI::SetAutoSelAll(bool bAutoSelAll)
{
    m_bAutoSelAll = bAutoSelAll;
}

LPCTSTR CEditUI::GetNormalImage()
{
    return m_diNormal.sDrawString;
}

void CEditUI::SetNormalImage(LPCTSTR pStrImage)
{
    if (m_diNormal.sDrawString == pStrImage && m_diNormal.pImageInfo != NULL) { return; }

    m_diNormal.Clear();
    m_diNormal.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetHotImage()
{
    return m_diHot.sDrawString;
}

void CEditUI::SetHotImage(LPCTSTR pStrImage)
{
    if (m_diHot.sDrawString == pStrImage && m_diHot.pImageInfo != NULL) { return; }

    m_diHot.Clear();
    m_diHot.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetFocusedImage()
{
    return m_diFocused.sDrawString;
}

void CEditUI::SetFocusedImage(LPCTSTR pStrImage)
{
    if (m_diFocused.sDrawString == pStrImage && m_diFocused.pImageInfo != NULL) { return; }

    m_diFocused.Clear();
    m_diFocused.sDrawString = pStrImage;
    Invalidate();
}

LPCTSTR CEditUI::GetDisabledImage()
{
    return m_diDisabled.sDrawString;
}

void CEditUI::SetDisabledImage(LPCTSTR pStrImage)
{
    if (m_diDisabled.sDrawString == pStrImage && m_diDisabled.pImageInfo != NULL) { return; }

    m_diDisabled.Clear();
    m_diDisabled.sDrawString = pStrImage;
    Invalidate();
}

DWORD CEditUI::GetNativeEditBkColor() const
{
    return GetBkColor();
}

void CEditUI::SetSel(long nStartChar, long nEndChar)
{
    if (m_pWindow != NULL) { Edit_SetSel(*m_pWindow, nStartChar, nEndChar); }
}

void CEditUI::SetSelAll()
{
    SetSel(0, -1);
}

void CEditUI::SetReplaceSel(LPCTSTR lpszReplace)
{
    if (m_pWindow != NULL) { Edit_ReplaceSel(*m_pWindow, lpszReplace); }
}

void CEditUI::SetPos(RECT rc, bool bNeedInvalidate)
{
    CControlUI::SetPos(rc, bNeedInvalidate);

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

void CEditUI::Move(SIZE szOffset, bool bNeedInvalidate)
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

void CEditUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);

    if (!IsVisible() && m_pWindow != NULL) { m_pManager->SetFocus(NULL); }
}

void CEditUI::SetInternVisible(bool bVisible)
{
    if (!IsVisible() && m_pWindow != NULL) { m_pManager->SetFocus(NULL); }
}

SIZE CEditUI::EstimateSize(SIZE szAvailable)
{
    if (m_cxyFixed.cy == 0) { return CDuiSize(m_cxyFixed.cx, m_pManager->GetFontInfo(GetFont())->tm.tmHeight + 8); }

    return CLabelUI::EstimateSize(szAvailable);
}

void CEditUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("readonly")) == 0) { SetReadOnly(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("numberonly")) == 0) { SetNumberOnly(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("password")) == 0) { SetPasswordMode(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("autoselall")) == 0) { SetAutoSelAll(_tcscmp(pstrValue, _T("true")) == 0); }
    else if (_tcscmp(pstrName, _T("maxchar")) == 0) { SetMaxChar(_ttoi(pstrValue)); }
    else if (_tcscmp(pstrName, _T("normalimage")) == 0) { SetNormalImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("hotimage")) == 0) { SetHotImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("focusedimage")) == 0) { SetFocusedImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("disabledimage")) == 0) { SetDisabledImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("tiptext")) == 0) { SetTipText(pstrValue); }
    else if (_tcscmp(pstrName, _T("tipcolor")) == 0)
    {
        while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) { pstrValue = ::CharNext(pstrValue); }

        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetTipColor(clrColor);
    }
    // 2017-07-21 zhuyadong ���� minmaxnumber ����
    else if (_tcscmp(pstrName, _T("minmaxnumber")) == 0)
    {
        TCHAR *pDot = _tcschr((LPTSTR)pstrValue, _T(','));

        if (NULL != pDot)
        {
            *pDot = _T('\0');
            UINT dwMin = _ttoi(pstrValue);
            UINT dwMax = _ttoi(pDot + 1);
            SetMinMaxNumber(dwMin, dwMax);
        }
    }
    else if (_tcscmp(pstrName, _T("charfilter")) == 0)
    {
        SetCharFilter((_tcscmp(pstrValue, _T("true")) == 0) ? true : false);
    }
    else if (_tcscmp(pstrName, _T("whitelist")) == 0)
    {
        m_bWiteList = ((_tcscmp(pstrValue, _T("true")) == 0) ? true : false);
    }
    else if (_tcscmp(pstrName, _T("regexp")) == 0)
    {
        SetRegExpFilter((_tcscmp(pstrValue, _T("true")) == 0) ? true : false);
    }
    else if (_tcscmp(pstrName, _T("dragenable")) == 0) { DUITRACE(_T("��֧������:dragenable")); }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { DUITRACE(_T("��֧������:drageimage")); }
    else { CLabelUI::SetAttribute(pstrName, pstrValue); }
}

void CEditUI::PaintStatusImage(HDC hDC)
{
    if (IsFocused()) { m_uButtonState |= UISTATE_FOCUSED; }
    else { m_uButtonState &= ~ UISTATE_FOCUSED; }

    if (!IsEnabled()) { m_uButtonState |= UISTATE_DISABLED; }
    else { m_uButtonState &= ~ UISTATE_DISABLED; }

    if ((m_uButtonState & UISTATE_DISABLED) != 0)
    {
        if (DrawImage(hDC, m_diDisabled)) { return; }
    }
    else if ((m_uButtonState & UISTATE_FOCUSED) != 0)
    {
        if (DrawImage(hDC, m_diFocused)) { return; }
    }
    else if ((m_uButtonState & UISTATE_HOT) != 0)
    {
        if (DrawImage(hDC, m_diHot)) { return; }
    }

    if (DrawImage(hDC, m_diNormal)) { return; }
}

void CEditUI::PaintText(HDC hDC)
{
    if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }

    if (m_dwDisabledTextColor == 0) { m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor(); }

    DWORD mCurTextColor = m_dwTextColor;
    CDuiString sText;

    if (m_sText.IsEmpty() && !m_sTipText.IsEmpty())
    {
        mCurTextColor = m_dwTipColor;
        sText = m_sTipText;
    }
    else
    {
        if (m_bPasswordMode)
        {
            LPCTSTR p = m_sText.GetData();

            while (*p != _T('\0'))
            {
                sText += m_cPasswordChar;
                p = ::CharNext(p);
            }
        }
        else
        {
            sText = m_sText;
        }
    }

    if (sText.IsEmpty()) { return; }

    RECT rc = m_rcItem;
    rc.left += m_rcTextPadding.left;
    rc.right -= m_rcTextPadding.right;
    rc.top += m_rcTextPadding.top;
    rc.bottom -= m_rcTextPadding.bottom;

    if (IsEnabled())
    {
        CRenderEngine::DrawText(hDC, m_pManager, rc, sText, mCurTextColor, \
                                m_iFont, DT_SINGLELINE | m_uTextStyle);
    }
    else
    {
        CRenderEngine::DrawText(hDC, m_pManager, rc, sText, m_dwDisabledTextColor, \
                                m_iFont, DT_SINGLELINE | m_uTextStyle);

    }
}

//2017-02-25 zhuyadong ������ʾ���ּ�����ɫ
void CEditUI::SetTipText(LPCTSTR pstrTip)
{
    m_sTipText = pstrTip;
    m_sTipTextOrig = m_sText;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sTipText);
    Invalidate();
}

LPCTSTR CEditUI::GetTipText()
{
    return m_sTipText.GetData();
}

void CEditUI::ReloadText(void)
{
    m_sTipText = m_sTipTextOrig;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sTipText);
    CLabelUI::ReloadText();
}

void CEditUI::SetTipColor(DWORD dwColor)
{
    m_dwTipColor = dwColor;
}

DWORD CEditUI::GetTipColor()
{
    return m_dwTipColor;
}
// 2017-07-21 zhuyadong ���� minmaxnumber ����
void CEditUI::SetMinMaxNumber(int nMin, int nMax)
{
    m_nMinNumber = nMin;
    m_nMaxNumber = nMax;
}
int CEditUI::GetMinNumber()
{
    return m_nMinNumber;
}
int CEditUI::GetMaxNumber()
{
    return m_nMaxNumber;
}

void CEditUI::SetCharFilter(bool bFilter)
{
    ASSERT(!m_bRegExp);

    if (!m_bRegExp)
    {
        m_bCharFilter = bFilter;
        m_iWindowStyls = bFilter ? (m_iWindowStyls &= ~ES_NUMBER) : m_iWindowStyls;
    }
}

bool CEditUI::IsCharFilter(void)
{
    return m_bCharFilter;
}

void CEditUI::SetFilterMode(bool bWiteList)
{
    m_bWiteList = bWiteList;
}

bool CEditUI::GetFilterMode(void)
{
    return m_bWiteList;
}

bool CEditUI::IsValidChar(TCHAR ch)
{
    int nFind = m_sFilterCharSet.Find(ch);

    if (m_bWiteList)
    {
        return (nFind > -1) ? true : false;
    }
    else
    {
        return (nFind < 0) ? true : false;
    }
}

#ifndef UNICODE
bool CEditUI::IsValidChar(LPCTSTR pstr)
{
    int nFind = m_sFilterCharSet.Find(pstr);

    if (m_bWiteList)
    {
        return (nFind > -1) ? true : false;
    }
    else
    {
        return (nFind < 0) ? true : false;
    }
}
#endif // UNICODE

DuiLib::CDuiString CEditUI::GetFilterCharSet(void)
{
    return m_sFilterCharSet;
}

void CEditUI::SetFilterCharSet(CDuiString sCharSet)
{
    m_sFilterCharSet = sCharSet;
}

void CEditUI::SetFilterCharSet(int nFilterType)
{
    m_sFilterCharSet = _T("");

    if (nFilterType & EFILTER_CHAR_UPPER)
    {
        m_sFilterCharSet += ALPHABET_UPPER;
    }

    if (nFilterType & EFILTER_CHAR_LOWER)
    {
        m_sFilterCharSet += ALPHABET_LOWER;
    }

    if (nFilterType & EFILTER_CHAR_BLANK)
    {
        m_sFilterCharSet += BLANK;
    }

    if (nFilterType & EFILTER_SPECIAL_SYMBOL)
    {
        m_sFilterCharSet += SPECIAL_SYMBOL;
    }

    if (nFilterType & EFILTER_CHAR_LINUX)
    {
        m_sFilterCharSet += LINUX_NOTSUPPORT;
    }

    if (nFilterType & EFILTER_NUMBER_BIN)
    {
        m_sFilterCharSet += NUMBERS_BIN;
    }

    if (nFilterType & EFILTER_NUMBER_DEC)
    {
        m_sFilterCharSet += NUMBERS_DEC;
    }

    if (nFilterType & EFILTER_NUMBER_HEX)
    {
        m_sFilterCharSet += NUMBERS_HEX;
    }
}

void CEditUI::AppendFilterCharSet(CDuiString sCharSet)
{
    TCHAR ch;

    for (int i = 0; i < sCharSet.GetLength(); ++i)
    {
        ch = sCharSet.GetAt(i);

        if (m_sFilterCharSet.Find(ch) < 0)
        {
            m_sFilterCharSet += ch;
        }
    }
}

void CEditUI::SetRegExpFilter(bool bRegExp)
{
    ASSERT(!m_bCharFilter);

    if (!m_bCharFilter)
    {
        m_bRegExp = bRegExp;
        m_iWindowStyls = bRegExp ? (m_iWindowStyls &= ~ES_NUMBER) : m_iWindowStyls;
    }
}

bool CEditUI::IsRegExpFilter(void)
{
    return m_bRegExp;
}

void CEditUI::SetRegExpPattern(CDuiString sRegExp)
{
    if (NULL == m_pRegExp) { m_pRegExp = new CRegexpT<TCHAR>(NULL, 0); }

    if (NULL != m_pRegExp)
    {
        //CDuiString sRE;
        //sRE.Append(_T("\\Q")).Append(sRegExp).Append(_T("\\E"));
        //m_pRegExp->Compile(sRE.GetData());
        if (sRegExp.GetAt(0) != _T('^')) { sRegExp.Format(_T("%c%s"), _T('^'), sRegExp); }

        if (sRegExp.GetAt(sRegExp.GetLength() - 1) != _T('$')) { sRegExp.Append(_T("$")); }

        m_pRegExp->Compile(sRegExp.GetData());
    }
}

bool CEditUI::IsRegExpMatch(TCHAR ch)
{
    if (NULL == m_pRegExp) { return false; }

    CDuiString sTxt = m_sText;
    sTxt += ch;

    int result = m_pRegExp->MatchExact(sTxt.GetData()).IsMatched();
    return (0 != result) ? true : false;
}

bool CEditUI::IsRegExpMatch(LPCTSTR pstr)
{
    if (NULL == m_pRegExp || NULL == pstr) { return false; }

    int result = m_pRegExp->MatchExact(pstr).IsMatched();
    return (0 != result) ? true : false;
}

}