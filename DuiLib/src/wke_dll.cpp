#include "stdafx.h"
#include <string>

namespace DuiLib {

namespace {
static HMODULE s_hDll = NULL;
static int s_nInitCount = 0;

std::string WStr2Utf8(const wchar_t *src)
{
    int ilen = WideCharToMultiByte(CP_UTF8, 0, src, -1, NULL, 0, NULL, NULL); //获取长度
    char *szBuf = new char[ilen + 1];
    memset(szBuf, 0, ilen + 1);
    WideCharToMultiByte(CP_UTF8, 0, src, -1, szBuf, ilen, NULL, NULL); //UTF转码
    std::string str(szBuf);
    delete []szBuf;
    return str;
}

std::wstring Utf82WStr(const char *src)
{
    int ilen = MultiByteToWideChar(CP_UTF8, 0, src, -1, NULL, NULL); //获取长度
    wchar_t *wszBuf = new wchar_t[ilen + 1];
    memset(wszBuf, 0, 2 * (ilen + 1));
    MultiByteToWideChar(CP_UTF8, 0, src, -1, wszBuf, ilen); //UTF转码
    std::wstring str(wszBuf);
    delete []wszBuf;
    return str;
}

}


#define LOADFUN(FunName) FUN pfn = (FUN)::GetProcAddress(s_hDll, #FunName );

void            wkeInitialize();
void            wkeFinalize();

bool LoadWke(LPCTSTR szWkeDll)
{
    if (NULL == s_hDll)
    {
        s_hDll = LoadLibrary(szWkeDll);
    }

    if (NULL != s_hDll)
    {
        if (0 == s_nInitCount) { wkeInitialize(); }

        s_nInitCount += 1;
    }

    return (NULL != s_hDll);
}

void FreeWke(void)
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


void wkeOnMouseOverUrlChanged(wkeWebView webView, wkeTitleChangedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeTitleChangedCallback, void *);
    LOADFUN(wkeOnMouseOverUrlChanged);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeTitleChangedCallback, void *);
    LOADFUN(wkeOnTitleChanged);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeURLChangedCallback, void *);
    LOADFUN(wkeOnURLChanged);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnURLChanged2(wkeWebView webView, wkeURLChangedCallback2 callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeURLChangedCallback2, void *);
    LOADFUN(wkeOnURLChanged2);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkePaintUpdatedCallback, void *);
    LOADFUN(wkeOnPaintUpdated);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnPaintBitUpdated(wkeWebView webView, wkePaintBitUpdatedCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkePaintBitUpdatedCallback, void *);
    LOADFUN(wkeOnPaintBitUpdated);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeAlertBoxCallback, void *);
    LOADFUN(wkeOnAlertBox);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeConfirmBoxCallback, void *);
    LOADFUN(wkeOnConfirmBox);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkePromptBoxCallback, void *);
    LOADFUN(wkeOnPromptBox);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnNavigation(wkeWebView webView, wkeNavigationCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeNavigationCallback, void *);
    LOADFUN(wkeOnNavigation);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeCreateViewCallback, void *);
    LOADFUN(wkeOnCreateView);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDocumentReadyCallback, void *);
    LOADFUN(wkeOnDocumentReady);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnDocumentReady2(wkeWebView webView, wkeDocumentReady2Callback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDocumentReady2Callback, void *);
    LOADFUN(wkeOnDocumentReady2);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeLoadingFinishCallback, void *);
    LOADFUN(wkeOnLoadingFinish);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDownloadCallback, void *);
    LOADFUN(wkeOnDownload);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnConsole(wkeWebView webView, wkeConsoleCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeConsoleCallback, void *);
    LOADFUN(wkeOnConsole);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeSetUIThreadCallback(wkeWebView webView, wkeCallUiThread callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeCallUiThread, void *);
    LOADFUN(wkeSetUIThreadCallback);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeLoadUrlBeginCallback, void *);
    LOADFUN(wkeOnLoadUrlBegin);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeLoadUrlEndCallback, void *);
    LOADFUN(wkeOnLoadUrlEnd);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnDidCreateScriptContext(wkeWebView webView, wkeDidCreateScriptContextCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDidCreateScriptContextCallback, void *);
    LOADFUN(wkeOnDidCreateScriptContext);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnWillReleaseScriptContext(wkeWebView webView, wkeWillReleaseScriptContextCallback callback,
                                   void *param)
{
    typedef void(*FUN)(wkeWebView, wkeWillReleaseScriptContextCallback, void *);
    LOADFUN(wkeOnWillReleaseScriptContext);
    pfn ? pfn(webView, callback, param) : pfn;
}

void wkeOnWindowClosing(wkeWebView webWindow, wkeWindowClosingCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeWindowClosingCallback, void *);
    LOADFUN(wkeOnWindowClosing);
    pfn ? pfn(webWindow, callback, param) : pfn;
}

void wkeOnWindowDestroy(wkeWebView webWindow, wkeWindowDestroyCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeWindowDestroyCallback, void *);
    LOADFUN(wkeOnWindowDestroy);
    pfn ? pfn(webWindow, callback, param) : pfn;
}

void wkeOnDraggableRegionsChanged(wkeWebView webWindow, wkeDraggableRegionsChangedCallback callback,
                                  void *param)
{
    typedef void(*FUN)(wkeWebView, wkeDraggableRegionsChangedCallback, void *);
    LOADFUN(wkeOnDraggableRegionsChanged);
    pfn ? pfn(webWindow, callback, param) : pfn;
}

