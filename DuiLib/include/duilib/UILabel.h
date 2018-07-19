#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib {
class DUILIB_API CLabelUI : public CControlUI
{
public:
    CLabelUI();
    ~CLabelUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetFixedWidth(int cx);
    void SetFixedHeight(int cy);
    void SetText(LPCTSTR pstrText);

    void SetTextStyle(UINT uStyle);
    UINT GetTextStyle() const;
    bool IsMultiLine();
    void SetMultiLine(bool bMultiLine = true);
    void SetTextColor(DWORD dwTextColor);
    DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
    DWORD GetDisabledTextColor() const;
    void SetFont(int index);
    int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
    void DoEvent(TEventUI &event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void PaintText(HDC hDC);

    void        SetAutoWidth(bool bAutoWidth);
    bool        GetAutoWidth(void);
    void        SetEnabledEffect(bool _EnabledEffect);
    bool        GetEnabledEffect();
#ifdef USE_GDIPLUS
    void        SetEnabledLuminous(bool bEnableLuminous);
    bool        GetEnabledLuminous();
    void        SetGradientLength(int _GradientLength);
    int         GetGradientLength();
    void        SetGradientAngle(int _SetGradientAngle);
    int         GetGradientAngle();
    void        SetLuminousFuzzy(float fFuzzy);
    float       GetLuminousFuzzy();
    void        SetEnabledStroke(bool _EnabledStroke);
    bool        GetEnabledStroke();
    void        SetStrokeColor(DWORD _StrokeColor);
    DWORD       GetStrokeColor();
    void        SetEnabledShadow(bool _EnabledShadow);
    bool        GetEnabledShadow();
    void        SetShadowOffset(int _offset, int _angle);
    RectF       GetShadowOffset();
    void        SetTextColor1(DWORD _TextColor1);
    DWORD       GetTextColor1();
    void        SetTextShadowColorA(DWORD _TextShadowColorA);
    DWORD       GetTextShadowColorA();
    void        SetTextShadowColorB(DWORD _TextShadowColorB);
    DWORD       GetTextShadowColorB();
#endif // USE_GDIPLUS

protected:
    void    PaintTextEffect(HDC hDC, RECT rt);
    virtual int *GetLinksNum(void) { return NULL; }
    virtual RECT *GetRectLinks(void) { return NULL; }
    virtual CDuiString *GetStringLinks(void) { return NULL; }
    RECT GetTextRect(RECT rc);

protected:
    LPWSTR  m_pWideText;
    DWORD   m_dwTextColor;
    DWORD   m_dwDisabledTextColor;
    int     m_iFont;
    UINT    m_uTextStyle;
    RECT    m_rcTextPadding;
    bool    m_bShowHtml;
    SIZE    m_szAvailableLast;
    SIZE    m_cxyFixedLast;
    bool    m_bNeedEstimateSize;
    bool    m_bAutoWidth;
    bool    m_bEnableEffect;

#ifdef USE_GDIPLUS
    float                   m_fLuminousFuzzy;
    int                     m_GradientLength;
    int                     m_GradientAngle;
    bool                    m_bEnableLuminous;
    bool                    m_EnabledStroke;
    bool                    m_EnabledShadow;
    DWORD                   m_dwTextColor1;
    DWORD                   m_dwTextShadowColorA;
    DWORD                   m_dwTextShadowColorB;
    DWORD                   m_dwStrokeColor;
    Gdiplus::RectF          m_ShadowOffset;
#endif // USE_GDIPLUS
};
}

#endif // __UILABEL_H__
