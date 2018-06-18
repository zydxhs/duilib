/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file dataobject.h
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
#pragma once

namespace DuiLib {

HRESULT CreateEnumFormatEtc(UINT nNumFormats, FORMATETC *pFormatEtc, IEnumFORMATETC **ppEnumFormatEtc);

class CDataObject : public IDataObject
{
public:
    CDataObject(void);
    ~CDataObject(void);

    HRESULT __stdcall QueryInterface(REFIID riid, void **ppvObject);
    ULONG   __stdcall AddRef(void);
    ULONG   __stdcall Release(void);

    virtual HRESULT __stdcall GetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium);
    virtual HRESULT __stdcall GetDataHere(FORMATETC *pFormatEtc, STGMEDIUM *pMedium);
    virtual HRESULT __stdcall QueryGetData(FORMATETC *pFormatEtc);
    virtual HRESULT __stdcall GetCanonicalFormatEtc(FORMATETC *pFormatEct, FORMATETC *pFormatEtcOut);
    virtual HRESULT __stdcall SetData(FORMATETC *pFormatEtc, STGMEDIUM *pMedium, BOOL fRelease);
    virtual HRESULT __stdcall EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC **ppEnumFormatEtc);
    virtual HRESULT __stdcall DAdvise(FORMATETC *pFormatEtc, DWORD advf, IAdviseSink *pAdvSink,
                                      DWORD *pdwConnection);
    virtual HRESULT __stdcall DUnadvise(DWORD dwConnection);
    virtual HRESULT __stdcall EnumDAdvise(IEnumSTATDATA **ppEnumAdvise);

private:
    int LookupFormatEtc(FORMATETC *pFormatEtc);
    HRESULT CopyMedium(FORMATETC *fmt, STGMEDIUM *stm, STGMEDIUM *dst);

    LONG            m_lRefCount;
    CDuiValArray    m_cFmtEtc;
    CDuiValArray    m_cStgMed;
};

//////////////////////////////////////////////////////////////////////////
class CEnumFormatEtc : public IEnumFORMATETC
{
public:
    CEnumFormatEtc(FORMATETC *pFormatEtc, int nNumFormats);
    ~CEnumFormatEtc();

    HRESULT __stdcall  QueryInterface(REFIID iid, void **ppvObject);
    ULONG   __stdcall  AddRef(void);
    ULONG   __stdcall  Release(void);

    HRESULT __stdcall  Next(ULONG celt, FORMATETC *pFormatEtc, ULONG *pceltFetched);
    HRESULT __stdcall  Skip(ULONG celt);
    HRESULT __stdcall  Reset(void);
    HRESULT __stdcall  Clone(IEnumFORMATETC **ppEnumFormatEtc);

private:
    LONG        m_lRefCount;        // Reference count for this COM interface
    ULONG       m_nIndex;           // current enumerator index
    ULONG       m_nNumFormats;      // number of FORMATETC members
    FORMATETC  *m_pFormatEtc;        // array of FORMATETC objects
};

}