void wkeOnWillMediaLoad(wkeWebView webWindow, wkeWillMediaLoadCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeWillMediaLoadCallback, void *);
    LOADFUN(wkeOnWillMediaLoad);
    pfn ? pfn(webWindow, callback, param) : pfn;
}

void wkeOnOtherLoad(wkeWebView webWindow, wkeOnOtherLoadCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeOnOtherLoadCallback, void *);
    LOADFUN(wkeOnOtherLoad);
    pfn ? pfn(webWindow, callback, param) : pfn;
}

void wkeInitialize(void)
{
    typedef void(*FUN)(void);
    LOADFUN(wkeInitialize);
    pfn ? pfn() : pfn;
}

void wkeInitializeEx(const wkeSettings *settings)
{
    typedef void(*FUN)(const wkeSettings *);
    LOADFUN(wkeInitializeEx);
    pfn ? pfn(settings) : pfn;
}

void wkeConfigure(const wkeSettings *settings)
{
    typedef void(*FUN)(const wkeSettings *);
    LOADFUN(wkeConfigure);
    pfn ? pfn(settings) : pfn;
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
    return pfn ? pfn() : pfn;
}

void wkeFinalize()
{
    typedef void(*FUN)();
    LOADFUN(wkeFinalize);
    return pfn ? pfn() : pfn;
}

void wkeShutdown(void)
{
    typedef void(*FUN)(void);
    LOADFUN(wkeShutdown);
    pfn ? pfn() : pfn;
}

void wkeSetViewSettings(wkeWebView webView, const wkeViewSettings *settings)
{
    typedef void(*FUN)(wkeWebView, const wkeViewSettings *);
    LOADFUN(wkeSetViewSettings);
    pfn ? pfn(webView, settings) : pfn;
}

void wkeSetDebugConfig(wkeWebView webView, const char *debugString, const char *param)
{
    typedef void(*FUN)(wkeWebView, const char *, const char *);
    LOADFUN(wkeSetDebugConfig);
    pfn ? pfn(webView, debugString, param) : pfn;
}

unsigned int wkeGetVersion(void)
{
    typedef unsigned int(*FUN)(void);
    LOADFUN(wkeGetVersion);
    return pfn ? pfn() : -1;
}

const utf8 *wkeGetVersionString(void)
{
    typedef const utf8 *(*FUN)(void);
    LOADFUN(wkeGetVersionString);
    return pfn ? pfn() : NULL;
}

void wkeGC(wkeWebView webView, long delayMs)
{
    typedef void(*FUN)(wkeWebView, long);
    LOADFUN(wkeGC);
    pfn ? pfn(webView, delayMs) : pfn;
}

void wkeSetFileSystem(WKE_FILE_OPEN pfnOpen, WKE_FILE_CLOSE pfnClose, WKE_FILE_SIZE pfnSize,
                      WKE_FILE_READ pfnRead, WKE_FILE_SEEK pfnSeek)
{
    typedef void(*FUN)(WKE_FILE_OPEN, WKE_FILE_CLOSE, WKE_FILE_SIZE, WKE_FILE_READ, WKE_FILE_SEEK);
    LOADFUN(wkeSetFileSystem);
    pfn ? pfn(pfnOpen, pfnClose, pfnSize, pfnRead, pfnSeek) : pfn;
}

const char *wkeGetWebViewName(wkeWebView webView)
{
    typedef const char *(*FUN)(wkeWebView);
    LOADFUN(wkeWebViewName);
    return pfn ? pfn(webView) : NULL;
}

void wkeSetWebViewName(wkeWebView webView, const char *name)
{
    typedef void(*FUN)(wkeWebView, const char *);
    LOADFUN(wkeSetWebViewName);
    pfn ? pfn(webView, name) : pfn;
}

void wkeDestroyWebView(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeDestroyWebView);
    pfn ? pfn(webView) : pfn;
}

wkeWebView wkeCreateWebView(void)
{
    typedef wkeWebView(*FUN)(void);
    LOADFUN(wkeCreateWebView);
    return pfn ? pfn() : NULL;
}

wkeWebView wkeGetWebViewForCurrentContext()
{
    typedef wkeWebView(*FUN)(void);
    LOADFUN(wkeGetWebViewForCurrentContext);
    return pfn ? pfn() : NULL;
}

bool wkeIsLoaded(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsLoaded);
    return pfn ? pfn(webView) : false;
}

bool wkeIsLoadFailed(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsLoadFailed);
    return pfn ? pfn(webView) : false;
}

bool wkeIsLoadComplete(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsLoadComplete);
    return pfn ? pfn(webView) : false;
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
    pfn ? pfn(webView) : pfn;
}

void wkeReload(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeReload);
    pfn ? pfn(webView) : pfn;
}

const utf8 *wkeGetSource(wkeWebView webView)
{
    typedef const utf8 *(*FUN)(wkeWebView);
    LOADFUN(wkeGetSource);
    return pfn ? pfn(webView) : NULL;
}

const utf8 *wkeGetTitleA(wkeWebView webView)
{
    typedef const utf8 *(*FUN)(wkeWebView);
    LOADFUN(wkeGetTitle);
    return pfn ? pfn(webView) : NULL;
}

const wchar_t *wkeGetTitleW(wkeWebView webView)
{
    typedef const wchar_t *(*FUN)(wkeWebView);
    LOADFUN(wkeGetTitleW);
    return pfn ? pfn(webView) : NULL;
}

void wkeResize(wkeWebView webView, int w, int h)
{
    typedef void(*FUN)(wkeWebView, int, int);
    LOADFUN(wkeResize);
    pfn ? pfn(webView, w, h) : pfn;
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

void wkeEditorSelectAll(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorSelectAll);
    pfn ? pfn(webView) : pfn;
}

