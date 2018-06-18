/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file dataobject.cpp
/// \brief 数据对象
///
///
/// \author zhuyadong@kedacom.com
/// \date 2018-04-27
/// \note
/// -----------------------------------------------------------------------------
/// 修改记录：
/// 日  期        版本        修改人        走读人    修改内容
///
#include "stdafx.h"
#include <ObjIdl.h>
#include "Shlwapi.h"
#include "DataObject.h"

namespace DuiLib {

HRESULT CreateEnumFormatEtc(UINT nNumFormats, FORMATETC *pFormatEtc, IEnumFORMATETC **ppEnumFormatEtc)
{
    if (nNumFormats == 0 || pFormatEtc == 0 || ppEnumFormatEtc == 0) { return E_INVALIDARG; }

    *ppEnumFormatEtc = new CEnumFormatEtc(pFormatEtc, nNumFormats);

    return (*ppEnumFormatEtc) ? S_OK : E_OUTOFMEMORY;
}

HGLOBAL DupMem(HGLOBAL hMem)
{
    // lock the source memory object
    DWORD   len = GlobalSize(hMem);
    PVOID   source = GlobalLock(hMem);

    // create a fixed "global" block - i.e. just
    // a regular lump of our process heap
    PVOID   dest = GlobalAlloc(GMEM_FIXED, len);

    memcpy(dest, source, len);
    GlobalUnlock(hMem);
    return dest;
}

//
//  Helper function to perform a "deep" copy of a FORMATETC
//
static void DeepCopyFormatEtc(FORMATETC *dest, FORMATETC *source)
{
    // copy the source FORMATETC into dest
    *dest = *source;

    if (source->ptd)
    {
        // allocate memory for the DVTARGETDEVICE if necessary
        dest->ptd = (DVTARGETDEVICE *)CoTaskMemAlloc(sizeof(DVTARGETDEVICE));

        // copy the contents of the source DVTARGETDEVICE into dest->ptd
        *(dest->ptd) = *(source->ptd);
    }
}

//////////////////////////////////////////////////////////////////////////
#define DATA_ELEMENTS_COUNT 2

CDataObject::CDataObject(void)
    : m_lRefCount(1)
    , m_cFmtEtc(sizeof(FORMATETC), DATA_ELEMENTS_COUNT)
    , m_cStgMed(sizeof(STGMEDIUM), DATA_ELEMENTS_COUNT)
{
    m_lRefCount = 1;
}

CDataObject::~CDataObject()
{
    for (int i = 0; i < m_cStgMed.GetSize(); ++i)
    {
        STGMEDIUM *pSTM = (STGMEDIUM *)m_cStgMed.GetAt(i);
        ReleaseStgMedium(pSTM);
    }

    m_cFmtEtc.Empty();
    m_cStgMed.Empty();
}

HRESULT __stdcall CDataObject::QueryInterface(REFIID riid, void **ppvObject)
{
    //if (iid == IID_IDataObject || iid == IID_IUnknown)
    //{
    //    AddRef();
    //    *ppvObject = this;
    //    return S_OK;
    //}
    //else
    //{
    //    *ppvObject = 0;
    //    return E_NOINTERFACE;
    //}
    static const QITAB qit[ ] =
    {
        QITABENT(CDataObject, IDataObject), { 0 },
    };
    return QISearch(this, qit, riid, ppvObject);
}

ULONG __stdcall CDataObject::AddRef(void)
{
    return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CDataObject::Release(void)
{
    LONG count = InterlockedDecrement(&m_lRefCount);

    if (count == 0) { delete this; }

    return count;
}

HRESULT __stdcall CDataObject::GetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
    int idx = 0;

    //
    // try to match the requested FORMATETC with one of our supported formats
    //
    if ((idx = LookupFormatEtc(pFormatEtc)) == -1) { return DV_E_FORMATETC; }

    //
    // found a match! transfer the data into the supplied storage-medium
    //
    FORMATETC *pFMT = (FORMATETC *)m_cFmtEtc.GetAt(idx);
    pMedium->tymed = pFMT->tymed;
    pMedium->pUnkForRelease = 0;
    STGMEDIUM *pSTM = (STGMEDIUM *)m_cStgMed.GetAt(idx);

    return CopyMedium(pFMT, pSTM, pMedium);
}

HRESULT __stdcall CDataObject::GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pMedium)
{
    // GetDataHere is only required for IStream and IStorage mediums
    // It is an error to call GetDataHere for things like HGLOBAL and other clipboard formats
    //
    //  OleFlushClipboard
    //
    return E_NOTIMPL;
}

