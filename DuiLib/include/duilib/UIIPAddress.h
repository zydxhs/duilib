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

    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;
    virtual HWND GetNativeWindow() const;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
    virtual void SetEnabled(bool bEnable = true);
    virtual void SetText(LPCTSTR pstrText);

    virtual void SetVisible(bool bVisible = true);
    virtual void SetInternVisible(bool bVisible = true);
    virtual SIZE EstimateSize(SIZE szAvailable);
    virtual void DoEvent(TEventUI &event);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    virtual void PaintText(HDC hDC);

    void SetReadOnly(bool bReadOnly);
    bool IsReadOnly() const;
    HWND GetNativeEditHWND() const;

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