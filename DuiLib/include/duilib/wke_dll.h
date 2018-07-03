#ifndef __WKE_DLL_H__
#define __WKE_DLL_H__
#pragma once

namespace DuiLib {


typedef struct
{
    int x;
    int y;
    int w;
    int h;
} wkeRect;

typedef enum
{
    WKE_LBUTTON = 0x01,
    WKE_RBUTTON = 0x02,
    WKE_SHIFT = 0x04,
    WKE_CONTROL = 0x08,
    WKE_MBUTTON = 0x10,
} wkeMouseFlags;

typedef enum
{
    WKE_EXTENDED = 0x0100,
    WKE_REPEAT = 0x4000,
} wkeKeyFlags;

typedef enum
{
    WKE_MSG_MOUSEMOVE = 0x0200,
    WKE_MSG_LBUTTONDOWN = 0x0201,
    WKE_MSG_LBUTTONUP = 0x0202,
    WKE_MSG_LBUTTONDBLCLK = 0x0203,
    WKE_MSG_RBUTTONDOWN = 0x0204,
    WKE_MSG_RBUTTONUP = 0x0205,
    WKE_MSG_RBUTTONDBLCLK = 0x0206,
    WKE_MSG_MBUTTONDOWN = 0x0207,
    WKE_MSG_MBUTTONUP = 0x0208,
    WKE_MSG_MBUTTONDBLCLK = 0x0209,
    WKE_MSG_MOUSEWHEEL = 0x020A,
} wkeMouseMsg;



#if !defined(__cplusplus)
    #ifndef HAVE_WCHAR_T
        typedef unsigned short wchar_t;
    #endif
#endif

#include <stdbool.h>

typedef char utf8;

#if !defined(__cplusplus)
    typedef void *jsExecState;
#else
    struct JsExecStateInfo;
    typedef JsExecStateInfo *jsExecState;
#endif

typedef __int64 jsValue;

struct _tagWkeWebView;
typedef struct _tagWkeWebView *wkeWebView;

struct _tagWkeString;
typedef struct _tagWkeString *wkeString;

typedef enum
{
    WKE_PROXY_NONE,
    WKE_PROXY_HTTP,
    WKE_PROXY_SOCKS4,
    WKE_PROXY_SOCKS4A,
    WKE_PROXY_SOCKS5,
    WKE_PROXY_SOCKS5HOSTNAME
} wkeProxyType;

typedef struct
{
    wkeProxyType type;
    char hostname[100];
    unsigned short port;
    char username[50];
    char password[50];
} wkeProxy;

enum wkeSettingMask
{
    WKE_SETTING_PROXY = 1,
    WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD = 1 << 2,
};

typedef struct
{
    wkeProxy proxy;
    unsigned int mask;
} wkeSettings;

typedef struct
{
    int size;
    unsigned int bgColor;
} wkeViewSettings;

typedef void *wkeWebFrameHandle;


typedef void *(*WKE_FILE_OPEN)(const char *path);
typedef void(*WKE_FILE_CLOSE)(void *handle);
typedef size_t(*WKE_FILE_SIZE)(void *handle);
typedef int(*WKE_FILE_READ)(void *handle, void *buffer, size_t size);
typedef int(*WKE_FILE_SEEK)(void *handle, int offset, int origin);
typedef bool(*WKE_EXISTS_FILE)(const char *path);

struct _wkeClientHandler; // declare warning fix
typedef void(*ON_TITLE_CHANGED)(const struct _wkeClientHandler *clientHandler, const wkeString title);
typedef void(*ON_URL_CHANGED)(const struct _wkeClientHandler *clientHandler, const wkeString url);

typedef struct _wkeClientHandler
{
    ON_TITLE_CHANGED onTitleChanged;
    ON_URL_CHANGED onURLChanged;
} wkeClientHandler;


typedef bool(*wkeCookieVisitor)(
    void *params,
    const char *name,
    const char *value,
    const char *domain,
    const char *path, // If |path| is non-empty only URLs at or below the path will get the cookie value.
    int secure, // If |secure| is true the cookie will only be sent for HTTPS requests.
    int httpOnly, // If |httponly| is true the cookie will only be sent for HTTP requests.
    int *expires // The cookie expiration date is only valid if |has_expires| is true.
);

typedef enum
{
    wkeCookieCommandClearAllCookies,
    wkeCookieCommandClearSessionCookies,
    wkeCookieCommandFlushCookiesToFile,
    wkeCookieCommandReloadCookiesFromFile,
} wkeCookieCommand;

typedef enum
{
    WKE_NAVIGATION_TYPE_LINKCLICK,
    WKE_NAVIGATION_TYPE_FORMSUBMITTE,
    WKE_NAVIGATION_TYPE_BACKFORWARD,
    WKE_NAVIGATION_TYPE_RELOAD,
    WKE_NAVIGATION_TYPE_FORMRESUBMITT,
    WKE_NAVIGATION_TYPE_OTHER
} wkeNavigationType;

typedef enum
{
    WkeCursorInfoPointer,
    WkeCursorInfoCross,
    WkeCursorInfoHand,
    WkeCursorInfoIBeam,
    WkeCursorInfoWait,
    WkeCursorInfoHelp,
    WkeCursorInfoEastResize,
    WkeCursorInfoNorthResize,
    WkeCursorInfoNorthEastResize,
    WkeCursorInfoNorthWestResize,
    WkeCursorInfoSouthResize,
    WkeCursorInfoSouthEastResize,
    WkeCursorInfoSouthWestResize,
    WkeCursorInfoWestResize,
    WkeCursorInfoNorthSouthResize,
    WkeCursorInfoEastWestResize,
    WkeCursorInfoNorthEastSouthWestResize,
    WkeCursorInfoNorthWestSouthEastResize,
    WkeCursorInfoColumnResize,
    WkeCursorInfoRowResize,
    WkeCursorInfoMiddlePanning,
    WkeCursorInfoEastPanning,
    WkeCursorInfoNorthPanning,
    WkeCursorInfoNorthEastPanning,
    WkeCursorInfoNorthWestPanning,
    WkeCursorInfoSouthPanning,
    WkeCursorInfoSouthEastPanning,
    WkeCursorInfoSouthWestPanning,
    WkeCursorInfoWestPanning,
    WkeCursorInfoMove,
    WkeCursorInfoVerticalText,
    WkeCursorInfoCell,
    WkeCursorInfoContextMenu,
    WkeCursorInfoAlias,
    WkeCursorInfoProgress,
    WkeCursorInfoNoDrop,
    WkeCursorInfoCopy,
    WkeCursorInfoNone,
    WkeCursorInfoNotAllowed,
    WkeCursorInfoZoomIn,
    WkeCursorInfoZoomOut,
    WkeCursorInfoGrab,
    WkeCursorInfoGrabbing,
    WkeCursorInfoCustom
} WkeCursorInfoType;

typedef struct
{
    int x;
    int y;
    int width;
    int height;

    bool menuBarVisible;
    bool statusBarVisible;
    bool toolBarVisible;
    bool locationBarVisible;
    bool scrollbarsVisible;
    bool resizable;
    bool fullscreen;
} wkeWindowFeatures;

//////////////////////////////////////////////////////////////////////////
typedef struct
{
    struct Item
    {
        enum wkeStorageType
        {
            // String data with an associated MIME type. Depending on the MIME type, there may be
            // optional metadata attributes as well.
            StorageTypeString,
            // Stores the name of one file being dragged into the renderer.
            StorageTypeFilename,
            // An image being dragged out of the renderer. Contains a buffer holding the image data
            // as well as the suggested name for saving the image to.
            StorageTypeBinaryData,
            // Stores the filesystem URL of one file being dragged into the renderer.
            StorageTypeFileSystemFile,
        } storageType;

        // Only valid when storageType == StorageTypeString.
        wkeString stringType;
        wkeString stringData;

        // Only valid when storageType == StorageTypeFilename.
        wkeString filenameData;
        wkeString displayNameData;

        // Only valid when storageType == StorageTypeBinaryData.
        char *binaryData;
        int binaryDataLength;

        // Title associated with a link when stringType == "text/uri-list".
        // Filename when storageType == StorageTypeBinaryData.
        wkeString title;

        // Only valid when storageType == StorageTypeFileSystemFile.
        wkeString fileSystemURL;
        long long fileSystemFileSize;

        // Only valid when stringType == "text/html".
        wkeString baseURL;
    };

    struct Item *m_itemList;
    int m_itemListLength;

    int m_modifierKeyState; // State of Shift/Ctrl/Alt/Meta keys.
    wkeString m_filesystemId;
} wkeWebDragData;

typedef enum
{
    wkeWebDragOperationNone = 0,
    wkeWebDragOperationCopy = 1,
    wkeWebDragOperationLink = 2,
    wkeWebDragOperationGeneric = 4,
    wkeWebDragOperationPrivate = 8,
    wkeWebDragOperationMove = 16,
    wkeWebDragOperationDelete = 32,
    wkeWebDragOperationEvery = 0xffffffff
} wkeWebDragOperation;

typedef wkeWebDragOperation wkeWebDragOperationsMask;

typedef enum
{
    WKE_RESOURCE_TYPE_MAIN_FRAME = 0,       // top level page
    WKE_RESOURCE_TYPE_SUB_FRAME = 1,        // frame or iframe
    WKE_RESOURCE_TYPE_STYLESHEET = 2,       // a CSS stylesheet
    WKE_RESOURCE_TYPE_SCRIPT = 3,           // an external script
    WKE_RESOURCE_TYPE_IMAGE = 4,            // an image (jpg/gif/png/etc)
    WKE_RESOURCE_TYPE_FONT_RESOURCE = 5,    // a font
    WKE_RESOURCE_TYPE_SUB_RESOURCE = 6,     // an "other" subresource.
    WKE_RESOURCE_TYPE_OBJECT = 7,           // an object (or embed) tag for a plugin,
    // or a resource that a plugin requested.
    WKE_RESOURCE_TYPE_MEDIA = 8,            // a media resource.
    WKE_RESOURCE_TYPE_WORKER = 9,           // the main resource of a dedicated
    // worker.
    WKE_RESOURCE_TYPE_SHARED_WORKER = 10,   // the main resource of a shared worker.
    WKE_RESOURCE_TYPE_PREFETCH = 11,        // an explicitly requested prefetch
    WKE_RESOURCE_TYPE_FAVICON = 12,         // a favicon
    WKE_RESOURCE_TYPE_XHR = 13,             // a XMLHttpRequest
    WKE_RESOURCE_TYPE_PING = 14,            // a ping request for <a ping>
    WKE_RESOURCE_TYPE_SERVICE_WORKER = 15,  // the main resource of a service worker.
    WKE_RESOURCE_TYPE_LAST_TYPE
} wkeResourceType;

typedef struct
{
    bool isHolded;
    wkeString url;
    wkeString newUrl;
    wkeResourceType resourceType;
    int httpResponseCode;
    wkeString method;
    wkeString referrer;
    void *headers;
} wkeWillSendRequestInfo;

typedef enum
{
    wkeHttBodyElementTypeData,
    wkeHttBodyElementTypeFile,
} wkeHttBodyElementType;

typedef struct
{
    int size;
    void *data;
    size_t length;
} wkeMemBuf;

typedef struct
{
    int size;
    wkeHttBodyElementType type;
    wkeMemBuf *data;
    wkeString filePath;
    long long fileStart;
    long long fileLength; // -1 means to the end of the file.
} wkePostBodyElement;

typedef struct
{
    int size;
    wkePostBodyElement **element;
    size_t elementSize;
    bool isDirty;
} wkePostBodyElements;

typedef struct
{
    int size;
    wkeWebFrameHandle frame;
    wkeWillSendRequestInfo *willSendRequestInfo;
    const char *url;
    wkePostBodyElements *postBody;
} wkeTempCallbackInfo;

typedef enum _wkeRequestType
{
    kWkeRequestTypeInvalidation,
    kWkeRequestTypeGet,
    kWkeRequestTypePost,
    kWkeRequestTypePut,
} wkeRequestType;

typedef void(*wkeTitleChangedCallback)(wkeWebView webView, void *param, const wkeString title);
typedef void(*wkeURLChangedCallback)(wkeWebView webView, void *param, const wkeString url);
typedef void(*wkeURLChangedCallback2)(wkeWebView webView, void *param, wkeWebFrameHandle frameId,
                                      const wkeString url);
typedef void(*wkePaintUpdatedCallback)(wkeWebView webView, void *param, const HDC hdc, int x, int y, int cx,
                                       int cy);
typedef void(*wkePaintBitUpdatedCallback)(wkeWebView webView, void *param, const void *buffer,
                                          const wkeRect *r, int width, int height);
typedef void(*wkeAlertBoxCallback)(wkeWebView webView, void *param, const wkeString msg);
typedef bool(*wkeConfirmBoxCallback)(wkeWebView webView, void *param, const wkeString msg);
typedef bool(*wkePromptBoxCallback)(wkeWebView webView, void *param, const wkeString msg,
                                    const wkeString defaultResult, wkeString result);
typedef bool(*wkeNavigationCallback)(wkeWebView webView, void *param, wkeNavigationType navigationType,
                                     const wkeString url);
typedef wkeWebView(*wkeCreateViewCallback)(wkeWebView webView, void *param, wkeNavigationType navigationType,
                                           const wkeString url, const wkeWindowFeatures *windowFeatures);
typedef void(*wkeDocumentReadyCallback)(wkeWebView webView, void *param);
typedef void(*wkeDocumentReady2Callback)(wkeWebView webView, void *param, wkeWebFrameHandle frameId);

typedef wkeWebView(*wkeCreateViewCallback)(wkeWebView webView, void *param, wkeNavigationType navigationType,
                                           const wkeString url, const wkeWindowFeatures *windowFeatures);
typedef void(*wkeDocumentReadyCallback)(wkeWebView webView, void *param);
typedef void(*wkeDocumentReady2Callback)(wkeWebView webView, void *param, wkeWebFrameHandle frameId);

typedef struct
{
    int size;
    int width;
    int height;
    double duration;
} wkeMediaLoadInfo;
typedef void(*wkeWillMediaLoadCallback)(wkeWebView webView, void *param, const char *url,
                                        wkeMediaLoadInfo *info);

typedef enum
{
    WKE_DID_START_LOADING,
    WKE_DID_STOP_LOADING,
    WKE_DID_NAVIGATE,
    WKE_DID_NAVIGATE_IN_PAGE,
    WKE_DID_GET_RESPONSE_DETAILS,
    WKE_DID_GET_REDIRECT_REQUEST,
    WKE_DID_POST_REQUEST,
} wkeOtherLoadType;
typedef void(*wkeOnOtherLoadCallback)(wkeWebView webView, void *param, wkeOtherLoadType type,
                                      wkeTempCallbackInfo *info);

typedef enum
{
    WKE_LOADING_SUCCEEDED,
    WKE_LOADING_FAILED,
    WKE_LOADING_CANCELED
} wkeLoadingResult;

typedef void(*wkeLoadingFinishCallback)(wkeWebView webView, void *param, const wkeString url,
                                        wkeLoadingResult result, const wkeString failedReason);
typedef bool(*wkeDownloadCallback)(wkeWebView webView, void *param, const char *url);

typedef enum
{
    wkeLevelDebug = 4,
    wkeLevelLog = 1,
    wkeLevelInfo = 5,
    wkeLevelWarning = 2,
    wkeLevelError = 3,
    wkeLevelRevokedError = 6,
    wkeLevelLast = wkeLevelInfo
} wkeConsoleLevel;
typedef void(*wkeConsoleCallback)(wkeWebView webView, void *param, wkeConsoleLevel level,
                                  const wkeString message, const wkeString sourceName, unsigned sourceLine, const wkeString stackTrace);

typedef void(*wkeOnCallUiThread)(wkeWebView webView, void *paramOnInThread);
typedef void(*wkeCallUiThread)(wkeWebView webView, wkeOnCallUiThread func, void *param);

//wkeNet--------------------------------------------------------------------------------------
typedef bool(*wkeLoadUrlBeginCallback)(wkeWebView webView, void *param, const char *url, void *job);
typedef void(*wkeLoadUrlEndCallback)(wkeWebView webView, void *param, const char *url, void *job, void *buf,
                                     int len);
typedef void(*wkeDidCreateScriptContextCallback)(wkeWebView webView, void *param, wkeWebFrameHandle frameId,
                                                 void *context, int extensionGroup, int worldId);
typedef void(*wkeWillReleaseScriptContextCallback)(wkeWebView webView, void *param, wkeWebFrameHandle frameId,
                                                   void *context, int worldId);
typedef bool(*wkeNetResponseCallback)(wkeWebView webView, void *param, const char *url, void *job);

typedef void *v8ContextPtr;
typedef void *v8Isolate;

//wkewindow-----------------------------------------------------------------------------------
typedef enum
{
    WKE_WINDOW_TYPE_POPUP,
    WKE_WINDOW_TYPE_TRANSPARENT,
    WKE_WINDOW_TYPE_CONTROL

} wkeWindowType;

typedef bool(*wkeWindowClosingCallback)(wkeWebView webWindow, void *param);
typedef void(*wkeWindowDestroyCallback)(wkeWebView webWindow, void *param);

typedef struct
{
    RECT bounds;
    bool draggable;
} wkeDraggableRegion;
typedef void(*wkeDraggableRegionsChangedCallback)(wkeWebView webWindow, void *param,
                                                  const wkeDraggableRegion *rects, int rectCount);

//JavaScript Bind-----------------------------------------------------------------------------------
#define JS_CALL __fastcall
typedef jsValue(JS_CALL *jsNativeFunction)(jsExecState es);

typedef jsValue(*wkeJsNativeFunction)(jsExecState es, void *param);

typedef enum
{
    JSTYPE_NUMBER,
    JSTYPE_STRING,
    JSTYPE_BOOLEAN,
    JSTYPE_OBJECT,
    JSTYPE_FUNCTION,
    JSTYPE_UNDEFINED,
    JSTYPE_ARRAY,
} jsType;

// cexer JS对象、函数绑定支持
typedef jsValue(*jsGetPropertyCallback)(jsExecState es, jsValue object, const char *propertyName);
typedef bool(*jsSetPropertyCallback)(jsExecState es, jsValue object, const char *propertyName, jsValue value);
typedef jsValue(*jsCallAsFunctionCallback)(jsExecState es, jsValue object, jsValue *args, int argCount);
struct tagjsData; // declare warning fix
typedef void(*jsFinalizeCallback)(struct tagjsData *data);

typedef struct tagjsData
{
    char typeName[100];
    jsGetPropertyCallback propertyGet;
    jsSetPropertyCallback propertySet;
    jsFinalizeCallback finalize;
    jsCallAsFunctionCallback callAsFunction;
} jsData;



//////////////////////////////////////////////////////////////////////////
// miniblink 动态库的加载与初始化，必须成对调用。
// 内部使用引用计数，当引用计数为0时，卸载 miniblink 动态库
DUILIB_API bool LoadWke(LPCTSTR szWkeDll);
DUILIB_API void FreeWke(void);


DUILIB_API void            wkeOnMouseOverUrlChanged(wkeWebView webView, wkeTitleChangedCallback callback,
                                                    void *param);
DUILIB_API void            wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback,
                                             void *param);
