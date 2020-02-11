#ifndef __WKE_DLL_H__
#define __WKE_DLL_H__
#pragma once

namespace DuiLib {
#define WKE_CALL __cdecl

typedef char utf8;
typedef __int64 jsValue;

struct JsExecStateInfo;
typedef JsExecStateInfo *jsExecState;

struct _tagWkeWebView;
typedef struct _tagWkeWebView *wkeWebView;

struct _tagWkeString;
typedef struct _tagWkeString *wkeString;

typedef void *wkeWebFrameHandle;
typedef void *wkeNetJob;

typedef struct _tagWkeMediaPlayer *wkeMediaPlayer;
typedef struct _tagWkeMediaPlayerClient *wkeMediaPlayerClient;
typedef struct _tabblinkWebURLRequestPtr *blinkWebURLRequestPtr;


struct wkeRect
{
    int x;
    int y;
    int w;
    int h;
};

struct wkePoint
{
    int x;
    int y;
};

enum wkeMouseFlags
{
    WKE_LBUTTON = 0x01,
    WKE_RBUTTON = 0x02,
    WKE_SHIFT   = 0x04,
    WKE_CONTROL = 0x08,
    WKE_MBUTTON = 0x10,
};

enum wkeKeyFlags
{
    WKE_EXTENDED = 0x0100,
    WKE_REPEAT   = 0x4000,
};

enum wkeMouseMsg
{
    WKE_MSG_MOUSEMOVE     = 0x0200,
    WKE_MSG_LBUTTONDOWN   = 0x0201,
    WKE_MSG_LBUTTONUP     = 0x0202,
    WKE_MSG_LBUTTONDBLCLK = 0x0203,
    WKE_MSG_RBUTTONDOWN   = 0x0204,
    WKE_MSG_RBUTTONUP     = 0x0205,
    WKE_MSG_RBUTTONDBLCLK = 0x0206,
    WKE_MSG_MBUTTONDOWN   = 0x0207,
    WKE_MSG_MBUTTONUP     = 0x0208,
    WKE_MSG_MBUTTONDBLCLK = 0x0209,
    WKE_MSG_MOUSEWHEEL    = 0x020A,
};

enum wkeProxyType
{
    WKE_PROXY_NONE,
    WKE_PROXY_HTTP,
    WKE_PROXY_SOCKS4,
    WKE_PROXY_SOCKS4A,
    WKE_PROXY_SOCKS5,
    WKE_PROXY_SOCKS5HOSTNAME
};

enum wkeMenuItemId
{
    kWkeMenuSelectedAllId      = 1 << 1,
    kWkeMenuSelectedTextId     = 1 << 2,
    kWkeMenuUndoId             = 1 << 3,
    kWkeMenuCopyImageId        = 1 << 4,
    kWkeMenuInspectElementAtId = 1 << 5,
    kWkeMenuCutId              = 1 << 6,
    kWkeMenuPasteId            = 1 << 7,
    kWkeMenuPrintId            = 1 << 8,
    kWkeMenuGoForwardId        = 1 << 9,
    kWkeMenuGoBackId           = 1 << 10,
    kWkeMenuReloadId           = 1 << 11,
    kWkeMenuSaveImageId        = 1 << 12,
};

struct wkeGeolocationPosition
{
    double timestamp;
    double latitude;
    double longitude;
    double accuracy;
    bool providesAltitude;
    double altitude;
    bool providesAltitudeAccuracy;
    double altitudeAccuracy;
    bool providesHeading;
    double heading;
    bool providesSpeed;
    double speed;

    wkeGeolocationPosition(const wkeGeolocationPosition& other)
    {
        timestamp = other.timestamp;
        latitude = other.latitude;
        longitude = other.longitude;
        accuracy = other.accuracy;
        providesAltitude = other.providesAltitude;
        altitude = other.altitude;
        providesAltitudeAccuracy = other.providesAltitudeAccuracy;
        altitudeAccuracy = other.altitudeAccuracy;
        providesHeading = other.providesHeading;
        heading = other.heading;
        providesSpeed = other.providesSpeed;
        speed = other.speed;
    }
};

struct wkeSlist
{
    char *data;
    struct wkeSlist *next;
};

struct wkeProxy
{
    wkeProxyType type;
    char hostname[100];
    unsigned short port;
    char username[50];
    char password[50];
};

enum wkeSettingMask
{
    WKE_SETTING_PROXY                         = 1,
    WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD = 1 << 2,
};

struct wkeSettings
{
    wkeProxy proxy;
    unsigned int mask;
};

struct wkeViewSettings
{
    int size;
    unsigned int bgColor;
};

struct wkeMemBuf
{
    int size;
    void *data;
    size_t length;
};

enum wkeNavigationType
{
    WKE_NAVIGATION_TYPE_LINKCLICK,
    WKE_NAVIGATION_TYPE_FORMSUBMITTE,
    WKE_NAVIGATION_TYPE_BACKFORWARD,
    WKE_NAVIGATION_TYPE_RELOAD,
    WKE_NAVIGATION_TYPE_FORMRESUBMITT,
    WKE_NAVIGATION_TYPE_OTHER
};

enum wkeWebDragOperation
{
    wkeWebDragOperationNone    = 0,
    wkeWebDragOperationCopy    = 1,
    wkeWebDragOperationLink    = 2,
    wkeWebDragOperationGeneric = 4,
    wkeWebDragOperationPrivate = 8,
    wkeWebDragOperationMove    = 16,
    wkeWebDragOperationDelete  = 32,
    wkeWebDragOperationEvery   = 0xffffffff
};
typedef wkeWebDragOperation wkeWebDragOperationsMask;

struct wkeWebDragData
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
        wkeMemBuf *stringType;
        wkeMemBuf *stringData;
        // Only valid when storageType == StorageTypeFilename.
        wkeMemBuf *filenameData;
        wkeMemBuf *displayNameData;
        // Only valid when storageType == StorageTypeBinaryData.
        wkeMemBuf *binaryData;
        // Title associated with a link when stringType == "text/uri-list".
        // Filename when storageType == StorageTypeBinaryData.
        wkeMemBuf *title;
        // Only valid when storageType == StorageTypeFileSystemFile.
        wkeMemBuf *fileSystemURL;
        __int64 fileSystemFileSize;
        // Only valid when stringType == "text/html".
        wkeMemBuf *baseURL;
    };

    struct Item *m_itemList;
    int m_itemListLength;

    int m_modifierKeyState; // State of Shift/Ctrl/Alt/Meta keys.
    wkeMemBuf *m_filesystemId;
};

enum wkeHttBodyElementType
{
    wkeHttBodyElementTypeData,
    wkeHttBodyElementTypeFile,
};

struct wkePostBodyElement
{
    int size;
    wkeHttBodyElementType type;
    wkeMemBuf *data;
    wkeString filePath;
    __int64 fileStart;
    __int64 fileLength; // -1 means to the end of the file.
};

struct wkePostBodyElements
{
    int size;
    wkePostBodyElement **element;
    size_t elementSize;
    bool isDirty;
};

enum wkeResourceType
{
    WKE_RESOURCE_TYPE_MAIN_FRAME     = 0,   // top level page
    WKE_RESOURCE_TYPE_SUB_FRAME      = 1,   // frame or iframe
    WKE_RESOURCE_TYPE_STYLESHEET     = 2,   // a CSS stylesheet
    WKE_RESOURCE_TYPE_SCRIPT         = 3,   // an external script
    WKE_RESOURCE_TYPE_IMAGE          = 4,   // an image (jpg/gif/png/etc)
    WKE_RESOURCE_TYPE_FONT_RESOURCE  = 5,   // a font
    WKE_RESOURCE_TYPE_SUB_RESOURCE   = 6,   // an "other" subresource.
    WKE_RESOURCE_TYPE_OBJECT         = 7,   // an object (or embed) tag for a plugin, or a resource that a plugin requested.
    WKE_RESOURCE_TYPE_MEDIA          = 8,   // a media resource.
    WKE_RESOURCE_TYPE_WORKER         = 9,   // the main resource of a dedicated worker.
    WKE_RESOURCE_TYPE_SHARED_WORKER  = 10,  // the main resource of a shared worker.
    WKE_RESOURCE_TYPE_PREFETCH       = 11,  // an explicitly requested prefetch
    WKE_RESOURCE_TYPE_FAVICON        = 12,  // a favicon
    WKE_RESOURCE_TYPE_XHR            = 13,  // a XMLHttpRequest
    WKE_RESOURCE_TYPE_PING           = 14,  // a ping request for <a ping>
    WKE_RESOURCE_TYPE_SERVICE_WORKER = 15,  // the main resource of a service worker.
    WKE_RESOURCE_TYPE_LAST_TYPE
};

struct wkeWillSendRequestInfo
{
    wkeString url;
    wkeString newUrl;
    wkeResourceType resourceType;
    int httpResponseCode;
    wkeString method;
    wkeString referrer;
    void *headers;
};

struct wkeTempCallbackInfo
{
    int size;
    wkeWebFrameHandle frame;
    wkeWillSendRequestInfo *willSendRequestInfo;
    const char *url;
    wkePostBodyElements *postBody;
    wkeNetJob job;
};

struct wkeScreenshotSettings
{
    int structSize;
    int width;
    int height;
};


// miniblink 动态库的加载与初始化。内部使用引用计数，调用 Load 时当引用计数加1
// 此句必须在所有mb api前最先调用。并且所有mb api必须和调用wkeInit的线程为同个线程
// 参数：
// szWkeDll：miniblink 的路径，路径 + 文件名。
//   相对路径：相对exe的相对路径 + 文件名
//   绝对路径：全路径 + 文件名
// settings：
DUILIB_API bool LoadMiniBlink(LPCTSTR szWkeDll, const wkeSettings *settings = NULL);

