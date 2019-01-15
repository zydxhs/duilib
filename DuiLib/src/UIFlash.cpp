#include "stdafx.h"
#include <atlcomcli.h>
#include "flash11.tlh"

#define DISPID_FLASHEVENT_FLASHCALL  ( 0x00C5 )
#define DISPID_FLASHEVENT_FSCOMMAND  ( 0x0096 )
#define DISPID_FLASHEVENT_ONPROGRESS    ( 0x07A6 )

namespace DuiLib {

class CFlashEvents : public ShockwaveFlashObjects::_IShockwaveFlashEvents
{
public:
    CFlashEvents(void) : m_dwRef(1), m_pOwner(NULL) { }
    virtual ~CFlashEvents(void) { }
    void SetOwner(CFlashUI *pOwner)
    {
        m_pOwner = pOwner;
    }

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(__RPC__out UINT *pctinfo);
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid,
                                                  __RPC__deref_out_opt ITypeInfo **ppTInfo);
    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(__RPC__in REFIID riid,
                                                    __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames, UINT cNames, LCID lcid,
                                                    __RPC__out_ecount_full(cNames) DISPID *rgDispId);
    virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                                             DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

private:
    LONG m_dwRef;
    CFlashUI *m_pOwner;
};

HRESULT STDMETHODCALLTYPE CFlashEvents::QueryInterface(REFIID riid, void **ppvObject)
{
    *ppvObject = NULL;

    if (riid == IID_IUnknown)
    {
        *ppvObject = static_cast<LPUNKNOWN>(this);
    }
    else if (riid == IID_IDispatch)
    {
        *ppvObject = static_cast<IDispatch *>(this);
    }
    else if (riid == __uuidof(_IShockwaveFlashEvents))
    {
        *ppvObject = static_cast<_IShockwaveFlashEvents *>(this);
    }

    if (*ppvObject != NULL)
    {
        AddRef();
    }

    return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

ULONG STDMETHODCALLTYPE CFlashEvents::AddRef(void)
{
    ::InterlockedIncrement(&m_dwRef);
    return m_dwRef;
}

ULONG STDMETHODCALLTYPE CFlashEvents::Release(void)
{
    ::InterlockedDecrement(&m_dwRef);

    if (0 == m_dwRef) { delete this; }

    return m_dwRef;
}

HRESULT STDMETHODCALLTYPE CFlashEvents::GetTypeInfoCount(__RPC__out UINT *pctinfo)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashEvents::GetTypeInfo(UINT iTInfo, LCID lcid,
                                                    __RPC__deref_out_opt ITypeInfo **ppTInfo)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashEvents::GetIDsOfNames(__RPC__in REFIID riid,
                                                      __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames, UINT cNames, LCID lcid,
                                                      __RPC__out_ecount_full(cNames) DISPID *rgDispId)
{
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CFlashEvents::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags,
                                               DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
    switch (dispIdMember)
    {
    case DISPID_FLASHEVENT_FLASHCALL:
        {
            if (pDispParams->cArgs != 1 || pDispParams->rgvarg[0].vt != VT_BSTR) { return E_INVALIDARG; }

            CDuiString str(_bstr_t(pDispParams->rgvarg[0].bstrVal));
            return m_pOwner ? m_pOwner->FlashCall(str) : S_FALSE;
        }

    case DISPID_FLASHEVENT_FSCOMMAND:
        {
            if (pDispParams && pDispParams->cArgs == 2)
            {
                if (pDispParams->rgvarg[0].vt == VT_BSTR && pDispParams->rgvarg[1].vt == VT_BSTR)
                {
                    CDuiString str1(_bstr_t(pDispParams->rgvarg[1].bstrVal));
                    CDuiString str2(_bstr_t(pDispParams->rgvarg[0].bstrVal));
                    return m_pOwner ? m_pOwner->FSCommand(str1, str2) : S_FALSE;
                }
                else
                {
                    return DISP_E_TYPEMISMATCH;
                }
            }
            else
            {
                return DISP_E_BADPARAMCOUNT;
            }
        }

    case DISPID_FLASHEVENT_ONPROGRESS:
        {
            return m_pOwner ? m_pOwner->OnProgress(*pDispParams->rgvarg[0].plVal) : S_FALSE;
        }

    case DISPID_READYSTATECHANGE:
        {
            return m_pOwner ? m_pOwner->OnReadyStateChange(pDispParams->rgvarg[0].lVal) : S_FALSE;
        }
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////
CFlashUI::CFlashUI(void) : m_dwCookie(0), m_pFlash(NULL), m_pFlashEvents(NULL), m_nState(0), m_nProgress(0)
{
    CDuiString strFlashCLSID = _T("{D27CDB6E-AE6D-11CF-96B8-444553540000}");
    OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
    ::MultiByteToWideChar(::GetACP(), 0, strFlashCLSID, -1, szCLSID, LENGTHOF(szCLSID) - 1);
#else
    _tcsncpy(szCLSID, strFlashCLSID, LENGTHOF(szCLSID) - 1);
#endif
    ::CLSIDFromString(szCLSID, &m_clsid);
    m_pFlashEvents = new CFlashEvents;
    m_pFlashEvents->SetOwner(this);
}

CFlashUI::~CFlashUI(void)
{
    if (NULL != m_pFlashEvents) { m_pFlashEvents->Release(); m_pFlashEvents = NULL; }

    ReleaseControl();
}

LPCTSTR CFlashUI::GetClass() const
{
    return DUI_CTR_FLASH;
}

LPVOID CFlashUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_FLASH) == 0) { return static_cast<CFlashUI *>(this); }

    return CActiveXUI::GetInterface(pstrName);
}