void wkeEditorUnSelect(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorUnSelect);
    pfn ? pfn(webView) : pfn;
}

void wkeEditorCopy(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorCopy);
    pfn ? pfn(webView) : pfn;
}

void wkeEditorCut(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorCut);
    pfn ? pfn(webView) : pfn;
}

void wkeEditorPaste(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorPaste);
    pfn ? pfn(webView) : pfn;
}

void wkeEditorDelete(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorDelete);
    pfn ? pfn(webView) : pfn;
}

void wkeEditorUndo(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorUndo);
    pfn ? pfn(webView) : pfn;
}

void wkeEditorRedo(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeEditorRedo);
    pfn ? pfn(webView) : pfn;
}

void wkeSetEditable(wkeWebView webView, bool editable)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetEditable);
    pfn ? pfn(webView, editable) : pfn;
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

bool wkeFireKeyDownEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags,
                         bool systemKey)
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

bool wkeFireWindowsMessage(wkeWebView webView, HWND hWnd, UINT message,
                           WPARAM wParam, LPARAM lParam, LRESULT *result)
{
    typedef bool(*FUN)(wkeWebView, HWND, UINT, WPARAM, LPARAM, LRESULT *);
    LOADFUN(wkeFireWindowsMessage);
    return pfn ? pfn(webView, hWnd, message, wParam, lParam, result) : false;
}

void wkeSetFocus(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeSetFocus);
    pfn ? pfn(webView) : pfn;
}

void wkeKillFocus(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeKillFocus);
    pfn ? pfn(webView) : pfn;
}

wkeRect wkeGetCaretRect(wkeWebView webView)
{
    typedef wkeRect(*FUN)(wkeWebView);
    LOADFUN(wkeGetCaret);
    return pfn ? pfn(webView) : wkeRect { 0, 0, 0, 0 };
}

void wkeSetClientHandler(wkeWebView webView, const wkeClientHandler *handler)
{
    typedef void(*FUN)(wkeWebView, const wkeClientHandler *);
    LOADFUN(wkeSetClientHandler);
    pfn ? pfn(webView, handler) : pfn;
}

const wkeClientHandler *wkeGetClientHandler(wkeWebView webView)
{
    typedef const wkeClientHandler*(*FUN)(wkeWebView);
    LOADFUN(wkeGetClientHandler);
    return pfn ? pfn(webView) : NULL;
}

void wkeSetMemoryCacheEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetMemoryCacheEnable);
    pfn ? pfn(webView, b) : pfn;
}

void wkeSetMouseEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetMouseEnabled);
    pfn ? pfn(webView, b) : pfn;
}

void wkeSetTouchEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetTouchEnabled);
    pfn ? pfn(webView, b) : pfn;
}

void wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetNavigationToNewWindowEnable);
    pfn ? pfn(webView, b) : pfn;
}

void wkeSetCspCheckEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetCspCheckEnable);
    pfn ? pfn(webView, b) : pfn;
}

void wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetNpapiPluginsEnabled);
    pfn ? pfn(webView, b) : pfn;
}

void wkeSetHeadlessEnabled(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetHeadlessEnabled);
    pfn ? pfn(webView, b) : pfn;
}

void wkeGetCursorInfoType(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeGetCursorInfoType);
    pfn ? pfn(webView) : pfn;
}

void wkeSetDragEnable(wkeWebView webView, bool b)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetDragEnable);
    pfn ? pfn(webView, b) : pfn;
}

void wkeSetDragFiles(wkeWebView webView, const POINT *clintPos, const POINT *screenPos,
                     wkeString files[ ], int filesCount)
{
    typedef void(*FUN)(wkeWebView, const POINT *, const POINT *, wkeString[ ], int);
    LOADFUN(wkeSetDragFiles);
    pfn ? pfn(webView, clintPos, screenPos, files, filesCount) : pfn;
}

void wkeSetViewNetInterface(wkeWebView webView, const char *netInterface)
{
    typedef void(*FUN)(wkeWebView, const char *);
    LOADFUN(wkeSetViewNetInterface);
    pfn ? pfn(webView, netInterface) : pfn;
}

void wkeSetProxy(const wkeProxy *proxy)
{
    typedef void(*FUN)(const wkeProxy *);
    LOADFUN(wkeSetProxy);
    pfn ? pfn(proxy) : pfn;
}

void wkeSetViewProxy(wkeWebView webView, wkeProxy *proxy)
{
    typedef void(*FUN)(wkeWebView, wkeProxy *);
    LOADFUN(wkeSetViewProxy);
    pfn ? pfn(webView, proxy) : pfn;
}

void wkeSetHandle(wkeWebView webView, HWND wnd)
{
    typedef void(*FUN)(wkeWebView, HWND);
    LOADFUN(wkeSetHandle);
    pfn ? pfn(webView, wnd) : pfn;
}

void wkeSetHandleOffset(wkeWebView webView, int x, int y)
{
    typedef void(*FUN)(wkeWebView, int, int);
    LOADFUN(wkeSetHandleOffset);
    pfn ? pfn(webView, x, y) : pfn;
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
    pfn ? pfn(webView, transparent) : pfn;
}

void wkeSetUserAgentA(wkeWebView webView, const utf8 *userAgent)
{
    typedef void(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeSetUserAgent);
    pfn ? pfn(webView, userAgent) : pfn;
}

void wkeSetUserAgentW(wkeWebView webView, const wchar_t *userAgent)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetUserAgentW);
    pfn ? pfn(webView, userAgent) : pfn;
}

