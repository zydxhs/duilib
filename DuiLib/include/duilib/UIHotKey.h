//2017-02-25 zhuyadong 添加快捷键控件
#ifndef __UIHOTKEY_H__
#define __UIHOTKEY_H__
#pragma once

namespace DuiLib {
class CHotKeyWnd;

class DUILIB_API CHotKeyUI : public CLabelUI
{
    friend class CHotKeyWnd;
public:
    CHotKeyUI();
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;
    virtual void SetEnabled(bool bEnable = true) override;
    virtual void SetText(LPCTSTR pstrText) override;
    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);

    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual bool SetVisible(bool bVisible = true) override;
    virtual void SetInternVisible(bool bVisible = true) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual void PaintStatusImage(HDC hDC) override;
    virtual void PaintText(HDC hDC) override;
public:
    void GetHotKey(WORD &wVirtualKeyCode, WORD &wModifiers) const;
    DWORD GetHotKey(void) const;
    void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);

protected:
    DWORD GetNativeBkColor() const;

protected:
    CHotKeyWnd *m_pWindow;
    UINT m_uButtonState;
    TDrawInfo m_diNormal;
    TDrawInfo m_diHot;
    TDrawInfo m_diFocused;
    TDrawInfo m_diDisabled;

    WORD m_wVirtualKeyCode;
    WORD m_wModifiers;
};
}


#endif