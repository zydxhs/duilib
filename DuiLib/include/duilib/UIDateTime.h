#ifndef __UIDATETIME_H__
#define __UIDATETIME_H__

#pragma once

namespace DuiLib {
class CDateTimeWnd;

/// 时间选择控件
class DUILIB_API CDateTimeUI : public CLabelUI
{
    friend class CDateTimeWnd;
public:
    CDateTimeUI();
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;
    virtual HWND GetNativeWindow() const override;

    SYSTEMTIME &GetTime();
    void SetTime(SYSTEMTIME *pst);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    void UpdateText();

    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;

    virtual void DoEvent(TEventUI &event) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

protected:
    SYSTEMTIME m_sysTime;
    int        m_nDTUpdateFlag;
    bool       m_bReadOnly;

    CDateTimeWnd *m_pWindow;
};
}
#endif // __UIDATETIME_H__