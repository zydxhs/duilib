#ifndef __UIRENDER_H__
#define __UIRENDER_H__

#pragma once

namespace DuiLib {
class CGifInfo;
/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CRenderClip
{
public:
    CRenderClip();
    ~CRenderClip();
    RECT rcItem;
    HDC hDC;
    HRGN hRgn;
    HRGN hOldRgn;

    // 创建一个剪切区域，将绘图限制在rc区域内
    static void GenerateClip(HDC hDC, const RECT &rc, CRenderClip &clip);

    // 创建一个圆角剪切区域，将绘图限制在rc区域内
    // width、height 横/纵向圆角弧度
    static void GenerateRoundClip(HDC hDC, const RECT &rc, const RECT &rcItem, int width, int height,
                                  CRenderClip &clip);

    // 使用活跃区域作为绘图区
    static void UseOldClipBegin(HDC hDC, CRenderClip &clip);

    // 恢复控件完整活跃区
    static void UseOldClipEnd(HDC hDC, CRenderClip &clip);
};

/////////////////////////////////////////////////////////////////////////////////////
//
typedef BOOL(WINAPI *PFunAlphaBlend)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION);
PFunAlphaBlend GetAlphaBlend(void);

class DUILIB_API CRenderEngine
{
public:
    static void ParseDrawInfo(TDrawInfo &drawInfo, CDuiString &sResType, DWORD &dwMask, bool &bHSL);
    static DWORD AdjustColor(DWORD dwColor, short H, short S, short L);
    static HBITMAP CreateARGB32Bitmap(HDC hDC, int cx, int cy, COLORREF **pBits);
    static void AdjustImage(bool bUseHSL, TImageInfo *imageInfo, short H, short S, short L);
    static TImageInfo *LoadImage(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    static CGifInfo *LoadGif(TDrawInfo &drawInfo);
    static CGifInfo *LoadGif(STRINGorID bitmap, LPCTSTR type = NULL, DWORD mask = 0);
    static void FreeImage(TImageInfo *bitmap, bool bDelete = true);
    static void DrawImage(HDC hDC, HBITMAP hBitmap, const RECT &rc, const RECT &rcPaint, const RECT &rcBmpPart,
                          const RECT &rcScale9, bool alphaChannel, BYTE uFade = 255, bool hole = false,
                          bool xtiled = false, bool ytiled = false);
    static bool DrawImage(HDC hDC, CPaintManagerUI *pManager, const RECT &rcItem, const RECT &rcPaint,
                          TDrawInfo &drawInfo);
    static void DrawColor(HDC hDC, const RECT &rc, DWORD color);
    static void DrawGradient(HDC hDC, const RECT &rc, DWORD dwFirst, DWORD dwSecond, bool bVertical, int nSteps);

    // 以下函数中的颜色参数alpha值无效
    // 2018-07-14 以下 5 个函数 alpha 值无效问题已修复
    static void DrawLine(HDC hDC, const RECT &rc, int nSize, DWORD dwPenColor, int nStyle = PS_SOLID);
    static void DrawRect(HDC hDC, const RECT &rc, int nSize, DWORD dwPenColor, int nStyle = PS_SOLID);
    static void DrawRoundRect(HDC hDC, const RECT &rc, int width, int height, int nSize, DWORD dwPenColor,
                              int nStyle = PS_SOLID);
    static void DrawText(HDC hDC, CPaintManagerUI *pManager, RECT &rc, LPCTSTR pstrText, DWORD dwTextColor,
                         int iFont, UINT uStyle);
    static void DrawHtmlText(HDC hDC, CPaintManagerUI *pManager, RECT &rc, LPCTSTR pstrText,
                             DWORD dwTextColor, RECT *pLinks, CDuiString *sLinks, int *pnLinkRects,
                             int iDefaultFont, UINT uStyle);

    static HBITMAP GenerateBitmap(CPaintManagerUI *pManager, RECT rc, CControlUI *pStopControl = NULL,
                                  DWORD dwFilterColor = 0);
    static HBITMAP GenerateBitmap(CPaintManagerUI *pManager, CControlUI *pControl, const RECT &rc,
                                  DWORD dwFilterColor = 0);
    static SIZE GetTextSize(HDC hDC, CPaintManagerUI *pManager, LPCTSTR pstrText, int iFont, UINT uStyle);
};

} // namespace DuiLib

#endif // __UIRENDER_H__
