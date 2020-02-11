#include "stdafx.h"
#include <string>

namespace DuiLib {

namespace {
static HMODULE s_hDll = NULL;
static int s_nInitCount = 0;

std::string WStr2Utf8(const wchar_t *src)
{
    int ilen = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL);
    char *szBuf = new char[ilen + 1];
    memset(szBuf, 0, ilen + 1);
    WideCharToMultiByte(CP_UTF8, 0, src, -1, szBuf, ilen, NULL, NULL);
    std::string str(szBuf);
    delete []szBuf;
    return str;
}

std::wstring Utf82WStr(const char *src)
{
    int ilen = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, NULL);
    wchar_t *wszBuf = new wchar_t[ilen + 1];
    memset(wszBuf, 0, 2 * (ilen + 1));
    MultiByteToWideChar(CP_UTF8, 0, src, -1, wszBuf, ilen);
    std::wstring str(wszBuf);
    delete []wszBuf;
    return str;
}

std::string WStr2Ansi(const wchar_t *src)
{
    int ilen = WideCharToMultiByte(CP_ACP, 0, src, -1, NULL, 0, NULL, NULL);
    char *szBuf = new char[ilen + 1];
    memset(szBuf, 0, ilen + 1);
    WideCharToMultiByte(CP_ACP, 0, src, -1, szBuf, ilen, NULL, NULL);
    std::string str(szBuf);
    delete[ ]szBuf;
    return str;
}

std::wstring Ansi2WStr(const char *src)
{
    int ilen = MultiByteToWideChar(CP_ACP, 0, src, -1, NULL, NULL);
    wchar_t *wszBuf = new wchar_t[ilen + 1];
    memset(wszBuf, 0, 2 * (ilen + 1));
    MultiByteToWideChar(CP_ACP, 0, src, -1, wszBuf, ilen);
    std::wstring str(wszBuf);
    delete[ ]wszBuf;
    return str;
}

std::string Utf82Ansi(const char *src)
{
    return WStr2Ansi(Utf82WStr(src).c_str());
}

std::string Ansi2Utf8(const char *src)
{
    return WStr2Utf8(Ansi2WStr(src).c_str());
}

#define LOADFUN(FunName)                                    \
    FUN pfn = (FUN)::GetProcAddress(s_hDll, #FunName );     \
    ASSERT(pfn);

}


void wkeInitializeEx(const wkeSettings *settings)
{
    typedef void(*FUN)(const wkeSettings *);
    LOADFUN(wkeInitializeEx);
    pfn ? pfn(settings) : pfn; //lint !e62
}

void wkeFinalize()
{
    typedef void(*FUN)();
    LOADFUN(wkeFinalize);
    pfn ? pfn() : pfn; //lint !e62
}

bool LoadMiniBlink(LPCTSTR szWkeDll, const wkeSettings *settings)
{
    if (NULL == s_hDll)
    {
        s_hDll = LoadLibrary(szWkeDll);
    }

    if (NULL != s_hDll)
    {
        if (0 == s_nInitCount) { wkeInitializeEx(settings); }

        s_nInitCount += 1;
    }

    return (NULL != s_hDll);
}

void FreeMiniBlink(void)
{
    if (NULL != s_hDll)
    {
        s_nInitCount -= 1;

        if (0 == s_nInitCount)
        {
            wkeFinalize();
            FreeLibrary(s_hDll);
            s_hDll = NULL;
        }
    }
}

void wkeConfigure(const wkeSettings *settings)
{
    typedef void(*FUN)(const wkeSettings *);
    LOADFUN(wkeConfigure);
    pfn ? pfn(settings) : false; //lint !e62
}

bool wkeIsInitialize()
{
    typedef bool(*FUN)();
    LOADFUN(wkeIsInitialize);
    return pfn ? pfn() : false;
}

void wkeUpdate()
{
    typedef void(*FUN)();
    LOADFUN(wkeUpdate);
    pfn ? pfn() : false; //lint !e62
}

void wkeShutdown()
{
    typedef void(*FUN)();
    LOADFUN(wkeShutdown);
    pfn ? pfn() : false; //lint !e62
}

void wkeShutdownForDebug()
{
    typedef void(*FUN)();
    LOADFUN(wkeShutdownForDebug);
    pfn ? pfn() : false; //lint !e62
}

unsigned int wkeGetVersion(void)
{
    typedef unsigned int(*FUN)(void);
    LOADFUN(wkeGetVersion);
    return pfn ? pfn() : -1;
}

CDuiString wkeGetVersionStr()
{
    typedef const utf8 *(*FUN)(void);
    LOADFUN(wkeGetVersionString);
    const utf8 *pBuf = pfn ? pfn() : NULL;

#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

wkeWebView wkeCreateWebView(void)
{
    typedef wkeWebView(*FUN)(void);
    LOADFUN(wkeCreateWebView);
    return pfn ? pfn() : NULL;
}

void wkeDestroyWebView(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeDestroyWebView);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeSetViewSettings(wkeWebView webView, const wkeViewSettings *settings)
{
    typedef void(*FUN)(wkeWebView, const wkeViewSettings *);
    LOADFUN(wkeSetViewSettings);
    pfn ? pfn(webView, settings) : pfn; //lint !e62
}

void wkeSetDebugConfig(wkeWebView webView, const CDuiString &debugString, const CDuiString &param)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strDebug = WStr2Utf8(debugString);
    std::string strParam = WStr2Utf8(param);
#else
    std::string strDebug = Ansi2Utf8(debugString);
    std::string strParam = Ansi2Utf8(param);
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *, const utf8 *);
    LOADFUN(wkeSetDebugConfig);
    pfn ? pfn(webView, strDebug.c_str(), strParam.c_str()) : pfn; //lint !e62
}

DUILIB_API void *wkeGetDebugConfig(wkeWebView webView, const CDuiString &debugString)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strDebug = WStr2Utf8(debugString);
#else
    std::string strDebug = Ansi2Utf8(debugString.GetData());
#endif
    typedef void *(*FUN)(wkeWebView, const char *);
    LOADFUN(wkeGetDebugConfig);
    return pfn ? pfn(webView, strDebug.c_str()) : NULL;
}

void wkeSetResourceGc(wkeWebView webView, long intervalSec)
{
    typedef void(*FUN)(wkeWebView, long);
    LOADFUN(wkeSetResourceGc);
    pfn ? pfn(webView, intervalSec) : pfn; //lint !e62
}

void wkeSetMemoryCacheEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetMemoryCacheEnable);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetMouseEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetMouseEnabled);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetTouchEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetTouchEnabled);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetContextMenuEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetContextMenuEnabled);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetNavigationToNewWindowEnable);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetCspCheckEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetCspCheckEnable);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetNpapiPluginsEnabled);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetHeadlessEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetHeadlessEnabled);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetDragEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetDragEnable);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetDragDropEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetDragDropEnable);
    pfn ? pfn(webView, b) : pfn; //lint !e62
}

void wkeSetLanguage(wkeWebView webView, const CDuiString &language)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strLanguage = WStr2Utf8(language.GetData());
#else
    std::string strLanguage = Ansi2Utf8(language.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeSetLanguage);
    pfn ? pfn(webView, strLanguage.c_str()) : pfn; //lint !e62
}

DUILIB_API void wkeSetContextMenuItemShow(wkeWebView webView, wkeMenuItemId item, bool isShow)
{
    typedef void(*FUN)(wkeWebView, wkeMenuItemId, bool);
    LOADFUN(wkeSetContextMenuItemShow);
    pfn ? pfn(webView, item, isShow) : pfn; //lint !e62
}

void wkeSetViewNetInterface(wkeWebView webView, const CDuiString &netInterface)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strNet = WStr2Utf8(netInterface.GetData());
#else
    std::string strNet = Ansi2Utf8(netInterface.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeSetViewNetInterface);
    pfn ? pfn(webView, strNet.c_str()) : pfn; //lint !e62
}

void wkeSetProxy(const wkeProxy *proxy)
{
    typedef void(*FUN)(const wkeProxy *);
    LOADFUN(wkeSetProxy);
    pfn ? pfn(proxy) : pfn; //lint !e62
}

void wkeSetViewProxy(wkeWebView webView, wkeProxy *proxy)
{
    typedef void(*FUN)(wkeWebView, wkeProxy *);
    LOADFUN(wkeSetViewProxy);
    pfn ? pfn(webView, proxy) : pfn; //lint !e62
}

CDuiString wkeGetName(wkeWebView webView)
{
    typedef const utf8 *(*FUN)(wkeWebView);
    LOADFUN(wkeGetName);
    const utf8 *pBuf = pfn ? pfn(webView) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

void wkeSetName(wkeWebView webView, const CDuiString &name)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeSetName);
    pfn ? pfn(webView, strName.c_str()) : pfn; //lint !e62
}

void wkeSetHandle(wkeWebView webView, HWND wnd)
{
    typedef void(*FUN)(wkeWebView, HWND);
    LOADFUN(wkeSetHandle);
    pfn ? pfn(webView, wnd) : pfn; //lint !e62
}

void wkeSetHandleOffset(wkeWebView webView, int x, int y)
{
    typedef void(*FUN)(wkeWebView, int, int);
    LOADFUN(wkeSetHandleOffset);
    pfn ? pfn(webView, x, y) : pfn; //lint !e62
}

bool wkeIsTransparent(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsTransparent);
    return pfn ? pfn(webView) : false;
}

void wkeSetTransparent(wkeWebView webView, bool transparent)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetTransparent);
    pfn ? pfn(webView, transparent) : pfn; //lint !e62
}

void wkeSetUserAgent(wkeWebView webView, const CDuiString &userAgent)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strAgent = userAgent.GetData();
#else
    std::wstring strAgent = Ansi2WStr(userAgent.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetUserAgentW);
    pfn ? pfn(webView, strAgent.c_str()) : pfn; //lint !e62
}

