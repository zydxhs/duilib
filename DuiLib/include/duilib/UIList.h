#ifndef __UILIST_H__
#define __UILIST_H__

#pragma once
#include "UIVerticalLayout.h"
#include "UIHorizontalLayout.h"

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

typedef int (CALLBACK *PULVCompareFunc)(UINT_PTR, UINT_PTR, UINT_PTR);

class CListHeaderUI;

#define UILIST_MAX_COLUMNS 64

typedef struct tagTListInfoUI
{
    int       nColumns;
    RECT      rcColumn[UILIST_MAX_COLUMNS];
    UINT      uFixedHeight;
    int       nFont;
    UINT      uTextStyle;
    RECT      rcPadding;
    DWORD     dwTextColor;
    DWORD     dwBkColor;
    TDrawInfo diBk;
    bool      bAlternateBk;
    DWORD     dwSelectedTextColor;
    DWORD     dwSelectedBkColor;
    TDrawInfo diSelected;
    DWORD     dwHotTextColor;
    DWORD     dwHotBkColor;
    TDrawInfo diHot;
    DWORD     dwDisabledTextColor;
    DWORD     dwDisabledBkColor;
    TDrawInfo diDisabled;
    int       iHLineSize;
    DWORD     dwHLineColor;
    int       iVLineSize;
    DWORD     dwVLineColor;
    bool      bShowHtml;
    bool      bMultiExpandable;
    bool      bCheckBox;
} TListInfoUI;


/////////////////////////////////////////////////////////////////////////////////////
//

class IListCallbackUI
{
public:
    virtual LPCTSTR GetItemText(CControlUI *pList, int iItem, int iSubItem) = 0;
};

class IListCmbCallbackUI
{
public:
    virtual void GetComboItems(CControlUI *pCtrl, int iItem, int iSubItem) = 0;
};

class IListOwnerUI
{
public:
    virtual TListInfoUI *GetListInfo() = 0;
    virtual int GetCurSel() const = 0;
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) = 0;
    virtual void DoEvent(TEventUI &event) = 0;
    virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
    virtual int GetExpandedItem() const = 0;
};

class IListUI : public IListOwnerUI
{
public:
    virtual CListHeaderUI *GetHeader() const = 0;
    virtual CContainerUI *GetList() const = 0;
    virtual IListCallbackUI *GetTextCallback() const = 0;
    virtual void SetTextCallback(IListCallbackUI *pCallback) = 0;

    virtual void ShowEdit(int nRow, int nColumn, RECT &rt, CDuiString &sItemTxt) = 0;
    virtual void HideEdit() = 0;
    virtual IListCmbCallbackUI *GetCmbItemCallback() const = 0;
    virtual void SetCmbItemCallback(IListCmbCallbackUI *pCallback) = 0;
    virtual void ShowCombo(int nRow, int nColumn, RECT &rt) = 0;
    virtual void HideCombo() = 0;
};

class IListItemUI
{
public:
    virtual int GetIndex() const = 0;
    virtual void SetIndex(int iIndex) = 0;
    virtual int GetDrawIndex() const = 0;
    virtual void SetDrawIndex(int iIndex) = 0;
    virtual IListOwnerUI *GetOwner() = 0;
    virtual void SetOwner(CControlUI *pOwner) = 0;
    virtual bool IsSelected() const = 0;
    virtual bool Select(bool bSelect = true, bool bTriggerEvent = true) = 0;
    virtual bool IsExpanded() const = 0;
    virtual bool Expand(bool bExpand = true) = 0;
    virtual void DrawItemText(HDC hDC, const RECT &rcItem) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class CListBodyUI;
class CListHeaderUI;
class CComboUI;

class DUILIB_API CListUI : public CVerticalLayoutUI, public IListUI
{
public:
    CListUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    bool GetScrollSelect();
    void SetScrollSelect(bool bScrollSelect);
    int GetCurSel() const;
    bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true);

    CControlUI *GetItemAt(int iIndex) const;
    int GetItemIndex(CControlUI *pControl) const;
    bool SetItemIndex(CControlUI *pControl, int iIndex);
    bool SetMultiItemIndex(CControlUI *pStartControl, int iCount, int iNewStartIndex);
    int GetCount() const;
    bool Add(CControlUI *pControl);
    bool AddAt(CControlUI *pControl, int iIndex);
    bool Remove(CControlUI *pControl, bool bDoNotDestroy = false);
    bool RemoveAt(int iIndex, bool bDoNotDestroy = false);
    void RemoveAll();
    bool RemoveCount(int iIndex, int iCount, bool bDoNotDestroy = false);

    int FindItemByTag(UINT_PTR pTag);
    int FindItemByUserData(LPCTSTR pstrText);
    CControlUI *GetItemByTag(UINT_PTR pTag);
    CControlUI *GetItemByUserData(LPCTSTR pstrText);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

    int GetChildMargin() const;
    void SetChildMargin(int iMargin);