// 调用后引用计数减1，当引用计数为0时，卸载 miniblink 动态库，所有mb api将不能再被调用，否则会有崩溃
DUILIB_API void FreeMiniBlink(void);

// 设置一些配置项
// 参数
// typedef struct { wkeProxy proxy; unsigned int mask; } wkeSettings; mask可以取:
// WKE_SETTING_PROXY: 效果和wkeSetProxy一样, 通过proxy设置
// WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD：这是个高级用法，开启时，on paint回调会在另外个线程（其实就是渲染线程）。
// 这个是用来实现多线程上屏功能，性能更快。
DUILIB_API void wkeConfigure(const wkeSettings *settings);
DUILIB_API bool wkeIsInitialize();
DUILIB_API void wkeUpdate();

DUILIB_API void wkeShutdown();
DUILIB_API void wkeShutdownForDebug(); // 测试使用，不了解千万别用！

// 获取版本号
DUILIB_API unsigned int wkeGetVersion();
DUILIB_API CDuiString wkeGetVersionStr();

// 创建一个webview，但不创建真窗口。一般用在离屏渲染里，如游戏
DUILIB_API wkeWebView wkeCreateWebView();
// 效果同wkeDestroyWebWindow
DUILIB_API void wkeDestroyWebView(wkeWebView webView);

// 设置一些webview相关的设置.目前只有背景颜色可以设置
DUILIB_API void wkeSetViewSettings(wkeWebView webView, const wkeViewSettings *settings);
// 开启一些实验性选项。
// 参数：
// debugString：
// "showDevTools"   开启开发者工具，此时param要填写开发者工具的资源路径，如file:///c:/miniblink-release/front_end/inspector.html。注意param此时必须是utf8编码
// "wakeMinInterval"        设置帧率，默认值是10，值越大帧率越低
// "drawMinInterval"        设置帧率，默认值是3，值越大帧率越低
// "antiAlias"              设置抗锯齿渲染。param必须设置为"1"
// "minimumFontSize"        最小字体
// "minimumLogicalFontSize" 最小逻辑字体
// "defaultFontSize"        默认字体
// "defaultFixedFontSize"   默认fixed字体
// "consoleOutput"          参数值为"1"时，可关闭console在dbgview下的输出
DUILIB_API void wkeSetDebugConfig(wkeWebView webView, const CDuiString &debugString, const CDuiString &param);
DUILIB_API void *wkeGetDebugConfig(wkeWebView webView, const CDuiString &debugString);

// 单独设置资源回收间隔
DUILIB_API void wkeSetResourceGc(wkeWebView webView, long intervalSec);

// 开启内存缓存。网页的图片等都会在内存缓存里。关闭后，内存使用会降低一些，但容易引起一些问题，如果不懂怎么用，最好别开
DUILIB_API void wkeSetMemoryCacheEnable(wkeWebView webView, bool b);
// 开启关闭鼠标消息，可以在开启触屏后，关闭鼠标消息
DUILIB_API void wkeSetMouseEnabled(wkeWebView webView, bool b);
// 开启触屏模式。开启后，鼠标消息将自动转换成触屏消息
DUILIB_API void wkeSetTouchEnabled(wkeWebView webView, bool b);
DUILIB_API void wkeSetContextMenuEnabled(wkeWebView webView, bool b);
// 关闭后，点a标签将不会弹出新窗口，而是在本窗口跳转
DUILIB_API void wkeSetNavigationToNewWindowEnable(wkeWebView webView, bool b);
// 关闭后，跨域检查将被禁止，此时可以做任何跨域操作，如跨域ajax，跨域设置iframe
DUILIB_API void wkeSetCspCheckEnable(wkeWebView webView, bool b);
// 开启关闭npapi插件，如flash
DUILIB_API void wkeSetNpapiPluginsEnabled(wkeWebView webView, bool b);
// 开启无头模式。开启后，将不会渲染页面，提升了网页性能。此功能方便用来实现一些爬虫，或者刷单工具
DUILIB_API void wkeSetHeadlessEnabled(wkeWebView webView, bool b);
// 可关闭拖拽文件加载网页
DUILIB_API void wkeSetDragEnable(wkeWebView webView, bool b);
// 可关闭拖拽到其他进程
DUILIB_API void wkeSetDragDropEnable(wkeWebView webView, bool b);
DUILIB_API void wkeSetLanguage(wkeWebView webView, const CDuiString &language);
// 设置某项menu是否显示
DUILIB_API void wkeSetContextMenuItemShow(wkeWebView webView, wkeMenuItemId item, bool isShow);

DUILIB_API void wkeSetViewNetInterface(wkeWebView webView, const CDuiString &netInterface);

// 设置整个mb的代码。此句是全局生效
DUILIB_API void wkeSetProxy(const wkeProxy *proxy);
// 设置整个mb的代码。此句是针对特定webview生效
DUILIB_API void wkeSetViewProxy(wkeWebView webView, wkeProxy *proxy);

DUILIB_API CDuiString wkeGetName(wkeWebView webView);
DUILIB_API void wkeSetName(wkeWebView webView, const CDuiString &name);

// 设置wkeWebView对应的窗口句柄。
// 注意：只有在无窗口模式下才能使用。如果是用wkeCreateWebWindow创建的webview，已经自带窗口句柄了。
DUILIB_API void wkeSetHandle(wkeWebView webView, HWND wnd);
// 设置无窗口模式下的绘制偏移。在某些情况下（主要是离屏模式），绘制的地方不在真窗口的(0, 0)处，就需要手动调用此接口void wkeSetCspCheckEnable(wkeWebView webView, bool b)
// 关闭后，跨域检查将被禁止，此时可以做任何跨域操作，如跨域ajax，跨域设置iframe
DUILIB_API void wkeSetHandleOffset(wkeWebView webView, int x, int y);

// 判断窗口是否是分层窗口（layer window）
DUILIB_API bool wkeIsTransparent(wkeWebView webView);
// 通知无窗口模式下，webview开启透明模式。
DUILIB_API void wkeSetTransparent(wkeWebView webView, bool transparent);

// 设置webview的UA
DUILIB_API void wkeSetUserAgent(wkeWebView webView, const CDuiString &userAgent);
// 获取webview的UA
DUILIB_API CDuiString wkeGetUserAgent(wkeWebView webView);

typedef void(WKE_CALL *wkeOnShowDevtoolsCallback)(wkeWebView webView, void *param);
DUILIB_API void wkeShowDevtools(wkeWebView webView, const CDuiString &path,
                                wkeOnShowDevtoolsCallback callback, void *param);

// 加载url。url必须是网络路径，如"http://qq.com/"
DUILIB_API void wkeLoadURL(wkeWebView webView, const CDuiString &url);
DUILIB_API void wkePostURL(wkeWebView wkeView, const CDuiString &url, const CDuiString &postData);
// 加载一段html
// 参数：略
// 注意：如果html里有相对路径，则是相对exe所在目录的路径
DUILIB_API void wkeLoadHTML(wkeWebView webView, const CDuiString &html);
// 加载一段html，但可以指定baseURL，也就是相对于哪个目录的url
DUILIB_API void wkeLoadHtmlWithBaseUrl(wkeWebView webView, const CDuiString &html, const CDuiString &baseUrl);

DUILIB_API void wkeLoadFile(wkeWebView webView, const CDuiString &filename);

// 获取webview主frame的url
DUILIB_API CDuiString wkeGetURL(wkeWebView webView);

// 获取frame对应的url
DUILIB_API CDuiString wkeGetFrameUrl(wkeWebView webView, wkeWebFrameHandle frameId);

DUILIB_API bool wkeIsLoading(wkeWebView webView);
DUILIB_API bool wkeIsLoadingSucceeded(wkeWebView webView);
DUILIB_API bool wkeIsLoadingFailed(wkeWebView webView);
DUILIB_API bool wkeIsLoadingCompleted(wkeWebView webView);
// DOM 文档结构是否加载完成。
DUILIB_API bool wkeIsDocumentReady(wkeWebView webView);
// 停止加载页面
DUILIB_API void wkeStopLoading(wkeWebView webView);
// 重新加载页面
DUILIB_API void wkeReload(wkeWebView webView);
DUILIB_API void wkeGoToOffset(wkeWebView webView, int offset);
DUILIB_API void wkeGoToIndex(wkeWebView webView, int index);

DUILIB_API int wkeGetWebviewId(wkeWebView webView);
DUILIB_API bool wkeIsWebviewAlive(int id);
DUILIB_API bool wkeIsWebviewValid(wkeWebView webView);

DUILIB_API CDuiString wkeGetDocumentCompleteURL(wkeWebView webView, wkeWebFrameHandle frameId,
                                                const CDuiString &partialURL);

DUILIB_API wkeMemBuf *wkeCreateMemBuf(wkeWebView webView, void *buf, size_t length);
DUILIB_API void wkeFreeMemBuf(wkeMemBuf *buf);

// 获取页面标题
DUILIB_API CDuiString wkeGetTitle(wkeWebView webView);

// 重新设置页面的宽高。如果webView是带窗口模式的，会设置真窗口的宽高。
DUILIB_API void wkeResize(wkeWebView webView, int w, int h);
// 获取页面宽度
DUILIB_API int wkeGetWidth(wkeWebView webView);
// 获取页面高度
DUILIB_API int wkeGetHeight(wkeWebView webView);
// 获取网页排版出来的宽度
DUILIB_API int wkeGetContentWidth(wkeWebView webView);
// 获取网页排版出来的高度
DUILIB_API int wkeGetContentHeight(wkeWebView webView);