CDuiString wkeGetUserAgent(wkeWebView webView)
{
    typedef const utf8*(*FUN)(wkeWebView);
    LOADFUN(wkeGetUserAgent);
    const utf8 *pBuf = pfn ? pfn(webView) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

void wkeShowDevtools(wkeWebView webView, const CDuiString &path, wkeOnShowDevtoolsCallback callback,
                     void *param)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strPath = path.GetData();
#else
    std::wstring strPath = Ansi2WStr(path.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *, wkeOnShowDevtoolsCallback, void *);
    LOADFUN(wkeShowDevtools);
    pfn ? pfn(webView, strPath.c_str(), callback, param) : pfn; //lint !e62
}

void wkeLoadURL(wkeWebView webView, const CDuiString &url)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strUrl = url.GetData();
#else
    std::wstring strUrl = Ansi2WStr(url.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeLoadURLW);
    pfn ? pfn(webView, strUrl.c_str()) : pfn; //lint !e62
}

void wkePostURL(wkeWebView webView, const CDuiString &url, const CDuiString &postData)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strUrl = url.GetData();
    std::wstring strData = postData.GetData();
#else
    std::wstring strUrl = Ansi2WStr(url.GetData());
    std::wstring strData = Ansi2WStr(postData.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *, const wchar_t *, int);
    LOADFUN(wkePostURLW);
    pfn ? pfn(webView, strUrl.c_str(), strData.c_str(), strData.length()) : pfn; //lint !e62
}

void wkeLoadHTML(wkeWebView webView, const CDuiString &html)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strHtml = html.GetData();
#else
    std::wstring strHtml = Ansi2WStr(html.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeLoadHTMLW);
    pfn ? pfn(webView, strHtml.c_str()) : pfn; //lint !e62
}

void wkeLoadHtmlWithBaseUrl(wkeWebView webView, const CDuiString &html, const CDuiString &baseUrl)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strHtml = WStr2Utf8(html);
    std::string strBaseUrl = WStr2Utf8(baseUrl);
#else
    std::string strHtml = Ansi2Utf8(html);
    std::string strBaseUrl = Ansi2Utf8(baseUrl);
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *, const utf8 *);
    LOADFUN(wkeLoadHtmlWithBaseUrl);
    pfn ? pfn(webView, strHtml.c_str(), strBaseUrl.c_str()) : pfn; //lint !e62
}

void wkeLoadFile(wkeWebView webView, const CDuiString &filename)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strFile = filename.GetData();
#else
    std::wstring strFile = Ansi2WStr(filename.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeLoadFileW);
    pfn ? pfn(webView, strFile.c_str()) : pfn; //lint !e62
}

CDuiString wkeGetURL(wkeWebView webView)
{
    typedef const utf8 *(*FUN)(wkeWebView);
    LOADFUN(wkeGetURL);
    const utf8 *pBuf = pfn ? pfn(webView) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

CDuiString wkeGetFrameUrl(wkeWebView webView, wkeWebFrameHandle frameId)
{
    typedef utf8*(*FUN)(wkeWebView, wkeWebFrameHandle);
    LOADFUN(wkeGetFrameUrl);
    utf8 *pBuf = pfn ? pfn(webView, frameId) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

bool wkeIsLoading(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsLoading);
    return pfn ? pfn(webView) : false;
}

bool wkeIsLoadingSucceeded(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsLoadingSucceeded);
    return pfn ? pfn(webView) : false;
}

bool wkeIsLoadingFailed(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsLoadingFailed);
    return pfn ? pfn(webView) : false;
}

bool wkeIsLoadingCompleted(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsLoadingCompleted);
    return pfn ? pfn(webView) : false;
}

bool wkeIsDocumentReady(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsDocumentReady);
    return pfn ? pfn(webView) : false;
}

void wkeStopLoading(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeStopLoading);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeReload(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeReload);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeGoToOffset(wkeWebView webView, int offset)
{
    typedef void(*FUN)(wkeWebView, int);
    LOADFUN(wkeGoToOffset);
    pfn ? pfn(webView, offset) : pfn; //lint !e62
}

void wkeGoToIndex(wkeWebView webView, int index)
{
    typedef void(*FUN)(wkeWebView, int);
    LOADFUN(wkeGoToIndex);
    pfn ? pfn(webView, index) : pfn; //lint !e62
}

int wkeGetWebviewId(wkeWebView webView)
{
    typedef int(*FUN)(wkeWebView);
    LOADFUN(wkeGetWebviewId);
    return pfn ? pfn(webView) : 0;
}

bool wkeIsWebviewAlive(int id)
{
    typedef bool(*FUN)(int);
    LOADFUN(wkeIsWebviewAlive);
    return pfn ? pfn(id) : false;
}

DUILIB_API bool wkeIsWebviewValid(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsWebviewValid);
    return pfn ? pfn(webView) : false;
}

CDuiString wkeGetDocumentCompleteURL(wkeWebView webView, wkeWebFrameHandle frameId,
                                     const CDuiString &partialURL)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strUrl = WStr2Utf8(partialURL.GetData());
#else
    std::string strUrl = Ansi2Utf8(partialURL.GetData());
#endif
    typedef const utf8*(*FUN)(wkeWebView, wkeWebFrameHandle, const utf8 *);
    LOADFUN(wkeGetDocumentCompleteURL);
    const utf8 *pBuf = pfn ? pfn(webView, frameId, strUrl.c_str()) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

wkeMemBuf *wkeCreateMemBuf(wkeWebView webView, void *buf, size_t length)
{
    typedef wkeMemBuf*(*FUN)(wkeWebView, void *, size_t);
    LOADFUN(wkeCreateMemBuf);
    return pfn ? pfn(webView, buf, length) : NULL;
}

void wkeFreeMemBuf(wkeMemBuf *buf)
{
    typedef void(*FUN)(wkeMemBuf *);
    LOADFUN(wkeFreeMemBuf);
    pfn ? pfn(buf) : pfn; //lint !e62
}

CDuiString wkeGetTitle(wkeWebView webView)
{
    typedef const wchar_t *(*FUN)(wkeWebView);
    LOADFUN(wkeGetTitleW);
    const wchar_t *pBuf = pfn ? pfn(webView) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return pBuf;
#else
    return WStr2Ansi(pBuf).c_str();
#endif
}

void wkeResize(wkeWebView webView, int w, int h)
{
    typedef void(*FUN)(wkeWebView, int, int);
    LOADFUN(wkeResize);
    pfn ? pfn(webView, w, h) : pfn; //lint !e62
}

int wkeGetWidth(wkeWebView webView)
{
    typedef int(*FUN)(wkeWebView);
    LOADFUN(wkeGetWidth);
    return pfn ? pfn(webView) : -1;
}

int wkeGetHeight(wkeWebView webView)
{
    typedef int(*FUN)(wkeWebView);
    LOADFUN(wkeGetHeight);
    return pfn ? pfn(webView) : -1;
}

int wkeGetContentWidth(wkeWebView webView)
{
    typedef int(*FUN)(wkeWebView);
    LOADFUN(wkeGetContentWidth);
    return pfn ? pfn(webView) : -1;
}

int wkeGetContentHeight(wkeWebView webView)
{
    typedef int(*FUN)(wkeWebView);
    LOADFUN(wkeGetContentHeight);
    return pfn ? pfn(webView) : -1;
}

void wkeSetDirty(wkeWebView webView, bool dirty)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetDirty);
    pfn ? pfn(webView, dirty) : pfn; //lint !e62
}

bool wkeIsDirty(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsDirty);
    return pfn ? pfn(webView) : false;
}

void wkeAddDirtyArea(wkeWebView webView, int x, int y, int w, int h)
{
    typedef void(*FUN)(wkeWebView, int, int, int, int);
    LOADFUN(wkeAddDirtyArea);
    pfn ? pfn(webView, x, y, w, h) : pfn; //lint !e62
}

void wkeLayoutIfNeeded(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeLayoutIfNeeded);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkePaint2(wkeWebView webView, void *bits, int bufW, int bufH, int xDst, int yDst,
               int w, int h, int xSrc, int ySrc, bool bCopyAlpha)
{
    typedef void(*FUN)(wkeWebView, void *, int, int, int, int, int, int, int, int, bool);
    LOADFUN(wkePaint2);
    pfn ? pfn(webView, bits, bufW, bufH, xDst, yDst, w, h, xSrc, ySrc, bCopyAlpha) : pfn; //lint !e62
}

void wkePaint(wkeWebView webView, void *bits, int pitch)
{
    typedef void(*FUN)(wkeWebView, void *, int);
    LOADFUN(wkePaint);
    pfn ? pfn(webView, bits, pitch) : pfn; //lint !e62
}

void wkeRepaintIfNeeded(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeRepaintIfNeeded);
    pfn ? pfn(webView) : pfn; //lint !e62
}

HDC wkeGetViewDC(wkeWebView webView)
{
    typedef HDC(*FUN)(wkeWebView);
    LOADFUN(wkeGetViewDC);
    return pfn ? pfn(webView) : NULL;
}

HWND wkeGetHostHWND(wkeWebView webView)
{
    typedef HWND(*FUN)(wkeWebView);
    LOADFUN(wkeGetHostHWND);
    return pfn ? pfn(webView) : NULL;
}

bool wkeCanGoBack(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeCanGoBack);
    return pfn ? pfn(webView) : false;
}

bool wkeGoBack(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeGoBack);
    return pfn ? pfn(webView) : false;
}

bool wkeCanGoForward(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeCanGoForward);
    return pfn ? pfn(webView) : false;
}

bool wkeGoForward(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeGoForward);
    return pfn ? pfn(webView) : false;
}

void wkeEditorSelectAll(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorSelectAll);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeEditorUnSelect(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorUnSelect);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeEditorCopy(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorCopy);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeEditorCut(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorCut);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeEditorPaste(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorPaste);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeEditorDelete(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorDelete);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeEditorUndo(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorUndo);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeEditorRedo(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorRedo);
    pfn ? pfn(webView) : pfn; //lint !e62
}

