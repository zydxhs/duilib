#ifndef __UIOPTION_H__
#define __UIOPTION_H__

#pragma once

namespace DuiLib {
class DUILIB_API COptionUI : public CButtonUI
{
public:
    COptionUI();
    virtual ~COptionUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual void SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit = true) override;

    virtual bool Activate() override;
    virtual void SetEnabled(bool bEnable = true) override;

    void SetSelTextColor(DWORD dwTextColor);
    DWORD GetSelTextColor();

    // 选中时背景色。未选中时背影色，参考Buttton
    void SetSelNormalBkColor(DWORD dwColor);
    DWORD GetSelNormalBkColor();
    void SetSelHotBkColor(DWORD dwColor);
    DWORD GetSelHotBkColor();
    void SetSelFocusedBkColor(DWORD dwColor);
    DWORD GetSelFocusedBkColor();
    void SetSelPushedBkColor(DWORD dwColor);
    DWORD GetSelPushedBkColor();
    void SetSelDisabledBkColor(DWORD dwColor);
    DWORD GetSelDisabledBkColor();

    // 选中
    LPCTSTR GetSelNormalImage();
    void SetSelNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetSelHotImage();
    void SetSelHotImage(LPCTSTR pStrImage);
    LPCTSTR GetSelFocusedImage();
    void SetSelFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetSelPushedImage();
    void SetSelPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetSelDisabledImage();
    void SetSelDisabledImage(LPCTSTR pStrImage);
    // 未选中
    LPCTSTR GetUnselNormalImage();
    void SetUnselNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetUnselHotImage();
    void SetUnselHotImage(LPCTSTR pStrImage);
    LPCTSTR GetUnselFocusedImage();
    void SetUnselFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetUnselPushedImage();
    void SetUnselPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetUnselDisabledImage();
    void SetUnselDisabledImage(LPCTSTR pStrImage);

    LPCTSTR GetForeImage();
    void SetForeImage(LPCTSTR pStrImage);

    LPCTSTR GetGroup() const;
    void SetGroup(LPCTSTR pStrGroupName = NULL);
    bool IsSelected() const;
    virtual void Selected(bool bSelected, bool bTriggerEvent = true);

    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual void PaintBkColor(HDC hDC) override;
    virtual void PaintStatusImage(HDC hDC) override;
    virtual void PaintText(HDC hDC) override;
    virtual void PaintBorder(HDC hDC) override;

protected:
    void SwitchTabLayoutPage(void);

protected:
    bool            m_bSelected;
    CDuiString      m_sGroupName;

    // 选中时 5 态颜色
    DWORD           m_dwSelNormalBkColor;       // 正常
    DWORD           m_dwSelHotBkColor;          // 悬浮/选中 背景色
    DWORD           m_dwSelFocusedBkColor;      // 悬浮
    DWORD           m_dwSelPushedBkColor;       // 按下 / 选中 前景色 其它颜色见Buttton
    DWORD           m_dwSelDisabledBkColor;     // 按下/选中 背景色

    TDrawInfo       m_diSelNormal;      // 选中状态 正常
    TDrawInfo       m_diSelHot;         // 选中状态 鼠标悬浮
    TDrawInfo       m_diSelFocused;     // 选中状态 焦点状态
    TDrawInfo       m_diSelPushed;      // 选中状态 按下
    TDrawInfo       m_diSelDisabled;    // 选中状态 禁用

    TDrawInfo       m_diFore;

    int             m_nBindTabIndex;    // 单击后触发的 TabLayout 页面索引号
    CDuiString      m_sBindTabLayout;   // 绑定的 TabLayout 控件名
};

class CButtonGroupImpl;
class DUILIB_API CButtonGroup
{
public:
    CButtonGroup(void);
    virtual ~CButtonGroup(void);

    void AddButton(COptionUI *pBtn, int nId = -1);
    void DelButton(COptionUI *pBtn);

    void SetButtonId(COptionUI *pBtn, int nId);
    int  GetButtonId(COptionUI *pBtn);
    COptionUI *GetButton(int nId);

    COptionUI *GetSelectedButton(void);
    int        GetSelectedId(void);

    void SetSelectedButton(int nId);
    void SetSelectedButton(COptionUI *pBtn);
private:
    CButtonGroupImpl *m_pBtnGroupImpl;
};

} // namespace DuiLib

#endif // __UIOPTION_H__