DUILIB_API void wkeSetDirty(wkeWebView webView, bool dirty);
DUILIB_API bool wkeIsDirty(wkeWebView webView);
DUILIB_API void wkeAddDirtyArea(wkeWebView webView, int x, int y, int w, int h);
DUILIB_API void wkeLayoutIfNeeded(wkeWebView webView);
// 获取页面像素。
// 参数：
// bits 外部申请并传递给mb的buffer，大小是bufWid * bufHei * 4 字节
// bufW、bufH  bits的宽高
// xDst、yDst  绘制到bits的哪个坐标
// w、h、xSrc、ySrc  mb需要取的画面的起始坐标
// bCopyAlpha   是否拷贝画面的透明度值
// 注意：此函数一般给3d游戏使用。另外频繁使用此接口并拷贝像素有性能问题。最好用wkeGetViewDC再去拷贝dc。
DUILIB_API void wkePaint2(wkeWebView webView, void *bits, int bufW, int bufH, int xDst, int yDst, int w,
                          int h, int xSrc, int ySrc, bool bCopyAlpha);
// 获取页面的像素的简化版函数。
// 参数：
// bits：外部申请并传递给mb的buffer，大小是webview宽度 * 高度 * 4 字节。
// pitch：填0即可。这个参数玩过directX的人应该懂
DUILIB_API void wkePaint(wkeWebView webView, void *bits, int pitch);
DUILIB_API void wkeRepaintIfNeeded(wkeWebView webView);
// 获取webview的DC
DUILIB_API HDC wkeGetViewDC(wkeWebView webView);
// 获取webveiw对应的窗口句柄。实现和wkeGetWindowHandle完全相同
DUILIB_API HWND wkeGetHostHWND(wkeWebView webView);

// 页面是否可以后退，页面后退
DUILIB_API bool wkeCanGoBack(wkeWebView webView);
DUILIB_API bool wkeGoBack(wkeWebView webView);
// 页面是否可以前进，页面前进
DUILIB_API bool wkeCanGoForward(wkeWebView webView);
DUILIB_API bool wkeGoForward(wkeWebView webView);

// 给webview发送全选、取消选择命令
DUILIB_API void wkeEditorSelectAll(wkeWebView webView);
DUILIB_API void wkeEditorUnSelect(wkeWebView webView);
// 拷贝、剪切、粘贴、删除、撤消、重做 页面里被选中的字符串
DUILIB_API void wkeEditorCopy(wkeWebView webView);
DUILIB_API void wkeEditorCut(wkeWebView webView);
DUILIB_API void wkeEditorPaste(wkeWebView webView);
DUILIB_API void wkeEditorDelete(wkeWebView webView);
DUILIB_API void wkeEditorUndo(wkeWebView webView);
DUILIB_API void wkeEditorRedo(wkeWebView webView);

// 获取页面的cookie
DUILIB_API CDuiString wkeGetCookie(wkeWebView webView);
// 设置页面cookie。
// 注意：cookie必须符合curl的cookie写法。一个例子是：PERSONALIZE=123;expires=Monday, 13-Jun-2022 03:04:55 GMT; domain=.fidelity.com; path=/; secure
DUILIB_API void wkeSetCookie(wkeWebView webView, const CDuiString &url, const CDuiString &cookie);

typedef bool(WKE_CALL *wkeCookieVisitor)(
    void *params,
    const utf8 *name,
    const utf8 *value,
    const utf8 *domain,
    const utf8 *path, // If |path| is non-empty only URLs at or below the path will get the cookie value.
    int secure, // If |secure| is true the cookie will only be sent for HTTPS requests.
    int httpOnly, // If |httponly| is true the cookie will only be sent for HTTP requests.
    int *expires // The cookie expiration date is only valid if |has_expires| is true.
);
// 通过访问器visitor访问所有cookie。
// 参数：
// visitor：访问器。
DUILIB_API void wkeVisitAllCookie(void *params, wkeCookieVisitor visitor);

enum wkeCookieCommand
{
    wkeCookieCommandClearAllCookies,
    wkeCookieCommandClearSessionCookies,
    wkeCookieCommandFlushCookiesToFile,
    wkeCookieCommandReloadCookiesFromFile,
};
// 通过设置mb内置的curl来操作cookie。
// 参数：
// command:
// wkeCookieCommandClearAllCookies: 内部实际执行了curl_easy_setopt(curl, CURLOPT_COOKIELIST, "ALL");
// wkeCookieCommandClearSessionCookies: curl_easy_setopt(curl, CURLOPT_COOKIELIST, "SESS");
// wkeCookieCommandFlushCookiesToFile: curl_easy_setopt(curl, CURLOPT_COOKIELIST, "FLUSH");
// wkeCookieCommandReloadCookiesFromFile: curl_easy_setopt(curl, CURLOPT_COOKIELIST, "RELOAD");
// 注意：这个接口只是调用curl设置命令，并不会去修改js里的内容
DUILIB_API void wkePerformCookieCommand(wkeWebView webView, wkeCookieCommand command);

// 开启或关闭cookie
// 注意：这个接口只是影响blink，并不会设置curl。所以还是会生成curl的cookie文件
DUILIB_API void wkeSetCookieEnabled(wkeWebView webView, bool enable);
DUILIB_API bool wkeIsCookieEnabled(wkeWebView webView);
// 设置cookie的本地文件目录。默认是当前目录。cookies存在当前目录的“cookie.dat”里
DUILIB_API void wkeSetCookieJarPath(wkeWebView webView, const CDuiString &path);
// 设置cookie的全路径+文件名，如“c:\mb\cookie.dat”
DUILIB_API void wkeSetCookieJarFullPath(wkeWebView webView, const CDuiString &path);
DUILIB_API void wkeClearCookie(wkeWebView webView);
// 设置local storage的全路径。如“c:\mb\LocalStorage\”
// 注意：这个接口只能接受目录。
DUILIB_API void wkeSetLocalStorageFullPath(wkeWebView webView, const CDuiString &path);
DUILIB_API void wkeAddPluginDirectory(wkeWebView webView, const CDuiString &path);

// 设置、获取 音量
DUILIB_API void wkeSetMediaVolume(wkeWebView webView, float volume);
DUILIB_API float wkeGetMediaVolume(wkeWebView webView);

// 向mb发送鼠标消息
// 参数：
// message：可取WM_MOUSELEAVE等Windows相关鼠标消息
// x、y：坐标
// flags：可取值有WKE_CONTROL、WKE_SHIFT、WKE_LBUTTON、WKE_MBUTTON、WKE_RBUTTON，可通过“或”操作并联。
DUILIB_API bool wkeFireMouseEvent(wkeWebView webView, unsigned int message, int x, int y, unsigned int flags);
// 向mb发送菜单消息（未实现）
DUILIB_API bool wkeFireContextMenuEvent(wkeWebView webView, int x, int y, unsigned int flags);
// 向mb发送滚轮消息，用法和参数类似wkeFireMouseEvent。
DUILIB_API bool wkeFireMouseWheelEvent(wkeWebView webView, int x, int y, int delta, unsigned int flags);
// 向mb发送WM_KEYUP消息，
// 参数：
// virtualKeyCode：见https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
// flags：可取值有WKE_REPEAT、WKE_EXTENDED，可通过“或”操作并联。 systemKey：暂时没用
DUILIB_API bool wkeFireKeyUpEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags,
                                  bool systemKey);
DUILIB_API bool wkeFireKeyDownEvent(wkeWebView webView, unsigned int virtualKeyCode, unsigned int flags,
                                    bool systemKey);
// 参数：
// charCode：WM_CHAR消息的The character code of the key.
// 见https://msdn.microsoft.com/en-us/library/windows/desktop/ms646276(v=vs.85).aspx
DUILIB_API bool wkeFireKeyPressEvent(wkeWebView webView, unsigned int charCode, unsigned int flags,
                                     bool systemKey);
// 向mb发送任意windows消息。不过目前mb主要用来处理光标相关。mb在无窗口模式下，要响应光标事件，需要通过本函数手动发送光标消息
DUILIB_API bool wkeFireWindowsMessage(wkeWebView webView, HWND hWnd, UINT message, WPARAM wParam,
                                      LPARAM lParam, LRESULT *result);

// 设置webview是焦点态。如果webveiw关联了窗口，窗口也会有焦点
DUILIB_API void wkeSetFocus(wkeWebView webView);
DUILIB_API void wkeKillFocus(wkeWebView webView);

// 获取编辑框的那个游标的位置
DUILIB_API wkeRect wkeGetCaretRect(wkeWebView webView);

// 运行一段js。返回js的值jsValue。jsValue是个封装了内部v8各种类型的类，如果需要获取详细信息，有jsXXX相关接口可以调用。见下述。
// 注意，此函数以及wkeRunJS，执行的js，也就是script，是在一个闭包中
DUILIB_API jsValue wkeRunJS(wkeWebView webView, const CDuiString &script);

// 获取页面主frame的jsExecState。jsExecState是什么，见下述。
DUILIB_API jsExecState wkeGlobalExec(wkeWebView webView);
DUILIB_API jsExecState wkeGetGlobalExecByFrame(wkeWebView webView, wkeWebFrameHandle frameId);

DUILIB_API void wkeSleep(wkeWebView webView);
DUILIB_API void wkeWake(wkeWebView webView);
DUILIB_API bool wkeIsAwake(wkeWebView webView);

// 设置页面缩放系数，默认是1
DUILIB_API void wkeSetZoomFactor(wkeWebView webView, float factor);
DUILIB_API float wkeGetZoomFactor(wkeWebView webView);

DUILIB_API void wkeSetEditable(wkeWebView webView, bool editable);