CDuiString wkeGetCookie(wkeWebView webView)
{
    typedef wchar_t *(*FUN)(wkeWebView);
    LOADFUN(wkeGetCookieW);
    wchar_t *pBuf = pfn ? pfn(webView) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return pBuf;
#else
    return WStr2Ansi(pBuf).c_str();
#endif
}

void wkeSetCookie(wkeWebView webView, const CDuiString &url, const CDuiString &cookie)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strUrl = WStr2Utf8(url.GetData());
    std::string strCookie = WStr2Utf8(cookie.GetData());
#else
    std::string strUrl = Ansi2Utf8(url.GetData());
    std::string strCookie = Ansi2Utf8(cookie.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *, const utf8 *);
    LOADFUN(wkeSetCookie);
    pfn ? pfn(webView, strUrl.c_str(), strCookie.c_str()) : pfn; //lint !e62
}

void wkeVisitAllCookie(void *params, wkeCookieVisitor visitor)
{
    typedef void(*FUN)(void *, wkeCookieVisitor);
    LOADFUN(wkeVisitAllCookie);
    pfn ? pfn(params, visitor) : pfn; //lint !e62
}

void wkePerformCookieCommand(wkeWebView webView, wkeCookieCommand command)
{
    typedef void(*FUN)(wkeWebView, wkeCookieCommand);
    LOADFUN(wkePerformCookieCommand);
    pfn ? pfn(webView, command) : pfn; //lint !e62
}

void wkeSetCookieEnabled(wkeWebView webView, bool enable)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetCookieEnabled);
    pfn ? pfn(webView, enable) : pfn; //lint !e62
}

bool wkeIsCookieEnabled(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsCookieEnabled);
    return pfn ? pfn(webView) : false;
}

void wkeSetCookieJarPath(wkeWebView webView, const CDuiString &path)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strPath = path;
#else
    std::wstring strPath = Ansi2WStr(path);
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetCookieJarPath);
    pfn ? pfn(webView, strPath.c_str()) : pfn; //lint !e62
}

void wkeSetCookieJarFullPath(wkeWebView webView, const CDuiString &path)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strPath = path;
#else
    std::wstring strPath = Ansi2WStr(path);
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetCookieJarFullPath);
    pfn ? pfn(webView, strPath.c_str()) : pfn; //lint !e62
}

DUILIB_API void wkeClearCookie(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeClearCookie);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeSetLocalStorageFullPath(wkeWebView webView, const CDuiString &path)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strPath = path;
#else
    std::wstring strPath = Ansi2WStr(path);
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetLocalStorageFullPath);
    pfn ? pfn(webView, strPath.c_str()) : pfn; //lint !e62
}

void wkeAddPluginDirectory(wkeWebView webView, const CDuiString &path)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strPath = path;
#else
    std::wstring strPath = Ansi2WStr(path);
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeAddPluginDirectory);
    pfn ? pfn(webView, strPath.c_str()) : pfn; //lint !e62
}

void wkeSetMediaVolume(wkeWebView webView, float volume)
{
    typedef void(*FUN)(wkeWebView, float);
    LOADFUN(wkeSetMediaVolume);
    pfn ? pfn(webView, volume) : pfn; //lint !e62
}

float wkeGetMediaVolume(wkeWebView webView)
{
    typedef float(*FUN)(wkeWebView);
    LOADFUN(wkeGetMediaVolume);
    return pfn ? pfn(webView) : 0.0f;
}

bool wkeFireMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags)
{
    typedef bool(*FUN)(wkeWebView, unsigned int, int, int, unsigned int);
    LOADFUN(wkeFireMouseEvent);
    return pfn ? pfn(webView, message, x, y, flags) : false;
}

bool wkeFireContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags)
{
    typedef bool(*FUN)(wkeWebView, int, int, unsigned int);
    LOADFUN(wkeFireContextMenuEvent);
    return pfn ? pfn(webView, x, y, flags) : false;
}

bool wkeFireMouseWheelEvent(wkeWebView webView, int x, int y, int delta, unsigned int flags)
{
    typedef bool(*FUN)(wkeWebView, int, int, int, unsigned int);
    LOADFUN(wkeFireMouseWheelEvent);
    return pfn ? pfn(webView, x, y, delta, flags) : false;
}

bool wkeFireKeyUpEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    typedef bool(*FUN)(wkeWebView, unsigned int, unsigned int, bool);
    LOADFUN(wkeFireKeyUpEvent);
    return pfn ? pfn(webView, virtualKeyCode, flags, systemKey) : false;
}

bool wkeFireKeyDownEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags, bool systemKey)
{
    typedef bool(*FUN)(wkeWebView, unsigned int, unsigned int, bool);
    LOADFUN(wkeFireKeyDownEvent);
    return pfn ? pfn(webView, virtualKeyCode, flags, systemKey) : false;
}

bool wkeFireKeyPressEvent(wkeWebView webView, unsigned int charCode, unsigned int flags, bool systemKey)
{
    typedef bool(*FUN)(wkeWebView, unsigned int, unsigned int, bool);
    LOADFUN(wkeFireKeyPressEvent);
    return pfn ? pfn(webView, charCode, flags, systemKey) : false;
}

bool wkeFireWindowsMessage(wkeWebView webView, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam,
                           LRESULT *result)
{
    typedef bool(*FUN)(wkeWebView, HWND, UINT, WPARAM, LPARAM, LRESULT *);
    LOADFUN(wkeFireWindowsMessage);
    return pfn ? pfn(webView, hWnd, message, wParam, lParam, result) : false;
}

void wkeSetFocus(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeSetFocus);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeKillFocus(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeKillFocus);
    pfn ? pfn(webView) : pfn; //lint !e62
}

wkeRect wkeGetCaretRect(wkeWebView webView)
{
    typedef wkeRect(*FUN)(wkeWebView);
    LOADFUN(wkeGetCaret);
    return pfn ? pfn(webView) : wkeRect { 0, 0, 0, 0 };//lint !e533
}

jsValue wkeRunJS(wkeWebView webView, const CDuiString &script)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strScript = script.GetData();
#else
    std::wstring strScript = Ansi2WStr(script.GetData());
#endif
    typedef jsValue(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeRunJSW);
    return pfn ? pfn(webView, strScript.c_str()) : -1;
}

jsExecState wkeGlobalExec(wkeWebView webView)
{
    typedef jsExecState(*FUN)(wkeWebView);
    LOADFUN(wkeGlobalExec);
    return pfn ? pfn(webView) : NULL;
}

jsExecState wkeGetGlobalExecByFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    typedef jsExecState(*FUN)(wkeWebView, wkeWebFrameHandle);
    LOADFUN(wkeGetGlobalExecByFrame);
    return pfn ? pfn(webView, frameId) : NULL;
}

void wkeSleep(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeSleep);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeWake(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeWake);
    pfn ? pfn(webView) : pfn; //lint !e62
}

bool wkeIsAwake(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsAwake);
    return pfn ? pfn(webView) : false;
}

void wkeSetZoomFactor(wkeWebView webView, float factor)
{
    typedef void(*FUN)(wkeWebView, float);
    LOADFUN(wkeSetZoomFactor);
    pfn ? pfn(webView, factor) : pfn; //lint !e62
}

float wkeGetZoomFactor(wkeWebView webView)
{
    typedef float(*FUN)(wkeWebView);
    LOADFUN(wkeGetZoomFactor);
    return pfn ? pfn(webView) : 0.0f;
}

void wkeSetEditable(wkeWebView webView, bool editable)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetEditable);
    pfn ? pfn(webView, editable) : pfn; //lint !e62
}

CDuiString wkeGetString(const wkeString string)
{
    typedef const wchar_t *(*FUN)(const wkeString);
    LOADFUN(wkeGetStringW);
    const wchar_t *pBuf = pfn ? pfn(string) : NULL;

#if defined(UNICODE) || defined(_UNICODE)
    return pBuf;
#else
    return WStr2Ansi(pBuf).c_str();
#endif
}

void wkeSetString(wkeString string, const CDuiString &str)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strStr = str.GetData();
#else
    std::wstring strStr = Ansi2WStr(str.GetData());
#endif
    typedef void(*FUN)(wkeString, const wchar_t *, size_t);
    LOADFUN(wkeSetStringW);
    pfn ? pfn(string, strStr.c_str(), strStr.length()) : pfn; //lint !e62
}

wkeString wkeCreateString(const CDuiString &str)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strStr = str.GetData();
#else
    std::wstring strStr = Ansi2WStr(str.GetData());
#endif
    typedef wkeString(*FUN)(const wchar_t *, size_t);
    LOADFUN(wkeCreateStringW);
    return pfn ? pfn(strStr.c_str(), strStr.length()) : NULL;
}

size_t wkeGetStringLen(wkeString str)
{
    typedef size_t(*FUN)(wkeString);
    LOADFUN(wkeGetStringLen);
    return pfn ? pfn(str) : 0;
}

void wkeDeleteString(wkeString str)
{
    typedef void(*FUN)(wkeString);
    LOADFUN(wkeDeleteString);
    pfn ? pfn(str) : pfn; //lint !e62
}

wkeWebView wkeGetWebViewForCurrentContext()
{
    typedef wkeWebView(*FUN)();
    LOADFUN(wkeGetWebViewForCurrentContext);
    return pfn ? pfn() : NULL;
}

void wkeSetUserKeyValue(wkeWebView webView, const CDuiString &key, void *value)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strKey = WStr2Utf8(key.GetData());
#else
    std::string strKey = Ansi2Utf8(key.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *, void *);
    LOADFUN(wkeSetUserKeyValue);
    pfn ? pfn(webView, strKey.c_str(), value) : pfn; //lint !e62
}

void *wkeGetUserKeyValue(wkeWebView webView, const CDuiString &key)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strKey = WStr2Utf8(key.GetData());
#else
    std::string strKey = Ansi2Utf8(key.GetData());
#endif
    typedef void *(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeGetUserKeyValue);
    return pfn ? pfn(webView, strKey.c_str()) : NULL;
}

