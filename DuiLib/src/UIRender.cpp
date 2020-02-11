#include "StdAfx.h"
#include "gifinfo.h"
#include "XUnzip.h"
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

///////////////////////////////////////////////////////////////////////////////////////

namespace DuiLib {

static int g_iFontID = MAX_FONT_ID;


static const float OneThird = 1.0f / 3;

static void RGBtoHSL(DWORD ARGB, float *H, float *S, float *L)
{
    const float
    R = (float)GetRValue(ARGB),
    G = (float)GetGValue(ARGB),
    B = (float)GetBValue(ARGB),
    nR = (R < 0 ? 0 : (R > 255 ? 255 : R)) / 255,
    nG = (G < 0 ? 0 : (G > 255 ? 255 : G)) / 255,
    nB = (B < 0 ? 0 : (B > 255 ? 255 : B)) / 255,
    m = min(min(nR, nG), nB),
    M = max(max(nR, nG), nB);
    *L = (m + M) / 2;

    if (fabs(M - m) < 0.0001f) { *H = *S = 0; }
    else
    {
        const float
        f = (fabs(nR - m) < 0.0001f) ? (nG - nB) : ((fabs(nG - m) < 0.0001f) ? (nB - nR) : (nR - nG)),
        i = (fabs(nR - m) < 0.0001f) ? 3.0f : ((fabs(nG - m) < 0.0001f) ? 5.0f : 1.0f);
        *H = (i - f / (M - m));

        if (*H >= 6) { *H -= 6; }

        *H *= 60;
        *S = (2 * (*L) <= 1) ? ((M - m) / (M + m)) : ((M - m) / (2 - M - m));
    }
}

DUI_INLINE static void HSLtoRGB(DWORD *ARGB, float H, float S, float L)
{
    const float q = 2 * L < 1 ? L * (1 + S) : (L + S - L * S);
    const float p = 2 * L - q;
    const float h = H / 360;
    const float tr = h + OneThird;
    const float tg = h;
    const float tb = h - OneThird;
    const float ntr = tr < 0 ? tr + 1 : (tr > 1 ? tr - 1 : tr);
    const float ntg = tg < 0 ? tg + 1 : (tg > 1 ? tg - 1 : tg);
    const float ntb = tb < 0 ? tb + 1 : (tb > 1 ? tb - 1 : tb);
    const float B = 255 * (6 * ntr < 1 ? p + (q - p) * 6 * ntr : (2 * ntr < 1 ? q : (3 * ntr < 2 ? p +
                           (q - p) * 6 * (2.0f * OneThird - ntr) : p)));
    const float G = 255 * (6 * ntg < 1 ? p + (q - p) * 6 * ntg : (2 * ntg < 1 ? q : (3 * ntg < 2 ? p +
                           (q - p) * 6 * (2.0f * OneThird - ntg) : p)));
    const float R = 255 * (6 * ntb < 1 ? p + (q - p) * 6 * ntb : (2 * ntb < 1 ? q : (3 * ntb < 2 ? p +
                           (q - p) * 6 * (2.0f * OneThird - ntb) : p)));
    *ARGB &= 0xFF000000;
    *ARGB |= RGB((BYTE)(R < 0 ? 0 : (R > 255 ? 255 : R)), (BYTE)(G < 0 ? 0 : (G > 255 ? 255 : G)),
                 (BYTE)(B < 0 ? 0 : (B > 255 ? 255 : B)));
}

DUI_INLINE static COLORREF PixelAlpha(COLORREF clrSrc, double src_darken, COLORREF clrDest,
                                      double dest_darken)
{
    return RGB(GetRValue(clrSrc) * src_darken + GetRValue(clrDest) * dest_darken,
               GetGValue(clrSrc) * src_darken + GetGValue(clrDest) * dest_darken,
               GetBValue(clrSrc) * src_darken + GetBValue(clrDest) * dest_darken);
}

static BOOL WINAPI AlphaBitBlt(HDC hDC, int nDestX, int nDestY, int dwWidth, int dwHeight, HDC hSrcDC,
                               int nSrcX, int nSrcY, int wSrc, int hSrc, BLENDFUNCTION ftn)
{
    HDC hTempDC = ::CreateCompatibleDC(hDC);

    if (NULL == hTempDC) { return FALSE; }

    //Creates Source DIB
    LPBITMAPINFO lpbiSrc = NULL;
    // Fill in the BITMAPINFOHEADER
    lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];

    if (lpbiSrc == NULL) { ::DeleteDC(hTempDC); return FALSE; }

    lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiSrc->bmiHeader.biWidth = dwWidth;
    lpbiSrc->bmiHeader.biHeight = dwHeight;
    lpbiSrc->bmiHeader.biPlanes = 1;
    lpbiSrc->bmiHeader.biBitCount = 32;
    lpbiSrc->bmiHeader.biCompression = BI_RGB;
    lpbiSrc->bmiHeader.biSizeImage = dwWidth * dwHeight;
    lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
    lpbiSrc->bmiHeader.biClrUsed = 0;
    lpbiSrc->bmiHeader.biClrImportant = 0;

    COLORREF *pSrcBits = NULL;
    HBITMAP hSrcDib = CreateDIBSection(hSrcDC, lpbiSrc, DIB_RGB_COLORS, (void **)&pSrcBits, NULL, NULL);

    if ((NULL == hSrcDib) || (NULL == pSrcBits))
    {
        delete[ ] lpbiSrc;
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    HBITMAP hOldTempBmp = (HBITMAP)::SelectObject(hTempDC, hSrcDib);
    ::StretchBlt(hTempDC, 0, 0, dwWidth, dwHeight, hSrcDC, nSrcX, nSrcY, wSrc, hSrc, SRCCOPY);
    ::SelectObject(hTempDC, hOldTempBmp);

    //Creates Destination DIB
    LPBITMAPINFO lpbiDest = NULL;
    // Fill in the BITMAPINFOHEADER
    lpbiDest = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];

    if (lpbiDest == NULL)
    {
        delete[ ] lpbiSrc;
        ::DeleteObject(hSrcDib);
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    lpbiDest->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    lpbiDest->bmiHeader.biWidth = dwWidth;
    lpbiDest->bmiHeader.biHeight = dwHeight;
    lpbiDest->bmiHeader.biPlanes = 1;
    lpbiDest->bmiHeader.biBitCount = 32;
    lpbiDest->bmiHeader.biCompression = BI_RGB;
    lpbiDest->bmiHeader.biSizeImage = dwWidth * dwHeight;
    lpbiDest->bmiHeader.biXPelsPerMeter = 0;
    lpbiDest->bmiHeader.biYPelsPerMeter = 0;
    lpbiDest->bmiHeader.biClrUsed = 0;
    lpbiDest->bmiHeader.biClrImportant = 0;

    COLORREF *pDestBits = NULL;
    HBITMAP hDestDib = CreateDIBSection(hDC, lpbiDest, DIB_RGB_COLORS, (void **)&pDestBits, NULL, NULL);

    if ((NULL == hDestDib) || (NULL == pDestBits))
    {
        delete[ ] lpbiSrc;
        ::DeleteObject(hSrcDib);
        ::DeleteDC(hTempDC);
        return FALSE;
    }

    ::SelectObject(hTempDC, hDestDib);
    ::BitBlt(hTempDC, 0, 0, dwWidth, dwHeight, hDC, nDestX, nDestY, SRCCOPY);
    ::SelectObject(hTempDC, hOldTempBmp);

    double src_darken;
    BYTE nAlpha;

    for (int pixel = 0; pixel < dwWidth * dwHeight; pixel++, pSrcBits++, pDestBits++)
    {
        nAlpha = LOBYTE(*pSrcBits >> 24);
        src_darken = (double)(nAlpha * ftn.SourceConstantAlpha) / 255.0 / 255.0;

        if (src_darken < 0.0) { src_darken = 0.0; }

        *pDestBits = PixelAlpha(*pSrcBits, src_darken, *pDestBits, 1.0 - src_darken);
    } //for

    ::SelectObject(hTempDC, hDestDib);
    ::BitBlt(hDC, nDestX, nDestY, dwWidth, dwHeight, hTempDC, 0, 0, SRCCOPY);
    ::SelectObject(hTempDC, hOldTempBmp);

    delete[ ] lpbiDest;
    ::DeleteObject(hDestDib);

    delete[ ] lpbiSrc;
    ::DeleteObject(hSrcDib);

    ::DeleteDC(hTempDC);
    return TRUE;
}

PFunAlphaBlend GetAlphaBlend(void)
{
    static PFunAlphaBlend lpAB = (PFunAlphaBlend)::GetProcAddress(
                                     ::GetModuleHandle(_T("msimg32.dll")), "AlphaBlend");

    if (NULL == lpAB) { lpAB = AlphaBitBlt; }

    return lpAB;
}

//////////////////////////////////////////////////////////////////////////
// UI开发离不开GDI，然而要用传统的GDI函数来处理alpha通道通常是一个恶梦：
// 虽然有AlphaBlend这个API可以做alpha混合，但是前提必须是操作的DC中的位图有alpha通道的数据，
// 问题的关键在于GDI函数在操作的地方会把原来的alpha通道清空。

// 使用GDI做alpha混合还要增加透明度关键要解决2个问题：
// 1、需要把内容画到一个临时位图上，同时保护好alpha通道。
// 2、在于把临时位图的数据和原位图做混合，而且不能改变镂空部分原位图的alpha通道的值。
// 辅助类，用于解决 GDI 不支持 alpha 的问题

// 2019-07-12 该类开销比较大，因此不再使用。启用 layered 时使用 GdiPlus，反之使用 GDI。
// class CHDCHelper
// {
// public:
//     CHDCHelper(HDC hdc, const LPCRECT pRect, BYTE byAlpha, BOOL bCopyBits = TRUE)
//         : m_hdc(hdc)
//         , m_byAlpha(byAlpha)
//         , m_pRc(pRect)
//         , m_bCopyBits(bCopyBits)
//     {
//         m_nWidth = pRect->right - pRect->left;
//         m_nHeight = pRect->bottom - pRect->top;
//         m_hBmp = CRenderEngine::CreateARGB32Bitmap(m_hdc, m_nWidth, m_nHeight, (COLORREF **)&m_pBits);
//         m_hMemDC = ::CreateCompatibleDC(hdc);
//         ::SetBkMode(m_hMemDC, TRANSPARENT);
//         ::SelectObject(m_hMemDC, m_hBmp);
//         ::SetViewportOrgEx(m_hMemDC, -pRect->left, -pRect->top, NULL);
//         //从原DC中获得画笔，画刷，字体，颜色等
//         m_hCurPen = ::SelectObject(hdc, GetStockObject(BLACK_PEN));
//         m_hCurBrush = ::SelectObject(hdc, GetStockObject(BLACK_BRUSH));
//         m_hCurFont = ::SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));
//         COLORREF clrCur = ::GetTextColor(hdc);
//
//         //将画笔，画刷，字体设置到memdc里
//         ::SelectObject(m_hMemDC, m_hCurPen);
//         ::SelectObject(m_hMemDC, m_hCurBrush);
//         ::SelectObject(m_hMemDC, m_hCurFont);
//         ::SetTextColor(m_hMemDC, clrCur);
//
//         if (m_bCopyBits) { ::BitBlt(m_hMemDC, pRect->left, pRect->top, m_nWidth, m_nHeight, m_hdc, pRect->left, pRect->top, SRCCOPY); }
//
//         //将alpha全部强制修改为0xFF。
//         BYTE *p = m_pBits + 3;
//
//         for (int i = 0; i < m_nHeight; i++)
//         {
//             for (int j = 0; j < m_nWidth; j++, p += 4) { *p = 0xFF; }
//         }
//     }
//
//     ~CHDCHelper()
//     {
//         //将alpha为0xFF的改为0,为0的改为0xFF
//         BYTE *p = m_pBits + 3;
//
//         for (int i = 0; i < m_nHeight; i++) { for (int j = 0; j < m_nWidth; j++, p += 4) { *p = ~(*p); } }
//
//         BLENDFUNCTION bf = { AC_SRC_OVER, 0, m_byAlpha, AC_SRC_ALPHA };
//         static PFunAlphaBlend spfAlphaBlend = GetAlphaBlend();
//         BOOL bRet = spfAlphaBlend(m_hdc, m_pRc->left, m_pRc->top, m_nWidth, m_nHeight,
//                                   m_hMemDC, m_pRc->left, m_pRc->top, m_nWidth, m_nHeight, bf);
//         ::DeleteDC(m_hMemDC);
//         ::DeleteObject(m_hBmp);
//
//         //恢复原DC的画笔，画刷，字体
//         ::SelectObject(m_hdc, m_hCurPen);
//         ::SelectObject(m_hdc, m_hCurBrush);
//         ::SelectObject(m_hdc, m_hCurFont);
//     }
//
//     operator HDC() { return m_hMemDC; }
//
// protected:
//     HDC     m_hdc;
//     HDC     m_hMemDC;
//     HBITMAP m_hBmp;
//     LPBYTE  m_pBits;
//     BYTE    m_byAlpha;
//     LPCRECT m_pRc;
//     int     m_nWidth;
//     int     m_nHeight;
//     BOOL    m_bCopyBits;
//
//     HGDIOBJ m_hCurPen;
//     HGDIOBJ m_hCurBrush;
//     HGDIOBJ m_hCurFont;
// };

/////////////////////////////////////////////////////////////////////////////////////
//
//

CRenderClip::CRenderClip() : hDC(NULL), hRgn(NULL), hOldRgn(NULL)
{
    rcItem.left = rcItem.top = rcItem.right = rcItem.bottom = 0;
}

CRenderClip::~CRenderClip()
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    ASSERT(::GetObjectType(hRgn) == OBJ_REGION);
    ASSERT(::GetObjectType(hOldRgn) == OBJ_REGION);
    ::SelectClipRgn(hDC, hOldRgn);
    ::DeleteObject(hOldRgn);
    ::DeleteObject(hRgn);
}

void CRenderClip::GenerateClip(HDC hDC, const RECT &rc, CRenderClip &clip)
{
    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rc);
    ::CombineRgn(clip.hRgn, clip.hRgn, clip.hOldRgn, RGN_AND);
    ::SelectClipRgn(hDC, clip.hRgn);
    clip.hDC = hDC;
    clip.rcItem = rc;
}

void CRenderClip::GenerateRoundClip(HDC hDC, const RECT &rc, const RECT &rcItem, int width, int height,
                                    CRenderClip &clip)
{
    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    clip.hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    clip.hRgn = ::CreateRectRgnIndirect(&rc);
    HRGN hRgnItem = ::CreateRoundRectRgn(rcItem.left, rcItem.top, rcItem.right + 1, rcItem.bottom + 1,
                                         width, height);
    ::CombineRgn(clip.hRgn, clip.hRgn, hRgnItem, RGN_AND);
    ::CombineRgn(clip.hRgn, clip.hRgn, clip.hOldRgn, RGN_AND);
    ::SelectClipRgn(hDC, clip.hRgn);
    clip.hDC = hDC;
    clip.rcItem = rc;
    ::DeleteObject(hRgnItem);
}

DUI_INLINE void CRenderClip::UseOldClipBegin(HDC hDC, CRenderClip &clip)
{
    ::SelectClipRgn(hDC, clip.hOldRgn);
}