// 获取wkeString结构体对应的字符串，ansi/utf16编码
DUILIB_API CDuiString wkeGetString(const wkeString s);
// 设置wkeString结构体对应的字符串，ansi/utf16编码
DUILIB_API void wkeSetString(wkeString string, const CDuiString &str);

// 通过utf16编码的字符串，创建一个wkeString
DUILIB_API wkeString wkeCreateString(const CDuiString &str);
DUILIB_API size_t wkeGetStringLen(wkeString str);
// 析构这个wkeString
DUILIB_API void wkeDeleteString(wkeString str);

DUILIB_API wkeWebView wkeGetWebViewForCurrentContext();
// 对webView设置一个key value键值对。可以用来保存用户自己定义的任何指针
DUILIB_API void wkeSetUserKeyValue(wkeWebView webView, const CDuiString &key, void *value);
DUILIB_API void *wkeGetUserKeyValue(wkeWebView webView, const CDuiString &key);


enum WkeCursorInfoType
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
};
DUILIB_API int wkeGetCursorInfoType(wkeWebView webView);
DUILIB_API void wkeSetCursorInfoType(wkeWebView webView, int type);

DUILIB_API void wkeSetDragFiles(wkeWebView webView, const POINT *clintPos, const POINT *screenPos,
                                wkeString files[], int filesCount);

// 设置mb模拟的硬件设备环境。主要用在伪装手机设备场景
// 参数：
// device：设备的字符串。可取值有：
// "navigator.maxTouchPoints"   此时 paramInt 需要被设置，表示 touch 的点数
// "navigator.platform"         此时 paramStr 需要被设置，表示js里获取的 navigator.platform字符串
// "navigator.hardwareConcurrency"  此时 paramInt 需要被设置，表示js里获取的 navigator.hardwareConcurrency 整数值
// "screen.width"       此时 paramInt 需要被设置，表示js里获取的 screen.width 整数值
// "screen.height"      此时 paramInt 需要被设置，表示js里获取的 screen.height 整数值
// "screen.availWidth"  此时 paramInt 需要被设置，表示js里获取的 screen.availWidth 整数值
// "screen.availHeight" 此时 paramInt 需要被设置，表示js里获取的 screen.availHeight 整数值
// "screen.pixelDepth"  此时 paramInt 需要被设置，表示js里获取的 screen.pixelDepth 整数值
// "screen.pixelDepth"  目前等价于"screen.pixelDepth"
// "window.devicePixelRatio"    同上
// "navigator.connection.type"
DUILIB_API void wkeSetDeviceParameter(wkeWebView webView, const CDuiString &device,
                                      const CDuiString &paramStr, int paramInt, float paramFloat);

DUILIB_API wkeTempCallbackInfo *wkeGetTempCallbackInfo(wkeWebView webView);


//wke callback-----------------------------------------------------------------------------------
typedef void(WKE_CALL *wkeTitleChangedCallback)(wkeWebView webView, void *param, const wkeString title);
// 设置标题变化的通知回调
// 参数：typedef void(*wkeTitleChangedCallback)(wkeWebView webView, void* param, const wkeString title);
// title：标题的字符串封装。wkeString怎么用，见相关接口。 param：通过wkeOnTitleChanged的callbackParam设置
DUILIB_API void wkeOnTitleChanged(wkeWebView webView, wkeTitleChangedCallback callback, void *callbackParam);
// 鼠标划过的元素，如果是，则调用此回调，并发送a标签的url
DUILIB_API void wkeOnMouseOverUrlChanged(wkeWebView webView, wkeTitleChangedCallback callback,
                                         void *callbackParam);

typedef void(WKE_CALL *wkeURLChangedCallback)(wkeWebView webView, void *param, const wkeString url);
// url改变回调
DUILIB_API void wkeOnURLChanged(wkeWebView webView, wkeURLChangedCallback callback, void *callbackParam);
typedef void(WKE_CALL *wkeURLChangedCallback2)(wkeWebView webView, void *param,
                                               wkeWebFrameHandle frameId, const wkeString url);
// 和上个接口不同的是，回调多了个参数
// 参数：typedef void(*wkeURLChangedCallback2)(wkeWebView webView, void* param, wkeWebFrameHandle frameId, const wkeString url)
// frameId：表示frame的id。有相关接口可以判断这个frameId是否是主frame
DUILIB_API void wkeOnURLChanged2(wkeWebView webView, wkeURLChangedCallback2 callback, void *callbackParam);

typedef void(WKE_CALL *wkePaintUpdatedCallback)(wkeWebView webView, void *param, const HDC hdc,
                                                int x, int y, int cx, int cy);
// 页面有任何需要刷新的地方，将调用此回调
// 参数：typedef void(*wkePaintUpdatedCallback)(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
// x、y、cx、cy表示刷新的区域矩形
DUILIB_API void wkeOnPaintUpdated(wkeWebView webView, wkePaintUpdatedCallback callback, void *callbackParam);

typedef void(WKE_CALL *wkePaintBitUpdatedCallback)(wkeWebView webView, void *param, const void *buffer,
                                                   const wkeRect *r, int width, int height);
// 同上。不同的是回调过来的是填充好像素的buffer，而不是DC。方便嵌入到游戏中做离屏渲染
DUILIB_API void wkeOnPaintBitUpdated(wkeWebView webView, wkePaintBitUpdatedCallback callback,
                                     void *callbackParam);

// 网页调用alert会走到这个接口填入的回调
typedef void(WKE_CALL *wkeAlertBoxCallback)(wkeWebView webView, void *param, const wkeString msg);
DUILIB_API void wkeOnAlertBox(wkeWebView webView, wkeAlertBoxCallback callback, void *callbackParam);

typedef bool(WKE_CALL *wkeConfirmBoxCallback)(wkeWebView webView, void *param, const wkeString msg);
DUILIB_API void wkeOnConfirmBox(wkeWebView webView, wkeConfirmBoxCallback callback, void *callbackParam);

typedef bool(WKE_CALL *wkePromptBoxCallback)(wkeWebView webView, void *param, const wkeString msg,
                                             const wkeString defaultResult, wkeString result);
DUILIB_API void wkeOnPromptBox(wkeWebView webView, wkePromptBoxCallback callback, void *callbackParam);

typedef bool(WKE_CALL *wkeNavigationCallback)(wkeWebView webView, void *param,
                                              wkeNavigationType navigationType, wkeString url);

// 网页开始浏览将触发回调
// 参数：typedef bool(*wkeNavigationCallback)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url);
// wkeNavigationType: 表示浏览触发的原因。可以取的值有：
// WKE_NAVIGATION_TYPE_LINKCLICK：点击a标签触发
// WKE_NAVIGATION_TYPE_FORMSUBMITTE：点击form触发
// WKE_NAVIGATION_TYPE_BACKFORWARD：前进后退触发
// WKE_NAVIGATION_TYPE_RELOAD：重新加载触发
// 注意：wkeNavigationCallback回调的返回值，如果是true，表示可以继续进行浏览，false表示阻止本次浏览。
DUILIB_API void wkeOnNavigation(wkeWebView webView, wkeNavigationCallback callback, void *param);

struct wkeWindowFeatures
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
};

typedef wkeWebView(WKE_CALL *wkeCreateViewCallback)(wkeWebView webView, void *param,
                                                    wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures *windowFeatures);
// 网页点击a标签创建新窗口时将触发回调
// 参数：typedef wkeWebView(*wkeCreateViewCallback)(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures);
DUILIB_API void wkeOnCreateView(wkeWebView webView, wkeCreateViewCallback callback, void *param);

typedef void(WKE_CALL *wkeDocumentReadyCallback)(wkeWebView webView, void *param);
// 对应js里的body onload事件
DUILIB_API void wkeOnDocumentReady(wkeWebView webView, wkeDocumentReadyCallback callback, void *param);

typedef void(WKE_CALL *wkeDocumentReady2Callback)(wkeWebView webView, void *param, wkeWebFrameHandle frameId);
// 同上。区别是wkeDocumentReady2Callback多了wkeWebFrameHandle frameId参数。可以判断是否是主frame
DUILIB_API void wkeOnDocumentReady2(wkeWebView webView, wkeDocumentReady2Callback callback, void *param);

enum wkeLoadingResult
{
    WKE_LOADING_SUCCEEDED,
    WKE_LOADING_FAILED,
    WKE_LOADING_CANCELED
};

typedef void(WKE_CALL *wkeLoadingFinishCallback)(wkeWebView webView, void *param, const wkeString url,
                                                 wkeLoadingResult result, const wkeString failedReason);
DUILIB_API void wkeOnLoadingFinish(wkeWebView webView, wkeLoadingFinishCallback callback, void *param);

typedef bool(WKE_CALL *wkeDownloadCallback)(wkeWebView webView, void *param, const char *url);
// 页面下载事件回调。点击某些链接，触发下载会调用
DUILIB_API void wkeOnDownload(wkeWebView webView, wkeDownloadCallback callback, void *param);

enum wkeDownloadOpt
{
    kWkeDownloadOptCancel,
    kWkeDownloadOptCacheData,
};

typedef void(WKE_CALL *wkeNetJobDataRecvCallback)(void *ptr, wkeNetJob job, const char *data, int length);
typedef void(WKE_CALL *wkeNetJobDataFinishCallback)(void *ptr, wkeNetJob job, wkeLoadingResult result);

struct wkeNetJobDataBind
{
    void *ptr;
    wkeNetJobDataRecvCallback recvCallback;
    wkeNetJobDataFinishCallback finishCallback;
};

typedef wkeDownloadOpt(WKE_CALL *wkeDownload2Callback)(
    wkeWebView webView,
    void *param,
    size_t expectedContentLength,
    const char *url,
    const char *mime,
    const char *disposition,
    wkeNetJob job,
    wkeNetJobDataBind *dataBind);