DUILIB_API void            wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void *param);
DUILIB_API void            wkeOnURLChanged2(wkeWebView webView, wkeURLChangedCallback2 callback, void *param);
DUILIB_API void            wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback,
                                             void *param);
DUILIB_API void            wkeOnPaintBitUpdated(wkeWebView webView, wkePaintBitUpdatedCallback callback,
                                                void *param);
DUILIB_API void            wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void *param);
DUILIB_API void            wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void *param);
DUILIB_API void            wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void *param);
DUILIB_API void            wkeOnNavigation(wkeWebView webView, wkeNavigationCallback callback, void *param);
DUILIB_API void            wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void *param);
DUILIB_API void            wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback,
                                              void *param);
DUILIB_API void            wkeOnDocumentReady2(wkeWebView webView, wkeDocumentReady2Callback callback,
                                               void *param);
DUILIB_API void            wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback,
                                              void *param);
DUILIB_API void            wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void *param);
DUILIB_API void            wkeOnConsole(wkeWebView webView, wkeConsoleCallback callback, void *param);
DUILIB_API void            wkeSetUIThreadCallback(wkeWebView webView, wkeCallUiThread callback, void *param);
DUILIB_API void            wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback,
                                             void *param);
DUILIB_API void            wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void *param);
DUILIB_API void            wkeOnDidCreateScriptContext(wkeWebView webView,
                                                       wkeDidCreateScriptContextCallback callback, void *param);