DUI_INLINE void CRenderClip::UseOldClipEnd(HDC hDC, CRenderClip &clip)
{
    ::SelectClipRgn(hDC, clip.hRgn);
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

void CRenderEngine::ParseDrawInfo(TDrawInfo &drawInfo, CDuiString &sResType, DWORD &dwMask, bool &bHSL)
{
    // 1、aaa.jpg
    // 2、file='aaa.jpg' res='' restype='0' dest='0,0,0,0' source='0,0,0,0' scale9='0,0,0,0'
    // mask='#FF0000' fade='255' hole='false' xtiled='false' ytiled='false' hsl='false'
    // align='left' valign='center'
    if (drawInfo.sDrawString.IsEmpty()) { return; }

    CDuiString sImageName = drawInfo.sDrawString;
    sResType.Empty();
    dwMask = 0;
    bHSL = false;

    CDuiString sItem;
    CDuiString sValue;
    LPTSTR pstr = NULL;
    LPCTSTR pstrImage = drawInfo.sDrawString.GetData();

    while (*pstrImage != _T('\0'))
    {
        sItem.Empty();
        sValue.Empty();

        while (*pstrImage > _T('\0') && *pstrImage <= _T(' ')) { pstrImage = ::CharNext(pstrImage); }

        while (*pstrImage != _T('\0') && *pstrImage != _T('=') && *pstrImage > _T(' '))
        {
            LPTSTR pstrTemp = ::CharNext(pstrImage);

            while (pstrImage < pstrTemp)
            {
                sItem += *pstrImage++;
            }
        }

        while (*pstrImage > _T('\0') && *pstrImage <= _T(' ')) { pstrImage = ::CharNext(pstrImage); }

        if (*pstrImage++ != _T('=')) { break; }

        while (*pstrImage > _T('\0') && *pstrImage <= _T(' ')) { pstrImage = ::CharNext(pstrImage); }

        if (*pstrImage++ != _T('\'')) { break; }

        while (*pstrImage != _T('\0') && *pstrImage != _T('\''))
        {
            LPTSTR pstrTemp = ::CharNext(pstrImage);

            while (pstrImage < pstrTemp)
            {
                sValue += *pstrImage++;
            }
        }

        if (*pstrImage++ != _T('\'')) { break; }

        if (!sValue.IsEmpty())
        {
            if (sItem == _T("file"))
            {
                sImageName = sValue;
            }
            else if (sItem == _T("res"))
            {
                sImageName = sValue;
            }
            else if (sItem == _T("restype"))
            {
                sResType = sValue;
            }
            else if (sItem == _T("color"))
            {
                sResType = RES_TYPE_COLOR;
                sImageName = sValue;
            }
            else if (sItem == _T("dest"))
            {
                drawInfo.rcDestOffset = ParseRect(sValue);
            }
            else if (sItem == _T("source"))
            {
                drawInfo.rcBmpPart = ParseRect(sValue);
            }
            else if (sItem == _T("corner") || sItem == _T("scale9"))
            {
                drawInfo.rcScale9 = ParseRect(sValue);
            }
            else if (sItem == _T("mask"))
            {
                dwMask = ParseColor(sValue);
            }
            else if (sItem == _T("fade"))
            {
                drawInfo.byFade = ParseByte(sValue);
            }
            else if (sItem == _T("hole"))
            {
                drawInfo.bHole = ParseBool(sValue);
            }
            else if (sItem == _T("xtiled"))
            {
                drawInfo.bTiledX = ParseBool(sValue);
            }
            else if (sItem == _T("ytiled"))
            {
                drawInfo.bTiledY = ParseBool(sValue);
            }
            else if (sItem == _T("hsl"))
            {
                bHSL = ParseBool(sValue);
            }
            else if (sItem == _T("align"))
            {
                if (ParseString(sValue) == _T("left")) { drawInfo.byAlign |= EPIC_ALIGN_LEFT; }

                if (ParseString(sValue) == _T("center")) { drawInfo.byAlign |= EPIC_ALIGN_CENTER; }

                if (ParseString(sValue) == _T("right")) { drawInfo.byAlign |= EPIC_ALIGN_RIGHT; }
            }
            else if (sItem == _T("valign"))
            {
                if (ParseString(sValue) == _T("top")) { drawInfo.byAlign |= EPIC_ALIGN_TOP; }

                if (ParseString(sValue) == _T("center")) { drawInfo.byAlign |= EPIC_ALIGN_VCENTER; }

                if (ParseString(sValue) == _T("bottom")) { drawInfo.byAlign |= EPIC_ALIGN_BOTTOM; }
            }
        }

        if (*pstrImage++ != _T(' ')) { break; }
    }

    if (sResType == RES_TYPE_COLOR)
    {
        drawInfo.rcScale9.left = drawInfo.rcScale9.right = 0;
        drawInfo.rcScale9.top = drawInfo.rcScale9.bottom = 0;
    }

    drawInfo.sImageName = sImageName;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//

DWORD CRenderEngine::AdjustColor(DWORD dwColor, short H, short S, short L)
{
    if (H == 180 && S == 100 && L == 100) { return dwColor; }

    float fH, fS, fL;
    float S1 = S / 100.0f;
    float L1 = L / 100.0f;
    RGBtoHSL(dwColor, &fH, &fS, &fL);
    fH += (H - 180);
    fH = fH > 0 ? fH : fH + 360;
    fS *= S1;
    fL *= L1;
    HSLtoRGB(&dwColor, fH, fS, fL);
    return dwColor;
}

HBITMAP CRenderEngine::CreateARGB32Bitmap(HDC hDC, int cx, int cy, COLORREF **pBits)
{
    BITMAPINFO bmi;
    ::ZeroMemory(&bmi, sizeof(BITMAPINFO));

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = -cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = cx * cy * 4;
    bmi.bmiHeader.biXPelsPerMeter = 0;
    bmi.bmiHeader.biYPelsPerMeter = 0;
    bmi.bmiHeader.biClrUsed = 0;
    bmi.bmiHeader.biClrImportant = 0;

    HBITMAP hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);
    return hBitmap;
}

void CRenderEngine::AdjustImage(bool bUseHSL, TImageInfo *imageInfo, short H, short S, short L)
{
    if (imageInfo == NULL || imageInfo->bUseHSL == false || imageInfo->hBitmap == NULL ||
        imageInfo->pBits == NULL || imageInfo->pSrcBits == NULL)
    { return; }

    if (bUseHSL == false || (H == 180 && S == 100 && L == 100))
    {
        ::CopyMemory(imageInfo->pBits, imageInfo->pSrcBits, imageInfo->nX * imageInfo->nY * 4);
        return;
    }

    float fH, fS, fL;
    float S1 = S / 100.0f;
    float L1 = L / 100.0f;

    for (int i = 0; i < imageInfo->nX * imageInfo->nY; i++)
    {
        RGBtoHSL(*(DWORD *)(imageInfo->pSrcBits + i * 4), &fH, &fS, &fL);
        fH += (H - 180);
        fH = fH > 0 ? fH : fH + 360;
        fS *= S1;
        fL *= L1;
        HSLtoRGB((DWORD *)(imageInfo->pBits + i * 4), fH, fS, fL);
    }
}

TImageInfo *CRenderEngine::LoadImage(STRINGorID bitmap, LPCTSTR type, DWORD mask)
{
    LPBYTE pData = NULL;
    DWORD dwSize = 0;

    do
    {
        if (type == NULL || 0 == type[0])
        {
            // 从文件加载
            CDuiString sFile = CPaintManagerUI::GetResourcePath();

            if (CPaintManagerUI::GetResourceZip().IsEmpty())
            {
                sFile += bitmap.m_lpstr;
                HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL, NULL);

                if (hFile == INVALID_HANDLE_VALUE) { break; }

                dwSize = ::GetFileSize(hFile, NULL);

                if (dwSize == 0) { ::CloseHandle(hFile); break; }

                DWORD dwRead = 0;
                pData = new BYTE[ dwSize ];
                ::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
                ::CloseHandle(hFile);

                if (dwRead != dwSize)
                {
                    delete[] pData;
                    pData = NULL;
                    break;
                }
            }
            else
            {
                sFile += CPaintManagerUI::GetResourceZip();
                HZIP hz = NULL;

                if (CPaintManagerUI::IsCachedResourceZip()) { hz = (HZIP)CPaintManagerUI::GetResourceZipHandle(); }
                else { hz = OpenZip((void *)sFile.GetData(), 0, 2); }

                if (hz == NULL) { break; }

                ZIPENTRY ze;
                int i;

                if (FindZipItem(hz, bitmap.m_lpstr, true, &i, &ze) != 0) { break; }

                dwSize = ze.unc_size;

                if (dwSize == 0) { break; }

                pData = new BYTE[ dwSize ];
                int res = UnzipItem(hz, i, pData, dwSize, 3);

                if (res != 0x00000000 && res != 0x00000600)
                {
                    delete[] pData;
                    pData = NULL;

                    if (!CPaintManagerUI::IsCachedResourceZip()) { CloseZip(hz); }

                    break;
                }

                if (!CPaintManagerUI::IsCachedResourceZip()) { CloseZip(hz); }
            }
        }
        else if (_tcscmp(type, RES_TYPE_COLOR) == 0)
        {
            pData = (PBYTE)0x1;  /* dummy pointer */
        }
        else
        {
            // 从资源中加载
            HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), bitmap.m_lpstr, type);

            if (hResource == NULL) { break; }

            HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);

            if (hGlobal == NULL)
            {
                FreeResource(hResource);
                break;
            }

            dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);

            if (dwSize == 0) { break; }

            pData = new BYTE[ dwSize ];
            ::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
            ::FreeResource(hResource);
        }
    } while (0);

    while (!pData)
    {
        //读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE) { break; }

        dwSize = ::GetFileSize(hFile, NULL);

        if (dwSize == 0) { ::CloseHandle(hFile); break; }

        DWORD dwRead = 0;
        pData = new BYTE[ dwSize ];
        ::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
        ::CloseHandle(hFile);

        if (dwRead != dwSize)
        {
            delete[] pData;
            pData = NULL;
        }

        break;
    }

    if (!pData)
    {
        //::MessageBox(0, _T("读取图片数据失败！"), _T("抓BUG"), MB_OK);
        return NULL;
    }

    LPBYTE pImage = NULL;
    int x = 1, y = 1, n;

    if (!type || _tcscmp(type, RES_TYPE_COLOR) != 0)
    {
        pImage = stbi_load_from_memory(pData, dwSize, &x, &y, &n, 4);
        delete[] pData;

        if (!pImage)
        {
            //::MessageBox(0, _T("解析图片失败"), _T("抓BUG"), MB_OK);
            return NULL;
        }
    }

    BITMAPINFO bmi;
    ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = x;
    bmi.bmiHeader.biHeight = -y;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = x * y * 4;

    bool bAlphaChannel = false;
    LPBYTE pDest = NULL;
    HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pDest, NULL, 0);

    if (!hBitmap)
    {
        //::MessageBox(0, _T("CreateDIBSection失败"), _T("抓BUG"), MB_OK);
        return NULL;
    }

    BYTE bColorBits[4] = { 0 };

    if (type && _tcscmp(type, RES_TYPE_COLOR) == 0)
    {
        LPTSTR pstr = NULL;
        LPCTSTR pstrValue = bitmap.m_lpstr;

        if (*pstrValue == _T('#')) { pstrValue = ::CharNext(pstrValue); }

        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);

        pImage = (LPBYTE)&clrColor;
        /* BGRA -> RGBA */
        bColorBits[3] = pImage[3];
        bColorBits[2] = pImage[0];
        bColorBits[1] = pImage[1];
        bColorBits[0] = pImage[2];
        pImage = bColorBits;
    }

    for (int i = 0; i < x * y; i++)
    {
        pDest[i * 4 + 3] = pImage[i * 4 + 3];

        if (pDest[i * 4 + 3] < 255)
        {
            pDest[i * 4] = (BYTE)(DWORD(pImage[i * 4 + 2]) * pImage[i * 4 + 3] / 255);
            pDest[i * 4 + 1] = (BYTE)(DWORD(pImage[i * 4 + 1]) * pImage[i * 4 + 3] / 255);
            pDest[i * 4 + 2] = (BYTE)(DWORD(pImage[i * 4]) * pImage[i * 4 + 3] / 255);
            bAlphaChannel = true;
        }
        else
        {
            pDest[i * 4] = pImage[i * 4 + 2];
            pDest[i * 4 + 1] = pImage[i * 4 + 1];
            pDest[i * 4 + 2] = pImage[i * 4];
        }

        if (*(DWORD *)(&pDest[i * 4]) == mask)
        {
            pDest[i * 4] = (BYTE)0;
            pDest[i * 4 + 1] = (BYTE)0;
            pDest[i * 4 + 2] = (BYTE)0;
            pDest[i * 4 + 3] = (BYTE)0;
            bAlphaChannel = true;
        }
    }

    if (!type || _tcscmp(type, RES_TYPE_COLOR) != 0)
    {
        stbi_image_free(pImage);
    }

    TImageInfo *data = new TImageInfo;
    data->hBitmap = hBitmap;
    data->pBits = pDest;
    data->nX = x;
    data->nY = y;
    data->bAlpha = bAlphaChannel;
    data->bUseHSL = false;
    data->pSrcBits = NULL;
    return data;
}

DuiLib::CGifInfo *CRenderEngine::LoadGif(TDrawInfo &drawInfo)
{
    // 支持 file res restype mask
    CDuiString sResType;
    DWORD dwMask = 0;
    bool bUseHSL = false;
    ParseDrawInfo(drawInfo, sResType, dwMask, bUseHSL);
    // 如果用户使用的是非 gif 图片，则报错；使用资源图片时，不做判断
    ASSERT(sResType.IsEmpty() && -1 != drawInfo.sImageName.Find(_T(".gif")) && _T("Must be GIF picture."));

    if (sResType.IsEmpty() && -1 == drawInfo.sImageName.Find(_T(".gif"))) { return NULL; }

    return LoadGif((LPCTSTR)drawInfo.sImageName, sResType, dwMask);
}