DUILIB_API void wkeOnDownload2(wkeWebView webView, wkeDownload2Callback callback, void *param);

enum wkeConsoleLevel
{
    wkeLevelDebug        = 4,
    wkeLevelLog          = 1,
    wkeLevelInfo         = 5,
    wkeLevelWarning      = 2,
    wkeLevelError        = 3,
    wkeLevelRevokedError = 6,
    wkeLevelLast         = wkeLevelInfo
};

typedef void(WKE_CALL *wkeConsoleCallback)(wkeWebView webView, void *param, wkeConsoleLevel level,
                                           const wkeString message, const wkeString sourceName, unsigned sourceLine, const wkeString stackTrace);
// 网页调用console触发
DUILIB_API void wkeOnConsole(wkeWebView webView, wkeConsoleCallback callback, void *param);

typedef void(WKE_CALL *wkeOnCallUiThread)(wkeWebView webView, void *paramOnInThread);
typedef void(WKE_CALL *wkeCallUiThread)(wkeWebView webView, wkeOnCallUiThread func, void *param);
DUILIB_API void wkeSetUIThreadCallback(wkeWebView webView, wkeCallUiThread callback, void *param);

//wkeNet--------------------------------------------------------------------------------------
typedef bool(WKE_CALL *wkeLoadUrlBeginCallback)(wkeWebView webView, void *param, const utf8 *url,
                                                wkeNetJob job);
// 任何网络请求发起前会触发此回调
// 参数：typedef bool(*wkeLoadUrlBeginCallback)(wkeWebView webView, void* param, const utf8 *url, void *job)
// 注意：
// 1，此回调功能强大，在回调里，如果对job设置了wkeNetHookRequest，则表示mb会缓存获取到的网络数据，并在这次网络请求 结束后调用wkeOnLoadUrlEnd设置的回调，同时传递缓存的数据。在此期间，mb不会处理网络数据。
// 2，如果在wkeLoadUrlBeginCallback里没设置wkeNetHookRequest，则不会触发wkeOnLoadUrlEnd回调。
// 3，如果wkeLoadUrlBeginCallback回调里返回true，表示mb不处理此网络请求（既不会发送网络请求）。返回false，表示mb依然会发送网络请求。
// 用法举例：
// 假如需要hook百度某个url（如httdiv://baidu.com/a.js）,替换为本地c:\b.js，则可以这样实现：
// void readJsFile(const wchar_t* divath, std::vector* buffer) {
//      HANDLE hFile = CreateFileW(divath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//      if (INVALID_HANDLE_VALUE == hFile) {
//          DebugBreak();
//          return;
//      }
//
//      DWORD fileSizeHigh;
//      const DWORD bufferSize = ::GetFileSize(hFile, &amdiv;fileSizeHigh);
//
//      DWORD numberOfBytesRead = 0;
//      buffer->resize(bufferSize);
//      BOOL b = ::ReadFile(hFile, &amdiv;buffer->at(0), bufferSize, &amdiv;numberOfBytesRead, nulldivtr);
//      ::CloseHandle(hFile);
//
//  }
//
//  static bool HookUrl(void* job, const utf8* url, const utf8* hookUrl, const wchar_t* localFile, const utf8* mime) {
//      if (0 != strstr(url, hookUrl)) {
//          wkeNetSetMIMETydive(job, (utf8*)mime); // "text/html" "text/javascridivt"
//          std::vector buffer;
//              ReadJsFile(localFile, &amdiv;buffer);
//              wkeNetSetData(job, &amdiv;buffer[0], buffer.size());
//              return true;
//      }
//
//      return false;
//  }
//
//  bool handleLoadUrlBegin(wkeWebView webView, void* divaram, const utf8* url, void* job) {
//      if (HookUrl(job, url, "httdiv://baidu.com/a.js", L"c:\\b.js", "text/javascridivt")) {
//          return true;
//      }
//      return false;
//  }
//
// 如果需要拿到httdiv://baidu.com/a.js的真实网络数据再修改，则可以：
// bool handleLoadUrlBegin(wkeWebView webView, void* divaram, const utf8* url, void* job) {
//     if (0 != strstr(url, "httdiv://baidu.com/a.js")) {
//         wkeNetHookRequest(job);
//         return false;
//     }
//     return false;
// }
//
// void handleLoadUrlEnd(wkeWebView webView, void* divaram, const utf8* url, void* job, void* buf, int len) {
//     utf8 code[] = "console.log('test')";
//     wkeNetSetData(job, code, sizeof(code));
// }
DUILIB_API void wkeOnLoadUrlBegin(wkeWebView webView, wkeLoadUrlBeginCallback callback, void *callbackParam);

typedef void(WKE_CALL *wkeLoadUrlEndCallback)(wkeWebView webView, void *param, const char *url,
                                              wkeNetJob job, void *buf, int len);
// 见wkeOnLoadUrlBegin的描述
DUILIB_API void wkeOnLoadUrlEnd(wkeWebView webView, wkeLoadUrlEndCallback callback, void *callbackParam);

typedef void(WKE_CALL *wkeLoadUrlFailCallback)(wkeWebView webView, void *param, const utf8 *url,
                                               wkeNetJob job);
DUILIB_API void wkeOnLoadUrlFail(wkeWebView webView, wkeLoadUrlFailCallback callback, void *callbackParam);

typedef void(WKE_CALL *wkeDidCreateScriptContextCallback)(wkeWebView webView, void *param,
                                                          wkeWebFrameHandle frameId, void *context, int extensionGroup, int worldId);
// javascript的v8执行环境被创建时触发此回调
// 参数：略
// 注意：每个frame创建时都会触发此回调
DUILIB_API void wkeOnDidCreateScriptContext(wkeWebView webView, wkeDidCreateScriptContextCallback callback,
                                            void *callbackParam);

typedef void(WKE_CALL *wkeWillReleaseScriptContextCallback)(wkeWebView webView, void *param,
                                                            wkeWebFrameHandle frameId, void *context, int worldId);
// 每个frame的javascript的v8执行环境被关闭时触发此回调
DUILIB_API void wkeOnWillReleaseScriptContext(wkeWebView webView,
                                              wkeWillReleaseScriptContextCallback callback, void *param);

typedef bool(WKE_CALL *wkeWindowClosingCallback)(wkeWebView webView, void *param);
// wkeWebView如果是真窗口模式，则在收到WM_CLODE消息时触发此回调。可以通过在回调中返回false拒绝关闭窗口
DUILIB_API void wkeOnWindowClosing(wkeWebView webView, wkeWindowClosingCallback callback, void *param);

typedef void(WKE_CALL *wkeWindowDestroyCallback)(wkeWebView webView, void *param);
// 窗口即将被销毁时触发回调。不像wkeOnWindowClosing，这个操作无法取消
DUILIB_API void wkeOnWindowDestroy(wkeWebView webView, wkeWindowDestroyCallback callback, void *param);


struct wkeDraggableRegion
{
    RECT bounds;
    bool draggable;
};
typedef void(WKE_CALL *wkeDraggableRegionsChangedCallback)(wkeWebView webView, void *param,
                                                           const wkeDraggableRegion *rects, int rectCount);
DUILIB_API void wkeOnDraggableRegionsChanged(wkeWebView webView, wkeDraggableRegionsChangedCallback callback,
                                             void *param);

struct wkeMediaLoadInfo
{
    int size;
    int width;
    int height;
    double duration;
};
typedef void(WKE_CALL *wkeWillMediaLoadCallback)(wkeWebView webView, void *param, const char *url,
                                                 wkeMediaLoadInfo *info);
// video等多媒体标签创建时触发此回调
DUILIB_API void wkeOnWillMediaLoad(wkeWebView webView, wkeWillMediaLoadCallback callback,
                                   void *callbackParam);

typedef wkeString(WKE_CALL*wkeImageBufferToDataURL)(wkeWebView webView, void* param, const char* data, size_t size);

typedef void(WKE_CALL *wkeStartDraggingCallback)(
    wkeWebView webView,
    void *param,
    wkeWebFrameHandle frame,
    const wkeWebDragData *data,
    wkeWebDragOperationsMask mask,
    const void *image,
    const wkePoint *dragImageOffset
);
DUILIB_API void wkeOnStartDragging(wkeWebView webView, wkeStartDraggingCallback callback, void *param);

typedef void(WKE_CALL *wkeOnPrintCallback)(wkeWebView webView, void *param, wkeWebFrameHandle frameId,
                                           void *printParams);
DUILIB_API void wkeOnPrint(wkeWebView webView, wkeOnPrintCallback callback, void *param);

typedef void(WKE_CALL *wkeOnScreenshot)(wkeWebView webView, void *param, const char *data, size_t size);
DUILIB_API void wkeScreenshot(wkeWebView webView,
                              const wkeScreenshotSettings *settings,
                              wkeOnScreenshot callback, void *param);

enum wkeOtherLoadType
{
    WKE_DID_START_LOADING,
    WKE_DID_STOP_LOADING,
    WKE_DID_NAVIGATE,
    WKE_DID_NAVIGATE_IN_PAGE,
    WKE_DID_GET_RESPONSE_DETAILS,
    WKE_DID_GET_REDIRECT_REQUEST,
    WKE_DID_POST_REQUEST,
};
typedef void(WKE_CALL *wkeOnOtherLoadCallback)(wkeWebView webView, void *param, wkeOtherLoadType type,
                                               wkeTempCallbackInfo *info);
DUILIB_API void wkeOnOtherLoad(wkeWebView webView, wkeOnOtherLoadCallback callback, void *param);

DUILIB_API bool wkeIsProcessingUserGesture(wkeWebView webView);

