#ifndef __UIIPADDRESS_H__
#define __UIIPADDRESS_H__

#pragma once

namespace DuiLib {

enum EMIPField;
class CIPAddressWnd;

class DUILIB_API CIPAddressUI : public CLabelUI
{
    friend class CIPAddressWnd;
public:
    CIPAddressUI();
    virtual ~CIPAddressUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;
    virtual HWND GetNativeWindow() const override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;
    virtual void SetEnabled(bool bEnable = true) override;
    virtual void SetText(LPCTSTR pstrText) override;

    virtual bool SetVisible(bool bVisible = true) override;
    virtual void SetInternVisible(bool bVisible = true) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual void PaintText(HDC hDC) override;

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;

    // 主机序。点分表示法中第1个字段，在最高位
    void SetIp(DWORD dwIp);
    DWORD GetIp(void);

    void SetIpStr(CDuiString strIp);
    CDuiString GetIpStr(void);

protected:
    DWORD GetNativeEditBkColor() const;

    void SetField(EMIPField eField, int n);
    CDuiString GetFieldStr(EMIPField eField);
    CDuiRect GetFieldPos(EMIPField eField);
    void MoveToNextField(EMIPField eField);
    void MoveToBeforeField(EMIPField eField);

protected:
    CDuiRect        m_rcField[4];
    CIPAddressWnd  *m_pWindow;
    EMIPField       m_eField;
    DWORD           m_dwIp;
    bool            m_bReadOnly;
    UINT            m_uEditState;
};

}
#endif // __UIIPADDRESS_H__