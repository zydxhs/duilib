//2017-02-25 zhuyadong 添加菜单控件
// 菜单项选择消息：
// 1. 可以响应自定义窗口消息：WM_MENUITEM_CLICK
// 2. 可以响应 Notify 通知消息：DUI_MSGTYPE_MENUITEM_CLICK
#ifndef __UIMENU_H__
#define __UIMENU_H__

#pragma once

namespace DuiLib {

enum EMMenuAlign
{
    EMENU_ALIGN_LEFT    = 1 << 1,
    EMENU_ALIGN_TOP     = 1 << 2,
    EMENU_ALIGN_RIGHT   = 1 << 3,
    EMENU_ALIGN_BOTTOM  = 1 << 4,
};

/////////////////////////////////////////////////////////////////////////////////////
//
class CMenuUI;
class CMenuElementUI;
class DUILIB_API CMenuWnd : public CWindowWnd, public INotifyUI, public IDialogBuilderCallback,
    public IObserver
{
    friend class CMenuElementUI;
public:
    /*
    *   @pOwner     一级菜单不要指定这个参数，这是菜单内部使用的
    *   @xml        菜单的布局文件
    *   @pSkinType  菜单资源类型，当从资源加载菜单时有效
    *   @point      菜单的左上角坐标
    *   @pParent    菜单的父窗体管理器指针
    *   @dwAlign    菜单的出现位置，默认出现在鼠标的右下侧。
    */
    static CMenuWnd *CreateMenu(CMenuElementUI *pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT pt,
                                CPaintManagerUI *pParent, DWORD dwAlign = EMENU_ALIGN_LEFT | EMENU_ALIGN_TOP);

    static CDuiString       s_strName;      // 被单击菜单项的的 名字
    static CDuiString       s_strUserData;  // 被单击菜单项的的 用户数据
    static UINT_PTR         s_ptrTag;       // 被单击菜单项的的 Tag
public:
    CMenuWnd(void);

    //继承来的接口
    virtual void Notify(TNotifyUI &msg) { }
    virtual CControlUI *CreateControl(LPCTSTR pstrClassName);
    virtual void OnSubjectUpdate(WPARAM p1, WPARAM p2 = NULL, LPARAM p3 = NULL, CSubjectBase *pSub = NULL);

    // 获取根菜单控件，用于动态添加子菜单
    CMenuUI *GetMenuUI(void);
    // 重新调整菜单的大小
    void ResizeMenu(void);
    // 重新调整子菜单的大小
    void ResizeSubMenu(void);

protected:
    CPaintManagerUI *GetManager(void);
    virtual LPCTSTR GetWindowClassName(void) const;
    virtual void OnFinalMessage(HWND hWnd);
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
    void Init(CMenuElementUI *pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT ptClient,
              CPaintManagerUI *pParent, DWORD dwAlign = EMENU_ALIGN_LEFT | EMENU_ALIGN_TOP);

public:
    POINT               m_ptBase;
    STRINGorID          m_xml;
    CDuiString          m_sSkinType;
    CPaintManagerUI     m_pm;
    CMenuElementUI     *m_pOwner;
    DWORD               m_dwAlign;          // 菜单对齐方式
};

/////////////////////////////////////////////////////////////////////////////////////
//
class DUILIB_API CMenuUI : public CListUI
{
public:
    CMenuUI(void);
    virtual ~CMenuUI(void);

    // 返回一个新的菜单项对象，自动添加默认属性。
    // 用于解决 DuiLib 使用 /MT 编译选项时的断言错误：_pFirstBlock == pHead
    CMenuElementUI *NewMenuItem(void);
    // 添加/删除 菜单项
    virtual bool Add(CControlUI *pControl);
    virtual bool AddAt(CControlUI *pControl, int iIndex);
    virtual bool Remove(CControlUI *pControl, bool bDoNotDestroy = false);

    // 获取/设置 菜单项位置索引
    virtual int GetItemIndex(CControlUI *pControl) const;
    virtual bool SetItemIndex(CControlUI *pControl, int iIndex);

    // 查找菜单项。
    // 如果返回的菜单项包含子菜单，可以直接添加子菜单项
    // 否则，需要添加可展开属性
    CMenuElementUI *FindMenuItem(LPCTSTR pstrName);

    //////////////////////////////////////////////////////////////////////////
    virtual LPCTSTR GetClass(void) const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual void DoEvent(TEventUI &event);
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
};

/////////////////////////////////////////////////////////////////////////////////////
//
class DUILIB_API CMenuElementUI : public CListContainerElementUI
{
    friend CMenuWnd;
public:
    CMenuElementUI(void);
    ~CMenuElementUI(void);

    // 返回一个新的菜单项对象，自动添加默认属性。
    // 用于解决 DuiLib 使用 /MT 编译选项时的断言错误：_pFirstBlock == pHead
    CMenuElementUI *NewMenuItem(void);
    // 添加/删除 菜单项：直接调用 Addd/AddAt/Remove

    //属性
    void SetIconWidth(WORD wWidth);
    WORD GetIconWidth(void) const;
    void SetIconNormal(LPCTSTR pstrIcon);
    void SetIconChecked(LPCTSTR pstrIcon);
    void SetCheckItem(bool bCheckItem = false);
    bool IsCheckItem(void) const;
    void SetCheck(bool bCheck = true);
    bool IsChecked(void) const;

    void SetExpandWidth(WORD wWidth);
    WORD GetExpandWidth(void) const;
    void SetExpandIcon(LPCTSTR pstrIcon);

    void SetLine(void);
    bool IsLine(void) const;
    void SetLineColor(DWORD color);
    DWORD GetLineColor(void) const;

    //////////////////////////////////////////////////////////////////////////
    virtual LPCTSTR GetClass(void) const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoEvent(TEventUI &event);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    virtual void SetAttributeList(LPCTSTR pstrList);

private:
    void DrawItemIcon(HDC hDC, const RECT &rcItem);
    void DrawItemText(HDC hDC, const RECT &rcItem) override;
    void DrawItemExpand(HDC hDC, const RECT &rcItem);

    void OnTimer(TEventUI &event);
    void OnMouseEnter(TEventUI &event);
    void OnMouseLeave(TEventUI &event);
    void OnLButtonUp(TEventUI &event);
    void OnKeyDown(TEventUI &event);

private:
    enum { TIMERID_MOUSEENTER = 30, TIMERID_MOUSELEAVE = 31 };

    CMenuWnd   *m_pSubMenuWnd;

    //菜单项图标
    TDrawInfo   m_diIconNormal;     //正常状态图标
    TDrawInfo   m_diIconChecked;    //选中状态图标，仅当支持复选时有效
    bool        m_bCheckItem;       //是否支持复选
    bool        m_bChecked;         //当前是否选中
    WORD        m_wIconWidth;       //占用宽度

    //标识存在下级菜单的图标
    //IsExpandable()
    TDrawInfo   m_diExpandIcon;     //
    WORD        m_wExpandWidth;     //占用宽度

    //分割线
    bool        m_bLine;            //画分隔线
    DWORD       m_dwLineColor;      //分隔线颜色
};

} // namespace DuiLib

#endif // __UIMENU_H__