// 在wkeOnLoadUrlBegin回调里调用，表示设置http请求的MIME type
DUILIB_API void wkeNetSetMIMEType(wkeNetJob jobPtr, const CDuiString &type);
// 参数：第2个参数可以传nullptr
DUILIB_API CDuiString wkeNetGetMIMEType(wkeNetJob jobPtr, wkeString mime);
// 在wkeOnLoadUrlBegin回调里调用，表示设置http请求（或者file:///协议）的 http header field。response一直要被设置成false
DUILIB_API void wkeNetSetHTTPHeaderField(wkeNetJob jobPtr, const CDuiString &key, const CDuiString &value,
                                         bool response);

DUILIB_API CDuiString wkeNetGetHTTPHeaderField(wkeNetJob jobPtr, const CDuiString &key);
DUILIB_API CDuiString wkeNetGetHTTPHeaderFieldFromResponse(wkeNetJob jobPtr, const CDuiString &key);

// 在wkeOnLoadUrlEnd里被调用，表示设置hook后缓存的数据
// 调用此函数后,网络层收到数据会存储在一buf内,接收数据完成后响应OnLoadUrlEnd事件.#此调用严重影响性能,慎用
// 此函数和wkeNetHookRequest的区别是，wkeNetHookRequest会在接受到真正网络数据后再调用回调，并允许回调修改网络数据。"\
// 而wkeNetSetData是在网络数据还没发送的时候修改
DUILIB_API void wkeNetSetData(wkeNetJob jobPtr, void *buf, int len);
DUILIB_API void wkeNetHookRequest(wkeNetJob jobPtr);

typedef bool(WKE_CALL *wkeNetResponseCallback)(wkeWebView webView, void *param, const utf8 *url,
                                               wkeNetJob job);
// 一个网络请求发送后，收到服务器response触发回调
DUILIB_API void wkeNetOnResponse(wkeWebView webView, wkeNetResponseCallback callback, void *param);

enum wkeRequestType
{
    kWkeRequestTypeInvalidation,
    kWkeRequestTypeGet,
    kWkeRequestTypePost,
    kWkeRequestTypePut,
};

// 获取此请求的method，如post还是get
DUILIB_API wkeRequestType wkeNetGetRequestMethod(wkeNetJob jobPtr);

typedef void(WKE_CALL *wkeOnNetGetFaviconCallback)(wkeWebView webView, void *param, const utf8 *url,
                                                   wkeMemBuf *buf);
// 获取favicon。
// 参数：略
// 注意：此接口必须在wkeOnLoadingFinish回调里调用。可以用下面方式来判断是否是主frame的LoadingFinish:
//      wkeTempCallbackInfo* temInfo = wkeGetTempCallbackInfo(webView);
//      if (::wkeIsMainFrame(webView, temInfo->frame)) {
//          ::wkeNetGetFavicon(webView, HandleFaviconReceived, divaram);
//      }
DUILIB_API int wkeNetGetFavicon(wkeWebView webView, wkeOnNetGetFaviconCallback callback, void *param);

DUILIB_API void wkeNetContinueJob(wkeNetJob jobPtr);
DUILIB_API CDuiString wkeNetGetUrlByJob(wkeNetJob jobPtr);
DUILIB_API const wkeSlist *wkeNetGetRawHttpHead(wkeNetJob jobPtr);
DUILIB_API const wkeSlist *wkeNetGetRawResponseHead(wkeNetJob jobPtr);
// 在wkeOnLoadUrlBegin回调里调用，设置后，此请求将被取消。
DUILIB_API void wkeNetCancelRequest(wkeNetJob jobPtr);

// 高级用法。在wkeOnLoadUrlBegin回调里调用。 有时候，wkeOnLoadUrlBegin里拦截到一个请求后，不能马上判断出结果。此时可以调用本接口，然后在 异步的某个时刻，调用wkeNetContinueJob来让此请求继续进行
// 参数：略
// 返回值：TRUE代表成功，FALSE代表调用失败，不能再调用wkeNetContinueJob了
DUILIB_API BOOL wkeNetHoldJobToAsynCommit(wkeNetJob jobPtr);

DUILIB_API void wkeNetChangeRequestUrl(wkeNetJob jobPtr, const CDuiString &url);

typedef struct wkeWebUrlRequest *wkeWebUrlRequestPtr;
typedef struct wkeWebUrlResponse *wkeWebUrlResponsePtr;

DUILIB_API wkeWebUrlRequestPtr wkeNetCreateWebUrlRequest(const CDuiString &url, const CDuiString &method,
                                                         const CDuiString &mime);
DUILIB_API wkeWebUrlRequestPtr wkeNetCreateWebUrlRequest2(const blinkWebURLRequestPtr request);
DUILIB_API blinkWebURLRequestPtr wkeNetCopyWebUrlRequest(wkeNetJob jobPtr, bool needExtraData);
DUILIB_API void wkeNetDeleteBlinkWebURLRequestPtr(blinkWebURLRequestPtr request);
DUILIB_API void wkeNetAddHTTPHeaderFieldToUrlRequest(wkeWebUrlRequestPtr request, const CDuiString &name,
                                                     const CDuiString &value);

typedef void(WKE_CALL *wkeOnUrlRequestWillRedirectCallback)(wkeWebView webView, void *param,
                                                            wkeWebUrlRequestPtr oldRequest, wkeWebUrlRequestPtr request,
                                                            wkeWebUrlResponsePtr redirectResponse);
typedef void(WKE_CALL *wkeOnUrlRequestDidReceiveResponseCallback)(wkeWebView webView, void *param,
        wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr response);
typedef void(WKE_CALL *wkeOnUrlRequestDidReceiveDataCallback)(wkeWebView webView, void *param,
        wkeWebUrlRequestPtr request, const char *data, int dataLength);
typedef void(WKE_CALL *wkeOnUrlRequestDidFailCallback)(wkeWebView webView, void *param,
                                                       wkeWebUrlRequestPtr request, const utf8 *error);
typedef void(WKE_CALL *wkeOnUrlRequestDidFinishLoadingCallback)(wkeWebView webView, void *param,
        wkeWebUrlRequestPtr request, double finishTime);

struct wkeUrlRequestCallbacks
{
    wkeOnUrlRequestWillRedirectCallback willRedirectCallback;
    wkeOnUrlRequestDidReceiveResponseCallback didReceiveResponseCallback;
    wkeOnUrlRequestDidReceiveDataCallback didReceiveDataCallback;
    wkeOnUrlRequestDidFailCallback didFailCallback;
    wkeOnUrlRequestDidFinishLoadingCallback didFinishLoadingCallback;
};
DUILIB_API int wkeNetStartUrlRequest(wkeWebView webView, wkeWebUrlRequestPtr request, void *param,
                                     const wkeUrlRequestCallbacks *callbacks);
DUILIB_API int wkeNetGetHttpStatusCode(wkeWebUrlResponsePtr response);

DUILIB_API __int64 wkeNetGetExpectedContentLength(wkeWebUrlResponsePtr response);
DUILIB_API CDuiString wkeNetGetResponseUrl(wkeWebUrlResponsePtr response);
DUILIB_API void wkeNetCancelWebUrlRequest(int requestId);

// 获取此请求中的post数据。只有当请求是post时才有效果
DUILIB_API wkePostBodyElements *wkeNetGetPostBody(wkeNetJob jobPtr);

// 这四个接口要结合起来使用。
// 当wkeOnLoadUrlBegin里判断是post时，可以通过wkeNetCreatePostBodyElements来创建一个新的post数据包。
// 然后wkeNetFreePostBodyElements来释放原post数据。
DUILIB_API wkePostBodyElements *wkeNetCreatePostBodyElements(wkeWebView webView, size_t length);
DUILIB_API void wkeNetFreePostBodyElements(wkePostBodyElements *elements);

DUILIB_API wkePostBodyElement *wkeNetCreatePostBodyElement(wkeWebView webView);
DUILIB_API void wkeNetFreePostBodyElement(wkePostBodyElement *element);

// 判断frameId是否是主frame
DUILIB_API bool wkeIsMainFrame(wkeWebView webView, wkeWebFrameHandle frameId);
DUILIB_API bool wkeIsWebRemoteFrame(wkeWebView webView, wkeWebFrameHandle frameId);
// 获取主frame的句柄
DUILIB_API wkeWebFrameHandle wkeWebFrameGetMainFrame(wkeWebView webView);

// 运行js在指定的frame上，通过frameId
// 参数：isInClosure表示是否在外层包个function() {}形式的闭包
// 注意：如果需要返回值，在isInClosure为true时，需要写return，为false则不用
DUILIB_API jsValue wkeRunJsByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const CDuiString &script,
                                   bool isInClosure);
DUILIB_API void wkeInsertCSSByFrame(wkeWebView webView, wkeWebFrameHandle frameId, const CDuiString &cssText);

typedef void *v8ContextPtr;
DUILIB_API void wkeWebFrameGetMainWorldScriptContext(wkeWebView webView, wkeWebFrameHandle webFrameId,
                                                     v8ContextPtr contextOut);

typedef void *v8Isolate;
DUILIB_API v8Isolate wkeGetBlinkMainThreadIsolate();


//wkewindow-----------------------------------------------------------------------------------
enum wkeWindowType
{
    WKE_WINDOW_TYPE_POPUP,
    WKE_WINDOW_TYPE_TRANSPARENT,
    WKE_WINDOW_TYPE_CONTROL
};
// 创建一个带真实窗口的wkeWebView
// 参数：wkeWindowType
// WKE_WINDOW_TYPE_POPUP：      普通窗口
// WKE_WINDOW_TYPE_TRANSPARENT：透明窗口。mb内部通过layer window实现
// WKE_WINDOW_TYPE_CONTROL：    嵌入在父窗口里的子窗口。此时parent需要被设置
DUILIB_API wkeWebView wkeCreateWebWindow(wkeWindowType type, HWND parent, int x, int y, int width,
                                         int height);

