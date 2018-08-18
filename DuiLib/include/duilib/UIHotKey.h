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
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;
    void SetEnabled(bool bEnable = true);
    void SetText(LPCTSTR pstrText);
    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);

    void SetPos(RECT rc);
    bool SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI &event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintStatusImage(HDC hDC);
    void PaintText(HDC hDC);
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