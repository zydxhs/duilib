#ifndef __SYSTEMTRAY_H__
#define __SYSTEMTRAY_H__
// cmaunder@mail.com

#include <ShellAPI.h>

#if (WINVER < 0x0500) || (_WIN32_WINNT <0x0500)
    #error "Only support win2k and newer OS"
#endif

namespace DuiLib {
class CSystemTrayImpl;

class DUILIB_API CSystemTray
{
public:
    CSystemTray();
    CSystemTray(CPaintManagerUI *pTargetPM, LPCTSTR szTip, HICON icon, STRINGorID xml, LPCTSTR pSkinType = NULL,
                BOOL bHidden = FALSE, LPCTSTR szBalloonText = NULL, LPCTSTR szBalloonTitle = NULL,
                DWORD dwBalloonIcon = NIIF_NONE, UINT uBalloonTimeout = 10);
    virtual ~CSystemTray();

    // uBalloonTimeout: 10 ~ 30
    BOOL Create(CPaintManagerUI *pTargetPM, LPCTSTR szTip, HICON hIcon, STRINGorID xml, LPCTSTR pSkinType = NULL,
                BOOL bHidden = FALSE, LPCTSTR szBalloonText = NULL, LPCTSTR szBalloonTitle = NULL,
                DWORD dwBalloonIcon = NIIF_NONE, UINT uBalloonTimeout = 10);

    BOOL IsEnable();
    BOOL IsVisible();

    // Change or retrieve the Tooltip text
    BOOL   SetTooltipText(LPCTSTR pszTooltipText);
    BOOL   SetTooltipText(UINT uID);
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

    BOOL ShowBalloon(LPCTSTR szText, LPCTSTR szTitle = NULL, DWORD dwIcon = NIIF_NONE, UINT uTimeout = 10);

    // For icon animation
    BOOL  SetIconList(UINT uFirstIconID, UINT uLastIconID);
    BOOL  SetIconList(HICON *pHIconList, UINT nNumIcons);
    BOOL  Animate(UINT nDelayMilliSeconds, int nNumSeconds = -1);
    BOOL  StepAnimation();
    BOOL  StopAnimation();

    // Change menu default item. 如果默认菜单项支持复选，则其选中状态需要用户维护
    void  GetMenuDefaultItem(CDuiString &sItem);
    BOOL  SetMenuDefaultItem(const CDuiString &sItem);

    // Change or retrieve the window to send menu commands to
    BOOL  SetTargetWnd(CPaintManagerUI *pTargetPM);
    CPaintManagerUI *GetTargetWnd() const;

    // Change or retrieve  notification messages sent to the window
    BOOL  SetCallbackMessage(UINT uCallbackMessage);
    UINT  GetCallbackMessage() const;

    HWND  GetHWnd() const;
    UINT_PTR GetTimerID() const;

public:
    static void MinimiseToTray(HWND hWnd);
    static void MaximiseFromTray(HWND hWnd);

private:
    CSystemTrayImpl    *m_pImpl;
};

}

#endif  // __SYSTEMTRAY_H__
