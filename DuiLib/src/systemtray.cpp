#include "stdafx.h"
#include <ctime>
#include <vector>

namespace DuiLib {

using namespace std;
typedef vector<HICON> CVecHIcon;

class CSystemTrayImpl
{
public:
    CSystemTrayImpl();
    virtual ~CSystemTrayImpl();

    BOOL Create(CPaintManagerUI *pTargetPM, LPCTSTR szTip, HICON hIcon, STRINGorID xml, LPCTSTR pSkinType = NULL,
                BOOL bHidden = FALSE, LPCTSTR szBalloonText = NULL, LPCTSTR szBalloonTitle = NULL,
                DWORD dwBalloonIcon = NIIF_NONE, UINT uBalloonTimeout = 10);

    BOOL IsEnable();
    BOOL IsVisible();

    // Change or retrieve the Tooltip text
    BOOL   SetTooltipText(LPCTSTR pszTooltipText);
    BOOL   SetTooltipText(UINT nID);
    CDuiString GetTooltipText(void) const;

    // Change or retrieve the icon displayed
    BOOL  SetIcon(HICON hIcon);
    BOOL  SetIcon(LPCTSTR lpszIconName);
    BOOL  SetIcon(UINT nIDResource);
    BOOL  SetStandardIcon(LPCTSTR lpIconName);
    BOOL  SetStandardIcon(UINT nIDResource);
    HICON GetIcon() const;

    void  SetFocus();
    BOOL  HideIcon();
    BOOL  ShowIcon();
    BOOL  AddIcon();
    BOOL  RemoveIcon();
    BOOL  MoveToRight();

    BOOL ShowBalloon(LPCTSTR szText, LPCTSTR szTitle = NULL,
                     DWORD dwIcon = NIIF_NONE, UINT uTimeout = 10);

    // For icon animation
    BOOL  SetIconList(UINT uFirstIconID, UINT uLastIconID);
    BOOL  SetIconList(HICON *pHIconList, UINT nNumIcons);
    BOOL  Animate(UINT nDelayMilliSeconds, int nNumSeconds = -1);
    BOOL  StepAnimation();
    BOOL  StopAnimation();

    // Change menu default item
    void  GetMenuDefaultItem(CDuiString &sItem);
    BOOL  SetMenuDefaultItem(const CDuiString &sItem);

    // Change or retrieve the window to send icon notification messages to
    // BOOL  SetNotificationWnd(HWND hNotifyWnd);
    // HWND  GetNotificationWnd() const;

    // Change or retrieve the window to send menu commands to
    BOOL  SetTargetWnd(CPaintManagerUI *pTargetPM);
    CPaintManagerUI *GetTargetWnd() const;

    // Change or retrieve  notification messages sent to the window
    BOOL  SetCallbackMessage(UINT uCallbackMessage);
    UINT  GetCallbackMessage() const;

    HWND  GetHWnd() const;
    UINT_PTR GetTimerID() const;

    // Static callback functions and data
public:
    static CSystemTrayImpl *m_pThis;
    static LRESULT PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    static void MinimiseToTray(HWND hWnd);
    static void MaximiseFromTray(HWND hWnd);

protected:
    static const UINT m_nTimerID;
    static UINT  m_nMaxTooltipLength;
    static const UINT m_nTaskbarCreatedMsg;
    static HWND  m_hWndInvisible;

    static BOOL GetDoWndAnimation();
    static void GetTrayWndRect(LPRECT lprect);
    static BOOL RemoveTaskbarIcon(HWND hWnd);

    // message map functions
public:
    LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
    LRESULT OnTimer(UINT nIDEvent);
    // Default handler for tray notification message
    virtual LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
    LRESULT OnSettingChange(UINT uFlags, LPCTSTR lpszSection);

private:
    ATOM RegisterClass(HINSTANCE hInstance);
    void InstallIconPending();

private:
    CPaintManagerUI *m_pTargetPM;       // Window that menu commands are sent
    NOTIFYICONDATA  m_tnd;
    HWND            m_hWnd;

    BOOL            m_bEnabled;         // does O/S support tray icon?
    BOOL            m_bHidden;          // Has the icon been hidden?
    BOOL            m_bRemoved;         // Has the icon been removed?
    BOOL            m_bShowIconPending; // Show the icon once tha taskbar has been created
    BOOL            m_bWin2K;           // Use new W2K features?

    CVecHIcon       m_IconList;
    UINT_PTR        m_uIDTimer;
    int             m_nCurrentIcon;
    time_t          m_StartTime;
    int             m_nAnimationPeriod;
    HICON           m_hSavedIcon;
    UINT            m_uCreationFlags;

