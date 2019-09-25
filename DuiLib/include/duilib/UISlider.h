#ifndef __UISLIDER_H__
#define __UISLIDER_H__

#pragma once

namespace DuiLib {
class DUILIB_API CSliderUI : public CProgressUI
{
public:
    CSliderUI();

    virtual LPCTSTR GetClass() const override;
    virtual UINT GetControlFlags() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual void SetEnabled(bool bEnable = true) override;

    int GetChangeStep();
    void SetChangeStep(int step);
    void SetThumbSize(SIZE szXY);
    RECT GetThumbRect() const;
    bool IsImmMode() const;
    void SetImmMode(bool bImmMode);
    LPCTSTR GetThumbImage() const;
    void SetThumbImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbHotImage() const;
    void SetThumbHotImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbPushedImage() const;
    void SetThumbPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetThumbDisabledImage() const;
    void SetThumbDisabledImage(LPCTSTR pStrImage);

    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void PaintStatusImage(HDC hDC) override;

protected:
    SIZE m_szThumb;
    // UINT m_uButtonState;
    int m_nStep;
    bool m_bImmMode;

    TDrawInfo m_diThumb;
    TDrawInfo m_diThumbHot;
    TDrawInfo m_diThumbPushed;
    TDrawInfo m_diThumbDisabled;
};
}

#endif // __UISLIDER_H__