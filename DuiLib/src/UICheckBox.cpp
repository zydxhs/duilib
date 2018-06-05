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
    // 复选框，group属性无效
    if (_tcscmp(pstrName, _T("group")) == 0) { DUITRACE(_T("不支持属性:group")); }
    else { COptionUI::SetAttribute(pstrName, pstrValue); }
}

}