int wkeGetCursorInfoType(wkeWebView webView)
{
    typedef int(*FUN)(wkeWebView);
    LOADFUN(wkeGetCursorInfoType);
    return pfn ? pfn(webView) : 0;
}

void wkeSetCursorInfoType(wkeWebView webView, int type)
{
    typedef void(*FUN)(wkeWebView, int);
    LOADFUN(wkeSetCursorInfoType);
    pfn ? pfn(webView, type) : pfn; //lint !e62
}

void wkeSetDragFiles(wkeWebView webView, const POINT *clintPos, const POINT *screenPos, wkeString files[],
                     int filesCount)
{
    typedef void(*FUN)(wkeWebView, const POINT *, const POINT *, wkeString[], int);
    LOADFUN(wkeSetDragFiles);
    pfn ? pfn(webView, clintPos, screenPos, files, filesCount) : pfn; //lint !e62
}

void wkeSetDeviceParameter(wkeWebView webView, const CDuiString &device, const CDuiString &paramStr,
                           int paramInt, float paramFloat)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strDevice = WStr2Utf8(device.GetData());
    std::string strParam = WStr2Utf8(paramStr.GetData());
#else
    std::string strDevice = Ansi2Utf8(device.GetData());
    std::string strParam = Ansi2Utf8(paramStr.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *, const utf8 *, int, float);
    LOADFUN(wkeSetDeviceParameter);
    pfn ? pfn(webView, strDevice.c_str(), strParam.c_str(), paramInt, paramFloat) : pfn; //lint !e62
}

wkeTempCallbackInfo *wkeGetTempCallbackInfo(wkeWebView webView)
{
    typedef wkeTempCallbackInfo*(*FUN)(wkeWebView);
    LOADFUN(wkeGetTempCallbackInfo);
    return pfn ? pfn(webView) : NULL;
}


//wke callback-----------------------------------------------------------------------------------
void wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeTitleChangedCallback, void *);
    LOADFUN(wkeOnTitleChanged);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnMouseOverUrlChanged(wkeWebView webView, wkeTitleChangedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeTitleChangedCallback, void *);
    LOADFUN(wkeOnMouseOverUrlChanged);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeURLChangedCallback, void *);
    LOADFUN(wkeOnURLChanged);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnURLChanged2(wkeWebView webView, wkeURLChangedCallback2 callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeURLChangedCallback2, void *);
    LOADFUN(wkeOnURLChanged2);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkePaintUpdatedCallback, void *);
    LOADFUN(wkeOnPaintUpdated);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnPaintBitUpdated(wkeWebView webView, wkePaintBitUpdatedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkePaintBitUpdatedCallback, void *);
    LOADFUN(wkeOnPaintBitUpdated);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeAlertBoxCallback, void *);
    LOADFUN(wkeOnAlertBox);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeConfirmBoxCallback, void *);
    LOADFUN(wkeOnConfirmBox);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkePromptBoxCallback, void *);
    LOADFUN(wkeOnPromptBox);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnNavigation(wkeWebView webView, wkeNavigationCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeNavigationCallback, void *);
    LOADFUN(wkeOnNavigation);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeCreateViewCallback, void *);
    LOADFUN(wkeOnCreateView);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDocumentReadyCallback, void *);
    LOADFUN(wkeOnDocumentReady);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnDocumentReady2(wkeWebView webView, wkeDocumentReady2Callback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDocumentReady2Callback, void *);
    LOADFUN(wkeOnDocumentReady2);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeLoadingFinishCallback, void *);
    LOADFUN(wkeOnLoadingFinish);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDownloadCallback, void *);
    LOADFUN(wkeOnDownload);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnDownload2(wkeWebView webView, wkeDownload2Callback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDownload2Callback, void *);
    LOADFUN(wkeOnDownload2);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnConsole(wkeWebView webView, wkeConsoleCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeConsoleCallback, void *);
    LOADFUN(wkeOnConsole);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeSetUIThreadCallback(wkeWebView webView, wkeCallUiThread callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeCallUiThread, void *);
    LOADFUN(wkeSetUIThreadCallback);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeLoadUrlBeginCallback, void *);
    LOADFUN(wkeOnLoadUrlBegin);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeLoadUrlEndCallback, void *);
    LOADFUN(wkeOnLoadUrlEnd);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

DUILIB_API void wkeOnLoadUrlFail(wkeWebView webView, wkeLoadUrlFailCallback callback, void *callbackParam)
{
    typedef void(*FUN)(wkeWebView, wkeLoadUrlFailCallback, void *);
    LOADFUN(wkeOnLoadUrlFail);
    pfn ? pfn(webView, callback, callbackParam) : pfn; //lint !e62
}

void wkeOnDidCreateScriptContext(wkeWebView webView, wkeDidCreateScriptContextCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDidCreateScriptContextCallback, void *);
    LOADFUN(wkeOnDidCreateScriptContext);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnWillReleaseScriptContext(wkeWebView webView, wkeWillReleaseScriptContextCallback callback,
                                   void *param)
{
    typedef void(*FUN)(wkeWebView, wkeWillReleaseScriptContextCallback, void *);
    LOADFUN(wkeOnWillReleaseScriptContext);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnWindowClosing(wkeWebView webView, wkeWindowClosingCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeWindowClosingCallback, void *);
    LOADFUN(wkeOnWindowClosing);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnWindowDestroy(wkeWebView webView, wkeWindowDestroyCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeWindowDestroyCallback, void *);
    LOADFUN(wkeOnWindowDestroy);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnDraggableRegionsChanged(wkeWebView webView, wkeDraggableRegionsChangedCallback callback,
                                  void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDraggableRegionsChangedCallback, void *);
    LOADFUN(wkeOnDraggableRegionsChanged);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnWillMediaLoad(wkeWebView webView, wkeWillMediaLoadCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeWillMediaLoadCallback, void *);
    LOADFUN(wkeOnWillMediaLoad);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnStartDragging(wkeWebView webView, wkeStartDraggingCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeStartDraggingCallback, void *);
    LOADFUN(wkeOnStartDragging);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnPrint(wkeWebView webView, wkeOnPrintCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeOnPrintCallback, void *);
    LOADFUN(wkeOnPrint);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

DUILIB_API void wkeScreenshot(wkeWebView webView, const wkeScreenshotSettings *settings,
                              wkeOnScreenshot callback, void *param)
{
    typedef void(*FUN)(wkeWebView, const wkeScreenshotSettings *, wkeOnScreenshot, void *);
    LOADFUN(wkeScreenshot);
    pfn ? pfn(webView, settings, callback, param) : pfn; //lint !e62
}

void wkeOnOtherLoad(wkeWebView webView, wkeOnOtherLoadCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeOnOtherLoadCallback, void *);
    LOADFUN(wkeOnOtherLoad);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

bool wkeIsProcessingUserGesture(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsProcessingUserGesture);
    return pfn ? pfn(webView) : false;
}

void wkeNetSetMIMEType(wkeNetJob job, const CDuiString &type)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strType = WStr2Utf8(type.GetData());
#else
    std::string strType = Ansi2Utf8(type.GetData());
#endif
    typedef void(*FUN)(wkeNetJob, const utf8 *);
    LOADFUN(wkeNetSetMIMEType);
    pfn ? pfn(job, (utf8 *)strType.c_str()) : pfn; //lint !e62
}

CDuiString wkeNetGetMIMEType(wkeNetJob jobPtr, wkeString mime)
{
    typedef utf8*(*FUN)(wkeNetJob, wkeString);
    LOADFUN(wkeNetGetMIMEType);
    utf8 *pBuf = pfn ? pfn(jobPtr, mime) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return CDuiString(Utf82WStr(pBuf).c_str());
#else
    return CDuiString(Utf82Ansi(pBuf).c_str());
#endif
}

void wkeNetSetHTTPHeaderField(wkeNetJob jobPtr, const CDuiString &key, const CDuiString &value, bool response)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strKey = key.GetData();
    std::wstring strValue = value.GetData();
#else
    std::wstring strKey = Ansi2WStr(key.GetData());
    std::wstring strValue = Ansi2WStr(value.GetData());
#endif
    typedef void(*FUN)(wkeNetJob, const wchar_t *, const wchar_t *, bool);
    LOADFUN(wkeNetSetHTTPHeaderField);
    pfn ? pfn(jobPtr, strKey.c_str(), strValue.c_str(), response) : pfn; //lint !e62
}

CDuiString wkeNetGetHTTPHeaderField(wkeNetJob jobPtr, const CDuiString &key)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strKey = WStr2Utf8(key.GetData());
#else
    std::string strKey = Ansi2Utf8(key.GetData());
#endif
    typedef const utf8*(*FUN)(wkeNetJob, const utf8 *);
    LOADFUN(wkeNetGetHTTPHeaderField);
    const utf8 *pBuf = pfn ? pfn(jobPtr, strKey.c_str()) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

CDuiString wkeNetGetHTTPHeaderFieldFromResponse(wkeNetJob jobPtr, CDuiString key)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strKey = WStr2Utf8(key.GetData());
#else
    std::string strKey = Ansi2Utf8(key.GetData());
#endif
    typedef const utf8*(*FUN)(wkeNetJob, const utf8 *);
    LOADFUN(wkeNetGetHTTPHeaderFieldFromResponse);
    const utf8 *pBuf = pfn ? pfn(jobPtr, strKey.c_str()) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

void wkeNetSetData(wkeNetJob jobPtr, void *buf, int len)
{
    typedef void(*FUN)(wkeNetJob, void *, int);
    LOADFUN(wkeNetSetData);
    pfn ? pfn(jobPtr, buf, len) : pfn; //lint !e62
}

void wkeNetHookRequest(wkeNetJob jobPtr)
{
    typedef void(*FUN)(wkeNetJob);
    LOADFUN(wkeNetHookRequest);
    pfn ? pfn(jobPtr) : pfn; //lint !e62
}