DUILIB_API void            wkeOnWillReleaseScriptContext(wkeWebView webView,
                                                         wkeWillReleaseScriptContextCallback callback, void *param);
DUILIB_API void            wkeOnWindowClosing(wkeWebView webWindow, wkeWindowClosingCallback callback,
                                              void *param);
DUILIB_API void            wkeOnWindowDestroy(wkeWebView webWindow, wkeWindowDestroyCallback callback,
                                              void *param);
DUILIB_API void            wkeOnDraggableRegionsChanged(wkeWebView webWindow,
                                                        wkeDraggableRegionsChangedCallback callback, void *param);
DUILIB_API void            wkeOnWillMediaLoad(wkeWebView webWindow, wkeWillMediaLoadCallback callback,
                                              void *param);
DUILIB_API void            wkeOnOtherLoad(wkeWebView webWindow, wkeOnOtherLoadCallback callback, void *param);


DUILIB_API void            wkeInitialize();
DUILIB_API void            wkeInitializeEx(const wkeSettings *settings);
DUILIB_API void            wkeConfigure(const wkeSettings *settings);
DUILIB_API bool            wkeIsInitialize();
DUILIB_API void            wkeUpdate();

DUILIB_API void            wkeFinalize();
DUILIB_API void            wkeShutdown();