void wkeLoadURLA(wkeWebView webView, const utf8 *url)
{
    typedef void(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeLoadURL);
    pfn ? pfn(webView, url) : pfn;
}

void wkeLoadURLW(wkeWebView webView, const wchar_t *url)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeLoadURLW);
    pfn ? pfn(webView, url) : pfn;
}

void wkePostURLA(wkeWebView wkeView, const utf8 *url, const char *postData, int postLen)
{
    typedef void(*FUN)(wkeWebView, const utf8 *, const char *, int);
    LOADFUN(wkePostURL);
    pfn ? pfn(wkeView, url, postData, postLen) : pfn;
}

void wkePostURLW(wkeWebView wkeView, const wchar_t *url, const char *postData, int postLen)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *, const char *, int);
    LOADFUN(wkePostURLW);
    pfn ? pfn(wkeView, url, postData, postLen) : pfn;
}

void wkeLoadHTMLA(wkeWebView webView, const utf8 *html)
{
    typedef void(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeLoadHTML);
    pfn ? pfn(webView, html) : pfn;
}

void wkeLoadHTMLW(wkeWebView webView, const wchar_t *html)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeLoadHTMLW);
    pfn ? pfn(webView, html) : pfn;
}

void wkeLoadFileA(wkeWebView webView, const utf8 *filename)
{
    typedef void(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeLoadFile);
    pfn ? pfn(webView, filename) : pfn;
}

void wkeLoadFileW(wkeWebView webView, const wchar_t *filename)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeLoadFileW);
    pfn ? pfn(webView, filename) : pfn;
}

const utf8 *wkeGetURLA(wkeWebView webView)
{
    typedef const utf8 *(*FUN)(wkeWebView);
    LOADFUN(wkeGetURL);
    return pfn ? pfn(webView) : NULL;
}

const wchar_t *wkeGetURLW(wkeWebView webView)
{
    typedef const wchar_t *(*FUN)(wkeWebView);
    LOADFUN(wkeGetURLW);
    return pfn ? pfn(webView) : NULL;
}

void wkeSetDirty(wkeWebView webView, bool dirty)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetDirty);
    pfn ? pfn(webView, dirty) : pfn;
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
    pfn ? pfn(webView, x, y, w, h) : pfn;
}

void wkeLayoutIfNeeded(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeLayoutIfNeeded);
    pfn ? pfn(webView) : pfn;
}

void wkePaint(wkeWebView webView, void *bits, int pitch)
{
    typedef void(*FUN)(wkeWebView, void *, int);
    LOADFUN(wkePaint2);
    pfn ? pfn(webView, bits, pitch) : pfn;
}

void wkePaint2(wkeWebView webView, void *bits, int bufW, int bufH, int xDst, int yDst,
               int w, int h, int xSrc, int ySrc, bool bCopyAlpha)
{
    typedef void(*FUN)(wkeWebView, void *, int, int, int, int, int, int, int, int, bool);
    LOADFUN(wkePaint);
    pfn ? pfn(webView, bits, bufW, bufH, xDst, yDst, w, h, xSrc, ySrc, bCopyAlpha) : pfn;
}

bool wkeRepaintIfNeeded(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeRepaintIfNeeded);
    return pfn ? pfn(webView) : false;
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

const utf8 *wkeGetCookieA(wkeWebView webView)
{
    typedef utf8*(*FUN)(wkeWebView);
    LOADFUN(wkeGetCookie);
    return pfn ? pfn(webView) : NULL;
}

const wchar_t *wkeGetCookieW(wkeWebView webView)
{
    typedef wchar_t *(*FUN)(wkeWebView);
    LOADFUN(wkeGetCookieW);
    return pfn ? pfn(webView) : NULL;
}

bool wkeIsCookieEnabled(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsCookieEnabled);
    return pfn ? pfn(webView) : false;
}

void wkeSetCookieEnabled(wkeWebView webView, bool enable)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeSetCookieEnabled);
    pfn ? pfn(webView, enable) : pfn;
}

void wkeSetCookieA(wkeWebView webView, const utf8 *url, const utf8 *cookie)
{
    typedef void(*FUN)(wkeWebView, const utf8 *, const utf8 *);
    LOADFUN(wkeSetCookie);
    pfn ? pfn(webView, url, cookie) : pfn;
}

void wkeSetCookieW(wkeWebView webView, const wchar_t *url, const wchar_t *cookie)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *, const wchar_t *);
    LOADFUN(wkeSetCookieW);
    pfn ? pfn(webView, url, cookie) : pfn;
}

void wkeVisitAllCookie(void *params, wkeCookieVisitor visitor)
{
    typedef void(*FUN)(void *, wkeCookieVisitor);
    LOADFUN(wkeVisitAllCookie);
    pfn ? pfn(params, visitor) : pfn;
}

void wkePerformCookieCommand(wkeCookieCommand command)
{
    typedef void(*FUN)(wkeCookieCommand);
    LOADFUN(wkePerformCookieCommand);
    pfn ? pfn(command) : pfn;
}

void wkeSetCookieJarPathA(wkeWebView webView, const utf8 *path)
{
    // typedef void(*FUN)(wkeWebView, const utf8 *);
    // LOADFUN(wkeSetCookieJarPath);
    // pfn ? pfn(webView, path) : pfn;
}

void wkeSetCookieJarPathW(wkeWebView webView, const wchar_t *path)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetCookieJarPath);
    pfn ? pfn(webView, path) : pfn;
}

void wkeSetCookieJarFullPathA(wkeWebView webView, const utf8 *path)
{
    //typedef void(*FUN)(wkeWebView, const utf8 *);
    //LOADFUN(wkeSetCookieJarFullPath);
    //pfn ? pfn(webView, path) : pfn;
}

