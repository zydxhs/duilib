/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file dataobjecthelper.h
/// \brief Ole数据对象辅助类
///
/// RegisterClipboardFormat 可以注册自己的数据格式
/// \author zhuyadong@kedacom.com
/// \date 2018-04-28
/// \note
/// -----------------------------------------------------------------------------
/// 修改记录：
/// 日  期        版本        修改人        走读人    修改内容
///
#pragma once

namespace DuiLib {

// 支持的数据类型
// 1. 标准剪贴板数据类型
#define ECF_TEXT             1          // ANSI 文本
#define ECF_BITMAP           2          // 位图
#define ECF_METAFILEPICT     3
#define ECF_SYLK             4
#define ECF_DIF              5
#define ECF_TIFF             6
#define ECF_OEMTEXT          7
#define ECF_DIB              8
#define ECF_PALETTE          9
#define ECF_PENDATA          10
#define ECF_RIFF             11
#define ECF_WAVE             12
#define ECF_UNICODETEXT      13         // UNICODE 文本
#define ECF_ENHMETAFILE      14

#if(WINVER >= 0x0400)
    #define ECF_HDROP            15     // 资源管理器文件拖拽（通过剪贴板）
    #define ECF_LOCALE           16
#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0500)
    #define ECF_DIBV5            17
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0500)
    #define ECF_MAX              18
    #elif(WINVER >= 0x0400)
    #define ECF_MAX              17
#else
    #define ECF_MAX              15
#endif

#define ECF_OWNERDISPLAY     0x0080
#define ECF_DSPTEXT          0x0081
#define ECF_DSPBITMAP        0x0082
#define ECF_DSPMETAFILEPICT  0x0083
#define ECF_DSPENHMETAFILE   0x008E

// 2. "Private" formats don't get GlobalFree()'d
#define ECF_PRIVATEFIRST     0x0200
#define ECF_STRUCTDATA       0x0201
#define ECF_INT32            0x0202     // 32位整数
#define ECF_INT64            0x0203     // 64位整数
#define ECF_PRIVATELAST      0x02FF

// 3. "GDIOBJ" formats do get DeleteObject()'d
#define ECF_GDIOBJFIRST      0x0300
#define ECF_GDIOBJLAST       0x03FF

enum TDVASPECT
{
    EDVASPECT_CONTENT = 1,
    EDVASPECT_THUMBNAIL = 2,
    EDVASPECT_ICON = 4,
    EDVASPECT_DOCPRINT = 8
};


// 存储媒体类型，与 TYMED 的定义一致
enum EMTypeMedium
{
    ETYMED_NULL = 0,
    ETYMED_HGLOBAL = 1,
    ETYMED_FILE = 2,
    ETYMED_ISTREAM = 4,
    ETYMED_ISTORAGE = 8,
    ETYMED_GDI = 16,
    ETYMED_MFPICT = 32,
    ETYMED_ENHMF = 64,
};

struct DUILIB_API TDragData
{
    WORD    m_wClipFormat;  // 支持的数据类型
    WORD    m_wAspect;      // 详见 TDVASPECT(DVASPECT)
    int     m_eTypeMedium;  // 存储媒体类型，详见 EMTypeMedium(TYMED)
    void   *m_pData;        //
};

// 字符串存储到 HGLOBAL
HGLOBAL DUILIB_API StringToHandle(LPCWSTR lpcstrText, int nTextLen = -1);
HBITMAP DUILIB_API CopyHBitmap(HDC hDC, HBITMAP hBmpSrc);

class DUILIB_API COleDataHelper
{
public:
    COleDataHelper(void);
    ~COleDataHelper(void);

    // 添加数据。
    bool SetText(CDuiString sText);                         // 设置 文本
    bool SetBitmap(HBITMAP hBmp);                           // 设置 HBITMAP 位图
    bool SetCustomData(void *pData, DWORD dwLen, WORD wCF); // 设置 自定义数据
    bool SetCustomGDI(HGDIOBJ hGDI, WORD wCF);              // 设置 HBITMAP 以外的 GDI 对象
    bool SetInt(int nVal);                                  // 设置 32 位整数
    bool SetInt64(long long nnVal);                         // 设置 64 位整数

    // 读取数据
    CDuiString GetText(void);                       // 返回 文本
    HBITMAP GetBitmap(void);                        // 返回 HBITMAP 位图
    CDuiPtrArray GetFileList(void);                 // 返回 文件列表(TCHAR*)，需要 free 释放内存。接收资源管理器拖拽的文件列表
    void *GetCustomData(WORD wCF, DWORD &dwLen);    // 返回 自定义数据。需要 free 释放内存
    HGDIOBJ GetCustomGDI(WORD wCF);                 // 返回 HBITMAP 以外的 GDI 对象
    int GetInt(void);                               // 返回 32 位整数
    long long GetInt64(void);                       // 返回 64 位整数

    // TODO 删除数据

    // bRelease : TRUE 调用完成后，用户不能释放资源；FALSE 表示函数内部会复制一份资源，pData由用户释放
    bool SetData(void *pData, int nTM, WORD wCF, WORD wAspect = EDVASPECT_CONTENT, BOOL bRelease = TRUE);
    void *GetData(WORD wCF, int nTM, WORD wAspect = EDVASPECT_CONTENT);

    // IDataObject 数据枚举
    bool GetNext(WORD &wCF, WORD &wAspect, int &nTM);   // 从0开始枚举数据类型
    bool Reset(void);                                   // 重置索引为0，重新开始枚举

    bool HasText(void);                                 // 返回 文本是否可用
    bool HasBitmap(void);                               // 返回 位图是否可用
    bool HasFileList(void);                             // 返回 文件列表是否可用
    bool HasCustomData(WORD wCF = 0);                   // 返回 自定义数据是否可用
    bool HasCustomGDI(void);                            // 返回 HBITMAP 以外的 GDI 对象是否可用
    bool HasInt(void);                                  // 返回 32位整数是否可用
    bool HasInt64(void);                                // 返回 64位整数是否可用

    // 拖拽时返回当前的状态：DROPEFFECT_NONE | DROPEFFECT_COPY | DROPEFFECT_MOVE | DROPEFFECT_LINK | DROPEFFECT_SCROLL
    DWORD GetEffect(void) { return m_dwEffect; }
    void SetEffect(DWORD dwEffect) { m_dwEffect = dwEffect; }
    // 拖拽时按键状态：MK_CONTROL 等
    DWORD GetKeyState(void) { return m_dwKeyState; }

protected:
    friend class CPaintManagerUI;
    friend class CControlUI;

    IDataObject *CreateDataObject(void);
    bool CreateEnumFormatEtc(void);                     // 创建数据类型枚举接口
private:
    IDataObject    *m_pDataObj;
    bool            m_bAutoRelease;

    IEnumFORMATETC *m_pEnumFmt;

    DWORD           m_dwKeyState;
    DWORD           m_dwEffect;
};

}