// Called to see if the IDataObject supports the specified format of data
HRESULT __stdcall CDataObject::QueryGetData(FORMATETC *pFormatEtc)
{
    if (NULL == pFormatEtc) { return E_INVALIDARG; }

    if (!(DVASPECT_CONTENT & pFormatEtc->dwAspect)) { return DV_E_DVASPECT; }

    HRESULT hr = DV_E_TYMED;

    for (int i = 0; i < m_cFmtEtc.GetSize(); ++i)
    {
        FORMATETC *pFMT = (FORMATETC *)m_cFmtEtc.GetAt(i);

        if (pFMT->cfFormat == pFormatEtc->cfFormat)
        {
            if (pFMT->tymed & pFormatEtc->tymed) { return S_OK; }
            else { hr = DV_E_TYMED; }
        }
        else
        {
            hr = DV_E_CLIPFORMAT;
        }
    }

    return hr;
}

HRESULT __stdcall CDataObject::GetCanonicalFormatEtc(FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut)
{
    // Apparently we have to set this field to NULL even though we don't do anything else
    pFormatEtcOut->ptd = NULL;
    return E_NOTIMPL;
}

HRESULT __stdcall CDataObject::SetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease)
{
    if (NULL == pFormatEtc || NULL == pMedium) { return E_INVALIDARG; }

    if (pFormatEtc->tymed != pMedium->tymed) { return E_FAIL; }

    int idx = 0;

    if ((idx = LookupFormatEtc(pFormatEtc)) != -1) { return DV_E_FORMATETC; }

    m_cFmtEtc.Add(pFormatEtc);
    m_cStgMed.Add(pMedium);

    if (!fRelease)
    {
        STGMEDIUM *pSTM = (STGMEDIUM *)m_cStgMed.GetAt(m_cStgMed.GetSize() - 1);
        CopyMedium(pFormatEtc, pMedium, pSTM);
    }

    //if (fRelease) { ReleaseStgMedium(pMedium); }
    DUITRACE(_T("CDataObject::SetData size=%d, CF=%d"), m_cStgMed.GetSize(), pFormatEtc->cfFormat);
    return S_OK;
}

HRESULT __stdcall CDataObject::EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc)
{
    if (dwDirection == DATADIR_GET)
    {
        // for Win2k+ you can use the SHCreateStdEnumFmtEtc API call, however
        // to support all Windows platforms we need to implement IEnumFormatEtc ourselves.
        return CreateEnumFormatEtc(m_cFmtEtc.GetSize(), (FORMATETC *)m_cFmtEtc.GetData(), ppEnumFormatEtc);
    }
    else
    {
        // the direction specified is not support for drag+drop
        return E_NOTIMPL;
    }
}

HRESULT __stdcall CDataObject::DAdvise(FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink,
                                       DWORD *pdwConnection)
{
    return E_NOTIMPL;
}

HRESULT __stdcall CDataObject::DUnadvise(DWORD dwConnection)
{
    return E_NOTIMPL;
}

HRESULT __stdcall CDataObject::EnumDAdvise(IEnumSTATDATA **ppEnumAdvise)
{
    return E_NOTIMPL;
}

int CDataObject::LookupFormatEtc(FORMATETC *pFormatEtc)
{
    for (int i = 0; i < m_cFmtEtc.GetSize(); i++)
    {
        FORMATETC *pFMT = (FORMATETC *)m_cFmtEtc.GetAt(i);

        if ((pFMT->tymed & pFormatEtc->tymed) &&
            pFMT->cfFormat == pFormatEtc->cfFormat &&
            pFMT->dwAspect == pFormatEtc->dwAspect)
        {
            return i;
        }
    }

    return -1;
}

