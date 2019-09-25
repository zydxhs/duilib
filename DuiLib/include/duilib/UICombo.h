#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CComboWnd;

class DUILIB_API CComboUI : public CContainerUI, public IListOwnerUI
{
    friend class CComboWnd;
public:
    CComboUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual void DoInit() override;
    virtual UINT GetControlFlags() const override;

    virtual CDuiString GetText() const override;
    virtual void SetEnabled(bool bEnable = true) override;

    CDuiString GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);

    virtual int GetCurSel() const override;
    bool GetSelectCloseFlag();
    void SetSelectCloseFlag(bool flag);
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) override;
    virtual bool ExpandItem(int iIndex, bool bExpand = true) override;
    virtual int GetExpandedItem() const override;

    virtual bool SetItemIndex(CControlUI *pControl, int iNewIndex) override;
    virtual bool SetMultiItemIndex(CControlUI *pStartControl, int iCount, int iNewStartIndex) override;
    virtual bool Add(CControlUI *pControl) override;
    virtual bool AddAt(CControlUI *pControl, int iIndex) override;
    virtual bool Remove(CControlUI *pControl, bool bDoNotDestroy = false) override;
    virtual bool RemoveAt(int iIndex, bool bDoNotDestroy = false) override;
    virtual void RemoveAll() override;

    virtual bool Activate() override;

    bool GetShowText() const;
    void SetShowText(bool flag);
    void SetPadding(RECT rc);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);

    virtual TListInfoUI *GetListInfo() override;
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

    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;
    virtual void PaintText(HDC hDC) override;
    virtual void PaintStatusImage(HDC hDC) override;

    bool SelectItemByTag(UINT_PTR pTag);
    bool SelectItemByUserData(LPCTSTR pstrText);

protected:
    void SendDropUpNty();   // 2018-08-21 zhuyadong 下拉框收起通知

protected:
    CComboWnd  *m_pWindow;

    int         m_iCurSel;
    bool        m_bShowText;
    bool        m_bSelectCloseFlag;
    CDuiString  m_sDropBoxAttributes;
    SIZE        m_szDropBox;
    UINT        m_uButtonState;

    TDrawInfo   m_diNormal;
    TDrawInfo   m_diHot;
    TDrawInfo   m_diPushed;
    TDrawInfo   m_diFocused;
    TDrawInfo   m_diDisabled;

    TListInfoUI m_ListInfo;
};

} // namespace DuiLib

#endif // __UICOMBO_H__