CGifInfo *CRenderEngine::LoadGif(STRINGorID bitmap, LPCTSTR type, DWORD mask)
{
    if (NULL != type && _tcscmp(type, RES_TYPE_COLOR) == 0) { return NULL; }

    LPBYTE pData = NULL;
    DWORD dwSize = 0;

    do
    {
        if (type == NULL || 0 == type[0])
        {
            CDuiString sFile = CPaintManagerUI::GetResourcePath();

            if (CPaintManagerUI::GetResourceZip().IsEmpty())
            {
                sFile += bitmap.m_lpstr;
                HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                            FILE_ATTRIBUTE_NORMAL, NULL);

                if (hFile == INVALID_HANDLE_VALUE) { break; }

                dwSize = ::GetFileSize(hFile, NULL);

                if (dwSize == 0) { ::CloseHandle(hFile); break; }

                DWORD dwRead = 0;
                pData = new BYTE[dwSize];
                ::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
                ::CloseHandle(hFile);

                if (dwRead != dwSize)
                {
                    delete[ ] pData;
                    pData = NULL;
                    break;
                }
            }
            else
            {
                sFile += CPaintManagerUI::GetResourceZip();
                HZIP hz = NULL;

                if (CPaintManagerUI::IsCachedResourceZip()) { hz = (HZIP)CPaintManagerUI::GetResourceZipHandle(); }
                else { hz = OpenZip((void *)sFile.GetData(), 0, 2); }

                if (hz == NULL) { break; }

                ZIPENTRY ze;
                int i;

                if (FindZipItem(hz, bitmap.m_lpstr, true, &i, &ze) != 0) { break; }

                dwSize = ze.unc_size;

                if (dwSize == 0) { break; }

                pData = new BYTE[dwSize];
                int res = UnzipItem(hz, i, pData, dwSize, 3);

                if (res != 0x00000000 && res != 0x00000600)
                {
                    delete[ ] pData;
                    pData = NULL;

                    if (!CPaintManagerUI::IsCachedResourceZip()) { CloseZip(hz); }

                    break;
                }

                if (!CPaintManagerUI::IsCachedResourceZip()) { CloseZip(hz); }
            }
        }
        else
        {
            HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), bitmap.m_lpstr, type);

            if (hResource == NULL) { break; }

            HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);

            if (hGlobal == NULL)
            {
                FreeResource(hResource);
                break;
            }

            dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);

            if (dwSize == 0) { break; }

            pData = new BYTE[dwSize];
            ::CopyMemory(pData, (LPBYTE)::LockResource(hGlobal), dwSize);
            ::FreeResource(hResource);
        }
    } while (0);

    while (!pData)
    {
        //读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile = ::CreateFile(bitmap.m_lpstr, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                                    FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE) { break; }

        dwSize = ::GetFileSize(hFile, NULL);

        if (dwSize == 0) { ::CloseHandle(hFile); break; }

        DWORD dwRead = 0;
        pData = new BYTE[dwSize];
        ::ReadFile(hFile, pData, dwSize, &dwRead, NULL);
        ::CloseHandle(hFile);

        if (dwRead != dwSize)
        {
            delete[ ] pData;
            pData = NULL;
        }

        break;
    }

    if (!pData)
    {
        //::MessageBox(0, _T("读取图片数据失败！"), _T("抓BUG"), MB_OK);
        return NULL;
    }

    LPBYTE pImage = NULL;
    int w, h, n;
    int comp = 0;
    int *pDelay = NULL;
    pImage = stbi_load_gif_from_memory(pData, dwSize, &pDelay, &w, &h, &n, &comp, 4);
    delete[ ] pData;
    pData = NULL;
    ASSERT(NULL != pImage && n > 0 && w > 0 && h > 0 && _T("Incorrect GIF file."));

    if (NULL == pImage || n <= 0 || w <= 0 || h <= 0) { return NULL; }

    // 处理每一帧图像
    CGifInfo *pcGifInfo = new CGifInfo(n);

    pData = pImage;
    int nStride = w * h * comp;

    for (int i = 0; i < n; ++i)
    {
        TImageInfo *ptImg = new TImageInfo;
        ptImg->bUseHSL = false;
        ptImg->pSrcBits = NULL;
        ptImg->nX = w;                  // 宽
        ptImg->nY = h;                  // 高
        ptImg->dwMask = *(pDelay + i);  // 该帧显示时间
        pData = pImage + i * nStride;

        BITMAPINFO bmi;
        ::ZeroMemory(&bmi, sizeof(BITMAPINFO));
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = w;
        bmi.bmiHeader.biHeight = -h;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = comp * 8;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = w * h * comp;
        bool bAlpha = false;
        LPBYTE pDest = NULL;
        HBITMAP hBitmap = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pDest, NULL, 0);

        if (!hBitmap) { continue; }

        for (int i = 0; i < w * h; i++)
        {
            pDest[i * 4 + 3] = pData[i * 4 + 3];

            if (pDest[i * 4 + 3] < 255)
            {
                pDest[i * 4] = (BYTE)(DWORD(pData[i * 4 + 2]) * pData[i * 4 + 3] / 255);
                pDest[i * 4 + 1] = (BYTE)(DWORD(pData[i * 4 + 1]) * pData[i * 4 + 3] / 255);
                pDest[i * 4 + 2] = (BYTE)(DWORD(pData[i * 4]) * pData[i * 4 + 3] / 255);
                bAlpha = true;
            }
            else
            {
                pDest[i * 4] = pData[i * 4 + 2];
                pDest[i * 4 + 1] = pData[i * 4 + 1];
                pDest[i * 4 + 2] = pData[i * 4];
            }

            if (*(DWORD *)(&pDest[i * 4]) == mask)
            {
                pDest[i * 4] = (BYTE)0;
                pDest[i * 4 + 1] = (BYTE)0;
                pDest[i * 4 + 2] = (BYTE)0;
                pDest[i * 4 + 3] = (BYTE)0;
                bAlpha = true;
            }
        }//end for

        ptImg->bAlpha = bAlpha;
        ptImg->hBitmap = hBitmap;
        ptImg->pBits = pDest;
        pcGifInfo->AddFrame(ptImg);
    }

    stbi_image_free(pDelay);
    stbi_image_free(pImage);
    return pcGifInfo;
}

void CRenderEngine::FreeImage(TImageInfo *bitmap, bool bDelete)
{
    if (bitmap == NULL) { return; }

    if (bitmap->hBitmap)
    {
        ::DeleteObject(bitmap->hBitmap);
        bitmap->hBitmap = NULL;
    }

    if (bitmap->pSrcBits)
    {
        delete[] bitmap->pSrcBits;
        bitmap->pSrcBits = NULL;
    }

    if (bDelete) { delete bitmap; }
}

void CRenderEngine::DrawImage(HDC hDC, HBITMAP hBitmap, const RECT &rc, const RECT &rcPaint,
                              const RECT &rcBmpPart, const RECT &rcScale9, bool bAlpha,
                              BYTE uFade, bool bHole, bool bTiledX, bool bTiledY)
{
    // 2020-2-11 zhuyadong 加上 ASSERT，会导致Debug版本提示断言错误，不利于调试
    //ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    if (hDC == NULL || hBitmap == NULL) { return; }

    static PFunAlphaBlend spfAlphaBlend = GetAlphaBlend();

    HDC hCloneDC = ::CreateCompatibleDC(hDC);
    HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
    ::SetStretchBltMode(hDC, COLORONCOLOR);

    RECT rcTemp = {0};
    RECT rcDest = {0};

    if (spfAlphaBlend && (bAlpha || uFade < 255))
    {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, uFade, AC_SRC_ALPHA };

        // middle
        if (!bHole)
        {
            rcDest.left = rc.left + rcScale9.left;
            rcDest.top = rc.top + rcScale9.top;
            rcDest.right = rc.right - rcScale9.right;
            rcDest.bottom = rc.bottom - rcScale9.bottom;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                if (!bTiledX && !bTiledY)
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                  rcBmpPart.left + rcScale9.left, rcBmpPart.top + rcScale9.top,
                                  rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right,
                                  rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom, bf);
                }
                else if (bTiledX && bTiledY)
                {
                    LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right;
                    LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom;
                    int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                    int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;

                    for (int j = 0; j < iTimesY; ++j)
                    {
                        LONG lDestTop = rcDest.top + lHeight * j;
                        LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                        LONG lDrawHeight = lHeight;

                        if (lDestBottom > rcDest.bottom)
                        {
                            lDrawHeight -= lDestBottom - rcDest.bottom;
                            lDestBottom = rcDest.bottom;
                        }

                        for (int i = 0; i < iTimesX; ++i)
                        {
                            LONG lDestLeft = rcDest.left + lWidth * i;
                            LONG lDestRight = rcDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;

                            if (lDestRight > rcDest.right)
                            {
                                lDrawWidth -= lDestRight - rcDest.right;
                                lDestRight = rcDest.right;
                            }

                            spfAlphaBlend(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j,
                                          lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC,
                                          rcBmpPart.left + rcScale9.left, rcBmpPart.top + rcScale9.top, lDrawWidth, lDrawHeight, bf);
                        }
                    }
                }
                else if (bTiledX)
                {
                    LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right;
                    int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                    rcDest.bottom -= rcDest.top;

                    for (int i = 0; i < iTimes; ++i)
                    {
                        LONG lDestLeft = rcDest.left + lWidth * i;
                        LONG lDestRight = rcDest.left + lWidth * (i + 1);
                        LONG lDrawWidth = lWidth;

                        if (lDestRight > rcDest.right)
                        {
                            lDrawWidth -= lDestRight - rcDest.right;
                            lDestRight = rcDest.right;
                        }

                        spfAlphaBlend(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom,
                                      hCloneDC, rcBmpPart.left + rcScale9.left, rcBmpPart.top + rcScale9.top,
                                      lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom, bf);
                    }
                }
                else   // bTiledY
                {
                    LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom;
                    int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;
                    rcDest.right -= rcDest.left;

                    for (int i = 0; i < iTimes; ++i)
                    {
                        LONG lDestTop = rcDest.top + lHeight * i;
                        LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                        LONG lDrawHeight = lHeight;

                        if (lDestBottom > rcDest.bottom)
                        {
                            lDrawHeight -= lDestBottom - rcDest.bottom;
                            lDestBottom = rcDest.bottom;
                        }

                        spfAlphaBlend(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop,
                                      hCloneDC, rcBmpPart.left + rcScale9.left, rcBmpPart.top + rcScale9.top,
                                      rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right, lDrawHeight, bf);
                    }
                }
            }
        }

        // left-top
        if (rcScale9.left > 0 && rcScale9.top > 0)
        {
            rcDest.left = rc.left;
            rcDest.top = rc.top;
            rcDest.right = rcScale9.left;
            rcDest.bottom = rcScale9.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                              rcBmpPart.left, rcBmpPart.top, rcScale9.left, rcScale9.top, bf);
            }
        }

        // top
        if (rcScale9.top > 0)
        {
            rcDest.left = rc.left + rcScale9.left;
            rcDest.top = rc.top;
            rcDest.right = rc.right - rc.left - rcScale9.left - rcScale9.right;
            rcDest.bottom = rcScale9.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                              rcBmpPart.left + rcScale9.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left - \
                              rcScale9.left - rcScale9.right, rcScale9.top, bf);
            }
        }

        // right-top
        if (rcScale9.right > 0 && rcScale9.top > 0)
        {
            rcDest.left = rc.right - rcScale9.right;
            rcDest.top = rc.top;
            rcDest.right = rcScale9.right;
            rcDest.bottom = rcScale9.top;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC, \
                              rcBmpPart.right - rcScale9.right, rcBmpPart.top, rcScale9.right, rcScale9.top, bf);
            }
        }

        // left
        if (rcScale9.left > 0)
        {
            rcDest.left = rc.left;
            rcDest.top = rc.top + rcScale9.top;
            rcDest.right = rcScale9.left;
            rcDest.bottom = rc.bottom - rc.top - rcScale9.top - rcScale9.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                              rcBmpPart.left, rcBmpPart.top + rcScale9.top, rcScale9.left, rcBmpPart.bottom -
                              rcBmpPart.top - rcScale9.top - rcScale9.bottom, bf);
            }
        }

        // right
        if (rcScale9.right > 0)
        {
            rcDest.left = rc.right - rcScale9.right;
            rcDest.top = rc.top + rcScale9.top;
            rcDest.right = rcScale9.right;
            rcDest.bottom = rc.bottom - rc.top - rcScale9.top - rcScale9.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                              rcBmpPart.right - rcScale9.right, rcBmpPart.top + rcScale9.top, rcScale9.right,
                              rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom, bf);
            }
        }

        // left-bottom
        if (rcScale9.left > 0 && rcScale9.bottom > 0)
        {
            rcDest.left = rc.left;
            rcDest.top = rc.bottom - rcScale9.bottom;
            rcDest.right = rcScale9.left;
            rcDest.bottom = rcScale9.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                              rcBmpPart.left, rcBmpPart.bottom - rcScale9.bottom, rcScale9.left, rcScale9.bottom, bf);
            }
        }

        // bottom
        if (rcScale9.bottom > 0)
        {
            rcDest.left = rc.left + rcScale9.left;
            rcDest.top = rc.bottom - rcScale9.bottom;
            rcDest.right = rc.right - rc.left - rcScale9.left - rcScale9.right;
            rcDest.bottom = rcScale9.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                              rcBmpPart.left + rcScale9.left, rcBmpPart.bottom - rcScale9.bottom,
                              rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right, rcScale9.bottom, bf);
            }
        }

        // right-bottom
        if (rcScale9.right > 0 && rcScale9.bottom > 0)
        {
            rcDest.left = rc.right - rcScale9.right;
            rcDest.top = rc.bottom - rcScale9.bottom;
            rcDest.right = rcScale9.right;
            rcDest.bottom = rcScale9.bottom;
            rcDest.right += rcDest.left;
            rcDest.bottom += rcDest.top;

            if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
            {
                rcDest.right -= rcDest.left;
                rcDest.bottom -= rcDest.top;
                spfAlphaBlend(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                              rcBmpPart.right - rcScale9.right, rcBmpPart.bottom - rcScale9.bottom, rcScale9.right, \
                              rcScale9.bottom, bf);
            }
        }
    }
    else
    {
        if (rc.right - rc.left == rcBmpPart.right - rcBmpPart.left
            && rc.bottom - rc.top == rcBmpPart.bottom - rcBmpPart.top
            && rcScale9.left == 0 && rcScale9.right == 0 && rcScale9.top == 0 && rcScale9.bottom == 0)
        {
            if (::IntersectRect(&rcTemp, &rcPaint, &rc))
            {
                ::BitBlt(hDC, rcTemp.left, rcTemp.top, rcTemp.right - rcTemp.left, rcTemp.bottom - rcTemp.top,
                         hCloneDC, rcBmpPart.left + rcTemp.left - rc.left, rcBmpPart.top + rcTemp.top - rc.top, SRCCOPY);
            }
        }
        else
        {
            // middle
            if (!bHole)
            {
                rcDest.left = rc.left + rcScale9.left;
                rcDest.top = rc.top + rcScale9.top;
                rcDest.right = rc.right - rc.left - rcScale9.left - rcScale9.right;
                rcDest.bottom = rc.bottom - rc.top - rcScale9.top - rcScale9.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    if (!bTiledX && !bTiledY)
                    {
                        rcDest.right -= rcDest.left;
                        rcDest.bottom -= rcDest.top;
                        ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                     rcBmpPart.left + rcScale9.left, rcBmpPart.top + rcScale9.top,
                                     rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right,
                                     rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom, SRCCOPY);
                    }
                    else if (bTiledX && bTiledY)
                    {
                        LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right;
                        LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom;
                        int iTimesX = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;
                        int iTimesY = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;

                        for (int j = 0; j < iTimesY; ++j)
                        {
                            LONG lDestTop = rcDest.top + lHeight * j;
                            LONG lDestBottom = rcDest.top + lHeight * (j + 1);
                            LONG lDrawHeight = lHeight;

                            if (lDestBottom > rcDest.bottom)
                            {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }

                            for (int i = 0; i < iTimesX; ++i)
                            {
                                LONG lDestLeft = rcDest.left + lWidth * i;
                                LONG lDestRight = rcDest.left + lWidth * (i + 1);
                                LONG lDrawWidth = lWidth;

                                if (lDestRight > rcDest.right)
                                {
                                    lDrawWidth -= lDestRight - rcDest.right;
                                    lDestRight = rcDest.right;
                                }

                                ::BitBlt(hDC, rcDest.left + lWidth * i, rcDest.top + lHeight * j,
                                         lDestRight - lDestLeft, lDestBottom - lDestTop, hCloneDC,
                                         rcBmpPart.left + rcScale9.left, rcBmpPart.top + rcScale9.top, SRCCOPY);
                            }
                        }
                    }
                    else if (bTiledX)
                    {
                        LONG lWidth = rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right;
                        int iTimes = (rcDest.right - rcDest.left + lWidth - 1) / lWidth;

                        for (int i = 0; i < iTimes; ++i)
                        {
                            LONG lDestLeft = rcDest.left + lWidth * i;
                            LONG lDestRight = rcDest.left + lWidth * (i + 1);
                            LONG lDrawWidth = lWidth;

                            if (lDestRight > rcDest.right)
                            {
                                lDrawWidth -= lDestRight - rcDest.right;
                                lDestRight = rcDest.right;
                            }

                            rcDest.bottom -= rcDest.top;
                            ::StretchBlt(hDC, lDestLeft, rcDest.top, lDestRight - lDestLeft, rcDest.bottom,
                                         hCloneDC, rcBmpPart.left + rcScale9.left, rcBmpPart.top + rcScale9.top,
                                         lDrawWidth, rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom, SRCCOPY);
                        }
                    }
                    else   // bTiledY
                    {
                        LONG lHeight = rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom;
                        int iTimes = (rcDest.bottom - rcDest.top + lHeight - 1) / lHeight;

                        for (int i = 0; i < iTimes; ++i)
                        {
                            LONG lDestTop = rcDest.top + lHeight * i;
                            LONG lDestBottom = rcDest.top + lHeight * (i + 1);
                            LONG lDrawHeight = lHeight;

                            if (lDestBottom > rcDest.bottom)
                            {
                                lDrawHeight -= lDestBottom - rcDest.bottom;
                                lDestBottom = rcDest.bottom;
                            }

                            rcDest.right -= rcDest.left;
                            ::StretchBlt(hDC, rcDest.left, rcDest.top + lHeight * i, rcDest.right, lDestBottom - lDestTop,
                                         hCloneDC, rcBmpPart.left + rcScale9.left, rcBmpPart.top + rcScale9.top,
                                         rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right, lDrawHeight, SRCCOPY);
                        }
                    }
                }
            }

            // left-top
            if (rcScale9.left > 0 && rcScale9.top > 0)
            {
                rcDest.left = rc.left;
                rcDest.top = rc.top;
                rcDest.right = rcScale9.left;
                rcDest.bottom = rcScale9.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                 rcBmpPart.left, rcBmpPart.top, rcScale9.left, rcScale9.top, SRCCOPY);
                }
            }

            // top
            if (rcScale9.top > 0)
            {
                rcDest.left = rc.left + rcScale9.left;
                rcDest.top = rc.top;
                rcDest.right = rc.right - rc.left - rcScale9.left - rcScale9.right;
                rcDest.bottom = rcScale9.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                 rcBmpPart.left + rcScale9.left, rcBmpPart.top, rcBmpPart.right - rcBmpPart.left -
                                 rcScale9.left - rcScale9.right, rcScale9.top, SRCCOPY);
                }
            }

            // right-top
            if (rcScale9.right > 0 && rcScale9.top > 0)
            {
                rcDest.left = rc.right - rcScale9.right;
                rcDest.top = rc.top;
                rcDest.right = rcScale9.right;
                rcDest.bottom = rcScale9.top;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                 rcBmpPart.right - rcScale9.right, rcBmpPart.top, rcScale9.right, rcScale9.top, SRCCOPY);
                }
            }

            // left
            if (rcScale9.left > 0)
            {
                rcDest.left = rc.left;
                rcDest.top = rc.top + rcScale9.top;
                rcDest.right = rcScale9.left;
                rcDest.bottom = rc.bottom - rc.top - rcScale9.top - rcScale9.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                 rcBmpPart.left, rcBmpPart.top + rcScale9.top, rcScale9.left, rcBmpPart.bottom -
                                 rcBmpPart.top - rcScale9.top - rcScale9.bottom, SRCCOPY);
                }
            }

            // right
            if (rcScale9.right > 0)
            {
                rcDest.left = rc.right - rcScale9.right;
                rcDest.top = rc.top + rcScale9.top;
                rcDest.right = rcScale9.right;
                rcDest.bottom = rc.bottom - rc.top - rcScale9.top - rcScale9.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                 rcBmpPart.right - rcScale9.right, rcBmpPart.top + rcScale9.top, rcScale9.right,
                                 rcBmpPart.bottom - rcBmpPart.top - rcScale9.top - rcScale9.bottom, SRCCOPY);
                }
            }

            // left-bottom
            if (rcScale9.left > 0 && rcScale9.bottom > 0)
            {
                rcDest.left = rc.left;
                rcDest.top = rc.bottom - rcScale9.bottom;
                rcDest.right = rcScale9.left;
                rcDest.bottom = rcScale9.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                 rcBmpPart.left, rcBmpPart.bottom - rcScale9.bottom, rcScale9.left, rcScale9.bottom, SRCCOPY);
                }
            }

            // bottom
            if (rcScale9.bottom > 0)
            {
                rcDest.left = rc.left + rcScale9.left;
                rcDest.top = rc.bottom - rcScale9.bottom;
                rcDest.right = rc.right - rc.left - rcScale9.left - rcScale9.right;
                rcDest.bottom = rcScale9.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                 rcBmpPart.left + rcScale9.left, rcBmpPart.bottom - rcScale9.bottom,
                                 rcBmpPart.right - rcBmpPart.left - rcScale9.left - rcScale9.right, rcScale9.bottom, SRCCOPY);
                }
            }

            // right-bottom
            if (rcScale9.right > 0 && rcScale9.bottom > 0)
            {
                rcDest.left = rc.right - rcScale9.right;
                rcDest.top = rc.bottom - rcScale9.bottom;
                rcDest.right = rcScale9.right;
                rcDest.bottom = rcScale9.bottom;
                rcDest.right += rcDest.left;
                rcDest.bottom += rcDest.top;

                if (::IntersectRect(&rcTemp, &rcPaint, &rcDest))
                {
                    rcDest.right -= rcDest.left;
                    rcDest.bottom -= rcDest.top;
                    ::StretchBlt(hDC, rcDest.left, rcDest.top, rcDest.right, rcDest.bottom, hCloneDC,
                                 rcBmpPart.right - rcScale9.right, rcBmpPart.bottom - rcScale9.bottom, rcScale9.right, \
                                 rcScale9.bottom, SRCCOPY);
                }
            }
        }
    }

    ::SelectObject(hCloneDC, hOldBitmap);
    ::DeleteDC(hCloneDC);
}

