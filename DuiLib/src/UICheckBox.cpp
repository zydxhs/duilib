#include "stdafx.h"

namespace DuiLib {
LPCTSTR CCheckBoxUI::GetClass() const
{
    return DUI_CTR_CHECKBOX;
}

LPVOID CCheckBoxUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_CHECKBOX) == 0) { return static_cast<CCheckBoxUI *>(this); }

    return COptionUI::GetInterface(pstrName);
}

void CCheckBoxUI::SetCheck(bool bCheck, bool bTriggerEvent)
{
    Selected(bCheck, bTriggerEvent);
}

bool  CCheckBoxUI::GetCheck() const
{
    return IsSelected();
}

void CCheckBoxUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("seltext")) == 0) { SetSelText(pstrValue); }   // 2019-01-10 zhuyadong 不做任何处理
    // 复选框，group属性无效
    else if (_tcscmp(pstrName, _T("group")) == 0) { DUITRACE(_T("不支持属性:group")); }
    else { COptionUI::SetAttribute(pstrName, pstrValue); }
}

void CCheckBoxUI::PaintText(HDC hDC)
{
    if (!m_sSelText.IsEmpty() && IsSelected())
    {
        CDuiString sText = m_sText;
        m_sText = m_sSelText;
        COptionUI::PaintText(hDC);
        m_sText = sText;
    }
    else
    {
        COptionUI::PaintText(hDC);
    }
}

void CCheckBoxUI::ReloadText(void)
{
    COptionUI::ReloadText();
    SetSelText(m_sSelTextOrig);
}

void CCheckBoxUI::SetSelText(LPCTSTR pstrValue)
{
    if (m_sSelText == pstrValue) { return; }

    m_sSelText = pstrValue;
    m_sSelTextOrig = m_sSelText;
    CPaintManagerUI::ProcessMultiLanguageTokens(m_sSelText);
    Invalidate();
}

CDuiString CCheckBoxUI::GetSelText(void)
{
    return m_sSelText;
}

}