    // 默认菜单项
    CDuiString      m_sItemName;        // 菜单项的 名字
    CDuiString      m_sItemUserData;    // 菜单项的 用户数据
    UINT_PTR        m_ptrItemTag;       // 菜单项的 Tag
    // 菜单资源
    STRINGorID      m_xml;
    CDuiString      m_sSkinType;
};

const UINT CSystemTrayImpl::m_nTimerID = 4567;
const UINT CSystemTrayImpl::m_nTaskbarCreatedMsg = ::RegisterWindowMessage(_T("TaskbarCreated"));

CSystemTrayImpl *CSystemTrayImpl::m_pThis = NULL;
UINT CSystemTrayImpl::m_nMaxTooltipLength = 0;
HWND CSystemTrayImpl::m_hWndInvisible = NULL;

#define TRAYICON_CLASS _T("TrayIconClass")

CSystemTrayImpl::CSystemTrayImpl()
    : m_pTargetPM(NULL)
    , m_bEnabled(FALSE)
    , m_bHidden(TRUE)
    , m_bRemoved(TRUE)
    , m_bShowIconPending(FALSE)
    , m_uIDTimer(0)
    , m_hSavedIcon(NULL)
    , m_uCreationFlags(0)
    , m_ptrItemTag(0)
    , m_xml(_T(""))
    , m_hWnd(NULL)
    , m_nCurrentIcon(NULL)
    , m_StartTime(0)
    , m_nAnimationPeriod(0)
{
    m_pThis = this;
    memset(&m_tnd, 0, sizeof(m_tnd));
    OSVERSIONINFO os = { sizeof(os) };
    GetVersionEx(&os);
    m_bWin2K = (VER_PLATFORM_WIN32_NT == os.dwPlatformId && os.dwMajorVersion >= 5);
    m_bEnabled = m_bWin2K;

}

CSystemTrayImpl::~CSystemTrayImpl()
{
    RemoveIcon();
    m_IconList.clear();

    if (m_hWnd) { ::DestroyWindow(m_hWnd); m_hWnd = NULL; }
}

BOOL CSystemTrayImpl::Create(CPaintManagerUI *pTargetPM, LPCTSTR szTip, HICON hIcon, STRINGorID xml,
                             LPCTSTR pSkinType, BOOL bHidden, LPCTSTR szBalloonText, LPCTSTR szBalloonTitle,
                             DWORD dwBalloonIcon, UINT uBalloonTimeout)
{
    if (!m_bEnabled) { ASSERT(m_bEnabled); return FALSE; }

    m_nMaxTooltipLength = _countof(m_tnd.szTip);

    // Tray only supports tooltip text up to m_nMaxTooltipLength) characters
    ASSERT(_tcslen(szTip) <= m_nMaxTooltipLength);

    RegisterClass(CPaintManagerUI::GetInstance());

    // Create an invisible window
    m_hWnd = ::CreateWindow(TRAYICON_CLASS, _T(""), WS_POPUP,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            NULL, 0, CPaintManagerUI::GetInstance(), 0);

    m_pTargetPM = pTargetPM;
    m_xml = xml;
    m_sSkinType = pSkinType;
    // load up the NOTIFYICONDATA structure
    m_tnd.cbSize = sizeof(NOTIFYICONDATA);
    m_tnd.hWnd = m_hWnd;
    // m_tnd.uID = uID;
    m_tnd.hIcon = hIcon;
    m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    ASSERT(WM_ICON_NOTIFY >= WM_APP);
    m_tnd.uCallbackMessage = WM_ICON_NOTIFY;
    _tcsncpy(m_tnd.szTip, szTip, m_nMaxTooltipLength);

    if (m_bWin2K && szBalloonText)
    {
        // The balloon tooltip text can be up to 255 chars long.
        ASSERT(szBalloonText && lstrlen(szBalloonText) < 256);

        // The balloon title text can be up to 63 chars long.
        ASSERT(szBalloonTitle && lstrlen(szBalloonTitle) < 64);

        // dwBalloonIcon must be valid.
        ASSERT(NIIF_NONE == dwBalloonIcon || NIIF_INFO == dwBalloonIcon ||
               NIIF_WARNING == dwBalloonIcon || NIIF_ERROR == dwBalloonIcon);

        // The timeout must be between 10 and 30 seconds.
        ASSERT(uBalloonTimeout >= 10 && uBalloonTimeout <= 30);

        m_tnd.uFlags |= NIF_INFO;

        if (szBalloonText)   { _tcsncpy(m_tnd.szInfo, szBalloonText, 255); }
        else                { m_tnd.szInfo[0] = _T('\0'); }

        if (szBalloonTitle) { _tcsncpy(m_tnd.szInfoTitle, szBalloonTitle, 63); }
        else                { m_tnd.szInfoTitle[0] = _T('\0'); }

        m_tnd.uTimeout = uBalloonTimeout * 1000; // convert time to ms
        m_tnd.dwInfoFlags = dwBalloonIcon;
    }

    m_bHidden = bHidden;

    if (m_bWin2K && m_bHidden)
    {
        m_tnd.uFlags = NIF_STATE;
        m_tnd.dwState = NIS_HIDDEN;
        m_tnd.dwStateMask = NIS_HIDDEN;
    }

    m_uCreationFlags = m_tnd.uFlags;    // Store in case we need to recreate in OnTaskBarCreate
    BOOL bResult = TRUE;

    if (!m_bHidden || m_bWin2K)
    {
        bResult = Shell_NotifyIcon(NIM_ADD, &m_tnd);
        m_bShowIconPending = m_bHidden = m_bRemoved = !bResult;
    }

    // Zero out the balloon text string so that later operations won't redisplay the balloon.
    if (m_bWin2K && szBalloonText) { m_tnd.szInfo[0] = _T('\0'); }

    return bResult;
}

BOOL CSystemTrayImpl::IsEnable() { return m_bEnabled; }

BOOL CSystemTrayImpl::IsVisible() { return m_bHidden; }

BOOL CSystemTrayImpl::SetTooltipText(LPCTSTR pszTooltipText)
{
    ASSERT(_tcslen(pszTooltipText) < m_nMaxTooltipLength);

    if (!m_bEnabled) { return FALSE; }

    m_tnd.uFlags = NIF_TIP;
    _tcsncpy(m_tnd.szTip, pszTooltipText, m_nMaxTooltipLength - 1);

    if (m_bHidden) { return TRUE; }
    else { return Shell_NotifyIcon(NIM_MODIFY, &m_tnd); }
}

BOOL CSystemTrayImpl::SetTooltipText(UINT nID)
{
    TCHAR strBuffer[1024];
    ASSERT(1024 >= m_nMaxTooltipLength);

    if (!LoadString(CPaintManagerUI::GetInstance(), nID, strBuffer, m_nMaxTooltipLength - 1)) { return FALSE; }

    return SetTooltipText(strBuffer);
}

DuiLib::CDuiString CSystemTrayImpl::GetTooltipText(void) const
{
    return m_bEnabled ? CDuiString(m_tnd.szTip) : CDuiString(_T(""));
}

BOOL CSystemTrayImpl::SetIcon(HICON hIcon)
{
    if (!m_bEnabled) { return FALSE; }

    m_tnd.uFlags = NIF_ICON;
    m_tnd.hIcon = hIcon;

    return m_bHidden ? TRUE : Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL CSystemTrayImpl::SetIcon(LPCTSTR lpszIconName)
{
    HICON hIcon = (HICON) ::LoadImage(CPaintManagerUI::GetInstance(), lpszIconName, IMAGE_ICON,
                                      0, 0, LR_LOADFROMFILE);

    if (!hIcon) { return FALSE; }

    BOOL returnCode = SetIcon(hIcon);
    ::DestroyIcon(hIcon);
    return returnCode;
}

BOOL CSystemTrayImpl::SetIcon(UINT nIDResource)
{
    HICON hIcon = (HICON)::LoadImage(CPaintManagerUI::GetInstance(), MAKEINTRESOURCE(nIDResource),
                                     IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);

    BOOL returnCode = SetIcon(hIcon);
    ::DestroyIcon(hIcon);
    return returnCode;
}

BOOL CSystemTrayImpl::SetStandardIcon(LPCTSTR lpIconName)
{
    HICON hIcon = ::LoadIcon(NULL, lpIconName);
    return SetIcon(hIcon);
}

BOOL CSystemTrayImpl::SetStandardIcon(UINT nIDResource)
{
    HICON hIcon = ::LoadIcon(NULL, MAKEINTRESOURCE(nIDResource));
    return SetIcon(hIcon);
}

HICON CSystemTrayImpl::GetIcon() const
{
    return (m_bEnabled) ? m_tnd.hIcon : NULL;
}

void CSystemTrayImpl::SetFocus()
{
    Shell_NotifyIcon(NIM_SETFOCUS, &m_tnd);
}

BOOL CSystemTrayImpl::HideIcon()
{
    if (!m_bEnabled || m_bRemoved || m_bHidden) { return TRUE; }

    if (m_bWin2K)
    {
        m_tnd.uFlags = NIF_STATE;
        m_tnd.dwState = NIS_HIDDEN;
        m_tnd.dwStateMask = NIS_HIDDEN;

        m_bHidden = Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
    }
    else { RemoveIcon(); }

    return (m_bHidden == TRUE);
}

BOOL CSystemTrayImpl::ShowIcon()
{
    if (m_bRemoved) { return AddIcon(); }

    if (!m_bHidden) { return TRUE; }

    if (m_bWin2K)
    {
        m_tnd.uFlags = NIF_STATE;
        m_tnd.dwState = 0;
        m_tnd.dwStateMask = NIS_HIDDEN;
        Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
    }
    else { AddIcon(); }

    return (m_bHidden == FALSE);
}

BOOL CSystemTrayImpl::AddIcon()
{
    if (!m_bRemoved) { RemoveIcon(); }

    if (m_bEnabled)
    {
        m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

        if (!Shell_NotifyIcon(NIM_ADD, &m_tnd))
        {
            m_bShowIconPending = TRUE;
        }
        else
        {
            m_bRemoved = m_bHidden = FALSE;
        }
    }

    return (m_bRemoved == FALSE);
}

BOOL CSystemTrayImpl::RemoveIcon()
{
    m_bShowIconPending = FALSE;

    if (!m_bEnabled || m_bRemoved) { return TRUE; }

    m_tnd.uFlags = 0;

    if (Shell_NotifyIcon(NIM_DELETE, &m_tnd)) { m_bRemoved = m_bHidden = TRUE; }

    return (m_bRemoved == TRUE);
}

BOOL CSystemTrayImpl::MoveToRight()
{
    RemoveIcon();
    return AddIcon();
}

BOOL CSystemTrayImpl::ShowBalloon(LPCTSTR szText, LPCTSTR szTitle /*= NULL*/, DWORD dwIcon /*= NIIF_NONE*/,
                                  UINT uTimeout /*= 10*/)
{
    // Bail out if we're not on Win 2K.
    if (!m_bWin2K) { return FALSE; }

    // Verify input parameters.

    // The balloon tooltip text can be up to 255 chars long.
    //ASSERT(AfxIsValidString(szText));
    ASSERT(lstrlen(szText) < 256);

    // The balloon title text can be up to 63 chars long.
    if (szTitle) { ASSERT(szTitle && lstrlen(szTitle) < 64); }

    // dwBalloonIcon must be valid.
    ASSERT(NIIF_NONE == dwIcon || NIIF_INFO == dwIcon || NIIF_WARNING == dwIcon || NIIF_ERROR == dwIcon);

    // The timeout must be between 10 and 30 seconds.
    ASSERT(uTimeout >= 10 && uTimeout <= 30);


    m_tnd.uFlags = NIF_INFO;
    _tcsncpy(m_tnd.szInfo, szText, 256);

    if (szTitle) { _tcsncpy(m_tnd.szInfoTitle, szTitle, 64); }
    else         { m_tnd.szInfoTitle[0] = _T('\0'); }

    m_tnd.dwInfoFlags = dwIcon;
    m_tnd.uTimeout = uTimeout * 1000;   // convert time to ms

    BOOL bSuccess = Shell_NotifyIcon(NIM_MODIFY, &m_tnd);

    // Zero out the balloon text string so that later operations won't redisplay the balloon.
    m_tnd.szInfo[0] = _T('\0');
    return bSuccess;
}

BOOL CSystemTrayImpl::SetIconList(UINT uFirstIconID, UINT uLastIconID)
{
    if (uFirstIconID > uLastIconID) { return FALSE; }

    UINT uIconArraySize = uLastIconID - uFirstIconID + 1;
    m_IconList.clear();
    HINSTANCE hInst = CPaintManagerUI::GetInstance();

    for (UINT i = uFirstIconID; i <= uLastIconID; i++)
    {
        HICON hIcon = ::LoadIcon(hInst, MAKEINTRESOURCE(i));

        if (hIcon) { m_IconList.push_back(hIcon); }
    }

    return m_IconList.empty() ? FALSE : TRUE;
}

BOOL CSystemTrayImpl::SetIconList(HICON *pHIconList, UINT nNumIcons)
{
    m_IconList.clear();

    for (UINT i = 0; i <= nNumIcons; i++)
    {
        m_IconList.push_back(pHIconList[i]);
    }

    return m_IconList.empty() ? FALSE : TRUE;
}

BOOL CSystemTrayImpl::Animate(UINT nDelayMilliSeconds, int nNumSeconds /*= -1*/)
{
    if (m_IconList.empty()) { return FALSE; }

    StopAnimation();

    m_nCurrentIcon = 0;
    time(&m_StartTime);
    m_nAnimationPeriod = nNumSeconds;
    m_hSavedIcon = GetIcon();

    // Setup a timer for the animation
    m_uIDTimer = ::SetTimer(m_hWnd, m_nTimerID, nDelayMilliSeconds, NULL);
    return (m_uIDTimer != 0);
}

BOOL CSystemTrayImpl::StepAnimation()
{
    if (!m_IconList.size()) { return FALSE; }

    m_nCurrentIcon++;

    if (m_nCurrentIcon >= (int)m_IconList.size()) { m_nCurrentIcon = 0; }

    return SetIcon(m_IconList[m_nCurrentIcon]);
}

BOOL CSystemTrayImpl::StopAnimation()
{
    BOOL bResult = FALSE;

    if (m_uIDTimer) { bResult = ::KillTimer(m_hWnd, m_uIDTimer); }

    m_uIDTimer = 0;

    if (m_hSavedIcon) { SetIcon(m_hSavedIcon); }

    m_hSavedIcon = NULL;
    return bResult;
}

void CSystemTrayImpl::GetMenuDefaultItem(CDuiString &sItem)
{
    sItem = m_sItemName;
}

BOOL CSystemTrayImpl::SetMenuDefaultItem(const CDuiString &sItem)
{
    bool bRet = CMenuWnd::GetMenuItemInfo(m_pTargetPM, m_xml, m_sSkinType, sItem, m_sItemUserData, m_ptrItemTag);
    m_sItemName = bRet ? sItem : m_sItemName;
    return bRet;
}

// BOOL CSystemTrayImpl::SetNotificationWnd(HWND hNotifyWnd)
// {
//     if (!m_bEnabled) { return FALSE; }
//
//     // Make sure Notification window is valid
//     if (!hNotifyWnd || !::IsWindow(hNotifyWnd))
//     {
//         ASSERT(FALSE);
//         return FALSE;
//     }
//
//     m_tnd.hWnd = hNotifyWnd;
//     m_tnd.uFlags = 0;
//
//     return m_bHidden ? TRUE : Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
// }

// HWND CSystemTrayImpl::GetNotificationWnd() const
// {
//     return m_tnd.hWnd;
// }

BOOL CSystemTrayImpl::SetTargetWnd(CPaintManagerUI *pTargetPM)
{
    m_pTargetPM = pTargetPM;
    return TRUE;
}

CPaintManagerUI *CSystemTrayImpl::GetTargetWnd() const
{
    return m_pTargetPM;
}

BOOL CSystemTrayImpl::SetCallbackMessage(UINT uCallbackMessage)
{
    if (!m_bEnabled) { return FALSE; }

    // Make sure we avoid conflict with other messages
    ASSERT(uCallbackMessage >= WM_APP);

    m_tnd.uCallbackMessage = uCallbackMessage;
    m_tnd.uFlags = NIF_MESSAGE;

    return m_bHidden ? TRUE : Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

UINT CSystemTrayImpl::GetCallbackMessage() const
{
    return m_tnd.uCallbackMessage;
}

HWND CSystemTrayImpl::GetHWnd() const
{
    return this ? m_hWnd : NULL;
}

UINT_PTR CSystemTrayImpl::GetTimerID() const
{
    return m_nTimerID;
}

ATOM CSystemTrayImpl::RegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = (WNDPROC)WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = 0;
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = TRAYICON_CLASS;
    wcex.hIconSm = 0;

    return RegisterClassEx(&wcex);
}

LRESULT PASCAL CSystemTrayImpl::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // The option here is to maintain a list of all TrayIcon windows,
    // and iterate through them. If you do this, remove these 3 lines.
    CSystemTrayImpl *pTrayIcon = m_pThis;

    if (pTrayIcon->GetHWnd() != hWnd) { return ::DefWindowProc(hWnd, message, wParam, lParam); }

    // If maintaining a list of TrayIcon windows, then the following...
    // pTrayIcon = GetFirstTrayIcon()
    // while (pTrayIcon != NULL)
    // {
    //    if (pTrayIcon->GetSafeHwnd() != hWnd) continue;

    // Taskbar has been recreated - all TrayIcons must process this.
    if (message == CSystemTrayImpl::m_nTaskbarCreatedMsg)
    {
        return pTrayIcon->OnTaskbarCreated(wParam, lParam);
    }

    // Animation timer
    if (message == WM_TIMER && wParam == pTrayIcon->GetTimerID())
    {
        return pTrayIcon->OnTimer(wParam);
    }

    // Settings changed
    if (message == WM_SETTINGCHANGE && wParam == pTrayIcon->GetTimerID())
    {
        return pTrayIcon->OnSettingChange(wParam, (LPCTSTR)lParam);
    }

    DUITRACE(_T("message=%x - %x"), message, pTrayIcon->GetCallbackMessage());

    // Is the message from the icon for this TrayIcon?
    if (message == pTrayIcon->GetCallbackMessage())
    {
        return pTrayIcon->OnTrayNotification(wParam, lParam);
    }

    // Message has not been processed, so default.
    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

void CSystemTrayImpl::MinimiseToTray(HWND hWnd)
{
    if (GetDoWndAnimation())
    {
        RECT rectFrom, rectTo;

        GetWindowRect(hWnd, &rectFrom);
        GetTrayWndRect(&rectTo);

        DrawAnimatedRects(hWnd, IDANI_CAPTION, &rectFrom, &rectTo);
    }

    RemoveTaskbarIcon(hWnd);
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_VISIBLE);
}

void CSystemTrayImpl::MaximiseFromTray(HWND hWnd)
{
    if (GetDoWndAnimation())
    {
        RECT rectTo;
        ::GetWindowRect(hWnd, &rectTo);

        RECT rectFrom;
        GetTrayWndRect(&rectFrom);

        ::SetParent(hWnd, NULL);
        DrawAnimatedRects(hWnd, IDANI_CAPTION, &rectFrom, &rectTo);
    }
    else
    {
        ::SetParent(hWnd, NULL);
    }

    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_VISIBLE);
    RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME |
                 RDW_INVALIDATE | RDW_ERASE);

    // Move focus away and back again to ensure taskbar icon is recreated
    if (::IsWindow(m_hWndInvisible))
    {
        SetActiveWindow(m_hWndInvisible);
    }

    SetActiveWindow(hWnd);
    SetForegroundWindow(hWnd);
}