bool CRenderEngine::DrawImage(HDC hDC, CPaintManagerUI *pManager, const RECT &rcItem, const RECT &rcPaint,
                              TDrawInfo &drawInfo)
{
    if (pManager == NULL) { return true; }

    if (drawInfo.pImageInfo == NULL)
    {
        if (drawInfo.bLoaded) { return false; }

        drawInfo.bLoaded = true;

        CDuiString sResType;
        DWORD dwMask = 0;
        bool bHSL = false;
        ParseDrawInfo(drawInfo, sResType, dwMask, bHSL);
        const TImageInfo *data = pManager->GetImageEx(drawInfo.sImageName, sResType, dwMask, bHSL);

        if (!data) { return false; }

        drawInfo.pImageInfo = data;

        if (drawInfo.rcBmpPart.left == 0 && drawInfo.rcBmpPart.right == 0 &&
            drawInfo.rcBmpPart.top == 0 && drawInfo.rcBmpPart.bottom == 0)
        {
            drawInfo.rcBmpPart.right = data->nX;
            drawInfo.rcBmpPart.bottom = data->nY;
        }
    }

    if (drawInfo.rcBmpPart.right > drawInfo.pImageInfo->nX) { drawInfo.rcBmpPart.right = drawInfo.pImageInfo->nX; }

    if (drawInfo.rcBmpPart.bottom > drawInfo.pImageInfo->nY) { drawInfo.rcBmpPart.bottom = drawInfo.pImageInfo->nY; }

    RECT rcDest = rcItem;

    if (drawInfo.rcDestOffset.left != 0 || drawInfo.rcDestOffset.top != 0 ||
        drawInfo.rcDestOffset.right != 0 || drawInfo.rcDestOffset.bottom != 0)
    {
        rcDest.left = rcItem.left + drawInfo.rcDestOffset.left;
        rcDest.top = rcItem.top + drawInfo.rcDestOffset.top;

        if (drawInfo.rcDestOffset.right >= 0) { rcDest.right = rcItem.left + drawInfo.rcDestOffset.right; }
        else { rcDest.right = rcItem.right + drawInfo.rcDestOffset.right; }

        if (rcDest.right > rcItem.right) { rcDest.right = rcItem.right; }

        if (drawInfo.rcDestOffset.bottom >= 0) { rcDest.bottom = rcItem.top + drawInfo.rcDestOffset.bottom; }
        else { rcDest.bottom = rcItem.bottom + drawInfo.rcDestOffset.bottom; }

        if (rcDest.bottom > rcItem.bottom) { rcDest.bottom = rcItem.bottom; }
    }

    // 2018-07-16 根据图片对齐属性，调整绘制大小、位置
    if (EPIC_ALIGN_NONE != drawInfo.byAlign)
    {
        LONG nW = rcDest.right - rcDest.left;
        LONG nH = rcDest.bottom - rcDest.top;

        if (nW > drawInfo.pImageInfo->nX)
        {
            // 水平对齐
            if (drawInfo.byAlign & EPIC_ALIGN_LEFT)  { rcDest.right = rcDest.left + drawInfo.pImageInfo->nX; }

            if (drawInfo.byAlign & EPIC_ALIGN_RIGHT) { rcDest.left = rcDest.right - drawInfo.pImageInfo->nX; }

            if (drawInfo.byAlign & EPIC_ALIGN_CENTER)
            {
                rcDest.left = rcDest.left + (nW - drawInfo.pImageInfo->nX) / 2;
                rcDest.right = rcDest.left + drawInfo.pImageInfo->nX;
            }
        }

        if (nH > drawInfo.pImageInfo->nY)
        {
            // 垂直对齐
            if (drawInfo.byAlign & EPIC_ALIGN_TOP)    { rcDest.bottom = rcDest.top + drawInfo.pImageInfo->nY; }

            if (drawInfo.byAlign & EPIC_ALIGN_BOTTOM) { rcDest.top = rcDest.bottom - drawInfo.pImageInfo->nY; }

            if (drawInfo.byAlign & EPIC_ALIGN_VCENTER)
            {
                rcDest.top = rcDest.top + (nH - drawInfo.pImageInfo->nY) / 2;
                rcDest.bottom = rcDest.top + drawInfo.pImageInfo->nY;
            }
        }
    }

    RECT rcTemp;

    if (!::IntersectRect(&rcTemp, &rcDest, &rcItem)) { return true; }

    if (!::IntersectRect(&rcTemp, &rcDest, &rcPaint)) { return true; }

    DrawImage(hDC, drawInfo.pImageInfo->hBitmap, rcDest, rcPaint, drawInfo.rcBmpPart,
              drawInfo.rcScale9, pManager->IsLayered() ? true : drawInfo.pImageInfo->bAlpha,
              drawInfo.byFade, drawInfo.bHole, drawInfo.bTiledX, drawInfo.bTiledY);
    return true;
}

void CRenderEngine::DrawColor(HDC hDC, const RECT &rc, DWORD color)
{
    if (color <= 0x00FFFFFF) { return; }

    //if (color >= 0xFF000000)
    //{
    //    ::SetBkColor(hDC, RGB(GetBValue(color), GetGValue(color), GetRValue(color)));
    //    ::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
    //}
    //else
    {
        // Create a new 32bpp bitmap with room for an alpha channel
        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = 1;
        bmi.bmiHeader.biHeight = 1;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;
        bmi.bmiHeader.biSizeImage = 1 * 1 * sizeof(DWORD);
        LPDWORD pDest = NULL;
        HBITMAP hBitmap = ::CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (LPVOID *) &pDest, NULL, 0);

        if (!hBitmap) { return; }

        // 2018-03-29 zyd 颜色透明显示
        *pDest = color;

        BLENDFUNCTION bf = { AC_SRC_OVER, 0, (color & 0xff000000) >> 24, AC_SRC_ALPHA };
        HDC hCloneDC = ::CreateCompatibleDC(hDC);
        HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
        static PFunAlphaBlend spfAlphaBlend = GetAlphaBlend();
        spfAlphaBlend(hDC, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hCloneDC, 0, 0, 1, 1, bf);
        ::SelectObject(hCloneDC, hOldBitmap);
        ::DeleteDC(hCloneDC);
        ::DeleteObject(hBitmap);
        //////////////////////////////////////////////////////////////////////////
        // RECT rcBmpPart = {0, 0, 1, 1};
        // RECT rcCorners = {0};
        // DrawImage(hDC, hBitmap, rc, rc, rcBmpPart, rcCorners, true, 255);
        // ::DeleteObject(hBitmap);
    }
}

