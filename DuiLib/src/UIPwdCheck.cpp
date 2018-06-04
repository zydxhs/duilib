#include "stdafx.h"
#include "deelx.h"

namespace DuiLib {

CPwdCheckUI::CPwdCheckUI(void) :
    m_byPwdStrongth(EPS_WEAK),
    m_bShowTxt(false),
    m_dwClrWeak(0),
    m_dwClrMiddle(0),
    m_dwClrStrong(0)
{

}

CPwdCheckUI::~CPwdCheckUI(void)
{

}

LPCTSTR CPwdCheckUI::GetClass(void) const
{
    return DUI_CTR_PWDCHECK;
}

LPVOID CPwdCheckUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_PWDCHECK) == 0) { return static_cast<CPwdCheckUI *>(this); }

    return CLabelUI::GetInterface(pstrName);
}


void CPwdCheckUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("clrweak")) == 0)
    {
        while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) { pstrValue = ::CharNext(pstrValue); }

        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetClrWeak(clrColor);
    }
    else if (_tcscmp(pstrName, _T("clrmiddle")) == 0)
    {
        while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) { pstrValue = ::CharNext(pstrValue); }

        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetClrMiddle(clrColor);
    }
    else if (_tcscmp(pstrName, _T("clrstrong")) == 0)
    {
        while (*pstrValue > _T('\0') && *pstrValue <= _T(' ')) { pstrValue = ::CharNext(pstrValue); }

        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        SetClrStrong(clrColor);
    }
    else if (_tcscmp(pstrName, _T("showtext")) == 0)
    {
        SetShowTxt(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if (_tcscmp(pstrName, _T("text")) == 0)
    {
        SetText(pstrValue);
    }
    else if (_tcscmp(pstrName, _T("pwd")) == 0)
    {
        SetPwd(pstrValue);
    }
    else if (_tcscmp(pstrName, _T("dragenable")) == 0) { DUITRACE(_T("不支持属性:dragenable")); }
    else if (_tcscmp(pstrName, _T("dragimage")) == 0) { DUITRACE(_T("不支持属性:drageimage")); }
    else if (_tcscmp(pstrName, _T("dropenable")) == 0) { DUITRACE(_T("不支持属性:dropenable")); }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CLabelUI::SetAttribute(pstrName, pstrValue); }
}

void CPwdCheckUI::PaintBkColor(HDC hDC)
{
    CLabelUI::PaintBkColor(hDC);

    RECT rc = m_rcItem;
    rc.right = (rc.right - rc.left) / 3 + rc.left;
    CRenderEngine::DrawColor(hDC, rc, GetAdjustColor(m_dwClrWeak));

    if (m_byPwdStrongth >= EPS_MIDDLE)
    {
        int w = rc.right - rc.left;
        rc.left += w;
        rc.right += w;
        CRenderEngine::DrawColor(hDC, rc, GetAdjustColor(m_dwClrMiddle));
    }

    if (m_byPwdStrongth >= EPS_STRONG)
    {
        rc.left = rc.right;
        rc.right = m_rcItem.right;
        CRenderEngine::DrawColor(hDC, rc, GetAdjustColor(m_dwClrStrong));
    }
}

void CPwdCheckUI::PaintText(HDC hDC)
{
    if (!m_bShowTxt || m_sTxtWeak.IsEmpty() || m_sTxtMiddle.IsEmpty() || m_sTxtStrong.IsEmpty()) { return; }

    RECT rcBak = m_rcItem;

    m_rcItem.right = (m_rcItem.right - m_rcItem.left) / 3 + m_rcItem.left;
    m_sText = m_sTxtWeak;
    CLabelUI::PaintText(hDC);

    if (m_byPwdStrongth >= EPS_MIDDLE)
    {
        int w = m_rcItem.right - m_rcItem.left;
        m_rcItem.left += w;
        m_rcItem.right += w;
        m_sText = m_sTxtMiddle;
        CLabelUI::PaintText(hDC);
    }

    if (m_byPwdStrongth >= EPS_STRONG)
    {
        m_rcItem.left = m_rcItem.right;
        m_rcItem.right = rcBak.right;
        m_sText = m_sTxtStrong;
        CLabelUI::PaintText(hDC);
    }

    m_sText = _T("");
    m_rcItem = rcBak;
}

