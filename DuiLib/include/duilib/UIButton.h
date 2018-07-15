#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DuiLib {
class DUILIB_API CButtonUI : public CLabelUI
{
public:
    CButtonUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    UINT GetControlFlags() const;

    bool Activate();
    void SetEnabled(bool bEnable = true);
    void DoEvent(TEventUI &event);

    LPCTSTR GetNormalImage();
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage();
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage();
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage();
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage();
    void SetDisabledImage(LPCTSTR pStrImage);
    LPCTSTR GetForeImage();
    void SetForeImage(LPCTSTR pStrImage);
    LPCTSTR GetHotForeImage();
    void SetHotForeImage(LPCTSTR pStrImage);

    // 对应按钮的5个状态图
    void SetFiveStatusImage(LPCTSTR pStrImage);
    void SetFadeAlphaDelta(BYTE uDelta);
    BYTE GetFadeAlphaDelta();

    void SetHotBkColor(DWORD dwColor);
    DWORD GetHotBkColor() const;
    void SetHotTextColor(DWORD dwColor);
    DWORD GetHotTextColor() const;
    void SetPushedTextColor(DWORD dwColor);
    DWORD GetPushedTextColor() const;
    void SetFocusedTextColor(DWORD dwColor);
    DWORD GetFocusedTextColor() const;
    SIZE EstimateSize(SIZE szAvailable);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    virtual void PaintBkColor(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);

protected:
    void DrawNormalBkImg(HDC hDC, TDrawInfo &diNormal, TDrawInfo *pdiHot = NULL, bool bHot = false);

protected:
    UINT m_uButtonState;

    // Control::m_dwBackColor       正常 背景色
    // Label::m_dwTextColor         正常 前景色
    // Label::m_dwDisabledTextColor 禁用 前景色
    DWORD m_dwHotBkColor;           // 鼠标悬浮 背景色
    DWORD m_dwHotTextColor;         // 鼠标悬浮 前景色
    DWORD m_dwFocusedTextColor;     // 焦点状态 前景色
    DWORD m_dwPushedTextColor;      // 鼠标按下 前景色
    BYTE m_byFadeAlpha;
    BYTE m_byFadeAlphaDelta;
    BYTE m_byDisableSeconds;        // 鼠标/键盘动作响应频率。默认0，关闭该特性，非0表示间隔秒数。
    BYTE m_byEllapseSeconds;        // 响应鼠标/键盘动作后，流逝的秒数

    TDrawInfo m_diNormal;
    TDrawInfo m_diHot;
    TDrawInfo m_diHotFore;
    TDrawInfo m_diPushed;
    TDrawInfo m_diFocused;
    TDrawInfo m_diDisabled;
};

}   // namespace DuiLib

#endif // __UIBUTTON_H__