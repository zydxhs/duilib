#pragma once
//////////////BEGIN消息映射宏定义////////////////////////////////////////////////////
///

namespace DuiLib {

// 窗体数据更新方向。用户通常不需要
enum EMParamUpdateDirection
{
    EPARAM_INIT,        // 初始化
    EPARAM_SAVE,        // 保存
};

// 消息ID定义说明：
// 0        - WM_USER-1 系统预留
// WM_USER  - 0x7FFF    私有窗口类使用的整形消息
// WM_APP   - 0xBFFF    可以被应用程序使用的消息
// 0xC000   - 0xFFFF    被应用程序使用的字符串消息
// 0xFFFF   -           系统预留
//自定义消息
enum EMCUSTOM_EVENT
{
    // duilib 内部使用的消息
    WM_ASYNC_NOTIFY = WM_APP,               // 异步通知消息   duilib内部使用
    WM_LBUTTON_CLICK,                       // 左键 单击 消息 duilib内部使用
    WM_LBUTTON_DBLCLK,                      // 左键 双击 消息 duilib内部使用
    WM_RBUTTON_CLICK,                       // 右键 单击 消息 duilib内部使用
    WM_RBUTTON_DBLCLK,                      // 右键 双击 消息 duilib内部使用
    WM_WNDDATA_UPDATE,                      // 窗体数据 初始化/保存 WPARAM=EMParamUpdateDirection

    WM_DUILIB_INNER_MAX = WM_APP + 200,     // DUILIB 内部消息的最大值

    // duilib 发出的消息，用户程序也可以接收这些消息
    WM_LANGUAGE_UPDATE,                     // 更新界面语言。用户通常只需要重载处理函数即可。
    WM_MENUITEM_CLICK,                      // 用来接收菜单单击的消息。还可以处理 Notify 通知。

    WM_DUILIB_PUB_MAX = 0xBFFF,             // 这里定义的消息ID，不能大于该值
};

//
#define TIMERID_USER_BEGIN      1024    // 用户定时器ID的最小值。小于该值的定时器为duilib内部保留

// DuiLib 使用的定时器ID
enum EMInnerTimerId
{
    // DuiLib 内部通用定时器ID
    TIMERID_TRIGGER_MAX = 49,       // 特效用定时器ID 最大值
    TIMERID_CARET = 50,             // 光标闪烁定时器ID