void CRenderEngine::DrawGradient(HDC hDC, const RECT &rc, DWORD dwFirst, DWORD dwSecond, bool bVertical,
                                 int nSteps)
{
    typedef BOOL (WINAPI * PFunGradientFill)(HDC, PTRIVERTEX, ULONG, PVOID, ULONG, ULONG);
    static PFunGradientFill spfGradientFill =
        (PFunGradientFill)::GetProcAddress(::GetModuleHandle(_T("msimg32.dll")), "GradientFill");

    BYTE bAlpha = (BYTE)(((dwFirst >> 24) + (dwSecond >> 24)) >> 1);

    if (bAlpha == 0) { return; }

    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;
    RECT rcPaint = rc;
    HDC hPaintDC = hDC;
    HBITMAP hPaintBitmap = NULL;
    HBITMAP hOldPaintBitmap = NULL;

    if (bAlpha < 255)
    {
        rcPaint.left = rcPaint.top = 0;
        rcPaint.right = cx;
        rcPaint.bottom = cy;
        hPaintDC = ::CreateCompatibleDC(hDC);
        hPaintBitmap = ::CreateCompatibleBitmap(hDC, cx, cy);
        ASSERT(hPaintDC);
        ASSERT(hPaintBitmap);
        hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
    }

    if (spfGradientFill != NULL)
    {
        TRIVERTEX triv[2] =
        {
            { rcPaint.left, rcPaint.top, GetBValue(dwFirst) << 8, GetGValue(dwFirst) << 8, GetRValue(dwFirst) << 8, 0xFF00 },
            { rcPaint.right, rcPaint.bottom, GetBValue(dwSecond) << 8, GetGValue(dwSecond) << 8, GetRValue(dwSecond) << 8, 0xFF00 }
        };
        GRADIENT_RECT grc = { 0, 1 };
        spfGradientFill(hPaintDC, triv, 2, &grc, 1, bVertical ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
    }
    else
    {
        // Determine how many shades
        int nShift = 1;

        if (nSteps >= 64) { nShift = 6; }
        else if (nSteps >= 32) { nShift = 5; }
        else if (nSteps >= 16) { nShift = 4; }
        else if (nSteps >= 8) { nShift = 3; }
        else if (nSteps >= 4) { nShift = 2; }

        int nLines = 1 << nShift;

        for (int i = 0; i < nLines; i++)
        {
            // Do a little alpha blending
            BYTE bR = (BYTE)((GetBValue(dwSecond) * (nLines - i) + GetBValue(dwFirst) * i) >> nShift);
            BYTE bG = (BYTE)((GetGValue(dwSecond) * (nLines - i) + GetGValue(dwFirst) * i) >> nShift);
            BYTE bB = (BYTE)((GetRValue(dwSecond) * (nLines - i) + GetRValue(dwFirst) * i) >> nShift);
            // ... then paint with the resulting color
            HBRUSH hBrush = ::CreateSolidBrush(RGB(bR, bG, bB));
            RECT r2 = rcPaint;

            if (bVertical)
            {
                r2.bottom = rc.bottom - ((i * (rc.bottom - rc.top)) >> nShift);
                r2.top = rc.bottom - (((i + 1) * (rc.bottom - rc.top)) >> nShift);

                if ((r2.bottom - r2.top) > 0) { ::FillRect(hDC, &r2, hBrush); }
            }
            else
            {
                r2.left = rc.right - (((i + 1) * (rc.right - rc.left)) >> nShift);
                r2.right = rc.right - ((i * (rc.right - rc.left)) >> nShift);

                if ((r2.right - r2.left) > 0) { ::FillRect(hPaintDC, &r2, hBrush); }
            }

            ::DeleteObject(hBrush);
        }
    }

    if (bAlpha < 255)
    {
        BLENDFUNCTION bf = { AC_SRC_OVER, 0, bAlpha, AC_SRC_ALPHA };
        static PFunAlphaBlend spfAlphaBlend = GetAlphaBlend();
        spfAlphaBlend(hDC, rc.left, rc.top, cx, cy, hPaintDC, 0, 0, cx, cy, bf);
        ::SelectObject(hPaintDC, hOldPaintBitmap);
        ::DeleteObject(hPaintBitmap);
        ::DeleteDC(hPaintDC);
    }
}

void CRenderEngine::DrawLine(HDC hDC, const RECT &rc, int nSize, DWORD dwPenColor, int nStyle)
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

    //LOGPEN lg;
    //lg.lopnColor = RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor));
    //lg.lopnStyle = nStyle;
    //lg.lopnWidth.x = nSize;
    //HPEN hPen = CreatePenIndirect(&lg);

    // LOGBRUSH lb = { BS_SOLID, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)), 0 };
    // HPEN hPen = ExtCreatePen(nStyle, nSize, &lb, 0, NULL);
    // HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    // ::MoveToEx(hDC, rc.left, rc.top, NULL);
    // ::LineTo(hDC, rc.right, rc.bottom);
    // ::SelectObject(hDC, hOldPen);
    // ::DeleteObject(hPen);

    Graphics g(hDC);

    Pen pen(Color(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)), (REAL)nSize);
    pen.SetDashStyle(DashStyleSolid);
    pen.SetAlignment(PenAlignmentInset);

    if (nStyle & PS_DASH)               { pen.SetDashStyle(DashStyleDash); }
    else if (nStyle & PS_DOT)           { pen.SetDashStyle(DashStyleDot); }
    else if (nStyle & PS_DASHDOT)       { pen.SetDashStyle(DashStyleDashDot); }
    else if (nStyle & PS_DASHDOTDOT)    { pen.SetDashStyle(DashStyleDashDotDot); }

    g.DrawLine(&pen, rc.left, rc.top, rc.right, rc.bottom);

    // RECT rtTmp = rc;
    //
    // if (rtTmp.left == rtTmp.right)
    // {
    //     rtTmp.left -= nSize / 2;
    //     rtTmp.right = rtTmp.left + nSize;
    // }
    // else if (rtTmp.top == rtTmp.bottom)
    // {
    //     rtTmp.top -= nSize / 2;
    //     rtTmp.bottom = rtTmp.top + nSize;
    // }
    //
    // CHDCHelper dcHelper(hDC, &rtTmp, BYTE(dwPenColor >> 24));
    // LOGBRUSH lb = { BS_SOLID, RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)), 0 };
    // HPEN hPen = ExtCreatePen(nStyle, nSize, &lb, 0, NULL);
    // HPEN hOldPen = (HPEN)::SelectObject(dcHelper, hPen);
    // POINT ptTemp = { 0 };
    // ::MoveToEx(dcHelper, rc.left, rc.top, &ptTemp);
    // ::LineTo(dcHelper, rc.right, rc.bottom);
    // ::SelectObject(dcHelper, hOldPen);
    // ::DeleteObject(hPen);
}

void CRenderEngine::DrawRect(HDC hDC, const RECT &rc, int nSize, DWORD dwPenColor, int nStyle)
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    // HPEN hPen = ::CreatePen(nStyle | PS_INSIDEFRAME, nSize,
    //                         RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
    // HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    // ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
    // ::Rectangle(hDC, rc.left, rc.top, rc.right, rc.bottom);
    // ::SelectObject(hDC, hOldPen);
    // ::DeleteObject(hPen);

    Graphics g(hDC);

    Pen pen(Color(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)), (REAL)nSize);
    pen.SetDashStyle(DashStyleSolid);
    pen.SetAlignment(PenAlignmentInset);

    if (nStyle & PS_DASH)               { pen.SetDashStyle(DashStyleDash); }
    else if (nStyle & PS_DOT)           { pen.SetDashStyle(DashStyleDot); }
    else if (nStyle & PS_DASHDOT)       { pen.SetDashStyle(DashStyleDashDot); }
    else if (nStyle & PS_DASHDOTDOT)    { pen.SetDashStyle(DashStyleDashDotDot); }

    g.DrawRectangle(&pen, rc.left, rc.top, rc.right - rc.left - 1, rc.bottom - rc.top - 1);

    // CHDCHelper dcHelper(hDC, &rc, BYTE(dwPenColor >> 24));
    // HPEN hPen = ::CreatePen(nStyle | PS_INSIDEFRAME, nSize,
    //                         RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
    // HPEN hOldPen = (HPEN)::SelectObject(dcHelper, hPen);
    // ::SelectObject(dcHelper, ::GetStockObject(HOLLOW_BRUSH));
    // ::Rectangle(dcHelper, rc.left, rc.top, rc.right, rc.bottom);
    // ::SelectObject(dcHelper, hOldPen);
    // ::DeleteObject(hPen);
}

void CRenderEngine::DrawRoundRect(HDC hDC, const RECT &rc, int nSize, int width, int height, DWORD dwPenColor,
                                  int nStyle)
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);
    // HPEN hPen = ::CreatePen(nStyle | PS_INSIDEFRAME, nSize,
    //                         RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
    // HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    // ::SelectObject(hDC, ::GetStockObject(HOLLOW_BRUSH));
    // ::RoundRect(hDC, rc.left, rc.top, rc.right, rc.bottom, width, height);
    // ::SelectObject(hDC, hOldPen);
    // ::DeleteObject(hPen);

    Graphics g(hDC);

    Pen pen(Color(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)), (REAL)nSize);
    pen.SetDashStyle(DashStyleSolid);
    pen.SetAlignment(PenAlignmentInset);

    if (nStyle & PS_DASH)               { pen.SetDashStyle(DashStyleDash); }
    else if (nStyle & PS_DOT)           { pen.SetDashStyle(DashStyleDot); }
    else if (nStyle & PS_DASHDOT)       { pen.SetDashStyle(DashStyleDashDot); }
    else if (nStyle & PS_DASHDOTDOT)    { pen.SetDashStyle(DashStyleDashDotDot); }

    int nDiameterX = width * 2, nDiameterY = height * 2;
    GraphicsPath path;
    // 左上角圆弧 + 上边
    path.AddArc(rc.left, rc.top, nDiameterX, nDiameterY, 180, 90);
    path.AddLine(rc.left + width, rc.top, rc.right - width, rc.top);
    // 右上角圆弧 + 右边
    path.AddArc(rc.right - nDiameterX - 1, rc.top, nDiameterX, nDiameterY, 270, 90);
    path.AddLine(rc.right - 1, rc.top + height, rc.right - 1, rc.bottom - height);
    // 右下角圆弧 + 下边
    path.AddArc(rc.right - nDiameterX - 1, rc.bottom - nDiameterY - 1, nDiameterX, nDiameterY, 0, 90);
    path.AddLine(rc.right - width, rc.bottom - 1, rc.left + width, rc.bottom - 1);
    // 左下角圆弧 + 左边
    path.AddArc(rc.left, rc.bottom - nDiameterY - 1, nDiameterX, nDiameterY, 90, 90);
    path.AddLine(rc.left, rc.bottom - height, rc.left, rc.top + height);

    g.DrawPath(&pen, &path);

    // CHDCHelper dcHelper(hDC, &rc, BYTE(dwPenColor >> 24));
    // HPEN hPen = ::CreatePen(nStyle | PS_INSIDEFRAME, nSize,
    //                         RGB(GetBValue(dwPenColor), GetGValue(dwPenColor), GetRValue(dwPenColor)));
    // HPEN hOldPen = (HPEN)::SelectObject(dcHelper, hPen);
    // ::SelectObject(dcHelper, ::GetStockObject(HOLLOW_BRUSH));
    // ::RoundRect(dcHelper, rc.left, rc.top, rc.right, rc.bottom, width, height);
    // ::SelectObject(dcHelper, hOldPen);
    // ::DeleteObject(hPen);
}