void wkeSetCookieJarFullPathW(wkeWebView webView, const wchar_t *path)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetCookieJarFullPath);
    pfn ? pfn(webView, path) : pfn;
}

void wkeSetLocalStorageFullPathA(wkeWebView webView, const utf8 *path)
{
    //typedef void(*FUN)(wkeWebView, const utf8 *);
    //LOADFUN(wkeSetLocalStorageFullPath);
    //pfn ? pfn(webView, path) : pfn;
}

void wkeSetLocalStorageFullPathW(wkeWebView webView, const wchar_t *path)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetLocalStorageFullPath);
    pfn ? pfn(webView, path) : pfn;
}

void wkeAddPluginDirectoryA(wkeWebView webView, const utf8 *path)
{
    //typedef void(*FUN)(wkeWebView, const utf8 *);
    //LOADFUN(wkeAddPluginDirectory);
    //pfn ? pfn(webView, path) : pfn;
}

void wkeAddPluginDirectoryW(wkeWebView webView, const wchar_t *path)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeAddPluginDirectory);
    pfn ? pfn(webView, path) : pfn;
}

float wkeGetMediaVolume(wkeWebView webView)
{
    typedef float(*FUN)(wkeWebView);
    LOADFUN(wkeGetMediaVolume);
    return pfn ? pfn(webView) : 0.0f;
}

void wkeSetMediaVolume(wkeWebView webView, float volume)
{
    typedef void(*FUN)(wkeWebView, float);
    LOADFUN(wkeSetMediaVolume);
    pfn ? pfn(webView, volume) : pfn;
}

float wkeGetZoomFactor(wkeWebView webView)
{
    typedef float(*FUN)(wkeWebView);
    LOADFUN(wkeGetZoomFactor);
    return pfn ? pfn(webView) : 0.0f;
}

void wkeSetZoomFactor(wkeWebView webView, float factor)
{
    typedef void(*FUN)(wkeWebView, float);
    LOADFUN(wkeSetZoomFactor);
    pfn ? pfn(webView, factor) : pfn;
}

void wkeSetStringA(wkeString *string, const utf8 *str, size_t len)
{
    typedef void(*FUN)(wkeString *, const utf8 *, size_t);
    LOADFUN(wkeSetString);
    pfn ? pfn(string, str, len) : pfn;
}

void wkeSetStringW(wkeString *string, const wchar_t *str, size_t len)
{
    typedef void(*FUN)(wkeString *, const wchar_t *, size_t);
    LOADFUN(wkeSetStringW);
    pfn ? pfn(string, str, len) : pfn;
}

const utf8 *wkeGetStringA(const wkeString string)
{
    typedef const utf8 *(*FUN)(const wkeString);
    LOADFUN(wkeGetString);
    return pfn ? pfn(string) : NULL;
}

const wchar_t *wkeGetStringW(const wkeString string)
{
    typedef const wchar_t *(*FUN)(const wkeString);
    LOADFUN(wkeGetStringW);
    return pfn ? pfn(string) : NULL;
}

wkeString wkeCreateStringA(const utf8 *str, size_t len)
{
    typedef wkeString(*FUN)(const utf8 *, size_t);
    LOADFUN(wkeCreateString);
    return pfn ? pfn(str, len) : NULL;
}

wkeString wkeCreateStringW(const wchar_t *str, size_t len)
{
    typedef wkeString(*FUN)(const wchar_t *, size_t);
    LOADFUN(wkeCreateStringW);
    return pfn ? pfn(str, len) : NULL;
}

void wkeDeleteString(wkeString str)
{
    typedef void(*FUN)(wkeString);
    LOADFUN(wkeDeleteString);
    pfn ? pfn(str) : pfn;
}

void wkeSleep(wkeWebView webView)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeSleep);
    pfn ? pfn(webView) : pfn;
}

bool wkeAwake(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeAwake);
    return pfn ? pfn(webView) : false;
}

bool wkeIsAwake(wkeWebView webView)
{
    typedef bool(*FUN)(wkeWebView);
    LOADFUN(wkeIsAwake);
    return pfn ? pfn(webView) : false;
}

void wkeSetUserKeyValue(wkeWebView webView, const char *key, void *value)
{
    typedef void(*FUN)(wkeWebView, const char *key, void *);
    LOADFUN(wkeSetUserKeyValue);
    pfn ? pfn(webView, key, value) : pfn;
}

void *wkeGetUserKeyValue(wkeWebView webView, const char *key)
{
    typedef void *(*FUN)(wkeWebView, const char *key);
    LOADFUN(wkeGetUserKeyValue);
    return pfn ? pfn(webView, key) : NULL;
}

void wkeSetDeviceParameter(wkeWebView webView, const char *device, const char *paramStr, int paramInt,
                           float paramFloat)
{
    typedef void(*FUN)(wkeWebView, const char *, const char *, int, float);
    LOADFUN(wkeSetDeviceParameter);
    pfn ? pfn(webView, device, paramStr, paramInt, paramFloat) : pfn;
}

wkeTempCallbackInfo wkeGetTempCallbackInfo(wkeWebView webView)
{
    typedef wkeTempCallbackInfo(*FUN)(wkeWebView);
    LOADFUN(wkeGetTempCallbackInfo);
    return pfn ? pfn(webView) : wkeTempCallbackInfo();
}

void wkeDeleteWillSendRequestInfo(wkeWebView webWindow, wkeWillSendRequestInfo *info)
{
    typedef void(*FUN)(wkeWebView, wkeWillSendRequestInfo *);
    LOADFUN(wkeDeleteWillSendRequestInfo);
    pfn ? pfn(webWindow, info) : pfn;
}