BOOL CSystemTrayImpl::GetDoWndAnimation()
{
    ANIMATIONINFO ai;

    ai.cbSize = sizeof(ai);
    SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai, 0);

    return ai.iMinAnimate ? TRUE : FALSE;
}

// For minimising/maximising from system tray
BOOL CALLBACK FindTrayWnd(HWND hwnd, LPARAM lParam)
{
    TCHAR szClassName[256];
    GetClassName(hwnd, szClassName, 255);

    // Did we find the Main System Tray? If so, then get its size and keep going
    if (_tcscmp(szClassName, _T("TrayNotifyWnd")) == 0)
    {
        LPRECT lpRect = (LPRECT)lParam;
        ::GetWindowRect(hwnd, lpRect);
        return TRUE;
    }

    // Did we find the System Clock? If so, then adjust the size of the rectangle
    // we have and quit (clock will be found after the system tray)
    if (_tcscmp(szClassName, _T("TrayClockWClass")) == 0)
    {
        LPRECT lpRect = (LPRECT)lParam;
        RECT rectClock;
        ::GetWindowRect(hwnd, &rectClock);

        // if clock is above system tray adjust accordingly
        if (rectClock.bottom < lpRect->bottom - 5) // 10 = random fudge factor.
        {
            lpRect->top = rectClock.bottom;
        }
        else
        {
            lpRect->right = rectClock.left;
        }

        return FALSE;
    }

    return TRUE;
}