struct wkeWindowCreateInfo
{
    int size;
    HWND parent;
    DWORD style;
    DWORD styleEx;
    int x;
    int y;
    int width;
    int height;
    COLORREF color;
};
DUILIB_API wkeWebView wkeCreateWebCustomWindow(const wkeWindowCreateInfo *info);

// 销毁wkeWebView对应的所有数据结构，包括真实窗口等
DUILIB_API void wkeDestroyWebWindow(wkeWebView webView);
// 获取窗口对应的真实句柄。和wkeGetHostHWND的实现完全相同
DUILIB_API HWND wkeGetWindowHandle(wkeWebView webView);

DUILIB_API void wkeShowWindow(wkeWebView webView, bool showFlag);
DUILIB_API void wkeEnableWindow(wkeWebView webView, bool enableFlag);

DUILIB_API void wkeMoveWindow(wkeWebView webView, int x, int y, int width, int height);
// 窗口在父窗口或屏幕里居中
DUILIB_API void wkeMoveToCenter(wkeWebView webView);
// resize窗口，和wkeResize效果一样
DUILIB_API void wkeResizeWindow(wkeWebView webView, int width, int height);

DUILIB_API wkeWebDragOperation wkeDragTargetDragEnter(wkeWebView webView, const wkeWebDragData *webDragData,
                                                      const POINT *clientPoint, const POINT *screenPoint, wkeWebDragOperationsMask operationsAllowed,
                                                      int modifiers);
DUILIB_API wkeWebDragOperation wkeDragTargetDragOver(wkeWebView webView, const POINT *clientPoint,
                                                     const POINT *screenPoint, wkeWebDragOperationsMask operationsAllowed, int modifiers);
DUILIB_API void wkeDragTargetDragLeave(wkeWebView webView);
DUILIB_API void wkeDragTargetDrop(wkeWebView webView, const POINT *clientPoint, const POINT *screenPoint,
                                  int modifiers);
DUILIB_API void wkeDragTargetEnd(wkeWebView webView, const POINT *clientPoint, const POINT *screenPoint,
                                 wkeWebDragOperation operation);

typedef void(WKE_CALL *wkeUiThreadRunCallback)(HWND hWnd, void *param);
typedef int(WKE_CALL *wkeUiThreadPostTaskCallback)(HWND hWnd, wkeUiThreadRunCallback callback,
                                                   void *param);

DUILIB_API void wkeUtilSetUiCallback(wkeUiThreadPostTaskCallback callback);
DUILIB_API CDuiString wkeUtilSerializeToMHTML(wkeWebView webView);

struct wkePdfDatas
{
    int count;
    size_t *sizes;
    const void **datas;
};

struct wkePrintSettings
{
    int  structSize;
    int  dpi;
    int  width; // in px
    int  height;
    int  marginTop;
    int  marginBottom;
    int  marginLeft;
    int  marginRight;
    BOOL isPrintPageHeadAndFooter;
    BOOL isPrintBackgroud;
    BOOL isLandscape;
};

DUILIB_API const wkePdfDatas *wkeUtilPrintToPdf(wkeWebView webView, wkeWebFrameHandle frameId,
                                                const wkePrintSettings *settings);
DUILIB_API const wkeMemBuf *wkePrintToBitmap(wkeWebView webView, wkeWebFrameHandle frameId,
                                             const wkeScreenshotSettings *settings);
DUILIB_API void wkeUtilRelasePrintPdfDatas(const wkePdfDatas *datas);

DUILIB_API void wkeSetWindowTitle(wkeWebView webView, const CDuiString &title);

typedef void(WKE_CALL *wkeNodeOnCreateProcessCallback)(wkeWebView webView, void *param,
                                                       const WCHAR *applicationPath, const WCHAR *arguments, STARTUPINFOW *startup);
DUILIB_API void wkeNodeOnCreateProcess(wkeWebView webView, wkeNodeOnCreateProcessCallback callback,
                                       void *param);

typedef void(WKE_CALL *wkeOnPluginFindCallback)(wkeWebView webView, void *param, const utf8 *mime,
                                                void *initializeFunc, void *getEntryPointsFunc, void *shutdownFunc);
DUILIB_API void wkeOnPluginFind(wkeWebView webView, const CDuiString &mime, wkeOnPluginFindCallback callback,
                                void *param);
DUILIB_API void wkeAddNpapiPlugin(wkeWebView webView, void *initializeFunc, void *getEntryPointsFunc,
                                  void *shutdownFunc);

DUILIB_API void wkePluginListBuilderAddPlugin(void *builder, const CDuiString &name, const CDuiString &desc,
                                              const CDuiString &fileName);
DUILIB_API void wkePluginListBuilderAddMediaTypeToLastPlugin(void *builder, const CDuiString &name,
        const CDuiString &desc);
DUILIB_API void wkePluginListBuilderAddFileExtensionToLastMediaType(void *builder,
        const CDuiString &fileExtension);

DUILIB_API wkeWebView wkeGetWebViewByNData(void *ndata);

DUILIB_API bool wkeRegisterEmbedderCustomElement(wkeWebView webView, wkeWebFrameHandle frameId,
                                                 const CDuiString &name, void *options, void *outResult);

typedef wkeMediaPlayer(WKE_CALL *wkeMediaPlayerFactory)(wkeWebView webView, wkeMediaPlayerClient client,
                                                        void *npBrowserFuncs, void *npPluginFuncs);
typedef bool(WKE_CALL *wkeOnIsMediaPlayerSupportsMIMEType)(const utf8 *mime);
DUILIB_API void wkeSetMediaPlayerFactory(wkeWebView webView, wkeMediaPlayerFactory factory,
                                         wkeOnIsMediaPlayerSupportsMIMEType callback);

DUILIB_API CDuiString wkeGetContentAsMarkup(wkeWebView webView, wkeWebFrameHandle frame, size_t *size);

DUILIB_API CDuiString wkeUtilDecodeURLEscape(const CDuiString &url);
DUILIB_API CDuiString wkeUtilEncodeURLEscape(const CDuiString &url);
DUILIB_API CDuiString wkeUtilBase64Encode(const CDuiString &str);
DUILIB_API CDuiString wkeUtilBase64Decode(const CDuiString &str);
DUILIB_API const wkeMemBuf *wkeUtilCreateV8Snapshot(const CDuiString &str);

DUILIB_API void wkeRunMessageLoop();

//JavaScript Bind-----------------------------------------------------------------------------------
#define JS_CALL __fastcall

typedef jsValue(JS_CALL *jsNativeFunction)(jsExecState es);

// 绑定一个全局函数到主frame的window上。
// 参数：略
// 注意：此接口只能绑定主frame，并且特别需要注意的是，因为历史原因，此接口是fastcall调用约定！（但wkeJsBindFunction不是）
// 另外此接口和wkeJsBindFunction必须在webview创建前调用
// 使用示例：
// c++里：
//  --------
//  jsValue JS_CALL onNativeFunction(jsExecState es) {
//      jsValue v = jsArg(es, 0);
//      const wchar_t* str = jsToTemdivStringW(es, v);
//      OutdivutdebugStringW(str);
//  }
//  jsBindFunction("testCall", onNativeFunction， 1);
//
// js里：
//  --------
//  window.testCall('testStrt');
DUILIB_API void __fastcall jsBindFunction(const CDuiString &name, jsNativeFunction fn, unsigned int argCount);
// 对js winows绑定一个属性访问器，在js里windows.XXX这种形式调用时，fn会被调用
// 示例：jsBindGetter("XXX")
DUILIB_API void jsBindGetter(const CDuiString &name, jsNativeFunction fn);
// 对js winows绑定一个属性设置器。
DUILIB_API void jsBindSetter(const CDuiString &name, jsNativeFunction fn);

typedef jsValue(WKE_CALL *jsNativeFunctionEx)(jsExecState es, void *param);

// 和jsBindFunction功能类似，但更方便一点，可以传一个param做自定义数据。
// 此接口和wkeJsBindFunction必须在webview创建前调用
DUILIB_API void jsBindFunctionEx(const CDuiString &name, jsNativeFunctionEx fn, void *param,
                                 unsigned int argCount);
// 对js winows绑定一个属性访问器，在js里windows.XXX这种形式调用时，fn会被调用
// 示例：jsBindGetterEx("XXX")
DUILIB_API void jsBindGetterEx(const CDuiString &name, jsNativeFunctionEx fn, void *param);
// 对js winows绑定一个属性设置器。
DUILIB_API void jsBindSetterEx(const CDuiString &name, jsNativeFunctionEx fn, void *param);

// 获取es里存的参数个数。一般是在绑定的js调用c++回调里使用，判断js传递了多少参数给c++
DUILIB_API int jsArgCount(jsExecState es);

enum jsType
{
    JSTYPE_NUMBER,
    JSTYPE_STRING,
    JSTYPE_BOOLEAN,
    JSTYPE_OBJECT,
    JSTYPE_FUNCTION,
    JSTYPE_UNDEFINED,
    JSTYPE_ARRAY,
    JSTYPE_NULL,
};

// 判断第argIdx个参数的参数类型。argIdx从是个0开始计数的值。如果超出jsArgCount返回的值，将发生崩溃
DUILIB_API jsType jsArgType(jsExecState es, int argIdx);

// 获取第argIdx对应的参数的jsValue值。
DUILIB_API jsValue jsArgValue(jsExecState es, int argIdx);

// 获取v对应的类型。
DUILIB_API jsType jsTypeOf(jsValue v);

