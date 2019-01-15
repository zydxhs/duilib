#include "StdAfx.h"

namespace DuiLib {

// bRemoveAll == true 时删除字符串中间的 空格/TAB
CDuiString Trim(LPCTSTR pstrVal, bool bRemoveAll = true)
{
    if (NULL == pstrVal) { return CDuiString(); }

    CDuiString str;

    if (bRemoveAll)
    {
        for (; *pstrVal != _T('\0'); pstrVal = ::CharNext(pstrVal))
        {
            if (*pstrVal != _T(' ') && *pstrVal != _T('\t'))
            {
                str += *pstrVal;
            }
        }
    }
    else
    {
        while (*pstrVal == _T(' ') || *pstrVal == _T('\t')) { pstrVal = ::CharNext(pstrVal); }

        int nLen = _tcslen(pstrVal) - 1;

        while (*(pstrVal + nLen) == _T(' ') || *(pstrVal + nLen) == _T('\t')) { nLen -= 1; }

        str.Assign(pstrVal, nLen + 1);
    }

    return str;
}

bool ParseBool(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return false; }

    CDuiString strVal = Trim(pstrVal);
    return (0 == strVal.Compare(_T("true"))) ? true : false;
}

BYTE ParseByte(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return false; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();

    if (*pstrVal == _T('0'))
    {
        pstrVal = ::CharNext(pstrVal);

        if (*pstrVal == _T('x') || *pstrVal == _T('X'))
        {
            pstrVal = ::CharNext(pstrVal);
            return (BYTE)_tcstoul(pstrVal, NULL, 16);
        }
        else
        {
            return (BYTE)_ttoi(pstrVal);
        }
    }
    else
    {
        return (BYTE)_ttoi(pstrVal);
    }
}

WORD ParseWord(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return false; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();

    if (*pstrVal == _T('0'))
    {
        pstrVal = ::CharNext(pstrVal);

        if (*pstrVal == _T('x') || *pstrVal == _T('X'))
        {
            pstrVal = ::CharNext(pstrVal);
            return (WORD)_tcstoul(pstrVal, NULL, 16);
        }
        else
        {
            return (WORD)_ttoi(pstrVal);
        }
    }
    else
    {
        return (WORD)_ttoi(pstrVal);
    }
}

INT ParseInt(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return false; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();

    if (*pstrVal == _T('0'))
    {
        pstrVal = ::CharNext(pstrVal);

        if (*pstrVal == _T('x') || *pstrVal == _T('X'))
        {
            pstrVal = ::CharNext(pstrVal);
            return (INT)_tcstol(pstrVal, NULL, 16);
        }
        else
        {
            return (INT)_ttoi(pstrVal);
        }
    }
    else
    {
        return (INT)_ttoi(pstrVal);
    }
}

DWORD ParseDWord(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return false; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();

    if (*pstrVal == _T('0'))
    {
        pstrVal = ::CharNext(pstrVal);

        if (*pstrVal == _T('x') || *pstrVal == _T('X'))
        {
            pstrVal = ::CharNext(pstrVal);
            return (DWORD)_tcstoul(pstrVal, NULL, 16);
        }
        else
        {
            return (DWORD)_ttoi(pstrVal);
        }
    }
    else
    {
        return (DWORD)_ttoi(pstrVal);
    }
}

DWORD_PTR ParseDWordPtr(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return false; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();

    if (*pstrVal == _T('0'))
    {
        pstrVal = ::CharNext(pstrVal);

        if (*pstrVal == _T('x') || *pstrVal == _T('X'))
        {
            pstrVal = ::CharNext(pstrVal);
            return (DWORD_PTR)_tcstoul(pstrVal, NULL, 16);
        }
        else
        {
            return (DWORD_PTR)_ttoi64(pstrVal);
        }
    }
    else
    {
        return (DWORD_PTR)_ttoi64(pstrVal);
    }
}

float ParseFloat(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return false; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();
    return (float)_tstof(pstrVal);
}

CDuiString ParseString(LPCTSTR pstrVal)
{
    return Trim(pstrVal, false);
}