void CSystemTrayImpl::GetTrayWndRect(LPRECT lprect)
{
#define DEFAULT_RECT_WIDTH 150
#define DEFAULT_RECT_HEIGHT 30

    HWND hShellTrayWnd = FindWindow(_T("Shell_TrayWnd"), NULL);

    if (hShellTrayWnd)
    {
        GetWindowRect(hShellTrayWnd, lprect);
        EnumChildWindows(hShellTrayWnd, FindTrayWnd, (LPARAM)lprect);
        return;
    }

    // OK, we failed to get the rect from the quick hack. Either explorer isn't
    // running or it's a new version of the shell with the window class names
    // changed (how dare Microsoft change these undocumented class names!) So, we
    // try to find out what side of the screen the taskbar is connected to. We
    // know that the system tray is either on the right or the bottom of the
    // taskbar, so we can make a good guess at where to minimize to
    APPBARDATA appBarData;
    appBarData.cbSize = sizeof(appBarData);

    if (SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData))
    {
        // We know the edge the taskbar is connected to, so guess the rect of the
        // system tray. Use various fudge factor to make it look good
        switch (appBarData.uEdge)
        {
        case ABE_LEFT:
        case ABE_RIGHT:
            // We want to minimize to the bottom of the taskbar
            lprect->top = appBarData.rc.bottom - 100;
            lprect->bottom = appBarData.rc.bottom - 16;
            lprect->left = appBarData.rc.left;
            lprect->right = appBarData.rc.right;
            break;

        case ABE_TOP:
        case ABE_BOTTOM:
            // We want to minimize to the right of the taskbar
            lprect->top = appBarData.rc.top;
            lprect->bottom = appBarData.rc.bottom;
            lprect->left = appBarData.rc.right - 100;
            lprect->right = appBarData.rc.right - 16;
            break;
        }

        return;
    }

    // Blimey, we really aren't in luck. It's possible that a third party shell
    // is running instead of explorer. This shell might provide support for the
    // system tray, by providing a Shell_TrayWnd window (which receives the
    // messages for the icons) So, look for a Shell_TrayWnd window and work out
    // the rect from that. Remember that explorer's taskbar is the Shell_TrayWnd,
    // and stretches either the width or the height of the screen. We can't rely
    // on the 3rd party shell's Shell_TrayWnd doing the same, in fact, we can't
    // rely on it being any size. The best we can do is just blindly use the
    // window rect, perhaps limiting the width and height to, say 150 square.
    // Note that if the 3rd party shell supports the same configuraion as
    // explorer (the icons hosted in NotifyTrayWnd, which is a child window of
    // Shell_TrayWnd), we would already have caught it above
    if (hShellTrayWnd)
    {
        ::GetWindowRect(hShellTrayWnd, lprect);

        if (lprect->right - lprect->left > DEFAULT_RECT_WIDTH)
        {
            lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
        }

        if (lprect->bottom - lprect->top > DEFAULT_RECT_HEIGHT)
        {
            lprect->top = lprect->bottom - DEFAULT_RECT_HEIGHT;
        }

        return;
    }

    // OK. Haven't found a thing. Provide a default rect based on the current work
    // area
    SystemParametersInfo(SPI_GETWORKAREA, 0, lprect, 0);
    lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
    lprect->top = lprect->bottom - DEFAULT_RECT_HEIGHT;
}