    CListHeaderUI *GetHeader() const;
    CContainerUI *GetList() const;
    TListInfoUI *GetListInfo();

    UINT GetItemFixedHeight();
    void SetItemFixedHeight(UINT nHeight);
    int GetItemFont(int index);
    void SetItemFont(int index);
    UINT GetItemTextStyle();
    void SetItemTextStyle(UINT uStyle);
    RECT GetItemPadding() const;
    void SetItemPadding(RECT rc);
    DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
    DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
    LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
    DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
    DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
    LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
    DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
    DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
    LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
    DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
    DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
    LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
    int GetItemHLineSize() const;
    void SetItemHLineSize(int iSize);
    DWORD GetItemHLineColor() const;
    void SetItemHLineColor(DWORD dwLineColor);
    int GetItemVLineSize() const;
    void SetItemVLineSize(int iSize);
    DWORD GetItemVLineColor() const;
    void SetItemVLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    void SetMultiExpanding(bool bMultiExpandable);
    int GetExpandedItem() const;
    bool ExpandItem(int iIndex, bool bExpand = true);

    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(TEventUI &event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    IListCallbackUI *GetTextCallback() const;
    void SetTextCallback(IListCallbackUI *pCallback);

    SIZE GetScrollPos() const;
    SIZE GetScrollRange() const;
    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();
    void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
    virtual CScrollBarUI *GetVerticalScrollBar() const;
    virtual CScrollBarUI *GetHorizontalScrollBar() const;
    bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

    TDrawInfo &GetUnSelImage(void);
    TDrawInfo &GetSelImage(void);
    void GetAllSelectedItem(CDuiValArray &arySelIdx);
    void SetAllItemSelected(bool bSelect);
    virtual void DoInit();

    void GetLastModifiedItem(int &nRow, int &nColumn);
    virtual void ShowEdit(int nRow, int nColumn, RECT &rt, CDuiString &sItemTxt);
    virtual void HideEdit();
    CEditUI *GetEditUI();

    virtual IListCmbCallbackUI *GetCmbItemCallback() const;
    virtual void SetCmbItemCallback(IListCmbCallbackUI *pCallback);
    virtual void ShowCombo(int nRow, int nColumn, RECT &rt);
    virtual void HideCombo();
    CComboUI *GetComboUI();

protected:
    bool OnFirstHeaderItemNotify(void *pParam);
    bool OnEditNotify(void *pParam);
    bool OnComboNotify(void *pParam);
    bool OnScrollNotify(void *pParam);

protected:
    bool             m_bScrollSelect;
    int              m_iCurSel;
    int              m_iExpandedItem;
    IListCallbackUI *m_pCallback;
    CListBodyUI     *m_pList;
    CListHeaderUI   *m_pHeader;
    TListInfoUI      m_ListInfo;

    TDrawInfo        m_diUnSel;         // 复选框未选中状态图片
    TDrawInfo        m_diSel;           // 复选框选中状态图片

    // 2019-05-19 zhuyadong 编辑框、下拉框使用说明：
    // 1. ListHeaderItem 的属性 editable/comboable，用于标识该列是否可编辑、下拉框
    // 2. 编辑框、下拉框的消息，需要开发人员处理。比如编辑框的过滤、回车键，下拉框的展开、收起、选项改变等等
    // 3. 用户：双击时显示编辑框、下拉框；
    // 4. 编辑框中按回车键，自动更新List、隐藏编辑框，开发人员需要处理 DUI_MSGTYPE_RETURN 保存更新后的数据
    int                 m_nRow;         // 编辑框所在的行、列
    int                 m_nColumn;
    CEditUI            *m_pEdit;
    IListCmbCallbackUI *m_pCmbCallback; // 用于获取下拉框可选项列表
    CComboUI           *m_pCombo;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListHeaderUI : public CHorizontalLayoutUI
{
public:
    CListHeaderUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    SIZE EstimateSize(SIZE szAvailable);
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListHeaderItemUI : public CContainerUI
{
public:
    CListHeaderItemUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    void SetEnabled(bool bEnable = true);

    bool IsDragable() const;
    void SetDragable(bool bDragable);
    DWORD GetSepWidth() const;
    void SetSepWidth(int iWidth);
    DWORD GetTextStyle() const;
    void SetTextStyle(UINT uStyle);
    DWORD GetTextColor() const;
    void SetTextColor(DWORD dwTextColor);
    DWORD GetSepColor() const;
    void SetSepColor(DWORD dwSepColor);
    void SetPadding(RECT rc);
    void SetFont(int index);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetSepImage() const;
    void SetSepImage(LPCTSTR pStrImage);

    void DoEvent(TEventUI &event);
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    RECT GetThumbRect() const;

    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

    void SetEditable(bool bEditable);
    bool IsEditable();

    void SetComboable(bool bComboable);
    bool IsComboable();

protected:
    POINT     m_ptLastMouse;
    bool      m_bDragable;
    bool      m_bShowHtml;
    bool      m_bEditable;      // 当前列是否支持编辑
    bool      m_bComboable;     // 当前列是否支持下拉框
    UINT      m_uButtonState;
    int       m_iSepWidth;
    DWORD     m_dwTextColor;
    DWORD     m_dwSepColor;
    int       m_iFont;
    UINT      m_uTextStyle;
    TDrawInfo m_diNormal;
    TDrawInfo m_diHot;
    TDrawInfo m_diPushed;
    TDrawInfo m_diFocused;
    TDrawInfo m_diSep;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListElementUI : public CControlUI, public IListItemUI
{
public:
    CListElementUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetEnabled(bool bEnable = true);

    int GetIndex() const;
    void SetIndex(int iIndex);
    int GetDrawIndex() const;
    void SetDrawIndex(int iIndex);

    IListOwnerUI *GetOwner();
    void SetOwner(CControlUI *pOwner);
    bool SetVisible(bool bVisible = true);

    bool IsSelected() const;
    bool Select(bool bSelect = true, bool bTriggerEvent = true);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);

    void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    bool Activate();

    void DoEvent(TEventUI &event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void DrawItemBk(HDC hDC, const RECT &rcItem);

    int GetMouseColumn(POINT pt);                       // 返回鼠标所在的列
    // 返回鼠标所在列的矩形。
    // bList=true 表示相对当前列表的位置；false 表示相对当前窗体的位置
    RECT GetSubItemPos(int nColumn, bool bList = false);
protected:
    int m_iIndex;
    int m_iDrawIndex;
    bool m_bSelected;
    UINT m_uButtonState;
    IListOwnerUI *m_pOwner;
    bool m_bCustomBk;
};


/////////////////////////////////////////////////////////////////////////////////////
// CListLabelElementUI 不支持复选框

class DUILIB_API CListLabelElementUI : public CListElementUI
{
public:
    CListLabelElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetOwner(CControlUI *pOwner);

    void SetFixedWidth(int cx);
    void SetFixedHeight(int cy);
    void SetText(LPCTSTR pstrText);

    void DoEvent(TEventUI &event);
    SIZE EstimateSize(SIZE szAvailable);
    bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl);

    void DrawItemText(HDC hDC, const RECT &rcItem);

protected:
    SIZE    m_cxyFixedLast;
    bool    m_bNeedEstimateSize;

    SIZE    m_szAvailableLast;
    UINT    m_uFixedHeightLast;
    int     m_nFontLast;
    UINT    m_uTextStyleLast;
    RECT    m_rcTextPaddingLast;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListTextElementUI : public CListLabelElementUI
{
public:
    CListTextElementUI();
    ~CListTextElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    LPCTSTR GetText(int iIndex) const;
    void SetText(int iIndex, LPCTSTR pstrText);

    void SetOwner(CControlUI *pOwner);
    CDuiString *GetLinkContent(int iIndex);

    void DoEvent(TEventUI &event);
    SIZE EstimateSize(SIZE szAvailable);

    void DrawItemText(HDC hDC, const RECT &rcItem);

    void SetCheckBoxState(bool bSelect);
    bool GetCheckBoxState(void);
protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
    RECT m_rcLinks[MAX_LINK];
    CDuiString m_sLinks[MAX_LINK];
    int m_nHoverLink;
    IListUI *m_pOwner;
    CDuiPtrArray m_aTexts;

    CDuiString m_sTextLast;
    bool m_bCheckBoxSelect;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListContainerElementUI : public CContainerUI, public IListItemUI
{
public:
    CListContainerElementUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    int GetIndex() const;
    void SetIndex(int iIndex);
    int GetDrawIndex() const;
    void SetDrawIndex(int iIndex);

    IListOwnerUI *GetOwner();
    void SetOwner(CControlUI *pOwner);
    bool SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);

    bool IsSelected() const;
    bool Select(bool bSelect = true, bool bTriggerEvent = true);
    bool IsExpandable() const;
    void SetExpandable(bool bExpandable);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);

    void Invalidate(); // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    bool Activate();

    void DoEvent(TEventUI &event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl);

    void DrawItemText(HDC hDC, const RECT &rcItem);
    void DrawItemBk(HDC hDC, const RECT &rcItem);

    SIZE EstimateSize(SIZE szAvailable);

    void SetCheckBoxState(bool bSelect);
    bool GetCheckBoxState(void);

protected:
    bool GetCheckBoxState(CContainerUI *pRoot);
    void SetCheckBoxState(CContainerUI *pRoot, bool bSelect);
protected:
    int m_iIndex;
    int m_iDrawIndex;
    bool m_bSelected;
    bool m_bExpandable;
    bool m_bExpand;
    UINT m_uButtonState;
    IListOwnerUI *m_pOwner;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListHBoxElementUI : public CListContainerElementUI
{
public:
    CListHBoxElementUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetPos(RECT rc, bool bNeedInvalidate = true);
    bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl);
};

} // namespace DuiLib

#endif // __UILIST_H__