void CPwdCheckUI::SetClrWeak(DWORD dwClr)
{
    m_dwClrWeak = dwClr;
    NeedUpdate();
}

DWORD CPwdCheckUI::GetClrWeak(void)
{
    return m_dwClrWeak;
}

void CPwdCheckUI::SetClrMiddle(DWORD dwClr)
{
    m_dwClrMiddle = dwClr;
    NeedUpdate();
}

DWORD CPwdCheckUI::GetClrMiddle(void)
{
    return m_dwClrMiddle;
}

void CPwdCheckUI::SetClrStrong(DWORD dwClr)
{
    m_dwClrStrong = dwClr;
    NeedUpdate();
}

DWORD CPwdCheckUI::GetClrStrong(void)
{
    return m_dwClrStrong;
}

void CPwdCheckUI::SetPwd(LPCTSTR pstrPwd)
{
    if (NULL != pstrPwd && m_sPwd != pstrPwd)
    {
        m_sPwd = pstrPwd;
        m_byPwdStrongth = CalcPwdStrongth();
        NeedUpdate();
    }
}

CDuiString CPwdCheckUI::GetPwd(void)
{
    return m_sPwd;
}

void CPwdCheckUI::SetShowTxt(bool bShow)
{
    m_bShowTxt = bShow;
    NeedUpdate();
}

bool CPwdCheckUI::IsShowTxt(void)
{
    return m_bShowTxt;
}

DuiLib::CDuiString CPwdCheckUI::GetText(void) const
{
    return m_sTxtWeak + _T(",") + m_sTxtMiddle + _T(",") + m_sTxtStrong;
}

void CPwdCheckUI::SetText(LPCTSTR pstrText)
{
    if (NULL == pstrText || _tcslen(pstrText) == 0) { return; }

    CDuiString str(pstrText);
    int nPos1 = 0, nPos2 = 0;
    nPos1 = str.Find(_T(","), nPos1);
    m_sTxtWeak = str.Left(nPos1);
    m_sTxtWeakOrig = m_sTxtWeak;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sTxtWeak);

    nPos1 += 1;
    nPos2 = str.Find(_T(","), nPos1);
    m_sTxtMiddle = str.Mid(nPos1, nPos2 - nPos1);
    m_sTxtMiddleOrig = m_sTxtMiddle;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sTxtMiddle);

    m_sTxtStrong = str.Right(str.GetLength() - nPos2 - 1);
    m_sTxtStrongOrig = m_sTxtStrong;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sTxtStrong);
    NeedUpdate();
}

int CPwdCheckUI::CalcPwdStrongth(void)
{
    if (m_sPwd.GetLength() < 5) { return EPS_WEAK; }

    static CRegexpT<TCHAR> rgp1(_T("[0-9]+"));
    static CRegexpT<TCHAR> rgp2(_T("[a-zA-Z]+"));
    static CRegexpT<TCHAR> rgp3(_T("[^0-9a-zA-Z]+"));

    int n1 = rgp1.Match(m_sPwd).IsMatched();
    int n2 = rgp2.Match(m_sPwd).IsMatched();
    int n3 = rgp3.Match(m_sPwd).IsMatched();

    if (0 != n1 && 0 != n2 && 0 != n3) { return EPS_STRONG; }

    if ((0 != n1 && 0 != n2) || (0 != n2 && 0 != n3) || (0 != n1 && 0 != n3)) { return EPS_MIDDLE; }

    return EPS_WEAK;
}

}