void wkeNetOnResponse(wkeWebView webView, wkeNetResponseCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeNetResponseCallback, void *);
    LOADFUN(wkeNetOnResponse);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

wkeRequestType wkeNetGetRequestMethod(wkeNetJob jobPtr)
{
    typedef wkeRequestType(*FUN)(wkeNetJob);
    LOADFUN(wkeRequestType);
    return pfn ? pfn(jobPtr) : kWkeRequestTypeInvalidation;
}

int wkeNetGetFavicon(wkeWebView webView, wkeOnNetGetFaviconCallback callback, void *param)
{
    typedef int(*FUN)(wkeWebView, wkeOnNetGetFaviconCallback, void *);
    LOADFUN(wkeNetGetFavicon);
    return pfn ? pfn(webView, callback, param) : 0;
}

void wkeNetContinueJob(wkeNetJob jobPtr)
{
    typedef void(*FUN)(wkeNetJob);
    LOADFUN(wkeNetContinueJob);
    pfn ? pfn(jobPtr) : pfn; //lint !e62
}

CDuiString wkeNetGetUrlByJob(wkeNetJob jobPtr)
{
    typedef const utf8*(*FUN)(wkeNetJob);
    LOADFUN(wkeNetContinueJob);
    const utf8 *pBuf = pfn ? pfn(jobPtr) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

const wkeSlist *wkeNetGetRawHttpHead(wkeNetJob jobPtr)
{
    typedef const wkeSlist*(*FUN)(wkeNetJob jobPtr);
    LOADFUN(wkeNetGetRawHttpHead);
    return pfn ? pfn(jobPtr) : NULL;
}

const wkeSlist *wkeNetGetRawResponseHead(wkeNetJob jobPtr)
{
    typedef const wkeSlist*(*FUN)(wkeNetJob jobPtr);
    LOADFUN(wkeNetGetRawResponseHead);
    return pfn ? pfn(jobPtr) : NULL;
}

void wkeNetCancelRequest(wkeNetJob jobPtr)
{
    typedef void(*FUN)(wkeNetJob);
    LOADFUN(wkeNetCancelRequest);
    pfn ? pfn(jobPtr) : pfn; //lint !e62
}

BOOL wkeNetHoldJobToAsynCommit(wkeNetJob jobPtr)
{
    typedef BOOL(*FUN)(wkeNetJob);
    LOADFUN(wkeNetHoldJobToAsynCommit);
    return pfn ? pfn(jobPtr) : FALSE;
}

void wkeNetChangeRequestUrl(wkeNetJob jobPtr, const CDuiString &url)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strUrl = WStr2Utf8(url.GetData());
#else
    std::string strUrl = Ansi2Utf8(url.GetData());
#endif
    typedef void(*FUN)(wkeNetJob, const utf8 *);
    LOADFUN(wkeNetChangeRequestUrl);
    pfn ? pfn(jobPtr, strUrl.c_str()) : pfn; //lint !e62
}

wkeWebUrlRequestPtr wkeNetCreateWebUrlRequest(const CDuiString &url, const CDuiString &method,
                                              const CDuiString &mime)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strUrl = WStr2Utf8(url.GetData());
    std::string strMethod = WStr2Utf8(method.GetData());
    std::string strMime = WStr2Utf8(mime.GetData());
#else
    std::string strUrl = Ansi2Utf8(url.GetData());
    std::string strMethod = Ansi2Utf8(url.GetData());
    std::string strMime = Ansi2Utf8(url.GetData());
#endif
    typedef wkeWebUrlRequestPtr(*FUN)(const utf8 *, const utf8 *, const utf8 *);
    LOADFUN(wkeNetCreateWebUrlRequest);
    return pfn ? pfn(strUrl.c_str(), strMethod.c_str(), strMime.c_str()) : NULL;
}

wkeWebUrlRequestPtr wkeNetCreateWebUrlRequest2(const blinkWebURLRequestPtr request)
{
    typedef wkeWebUrlRequestPtr(*FUN)(const blinkWebURLRequestPtr);
    LOADFUN(wkeNetCreateWebUrlRequest2);
    return pfn ? pfn(request) : NULL;
}

blinkWebURLRequestPtr wkeNetCopyWebUrlRequest(wkeNetJob jobPtr, bool needExtraData)
{
    typedef blinkWebURLRequestPtr(*FUN)(wkeNetJob, bool);
    LOADFUN(wkeNetCopyWebUrlRequest);
    return pfn ? pfn(jobPtr, needExtraData) : NULL;
}

void wkeNetDeleteBlinkWebURLRequestPtr(blinkWebURLRequestPtr request)
{
    typedef void(*FUN)(blinkWebURLRequestPtr);
    LOADFUN(wkeNetDeleteBlinkWebURLRequestPtr);
    pfn ? pfn(request) : pfn; //lint !e62
}

void wkeNetAddHTTPHeaderFieldToUrlRequest(wkeWebUrlRequestPtr request, const CDuiString &name,
                                          const CDuiString &value)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
    std::string strValue = WStr2Utf8(value.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
    std::string strValue = Ansi2Utf8(value.GetData());
#endif
    typedef void(*FUN)(wkeWebUrlRequestPtr, const utf8 *, const utf8 *);
    LOADFUN(wkeNetAddHTTPHeaderFieldToUrlRequest);
    pfn ? pfn(request, strName.c_str(), strValue.c_str()) : pfn; //lint !e62
}

int wkeNetStartUrlRequest(wkeWebView webView, wkeWebUrlRequestPtr request, void *param,
                          const wkeUrlRequestCallbacks *callback)
{
    typedef int(*FUN)(wkeWebView, wkeWebUrlRequestPtr, void *, const wkeUrlRequestCallbacks *);
    LOADFUN(wkeNetStartUrlRequest);
    return pfn ? pfn(webView, request, param, callback) : 0;
}

int wkeNetGetHttpStatusCode(wkeWebUrlResponsePtr response)
{
    typedef int(*FUN)(wkeWebUrlResponsePtr);
    LOADFUN(wkeNetGetHttpStatusCode);
    return pfn ? pfn(response) : 0;
}

__int64 wkeNetGetExpectedContentLength(wkeWebUrlResponsePtr response)
{
    typedef __int64(*FUN)(wkeWebUrlResponsePtr);
    LOADFUN(wkeNetGetExpectedContentLength);
    return pfn ? pfn(response) : 0;
}

CDuiString wkeNetGetResponseUrl(wkeWebUrlResponsePtr response)
{
    typedef const utf8*(*FUN)(wkeWebUrlResponsePtr);
    LOADFUN(wkeNetGetResponseUrl);
    const utf8 *pBuf = pfn ? pfn(response) : 0;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

void wkeNetCancelWebUrlRequest(int requestId)
{
    typedef void(*FUN)(int);
    LOADFUN(wkeNetCancelWebUrlRequest);
    pfn ? pfn(requestId) : pfn; //lint !e62
}

wkePostBodyElements *wkeNetGetPostBody(wkeNetJob jobPtr)
{
    typedef wkePostBodyElements *(*FUN)(void *);
    LOADFUN(wkeNetGetPostBody);
    return pfn ? pfn(jobPtr) : NULL;
}

wkePostBodyElements *wkeNetCreatePostBodyElements(wkeWebView webView, size_t length)
{
    typedef wkePostBodyElements*(*FUN)(wkeWebView, size_t);
    LOADFUN(wkeNetCreatePostBodyElements);
    return pfn ? pfn(webView, length) : NULL;
}

void wkeNetFreePostBodyElements(wkePostBodyElements *element)
{
    typedef void(*FUN)(wkePostBodyElements *);
    LOADFUN(wkeNetFreePostBodyElements);
    pfn ? pfn(element) : pfn; //lint !e62
}

wkePostBodyElement *wkeNetCreatePostBodyElement(wkeWebView webView)
{
    typedef wkePostBodyElement*(*FUN)(wkeWebView);
    LOADFUN(wkeNetCreatePostBodyElement);
    return pfn ? pfn(webView) : NULL;
}

void wkeNetFreePostBodyElement(wkePostBodyElement *element)
{
    typedef void(*FUN)(wkePostBodyElement *);
    LOADFUN(wkeNetFreePostBodyElement);
    pfn ? pfn(element) : pfn; //lint !e62
}

bool wkeIsMainFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    typedef bool(*FUN)(wkeWebView, wkeWebFrameHandle);
    LOADFUN(wkeIsMainFrame);
    return pfn ? pfn(webView, frameId) : false;
}

bool wkeIsWebRemoteFrame(wkeWebView webView, wkeWebFrameHandle frameId)
{
    typedef bool(*FUN)(wkeWebView, wkeWebFrameHandle);
    LOADFUN(wkeIsWebRemoteFrame);
    return pfn ? pfn(webView, frameId) : false;
}

wkeWebFrameHandle wkeWebFrameGetMainFrame(wkeWebView webView)
{
    typedef wkeWebFrameHandle(*FUN)(wkeWebView);
    LOADFUN(wkeWebFrameGetMainFrame);
    return pfn ? pfn(webView) : NULL;
}

jsValue wkeRunJsByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const CDuiString &script,
                        bool isInClosure)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strScript = WStr2Utf8(script.GetData());
#else
    std::string strScript = Ansi2Utf8(script.GetData());
#endif
    typedef jsValue(*FUN)(wkeWebView, wkeWebFrameHandle, const utf8 *, bool);
    LOADFUN(wkeRunJsByFrame);
    return pfn ? pfn(webView, frameId, strScript.c_str(), isInClosure) : -1;
}

void wkeInsertCSSByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const CDuiString &cssText)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strCSS = WStr2Utf8(cssText.GetData());
#else
    std::string strCSS = Ansi2Utf8(cssText.GetData());
#endif
    typedef void(*FUN)(wkeWebView, wkeWebFrameHandle, const utf8 *);
    LOADFUN(wkeInsertCSSByFrame);
    pfn ? pfn(webView, frameId, strCSS.c_str()) : pfn; //lint !e62
}