void wkeNetSetMIMEType(void *job, char *type)
{
    typedef void(*FUN)(void *, char *);
    LOADFUN(wkeNetSetMIMEType);
    pfn ? pfn(job, type) : pfn;
}

void wkeNetGetMIMEType(void *job, wkeString mime)
{
    typedef void(*FUN)(void *, wkeString);
    LOADFUN(wkeNetGetMIMEType);
    pfn ? pfn(job, mime) : pfn;
}

void wkeNetSetHTTPHeaderField(void *job, wchar_t *key, wchar_t *value, bool response)
{
    typedef void(*FUN)(void *, wchar_t *, wchar_t *, bool);
    LOADFUN(wkeNetSetHTTPHeaderField);
    pfn ? pfn(job, key, value, response) : pfn;
}

const char *wkeNetGetHTTPHeaderField(void *job, const char *key)
{
    typedef const char *(*FUN)(void *, const char *);
    LOADFUN(wkeNetGetHTTPHeaderField);
    return pfn ? pfn(job, key) : NULL;
}

void wkeNetSetURL(void *job, const char *url)
{
    typedef void(*FUN)(void *, const char *);
    LOADFUN(wkeNetSetURL);
    return pfn ? pfn(job, url) : NULL;
}

void wkeNetSetData(void *job, void *buf, int len)
{
    typedef void(*FUN)(void *, void *, int);
    LOADFUN(wkeNetSetData);
    pfn ? pfn(job, buf, len) : pfn;
}

void wkeNetHookRequest(void *job)
{
    typedef void(*FUN)(void *);
    LOADFUN(wkeNetHookRequest);
    pfn ? pfn(job) : pfn;
}

void wkeNetOnResponse(wkeWebView webView, wkeNetResponseCallback callback, void *param)
{
    typedef void(*FUN)(wkeWebView, wkeNetResponseCallback, void *);
    LOADFUN(wkeNetOnResponse);
    pfn ? pfn(webView, callback, param) : pfn;
}

wkePostBodyElements *wkeNetGetPostBody(void *jobPtr)
{
    typedef wkePostBodyElements *(*FUN)(void *);
    LOADFUN(wkeNetGetPostBody);
    return pfn ? pfn(jobPtr) : NULL;
}

wkeRequestType wkeNetGetRequestMethod(void *jobPtr)
{
    typedef wkeRequestType(*FUN)(void *);
    LOADFUN(wkeRequestType);
    return pfn ? pfn(jobPtr) : kWkeRequestTypeInvalidation;
}

void wkeNetContinueJob(void *jobPtr)
{
    typedef void(*FUN)(void *);
    LOADFUN(wkeNetContinueJob);
    pfn ? pfn(jobPtr) : pfn;
}

const char *wkeNetGetUrlByJob(void *jobPtr)
{
    typedef const char *(*FUN)(void *);
    LOADFUN(wkeNetGetUrlByJob);
    return pfn ? pfn(jobPtr) : NULL;
}

void wkeNetCancelRequest(void *jobPtr)
{
    typedef void(*FUN)(void *);
    LOADFUN(wkeNetCancelRequest);
    pfn ? pfn(jobPtr) : pfn;
}

void wkeNetChangeRequestUrl(void *jobPtr, const char *url)
{
    typedef void(*FUN)(void *, const char *);
    LOADFUN(wkeNetChangeRequestUrl);
    pfn ? pfn(jobPtr, url) : pfn;
}

void wkeNetHoldJobToAsynCommit(void *jobPtr)
{
    typedef void(*FUN)(void *);
    LOADFUN(wkeNetHoldJobToAsynCommit);
    pfn ? pfn(jobPtr) : pfn;
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
    pfn ? pfn(element) : pfn;
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
    pfn ? pfn(element) : pfn;
}

wkeMemBuf *wkeCreateMemBuf(wkeWebView webView, void *buf, size_t length)
{
    typedef wkeMemBuf*(*FUN)(wkeWebView, void *, size_t);
    LOADFUN(wkeCreateMemBuf);
    return pfn ? pfn(webView, buf, length) : NULL;
}

void wkeFreeMemBuf(wkeMemBuf *buffer)
{
    typedef void(*FUN)(wkeMemBuf *);
    LOADFUN(wkeFreeMemBuf);
    pfn ? pfn(buffer) : pfn;
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

void wkeWebFrameGetMainWorldScriptContext(wkeWebView webView, wkeWebFrameHandle webFrameId,
                                          v8ContextPtr contextOut)
{
    typedef void(*FUN)(wkeWebView, wkeWebFrameHandle, v8ContextPtr);
    LOADFUN(wkeWebFrameGetMainWorldScriptContext);
    pfn ? pfn(webView, webFrameId, contextOut) : pfn;
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

void wkeDestroyWebWindow(wkeWebView webWindow)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeDestroyWebWindow);
    pfn ? pfn(webWindow) : pfn;
}

void *wkeGetWindowHandle(wkeWebView webWindow)
{
    typedef void *(*FUN)(wkeWebView);
    LOADFUN(wkeGetWindowHandle);
    return pfn ? pfn(webWindow) : NULL;
}

void wkeShowWindow(wkeWebView webWindow, bool show)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeShowWindow);
    pfn ? pfn(webWindow, show) : pfn;
}

void wkeEnableWindow(wkeWebView webWindow, bool enable)
{
    typedef void(*FUN)(wkeWebView, bool);
    LOADFUN(wkeEnableWindow);
    pfn ? pfn(webWindow, enable) : pfn;
}