void CRenderEngine::DrawText(HDC hDC, CPaintManagerUI *pManager, RECT &rc, LPCTSTR pstrText,
                             DWORD dwTextColor, int iFont, UINT uStyle)
{
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

    if (pstrText == NULL || pManager == NULL) { return; }

    if (pManager->IsLayered())
    {
        Graphics g(hDC);

        Font font(hDC, pManager->GetFont(iFont));
        RectF rectF((REAL)rc.left, (REAL)rc.top, REAL(rc.right - rc.left), REAL(rc.bottom - rc.top));
        SolidBrush brush(Color(255, GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));

        StringFormat stringFormat = StringFormat::GenericTypographic();

        if ((uStyle & DT_END_ELLIPSIS) != 0) { stringFormat.SetTrimming(StringTrimmingEllipsisCharacter); }

        int formatFlags = 0;

        if ((uStyle & DT_NOCLIP) != 0)      { formatFlags |= StringFormatFlagsNoClip; }

        if ((uStyle & DT_SINGLELINE) != 0)  { formatFlags |= StringFormatFlagsNoWrap; }

        stringFormat.SetFormatFlags(formatFlags);

        if ((uStyle & DT_LEFT) != 0)        { stringFormat.SetAlignment(StringAlignmentNear); }
        else if ((uStyle & DT_CENTER) != 0) { stringFormat.SetAlignment(StringAlignmentCenter); }
        else if ((uStyle & DT_RIGHT) != 0)  { stringFormat.SetAlignment(StringAlignmentFar); }
        else                                { stringFormat.SetAlignment(StringAlignmentNear); }

        stringFormat.GenericTypographic();

        if ((uStyle & DT_TOP) != 0)          { stringFormat.SetLineAlignment(StringAlignmentNear); }
        else if ((uStyle & DT_VCENTER) != 0) { stringFormat.SetLineAlignment(StringAlignmentCenter); }
        else if ((uStyle & DT_BOTTOM) != 0)  { stringFormat.SetLineAlignment(StringAlignmentFar); }
        else                                 { stringFormat.SetLineAlignment(StringAlignmentNear); }

#if !defined(UNICODE) || !defined(_UNICODE)
        LPWSTR pwstrText = NULL;
        int iLen = _tcslen(pstrText);
        pwstrText = new WCHAR[iLen + 1];
        ::ZeroMemory(pwstrText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, pstrText, -1, (LPWSTR)pwstrText, iLen);
#else
        LPCWSTR pwstrText = pstrText;
#endif

        if ((uStyle & DT_CALCRECT) != 0)
        {
            RectF bounds;
            g.MeasureString(pwstrText, -1, &font, rectF, &stringFormat, &bounds);

            // MeasureString存在计算误差，这里加一像素
            rc.bottom = rc.top + (long)bounds.Height + 1;
            rc.right = rc.left + (long)bounds.Width + 1;
        }
        else
        {
            g.DrawString(pwstrText, -1, &font, rectF, &stringFormat, &brush);
        }
    }
    else
    {
        ::SetBkMode(hDC, TRANSPARENT);
        ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
        HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
        ::DrawText(hDC, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
        ::SelectObject(hDC, hOldFont);

        // if (DT_CALCRECT & uStyle)
        // {
        //     // 由于 CHDCHelper 的构造和析构需要消耗大量时间，因此计算文本占用的矩形时直接在原HDC上进行
        //     ::SetBkMode(hDC, TRANSPARENT);
        //     ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
        //     HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
        //     ::DrawText(hDC, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
        //     ::SelectObject(hDC, hOldFont);
        // }
        // else
        // {
        //     CHDCHelper dcHelper(hDC, &rc, BYTE(dwTextColor >> 24));
        //     ::SetBkMode(dcHelper, TRANSPARENT);
        //     ::SetTextColor(dcHelper, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
        //     HFONT hOldFont = (HFONT)::SelectObject(dcHelper, pManager->GetFont(iFont));
        //     ::DrawText(dcHelper, pstrText, -1, &rc, uStyle | DT_NOPREFIX);
        //     ::SelectObject(dcHelper, hOldFont);
        // }
    }
}

void CRenderEngine::DrawHtmlText(HDC hDC, CPaintManagerUI *pManager, RECT &rc, LPCTSTR pstrText,
                                 DWORD dwTextColor, RECT *prcLinks, CDuiString *sLinks, int *pnLinkRects,
                                 int iDefaultFont, UINT uStyle)
{
    // 考虑到在xml编辑器中使用<>符号不方便，可以使用{}符号代替
    // 支持标签嵌套（如<l><b>text</b></l>），但是交叉嵌套是应该避免的（如<l><b>text</l></b>）
    // The string formatter supports a kind of "mini-html" that consists of various short tags:
    //
    //   Bold:             <b>text</b>
    //   Color:            <c #xxxxxx>text</c>  where x = RGB in hex
    //   Font:             <f x>text</f>        where x = font id
    //   Italic:           <i>text</i>
    //   Image:            <i x y z>            where x = image name and y = imagelist num and z(optional) = imagelist id
    //   Link:             <a x>text</a>        where x(optional) = link content, normal like app:notepad or http:www.xxx.com
    //   NewLine           <n>
    //   Paragraph:        <p x>text</p>        where x = extra pixels indent in p
    //   Raw Text:         <r>text</r>
    //   Selected:         <s>text</s>
    //   Underline:        <u>text</u>
    //   X Indent:         <x i>                where i = hor indent in pixels
    //   Y Indent:         <y i>                where i = ver indent in pixels
    //   Vertical align    <v x>                where x = top or x = center or x = bottom

    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

    if (pstrText == NULL || pManager == NULL) { return; }

    if (::IsRectEmpty(&rc)) { return; }

    int nLinkRects = (NULL != pnLinkRects) ? *pnLinkRects : 0;
    bool bDraw = (uStyle & DT_CALCRECT) == 0;

    CDuiPtrArray aFontArray(10);
    CDuiPtrArray aColorArray(10);
    CDuiPtrArray aPIndentArray(10);
    CDuiPtrArray aVAlignArray(10);

    RECT rcClip = { 0 };
    ::GetClipBox(hDC, &rcClip);
    HRGN hOldRgn = ::CreateRectRgnIndirect(&rcClip);
    HRGN hRgn = ::CreateRectRgnIndirect(&rc);

    if (bDraw) { ::ExtSelectClipRgn(hDC, hRgn, RGN_AND); }

    TEXTMETRIC *pTm = &pManager->GetFontInfo(iDefaultFont)->tm;
    HFONT hOldFont = (HFONT) ::SelectObject(hDC, pManager->GetFontInfo(iDefaultFont)->hFont);
    ::SetBkMode(hDC, TRANSPARENT);
    ::SetTextColor(hDC, RGB(GetBValue(dwTextColor), GetGValue(dwTextColor), GetRValue(dwTextColor)));
    DWORD dwBkColor = pManager->GetDefaultSelectedBkColor();
    ::SetBkColor(hDC, RGB(GetBValue(dwBkColor), GetGValue(dwBkColor), GetRValue(dwBkColor)));

    // If the drawstyle include a alignment, we'll need to first determine the text-size so
    // we can draw it at the correct position...
    if (((uStyle & DT_CENTER) != 0 || (uStyle & DT_RIGHT) != 0 || (uStyle & DT_VCENTER) != 0 ||
         (uStyle & DT_BOTTOM) != 0) && (uStyle & DT_CALCRECT) == 0)
    {
        RECT rcText = { 0, 0, MAX_CTRL_WIDTH, 100 };

        if ((uStyle & DT_SINGLELINE) == 0)
        {
            rcText.right = rc.right - rc.left;
            rcText.bottom = rc.bottom - rc.top;
        }

        DrawHtmlText(hDC, pManager, rcText, pstrText, dwTextColor, NULL, NULL, NULL, iDefaultFont,
                     uStyle | DT_CALCRECT & ~DT_CENTER & ~DT_RIGHT & ~DT_VCENTER & ~DT_BOTTOM);

        if ((uStyle & DT_SINGLELINE) != 0)
        {
            if ((uStyle & DT_CENTER) != 0)
            {
                rc.left = rc.left + ((rc.right - rc.left) / 2) - ((rcText.right - rcText.left) / 2);
                rc.right = rc.left + (rcText.right - rcText.left);
            }

            if ((uStyle & DT_RIGHT) != 0)
            {
                rc.left = rc.right - (rcText.right - rcText.left);
            }
        }

        if ((uStyle & DT_VCENTER) != 0)
        {
            rc.top = rc.top + ((rc.bottom - rc.top) / 2) - ((rcText.bottom - rcText.top) / 2);
            rc.bottom = rc.top + (rcText.bottom - rcText.top);
        }

        if ((uStyle & DT_BOTTOM) != 0)
        {
            rc.top = rc.bottom - (rcText.bottom - rcText.top);
        }
    }

    bool bHoverLink = false;
    CDuiString sHoverLink;
    POINT ptMouse = pManager->GetMousePos();

    for (int i = 0; !bHoverLink && i < nLinkRects; i++)
    {
        if (::PtInRect(prcLinks + i, ptMouse))
        {
            sHoverLink = *(CDuiString *)(sLinks + i);
            bHoverLink = true;
        }
    }

    POINT pt = { rc.left, rc.top };
    int iLinkIndex = 0;
    int cxLine = 0;
    int cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);
    int cyMinHeight = 0;
    int cxMaxWidth = 0;
    POINT ptLinkStart = { 0 };
    bool bLineEnd = false;
    bool bInRaw = false;
    bool bInLink = false;
    bool bInSelected = false;
    int iLineLinkIndex = 0;

    // 排版习惯是图文底部对齐，所以每行绘制都要分两步，先计算高度，再绘制
    CDuiPtrArray aLineFontArray;
    CDuiPtrArray aLineColorArray;
    CDuiPtrArray aLinePIndentArray;
    CDuiPtrArray aLineVAlignArray;
    LPCTSTR pstrLineBegin = pstrText;
    bool bLineInRaw = false;
    bool bLineInLink = false;
    bool bLineInSelected = false;
    UINT iVAlign = DT_BOTTOM;
    int cxLineWidth = 0;
    int cyLineHeight = 0;
    int cxOffset = 0;
    bool bLineDraw = false; // 行的第二阶段：绘制

    while (*pstrText != _T('\0'))
    {
        if (pt.x >= rc.right || *pstrText == _T('\n') || bLineEnd)
        {
            if (*pstrText == _T('\n')) { pstrText++; }

            if (bLineEnd) { bLineEnd = false; }

            if (!bLineDraw)
            {
                if (bInLink && iLinkIndex < nLinkRects)
                {
                    ::SetRect(&prcLinks[iLinkIndex++], ptLinkStart.x, ptLinkStart.y,
                              std::min<long>(pt.x, rc.right), pt.y + cyLine);
                    CDuiString *pStr1 = (CDuiString *)(sLinks + iLinkIndex - 1);
                    CDuiString *pStr2 = (CDuiString *)(sLinks + iLinkIndex);
                    *pStr2 = *pStr1;
                }

                for (int i = iLineLinkIndex; i < iLinkIndex; i++)
                {
                    prcLinks[i].bottom = pt.y + cyLine;
                }

                if (bDraw)
                {
                    bInLink = bLineInLink;
                    iLinkIndex = iLineLinkIndex;
                }
            }
            else
            {
                if (bInLink && iLinkIndex < nLinkRects) { iLinkIndex++; }

                bLineInLink = bInLink;
                iLineLinkIndex = iLinkIndex;
            }

            if ((uStyle & DT_SINGLELINE) != 0 && (!bDraw || bLineDraw)) { break; }

            if (bDraw) { bLineDraw = !bLineDraw; }  // !

            pt.x = rc.left;
            cxOffset = 0;

            if (bLineDraw)
            {
                if ((uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_CENTER) != 0)
                {
                    cxOffset = (rc.right - rc.left - cxLineWidth) / 2;
                }
                else if ((uStyle & DT_SINGLELINE) == 0 && (uStyle & DT_RIGHT) != 0)
                {
                    cxOffset = rc.right - rc.left - cxLineWidth;
                }
            }
            else
            {
                pt.y += cyLine;
            }

            if (pt.y > rc.bottom && bDraw) { break; }

            ptLinkStart = pt;
            cyLine = pTm->tmHeight + pTm->tmExternalLeading + (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1);

            if (pt.x >= rc.right) { break; }
        }
        else if (!bInRaw && (*pstrText == _T('<') || *pstrText == _T('{'))
                 && (pstrText[1] >= _T('a') && pstrText[1] <= _T('z'))
                 && (pstrText[2] == _T(' ') || pstrText[2] == _T('>') || pstrText[2] == _T('}')))
        {
            pstrText++;
            LPCTSTR pstrNextStart = NULL;

            switch (*pstrText)
            {
            case _T('a'):  // Link
                {
                    pstrText++;

                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                    if (iLinkIndex < nLinkRects && !bLineDraw)
                    {
                        CDuiString *pStr = (CDuiString *)(sLinks + iLinkIndex);
                        pStr->Empty();

                        while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
                        {
                            LPCTSTR pstrTemp = ::CharNext(pstrText);

                            while (pstrText < pstrTemp)
                            {
                                *pStr += *pstrText++;
                            }
                        }
                    }

                    DWORD clrColor = pManager->GetDefaultLinkFontColor();

                    if (bHoverLink && iLinkIndex < nLinkRects)
                    {
                        CDuiString *pStr = (CDuiString *)(sLinks + iLinkIndex);

                        if (sHoverLink == *pStr) { clrColor = pManager->GetDefaultLinkHoverFontColor(); }
                    }

                    //else if( prcLinks == NULL ) {
                    //    if( ::PtInRect(&rc, ptMouse) )
                    //        clrColor = pManager->GetDefaultLinkHoverFontColor();
                    //}
                    aColorArray.Add((LPVOID)clrColor);
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                    TFontInfo *pFontInfo = pManager->GetFontInfo(iDefaultFont);

                    if (aFontArray.GetSize() > 0)
                    {
                        pFontInfo = (TFontInfo *)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    }

                    if (pFontInfo->bUnderline == false)
                    {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize,
                                                        pFontInfo->bBold, true, pFontInfo->bItalic);

                        if (hFont == NULL)
                        {
                            hFont = pManager->AddFont(g_iFontID, pFontInfo->sFontName, pFontInfo->iSize,
                                                      pFontInfo->bBold, true, pFontInfo->bItalic);
                            g_iFontID += 1;
                        }

                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = std::max<int>(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                                               (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }

                    ptLinkStart = pt;
                    bInLink = true;
                }
                break;

            case _T('b'):  // Bold
                {
                    pstrText++;
                    TFontInfo *pFontInfo = pManager->GetFontInfo(iDefaultFont);

                    if (aFontArray.GetSize() > 0)
                    {
                        pFontInfo = (TFontInfo *)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    }

                    if (pFontInfo->bBold == false)
                    {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize,
                                                        true, pFontInfo->bUnderline, pFontInfo->bItalic);

                        if (hFont == NULL)
                        {
                            hFont = pManager->AddFont(g_iFontID, pFontInfo->sFontName, pFontInfo->iSize,
                                                      true, pFontInfo->bUnderline, pFontInfo->bItalic);
                            g_iFontID += 1;
                        }

                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = std::max<int>(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                                               (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }
                }
                break;

            case _T('c'):  // Color
                {
                    pstrText++;

                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                    if (*pstrText == _T('#')) { pstrText++; }

                    DWORD clrColor = _tcstol(pstrText, const_cast<LPTSTR *>(&pstrText), 16);
                    aColorArray.Add((LPVOID)clrColor);
                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                }
                break;

            case _T('f'):  // Font
                {
                    pstrText++;

                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                    LPCTSTR pstrTemp = pstrText;
                    int iFont = (int)_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText), 10);

                    //if( isdigit(*pstrText) ) { // debug版本会引起异常
                    if (pstrTemp != pstrText)
                    {
                        TFontInfo *pFontInfo = pManager->GetFontInfo(iFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                    }
                    else
                    {
                        CDuiString sFontName;
                        int iFontSize = 10;
                        CDuiString sFontAttr;
                        bool bBold = false;
                        bool bUnderline = false;
                        bool bItalic = false;

                        while (*pstrText != _T('\0') && *pstrText != _T('>') &&
                               *pstrText != _T('}') && *pstrText != _T(' '))
                        {
                            pstrTemp = ::CharNext(pstrText);

                            while (pstrText < pstrTemp)
                            {
                                sFontName += *pstrText++;
                            }
                        }

                        while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                        if (isdigit(*pstrText))
                        {
                            iFontSize = (int)_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText), 10);
                        }

                        while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                        while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
                        {
                            pstrTemp = ::CharNext(pstrText);

                            while (pstrText < pstrTemp)
                            {
                                sFontAttr += *pstrText++;
                            }
                        }

                        sFontAttr.MakeLower();

                        if (sFontAttr.Find(_T("bold")) >= 0) { bBold = true; }

                        if (sFontAttr.Find(_T("underline")) >= 0) { bUnderline = true; }

                        if (sFontAttr.Find(_T("italic")) >= 0) { bItalic = true; }

                        HFONT hFont = pManager->GetFont(sFontName, iFontSize, bBold, bUnderline, bItalic);

                        if (hFont == NULL)
                        {
                            hFont = pManager->AddFont(g_iFontID, sFontName, iFontSize, bBold, bUnderline, bItalic);
                            g_iFontID += 1;
                        }

                        TFontInfo *pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                    }

                    cyLine = std::max<int>(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                                           (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                }
                break;

            case _T('i'):  // Italic or Image
                {
                    pstrNextStart = pstrText - 1;
                    pstrText++;
                    CDuiString sImageString = pstrText;
                    int iWidth = 0;
                    int iHeight = 0;

                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                    const TImageInfo *pImageInfo = NULL;
                    CDuiString sName;

                    while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}') && *pstrText != _T(' '))
                    {
                        LPCTSTR pstrTemp = ::CharNext(pstrText);

                        while (pstrText < pstrTemp)
                        {
                            sName += *pstrText++;
                        }
                    }

                    if (sName.IsEmpty())    // Italic
                    {
                        pstrNextStart = NULL;
                        TFontInfo *pFontInfo = pManager->GetFontInfo(iDefaultFont);

                        if (aFontArray.GetSize() > 0)
                        {
                            pFontInfo = (TFontInfo *)aFontArray.GetAt(aFontArray.GetSize() - 1);
                        }

                        if (pFontInfo->bItalic == false)
                        {
                            HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize,
                                                            pFontInfo->bBold, pFontInfo->bUnderline, true);

                            if (hFont == NULL)
                            {
                                hFont = pManager->AddFont(g_iFontID, pFontInfo->sFontName, pFontInfo->iSize,
                                                          pFontInfo->bBold, pFontInfo->bUnderline, true);
                                g_iFontID += 1;
                            }

                            pFontInfo = pManager->GetFontInfo(hFont);
                            aFontArray.Add(pFontInfo);
                            pTm = &pFontInfo->tm;
                            ::SelectObject(hDC, pFontInfo->hFont);
                            cyLine = std::max<int>(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                                                   (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                        }
                    }
                    else
                    {
                        while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                        int iImageListNum = (int)_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText), 10);

                        if (iImageListNum <= 0) { iImageListNum = 1; }

                        while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                        int iImageListIndex = (int)_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText), 10);

                        if (iImageListIndex < 0 || iImageListIndex >= iImageListNum) { iImageListIndex = 0; }

                        if (_tcsstr(sImageString.GetData(), _T("file=\'")) != NULL ||
                            _tcsstr(sImageString.GetData(), _T("res=\'")) != NULL)
                        {
                            CDuiString sImageResType;
                            CDuiString sImageName;
                            LPCTSTR pStrImage = sImageString.GetData();
                            CDuiString sItem;
                            CDuiString sValue;

                            while (*pStrImage != _T('\0'))
                            {
                                sItem.Empty();
                                sValue.Empty();

                                while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) { pStrImage = ::CharNext(pStrImage); }

                                while (*pStrImage != _T('\0') && *pStrImage != _T('=') && *pStrImage > _T(' '))
                                {
                                    LPTSTR pstrTemp = ::CharNext(pStrImage);

                                    while (pStrImage < pstrTemp)
                                    {
                                        sItem += *pStrImage++;
                                    }
                                }

                                while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) { pStrImage = ::CharNext(pStrImage); }

                                if (*pStrImage++ != _T('=')) { break; }

                                while (*pStrImage > _T('\0') && *pStrImage <= _T(' ')) { pStrImage = ::CharNext(pStrImage); }

                                if (*pStrImage++ != _T('\'')) { break; }

                                while (*pStrImage != _T('\0') && *pStrImage != _T('\''))
                                {
                                    LPTSTR pstrTemp = ::CharNext(pStrImage);

                                    while (pStrImage < pstrTemp)
                                    {
                                        sValue += *pStrImage++;
                                    }
                                }

                                if (*pStrImage++ != _T('\'')) { break; }

                                if (!sValue.IsEmpty())
                                {
                                    if (sItem == _T("file") || sItem == _T("res"))
                                    {
                                        sImageName = sValue;
                                    }
                                    else if (sItem == _T("restype"))
                                    {
                                        sImageResType = sValue;
                                    }
                                }

                                if (*pStrImage++ != _T(' ')) { break; }
                            }

                            pImageInfo = pManager->GetImageEx((LPCTSTR)sImageName, sImageResType);
                        }
                        else
                        { pImageInfo = pManager->GetImageEx((LPCTSTR)sName); }

                        if (pImageInfo)
                        {
                            iWidth = pImageInfo->nX;
                            iHeight = pImageInfo->nY;

                            if (iImageListNum > 1) { iWidth /= iImageListNum; }

                            if (pt.x + iWidth > rc.right && pt.x > rc.left && (uStyle & DT_SINGLELINE) == 0)
                            {
                                bLineEnd = true;
                                cxLine = pt.x - rc.left;
                            }
                            else
                            {
                                pstrNextStart = NULL;

                                if (bDraw && bLineDraw)
                                {
                                    CDuiRect rcImage(pt.x + cxOffset, pt.y + cyLineHeight - iHeight,
                                                     pt.x + cxOffset + iWidth, pt.y + cyLineHeight);
                                    iVAlign = DT_BOTTOM;

                                    if (aVAlignArray.GetSize() > 0)
                                    {
                                        iVAlign = (UINT)aVAlignArray.GetAt(aVAlignArray.GetSize() - 1);
                                    }

                                    if (iVAlign == DT_VCENTER)
                                    {
                                        if (iHeight < cyLineHeight)
                                        {
                                            rcImage.bottom -= (cyLineHeight - iHeight) / 2;
                                            rcImage.top = rcImage.bottom - iHeight;
                                        }
                                    }
                                    else if (iVAlign == DT_TOP)
                                    {
                                        if (iHeight < cyLineHeight)
                                        {
                                            rcImage.bottom = pt.y + iHeight;
                                            rcImage.top = pt.y;
                                        }
                                    }

                                    CDuiRect rcBmpPart(0, 0, iWidth, iHeight);
                                    rcBmpPart.left = iWidth * iImageListIndex;
                                    rcBmpPart.right = iWidth * (iImageListIndex + 1);
                                    CDuiRect rcCorner(0, 0, 0, 0);
                                    DrawImage(hDC, pImageInfo->hBitmap, rcImage, rcImage, rcBmpPart, rcCorner,
                                              pImageInfo->bAlpha, 255);
                                }

                                cyLine = std::max<int>(iHeight, cyLine);
                                pt.x += iWidth;
                                cxMaxWidth = std::max<int>(cxMaxWidth, pt.x);
                                cxLine = pt.x - rc.left;
                                cyMinHeight = pt.y + iHeight;
                            }
                        }
                        else { pstrNextStart = NULL; }
                    }
                }
                break;

            case _T('n'):  // Newline
                {
                    pstrText++;

                    if ((uStyle & DT_SINGLELINE) != 0) { break; }

                    bLineEnd = true;
                }
                break;

            case _T('p'):  // Paragraph
                {
                    pstrText++;

                    if (pt.x > rc.left) { bLineEnd = true; }

                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                    int cyLineExtra = (int)_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText), 10);
                    aPIndentArray.Add((LPVOID)cyLineExtra);
                    cyLine = std::max<int>(cyLine, pTm->tmHeight + pTm->tmExternalLeading + cyLineExtra);
                }
                break;

            case _T('v'):  // Vertical Align
                {
                    pstrText++;

                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                    CDuiString sVAlignStyle;

                    while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
                    {
                        LPCTSTR pstrTemp = ::CharNext(pstrText);

                        while (pstrText < pstrTemp)
                        {
                            sVAlignStyle += *pstrText++;
                        }
                    }

                    UINT iVAlign = DT_BOTTOM;

                    if (sVAlignStyle.CompareNoCase(_T("center")) == 0) { iVAlign = DT_VCENTER; }
                    else if (sVAlignStyle.CompareNoCase(_T("top")) == 0) { iVAlign = DT_TOP; }

                    aVAlignArray.Add((LPVOID)iVAlign);
                }
                break;

            case _T('r'):  // Raw Text
                {
                    pstrText++;
                    bInRaw = true;
                }
                break;

            case _T('s'):  // Selected text background color
                {
                    pstrText++;
                    bInSelected = !bInSelected;

                    if (bDraw && bLineDraw)
                    {
                        if (bInSelected) { ::SetBkMode(hDC, OPAQUE); }
                        else             { ::SetBkMode(hDC, TRANSPARENT); }
                    }
                }
                break;

            case _T('u'):  // Underline text
                {
                    pstrText++;
                    TFontInfo *pFontInfo = pManager->GetFontInfo(iDefaultFont);

                    if (aFontArray.GetSize() > 0)
                    {
                        pFontInfo = (TFontInfo *)aFontArray.GetAt(aFontArray.GetSize() - 1);
                    }

                    if (pFontInfo->bUnderline == false)
                    {
                        HFONT hFont = pManager->GetFont(pFontInfo->sFontName, pFontInfo->iSize,
                                                        pFontInfo->bBold, true, pFontInfo->bItalic);

                        if (hFont == NULL)
                        {
                            hFont = pManager->AddFont(g_iFontID, pFontInfo->sFontName, pFontInfo->iSize,
                                                      pFontInfo->bBold, true, pFontInfo->bItalic);
                            g_iFontID += 1;
                        }

                        pFontInfo = pManager->GetFontInfo(hFont);
                        aFontArray.Add(pFontInfo);
                        pTm = &pFontInfo->tm;
                        ::SelectObject(hDC, pFontInfo->hFont);
                        cyLine = std::max<int>(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                                               (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                    }
                }
                break;

            case _T('x'):  // X Indent
                {
                    pstrText++;

                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                    int iWidth = (int)_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText), 10);
                    pt.x += iWidth;
                }
                break;

            case _T('y'):  // Y Indent
                {
                    pstrText++;

                    while (*pstrText > _T('\0') && *pstrText <= _T(' ')) { pstrText = ::CharNext(pstrText); }

                    cyLine = (int)_tcstol(pstrText, const_cast<LPTSTR *>(&pstrText), 10);
                }
                break;
            }

            if (pstrNextStart != NULL) { pstrText = pstrNextStart; }
            else
            {
                while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
                { pstrText = ::CharNext(pstrText); }

                pstrText = ::CharNext(pstrText);
            }
        }
        else if (!bInRaw && (*pstrText == _T('<') || *pstrText == _T('{')) && pstrText[1] == _T('/'))
        {
            pstrText++;
            pstrText++;

            switch (*pstrText)
            {
            case _T('c'):
                {
                    pstrText++;
                    aColorArray.Remove(aColorArray.GetSize() - 1);
                    DWORD clrColor = dwTextColor;

                    if (aColorArray.GetSize() > 0)
                    {
                        clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
                    }

                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                }
                break;

            case _T('p'):
                pstrText++;

                if (pt.x > rc.left) { bLineEnd = true; }

                aPIndentArray.Remove(aPIndentArray.GetSize() - 1);
                cyLine = std::max<int>(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                                       (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                break;

            case _T('v'):
                pstrText++;
                aVAlignArray.Remove(aVAlignArray.GetSize() - 1);
                break;

            case _T('s'):
                {
                    pstrText++;
                    bInSelected = !bInSelected;

                    if (bDraw && bLineDraw)
                    {
                        if (bInSelected) { ::SetBkMode(hDC, OPAQUE); }
                        else             { ::SetBkMode(hDC, TRANSPARENT); }
                    }
                }
                break;

            case _T('a'):
                {
                    if (iLinkIndex < nLinkRects)
                    {
                        if (!bLineDraw)
                        {
                            ::SetRect(&prcLinks[iLinkIndex], ptLinkStart.x, ptLinkStart.y,
                                      std::min<long>(pt.x, rc.right),
                                      pt.y + pTm->tmHeight + pTm->tmExternalLeading);
                        }

                        iLinkIndex++;
                    }

                    aColorArray.Remove(aColorArray.GetSize() - 1);
                    DWORD clrColor = dwTextColor;

                    if (aColorArray.GetSize() > 0)
                    {
                        clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1);
                    }

                    ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                    bInLink = false;
                }

            case _T('b'):
            case _T('f'):
            case _T('i'):
            case _T('u'):
                {
                    pstrText++;
                    aFontArray.Remove(aFontArray.GetSize() - 1);
                    TFontInfo *pFontInfo = (TFontInfo *)aFontArray.GetAt(aFontArray.GetSize() - 1);

                    if (pFontInfo == NULL) { pFontInfo = pManager->GetFontInfo(iDefaultFont); }

                    if (pTm->tmItalic && pFontInfo->bItalic == false)
                    {
                        ABC abc;
                        ::GetCharABCWidths(hDC, _T(' '), _T(' '), &abc);
                        // 简单修正一下斜体混排的问题, 正确做法应该是http://support.microsoft.com/kb/244798/en-us
                        pt.x += abc.abcC / 2;
                    }

                    pTm = &pFontInfo->tm;
                    ::SelectObject(hDC, pFontInfo->hFont);
                    cyLine = std::max<int>(cyLine, pTm->tmHeight + pTm->tmExternalLeading +
                                           (int)aPIndentArray.GetAt(aPIndentArray.GetSize() - 1));
                }
                break;
            }

            while (*pstrText != _T('\0') && *pstrText != _T('>') && *pstrText != _T('}'))
            { pstrText = ::CharNext(pstrText); }

            pstrText = ::CharNext(pstrText);
        }
        else if (!bInRaw && *pstrText == _T('<') && pstrText[2] == _T('>') &&
                 (pstrText[1] == _T('{') || pstrText[1] == _T('}')))
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);

            if (bDraw && bLineDraw)
            {
                iVAlign = DT_BOTTOM;
                // RECT rcTxt = { pt.x + cxOffset, pt.y, pt.x + cxOffset + cxLineWidth, pt.y + cyLineHeight };
                // CHDCHelper dcHelper(hDC, &rcTxt, dwTextColor >> 24);
                Graphics g(hDC);
                Font f(hDC);
                COLORREF clrText = ::GetTextColor(hDC);
                SolidBrush brush(Color(GetRValue(clrText), GetGValue(clrText), GetBValue(clrText)));
                PointF ptF(REAL(pt.x + cxOffset), 0.0);

                if (aVAlignArray.GetSize() > 0) { iVAlign = (UINT)aVAlignArray.GetAt(aVAlignArray.GetSize() - 1); }

                if (iVAlign == DT_VCENTER)
                {
                    ptF.Y = REAL(pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2);
                    // ::TextOut(dcHelper, pt.x + cxOffset,
                    //           pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2,
                    //           &pstrText[1], 1);
                }
                else if (iVAlign == DT_TOP)
                {
                    ptF.Y = REAL(pt.y);
                    //::TextOut(dcHelper, pt.x + cxOffset, pt.y, &pstrText[1], 1);
                }
                else
                {
                    ptF.Y = REAL(pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading);
                    //::TextOut(dcHelper, pt.x + cxOffset,
                    //          pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading,
                    //          &pstrText[1], 1);
                }

#if !defined(UNICODE) || !defined(_UNICODE)
                LPWSTR pwstrText = NULL;
                int iLen = 1;
                pwstrText = new WCHAR[iLen + 1];
                ::ZeroMemory(pwstrText, (iLen + 1) * sizeof(WCHAR));
                ::MultiByteToWideChar(CP_ACP, 0, pstrText, iLen, (LPWSTR)pwstrText, iLen);
                g.DrawString(pwstrText, 1, &f, ptF, &brush);
#else
                g.DrawString(&pstrText[1], 1, &f, ptF, &brush);
#endif
            }

            pt.x += szSpace.cx;
            cxMaxWidth = std::max<int>(cxMaxWidth, pt.x);
            cxLine = pt.x - rc.left;
            pstrText++; pstrText++; pstrText++;
        }
        else if (!bInRaw &&  *pstrText == _T('{') && pstrText[2] == _T('}') &&
                 (pstrText[1] == _T('<')  || pstrText[1] == _T('>')))
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, &pstrText[1], 1, &szSpace);

            if (bDraw && bLineDraw)
            {
                iVAlign = DT_BOTTOM;
                // RECT rcTxt = { pt.x + cxOffset, pt.y, pt.x + cxOffset + cxLineWidth, pt.y + cyLineHeight };
                // CHDCHelper dcHelper(hDC, &rcTxt, dwTextColor >> 24);
                Graphics g(hDC);
                Font f(hDC);
                COLORREF clrText = ::GetTextColor(hDC);
                SolidBrush brush(Color(GetRValue(clrText), GetGValue(clrText), GetBValue(clrText)));
                PointF ptF(REAL(pt.x + cxOffset), 0.0);

                if (aVAlignArray.GetSize() > 0) { iVAlign = (UINT)aVAlignArray.GetAt(aVAlignArray.GetSize() - 1); }

                if (iVAlign == DT_VCENTER)
                {
                    ptF.Y = REAL(pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2);
                    // ::TextOut(dcHelper, pt.x + cxOffset,
                    //           pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2,
                    //           &pstrText[1], 1);
                }
                else if (iVAlign == DT_TOP)
                {
                    ptF.Y = REAL(pt.y);
                    // ::TextOut(dcHelper, pt.x + cxOffset, pt.y, &pstrText[1], 1);
                }
                else
                {
                    ptF.Y = REAL(pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading);
                    // ::TextOut(dcHelper, pt.x + cxOffset,
                    //          pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading,
                    //          &pstrText[1], 1);
                }

#if !defined(UNICODE) || !defined(_UNICODE)
                LPWSTR pwstrText = NULL;
                int iLen = 1;
                pwstrText = new WCHAR[iLen + 1];
                ::ZeroMemory(pwstrText, (iLen + 1) * sizeof(WCHAR));
                ::MultiByteToWideChar(CP_ACP, 0, pstrText, iLen, (LPWSTR)pwstrText, iLen);
                g.DrawString(pwstrText, 1, &f, ptF, &brush);
#else
                g.DrawString(&pstrText[1], 1, &f, ptF, &brush);
#endif
            }

            pt.x += szSpace.cx;
            cxMaxWidth = std::max<int>(cxMaxWidth, pt.x);
            cxLine = pt.x - rc.left;
            pstrText++; pstrText++; pstrText++;
        }
        else if (!bInRaw &&  *pstrText == _T(' '))
        {
            SIZE szSpace = { 0 };
            ::GetTextExtentPoint32(hDC, _T(" "), 1, &szSpace);

            // Still need to paint the space because the font might have
            // underline formatting.
            if (bDraw && bLineDraw)
            {
                iVAlign = DT_BOTTOM;
                // RECT rcTxt = { pt.x + cxOffset, pt.y, pt.x + cxOffset + cxLineWidth, pt.y + cyLineHeight };
                // CHDCHelper dcHelper(hDC, &rcTxt, dwTextColor >> 24);
                Graphics g(hDC);
                Font f(hDC);
                COLORREF clrText = ::GetTextColor(hDC);
                SolidBrush brush(Color(GetRValue(clrText), GetGValue(clrText), GetGValue(clrText)));
                PointF ptF(REAL(pt.x + cxOffset), 0.0);

                if (aVAlignArray.GetSize() > 0) { iVAlign = (UINT)aVAlignArray.GetAt(aVAlignArray.GetSize() - 1); }

                if (iVAlign == DT_VCENTER)
                {
                    ptF.Y = REAL(pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2);
                    // ::TextOut(dcHelper, pt.x + cxOffset,
                    //           pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2,
                    //           _T(" "), 1);
                }
                else if (iVAlign == DT_TOP)
                {
                    ptF.Y = REAL(pt.y);
                    // ::TextOut(dcHelper, pt.x + cxOffset, pt.y, _T(" "), 1);
                }
                else
                {
                    ptF.Y = REAL(pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading);
                    // ::TextOut(dcHelper, pt.x + cxOffset,
                    //           pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading,
                    //           _T(" "), 1);
                }

#if !defined(UNICODE) || !defined(_UNICODE)
                LPWSTR pwstrText = NULL;
                int iLen = 1;
                pwstrText = new WCHAR[iLen + 1];
                ::ZeroMemory(pwstrText, (iLen + 1) * sizeof(WCHAR));
                ::MultiByteToWideChar(CP_ACP, 0, pstrText, iLen, (LPWSTR)pwstrText, iLen);
                g.DrawString(pwstrText, 1, &f, ptF, &brush);
#else
                g.DrawString(&pstrText[1], 1, &f, ptF, &brush);
#endif
            }

            pt.x += szSpace.cx;
            cxMaxWidth = std::max<int>(cxMaxWidth, pt.x);
            cxLine = pt.x - rc.left;
            pstrText++;
        }
        else
        {
            int cchChars = 0;
            int cchSize = 0;
            int cchLastGoodWord = 0;
            int cchLastGoodSize = 0;
            LPCTSTR p = pstrText;
            LPCTSTR pstrNext;
            SIZE szText = { 0 };

            if (!bInRaw && *p == _T('<') || *p == _T('{')) { p++, cchChars++, cchSize++; }

            while (*p != _T('\0') && *p != _T('\n'))
            {
                // This part makes sure that we're word-wrapping if needed or providing support
                // for DT_END_ELLIPSIS. Unfortunately the GetTextExtentPoint32() call is pretty
                // slow when repeated so often.
                // TODO: Rewrite and use GetTextExtentExPoint() instead!
                if (bInRaw)
                {
                    if ((*p == _T('<') || *p == _T('{')) && p[1] == _T('/')
                        && p[2] == _T('r') && (p[3] == _T('>') || p[3] == _T('}')))
                    {
                        p += 4;
                        bInRaw = false;
                        break;
                    }
                }
                else
                {
                    if (*p == _T('<') || *p == _T('{')) { break; }
                }

                pstrNext = ::CharNext(p);
                cchChars++;
                cchSize += (int)(pstrNext - p);
                szText.cx = cchChars * pTm->tmMaxCharWidth;

                if (pt.x + szText.cx >= rc.right)
                {
                    ::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);
                }

                if (pt.x + szText.cx > rc.right)
                {
                    if (pt.x + szText.cx > rc.right && cchChars > 1)
                    {
                        cchChars--;
                        cchSize -= (int)(pstrNext - p);
                    }

                    if ((uStyle & DT_WORDBREAK) != 0 && cchLastGoodWord > 0)
                    {
                        cchChars = cchLastGoodWord;
                        cchSize = cchLastGoodSize;
                    }

                    if ((uStyle & DT_END_ELLIPSIS) != 0 && cchChars > 0)
                    {
                        cchChars -= 1;
                        LPCTSTR pstrPrev = ::CharPrev(pstrText, p);

                        if (cchChars > 0)
                        {
                            cchChars -= 1;
                            pstrPrev = ::CharPrev(pstrText, pstrPrev);
                            cchSize -= (int)(p - pstrPrev);
                        }
                        else
                        { cchSize -= (int)(p - pstrPrev); }

                        pt.x = rc.right;
                    }

                    bLineEnd = true;
                    cxMaxWidth = std::max<int>(cxMaxWidth, pt.x);
                    cxLine = pt.x - rc.left;
                    break;
                }

                if (!((p[0] >= _T('a') && p[0] <= _T('z')) || (p[0] >= _T('A') && p[0] <= _T('Z'))))
                {
                    cchLastGoodWord = cchChars;
                    cchLastGoodSize = cchSize;
                }

                if (*p == _T(' '))
                {
                    cchLastGoodWord = cchChars;
                    cchLastGoodSize = cchSize;
                }

                p = ::CharNext(p);
            }

            ::GetTextExtentPoint32(hDC, pstrText, cchSize, &szText);

            if (bDraw && bLineDraw)
            {
                iVAlign = DT_BOTTOM;
                // RECT rcTxt = { pt.x + cxOffset, pt.y, rc.right, pt.y + cyLineHeight };
                // CHDCHelper dcHelper(hDC, &rcTxt, dwTextColor >> 24);
                Graphics g(hDC);
                Font f(hDC);
                COLORREF clrText = ::GetTextColor(hDC);
                SolidBrush brush(Color(GetRValue(clrText), GetGValue(clrText), GetBValue(clrText)));
                PointF ptF(REAL(pt.x + cxOffset), 0.0);

                if (aVAlignArray.GetSize() > 0) { iVAlign = (UINT)aVAlignArray.GetAt(aVAlignArray.GetSize() - 1); }

                if (iVAlign == DT_VCENTER)
                {
                    ptF.Y = REAL(pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2);
                    // ::TextOut(dcHelper, rcTxt.left,
                    //           pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2,
                    //           pstrText, cchSize);
                }
                else if (iVAlign == DT_TOP)
                {
                    ptF.Y = REAL(pt.y);
                    // ::TextOut(dcHelper, rcTxt.left, pt.y, pstrText, cchSize);
                }
                else
                {
                    ptF.Y = REAL(pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading);
                    // ::TextOut(dcHelper, rcTxt.left,
                    //           pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading,
                    //           pstrText, cchSize);
                }

#if !defined(UNICODE) || !defined(_UNICODE)
                LPWSTR pwstrText = NULL;
                int iLen = cchSize;
                pwstrText = new WCHAR[iLen + 1];
                ::ZeroMemory(pwstrText, (iLen + 1) * sizeof(WCHAR));
                ::MultiByteToWideChar(CP_ACP, 0, pstrText, iLen, (LPWSTR)pwstrText, iLen);
                g.DrawString(pwstrText, cchSize, &f, ptF, &brush);
#else
                g.DrawString(pstrText, cchSize, &f, ptF, &brush);
#endif

                if (pt.x >= rc.right && (uStyle & DT_END_ELLIPSIS) != 0)
                {
                    // ptF.Y 不变。
                    ptF.X += REAL(szText.cx);
                    g.DrawString(L"...", 3, &f, ptF, &brush);
                    // if (iVAlign == DT_VCENTER)
                    // {
                    //     ::TextOut(dcHelper, rcTxt.left + szText.cx,
                    //               pt.y + (cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading) / 2,
                    //               _T("..."), 3);
                    // }
                    // else if (iVAlign == DT_TOP)
                    // {
                    //     ::TextOut(dcHelper, rcTxt.left + szText.cx, pt.y, _T("..."), 3);
                    // }
                    // else
                    // {
                    //     ::TextOut(dcHelper, rcTxt.left + szText.cx,
                    //               pt.y + cyLineHeight - pTm->tmHeight - pTm->tmExternalLeading,
                    //               _T("..."), 3);
                    // }
                }
            }

            pt.x += szText.cx;
            cxMaxWidth = std::max<int>(cxMaxWidth, pt.x);
            cxLine = pt.x - rc.left;
            pstrText += cchSize;
        }

        if (pt.x >= rc.right || *pstrText == _T('\n') || *pstrText == _T('\0')) { bLineEnd = true; }

        if (bDraw && bLineEnd)
        {
            if (!bLineDraw)
            {
                aFontArray.Resize(aLineFontArray.GetSize());
                ::CopyMemory(aFontArray.GetData(), aLineFontArray.GetData(),
                             aLineFontArray.GetSize() * sizeof(LPVOID));
                aColorArray.Resize(aLineColorArray.GetSize());
                ::CopyMemory(aColorArray.GetData(), aLineColorArray.GetData(),
                             aLineColorArray.GetSize() * sizeof(LPVOID));
                aPIndentArray.Resize(aLinePIndentArray.GetSize());
                ::CopyMemory(aPIndentArray.GetData(), aLinePIndentArray.GetData(),
                             aLinePIndentArray.GetSize() * sizeof(LPVOID));
                aVAlignArray.Resize(aLineVAlignArray.GetSize());
                ::CopyMemory(aVAlignArray.GetData(), aLineVAlignArray.GetData(),
                             aLineVAlignArray.GetSize() * sizeof(LPVOID));

                cxLineWidth = cxLine;
                cyLineHeight = cyLine;
                pstrText = pstrLineBegin;
                bInRaw = bLineInRaw;
                bInSelected = bLineInSelected;

                DWORD clrColor = dwTextColor;

                if (aColorArray.GetSize() > 0) { clrColor = (int)aColorArray.GetAt(aColorArray.GetSize() - 1); }

                ::SetTextColor(hDC, RGB(GetBValue(clrColor), GetGValue(clrColor), GetRValue(clrColor)));
                TFontInfo *pFontInfo = (TFontInfo *)aFontArray.GetAt(aFontArray.GetSize() - 1);

                if (pFontInfo == NULL) { pFontInfo = pManager->GetFontInfo(iDefaultFont); }

                pTm = &pFontInfo->tm;
                ::SelectObject(hDC, pFontInfo->hFont);

                if (bInSelected) { ::SetBkMode(hDC, OPAQUE); }
            }
            else
            {
                aLineFontArray.Resize(aFontArray.GetSize());
                ::CopyMemory(aLineFontArray.GetData(), aFontArray.GetData(),
                             aFontArray.GetSize() * sizeof(LPVOID));
                aLineColorArray.Resize(aColorArray.GetSize());
                ::CopyMemory(aLineColorArray.GetData(), aColorArray.GetData(),
                             aColorArray.GetSize() * sizeof(LPVOID));
                aLinePIndentArray.Resize(aPIndentArray.GetSize());
                ::CopyMemory(aLinePIndentArray.GetData(), aPIndentArray.GetData(),
                             aPIndentArray.GetSize() * sizeof(LPVOID));
                aLineVAlignArray.Resize(aVAlignArray.GetSize());
                ::CopyMemory(aLineVAlignArray.GetData(), aVAlignArray.GetData(),
                             aVAlignArray.GetSize() * sizeof(LPVOID));

                pstrLineBegin = pstrText;
                bLineInSelected = bInSelected;
                bLineInRaw = bInRaw;
            }
        }

        ASSERT(iLinkIndex <= nLinkRects);
    }

    nLinkRects = iLinkIndex;

    // Return size of text when requested
    if ((uStyle & DT_CALCRECT) != 0)
    {
        rc.bottom = std::max<int>(cyMinHeight, pt.y + cyLine);
        rc.right = std::min<int>(rc.right, cxMaxWidth);
    }

    if (bDraw) { ::SelectClipRgn(hDC, hOldRgn); }

    ::DeleteObject(hOldRgn);
    ::DeleteObject(hRgn);

    ::SelectObject(hDC, hOldFont);

    if (NULL != pnLinkRects) { *pnLinkRects = nLinkRects; }
}

