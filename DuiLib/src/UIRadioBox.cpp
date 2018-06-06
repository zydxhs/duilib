#include "StdAfx.h"

namespace DuiLib {

LPCTSTR CRadioBoxUI::GetClass() const
{
    return DUI_CTR_RADIOBOX;
}

LPVOID CRadioBoxUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_RADIOBOX) == 0) { return static_cast<CRadioBoxUI *>(this); }

    return COptionUI::GetInterface(pstrName);
}

}
