#ifndef __UIBASE_H__
#define __UIBASE_H__


#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

#define UI_WNDSTYLE_CONTAINER  (0)
#define UI_WNDSTYLE_FRAME      (WS_VISIBLE | WS_OVERLAPPEDWINDOW)
#define UI_WNDSTYLE_CHILD      (WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
#define UI_WNDSTYLE_DIALOG     (WS_VISIBLE | WS_POPUPWINDOW | WS_CAPTION | WS_DLGFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)

#define UI_WNDSTYLE_EX_FRAME   (WS_EX_WINDOWEDGE)
#define UI_WNDSTYLE_EX_DIALOG  (WS_EX_TOOLWINDOW | WS_EX_DLGMODALFRAME)

#define UI_CLASSSTYLE_CONTAINER  (0)
#define UI_CLASSSTYLE_FRAME      (CS_VREDRAW | CS_HREDRAW)
#define UI_CLASSSTYLE_CHILD      (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)
#define UI_CLASSSTYLE_DIALOG     (CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS | CS_SAVEBITS)


/////////////////////////////////////////////////////////////////////////////////////
//
#ifndef ASSERT
    #define ASSERT(expr)  _ASSERTE(expr)
#endif

#ifdef _DEBUG
    #ifndef DUITRACE
        #define DUITRACE DUI__Trace
    #endif
    #define DUITRACEMSG DUI__TraceMsg
#else
    #ifndef DUITRACE
        #define DUITRACE
    #endif
    #define DUITRACEMSG _T("")
#endif

void DUILIB_API DUI__Trace(LPCTSTR pstrFormat, ...);
LPCTSTR DUILIB_API DUI__TraceMsg(UINT uMsg);

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CNotifyPump
{
public:
    bool AddVirtualWnd(CDuiString strName, CNotifyPump *pObject);
    bool RemoveVirtualWnd(CDuiString strName);
    void NotifyPump(TNotifyUI &msg);
    bool LoopDispatch(TNotifyUI &msg);
    DUI_DECLARE_MESSAGE_MAP()
private:
    CDuiStringPtrMap m_VirtualWndMap;
};

class DUILIB_API CWindowWnd
{
public:
    CWindowWnd();

    HWND GetHWND() const;
    operator HWND() const;

    HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc,
                HMENU hMenu = NULL);
    HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT,
                int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
    HWND CreateDuiWindow(HWND hwndParent, LPCTSTR pstrWindowName, DWORD dwStyle = 0, DWORD dwExStyle = 0);
    HWND Subclass(HWND hWnd);
    void Unsubclass();
    void ShowWindow(bool bShow = true, bool bTakeFocus = true);
    UINT ShowModal();
    void Close(UINT nRet = IDOK, bool bSync = false);   // bSync=true 表示同步，否则异步
    void CenterWindow();    // 居中，支持扩展屏幕
    void MoveWindow(int x, int y);  // 把窗口移动到(x,y)。即移动后，窗口的左上角在(x,y)，大小不变；不重绘窗口
    void MoveWindow(int x, int y, int w, int h, BOOL bRepaint = FALSE);
    void SetIcon(UINT nRes);

    void SetParent(HWND hParent);
    HWND GetParent(HWND hWnd = NULL);
    void SetOwner(HWND hOwner);
    HWND GetOwner();

    LRESULT SendMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    LRESULT PostMessage(UINT uMsg, WPARAM wParam = 0, LPARAM lParam = 0L);
    void ResizeClient(int cx = -1, int cy = -1);

protected:
    virtual void OnFinalMessage(HWND hWnd);

    virtual LPCTSTR GetWindowClassName() const = 0;
    virtual LPCTSTR GetSuperClassName() const;
    virtual UINT GetClassStyle() const;

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK __WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK __ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void ReloadTitle(void);

    bool RegisterWindowClass();
    bool RegisterSuperclass();
protected:
    HWND m_hWnd;
    HWND m_hOwner;              // 当前窗口的拥有者
    WNDPROC m_OldWndProc;
    bool m_bSubclassed;
    CDuiString  m_sTitle;       // 窗口标题
    CDuiString  m_sTitleOrig;   // 窗口标题（翻译前）
};

} // namespace DuiLib

#endif // __UIBASE_H__