BOOL CSystemTrayImpl::RemoveTaskbarIcon(HWND hWnd)
{
    // Create static invisible window
    if (!::IsWindow(m_hWndInvisible))
    {
        m_hWndInvisible = CreateWindowEx(0, _T("Static"), _T(""), WS_POPUP,
                                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                         NULL, 0, NULL, 0);

        if (!m_hWndInvisible)
        {
            return FALSE;
        }
    }

    SetParent(hWnd, m_hWndInvisible);
    return TRUE;
}

// This is called whenever the taskbar is created (eg after explorer crashes
// and restarts. Please note that the WM_TASKBARCREATED message is only passed
// to TOP LEVEL windows (like WM_QUERYNEWPALETTE)
LRESULT CSystemTrayImpl::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
    InstallIconPending();
    return 0L;
}

LRESULT CSystemTrayImpl::OnTimer(UINT nIDEvent)
{
    if (nIDEvent != m_uIDTimer)
    {
        ASSERT(FALSE);
        return 0L;
    }

    time_t CurrentTime;
    time(&CurrentTime);

    time_t period = CurrentTime - m_StartTime;

    if (m_nAnimationPeriod > 0 && m_nAnimationPeriod < period)
    {
        StopAnimation();
        return 0L;
    }

    StepAnimation();
    return 0L;
}