HRESULT CFlashUI::OnReadyStateChange(long newState)
{
    //if (m_pFlashEventHandler) { return m_pFlashEventHandler->OnReadyStateChange(newState); }

    return S_OK;
}

HRESULT CFlashUI::OnProgress(long percentDone)
{
    //if (m_pFlashEventHandler) { return m_pFlashEventHandler->OnProgress(percentDone); }

    return S_OK;
}

HRESULT CFlashUI::FSCommand(CDuiString &command, CDuiString &args)
{
    //if (m_pFlashEventHandler) { return m_pFlashEventHandler->FSCommand(command, args); }

    return S_OK;
}

HRESULT CFlashUI::FlashCall(CDuiString &request)
{
    //if (m_pFlashEventHandler) { return m_pFlashEventHandler->FlashCall(request); }

    return S_OK;
}

void CFlashUI::ReleaseControl()
{
    //GetManager()->RemoveTranslateAccelerator(this);
    RegisterEventHandler(FALSE);

    if (m_pFlash)
    {
        m_pFlash->Release();
        m_pFlash = NULL;
    }
}

bool CFlashUI::DoCreateControl()
{
    if (!CActiveXUI::DoCreateControl()) { return false; }

    //GetManager()->AddTranslateAccelerator(this);
    if (NULL == m_pFlash)
    {
        GetControl(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (LPVOID *)&m_pFlash);
    }

    if (NULL != m_pFlash)
    {
        _bstr_t str;
        str = m_sAlign;
        m_pFlash->put_SAlign(str);
        m_pFlash->put_BackgroundColor(m_dwBackColor);
        str = m_sWMode;
        m_pFlash->put_WMode(str);
        str = (CPaintManagerUI::GetResourcePath() + m_sMovie).GetData();
        m_pFlash->put_Movie(str);
        str = m_sBase;
        m_pFlash->put_Base(str);
    }

    RegisterEventHandler(TRUE);
    return true;
}

ShockwaveFlashObjects::IShockwaveFlash *CFlashUI::GetShockwaveFlash(void)
{
    if (NULL == m_pFlash)
    {
        GetControl(__uuidof(ShockwaveFlashObjects::IShockwaveFlash), (LPVOID *)&m_pFlash);
    }

    return m_pFlash;
}

LRESULT CFlashUI::TranslateAccelerator(MSG *pMsg)
{
    if (pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) { return S_FALSE; }

    if (m_pFlash == NULL) { return E_NOTIMPL; }

    // 当前Flash窗口不是焦点,不处理加速键
    BOOL bIsChild = FALSE;
    HWND hTempWnd = NULL;
    HWND hWndFocus = ::GetFocus();

    hTempWnd = hWndFocus;

    while (hTempWnd != NULL)
    {
        if (hTempWnd == m_hwndHost)
        {
            bIsChild = TRUE;
            break;
        }

        hTempWnd = ::GetParent(hTempWnd);
    }

    if (!bIsChild) { return S_FALSE; }

    CComPtr<IOleInPlaceActiveObject> pObj;

    if (FAILED(m_pFlash->QueryInterface(IID_IOleInPlaceActiveObject, (LPVOID *)&pObj))) { return S_FALSE; }

    HRESULT hResult = pObj->TranslateAccelerator(pMsg);
    return hResult;
}