void wkeMoveWindow(wkeWebView webWindow, int x, int y, int width, int height)
{
    typedef void(*FUN)(wkeWebView, int, int, int, int);
    LOADFUN(wkeMoveWindow);
    pfn ? pfn(webWindow, x, y, width, height) : pfn;
}

void wkeMoveToCenter(wkeWebView webWindow)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeMoveToCenter);
    pfn ? pfn(webWindow) : pfn;
}

void wkeResizeWindow(wkeWebView webWindow, int width, int height)
{
    typedef void(*FUN)(wkeWebView, int, int);
    LOADFUN(wkeResizeWindow);
    pfn ? pfn(webWindow, width, height) : pfn;
}

wkeWebDragOperation wkeDragTargetDragEnter(wkeWebView webWindow, const wkeWebDragData *webDragData,
                                           const POINT *clientPoint, const POINT *screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers)
{
    typedef wkeWebDragOperation(*FUN)(wkeWebView, const wkeWebDragData *, const POINT *, const POINT *,
                                      wkeWebDragOperationsMask, int);
    LOADFUN(wkeDragTargetDragEnter);
    return pfn ? pfn(webWindow, webDragData, clientPoint, screenPoint, operationsAllowed, modifiers)
           : wkeWebDragOperationNone;
}

wkeWebDragOperation wkeDragTargetDragOver(wkeWebView webWindow, const POINT *clientPoint,
                                          const POINT *screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers)
{
    typedef wkeWebDragOperation(*FUN)(wkeWebView, const POINT *, const POINT *, wkeWebDragOperationsMask, int);
    LOADFUN(wkeDragTargetDragOver);
    return pfn ? pfn(webWindow, clientPoint, screenPoint, operationsAllowed, modifiers) : wkeWebDragOperationNone;
}

void wkeDragTargetDragLeave(wkeWebView webWindow)
{
    typedef void(*FUN)(wkeWebView);
    LOADFUN(wkeDragTargetDragLeave);
    pfn ? pfn(webWindow) : pfn;
}

void wkeDragTargetDrop(wkeWebView webWindow, const POINT *clientPoint, const POINT *screenPoint,
                       int modifiers)
{
    typedef void(*FUN)(wkeWebView, const POINT *, const POINT *, int);
    LOADFUN(wkeDragTargetDrop);
    pfn ? pfn(webWindow, clientPoint, screenPoint, modifiers) : pfn;
}

void wkeSetWindowTitleA(wkeWebView webWindow, const utf8 *title)
{
    typedef void(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeSetWindowTitle);
    pfn ? pfn(webWindow, title) : pfn;
}

void wkeSetWindowTitleW(wkeWebView webWindow, const wchar_t *title)
{
    typedef void(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeSetWindowTitleW);
    pfn ? pfn(webWindow, title) : pfn;
}

//////////////////////////////////////////////////////////////////////////
// 以下是C/C++ 与 JS 互调相关 API

jsValue wkeRunJSA(wkeWebView webView, const utf8 *script)
{
    typedef jsValue(*FUN)(wkeWebView, const utf8 *);
    LOADFUN(wkeRunJS);
    return pfn ? pfn(webView, script) : -1;
}

jsValue wkeRunJSW(wkeWebView webView, const wchar_t *script)
{
    typedef jsValue(*FUN)(wkeWebView, const wchar_t *);
    LOADFUN(wkeRunJSW);
    return pfn ? pfn(webView, script) : -1;
}

jsValue wkeRunJsByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const utf8 *script, bool isInClosure)
{
    typedef jsValue(*FUN)(wkeWebView, wkeWebFrameHandle, const utf8 *, bool);
    LOADFUN(wkeRunJsByFrame);
    return pfn ? pfn(webView, frameId, script, isInClosure) : -1;
}

jsExecState wkeGlobalExec(wkeWebView webView)
{
    typedef jsExecState(*FUN)(wkeWebView);
    LOADFUN(wkeGlobalExec);
    return pfn ? pfn(webView) : NULL;
}

void jsBindFunction(const char *name, jsNativeFunction fn, unsigned int argCount)
{
    typedef void(*FUN)(const char *, jsNativeFunction, unsigned int);
    LOADFUN(jsBindFunction);
    pfn ? pfn(name, fn, argCount) : pfn;
}

void jsBindGetter(const char *name, jsNativeFunction fn)
{
    typedef void(*FUN)(const char *, jsNativeFunction);
    LOADFUN(jsBindGetter);
    pfn ? pfn(name, fn) : pfn;
}

void jsBindSetter(const char *name, jsNativeFunction fn)
{
    typedef void(*FUN)(const char *, jsNativeFunction);
    LOADFUN(jsBindSetter);
    pfn ? pfn(name, fn) : pfn;
}

void jsBindFunctionEx(const char *name, wkeJsNativeFunction fn, void *param, unsigned int argCount)
{
    typedef void(*FUN)(const char *, wkeJsNativeFunction, void *, unsigned int);
    LOADFUN(wkeJsBindFunction);
    pfn ? pfn(name, fn, param, argCount) : pfn;
}

void jsBindGetterEx(const char *name, wkeJsNativeFunction fn, void *param)
{
    typedef void(*FUN)(const char *, wkeJsNativeFunction, void *);
    LOADFUN(wkeJsBindGetter);
    pfn ? pfn(name, fn, param) : pfn;
}

void jsBindSetterEx(const char *name, wkeJsNativeFunction fn, void *param)
{
    typedef void(*FUN)(const char *, wkeJsNativeFunction, void *);
    LOADFUN(wkeJsBindSetter);
    pfn ? pfn(name, fn, param) : pfn;
}