SIZE ParseSize(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return SIZE { 0, 0 }; }

    CDuiString str = Trim(pstrVal, true);
    pstrVal = str.GetData();
    LPTSTR pstr = NULL;
    SIZE sz = { 0, 0 };
    sz.cx = _tcstol(pstrVal, &pstr, 10);

    if (*pstr != _T('\0'))
    {
        pstrVal = ::CharNext(pstr);
        sz.cy = _tcstol(pstrVal, &pstr, 10);
    }
    else
    {
        sz.cy = sz.cx;
    }

    return sz;
}

RECT ParseRect(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return RECT { 0, 0, 0, 0 }; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();
    LPTSTR pstr = NULL;
    RECT rt = { 0, 0, 0, 0 };
    rt.left = _tcstol(pstrVal, &pstr, 10);

    if (*pstr != _T('\0'))
    {
        pstrVal = ::CharNext(pstr);
        rt.top = _tcstol(pstrVal, &pstr, 10);

        if (*pstr != _T('\0'))
        {
            pstrVal = ::CharNext(pstr);
            rt.right = _tcstol(pstrVal, &pstr, 10);

            if (*pstr != _T('\0'))
            {
                pstrVal = ::CharNext(pstr);
                rt.bottom = _tcstol(pstrVal, &pstr, 10);
            }
            else
            {
                rt.bottom = rt.top;
            }
        }
        else
        {
            rt.right = rt.left;
            rt.bottom = rt.top;
        }
    }
    else
    {
        rt.top = rt.right = rt.bottom = rt.left;
    }

    return rt;
}

DuiLib::TPercentInfo ParseRectPercent(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return TPercentInfo { 0.0, 0.0, 0.0, 0.0 }; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();
    LPTSTR pstr = NULL;
    TPercentInfo rt = { 0.0, 0.0, 0.0, 0.0 };
    rt.left = _tcstod(pstrVal, &pstr);

    if (*pstr != _T('\0'))
    {
        pstrVal = ::CharNext(pstr);
        rt.top = _tcstod(pstrVal, &pstr);

        if (*pstr != _T('\0'))
        {
            pstrVal = ::CharNext(pstr);
            rt.right = _tcstod(pstrVal, &pstr);

            if (*pstr != _T('\0'))
            {
                pstrVal = ::CharNext(pstr);
                rt.bottom = _tcstod(pstrVal, &pstr);
            }
            else
            {
                rt.bottom = rt.top;
            }
        }
        else
        {
            rt.right = rt.left;
            rt.bottom = rt.top;
        }
    }
    else
    {
        rt.top = rt.right = rt.bottom = rt.left;
    }

    return rt;
}

DWORD ParseColor(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return 0; }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();

    if (*pstrVal == _T('#')) { pstrVal = ::CharNext(pstrVal); }

    LPTSTR pstr = NULL;
    DWORD clrColor = _tcstoul(pstrVal, &pstr, 16);
    return clrColor;
}

TEffectProperty ParseEffect(LPCTSTR pstrVal)
{
    if (NULL == pstrVal) { return TEffectProperty(); }

    CDuiString str = Trim(pstrVal);
    pstrVal = str.GetData();
    LPTSTR pstr = NULL;
    TEffectProperty tEP;
    tEP.m_byId = (BYTE)_tcstol(pstrVal, &pstr, 10);

    if (*pstr != _T('\0'))
    {
        pstrVal = ::CharNext(pstr);
        tEP.m_byFrequency = (WORD)_tcstol(pstrVal, &pstr, 10);

        if (*pstr != _T('\0'))
        {
            pstrVal = ::CharNext(pstr);
            tEP.m_byDirection = (_tcsncmp(pstrVal, _T("true"), 4) == 0) ? true : false;
            pstrVal = _tcschr(pstrVal, _T(','));

            if (NULL != pstrVal && *pstrVal != _T('\0'))
            {
                pstrVal = ::CharNext(pstrVal);
                tEP.m_byLoop = (_tcsncmp(pstrVal, _T("true"), 4) == 0) ? true : false;
            }
        }
    }

    return tEP;
}

}
