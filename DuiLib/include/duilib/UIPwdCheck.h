#ifndef __UIPWDCHECK_H__
#define __UIPWDCHECK_H__

#pragma once

namespace DuiLib {
class DUILIB_API CPwdCheckUI : public CLabelUI //CControlUI
{
public:
    CPwdCheckUI(void);
    virtual ~CPwdCheckUI(void);

    virtual LPCTSTR GetClass(void) const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintText(HDC hDC);

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
    virtual CDuiString GetText(void) const;
    virtual void SetText(LPCTSTR pstrText);

private:
    int CalcPwdStrongth(void);

private:
    enum {EPS_WEAK = 1, EPS_MIDDLE, EPS_STRONG };

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
