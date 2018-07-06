#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

namespace DuiLib {

enum UILIB_RESOURCETYPE
{
    UILIB_FILE = 1,     // 来自磁盘文件
    UILIB_ZIP,          // 来自磁盘zip压缩包
    UILIB_RESOURCE,     // 来自资源
    UILIB_ZIPRESOURCE,  // 来自资源的zip压缩包
};

class CButtonUI;

class DUILIB_API CWndImplBase
    : public CWindowWnd
    , public CNotifyPump
    , public INotifyUI
    , public IMessageFilterUI
    , public IDialogBuilderCallback
{
public:
    CWndImplBase();
    virtual ~CWndImplBase() {};

    // IMessageFilterUI 接口，进行消息发送到窗口过程前的过滤处理
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled);

    // INotifyUI 接口，duilib 控件通知消息
    virtual void Notify(TNotifyUI &msg);

    void ShowWindow(bool bShow = true, bool bTakeFocus = true);

protected:
    DUI_DECLARE_MESSAGE_MAP()
    virtual void OnClick(TNotifyUI &msg);

    // 添加在标题栏上不支持鼠标/键盘操作的控件。
    // 默认已添加控件：Control/Label/Text//GifAnim/PwdCheck/Progress/所有布局控件
    void AddIncludeCtrlForCaption(LPCTSTR szCtrlName);
    //
    bool IsCaptionCtrl(CControlUI *pCtrl);

protected:
    // 窗口类相关接口
    virtual LPCTSTR GetWindowClassName(void) const = 0;
    // virtual LPCTSTR GetSuperClassName() const;
    virtual UINT GetClassStyle() const { return CS_DBLCLKS; }

    // 窗口资源相关接口
    virtual CDuiString GetSkinFolder() = 0;
    virtual CDuiString GetSkinFile() = 0;
    virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE; }
    virtual CDuiString GetZIPFileName() const { return _T(""); }
    virtual LPCTSTR GetResourceID() const { return _T(""); }

    // IDialogBuilderCallback 接口，创建自定义控件
    virtual CControlUI *CreateControl(LPCTSTR pstrClass) { return NULL; }

    // 窗体生命周期中，第一次和最后一次被调用的接口，且仅被调用一次。
    // .窗体创建后，绑定数据。比如控件与变量的绑定、控件属性设置等只需要设置一次的操作。
    virtual void OnInitWindow(void);
    // .窗体销毁后，类对象销毁前，最后被调用的接口。new 的窗体，在该接口中调用 delete 销毁对象。
    virtual void OnFinalMessage(HWND hWnd);
    // 窗体数据显示数据的初始化、保存
    // .窗体 创建|显示 之前 初始化 数据。在 OnInitWindow 和 OnFinalMesssage 之间调用
    virtual void OnDataInit(void) { }
    // .窗体 销毁|隐藏 之前  保存  数据。在 OnInitWindow 和 OnFinalMesssage 之间调用
    virtual void OnDataSave(void) { }
    // 窗体布局第一次正确初始化后调用的接口，此时窗口将提交刷新/动画。在窗口显示前调用
    virtual void OnPrepare(void) { }

    virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
    //2017-02-25 zhuyadong 完善多语言切换
    virtual LRESULT OnLanguageUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam);

    // 自定义消息处理。发送到窗口的消息，在交给 CPaintManagerUI 处理之前截获
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
    virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcLButtonDblClk(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
#endif
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    // 窗口处理过程，通常不需要重载。所有发送到窗口的消息，都可以在此截获
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    LRESULT OnWndDataUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    static LPBYTE m_lpResourceZIPBuffer;

    CPaintManagerUI m_pm;

    CButtonUI      *m_pbtnMin;              // 最小化
    CButtonUI      *m_pbtnMax;              // 最大化
    CButtonUI      *m_pbtnRestore;          // 还原
    CButtonUI      *m_pbtnClose;            // 关闭
private:
    int             m_nWndState;            // 用于判断是否发送窗体数据初始化/保存消息
    CDuiPtrArray    m_aryCtrlStatic;        // 标题栏上不支持鼠标/键盘操作的控件
};

}

#endif // WIN_IMPL_BASE_HPP
