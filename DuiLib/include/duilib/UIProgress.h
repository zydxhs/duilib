#ifndef __UIPROGRESS_H__
#define __UIPROGRESS_H__

#pragma once

namespace DuiLib {
class DUILIB_API CProgressUI : public CLabelUI
{
public:
    CProgressUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    bool IsHorizontal();
    void SetHorizontal(bool bHorizontal = true);
    int GetMinValue() const;
    void SetMinValue(int nMin);
    int GetMaxValue() const;
    void SetMaxValue(int nMax);
    int GetValue() const;
    void SetValue(int nValue);
    LPCTSTR GetForeImage() const;
    void SetForeImage(LPCTSTR pStrImage);
    LPCTSTR GetForeDisabledImage() const;
    void SetForeDisabledImage(LPCTSTR pStrImage);

    virtual void SetEnabled(bool bEnable = true) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void PaintStatusImage(HDC hDC) override;

protected:
    UINT m_uButtonState;
    bool m_bHorizontal;
    int m_nMax;
    int m_nMin;
    int m_nValue;

    TDrawInfo m_diFore;
    TDrawInfo m_diForeDisabled;
};

} // namespace DuiLib

#endif // __UIPROGRESS_H__