int jsArgCount(jsExecState es)
{
    typedef int(*FUN)(jsExecState);
    LOADFUN(jsArgCount);
    return pfn ? pfn(es) : -1;
}

jsType jsArgType(jsExecState es, int index)
{
    typedef jsType(*FUN)(jsExecState, int);
    LOADFUN(jsArgType);
    return pfn ? pfn(es, index) : JSTYPE_UNDEFINED;
}

jsValue jsArgValue(jsExecState es, int index)
{
    typedef jsValue(*FUN)(jsExecState, int);
    LOADFUN(jsArg);
    return pfn ? pfn(es, index) : -1;
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

float jsToFloat(jsExecState es, jsValue v)
{
    typedef float(*FUN)(jsExecState, jsValue);
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

const utf8 *jsToTempStringA(jsExecState es, jsValue v)
{
    typedef const utf8 *(*FUN)(jsExecState, jsValue);
    LOADFUN(jsToTempString);
    return pfn ? pfn(es, v) : NULL;
}

const wchar_t *jsToTempStringW(jsExecState es, jsValue v)
{
    typedef const wchar_t *(*FUN)(jsExecState, jsValue);
    LOADFUN(jsToTempStringW);
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

jsValue jsUndefined(jsExecState es)
{
    typedef jsValue(*FUN)(jsExecState);
    LOADFUN(jsUndefined);
    return pfn ? pfn(es) : -1;
}

jsValue jsNull(jsExecState es)
{
    typedef jsValue(*FUN)(jsExecState);
    LOADFUN(jsNull);
    return pfn ? pfn(es) : -1;
}

jsValue jsTrue(jsExecState es)
{
    typedef jsValue(*FUN)(jsExecState);
    LOADFUN(jsTrue);
    return pfn ? pfn(es) : -1;
}

jsValue jsFalse(jsExecState es)
{
    typedef jsValue(*FUN)(jsExecState);
    LOADFUN(jsFalse);
    return pfn ? pfn(es) : -1;
}

jsValue jsStringA(jsExecState es, const utf8 *str)
{
    typedef jsValue(*FUN)(jsExecState, const utf8 *);
    LOADFUN(jsString);
    return pfn ? pfn(es, str) : -1;
}

jsValue jsStringW(jsExecState es, const wchar_t *str)
{
    typedef jsValue(*FUN)(jsExecState, const wchar_t *);
    LOADFUN(jsStringW);
    return pfn ? pfn(es, str) : -1;
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

jsValue jsArrayBuffer(jsExecState es, char *buffer, size_t size)
{
    typedef jsValue(*FUN)(jsExecState, char *, size_t);
    LOADFUN(jsArrayBuffer);
    return pfn ? pfn(es, buffer, size) : -1;
}

jsValue jsObject(jsExecState es, jsData *obj)
{
    typedef jsValue(*FUN)(jsExecState, jsData *);
    LOADFUN(jsObject);
    return pfn ? pfn(es, obj) : -1;
}

jsValue jsSetObjData(jsExecState es, jsData *obj)
{
    typedef jsValue(*FUN)(jsExecState, jsData *);
    LOADFUN(jsFunction);
    return pfn ? pfn(es, obj) : -1;
}

jsData *jsGetObjData(jsExecState es, jsValue obj)
{
    typedef jsData*(*FUN)(jsExecState, jsValue);
    LOADFUN(jsGetData);
    return pfn ? pfn(es, obj) : NULL;
}

jsValue jsGet(jsExecState es, jsValue obj, const char *prop)
{
    typedef jsValue(*FUN)(jsExecState, jsValue, const char *);
    LOADFUN(jsGet);
    return pfn ? pfn(es, obj, prop) : -1;
}

void jsSet(jsExecState es, jsValue obj, const char *prop, jsValue v)
{
    typedef void(*FUN)(jsExecState, jsValue, const char *, jsValue);
    LOADFUN(jsSet);
    pfn ? pfn(es, obj, prop, v) : pfn;
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
    pfn ? pfn(es, obj, index, v) : pfn;
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
    pfn ? pfn(es, obj, length) : pfn;
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

jsValue jsEvalA(jsExecState es, const utf8 *str)
{
    typedef jsValue(*FUN)(jsExecState, const utf8 *);
    LOADFUN(jsEval);
    return pfn ? pfn(es, str) : -1;
}

jsValue jsEvalW(jsExecState es, const wchar_t *str)
{
    typedef jsValue(*FUN)(jsExecState, const wchar_t *);
    LOADFUN(jsEvalW);
    return pfn ? pfn(es, str) : -1;
}

jsValue jsEvalExW(jsExecState es, const wchar_t *str, bool isInClosure)
{
    typedef jsValue(*FUN)(jsExecState, const wchar_t *, bool);
    LOADFUN(jsEvalExW);
    return pfn ? pfn(es, str, isInClosure) : -1;
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

jsValue jsGetGlobal(jsExecState es, const char *prop)
{
    typedef jsValue(*FUN)(jsExecState, const char *);
    LOADFUN(jsGetGlobal);
    return pfn ? pfn(es, prop) : -1;
}

void jsSetGlobal(jsExecState es, const char *prop, jsValue v)
{
    typedef void(*FUN)(jsExecState, const char *, jsValue);
    LOADFUN(jsSetGlobal);
    pfn ? pfn(es, prop, v) : pfn;
}

void jsGC()
{
    typedef void(*FUN)();
    LOADFUN(jsGC);
    pfn ? pfn() : pfn;
}


}