// 判断v是否为数字、字符串、bool
DUILIB_API bool jsIsNumber(jsValue v);
DUILIB_API bool jsIsString(jsValue v);
DUILIB_API bool jsIsBoolean(jsValue v);
// 当v不是数字、字符串、undefined、null、函数的时候，此接口返回true
DUILIB_API bool jsIsObject(jsValue v);
DUILIB_API bool jsIsFunction(jsExecState es, jsValue v);
DUILIB_API bool jsIsUndefined(jsExecState es, jsValue v);
DUILIB_API bool jsIsNull(jsExecState es, jsValue v);
DUILIB_API bool jsIsArray(jsExecState es, jsValue v);
// 如果v本身是个布尔值，返回对应的true或者false；如果是个对象（JSTYPE_OBJECT），返回false（这里注意）
DUILIB_API bool jsIsTrue(jsValue v);
// 等价于!jsIsTrue(v)
DUILIB_API bool jsIsFalse(jsValue v);


// 如果v是个整形或者浮点，返回相应值（如果是浮点，返回取整后的值）。如果是其他类型，返回0（这里注意）
DUILIB_API int jsToInt(jsExecState es, jsValue v);
// 如果v是个浮点形，返回相应值。如果是其他类型，返回0.0（这里注意）
DUILIB_API double jsToFloat(jsExecState es, jsValue v);
DUILIB_API double jsToDouble(jsExecState es, jsValue v);
DUILIB_API bool jsToBoolean(jsExecState es, jsValue v);

// 构建一个js的arraybuffer类型的jaValue。主要用来处理一些二进制数据，注意是创建
DUILIB_API jsValue jsArrayBuffer(jsExecState es, char *buffer, size_t size);
// 获取一个js的arraybuffer类型的数据。主要用来处理一些二进制数据
DUILIB_API wkeMemBuf *jsGetArrayBuffer(jsExecState es, jsValue v);

// 如果v是个字符串，返回相应值。如果是其他类型，返回L""（这里注意） 另外，返回的字符串不需要外部释放。mb会在下一帧自动释放
DUILIB_API CDuiString jsToTempString(jsExecState es, jsValue v);

// return v8::Persistent<v8::Value>*
DUILIB_API void *jsToV8Value(jsExecState es, jsValue v);

// 创建建一个int型的jsValue，注意是创建
DUILIB_API jsValue jsInt(int v);
DUILIB_API jsValue jsFloat(float v);
DUILIB_API jsValue jsDouble(jsExecState es, double v);
DUILIB_API jsValue jsBoolean(jsExecState es, bool v);

DUILIB_API jsValue jsUndefined();
DUILIB_API jsValue jsNull();
DUILIB_API jsValue jsTrue();
DUILIB_API jsValue jsFalse();

// 构建一个utf8编码的字符串的的jsValue。str会在内部拷贝保存，注意是创建
DUILIB_API jsValue jsString(jsExecState es, const CDuiString &str);
// 构建一个临时js object的jsValue，注意是创建
DUILIB_API jsValue jsEmptyObject(jsExecState es);
DUILIB_API jsValue jsEmptyArray(jsExecState es);

typedef jsValue(WKE_CALL *jsGetPropertyCallback)(jsExecState es, jsValue obj, const char *propertyName);
typedef bool(WKE_CALL *jsSetPropertyCallback)(jsExecState es, jsValue obj, const char *propertyName,
                                              jsValue value);
typedef jsValue(WKE_CALL *jsCallAsFunctionCallback)(jsExecState es, jsValue obj, jsValue *args,
                                                    int argCount);
struct tagjsData; // declare warning fix
typedef void(WKE_CALL *jsFinalizeCallback)(struct tagjsData *data);

typedef struct tagjsData
{
    char typeName[100];
    jsGetPropertyCallback propertyGet;
    jsSetPropertyCallback propertySet;
    jsFinalizeCallback finalize;
    jsCallAsFunctionCallback callAsFunction;
} jsData;

// 构建一个js Objcet，可以传递给js使用。
// 参数：
// DUILIB_API typedef jsValue(*jsGetPropertyCallback)(jsExecState es, jsValue obj, const utf8* propertyName);;
//   属性访问器。在js里XXX.yyy这样调用时，jsGetPropertyCallback会被触发，并且propertyName的值为yyy
// typedef bool(*jsSetPropertyCallback)(jsExecState es, jsValue obj, const utf8* propertyName, jsValue value);
//   属性设置器。在js里XXX.yyy=1这样调用时，jsSetPropertyCallback会被触发，并且propertyName的值为yyy，value的值是字符串类型。可以通过之前提到的字符串操作接口来获取
// typedef jsValue(*jsCallAsFunctionCallback)(jsExecState es, jsValue obj, jsValue* args, int argCount);
//   在js里XXX()这样调用时会触发。
// typedef void(*jsFinalizeCallback)(struct tagjsData* data);
//   在js里没人引用，且垃圾回收时会触发
DUILIB_API jsValue jsObject(jsExecState es, jsData *data);
// 创建一个主frame的全局函数。jsData的用法如上。js调用：XXX() 此时jsData的callAsFunction触发。 其实jsFunction和jsObject功能基本类似。且jsObject的功能更强大一些
DUILIB_API jsValue jsFunction(jsExecState es, jsData *data);
// 获取jsObject或jsFunction创建的jsValue对应的jsData指针。
DUILIB_API jsData *jsGetData(jsExecState es, jsValue value);

// 如果object是个js的object，则获取prop指定的属性。如果object不是js object类型，则返回jsUndefined
DUILIB_API jsValue jsGet(jsExecState es, jsValue obj, const  CDuiString &prop);
// 设置object的属性
DUILIB_API void jsSet(jsExecState es, jsValue obj, const CDuiString &prop, jsValue v);

// 设置js arrary的第index个成员的值，object必须是js array才有用，否则会返回jsUndefined
DUILIB_API jsValue jsGetAt(jsExecState es, jsValue obj, int index);
// 设置js arrary的第index个成员的值，object必须是js array才有用。
DUILIB_API void jsSetAt(jsExecState es, jsValue obj, int index, jsValue value);

struct jsKeys
{
    unsigned int length;
    const utf8 **keys;
};
// 获取object有哪些key
DUILIB_API jsKeys *jsGetKeys(jsExecState es, jsValue obj);
DUILIB_API void jsFreeKeys(jsKeys *obj);

DUILIB_API bool jsIsJsValueValid(jsExecState es, jsValue obj);
DUILIB_API bool jsIsValidExecState(jsExecState es);
DUILIB_API void jsDeleteObjectProp(jsExecState es, jsValue obj, const CDuiString &prop);

// 获取js arrary的长度，object必须是js array才有用。
DUILIB_API int jsGetLength(jsExecState es, jsValue obj);
// 设置js arrary的长度，object必须是js array才有用。
DUILIB_API void jsSetLength(jsExecState es, jsValue obj, int length);

DUILIB_API jsValue jsGlobalObject(jsExecState es);
// 获取es对应的webview
DUILIB_API wkeWebView jsGetWebView(jsExecState es);

// 执行一段js，并返回值。
// 注意：str的代码会在mb内部自动被包裹在一个function(){}中。所以使用的变量会被隔离 注意：要获取返回值，请写return。这和wke不太一样。wke不需要写retrun
DUILIB_API jsValue jsEval(jsExecState es, const CDuiString &str);
// 和上述接口的区别是，isInClosure表示是否要包裹一层function(){}。jsEvalW相当于jsEvalExW(es, str, false)
// 注意：如果需要返回值，在isInClosure为true时，需要写return，为false则不用
DUILIB_API jsValue jsEvalExW(jsExecState es, const CDuiString &str, bool isInClosure);

// 调用一个func对应的js函数。如果此js函数是成员函数，则需要填thisValue。 否则可以传jsUndefined。args是个数组，个数由argCount控制。 func可以是从js里取的，也可以是自行构造的。
DUILIB_API jsValue jsCall(jsExecState es, jsValue func, jsValue thisValue, jsValue *args, int argCount);
// 调用window上的全局函数
DUILIB_API jsValue jsCallGlobal(jsExecState es, jsValue func, jsValue *args, int argCount);

// 获取window上的属性
DUILIB_API jsValue jsGetGlobal(jsExecState es, const CDuiString &prop);
// 设置window上的属性
DUILIB_API void jsSetGlobal(jsExecState es, const CDuiString &prop, jsValue v);

// 强制垃圾回收
DUILIB_API void jsGC();

DUILIB_API bool jsAddRef(jsExecState es, jsValue val);
DUILIB_API bool jsReleaseRef(jsExecState es, jsValue val);

struct jsExceptionInfo
{
    const utf8 *message; // Returns the exception message.
    const utf8 *sourceLine; // Returns the line of source code that the exception occurred within.
    const utf8
    *scriptResourceName; // Returns the resource name for the script from where the function causing the error originates.
    int lineNumber; // Returns the 1-based number of the line where the error occurred or 0 if the line number is unknown.
    int startPosition; // Returns the index within the script of the first character where the error occurred.
    int endPosition; // Returns the index within the script of the last character where the error occurred.
    int startColumn; // Returns the index within the line of the first character where the error occurred.
    int endColumn; // Returns the index within the line of the last character where the error occurred.
    const utf8 *callstackString;
};

// 当wkeRunJs、jsCall等接口调用时，如果执行的js代码有异常，此接口将获取到异常信息。否则返回nullptr。
DUILIB_API jsExceptionInfo *jsGetLastErrorIfException(jsExecState es);

DUILIB_API jsValue jsThrowException(jsExecState es, const utf8 *exception);
DUILIB_API CDuiString jsGetCallstack(jsExecState es);

}

#endif // __WKE_DLL_H__