HBITMAP CRenderEngine::GenerateBitmap(CPaintManagerUI *pManager, RECT rc, CControlUI *pStopControl,
                                      DWORD dwFilterColor)
{
    if (pManager == NULL) { return NULL; }

    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;

    bool bUseOffscreenBitmap = true;
    HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    ASSERT(hPaintDC);
    HBITMAP hPaintBitmap = NULL;

    if (pStopControl == NULL && !pManager->IsLayered()) { hPaintBitmap = pManager->GetPaintOffscreenBitmap(); }

    if (hPaintBitmap == NULL)
    {
        bUseOffscreenBitmap = false;
        hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
        ASSERT(hPaintBitmap);
    }

    HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);

    if (!bUseOffscreenBitmap)
    {
        CControlUI *pRoot = pManager->GetRoot();
        pRoot->Paint(hPaintDC, rc, pStopControl);
    }

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = cx;
    bmi.bmiHeader.biHeight = cy;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = cx * cy * sizeof(DWORD);
    LPDWORD pDest = NULL;
    HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    HBITMAP hBitmap = ::CreateDIBSection(pManager->GetPaintDC(), &bmi, DIB_RGB_COLORS,
                                         (LPVOID *) &pDest, NULL, 0);
    ASSERT(hCloneDC);
    ASSERT(hBitmap);

    if (hBitmap != NULL)
    {
        HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
        ::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
        RECT rcClone = {0, 0, cx, cy};

        if (dwFilterColor > 0x00FFFFFF) { DrawColor(hCloneDC, rcClone, dwFilterColor); }

        ::SelectObject(hCloneDC, hOldBitmap);
        ::DeleteDC(hCloneDC);
        ::GdiFlush();
    }

    // Cleanup
    ::SelectObject(hPaintDC, hOldPaintBitmap);

    if (!bUseOffscreenBitmap) { ::DeleteObject(hPaintBitmap); }

    ::DeleteDC(hPaintDC);

    return hBitmap;
}