void wkeWebFrameGetMainWorldScriptContext(wkeWebView webView, wkeWebFrameHandle frameId,
                                          v8ContextPtr contextOut)
{
    typedef void(*FUN)(wkeWebView, wkeWebFrameHandle, v8ContextPtr);
    LOADFUN(wkeWebFrameGetMainWorldScriptContext);
    pfn ? pfn(webView, frameId, contextOut) : pfn; //lint !e62
}

v8Isolate wkeGetBlinkMainThreadIsolate()
{
    typedef v8Isolate(*FUN)();
    LOADFUN(wkeGetBlinkMainThreadIsolate);
    return pfn ? pfn() : NULL;
}


wkeWebView wkeCreateWebWindow(wkeWindowType type, HWND parent, int x, int y, int width, int height)
{
    typedef wkeWebView(*FUN)(wkeWindowType, HWND, int, int, int, int);
    LOADFUN(wkeCreateWebWindow);
    return pfn ? pfn(type, parent, x, y, width, height) : NULL;
}

wkeWebView wkeCreateWebCustomWindow(const wkeWindowCreateInfo *info)
{
    typedef wkeWebView(*FUN)(const wkeWindowCreateInfo *);
    LOADFUN(wkeCreateWebWindow);
    return pfn ? pfn(info) : NULL;
}

void wkeDestroyWebWindow(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeDestroyWebWindow);
    pfn ? pfn(webView) : pfn; //lint !e62
}

HWND wkeGetWindowHandle(wkeWebView webView)
{
    typedef HWND(*FUN)(wkeWebView);
    LOADFUN(wkeGetWindowHandle);
    return pfn ? pfn(webView) : NULL;
}

void wkeShowWindow(wkeWebView webView, bool show)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeShowWindow);
    pfn ? pfn(webView, show) : pfn; //lint !e62
}

void wkeEnableWindow(wkeWebView webView, bool enable)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeEnableWindow);
    pfn ? pfn(webView, enable) : pfn; //lint !e62
}

void wkeMoveWindow(wkeWebView webView, int x, int y, int width, int height)
{
    typedef void(*FUN)(wkeWebView, int, int, int, int);
    LOADFUN(wkeMoveWindow);
    pfn ? pfn(webView, x, y, width, height) : pfn; //lint !e62
}

void wkeMoveToCenter(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeMoveToCenter);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeResizeWindow(wkeWebView webView, int width, int height)
{
    typedef void(*FUN)(wkeWebView, int, int);
    LOADFUN(wkeResizeWindow);
    pfn ? pfn(webView, width, height) : pfn; //lint !e62
}

wkeWebDragOperation wkeDragTargetDragEnter(wkeWebView webView, const wkeWebDragData *webDragData,
                                           const POINT *clientPoint, const POINT *screenPoint,
                                           wkeWebDragOperationsMask operationsAllowed, int modifiers)
{
    typedef wkeWebDragOperation(*FUN)(wkeWebView, const wkeWebDragData *, const POINT *, const POINT *,
                                      wkeWebDragOperationsMask, int);
    LOADFUN(wkeDragTargetDragEnter);
    return pfn ?
           pfn(webView, webDragData, clientPoint, screenPoint, operationsAllowed, modifiers) : wkeWebDragOperationNone;
}

wkeWebDragOperation wkeDragTargetDragOver(wkeWebView webView, const POINT *clientPoint,
                                          const POINT *screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers)
{
    typedef wkeWebDragOperation(*FUN)(wkeWebView, const POINT *, const POINT *, wkeWebDragOperationsMask, int);
    LOADFUN(wkeDragTargetDragOver);
    return pfn ? pfn(webView, clientPoint, screenPoint, operationsAllowed, modifiers) : wkeWebDragOperationNone;
}

void wkeDragTargetDragLeave(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeDragTargetDragLeave);
    pfn ? pfn(webView) : pfn; //lint !e62
}

void wkeDragTargetDrop(wkeWebView webView, const POINT *clientPoint, const POINT *screenPoint, int modifiers)
{
    typedef void(*FUN)(wkeWebView, const POINT *, const POINT *, int);
    LOADFUN(wkeDragTargetDrop);
    pfn ? pfn(webView, clientPoint, screenPoint, modifiers) : pfn; //lint !e62
}

void wkeDragTargetEnd(wkeWebView webView, const POINT *clientPoint, const POINT *screenPoint,
                      wkeWebDragOperation operation)
{
    typedef void(*FUN)(wkeWebView, const POINT *, const POINT *, wkeWebDragOperation);
    LOADFUN(wkeDragTargetEnd);
    pfn ? pfn(webView, clientPoint, screenPoint, operation) : pfn; //lint !e62
}

void wkeUtilSetUiCallback(wkeUiThreadPostTaskCallback callback)
{
    typedef void(*FUN)(wkeUiThreadPostTaskCallback);
    LOADFUN(wkeUtilSetUiCallback);
    pfn ? pfn(callback) : pfn; //lint !e62
}

CDuiString wkeUtilSerializeToMHTML(wkeWebView webView)
{
    typedef const utf8*(*FUN)(wkeWebView);
    LOADFUN(wkeUtilSerializeToMHTML);
    const utf8 *pBuf = pfn ? pfn(webView) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return CDuiString(Utf82WStr(pBuf).c_str());
#else
    return CDuiString(Utf82Ansi(pBuf).c_str());
#endif
}

const wkePdfDatas *wkeUtilPrintToPdf(wkeWebView webView, wkeWebFrameHandle frameId,
                                     const wkePrintSettings *settings)
{
    typedef const wkePdfDatas*(*FUN)(wkeWebView, wkeWebFrameHandle, const wkePrintSettings *);
    LOADFUN(wkeUtilPrintToPdf);
    return pfn ? pfn(webView, frameId, settings) : NULL;
}

const wkeMemBuf *wkePrintToBitmap(wkeWebView webView, wkeWebFrameHandle frameId,
                                  const wkeScreenshotSettings *settings)
{
    typedef const wkeMemBuf*(*FUN)(wkeWebView, wkeWebFrameHandle, const wkeScreenshotSettings *);
    LOADFUN(wkePrintToBitmap);
    return pfn ? pfn(webView, frameId, settings) : NULL;
}

void wkeUtilRelasePrintPdfDatas(const wkePdfDatas *datas)
{
    typedef void(*FUN)(const wkePdfDatas *);
    LOADFUN(wkeUtilRelasePrintPdfDatas);
    pfn ? pfn(datas) : pfn; //lint !e62
}

void wkeSetWindowTitle(wkeWebView webView, const CDuiString &title)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strTitle = title.GetData();
#else
    std::wstring strTitle = Ansi2WStr(title.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetWindowTitleW);
    pfn ? pfn(webView, strTitle.c_str()) : pfn; //lint !e62
}

void wkeNodeOnCreateProcess(wkeWebView webView, wkeNodeOnCreateProcessCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeNodeOnCreateProcessCallback, void *);
    LOADFUN(wkeNodeOnCreateProcess);
    pfn ? pfn(webView, callback, param) : pfn; //lint !e62
}

void wkeOnPluginFind(wkeWebView webView, const CDuiString &mime, wkeOnPluginFindCallback callback,
                     void *param)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strMime = WStr2Utf8(mime.GetData());
#else
    std::string strMime = Ansi2Utf8(mime.GetData());
#endif
    typedef void(*FUN)(wkeWebView, const utf8 *, wkeOnPluginFindCallback, void *);
    LOADFUN(wkeOnPluginFind);
    pfn ? pfn(webView, strMime.c_str(), callback, param) : pfn; //lint !e62
}

void wkeAddNpapiPlugin(wkeWebView webView, void *initializeFunc, void *getEntryPointsFunc, void *shutdownFunc)
{
    typedef void(*FUN)(wkeWebView, void *, void *, void *);
    LOADFUN(wkeAddNpapiPlugin);
    pfn ? pfn(webView, initializeFunc, getEntryPointsFunc, shutdownFunc) : pfn; //lint !e62
}

void wkePluginListBuilderAddPlugin(void *builder, const CDuiString &name, const CDuiString &desc,
                                   const CDuiString &fileName)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
    std::string strDesc = WStr2Utf8(desc.GetData());
    std::string strFile = WStr2Utf8(fileName.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
    std::string strDesc = Ansi2Utf8(desc.GetData());
    std::string strFile = Ansi2Utf8(fileName.GetData());
#endif
    typedef void(*FUN)(void *, const utf8 *, const utf8 *, const utf8 *);
    LOADFUN(wkePluginListBuilderAddPlugin);
    pfn ? pfn(builder, strName.c_str(), strDesc.c_str(), strFile.c_str()) : pfn; //lint !e62
}

void wkePluginListBuilderAddMediaTypeToLastPlugin(void *builder, const CDuiString &name,
                                                  const CDuiString &desc)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
    std::string strDesc = WStr2Utf8(desc.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
    std::string strDesc = Ansi2Utf8(desc.GetData());
#endif
    typedef void(*FUN)(void *, const utf8 *, const utf8 *);
    LOADFUN(wkePluginListBuilderAddMediaTypeToLastPlugin);
    pfn ? pfn(builder, strName.c_str(), strDesc.c_str()) : pfn; //lint !e62
}

void wkePluginListBuilderAddFileExtensionToLastMediaType(void *builder, const CDuiString &fileExtension)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strExt = WStr2Utf8(fileExtension.GetData());
#else
    std::string strExt = Ansi2Utf8(fileExtension.GetData());
#endif
    typedef void(*FUN)(void *, const utf8 *);
    LOADFUN(wkePluginListBuilderAddFileExtensionToLastMediaType);
    pfn ? pfn(builder, strExt.c_str()) : pfn; //lint !e62
}

wkeWebView wkeGetWebViewByNData(void *ndata)
{
    typedef wkeWebView(*FUN)(void *);
    LOADFUN(wkeGetWebViewByNData);
    return pfn ? pfn(ndata) : NULL;
}