DUILIB_API void            wkeSetViewSettings(wkeWebView webView, const wkeViewSettings *settings);
DUILIB_API void            wkeSetDebugConfig(wkeWebView webView, const char *debugString, const char *param);

DUILIB_API unsigned int    wekGetVersion();
DUILIB_API const utf8     *wkeGetVersionString();
#define wkeVersion          wkeGetVersion
#define wkeVersionString    wkeGetVersionString

DUILIB_API void            wkeGC(wkeWebView webView, long delayMs);
DUILIB_API void            wkeSetFileSystem(WKE_FILE_OPEN pfnOpen, WKE_FILE_CLOSE pfnClose,
                                            WKE_FILE_SIZE pfnSize, WKE_FILE_READ pfnRead, WKE_FILE_SEEK pfnSeek);

DUILIB_API const char     *wkeGetWebViewName(wkeWebView webView);
DUILIB_API void            wkeSetWebViewName(wkeWebView webView, const char *name);
#define wkeWebViewName      wkeGetWebViewName

DUILIB_API void            wkeDestroyWebView(wkeWebView webView);
DUILIB_API wkeWebView      wkeCreateWebView();
DUILIB_API wkeWebView      wkeGetWebViewForCurrentContext();

DUILIB_API bool            wkeIsLoaded(wkeWebView webView);
DUILIB_API bool            wkeIsLoadFailed(wkeWebView webView);
DUILIB_API bool            wkeIsLoadComplete(wkeWebView webView);

DUILIB_API bool            wkeIsLoading(wkeWebView webView);
DUILIB_API bool            wkeIsLoadingSucceeded(wkeWebView webView);
DUILIB_API bool            wkeIsLoadingFailed(wkeWebView webView);
DUILIB_API bool            wkeIsLoadingCompleted(wkeWebView webView);
DUILIB_API bool            wkeIsDocumentReady(wkeWebView webView);
DUILIB_API void            wkeStopLoading(wkeWebView webView);
DUILIB_API void            wkeReload(wkeWebView webView);

DUILIB_API const utf8     *wkeGetSource(wkeWebView webView);
DUILIB_API const utf8     *wkeGetTitleA(wkeWebView webView);
DUILIB_API const wchar_t  *wkeGetTitleW(wkeWebView webView);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeGetTitle     wkeGetTitleW
#else
    #define wkeGetTitle     wkeGetTitleA
#endif

DUILIB_API void            wkeResize(wkeWebView webView, int w, int h);
DUILIB_API int             wkeGetWidth(wkeWebView webView);
DUILIB_API int             wkeGetHeight(wkeWebView webView);
#define wkeWidth    wkeGetWidth
#define wkeHeight   wkeGetHeight