    // DuiLib 各控件使用
    TIMERID_LAYEREDUPDATE = 200,    // Manager  分层窗口更新定时器
    TIMERID_DBLCLICK,               // Manager  判断单击、双击的定时器
    TIMERID_DELAY_NTY,              // Edit     延时通知编辑框内容变化
    TIMERID_REPEAT,                 // ScrollBar滚动条定时器ID
    TIMERID_WEB_REFRESH,            // Browser  网页内容刷新定时器ID
    TIMERID_MOUSEENTER,             // Menu     鼠标进入菜单项定时器
    TIMERID_MOUSELEAVE,             // Menu     鼠标离开菜单项定时器
    TIMERID_DISABLE,                // Button   按钮响应频率控制定时器
    TIMERID_FADE,                   // Button   按钮图片透明度变化
};

// 用户不需要的常量
#define _USEIMM                 1
#define MAX_FONT_ID             30000
#define RES_TYPE_COLOR          _T("*COLOR*")
#define SCROLLBAR_LINESIZE      8

enum DuiSig
{
    DuiSig_end = 0, // [marks end of message map]
    DuiSig_lwl,     // LRESULT (WPARAM, LPARAM)
    DuiSig_vn,      // void (TNotifyUI)
};

class CControlUI;

// Structure for notifications to the outside world
typedef struct tagTNotifyUI
{
    CDuiString sType;
    CDuiString sVirtualWnd;
    CControlUI *pSender;
    DWORD dwTimestamp;
    POINT ptMouse;
    WPARAM wParam;          // 拖拽： COldDataHelper 指针
    LPARAM lParam;
} TNotifyUI;

class CNotifyPump;
typedef void (CNotifyPump::*DUI_PMSG)(TNotifyUI &msg);  //指针类型

union DuiMessageMapFunctions
{
    DUI_PMSG pfn;   // generic member function pointer
    LRESULT(CNotifyPump::*pfn_Notify_lwl)(WPARAM, LPARAM);
    void (CNotifyPump::*pfn_Notify_vn)(TNotifyUI &);
};

//定义所有消息类型
//////////////////////////////////////////////////////////////////////////

#define DUI_MSGTYPE_MENU                   (_T("menu"))
#define DUI_MSGTYPE_LINK                   (_T("link"))

#define DUI_MSGTYPE_TIMER                  (_T("timer"))
#define DUI_MSGTYPE_CLICK                  (_T("click"))
#define DUI_MSGTYPE_DBLCLICK               (_T("dblclick"))

#define DUI_MSGTYPE_RETURN                 (_T("return"))
#define DUI_MSGTYPE_SCROLL                 (_T("scroll"))

#define DUI_MSGTYPE_DROPDOWN               (_T("dropdown"))
#define DUI_MSGTYPE_SETFOCUS               (_T("setfocus"))

#define DUI_MSGTYPE_KILLFOCUS              (_T("killfocus"))
#define DUI_MSGTYPE_ITEMCLICK              (_T("itemclick"))
#define DUI_MSGTYPE_TABSELECT              (_T("tabselect"))

#define DUI_MSGTYPE_ITEMSELECT             (_T("itemselect"))
#define DUI_MSGTYPE_ITEMEXPAND             (_T("itemexpand"))

#define DUI_MSGTYPE_WINDOWINIT             (_T("windowinit"))
#define DUI_MSGTYPE_BUTTONDOWN             (_T("buttondown"))
#define DUI_MSGTYPE_MOUSEENTER             (_T("mouseenter"))
#define DUI_MSGTYPE_MOUSELEAVE             (_T("mouseleave"))

#define DUI_MSGTYPE_TEXTCHANGED            (_T("textchanged"))
#define DUI_MSGTYPE_HEADERCLICK            (_T("headerclick"))
#define DUI_MSGTYPE_ITEMDBCLICK            (_T("itemdbclick"))
#define DUI_MSGTYPE_SHOWACTIVEX            (_T("showactivex"))

#define DUI_MSGTYPE_ITEMCOLLAPSE           (_T("itemcollapse"))
#define DUI_MSGTYPE_ITEMACTIVATE           (_T("itemactivate"))
#define DUI_MSGTYPE_VALUECHANGED           (_T("valuechanged"))

#define DUI_MSGTYPE_SELECTCHANGED          (_T("selectchanged"))

#define DUI_MSGTYPE_MENUITEM_CLICK         (_T("menuitemclick"))

#define DUI_MSGTYPE_DRAGDATA               (_T("dragdata"))         // 拖拽时通知用户填充数据
#define DUI_MSGTYPE_DRAGENTER              (_T("dragenter"))        // 拖拽时进入控件
#define DUI_MSGTYPE_DRAGOVER               (_T("dragover"))         // 拖拽时在控件上移动
#define DUI_MSGTYPE_DRAGLEAVE              (_T("dragleave"))        // 拖拽时离开控件
#define DUI_MSGTYPE_DRAGDROP               (_T("dragdrop"))         // 拖拽时在控件上释放

#define DUI_MSGTYPE_WEBNOTIFY              (_T("webnotify"))        // WEB 控件通知消息
//////////////////////////////////////////////////////////////////////////



struct DUI_MSGMAP_ENTRY;
struct DUI_MSGMAP
{
#ifndef UILIB_STATIC
    const DUI_MSGMAP *(PASCAL *pfnGetBaseMap)();
#else
    const DUI_MSGMAP *pBaseMap;
#endif
    const DUI_MSGMAP_ENTRY *lpEntries;
};

//结构定义
struct DUI_MSGMAP_ENTRY //定义一个结构体，来存放消息信息
{
    CDuiString sMsgType;          // DUI消息类型
    CDuiString sCtrlName;         // 控件名称
    UINT       nSig;              // 标记函数指针类型
    DUI_PMSG   pfn;               // 指向函数的指针
};

//定义
#ifndef UILIB_STATIC
#define DUI_DECLARE_MESSAGE_MAP()                                         \
    private:                                                              \
    static const DUI_MSGMAP_ENTRY _messageEntries[];                      \
    protected:                                                            \
    static const DUI_MSGMAP messageMap;                                   \
    static const DUI_MSGMAP* PASCAL _GetBaseMessageMap();                 \
    virtual const DUI_MSGMAP* GetMessageMap() const;

#else
#define DUI_DECLARE_MESSAGE_MAP()                                         \
    private:                                                              \
    static const DUI_MSGMAP_ENTRY _messageEntries[];                      \
    protected:                                                            \
    static  const DUI_MSGMAP messageMap;                                  \
    virtual const DUI_MSGMAP* GetMessageMap() const;

#endif


//基类声明开始
#ifndef UILIB_STATIC
#define DUI_BASE_BEGIN_MESSAGE_MAP(theClass)                                   \
    const DUI_MSGMAP* PASCAL theClass::_GetBaseMessageMap()                    \
    { return NULL; }                                                           \
    const DUI_MSGMAP* theClass::GetMessageMap() const                          \
    { return &theClass::messageMap; }                                          \
    UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                       \
            {  &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] }; \
    UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] =          \
            {

#else
#define DUI_BASE_BEGIN_MESSAGE_MAP(theClass)                              \
    const DUI_MSGMAP* theClass::GetMessageMap() const                     \
    { return &theClass::messageMap; }                                     \
    UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
            {  NULL, &theClass::_messageEntries[0] };                     \
    UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] = {

#endif


//子类声明开始
#ifndef UILIB_STATIC
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                              \
    const DUI_MSGMAP* PASCAL theClass::_GetBaseMessageMap()                     \
    { return &baseClass::messageMap; }                                          \
    const DUI_MSGMAP* theClass::GetMessageMap() const                           \
    { return &theClass::messageMap; }                                           \
    UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                        \
            { &theClass::_GetBaseMessageMap, &theClass::_messageEntries[0] };   \
    UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] = {

#else
#define DUI_BEGIN_MESSAGE_MAP(theClass, baseClass)                        \
    const DUI_MSGMAP* theClass::GetMessageMap() const                     \
    { return &theClass::messageMap; }                                     \
    UILIB_COMDAT const DUI_MSGMAP theClass::messageMap =                  \
            { &baseClass::messageMap, &theClass::_messageEntries[0] };    \
    UILIB_COMDAT const DUI_MSGMAP_ENTRY theClass::_messageEntries[] = {

#endif


//声明结束
#define DUI_END_MESSAGE_MAP()                                             \
    { _T(""), _T(""), DuiSig_end, (DUI_PMSG)0 }                           \
    };


//定义消息类型--执行函数宏
#define DUI_ON_MSGTYPE(msgtype, memberFxn)                                \
    { msgtype, _T(""), DuiSig_vn, (DUI_PMSG)&memberFxn},


//定义消息类型--控件名称--执行函数宏
#define DUI_ON_MSGTYPE_CTRNAME(msgtype,ctrname,memberFxn)                 \
    { msgtype, ctrname, DuiSig_vn, (DUI_PMSG)&memberFxn },


//定义click消息的控件名称--执行函数宏
#define DUI_ON_CLICK_CTRNAME(ctrname,memberFxn)                           \
    { DUI_MSGTYPE_CLICK, ctrname, DuiSig_vn, (DUI_PMSG)&memberFxn },


//定义selectchanged消息的控件名称--执行函数宏
#define DUI_ON_SELECTCHANGED_CTRNAME(ctrname,memberFxn)                   \
    { DUI_MSGTYPE_SELECTCHANGED,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn },


//定义killfocus消息的控件名称--执行函数宏
#define DUI_ON_KILLFOCUS_CTRNAME(ctrname,memberFxn)                       \
    { DUI_MSGTYPE_KILLFOCUS,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn },


//定义menu消息的控件名称--执行函数宏
#define DUI_ON_MENU_CTRNAME(ctrname,memberFxn)                            \
    { DUI_MSGTYPE_MENU,ctrname,DuiSig_vn,(DUI_PMSG)&memberFxn },


//定义与控件名称无关的消息宏

//定义timer消息--执行函数宏
#define DUI_ON_TIMER()                                                    \
    { DUI_MSGTYPE_TIMER, _T(""), DuiSig_vn,(DUI_PMSG)&OnTimer },

// Mark method as deprecated.
// example: DUI_DEPRECATED void func();
#if defined(_MSC_VER)
    #define DUI_DEPRECATED __declspec(deprecated)
#elif defined(__GNUC__)
    #define DUI_DEPRECATED __attribute__((deprecated))
#else
    #pragma message("WARNING: You need to implement DUI_DEPRECATED for this compiler")
    #define DUI_DEPRECATED
#endif


///
//////////////END消息映射宏定义////////////////////////////////////////////////////


//////////////BEGIN控件名称宏定义//////////////////////////////////////////////////
///

#define DUI_CTR_EDIT                             (_T("Edit"))
#define DUI_CTR_LIST                             (_T("List"))
#define DUI_CTR_TEXT                             (_T("Text"))
#define DUI_CTR_TREE                             (_T("Tree"))
#define DUI_CTR_HBOX                             (_T("HBox"))   // HorizontalLayout
#define DUI_CTR_VBOX                             (_T("VBox"))   // VerticalLayout

#define DUI_CTR_ILIST                            (_T("IList"))
#define DUI_CTR_COMBO                            (_T("Combo"))
#define DUI_CTR_LABEL                            (_T("Label"))
#define DUI_CTR_FLASH                            (_T("Flash"))

#define DUI_CTR_BUTTON                           (_T("Button"))
#define DUI_CTR_OPTION                           (_T("Option"))
#define DUI_CTR_SLIDER                           (_T("Slider"))

#define DUI_CTR_CONTROL                          (_T("Control"))
#define DUI_CTR_ACTIVEX                          (_T("ActiveX"))
#define DUI_CTR_GIFANIM                          (_T("GifAnim"))

#define DUI_CTR_PROGRESS                         (_T("Progress"))
#define DUI_CTR_RICHEDIT                         (_T("RichEdit"))
#define DUI_CTR_CHECKBOX                         (_T("CheckBox"))
#define DUI_CTR_RADIOBOX                         (_T("RadioBox"))
#define DUI_CTR_COMBOBOX                         (_T("ComboBox"))   // Combo
#define DUI_CTR_DATETIME                         (_T("DateTime"))
#define DUI_CTR_TREEVIEW                         (_T("TreeView"))
#define DUI_CTR_TREENODE                         (_T("TreeNode"))

#define DUI_CTR_ILISTITEM                        (_T("IListItem"))
#define DUI_CTR_CONTAINER                        (_T("Container"))
#define DUI_CTR_TABLAYOUT                        (_T("TabLayout"))
#define DUI_CTR_SCROLLBAR                        (_T("ScrollBar"))

#define DUI_CTR_ICONTAINER                       (_T("IContainer"))
#define DUI_CTR_ILISTOWNER                       (_T("IListOwner"))
#define DUI_CTR_LISTHEADER                       (_T("ListHeader"))
#define DUI_CTR_TILELAYOUT                       (_T("TileLayout"))
// #define DUI_CTR_WEBBROWSER                       (_T("WebBrowser")) // 已废弃

#define DUI_CTR_CHILDLAYOUT                      (_T("ChildLayout"))
#define DUI_CTR_LISTELEMENT                      (_T("ListElement"))
#define DUI_CTR_VIRTUALLIST                      (_T("VirtualList"))

#define DUI_CTR_VERTICALLAYOUT                   (_T("VerticalLayout"))
#define DUI_CTR_LISTHEADERITEM                   (_T("ListHeaderItem"))

#define DUI_CTR_LISTHBOXELEMENT                  (_T("ListHBoxElement"))
#define DUI_CTR_LISTTEXTELEMENT                  (_T("ListTextElement"))

#define DUI_CTR_HORIZONTALLAYOUT                 (_T("HorizontalLayout"))
#define DUI_CTR_LISTLABELELEMENT                 (_T("ListLabelElement"))

#define DUI_CTR_LISTCONTAINERELEMENT             (_T("ListContainerElement"))

//以下是自定义控件
#define DUI_CTR_MENU                             (_T("Menu"))
#define DUI_CTR_MENUELEMENT                      (_T("MenuElement"))
#define DUI_CTR_HOTKEY                           (_T("HotKey"))
#define DUI_CTR_PWDCHECK                         (_T("PwdCheck"))
#define DUI_CTR_IPADDRESS                        (_T("IpAddress"))
#define DUI_CTR_HWEIGHTLAYOUT                    (_T("HWeightLayout"))
#define DUI_CTR_BROWSER                          (_T("Browser"))

///
//////////////END控件名称宏定义//////////////////////////////////////////////////


}// namespace DuiLib