LRESULT CSystemTrayImpl::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
    //Return quickly if its not for this tray icon
    if (wParam != m_tnd.uID || !m_pTargetPM) { return 0L; }

    // Clicking with right button brings up a context menu
    if (LOWORD(lParam) == WM_RBUTTONUP)
    {
        // Display and track the popup menu
        POINT pos;
        GetCursorPos(&pos);

        CMenuWnd *pWnd = CMenuWnd::CreateMenu(NULL, m_xml, m_sSkinType, pos, m_pTargetPM,
                                              EMENU_ALIGN_LEFT | EMENU_ALIGN_TOP);
    }

    if (LOWORD(lParam) == WM_LBUTTONDBLCLK)
    {
        // 触发默认菜单项
        CMenuWnd::PostMenuItemClickMsg(m_pTargetPM, m_sItemName, m_sItemUserData, m_ptrItemTag);
    }

    return 1;
}

LRESULT CSystemTrayImpl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
    if (uFlags == SPI_SETWORKAREA)
    {
        InstallIconPending();
    }

    return 0L;
}

void CSystemTrayImpl::InstallIconPending()
{
    // Is the icon display pending, and it's not been set as "hidden"?
    if (!m_bShowIconPending || m_bHidden) { return; }

    // Reset the flags to what was used at creation
    m_tnd.uFlags = m_uCreationFlags;

    // Try and recreate the icon
    m_bHidden = !Shell_NotifyIcon(NIM_ADD, &m_tnd);

    // If it's STILL hidden, then have another go next time...
    m_bShowIconPending = !m_bHidden;
    ASSERT(m_bHidden == FALSE);
}