bool wkeRegisterEmbedderCustomElement(wkeWebView webView, wkeWebFrameHandle frameId, const CDuiString &name,
                                      void *options, void *outResult)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
#endif
    typedef bool(*FUN)(wkeWebView, wkeWebFrameHandle, const utf8 *, void *, void *);
    LOADFUN(wkeRegisterEmbedderCustomElement);
    return pfn ? pfn(webView, frameId, strName.c_str(), options, outResult) : false;
}

void wkeSetMediaPlayerFactory(wkeWebView webView, wkeMediaPlayerFactory factory,
                              wkeOnIsMediaPlayerSupportsMIMEType callback)
{
    typedef void(*FUN)(wkeWebView, wkeMediaPlayerFactory, wkeOnIsMediaPlayerSupportsMIMEType);
    LOADFUN(wkeSetMediaPlayerFactory);
    pfn ? pfn(webView, factory, callback) : pfn; //lint !e62
}

CDuiString wkeGetContentAsMarkup(wkeWebView webView, wkeWebFrameHandle frame, size_t *size)
{
    typedef const utf8*(*FUN)(wkeWebView, wkeWebFrameHandle, size_t *);
    LOADFUN(wkeGetContentAsMarkup);
    const utf8 *pBuf = pfn ? pfn(webView, frame, size) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

CDuiString wkeUtilDecodeURLEscape(const CDuiString &url)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strUrl = WStr2Utf8(url.GetData());
#else
    std::string strUrl = Ansi2Utf8(url.GetData());
#endif
    typedef const utf8*(*FUN)(const utf8 *);
    LOADFUN(wkeUtilDecodeURLEscape);
    const utf8 *pBuf = pfn ? pfn(strUrl.c_str()) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

CDuiString wkeUtilEncodeURLEscape(const CDuiString &url)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strUrl = WStr2Utf8(url.GetData());
#else
    std::string strUrl = Ansi2Utf8(url.GetData());
#endif
    typedef const utf8*(*FUN)(const utf8 *);
    LOADFUN(wkeUtilEncodeURLEscape);
    const utf8 *pBuf = pfn ? pfn(strUrl.c_str()) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

CDuiString wkeUtilBase64Encode(const CDuiString &str)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strStr = WStr2Utf8(str.GetData());
#else
    std::string strStr = Ansi2Utf8(str.GetData());
#endif
    typedef const utf8*(*FUN)(const utf8 *);
    LOADFUN(wkeUtilEncodeURLEscape);
    const utf8 *pBuf = pfn ? pfn(strStr.c_str()) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

CDuiString wkeUtilBase64Decode(const CDuiString &str)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strStr = WStr2Utf8(str.GetData());
#else
    std::string strStr = Ansi2Utf8(str.GetData());
#endif
    typedef const utf8*(*FUN)(const utf8 *);
    LOADFUN(wkeUtilBase64Decode);
    const utf8 *pBuf = pfn ? pfn(strStr.c_str()) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

const wkeMemBuf *wkeUtilCreateV8Snapshot(const CDuiString &str)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strStr = WStr2Utf8(str.GetData());
#else
    std::string strStr = Ansi2Utf8(str.GetData());
#endif
    typedef const wkeMemBuf*(*FUN)(const utf8 *);
    LOADFUN(wkeUtilCreateV8Snapshot);
    return pfn ? pfn(strStr.c_str()) : NULL;
}

void wkeRunMessageLoop()
{
    typedef void(*FUN)();
    LOADFUN(wkeRunMessageLoop);
    pfn ? pfn() : pfn; //lint !e62
}


//JavaScript Bind-----------------------------------------------------------------------------------
void __fastcall jsBindFunction(const CDuiString &name, jsNativeFunction fn, unsigned int argCount)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
#endif
    typedef void(*FUN)(const utf8 *, jsNativeFunction, unsigned int);
    LOADFUN(jsBindFunction);
    pfn ? pfn(strName.c_str(), fn, argCount) : pfn; //lint !e62
}

void jsBindGetter(const CDuiString &name, jsNativeFunction fn)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
#endif
    typedef void(*FUN)(const utf8 *, jsNativeFunction);
    LOADFUN(jsBindGetter);
    pfn ? pfn(strName.c_str(), fn) : pfn; //lint !e62
}

void jsBindSetter(const CDuiString &name, jsNativeFunction fn)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
#endif
    typedef void(*FUN)(const utf8 *, jsNativeFunction);
    LOADFUN(jsBindSetter);
    pfn ? pfn(strName.c_str(), fn) : pfn; //lint !e62
}

void jsBindFunctionEx(const CDuiString &name, jsNativeFunctionEx fn, void *param, unsigned int argCount)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
#endif
    typedef void(*FUN)(const utf8 *, jsNativeFunctionEx, void *, unsigned int);
    LOADFUN(wkeJsBindFunction);
    pfn ? pfn(strName.c_str(), fn, param, argCount) : pfn; //lint !e62
}

void jsBindGetterEx(const CDuiString &name, jsNativeFunctionEx fn, void *param)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
#endif
    typedef void(*FUN)(const utf8 *, jsNativeFunctionEx, void *);
    LOADFUN(jsBindGetterEx);
    pfn ? pfn(strName.c_str(), fn, param) : pfn; //lint !e62
}

void jsBindSetterEx(const CDuiString &name, jsNativeFunctionEx fn, void *param)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strName = WStr2Utf8(name.GetData());
#else
    std::string strName = Ansi2Utf8(name.GetData());
#endif
    typedef void(*FUN)(const utf8 *, jsNativeFunctionEx, void *);
    LOADFUN(jsBindSetterEx);
    pfn ? pfn(strName.c_str(), fn, param) : pfn; //lint !e62
}

int jsArgCount(jsExecState es)
{
    typedef int(*FUN)(jsExecState);
    LOADFUN(jsArgCount);
    return pfn ? pfn(es) : -1;
}

jsType jsArgType(jsExecState es, int argIdx)
{
    typedef jsType(*FUN)(jsExecState, int);
    LOADFUN(jsArgType);
    return pfn ? pfn(es, argIdx) : JSTYPE_UNDEFINED;
}

jsValue jsArgValue(jsExecState es, int argIdx)
{
    typedef jsValue(*FUN)(jsExecState, int);
    LOADFUN(jsArg);
    return pfn ? pfn(es, argIdx) : -1;
}

jsType jsTypeOf(jsExecState es, jsValue v)
{
    typedef jsType(*FUN)(jsExecState, jsValue);
    LOADFUN(jsTypeOf);
    return pfn ? pfn(es, v) : JSTYPE_UNDEFINED;
}

bool jsIsNumber(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsNumber);
    return pfn ? pfn(es, v) : false;
}

bool jsIsString(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsString);
    return pfn ? pfn(es, v) : false;
}

bool jsIsBoolean(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsBoolean);
    return pfn ? pfn(es, v) : false;
}

bool jsIsObject(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsObject);
    return pfn ? pfn(es, v) : false;
}

bool jsIsFunction(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsFunction);
    return pfn ? pfn(es, v) : false;
}

bool jsIsUndefined(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsUndefined);
    return pfn ? pfn(es, v) : false;
}

bool jsIsNull(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsNull);
    return pfn ? pfn(es, v) : false;
}

bool jsIsArray(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsArray);
    return pfn ? pfn(es, v) : false;
}

bool jsIsTrue(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsTrue);
    return pfn ? pfn(es, v) : false;
}

bool jsIsFalse(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsFalse);
    return pfn ? pfn(es, v) : false;
}

int jsToInt(jsExecState es, jsValue v)
{
    typedef int(*FUN)(jsExecState, jsValue);
    LOADFUN(jsToInt);
    return pfn ? pfn(es, v) : -1;
}

double jsToFloat(jsExecState es, jsValue v)
{
    typedef double(*FUN)(jsExecState, jsValue);
    LOADFUN(jsToFloat);
    return pfn ? pfn(es, v) : 0.0f;
}

double jsToDouble(jsExecState es, jsValue v)
{
    typedef double(*FUN)(jsExecState, jsValue);
    LOADFUN(jsToDouble);
    return pfn ? pfn(es, v) : 0.0;
}

bool jsToBoolean(jsExecState es, jsValue v)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsToBoolean);
    return pfn ? pfn(es, v) : false;
}

jsValue jsArrayBuffer(jsExecState es, CDuiString buffer, size_t size)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strBuf = WStr2Utf8(buffer.GetData());
#else
    std::string strBuf = Ansi2Utf8(buffer.GetData());
#endif
    typedef jsValue(*FUN)(jsExecState, const utf8 *, size_t);
    LOADFUN(jsArrayBuffer);
    return pfn ? pfn(es, strBuf.c_str(), size) : -1;
}

wkeMemBuf *jsGetArrayBuffer(jsExecState es, jsValue v)
{
    typedef wkeMemBuf*(*FUN)(jsExecState, jsValue);
    LOADFUN(jsGetArrayBuffer);
    return pfn ? pfn(es, v) : NULL;
}

CDuiString jsToTempString(jsExecState es, jsValue v)
{
    typedef const wchar_t *(*FUN)(jsExecState, jsValue);
    LOADFUN(jsToTempStringW);
    const wchar_t *pBuf = pfn ? pfn(es, v) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return pBuf;
#else
    return WStr2Ansi(pBuf).c_str();
#endif
}

void *jsToV8Value(jsExecState es, jsValue v)
{
    typedef void *(*FUN)(jsExecState, jsValue);
    LOADFUN(jsToV8Value);
    return pfn ? pfn(es, v) : NULL;
}

jsValue jsInt(jsExecState es, int v)
{
    typedef jsValue(*FUN)(jsExecState, int);
    LOADFUN(jsInt);
    return pfn ? pfn(es, v) : -1;
}

jsValue jsFloat(jsExecState es, float v)
{
    typedef jsValue(*FUN)(jsExecState, float);
    LOADFUN(jsFloat);
    return pfn ? pfn(es, v) : -1;
}