DUILIB_API int             wkeGetContentWidth(wkeWebView webView);
DUILIB_API int             wkeGetContentHeight(wkeWebView webView);
#define wkeContentsWidth    wkeGetContentWidth
#define wkeContentsHeight   wkeGetContentHeight

DUILIB_API void            wkeEditorSelectAll(wkeWebView webView);
DUILIB_API void            wkeEditorUnSelect(wkeWebView webView);
DUILIB_API void            wkeEditorCopy(wkeWebView webView);
DUILIB_API void            wkeEditorCut(wkeWebView webView);
DUILIB_API void            wkeEditorPaste(wkeWebView webView);
DUILIB_API void            wkeEditorDelete(wkeWebView webView);
DUILIB_API void            wkeEditorUndo(wkeWebView webView);
DUILIB_API void            wkeEditorRedo(wkeWebView webView);
#define         wkeSelectAll        wkeEditorSelectAll
#define         wkeUnSelect         wkeEditorUnSelect
#define         wkeCopy             wkeEditorCopy
#define         wkeCut              wkeEditorCut
#define         wkePaste            wkeEditorPaste
#define         wkeDelete           wkeEditorDelete
#define         wkeUndo             wkeEditorUndo
#define         wkeRedo             wkeEditorRedo

DUILIB_API void            wkeSetEditable(wkeWebView webView, bool editable);

DUILIB_API bool            wkeFireMouseEvent(wkeWebView webView, unsigned int message, int x, int y,
                                             unsigned int flags);
DUILIB_API bool            wkeFireContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags);
DUILIB_API bool            wkeFireMouseWheelEvent(wkeWebView webView, int x, int y, int delta,
                                                  unsigned int flags);
DUILIB_API bool            wkeFireKeyUpEvent(wkeWebView webView, unsigned int virtualKeyCode,
                                             unsigned int flags, bool systemKey);
DUILIB_API bool            wkeFireKeyDownEvent(wkeWebView webView, unsigned int virtualKeyCode,
                                               unsigned int flags, bool systemKey);
DUILIB_API bool            wkeFireKeyPressEvent(wkeWebView webView, unsigned int charCode,
                                                unsigned int flags, bool systemKey);
DUILIB_API bool            wkeFireWindowsMessage(wkeWebView webView, HWND hWnd, UINT message,
                                                 WPARAM wParam, LPARAM lParam, LRESULT *result);
#define         wkeMouseEvent           wkeFireMouseEvent
#define         wkeContextMenuEvent     wkeFireContextMenuEvent
#define         wkeMouseWheel           wkeFireMouseWheelEvent
#define         wkeKeyUp                wkeFireKeyUpEvent
#define         wkeKeyDown              wkeFireKeyDownEvent
#define         wkeKeyPress             wkeFireKeyPressEvent
#define         wkeWindowsMessage       wkeFireWindowsMessage

DUILIB_API void            wkeSetFocus(wkeWebView webView);
DUILIB_API void            wkeKillFocus(wkeWebView webView);
#define wkeFocus    wkeSetFocus
#define wkeUnfocus  wkeKillFocus

DUILIB_API wkeRect         wkeGetCaretRect(wkeWebView webView);

DUILIB_API void            wkeSetClientHandler(wkeWebView webView, const wkeClientHandler *handler);
DUILIB_API const wkeClientHandler *wkeGetClientHandler(wkeWebView webView);

DUILIB_API void            wkeSetMemoryCacheEnable(wkeWebView webView, bool b);
DUILIB_API void            wkeSetMouseEnabled(wkeWebView webView, bool b);
DUILIB_API void            wkeSetTouchEnabled(wkeWebView webView, bool b);
DUILIB_API void            wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b);
DUILIB_API void            wkeSetCspCheckEnable(wkeWebView webView, bool b);
DUILIB_API void            wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b);
DUILIB_API void            wkeSetHeadlessEnabled(wkeWebView webView, bool b);  // 可以关闭渲染
DUILIB_API void            wkeGetCursorInfoType(wkeWebView webView);

DUILIB_API void            wkeSetDragEnable(wkeWebView webView,
                                            bool b);       // 可关闭拖拽文件加载网页
DUILIB_API void            wkeSetDragFiles(wkeWebView webView, const POINT *clintPos, const POINT *screenPos,
                                           wkeString files[ ], int filesCount);

DUILIB_API void            wkeSetViewNetInterface(wkeWebView webView, const char *netInterface);

DUILIB_API void            wkeSetProxy(const wkeProxy *proxy);
DUILIB_API void            wkeSetViewProxy(wkeWebView webView, wkeProxy *proxy);

DUILIB_API void            wkeSetHandle(wkeWebView webView, HWND wnd);
DUILIB_API void            wkeSetHandleOffset(wkeWebView webView, int x, int y);

DUILIB_API bool            wkeIsTransparent(wkeWebView webView);
DUILIB_API void            wkeSetTransparent(wkeWebView webView, bool transparent);

DUILIB_API void            wkeSetUserAgentA(wkeWebView webView, const utf8 *userAgent);
DUILIB_API void            wkeSetUserAgentW(wkeWebView webView, const wchar_t *userAgent);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeSetUserAgent     wkeSetUserAgentW
#else
    #define wkeSetUserAgent     wkeSetUserAgentA
#endif

DUILIB_API void            wkeLoadURLA(wkeWebView webView, const utf8 *url);
DUILIB_API void            wkeLoadURLW(wkeWebView webView, const wchar_t *url);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeLoadURL     wkeLoadURLW
#else
    #define wkeLoadURL     wkeLoadURLA
#endif

DUILIB_API void            wkePostURLA(wkeWebView wkeView, const utf8 *url,
                                       const char *postData, int  postLen);
DUILIB_API void            wkePostURLW(wkeWebView wkeView, const wchar_t *url,
                                       const char *postData, int postLen);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkePostURL     wkePostURLW
#else
    #define wkePostURL     wkePostURLA
#endif

DUILIB_API void            wkeLoadHTMLA(wkeWebView webView, const utf8 *html);
DUILIB_API void            wkeLoadHTMLW(wkeWebView webView, const wchar_t *html);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeLoadHTML     wkeLoadHTMLW
#else
    #define wkeLoadHTML     wkeLoadHTMLA
