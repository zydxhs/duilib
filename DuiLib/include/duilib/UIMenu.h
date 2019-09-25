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
    EMENU_ALIGN_LEFT    = 1 << 0,   // 菜单位于鼠标左侧
    EMENU_ALIGN_TOP     = 1 << 1,   // 菜单位于鼠标上侧
    EMENU_ALIGN_RIGHT   = 1 << 2,   // 菜单位于鼠标右侧
    EMENU_ALIGN_BOTTOM  = 1 << 3,   // 菜单位于鼠标下侧
    EMENU_ALIGN_HIDE    = 1 << 4,   // 菜单不显示，用于获取指定菜单项用户数据、tag
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
    /*!
     * @brief      创建菜单并显示。
     * @details
     * @param[in]  CMenuElementUI * pOwner  创建子菜单时有效，这是菜单内部使用的。创建一级菜单时置为空值。
     * @param[in]  STRINGorID xml           菜单的布局文件。.xml文件或资源ID。
     * @param[in]  LPCTSTR pSkinType        菜单资源类型。xml为文件时忽略该参数；为资源ID时通知取_T("XML")，由用于指定。
     * @param[in]  POINT pt                 菜单显示位置，菜单的左上角坐标。
     * @param[in]  CPaintManagerUI * pParent 菜单的父窗体管理器指针，接收菜单消息。
     * @param[in]  DWORD dwAlign            菜单位置鼠标的哪个位置。详见 EMMenuAlign，默认出现在鼠标的右下侧。
     * @return     成功返回菜单窗口指定；失败返回 NULL。
     * @attention  如果 dwAlign 组合了 EMENU_ALIGN_HIDE，则菜单不可见，用于获取指定菜单项信息。
     */
    static CMenuWnd *CreateMenu(CMenuElementUI *pOwner, STRINGorID xml, LPCTSTR pSkinType, POINT pt,
                                CPaintManagerUI *pParent, DWORD dwAlign = EMENU_ALIGN_RIGHT | EMENU_ALIGN_BOTTOM);
    static CMenuWnd *GetInstance(void);
    /*!
     * @brief      获取指定菜单项的用户数据、tag
     * @details
     * @param[in]  CPaintManagerUI * pm         菜单的父窗体管理器指针，接收菜单消息。
     * @param[in]  const STRINGorID & xml       菜单的布局文件。.xml文件或资源ID。
     * @param[in]  const CDuiString & sSkinType 菜单资源类型。xml为文件时忽略该参数；为资源ID时通知取_T("XML")，由用于指定。
     * @param[in]  const CDuiString & sName     菜单项名字
     * @param[in]  CDuiString & sUserData       用于保存返回的菜单项用户数据
     * @param[in]  UINT_PTR & ptrTag            用于保存返回的菜单项tag
     * @return     成功返回 true。
     * @attention
     */
    static bool GetMenuItemInfo(CPaintManagerUI *pm, const STRINGorID &xml, const CDuiString &sSkinType,
                                const CDuiString &sName, CDuiString &sUserData, UINT_PTR &ptrTag);
    /*!
     * @brief      向指定窗口发送指定的菜单项单击消息。
     * @details
     * @param[in]  CPaintManagerUI * pm         菜单的父窗体管理器指针，接收菜单消息。
     * @param[in]  const CDuiString & sName     菜单项名字
     * @param[in]  const CDuiString & sUserData 菜单项用户数据
     * @param[in]  UINT_PTR ptrTag              菜单项tag
     * @return
     * @attention
     */
    static void PostMenuItemClickMsg(CPaintManagerUI *pm, const CDuiString &sName,
                                     const CDuiString &sUserData, UINT_PTR ptrTag);

    static CDuiString       s_strName;      // 被单击菜单项的 名字
    static CDuiString       s_strUserData;  // 被单击菜单项的 用户数据
    static UINT_PTR         s_ptrTag;       // 被单击菜单项的 Tag
public:
    CMenuWnd(void);

    //继承来的接口
    virtual void Notify(TNotifyUI &msg) override { }
    virtual CControlUI *CreateControl(LPCTSTR pstrClassName) override;
    virtual bool OnSubjectUpdate(WPARAM p1, WPARAM p2 = NULL, LPARAM p3 = NULL,
                                 CSubjectBase *pSub = NULL) override;

    // 获取根菜单控件，用于动态添加子菜单
    CMenuUI *GetMenuUI(void);
    // 重新调整菜单的大小
    void ResizeMenu(void);
    // 重新调整子菜单的大小
    void ResizeSubMenu(void);

protected:
    CPaintManagerUI *GetManager(void);
    virtual LPCTSTR GetWindowClassName(void) const override;
    virtual void OnFinalMessage(HWND hWnd) override;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
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
    virtual bool Add(CControlUI *pControl) override;
    virtual bool AddAt(CControlUI *pControl, int iIndex) override;
    virtual bool Remove(CControlUI *pControl, bool bDoNotDestroy = false) override;

    // 获取/设置 菜单项位置索引
    virtual int GetItemIndex(CControlUI *pControl) const override;
    virtual bool SetItemIndex(CControlUI *pControl, int iIndex) override;

    // 查找菜单项。
    // 如果返回的菜单项包含子菜单，可以直接添加子菜单项
    // 否则，需要添加可展开属性
    CMenuElementUI *FindMenuItem(LPCTSTR pstrName);

    //////////////////////////////////////////////////////////////////////////
    virtual LPCTSTR GetClass(void) const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
};

/////////////////////////////////////////////////////////////////////////////////////
//
class DUILIB_API CMenuElementUI : public CListContainerElementUI
{
    friend CMenuWnd;
public:
    CMenuElementUI(void);
    virtual ~CMenuElementUI(void);

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
    virtual LPCTSTR GetClass(void) const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

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
