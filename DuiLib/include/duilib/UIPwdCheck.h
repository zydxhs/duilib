#ifndef __UIPWDCHECK_H__
#define __UIPWDCHECK_H__

#pragma once

namespace DuiLib {

enum { EPS_WEAK = 1, EPS_MIDDLE, EPS_STRONG };

class DUILIB_API CPwdCheckUI : public CLabelUI //CControlUI
{
public:
    CPwdCheckUI(void);
    virtual ~CPwdCheckUI(void);

    virtual LPCTSTR GetClass(void) const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void PaintBkColor(HDC hDC) override;
    virtual void PaintText(HDC hDC) override;

    void SetClrWeak(DWORD dwClr);
    DWORD GetClrWeak(void);
    void SetClrMiddle(DWORD dwClr);
    DWORD GetClrMiddle(void);
    void SetClrStrong(DWORD dwClr);
    DWORD GetClrStrong(void);
    void SetPwd(LPCTSTR pstrPwd);
    CDuiString GetPwd(void);

    void SetShowTxt(bool bShow);
    bool IsShowTxt(void);
    virtual CDuiString GetText(void) const override;
    virtual void SetText(LPCTSTR pstrText) override;

    BYTE GetPwdStrongth();

private:
    int CalcPwdStrongth(void);

private:
    BYTE        m_byPwdStrongth;    // 密码强度
    bool        m_bShowTxt;         // 是否显示文本

    DWORD       m_dwClrWeak;        // 弱密码
    DWORD       m_dwClrMiddle;      //
    DWORD       m_dwClrStrong;      // 强密码

    CDuiString  m_sTxtWeak;
    CDuiString  m_sTxtMiddle;
    CDuiString  m_sTxtStrong;
    CDuiString  m_sTxtWeakOrig;
    CDuiString  m_sTxtMiddleOrig;
    CDuiString  m_sTxtStrongOrig;

    CDuiString  m_sPwd;
};

}

#endif // __UIPWDCHECK_H__