HRESULT CFlashUI::RegisterEventHandler(BOOL inAdvise)
{
    if (m_pFlash == NULL) { return S_FALSE; }

    HRESULT hr = S_FALSE;
    CComPtr<IConnectionPointContainer>  pCPC;
    CComPtr<IConnectionPoint> pCP;

    hr = m_pFlash->QueryInterface(IID_IConnectionPointContainer, (void **)&pCPC);

    if (FAILED(hr)) { return hr; }

    hr = pCPC->FindConnectionPoint(__uuidof(ShockwaveFlashObjects::_IShockwaveFlashEvents), &pCP);

    if (FAILED(hr)) { return hr; }

    if (inAdvise)
    {
        hr = pCP->Advise(m_pFlashEvents, &m_dwCookie);
    }
    else
    {
        hr = pCP->Unadvise(m_dwCookie);
    }

    return hr;
}

void CFlashUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    // Flash 控件不支持这两个属性
    if (_tcscmp(pstrName, _T("homepage")) == 0) { DUITRACE(_T("不支持属性:homepage")); }
    else if (_tcscmp(pstrName, _T("autonavi")) == 0) { DUITRACE(_T("不支持属性:autonavi")); }
    else if (_tcscmp(pstrName, _T("align")) == 0)
    {
        m_sAlign = ParseString(pstrValue);
    }
    else if (_tcscmp(pstrName, _T("bkcolor")) == 0)
    {
        m_dwBackColor = ParseColor(pstrValue);
    }
    else if (_tcscmp(pstrName, _T("wmode")) == 0) { m_sWMode = ParseString(pstrValue); }
    else if (_tcscmp(pstrName, _T("movie")) == 0) { m_sMovie = ParseString(pstrValue); }
    else if (_tcscmp(pstrName, _T("base")) == 0)  { m_sBase = ParseString(pstrValue); }
    else if (_tcscmp(pstrName, _T("scale")) == 0) { m_sScale = ParseString(pstrValue); }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CActiveXUI::SetAttribute(pstrName, pstrValue); }
}

CDuiString CFlashUI::GetAlign(void)
{
    return m_sAlign;
}

void CFlashUI::SetAlign(CDuiString sAlign)
{
    if (NULL != m_pFlash)
    {
        _bstr_t str = m_sAlign;
        m_pFlash->put_SAlign(str);
        Invalidate();
    }
}

CDuiString CFlashUI::GetWMode(void)
{
    return m_sWMode;
}

void CFlashUI::SetWMode(CDuiString sWMode)
{
    if (NULL != m_pFlash)
    {
        _bstr_t str = m_sWMode;
        m_pFlash->put_WMode(str);
        Invalidate();
    }
}

CDuiString CFlashUI::GetMovie(void)
{
    return m_sMovie;
}

void CFlashUI::SetMovie(CDuiString sMovie)
{
    if (NULL != m_pFlash)
    {
        _bstr_t str = m_sMovie;
        m_pFlash->put_Movie(str);
        Invalidate();
    }
}

DuiLib::CDuiString CFlashUI::GetBase(void)
{
    return m_sBase;
}

void CFlashUI::SetBase(CDuiString sBase)
{
    if (NULL != m_pFlash)
    {
        _bstr_t str = m_sBase;
        m_pFlash->put_Base(str);
    }
}

CDuiString CFlashUI::GetScale(void)
{
    return m_sScale;
}

void CFlashUI::SetScale(CDuiString sScale)
{
    if (NULL != m_pFlash)
    {
        _bstr_t str = m_sScale;
        m_pFlash->put_Scale(str);
        Invalidate();
    }
}

bool CFlashUI::IsPlaying(void)
{
    if (NULL != m_pFlash)
    {
        VARIANT_BOOL bRet = VARIANT_FALSE;
        m_pFlash->get_Playing(&bRet);
        return bRet ? true : false;
    }

    return false;
}

};