//////////////////////////////////////////////////////////////////////////
CSystemTray::CSystemTray() : m_pImpl(NULL)
{
}

CSystemTray::CSystemTray(CPaintManagerUI *pTargetPM, LPCTSTR szTip, HICON hIcon, STRINGorID xml,
                         LPCTSTR pSkinType, BOOL bHidden, LPCTSTR szBalloonText, LPCTSTR szBalloonTitle,
                         DWORD dwBalloonIcon, UINT uBalloonTimeout)
{
    Create(pTargetPM, szTip, hIcon, xml, pSkinType, bHidden,
           szBalloonText, szBalloonTitle, dwBalloonIcon, uBalloonTimeout);
}

CSystemTray::~CSystemTray()
{
    if (NULL != m_pImpl) { delete m_pImpl; m_pImpl = NULL; }
}

BOOL CSystemTray::Create(CPaintManagerUI *pTargetPM, LPCTSTR szTip, HICON hIcon, STRINGorID xml,
                         LPCTSTR pSkinType, BOOL bHidden, LPCTSTR szBalloonText, LPCTSTR szBalloonTitle,
                         DWORD dwBalloonIcon, UINT uBalloonTimeout)
{
    if (NULL == m_pImpl) { m_pImpl = new CSystemTrayImpl(); }

    if (NULL == m_pImpl) { return FALSE; }

    return m_pImpl->Create(pTargetPM, szTip, hIcon, xml, pSkinType, bHidden,
                           szBalloonText, szBalloonTitle, dwBalloonIcon, uBalloonTimeout);
}

BOOL CSystemTray::IsEnable()
{
    return (NULL != m_pImpl) ? m_pImpl->IsEnable() : FALSE;
}

BOOL CSystemTray::IsVisible()
{
    return (NULL != m_pImpl) ? m_pImpl->IsEnable() : FALSE;
}

BOOL CSystemTray::SetTooltipText(LPCTSTR pszTooltipText)
{
    return (NULL != m_pImpl) ? m_pImpl->SetTooltipText(pszTooltipText) : FALSE;
}

BOOL CSystemTray::SetTooltipText(UINT uID)
{
    return (NULL != m_pImpl) ? m_pImpl->SetTooltipText(uID) : FALSE;
}

DuiLib::CDuiString CSystemTray::GetTooltipText(void) const
{
    return (NULL != m_pImpl) ? m_pImpl->GetTooltipText() : CDuiString(_T(""));
}

BOOL CSystemTray::SetIcon(HICON hIcon)
{
    return (NULL != m_pImpl) ? m_pImpl->SetIcon(hIcon) : FALSE;
}

BOOL CSystemTray::SetIcon(LPCTSTR lpszIconName)
{
    return (NULL != m_pImpl) ? m_pImpl->SetIcon(lpszIconName) : FALSE;
}

BOOL CSystemTray::SetIcon(UINT nIDResource)
{
    return (NULL != m_pImpl) ? m_pImpl->SetIcon(nIDResource) : FALSE;
}