#endif

DUILIB_API void            wkeLoadFileA(wkeWebView webView, const utf8 *filename);
DUILIB_API void            wkeLoadFileW(wkeWebView webView, const wchar_t *filename);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeLoadFile     wkeLoadFileW
#else
    #define wkeLoadFile     wkeLoadFileA
#endif

DUILIB_API const utf8     *wkeGetURLA(wkeWebView webView);
DUILIB_API const wchar_t  *wkeGetURLW(wkeWebView webView);     // TODO 未实现
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeGetURL     wkeGetURLW
#else
    #define wkeGetURL     wkeGetURLA
#endif

DUILIB_API void            wkeSetDirty(wkeWebView webView, bool dirty);
DUILIB_API bool            wkeIsDirty(wkeWebView webView);
DUILIB_API void            wkeAddDirtyArea(wkeWebView webView, int x, int y, int w, int h);
DUILIB_API void            wkeLayoutIfNeeded(wkeWebView webView);
DUILIB_API void            wkePaint(wkeWebView webView, void *bits, int pitch);
DUILIB_API void            wkePaint2(wkeWebView webView, void *bits, int bufW, int bufH, int xDst,
                                     int yDst, int w, int h, int xSrc, int ySrc, bool bCopyAlpha);
DUILIB_API bool            wkeRepaintIfNeeded(wkeWebView webView);
DUILIB_API HDC             wkeGetViewDC(wkeWebView webView);
DUILIB_API HWND            wkeGetHostHWND(wkeWebView webView);

DUILIB_API bool            wkeCanGoBack(wkeWebView webView);
DUILIB_API bool            wkeGoBack(wkeWebView webView);
DUILIB_API bool            wkeCanGoForward(wkeWebView webView);
DUILIB_API bool            wkeGoForward(wkeWebView webView);

DUILIB_API const utf8     *wkeGetCookieA(wkeWebView webView);
DUILIB_API const wchar_t  *wkeGetCookieW(wkeWebView webView);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeGetCookie     wkeGetCookieW
#else
    #define wkeGetCookie     wkeGetCookieA
#endif

DUILIB_API bool            wkeIsCookieEnabled(wkeWebView webView);
DUILIB_API void            wkeSetCookieEnabled(wkeWebView webView, bool enable);
#define wkeCookieEnabled    wkeIsCookieEnabled

// TODO unicode 未实现
// cookie格式必须是:Set-cookie: PRODUCTINFO=webxpress; domain=.fidelity.com; path=/; secure")
DUILIB_API void            wkeSetCookieA(wkeWebView webView, const utf8 *url, const utf8 *cookie);
DUILIB_API void            wkeSetCookieW(wkeWebView webView, const wchar_t *url, const wchar_t *cookie);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeSetCookie     wkeSetCookieW
#else
    #define wkeSetCookie     wkeSetCookieA
#endif

DUILIB_API void            wkeVisitAllCookie(void *params, wkeCookieVisitor visitor);
DUILIB_API void            wkePerformCookieCommand(wkeCookieCommand command);

// TODO ansi 未实现
DUILIB_API void            wkeSetCookieJarPathA(wkeWebView webView, const utf8 *path);
DUILIB_API void            wkeSetCookieJarPathW(wkeWebView webView, const wchar_t *path);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeSetCookieJarPath     wkeSetCookieJarPathW
#else
    #define wkeSetCookieJarPath     wkeSetCookieJarPathA
#endif

// TODO ansi 未实现
DUILIB_API void            wkeSetCookieJarFullPathA(wkeWebView webView, const utf8 *path);
DUILIB_API void            wkeSetCookieJarFullPathW(wkeWebView webView, const wchar_t *path);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeSetCookieJarFullPath     wkeSetCookieJarFullPathW
#else
    #define wkeSetCookieJarFullPath     wkeSetCookieJarFullPathA
#endif

// TODO ansi 未实现
DUILIB_API void            wkeSetLocalStorageFullPathA(wkeWebView webView, const utf8 *path);
DUILIB_API void            wkeSetLocalStorageFullPathW(wkeWebView webView, const wchar_t *path);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeSetLocalStorageFullPath     wkeSetLocalStorageFullPathW
#else
    #define wkeSetLocalStorageFullPath     wkeSetLocalStorageFullPathA
#endif

// TODO ansi 未实现
DUILIB_API void            wkeAddPluginDirectoryA(wkeWebView webView, const utf8 *path);
DUILIB_API void            wkeAddPluginDirectoryW(wkeWebView webView, const wchar_t *path);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeAddPluginDirectory     wkeAddPluginDirectoryW
#else
    #define wkeAddPluginDirectory     wkeAddPluginDirectoryA
#endif

DUILIB_API float           wkeGetMediaVolume(wkeWebView webView);
DUILIB_API void            wkeSetMediaVolume(wkeWebView webView, float volume);
#define wkeMediaVolume  wkeGetMediaVolume

DUILIB_API float           wkeGetZoomFactor(wkeWebView webView);
DUILIB_API void            wkeSetZoomFactor(wkeWebView webView, float factor);
#define wkeZoomFactor   wkeGetZoomFactor

DUILIB_API void            wkeSetStringA(wkeString *string, const utf8 *str, size_t len);
DUILIB_API void            wkeSetStringW(wkeString *string, const wchar_t *str, size_t len);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeSetString     wkeSetStringW
#else
    #define wkeSetString     wkeSetStringA
#endif

DUILIB_API const utf8     *wkeGetStringA(const wkeString string);
DUILIB_API const wchar_t  *wkeGetStringW(const wkeString string);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeGetString    wkeGetStringW
    #define wkeToString     wkeToStringW
#else
    #define wkeGetString    wkeGetStringA
    #define wkeToString     wkeGetStringA
#endif

DUILIB_API void            wkeDeleteString(wkeString str);
DUILIB_API wkeString       wkeCreateStringA(const utf8 *str, size_t len);  // TODO 未实现
DUILIB_API wkeString       wkeCreateStringW(const wchar_t *str, size_t len);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeCreateString     wkeCreateStringW
#else
    #define wkeCreateString     wkeCreateStringA
