#ifndef __UICONTAINER_H__
#define __UICONTAINER_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class IContainerUI
{
public:
    virtual CControlUI *GetItemAt(int iIndex) const = 0;
    virtual int GetItemIndex(CControlUI *pControl) const  = 0;
    virtual bool SetItemIndex(CControlUI *pControl, int iNewIndex) = 0;
    virtual bool SetMultiItemIndex(CControlUI *pStartControl, int iCount, int iNewStartIndex) = 0;
    virtual int GetCount() const = 0;
    virtual bool Add(CControlUI *pControl) = 0;
    virtual bool AddAt(CControlUI *pControl, int iIndex)  = 0;
    virtual bool Remove(CControlUI *pControl, bool bDoNotDestroy = false) = 0;
    virtual bool RemoveAt(int iIndex, bool bDoNotDestroy = false)  = 0;
    virtual void RemoveAll() = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//
class CScrollBarUI;

class DUILIB_API CContainerUI : public CControlUI, public IContainerUI
{
public:
    CContainerUI();
    virtual ~CContainerUI();

public:
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    CControlUI *GetItemAt(int iIndex) const;
    int GetItemIndex(CControlUI *pControl) const;
    bool SetItemIndex(CControlUI *pControl, int iNewIndex);
    bool SetMultiItemIndex(CControlUI *pStartControl, int iCount, int iNewStartIndex);
    int GetCount() const;
    bool Add(CControlUI *pControl);
    bool AddAt(CControlUI *pControl, int iIndex);
    bool Remove(CControlUI *pControl, bool bDoNotDestroy = false);
    bool RemoveAt(int iIndex, bool bDoNotDestroy = false);
    void RemoveAll();
    bool RemoveCount(int iIndex, int iCount, bool bDoNotDestroy = false);

    virtual void DoEvent(TEventUI &event);
    virtual bool SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true);
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);

    virtual RECT GetInset() const;
    virtual void SetInset(RECT rcInset); // 设置内边距，相当于设置客户区
    virtual int GetChildMargin() const;
    virtual void SetChildMargin(int iMargin);
    virtual UINT GetChildAlign() const;
    virtual void SetChildAlign(UINT iAlign);
    virtual UINT GetChildVAlign() const;
    virtual void SetChildVAlign(UINT iVAlign);
    virtual bool IsAutoDestroy() const;
    virtual void SetAutoDestroy(bool bAuto);
    virtual bool IsDelayedDestroy() const;
    virtual void SetDelayedDestroy(bool bDelayed);
    virtual bool IsMouseChildEnabled() const;
    virtual void SetMouseChildEnabled(bool bEnable = true);

    virtual int FindSelectable(int iIndex, bool bForward = true) const;

    virtual void ReloadText(void);

    RECT GetClientPos() const;      // 返回客户区域（除去scrollbar和inset/padding）
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);
    bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl);
    virtual SIZE EstimateSize(SIZE szAvailable);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit = true);
    CControlUI *FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    bool SetSubControlText(LPCTSTR pstrSubControlName, LPCTSTR pstrText);
    bool SetSubControlFixedHeight(LPCTSTR pstrSubControlName, int cy);
    bool SetSubControlFixedWdith(LPCTSTR pstrSubControlName, int cx);
    bool SetSubControlUserData(LPCTSTR pstrSubControlName, LPCTSTR pstrText);

    CDuiString GetSubControlText(LPCTSTR pstrSubControlName);
    int GetSubControlFixedHeight(LPCTSTR pstrSubControlName);
    int GetSubControlFixedWdith(LPCTSTR pstrSubControlName);
    const CDuiString GetSubControlUserData(LPCTSTR pstrSubControlName);
    CControlUI *FindSubControl(LPCTSTR pstrSubControlName);

    virtual SIZE GetScrollPos() const;
    virtual SIZE GetScrollRange() const;
    virtual void SetScrollPos(SIZE szPos);
    virtual void LineUp();
    virtual void LineDown();
    virtual void PageUp();
    virtual void PageDown();
    virtual void HomeUp();
    virtual void EndDown();
    virtual void LineLeft();
    virtual void LineRight();
    virtual void PageLeft();
    virtual void PageRight();
    virtual void HomeLeft();
    virtual void EndRight();
    virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false);
    virtual CScrollBarUI *GetVerticalScrollBar() const;
    virtual CScrollBarUI *GetHorizontalScrollBar() const;

    int FindItemByTag(UINT_PTR pTag);
    int FindItemByUserData(LPCTSTR pstrText);
    CControlUI *GetItemByTag(UINT_PTR pTag);
    CControlUI *GetItemByUserData(LPCTSTR pstrText);

protected:
    virtual void SetFloatPos(int iIndex);
    virtual void ProcessScrollBar(RECT rc, int cxRequired, int cyRequired);

protected:
    CDuiPtrArray m_items;
    RECT m_rcInset;         // 内边距
    int m_iChildMargin;     // 子控件外边距
    UINT m_iChildAlign;     // 子控件水平对齐方式
    UINT m_iChildVAlign;    // 子控件垂直对齐方式
    bool m_bAutoDestroy;    // 自动销毁子控件
    bool m_bDelayedDestroy;
    bool m_bMouseChildEnabled;
    bool m_bScrollProcess; // 防止SetPos循环调用

    CScrollBarUI *m_pVerticalScrollBar;
    CScrollBarUI *m_pHorizontalScrollBar;
};

} // namespace DuiLib

#endif // __UICONTAINER_H__