HRESULT CDataObject::CopyMedium(FORMATETC *fmt, STGMEDIUM *stm, STGMEDIUM *dst)
{
    if (NULL == dst || NULL == stm || NULL == fmt) { return E_INVALIDARG; }

    switch (stm->tymed)
    {
    case TYMED_HGLOBAL:
        dst->hGlobal = (HGLOBAL)OleDuplicateData(stm->hGlobal, fmt->cfFormat, GPTR);
        break;

    // OleDuplicateData 仅支持DDB，不支持DIB。即 CreateDIBSection 创建的 HBITMAP 无法复制。
    // 因此 这里不复制位图，由用户复制
    case TYMED_GDI:
        //dst->hBitmap = (HBITMAP)OleDuplicateData(stm->hBitmap, fmt->cfFormat, NULL);
        dst->hBitmap = stm->hBitmap;
        break;

    case TYMED_MFPICT:
        dst->hMetaFilePict = (HMETAFILEPICT)OleDuplicateData(stm->hMetaFilePict, fmt->cfFormat, NULL);
        break;

    case TYMED_ENHMF:
        dst->hEnhMetaFile = (HENHMETAFILE)OleDuplicateData(stm->hEnhMetaFile, fmt->cfFormat, NULL);
        break;

    case TYMED_FILE:
        dst->lpszFileName = (LPOLESTR)OleDuplicateData(stm->lpszFileName, fmt->cfFormat, NULL);
        break;

    case TYMED_ISTREAM:
        dst->pstm = stm->pstm;
        stm->pstm->AddRef();
        break;

    case TYMED_ISTORAGE:
        dst->pstg = stm->pstg;
        stm->pstg->AddRef();
        break;

    case TYMED_NULL:
    default:
        break;
    }

    dst->tymed = stm->tymed;
    dst->pUnkForRelease = NULL;

    if (stm->pUnkForRelease != NULL)
    {
        dst->pUnkForRelease = stm->pUnkForRelease;
        stm->pUnkForRelease->AddRef();
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////

CEnumFormatEtc::CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats)
{
    m_lRefCount = 1;
    m_nIndex = 0;
    m_nNumFormats = nNumFormats;
    m_pFormatEtc = new FORMATETC[nNumFormats];

    // copy the FORMATETC structures
    for (int i = 0; i < nNumFormats; i++)
    {
        DeepCopyFormatEtc(&m_pFormatEtc[i], &pFormatEtc[i]);
    }
}

CEnumFormatEtc::~CEnumFormatEtc()
{
    if (m_pFormatEtc)
    {
        for (ULONG i = 0; i < m_nNumFormats; i++)
        {
            if (m_pFormatEtc[i].ptd) { CoTaskMemFree(m_pFormatEtc[i].ptd); }
        }

        delete[ ] m_pFormatEtc;
    }
}

HRESULT __stdcall CEnumFormatEtc::QueryInterface(REFIID iid, void **ppvObject)
{
    if (iid == IID_IEnumFORMATETC || iid == IID_IUnknown)
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
}

ULONG __stdcall CEnumFormatEtc::AddRef(void)
{
    return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CEnumFormatEtc::Release(void)
{
    LONG count = InterlockedDecrement(&m_lRefCount);

    if (count == 0) { delete this; }

    return count;
}

HRESULT __stdcall CEnumFormatEtc::Next(ULONG celt, FORMATETC *pFormatEtc, ULONG *pceltFetched)
{
    ULONG copied = 0;

    // validate arguments
    if (celt == 0 || pFormatEtc == 0) { return E_INVALIDARG; }

    // copy FORMATETC structures into caller's buffer
    while (m_nIndex < m_nNumFormats && copied < celt)
    {
        DeepCopyFormatEtc(&pFormatEtc[copied], &m_pFormatEtc[m_nIndex]);
        copied++;
        m_nIndex++;
    }

    // store result
    if (pceltFetched != 0) { *pceltFetched = copied; }

    // did we copy all that was requested?
    return (copied == celt) ? S_OK : S_FALSE;
}

HRESULT __stdcall CEnumFormatEtc::Skip(ULONG celt)
{
    m_nIndex += celt;
    return (m_nIndex <= m_nNumFormats) ? S_OK : S_FALSE;
}

HRESULT __stdcall CEnumFormatEtc::Reset(void)
{
    m_nIndex = 0;
    return S_OK;
}

HRESULT __stdcall CEnumFormatEtc::Clone(IEnumFORMATETC **ppEnumFormatEtc)
{
    HRESULT hResult;

    // make a duplicate enumerator
    hResult = CreateEnumFormatEtc(m_nNumFormats, m_pFormatEtc, ppEnumFormatEtc);

    if (hResult == S_OK)
    {
        // manually set the index state
        ((CEnumFormatEtc *)*ppEnumFormatEtc)->m_nIndex = m_nIndex;
    }

    return hResult;
}

}