#endif

DUILIB_API void            wkeSleep(wkeWebView webView);
DUILIB_API bool            wkeAwake(wkeWebView webView);
DUILIB_API bool            wkeIsAwake(wkeWebView webView);
#define wkeAwaken   wkeAwake

// TODO 需要实现 unicode
DUILIB_API void            wkeSetUserKeyValue(wkeWebView webView, const char *key, void *value);
DUILIB_API void           *wkeGetUserKeyValue(wkeWebView webView, const char *key);

DUILIB_API void            wkeSetDeviceParameter(wkeWebView webView, const char *device,
                                                 const char *paramStr, int paramInt, float paramFloat);
DUILIB_API wkeTempCallbackInfo wkeGetTempCallbackInfo(wkeWebView webView);

DUILIB_API void            wkeDeleteWillSendRequestInfo(wkeWebView webWindow, wkeWillSendRequestInfo *info);

// TODO 需要实现 unicode/ansi
DUILIB_API void            wkeNetSetMIMEType(void *job, char *type);
DUILIB_API void            wkeNetGetMIMEType(void *job, wkeString mime);
DUILIB_API void            wkeNetSetHTTPHeaderField(void *job, wchar_t *key, wchar_t *value, bool response);
DUILIB_API const char     *wkeNetGetHTTPHeaderField(void *job, const char *key);
DUILIB_API void            wkeNetSetURL(void *job, const char *url);
// 调用此函数后,网络层收到数据会存储在一buf内,接收数据完成后响应OnLoadUrlEnd事件.#此调用严重影响性能,慎用
// 此函数和wkeNetSetData的区别是，wkeNetHookRequest会在接受到真正网络数据后再调用回调，并允许回调修改网络数据。
// 而wkeNetSetData是在网络数据还没发送的时候修改
DUILIB_API void            wkeNetSetData(void *job, void *buf, int len);
DUILIB_API void            wkeNetHookRequest(void *job);
DUILIB_API void            wkeNetOnResponse(wkeWebView webView, wkeNetResponseCallback callback, void *param);

DUILIB_API wkePostBodyElements    *wkeNetGetPostBody(void *jobPtr);
DUILIB_API wkeRequestType  wkeNetGetRequestMethod(void *jobPtr);

DUILIB_API void            wkeNetContinueJob(void *jobPtr);
DUILIB_API const char     *wkeNetGetUrlByJob(void *jobPtr);
DUILIB_API void            wkeNetCancelRequest(void *jobPtr);
DUILIB_API void            wkeNetChangeRequestUrl(void *jobPtr, const char *url);
DUILIB_API void            wkeNetHoldJobToAsynCommit(void *jobPtr);

DUILIB_API wkePostBodyElements    *wkeNetCreatePostBodyElements(wkeWebView webView, size_t length);
DUILIB_API void            wkeNetFreePostBodyElements(wkePostBodyElements *element);
DUILIB_API wkePostBodyElement     *wkeNetCreatePostBodyElement(wkeWebView webView);
DUILIB_API void            wkeNetFreePostBodyElement(wkePostBodyElement *element);
DUILIB_API wkeMemBuf      *wkeCreateMemBuf(wkeWebView webView, void *buf, size_t length);
DUILIB_API void            wkeFreeMemBuf(wkeMemBuf *buffer);

DUILIB_API bool            wkeIsMainFrame(wkeWebView webView, wkeWebFrameHandle frameId);
DUILIB_API bool            wkeIsWebRemoteFrame(wkeWebView webView, wkeWebFrameHandle frameId);
DUILIB_API wkeWebFrameHandle   wkeWebFrameGetMainFrame(wkeWebView webView);

DUILIB_API void            wkeWebFrameGetMainWorldScriptContext(wkeWebView webView,
        wkeWebFrameHandle webFrameId, v8ContextPtr contextOut);
DUILIB_API v8Isolate       wkeGetBlinkMainThreadIsolate();

DUILIB_API wkeWebView      wkeCreateWebWindow(wkeWindowType type, HWND parent,
                                              int x, int y, int width, int height);
DUILIB_API void            wkeDestroyWebWindow(wkeWebView webWindow);
DUILIB_API void           *wkeGetWindowHandle(wkeWebView webWindow);

DUILIB_API void            wkeShowWindow(wkeWebView webWindow, bool show);
DUILIB_API void            wkeEnableWindow(wkeWebView webWindow, bool enable);

DUILIB_API void            wkeMoveWindow(wkeWebView webWindow, int x, int y, int width, int height);
DUILIB_API void            wkeMoveToCenter(wkeWebView webWindow);
DUILIB_API void            wkeResizeWindow(wkeWebView webWindow, int width, int height);

DUILIB_API wkeWebDragOperation wkeDragTargetDragEnter(wkeWebView webWindow, const wkeWebDragData *webDragData,
                                                      const POINT *clientPoint, const POINT *screenPoint,
                                                      wkeWebDragOperationsMask operationsAllowed, int modifiers);
DUILIB_API wkeWebDragOperation wkeDragTargetDragOver(wkeWebView webWindow, const POINT *clientPoint,
                                                     const POINT *screenPoint, wkeWebDragOperationsMask operationsAllowed,
                                                     int modifiers);
DUILIB_API void            wkeDragTargetDragLeave(wkeWebView webWindow);
DUILIB_API void            wkeDragTargetDrop(wkeWebView webWindow, const POINT *clientPoint,
                                             const POINT *screenPoint, int modifiers);

DUILIB_API void            wkeSetWindowTitleA(wkeWebView webWindow, const utf8 *title);
DUILIB_API void            wkeSetWindowTitleW(wkeWebView webWindow, const wchar_t *title);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeSetWindowTitle     wkeSetWindowTitleW
#else
    #define wkeSetWindowTitle     wkeSetWindowTitleA
#endif

//////////////////////////////////////////////////////////////////////////
// 以下是C/C++ 与 JS 互调相关 API

DUILIB_API jsValue      wkeRunJSA(wkeWebView webView, const utf8 *script);
DUILIB_API jsValue      wkeRunJSW(wkeWebView webView, const wchar_t *script);
#if defined(UNICODE) || defined(_UNICODE)
    #define wkeRunJS     wkeRunJSW
