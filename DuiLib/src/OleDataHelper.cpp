/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file dataobjecthelper.cpp
/// \brief Ole数据对象辅助类
///
///
/// \author zhuyadong@kedacom.com
/// \date 2018-04-28
/// \note
/// -----------------------------------------------------------------------------
/// 修改记录：
/// 日  期        版本        修改人        走读人    修改内容
///
#include "objidl.h"
#include "stdafx.h"
#include "DataObject.h"
#include <string>

namespace DuiLib {


std::string DuiStr2Str(CDuiString &src)
{
#if defined(UNICODE) || defined(_UNICODE)
    int len = WideCharToMultiByte(CP_ACP, 0, src.GetData(), -1, NULL, 0, NULL, NULL);

    if (len <= 0) { return std::string(); }

    char *szBuf = new char[len + 1];

    if (NULL == szBuf) { return std::string(); }

    memset(szBuf, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, src.GetData(), -1, szBuf, len, NULL, NULL);

    std::string str(szBuf);
    delete []szBuf;
    return str;
#else
    return std::string(src.GetData());
#endif
}

std::wstring DuiStr2WStr(CDuiString &src)
{
#if defined(UNICODE) || defined(_UNICODE)
    return std::wstring(src.GetData());
#else
    int len = MultiByteToWideChar(CP_ACP, 0, src.GetData(), -1, 0, 0);

    if (len <= 0) { return std::wstring(); }

    WCHAR *szBuf = new WCHAR[len + 1];

    if (NULL == szBuf) { return std::wstring(); }

    memset(szBuf, 0, (len + 1) * 2);
    MultiByteToWideChar(CP_ACP, 0, src.GetData(), -1, szBuf, len);
    szBuf[len] = 0;

    std::wstring str(szBuf);
    delete []szBuf;
    return str;
#endif
}

CDuiString WStr2DuiStr(LPCWSTR lpcwstr)
{
#if defined(UNICODE) || defined(_UNICODE)
    return CDuiString(lpcwstr);
#else
    int len = WideCharToMultiByte(CP_ACP, 0, lpcwstr, -1, NULL, 0, NULL, NULL);

    if (len <= 0) { return CDuiString(); }

    char *szBuf = new char[len + 1];

    if (NULL == szBuf) { return CDuiString(); }

    memset(szBuf, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, lpcwstr, -1, szBuf, len, NULL, NULL);

    CDuiString str(szBuf);
    delete[ ]szBuf;
    return str;
#endif // UNICODE
}

CDuiString Str2DuiStr(LPCSTR lpcstr)
{
#if defined(UNICODE) || defined(_UNICODE)
    int len = MultiByteToWideChar(CP_ACP, 0, lpcstr, -1, 0, 0);

    if (len <= 0) { return CDuiString(); }

    WCHAR *szBuf = new WCHAR[len + 1];

    if (NULL == szBuf) { return CDuiString(); }

    memset(szBuf, 0, (len + 1) * 2);
    MultiByteToWideChar(CP_ACP, 0, lpcstr, -1, szBuf, len);
    szBuf[len] = 0;

    CDuiString str(szBuf);
    delete[ ]szBuf;
    return str;
#else
    return CDuiString(lpcstr);
#endif // UNICODE
}

HGLOBAL StringToHandle(LPCWSTR lpcstrText, int nTextLen)
{
    // if text length is -1 then treat as a nul-terminated string
    if (nTextLen == -1) { nTextLen = wcslen(lpcstrText) + 1; }

    nTextLen *= sizeof(wchar_t);
    // allocate and lock a global memory buffer. Make it fixed
    // data so we don't have to use GlobalLock
    void *ptr = (void *)GlobalAlloc(GPTR, nTextLen);

    // copy the string into the buffer
    memcpy(ptr, lpcstrText, nTextLen);
    return ptr;
}

HBITMAP CopyHBitmap(HDC hDC, HBITMAP hBmpSrc)
{
    //HDC hDCDst = hDC;
    HDC hDCSrc = CreateCompatibleDC(NULL);
    HDC hDCDst = CreateCompatibleDC(NULL);
    BITMAP bm = { 0 };
    ::GetObject(hBmpSrc, sizeof(bm), &bm);

    HBITMAP hBmpDst = ::CreateCompatibleBitmap(hDC, bm.bmWidth, bm.bmHeight);
    HBITMAP hBmpSrcOld = (HBITMAP)::SelectObject(hDCSrc, hBmpSrc);
    ::SelectObject(hDCDst, hBmpDst);
    BitBlt(hDCDst, 0, 0, bm.bmWidth, bm.bmHeight, hDCSrc, 0, 0, SRCCOPY);

    ::SelectObject(hDCSrc, hBmpSrcOld);
    ::DeleteObject(hDCDst);
    ::DeleteObject(hDCSrc);
    return hBmpDst;
}

//////////////////////////////////////////////////////////////////////////
COleDataHelper::COleDataHelper(void)
    : m_pDataObj(NULL)
    , m_bAutoRelease(false)
    , m_pEnumFmt(NULL)
    , m_dwKeyState(0)
    , m_dwEffect(DROPEFFECT_NONE)
{
}


COleDataHelper::~COleDataHelper(void)
{
    if (m_bAutoRelease && NULL != m_pDataObj) { m_pDataObj->Release(); }

    if (NULL != m_pEnumFmt) { m_pEnumFmt->Release(); }
}

bool COleDataHelper::SetText(CDuiString sText)
{
    HGLOBAL hGlobal = StringToHandle(DuiStr2WStr(sText).c_str());
    bool bRet = SetData(hGlobal, ETYMED_HGLOBAL, ECF_UNICODETEXT, EDVASPECT_CONTENT, TRUE);

    if (!bRet) { GlobalFree(hGlobal); }

    return bRet;
}

bool COleDataHelper::SetBitmap(HBITMAP hBmp)
{
    return SetData(hBmp, ETYMED_GDI, ECF_BITMAP, EDVASPECT_CONTENT, TRUE);
}

bool COleDataHelper::SetCustomData(void *pData, DWORD dwLen, WORD wCF)
{
    if (wCF >= ECF_PRIVATEFIRST && wCF <= ECF_PRIVATELAST)
    {
        void *ptr = (void *)GlobalAlloc(GPTR, dwLen);
        memcpy(ptr, pData, dwLen);
        bool bRet = SetData(ptr, ETYMED_HGLOBAL, wCF, EDVASPECT_CONTENT, TRUE);

        if (!bRet) { GlobalFree(ptr); }

        return bRet;
    }

    return false;
}

bool COleDataHelper::SetCustomGDI(HGDIOBJ hGDI, WORD wCF)
{
    if (wCF >= ECF_GDIOBJFIRST && wCF <= ECF_GDIOBJLAST)
    {
        bool bRet = SetData(hGDI, ETYMED_GDI, wCF, EDVASPECT_CONTENT, TRUE);
        return bRet;
    }

    return false;
}

bool COleDataHelper::SetInt(int nVal)
{
    return SetCustomData(&nVal, sizeof(nVal), ECF_INT32);
}

bool COleDataHelper::SetInt64(long long nnVal)
{
    return SetCustomData(&nnVal, sizeof(long long), ECF_INT64);
}

CDuiString COleDataHelper::GetText(void)
{
    CDuiString strRet;
    FORMATETC fmt = { CF_UNICODETEXT, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm = { TYMED_HGLOBAL, { 0 }, 0 };

    if (S_OK == m_pDataObj->GetData(&fmt, &stm))
    {
        LPCWSTR pBuff = (LPCWSTR)stm.hGlobal;
        strRet = WStr2DuiStr(pBuff);
        GlobalFree(stm.hGlobal);
    }
    else
    {
        fmt.cfFormat = CF_TEXT;

        if (S_OK == m_pDataObj->GetData(&fmt, &stm))
        {
            LPCSTR pBuff = (LPCSTR)stm.hGlobal;
            strRet = Str2DuiStr(pBuff);
            GlobalFree(stm.hGlobal);
        }
    }

    return strRet;
}

HBITMAP COleDataHelper::GetBitmap(void)
{
    HBITMAP hBMP = NULL;
    FORMATETC fmt = { CF_BITMAP, 0, DVASPECT_CONTENT, -1, TYMED_GDI};
    STGMEDIUM stm = { TYMED_GDI, { 0 }, 0 };

    if (S_OK == m_pDataObj->GetData(&fmt, &stm))
    {
        hBMP = stm.hBitmap;
    }

    return hBMP;
}

CDuiPtrArray COleDataHelper::GetFileList(void)
{
    CDuiPtrArray ptrArray(0);
    FORMATETC fmt = { ECF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm = { TYMED_HGLOBAL, { 0 }, 0 };

    if (S_OK != m_pDataObj->GetData(&fmt, &stm)) { return ptrArray; }

    HDROP hDrop = (HDROP)stm.hGlobal;
    int nFilesNum = DragQueryFile(hDrop, -1, NULL, 0);

    for (int i = 0; i < nFilesNum; ++i)
    {
        int cch = DragQueryFile(hDrop, i, NULL, 0);
        LPTSTR szFileSrc = (LPTSTR)GlobalAlloc(GPTR, (cch + 1) * 2);
        LPTSTR szFileDst = (LPTSTR)malloc((cch + 1) * 2);
        DragQueryFile(hDrop, i, szFileSrc, cch + 1);
        memcpy(szFileDst, szFileSrc, (cch + 1) * sizeof(TCHAR));
        ptrArray.Add(szFileDst);
        GlobalFree((HGLOBAL)szFileSrc);
    }

    GlobalFree(stm.hGlobal);
    return ptrArray;
}

void *COleDataHelper::GetCustomData(WORD wCF, DWORD &dwLen)
{
    if (wCF < ECF_PRIVATEFIRST || wCF > ECF_PRIVATELAST) { return NULL; }

    void *pData = NULL;
    FORMATETC fmt = { wCF, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stm = { TYMED_HGLOBAL, { 0 }, 0 };

    if (S_OK == m_pDataObj->GetData(&fmt, &stm))
    {
        dwLen = GlobalSize(stm.hGlobal);
        pData = malloc(dwLen);
        memcpy(pData, stm.hGlobal, dwLen);
    }

    GlobalFree(stm.hGlobal);
    return pData;
}

// TODO 验证是否正确
HGDIOBJ COleDataHelper::GetCustomGDI(WORD wCF)
{
    if (wCF < ECF_GDIOBJFIRST || wCF > ECF_GDIOBJLAST) { return NULL; }

    HGDIOBJ hGdiObj = NULL;
    FORMATETC fmt = { wCF, 0, DVASPECT_CONTENT, -1, TYMED_GDI };
    STGMEDIUM stm = { TYMED_GDI, { 0 }, 0 };

    if (S_OK == m_pDataObj->GetData(&fmt, &stm))
    {
        hGdiObj = stm.hBitmap;
    }

    return hGdiObj;
}

int COleDataHelper::GetInt(void)
{
    DWORD dwLen = sizeof(int);
    void *pData = GetCustomData(ECF_INT32, dwLen);
    int nRet = 0;

    if (NULL != pData)
    {
        nRet = *(int *)pData;   // 2020-08-19 解决总是返回 0 的问题
        free(pData);
    }

    return nRet;
}

long long COleDataHelper::GetInt64(void)
{
    DWORD dwLen = sizeof(int);
    void *pData = GetCustomData(ECF_INT64, dwLen);
    long long nRet = 0;

    if (NULL != pData)
    {
        nRet = *(long long *)pData;
        free(pData);
    }

    return nRet;
}

bool COleDataHelper::SetData(void *pData, int nTM, WORD wCF, WORD wAspect, BOOL bRelease)
{
    FORMATETC fmt = { wCF, NULL, wAspect, -1, nTM };
    STGMEDIUM stm = { nTM, { NULL }, NULL };

    switch (stm.tymed)
    {
    case ETYMED_HGLOBAL:    stm.hGlobal = (HGLOBAL)pData;              break;

    case ETYMED_FILE:       stm.lpszFileName = (LPOLESTR)pData;        break;

    case ETYMED_ISTREAM:    stm.pstm = (IStream *)pData;               break;

    case ETYMED_ISTORAGE:   stm.pstg = (IStorage *)pData;              break;

    case ETYMED_GDI:        stm.hBitmap = (HBITMAP)pData;              break;

    case ETYMED_MFPICT:     stm.hMetaFilePict = (HMETAFILEPICT)pData;  break;

    case ETYMED_ENHMF:      stm.hEnhMetaFile = (HENHMETAFILE)pData;    break;

    default:                                                            return false;
    }

    return S_OK == m_pDataObj->SetData(&fmt, &stm, bRelease);
}

void *COleDataHelper::GetData(WORD wCF, int nTM, WORD wAspect)
{
    void *pData = NULL;
    FORMATETC fmt = { wCF, 0, wAspect, -1, nTM};
    STGMEDIUM stm = { nTM, { 0 }, 0 };

    if (S_OK == m_pDataObj->GetData(&fmt, &stm))
    {
        switch (nTM)
        {
        case ETYMED_HGLOBAL:    pData = stm.hGlobal;           break;

        case ETYMED_FILE:       pData = stm.lpszFileName;       break;

        case ETYMED_ISTREAM:    pData = stm.pstm;               break;

        case ETYMED_ISTORAGE:   pData = stm.pstg;               break;

        case ETYMED_GDI:        pData = stm.hBitmap;            break;

        case ETYMED_MFPICT:     pData = stm.hMetaFilePict;      break;

        case ETYMED_ENHMF:      pData = stm.hEnhMetaFile;       break;

        default:                                                 break;
        }
    }

    return pData;
}

IDataObject *COleDataHelper::CreateDataObject(void)
{
    m_bAutoRelease = true;
    m_pDataObj = new CDataObject;
    return m_pDataObj;
}

bool COleDataHelper::CreateEnumFormatEtc(void)
{
    if (NULL != m_pEnumFmt) { m_pEnumFmt->Release(); }

    HRESULT hr = m_pDataObj->EnumFormatEtc(DATADIR_GET, &m_pEnumFmt);
    return S_OK == hr;
}

bool COleDataHelper::GetNext(WORD &wCF, WORD &wAspect, int &nTM)
{
    if (NULL == m_pEnumFmt)
    {
        CreateEnumFormatEtc();

        if (NULL == m_pEnumFmt) { return false; }
    }

    FORMATETC fmt = { 0 };
    ULONG celtFetched = 0;
    HRESULT hr = m_pEnumFmt->Next(1, &fmt, &celtFetched);
    wCF = fmt.cfFormat;
    wAspect = (WORD)fmt.dwAspect;
    nTM = fmt.tymed;
    CoTaskMemFree(fmt.ptd);
    return S_OK == hr;
}

bool COleDataHelper::Reset(void)
{
    if (NULL == m_pEnumFmt)
    {
        CreateEnumFormatEtc();

        if (NULL == m_pEnumFmt) { return false; }
    }

    m_pEnumFmt->Reset();
    return true;
}

bool COleDataHelper::HasText(void)
{
    Reset();
    WORD wCF = 0;
    WORD wAspect = 0;
    int nTM = 0;

    while (GetNext(wCF, wAspect, nTM))
    {
        if ((ECF_TEXT == wCF || ECF_UNICODETEXT == wCF) && ETYMED_HGLOBAL == nTM) { return true; }
    }

    return false;
}

bool COleDataHelper::HasBitmap(void)
{
    Reset();
    WORD wCF = 0;
    WORD wAspect = 0;
    int nTM = 0;

    while (GetNext(wCF, wAspect, nTM))
    {
        if (ECF_BITMAP == wCF && ETYMED_GDI == nTM) { return true; }
    }

    return false;
}

bool COleDataHelper::HasFileList(void)
{
    Reset();
    WORD wCF = 0;
    WORD wAspect = 0;
    int nTM = 0;

    while (GetNext(wCF, wAspect, nTM))
    {
        if (ECF_HDROP == wCF && ETYMED_HGLOBAL == nTM) { return true; }
    }

    return false;
}

bool COleDataHelper::HasCustomData(WORD wCF)
{
    Reset();
    WORD wCFTmp = 0;
    WORD wAspect = 0;
    int nTM = 0;
    bool bRet = false;

    while (GetNext(wCFTmp, wAspect, nTM))
    {
        if ((ECF_PRIVATEFIRST <= wCFTmp && ECF_PRIVATELAST >= wCFTmp) && ETYMED_HGLOBAL == nTM)
        {
            // 1. 只要有自定义数据，就返回 true
            // 2. 有自定义数据，并且与 wCF 指定的类型一致，才返回 true
            if (0 == wCF || wCFTmp == wCF) { bRet = true; break; }
        }
    }

    return bRet;
}

bool COleDataHelper::HasCustomGDI(void)
{
    Reset();
    WORD wCF = 0;
    WORD wAspect = 0;
    int nTM = 0;

    while (GetNext(wCF, wAspect, nTM))
    {
        if ((ECF_GDIOBJFIRST <= wCF && ECF_GDIOBJLAST >= wCF) && ETYMED_GDI == nTM) { return true; }
    }

    return false;
}

bool COleDataHelper::HasInt(void)
{
    return HasCustomData(ECF_INT32);
}

bool COleDataHelper::HasInt64(void)
{
    return HasCustomData(ECF_INT64);
}

}
