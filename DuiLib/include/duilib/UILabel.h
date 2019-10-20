#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib {
class DUILIB_API CLabelUI : public CControlUI
{
public:
    CLabelUI();
    virtual ~CLabelUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual void SetFixedWidth(int cx) override;
    virtual void SetFixedHeight(int cy) override;
    virtual void SetText(LPCTSTR pstrText) override;

    void SetTextStyle(UINT uStyle);
    UINT GetTextStyle() const;
    bool IsMultiLine();
    void SetMultiLine(bool bMultiLine = true);
    void SetFont(int index);
    int GetFont() const;
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);

    virtual void SetPadding(RECT rc) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void DoInit() override;

    virtual void PaintText(HDC hDC) override;

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
    int     m_iFont;
    UINT    m_uTextStyle;
    bool    m_bShowHtml;
    SIZE    m_szAvailableLast;
    SIZE    m_cxyFixedLast;
    bool    m_bNeedEstimateSize;
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