#else
    #define wkeRunJS     wkeRunJSA
#endif

DUILIB_API jsValue         wkeRunJsByFrame(wkeWebView webView, wkeWebFrameHandle frameId,
                                           const utf8 *script, bool isInClosure);

DUILIB_API jsExecState     wkeGlobalExec(wkeWebView webView);

DUILIB_API void            jsBindFunction(const char *name, jsNativeFunction fn, unsigned int argCount);
DUILIB_API void            jsBindGetter(const char *name, jsNativeFunction fn); /*get property*/
DUILIB_API void            jsBindSetter(const char *name, jsNativeFunction fn); /*set property*/

DUILIB_API void            jsBindFunctionEx(const char *name, wkeJsNativeFunction fn, void *param,
                                            unsigned int argCount);
DUILIB_API void            jsBindGetterEx(const char *name, wkeJsNativeFunction fn, void *param);
DUILIB_API void            jsBindSetterEx(const char *name, wkeJsNativeFunction fn, void *param);
#define wkeJsBindFunction   jsBindFunctionEx
#define wkeJsBindGetter     jsBindGetterEx
#define wkeJsBindSetter     jsBindSetterEx

DUILIB_API int             jsArgCount(jsExecState es);
DUILIB_API jsType          jsArgType(jsExecState es, int index);
DUILIB_API jsValue         jsArgValue(jsExecState es, int index);  // TODO 添加该接口，与 jsArg 并存
#define jsArg   jsArgValue

DUILIB_API jsType          jsTypeOf(jsExecState es, jsValue v);
DUILIB_API bool            jsIsNumber(jsExecState es, jsValue v);
DUILIB_API bool            jsIsString(jsExecState es, jsValue v);
DUILIB_API bool            jsIsBoolean(jsExecState es, jsValue v);
DUILIB_API bool            jsIsObject(jsExecState es, jsValue v);
DUILIB_API bool            jsIsFunction(jsExecState es, jsValue v);
DUILIB_API bool            jsIsUndefined(jsExecState es, jsValue v);
DUILIB_API bool            jsIsNull(jsExecState es, jsValue v);
DUILIB_API bool            jsIsArray(jsExecState es, jsValue v);
DUILIB_API bool            jsIsTrue(jsExecState es, jsValue v);
DUILIB_API bool            jsIsFalse(jsExecState es, jsValue v);

DUILIB_API int             jsToInt(jsExecState es, jsValue v);
DUILIB_API float           jsToFloat(jsExecState es, jsValue v);
DUILIB_API double          jsToDouble(jsExecState es, jsValue v);
DUILIB_API bool            jsToBoolean(jsExecState es, jsValue v);
DUILIB_API const utf8     *jsToTempStringA(jsExecState es, jsValue v);
DUILIB_API const wchar_t  *jsToTempStringW(jsExecState es, jsValue v);
#if defined(UNICODE) || defined(_UNICODE)
    #define jsToTempString      jsToTempStringW
    #define jsToString          jsToTempStringW
#else
    #define jsToTempString      jsToTempStringA
    #define jsToString          jsToTempStringA
#endif

DUILIB_API jsValue         jsInt(jsExecState es, int v);
DUILIB_API jsValue         jsFloat(jsExecState es, float v);
DUILIB_API jsValue         jsDouble(jsExecState es, double v);
DUILIB_API jsValue         jsBoolean(jsExecState es, bool v);
DUILIB_API jsValue         jsUndefined(jsExecState es);
DUILIB_API jsValue         jsNull(jsExecState es);
DUILIB_API jsValue         jsTrue(jsExecState es);
DUILIB_API jsValue         jsFalse(jsExecState es);
DUILIB_API jsValue         jsStringA(jsExecState es, const utf8 *str);
DUILIB_API jsValue         jsStringW(jsExecState es, const wchar_t *str);
#if defined(UNICODE) || defined(_UNICODE)
    #define jsString     jsStringW
#else
    #define jsString     jsStringA
#endif
DUILIB_API jsValue         jsEmptyObject(jsExecState es);
DUILIB_API jsValue         jsEmptyArray(jsExecState es);
DUILIB_API jsValue         jsArrayBuffer(jsExecState es, char *buffer, size_t size);

DUILIB_API jsValue         jsObject(jsExecState es, jsData *obj);
DUILIB_API jsValue         jsSetObjData(jsExecState es, jsData *obj);
DUILIB_API jsData         *jsGetObjData(jsExecState es, jsValue obj);
#define jsFunction  jsSetObjData
#define jsGetData   jsGetObjData

DUILIB_API jsValue         jsGet(jsExecState es, jsValue obj, const char *prop);
DUILIB_API void            jsSet(jsExecState es, jsValue obj, const char *prop, jsValue v);

DUILIB_API jsValue         jsGetAt(jsExecState es, jsValue obj, int index);
DUILIB_API void            jsSetAt(jsExecState es, jsValue obj, int index, jsValue v);

DUILIB_API int             jsGetLength(jsExecState es, jsValue obj);
DUILIB_API void            jsSetLength(jsExecState es, jsValue obj, int length);

DUILIB_API jsValue         jsGlobalObject(jsExecState es);
DUILIB_API wkeWebView      jsGetWebView(jsExecState es);

DUILIB_API jsValue         jsEvalA(jsExecState es, const utf8 *str);
DUILIB_API jsValue         jsEvalW(jsExecState es, const wchar_t *str);
#if defined(UNICODE) || defined(_UNICODE)
    #define jsEval     jsEvalW
#else
    #define jsEval     jsEvalA
#endif

// TOOD ANSI
DUILIB_API jsValue         jsEvalExW(jsExecState es, const wchar_t *str, bool isInClosure);

DUILIB_API jsValue         jsCall(jsExecState es, jsValue func, jsValue thisObject,
                                  jsValue *args, int argCount);
DUILIB_API jsValue         jsCallGlobal(jsExecState es, jsValue func, jsValue *args, int argCount);

DUILIB_API jsValue         jsGetGlobal(jsExecState es, const char *prop);
DUILIB_API void            jsSetGlobal(jsExecState es, const char *prop, jsValue v);

DUILIB_API void            jsGC();

}

#endif // __WKE_DLL_H__