HBITMAP CRenderEngine::GenerateBitmap(CPaintManagerUI *pManager, CControlUI *pControl, const RECT &rc,
                                      DWORD dwFilterColor)
{
    if (pManager == NULL || pControl == NULL) { return NULL; }

    int cx = rc.right - rc.left;
    int cy = rc.bottom - rc.top;

    HDC hPaintDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    HBITMAP hPaintBitmap = ::CreateCompatibleBitmap(pManager->GetPaintDC(), rc.right, rc.bottom);
    ASSERT(hPaintDC);
    ASSERT(hPaintBitmap);
    HBITMAP hOldPaintBitmap = (HBITMAP) ::SelectObject(hPaintDC, hPaintBitmap);
    pControl->Paint(hPaintDC, rc, NULL);

    LPDWORD pDest = NULL;
    HDC hCloneDC = ::CreateCompatibleDC(pManager->GetPaintDC());
    HBITMAP hBitmap = CRenderEngine::CreateARGB32Bitmap(pManager->GetPaintDC(), cx, cy, (COLORREF **)&pDest);
    ASSERT(hCloneDC);
    ASSERT(hBitmap);

    if (hBitmap != NULL)
    {
        HBITMAP hOldBitmap = (HBITMAP) ::SelectObject(hCloneDC, hBitmap);
        ::BitBlt(hCloneDC, 0, 0, cx, cy, hPaintDC, rc.left, rc.top, SRCCOPY);
        RECT rcClone = {0, 0, cx, cy};

        if (dwFilterColor > 0x00FFFFFF) { DrawColor(hCloneDC, rcClone, dwFilterColor); }

        ::SelectObject(hCloneDC, hOldBitmap);
        ::DeleteDC(hCloneDC);
        ::GdiFlush();
    }

    // Cleanup
    ::SelectObject(hPaintDC, hOldPaintBitmap);
    ::DeleteObject(hPaintBitmap);
    ::DeleteDC(hPaintDC);

    return hBitmap;
}

SIZE CRenderEngine::GetTextSize(HDC hDC, CPaintManagerUI *pManager, LPCTSTR pstrText, int iFont, UINT uStyle)
{
    SIZE size = {0, 0};
    ASSERT(::GetObjectType(hDC) == OBJ_DC || ::GetObjectType(hDC) == OBJ_MEMDC);

    if (pstrText == NULL || pManager == NULL) { return size; }

    ::SetBkMode(hDC, TRANSPARENT);
    HFONT hOldFont = (HFONT)::SelectObject(hDC, pManager->GetFont(iFont));
    GetTextExtentPoint32(hDC, pstrText, _tcslen(pstrText), &size);
    ::SelectObject(hDC, hOldFont);
    return size;
}

} // namespace DuiLib