BOOL CSystemTray::SetStandardIcon(LPCTSTR lpIconName)
{
    return (NULL != m_pImpl) ? m_pImpl->SetStandardIcon(lpIconName) : FALSE;
}

BOOL CSystemTray::SetStandardIcon(UINT nIDResource)
{
    return (NULL != m_pImpl) ? m_pImpl->SetStandardIcon(nIDResource) : FALSE;
}

HICON CSystemTray::GetIcon() const
{
    return (NULL != m_pImpl) ? m_pImpl->GetIcon() : NULL;
}

void CSystemTray::SetFocus()
{
    (NULL != m_pImpl) ? m_pImpl->SetFocus() : NULL; //lint !e62
}

BOOL CSystemTray::HideIcon()
{
    return (NULL != m_pImpl) ? m_pImpl->HideIcon() : FALSE;
}

BOOL CSystemTray::ShowIcon()
{
    return (NULL != m_pImpl) ? m_pImpl->ShowIcon() : FALSE;
}

BOOL CSystemTray::AddIcon()
{
    return (NULL != m_pImpl) ? m_pImpl->AddIcon() : FALSE;
}

BOOL CSystemTray::RemoveIcon()
{
    return (NULL != m_pImpl) ? m_pImpl->RemoveIcon() : FALSE;
}

BOOL CSystemTray::MoveToRight()
{
    return (NULL != m_pImpl) ? m_pImpl->MoveToRight() : FALSE;
}

BOOL CSystemTray::ShowBalloon(LPCTSTR szText, LPCTSTR szTitle /*= NULL*/, DWORD dwIcon /*= NIIF_NONE*/,
                              UINT uTimeout /*= 10*/)
{
    return (NULL != m_pImpl) ? m_pImpl->ShowBalloon(szText, szTitle, dwIcon, uTimeout) : FALSE;
}

BOOL CSystemTray::SetIconList(UINT uFirstIconID, UINT uLastIconID)
{
    return (NULL != m_pImpl) ? m_pImpl->SetIconList(uFirstIconID, uLastIconID) : FALSE;
}

BOOL CSystemTray::SetIconList(HICON *pHIconList, UINT nNumIcons)
{
    return (NULL != m_pImpl) ? m_pImpl->SetIconList(pHIconList, nNumIcons) : FALSE;
}

BOOL CSystemTray::Animate(UINT nDelayMilliSeconds, int nNumSeconds /*= -1*/)
{
    return (NULL != m_pImpl) ? m_pImpl->Animate(nDelayMilliSeconds, nNumSeconds) : FALSE;
}

BOOL CSystemTray::StepAnimation()
{
    return (NULL != m_pImpl) ? m_pImpl->StepAnimation() : FALSE;
}

BOOL CSystemTray::StopAnimation()
{
    return (NULL != m_pImpl) ? m_pImpl->StopAnimation() : FALSE;
}

void CSystemTray::GetMenuDefaultItem(CDuiString &sItem)
{
    (NULL != m_pImpl) ? m_pImpl->GetMenuDefaultItem(sItem) : NULL; //lint !e62
}

BOOL CSystemTray::SetMenuDefaultItem(const CDuiString &sItem)
{
    return (NULL != m_pImpl) ? m_pImpl->SetMenuDefaultItem(sItem) : FALSE;
}

// BOOL CSystemTray::SetNotificationWnd(HWND hNotifyWnd)
// {
//     return (NULL != m_pImpl) ? m_pImpl->SetNotificationWnd(hNotifyWnd) : FALSE;
// }

// HWND CSystemTray::GetNotificationWnd() const
// {
//     return (NULL != m_pImpl) ? m_pImpl->GetNotificationWnd() : NULL;
// }

BOOL CSystemTray::SetTargetWnd(CPaintManagerUI *pTargetPM)
{
    return (NULL != m_pImpl) ? m_pImpl->SetTargetWnd(pTargetPM) : FALSE;
}

CPaintManagerUI *CSystemTray::GetTargetWnd() const
{
    return (NULL != m_pImpl) ? m_pImpl->GetTargetWnd() : NULL;
}

BOOL CSystemTray::SetCallbackMessage(UINT uCallbackMessage)
{
    return (NULL != m_pImpl) ? m_pImpl->SetCallbackMessage(uCallbackMessage) : FALSE;
}

UINT CSystemTray::GetCallbackMessage() const
{
    return (NULL != m_pImpl) ? m_pImpl->GetCallbackMessage() : 0;
}

HWND CSystemTray::GetHWnd() const
{
    return (NULL != m_pImpl) ? m_pImpl->GetHWnd() : NULL;
}

UINT_PTR CSystemTray::GetTimerID() const
{
    return (NULL != m_pImpl) ? m_pImpl->GetTimerID() : 0;
}

void CSystemTray::MinimiseToTray(HWND hWnd)
{
    CSystemTrayImpl::MinimiseToTray(hWnd);
}

void CSystemTray::MaximiseFromTray(HWND hWnd)
{
    CSystemTrayImpl::MaximiseFromTray(hWnd);
}

}
