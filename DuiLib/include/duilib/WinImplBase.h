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
    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

    // INotifyUI 接口，duilib 控件通知消息
    virtual void Notify(TNotifyUI &msg) override;

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
    virtual LPCTSTR GetWindowClassName(void) const override = 0;
    // virtual LPCTSTR GetSuperClassName() const;
    virtual UINT GetClassStyle() const override { return CS_DBLCLKS; }

    // 窗口资源相关接口
    virtual CDuiString GetSkinFolder() = 0;
    virtual CDuiString GetSkinFile() = 0;
    virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE; }
    virtual CDuiString GetZIPFileName() const { return _T(""); }
    virtual LPCTSTR GetResourceID() const { return _T(""); }

    // 创建子窗体
    virtual CWndImplBase *CreateWnd(CDuiString strDlgType) { return NULL; };

    // IDialogBuilderCallback 接口，创建自定义控件
    virtual CControlUI *CreateControl(LPCTSTR pstrClass) { return NULL; }

    // 窗体生命周期中，第一次和最后一次被调用的接口，且仅被调用一次。
    // .窗体创建后，绑定数据。比如控件与变量的绑定、控件属性设置等只需要设置一次的操作。
    virtual void OnInitWindow(void);
    // .窗体销毁后，类对象销毁前，最后被调用的接口。new 的窗体，在该接口中调用 delete 销毁对象。
    virtual void OnFinalMessage(HWND hWnd) override;
    // 窗体数据显示数据的初始化、保存
    // .窗体 创建|显示 之前 初始化 数据。在 OnInitWindow 和 OnFinalMesssage 之间调用
    virtual void OnDataInit(void) { }
    // .窗体 销毁|隐藏 之前  保存  数据。在 OnInitWindow 和 OnFinalMesssage 之间调用
    virtual void OnDataSave(void);
    // 窗体布局第一次正确初始化后调用的接口，此时窗口将提交刷新/动画。在窗口显示前调用
    virtual void OnPrepare(void) { }

    virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
    //2017-02-25 zhuyadong 完善多语言切换
    virtual LRESULT OnLanguageUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam);

    // 自定义消息处理。发送到窗口的消息，在交给 CPaintManagerUI 处理之前截获
    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);


    // 窗体消息处理函数
    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
    virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
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
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    // 子窗口。派生类通常不需要调用。
    void AddChild(CDuiString strType, CWndImplBase *pWnd);
    void DelChild(CDuiString strType);
    CWndImplBase *GetChild(CDuiString strType);
    void DestroyChildDlg();
    void DestroyChildDlg(CDuiString strType);

    // pCtrl指向的容器大小改变时，触发Relayout调用，自动调整子窗体的大小/位置
    bool MakeCtrlSizeNty(CContainerUI *pCtrl);

    // 功能选项按钮
    void AddBtnDlgItem(CDuiString strBtnName, CDuiString strDlgType, CControlUI *pCtrl = NULL);
    void ResetBtnDlgItem(void);

    // 子窗口切换。用户必须实现 CreateWnd，以实现子窗口的创建。
    void SwitchChildDlg(CDuiString strBtnName);

    // 以下两个函数，DuiLib自动创建的子窗体，用户不需要调用；主要用于用户创建子窗体的维护。
    void SetCurDlgType(CDuiString strDlgType);  // 设置当前子窗口类型
    bool Relayout(void *pParam = NULL);         // 调用时机：父窗口改变大小、子窗口初次添加到父窗口

    // 设置/查询 窗体数据修改状态。用户使用。
    void SetModified(bool bModified = true);
    bool IsModified(void);

private:
    LRESULT OnWndDataUpdate(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnWndEffectShowEndNty(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    static LPBYTE m_lpResourceZIPBuffer;

    CPaintManagerUI m_pm;

    CButtonUI      *m_pbtnMin;              // 最小化
    CButtonUI      *m_pbtnMax;              // 最大化
    CButtonUI      *m_pbtnRestore;          // 还原
    CButtonUI      *m_pbtnClose;            // 关闭

    // true 表示窗体数据有修改，窗体闭关/隐藏前要提示用户。
    // 这是给用户提供的功能。
    bool            m_bModified;

private:
    int             m_nWndState;            // 用于判断是否发送窗体数据初始化/保存消息
    CDuiPtrArray    m_aryCtrlStatic;        // 标题栏上不支持鼠标/键盘操作的控件

    // 2018-07-15 设计思路：
    // 1. 每个窗体类与一个字符串关联，用字符串表示窗体类。只支持同时显示一个子窗口。
    // 2. 支持子窗体，放置在占位符容器中；并且当占位符容器大小/位置改变时，自动调整子窗体的大小/位置
    //    用户需要做的是：调用 MakeCtrlSizeNty 设置占位符容器；
    //                    添加子窗口到 m_mapChild 关联容器中。
    // 3. 当窗体拥有多个子窗口，并且每个子窗口与窗体上的某个控件关联时（比如 tab 标签页功能）
    //    用户只需要调用 AddBtnDlgItem 将窗体控件名，与子窗体类型或相关控件指针关联起来就可以了。
    //    单击窗体上的控件时，调用 SwitchChildDlg 即可切换到对应的子窗口，用户必须实现 CreateWnd。
    //
    // 自动创建维护子窗体：
    // 1. 调用 MakeCtrlSizeNty 设置子窗体的位置占位符容器；
    // 2. 调用 AddBtnDlgItem 把一个按钮名、子窗体类型绑定；
    // 3. 调用 SwitchChildDlg 切换到按钮关联的子窗体；
    // 4. 调用 ResetBtnDlgItem 删除按钮名、子窗体类型的绑定关系；此时已经创建的子窗体，不会销毁；
    //
    // 用户创建的子窗体，要想实现跟随父窗体大小变化，需要如下步骤：
    // 1. 调用 MakeCtrlSizeNty 设置子窗体的位置占位符容器；
    // 2. 调用 SetCurDlgType 设置当前子窗体类型；
    // 3. 子窗体第一次创建时，调用 AddChild 把子窗体类型和其指针关联起来；
    //    调用 Relayout 调整子窗体大小、位置；
    // 4. 如果子窗体改变了，必须调用 SetCurDlgType 更新当前子窗体类型；
    // 5. 子窗体销毁时，需要调用 DellChild 删除窗体类型与其指针的绑定关系。
    CContainerUI   *m_pCtrlPlaceHolder;     // 占位符，用于放置子窗体
    CDuiString      m_strChildDlgType;      // 当前子窗口类型
    // key=窗体类型字符串，唯一标识一个窗体；value=子窗口指针(CWndImplBase*)
    CDuiStringPtrMap    m_mapChild;         // 子窗体列表，用于窗体大小/位置变化时调整子窗口大小/位置。
    // key=控件的名字；value=控件关联的子窗体类型字符串或控件指针(TBuddyItem*)
    CDuiStringPtrMap    m_mapBtnItem;       // 功能选项按钮与关联子窗体映射关系

};

}

#endif // WIN_IMPL_BASE_HPP