jsValue jsDouble(jsExecState es, double v)
{
    typedef jsValue(*FUN)(jsExecState, double);
    LOADFUN(jsDouble);
    return pfn ? pfn(es, v) : -1;
}

jsValue jsBoolean(jsExecState es, bool v)
{
    typedef jsValue(*FUN)(jsExecState, int);
    LOADFUN(jsBoolean);
    return pfn ? pfn(es, v) : -1;
}

jsValue jsUndefined()
{
    typedef jsValue(*FUN)();
    LOADFUN(jsUndefined);
    return pfn ? pfn() : -1;
}

jsValue jsNull()
{
    typedef jsValue(*FUN)();
    LOADFUN(jsNull);
    return pfn ? pfn() : -1;
}

jsValue jsTrue()
{
    typedef jsValue(*FUN)();
    LOADFUN(jsTrue);
    return pfn ? pfn() : -1;
}

jsValue jsFalse()
{
    typedef jsValue(*FUN)();
    LOADFUN(jsFalse);
    return pfn ? pfn() : -1;
}

jsValue jsString(jsExecState es, const CDuiString &str)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strStr = str.GetData();
#else
    std::wstring strStr = Ansi2WStr(str.GetData());
#endif
    typedef jsValue(*FUN)(jsExecState, const wchar_t *);
    LOADFUN(jsStringW);
    return pfn ? pfn(es, strStr.c_str()) : -1;
}

jsValue jsEmptyObject(jsExecState es)
{
    typedef jsValue(*FUN)(jsExecState);
    LOADFUN(jsEmptyObject);
    return pfn ? pfn(es) : -1;
}

jsValue jsEmptyArray(jsExecState es)
{
    typedef jsValue(*FUN)(jsExecState);
    LOADFUN(jsEmptyArray);
    return pfn ? pfn(es) : -1;
}

jsValue jsObject(jsExecState es, jsData *obj)
{
    typedef jsValue(*FUN)(jsExecState, jsData *);
    LOADFUN(jsObject);
    return pfn ? pfn(es, obj) : -1;
}

jsValue jsFunction(jsExecState es, jsData *data)
{
    typedef jsValue(*FUN)(jsExecState, jsData *);
    LOADFUN(jsFunction);
    return pfn ? pfn(es, data) : -1;
}

jsData *jsGetData(jsExecState es, jsValue obj)
{
    typedef jsData*(*FUN)(jsExecState, jsValue);
    LOADFUN(jsGetData);
    return pfn ? pfn(es, obj) : NULL;
}

jsValue jsGet(jsExecState es, jsValue obj, const CDuiString &prop)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strProp = WStr2Utf8(prop.GetData());
#else
    std::string strProp = Ansi2Utf8(prop.GetData());
#endif
    typedef jsValue(*FUN)(jsExecState, jsValue, const utf8 *);
    LOADFUN(jsGet);
    return pfn ? pfn(es, obj, strProp.c_str()) : -1;
}

void jsSet(jsExecState es, jsValue obj, const CDuiString &prop, jsValue v)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strProp = WStr2Utf8(prop.GetData());
#else
    std::string strProp = Ansi2Utf8(prop.GetData());
#endif
    typedef void(*FUN)(jsExecState, jsValue, const utf8 *, jsValue);
    LOADFUN(jsSet);
    pfn ? pfn(es, obj, strProp.c_str(), v) : pfn; //lint !e62
}

jsValue jsGetAt(jsExecState es, jsValue obj, int index)
{
    typedef jsValue(*FUN)(jsExecState, jsValue, int);
    LOADFUN(jsGetAt);
    return pfn ? pfn(es, obj, index) : -1;
}

void jsSetAt(jsExecState es, jsValue obj, int index, jsValue v)
{
    typedef void(*FUN)(jsExecState, jsValue, int, jsValue);
    LOADFUN(jsSetAt);
    pfn ? pfn(es, obj, index, v) : pfn; //lint !e62
}

jsKeys *jsGetKeys(jsExecState es, jsValue obj)
{
    typedef jsKeys *(*FUN)(jsExecState, jsValue);
    LOADFUN(jsGetKeys);
    return pfn ? pfn(es, obj) : NULL;
}

void jsFreeKeys(jsKeys *obj)
{
    // 无需释放 key 对象，系统自动释放
    if (obj)
    {
        for (size_t i = 0; i < obj->length; ++i)
        {
            char *key = *(utf8 **)(obj->keys + i);
            delete[ ] key;
        }

        delete obj->keys;
    }
}

bool jsIsJsValueValid(jsExecState es, jsValue obj)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsIsJsValueValid);
    return pfn ? pfn(es, obj) : NULL;
}

bool jsIsValidExecState(jsExecState es)
{
    typedef bool(*FUN)(jsExecState);
    LOADFUN(jsIsValidExecState);
    return pfn ? pfn(es) : false;
}

void jsDeleteObjectProp(jsExecState es, jsValue obj, const CDuiString &prop)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strProp = WStr2Utf8(prop.GetData());
#else
    std::string strProp = Ansi2Utf8(prop.GetData());
#endif
    typedef void(*FUN)(jsExecState, jsValue, const utf8 *);
    LOADFUN(jsDeleteObjectProp);
    pfn ? pfn(es, obj, strProp.c_str()) : pfn; //lint !e62
}

int jsGetLength(jsExecState es, jsValue obj)
{
    typedef int(*FUN)(jsExecState, jsValue);
    LOADFUN(jsGetLength);
    return pfn ? pfn(es, obj) : -1;
}

void jsSetLength(jsExecState es, jsValue obj, int length)
{
    typedef void(*FUN)(jsExecState, jsValue, int);
    LOADFUN(jsSetLength);
    pfn ? pfn(es, obj, length) : pfn; //lint !e62
}

jsValue jsGlobalObject(jsExecState es)
{
    typedef jsValue(*FUN)(jsExecState);
    LOADFUN(jsGlobalObject);
    return pfn ? pfn(es) : -1;
}

wkeWebView jsGetWebView(jsExecState es)
{
    typedef wkeWebView(*FUN)(jsExecState);
    LOADFUN(jsGetWebView);
    return pfn ? pfn(es) : NULL;
}

jsValue jsEval(jsExecState es, const CDuiString &str)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strStr = str.GetData();
#else
    std::wstring strStr = Ansi2WStr(str.GetData());
#endif
    typedef jsValue(*FUN)(jsExecState, const wchar_t *);
    LOADFUN(jsEvalW);
    return pfn ? pfn(es, strStr.c_str()) : -1;
}

jsValue jsEvalEx(jsExecState es, const CDuiString &str, bool isInClosure)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring strStr = str.GetData();
#else
    std::wstring strStr = Ansi2WStr(str.GetData());
#endif
    typedef jsValue(*FUN)(jsExecState, const wchar_t *, bool);
    LOADFUN(jsEvalExW);
    return pfn ? pfn(es, strStr.c_str(), isInClosure) : -1;
}

jsValue jsCall(jsExecState es, jsValue func, jsValue thisObject, jsValue *args, int argCount)
{
    typedef jsValue(*FUN)(jsExecState, jsValue, jsValue, jsValue *, int);
    LOADFUN(jsCall);
    return pfn ? pfn(es, func, thisObject, args, argCount) : -1;
}

jsValue jsCallGlobal(jsExecState es, jsValue func, jsValue *args, int argCount)
{
    typedef jsValue(*FUN)(jsExecState, jsValue, jsValue *, int);
    LOADFUN(jsCallGlobal);
    return pfn ? pfn(es, func, args, argCount) : -1;
}

jsValue jsGetGlobal(jsExecState es, const CDuiString &prop)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strProp = WStr2Utf8(prop.GetData());
#else
    std::string strProp = Ansi2Utf8(prop.GetData());
#endif
    typedef jsValue(*FUN)(jsExecState, const utf8 *);
    LOADFUN(jsGetGlobal);
    return pfn ? pfn(es, strProp.c_str()) : -1;
}

void jsSetGlobal(jsExecState es, const CDuiString &prop, jsValue v)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strProp = WStr2Utf8(prop.GetData());
#else
    std::string strProp = Ansi2Utf8(prop.GetData());
#endif
    typedef void(*FUN)(jsExecState, const utf8 *, jsValue);
    LOADFUN(jsSetGlobal);
    pfn ? pfn(es, strProp.c_str(), v) : pfn; //lint !e62
}

void jsGC()
{
    typedef void(*FUN)();
    LOADFUN(jsGC);
    pfn ? pfn() : pfn; //lint !e62
}

bool jsAddRef(jsExecState es, jsValue val)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsAddRef);
    return pfn ? pfn(es, val) : false;
}

bool jsReleaseRef(jsExecState es, jsValue val)
{
    typedef bool(*FUN)(jsExecState, jsValue);
    LOADFUN(jsReleaseRef);
    return pfn ? pfn(es, val) : false;
}

jsExceptionInfo *jsGetLastErrorIfException(jsExecState es)
{
    typedef jsExceptionInfo*(*FUN)(jsExecState);
    LOADFUN(jsGetLastErrorIfException);
    return pfn ? pfn(es) : NULL;
}

jsValue jsThrowException(jsExecState es, CDuiString exception)
{
#if defined(UNICODE) || defined(_UNICODE)
    std::string strException = WStr2Utf8(exception.GetData());
#else
    std::string strException = Ansi2Utf8(exception.GetData());
#endif
    typedef jsValue(*FUN)(jsExecState, const utf8 *);
    LOADFUN(jsThrowException);
    return pfn ? pfn(es, strException.c_str()) : -1;
}

CDuiString jsGetCallstack(jsExecState es)
{
    typedef const utf8*(*FUN)(jsExecState);
    LOADFUN(jsGetCallstack);
    const utf8 *pBuf = pfn ? pfn(es) : NULL;
#if defined(UNICODE) || defined(_UNICODE)
    return Utf82WStr(pBuf).c_str();
#else
    return Utf82Ansi(pBuf).c_str();
#endif
}

}
