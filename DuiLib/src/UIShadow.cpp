#include "StdAfx.h"
#include "UIShadow.h"
#include "math.h"
#include "crtdbg.h"

namespace DuiLib {

const TCHAR *strWndClassName = _T("PerryShadowWnd");
bool CShadowUI::s_bHasInit = FALSE;

CShadowUI::CShadowUI(void)
    : m_hWnd((HWND)NULL)
    , m_OriParentProc(NULL)
    , m_Status(0)
    , m_nDarkness(200)
      //, m_nSharpness(5)
    , m_nSize(5)
    , m_nxOffset(0)
    , m_nyOffset(0)
    , m_Color(RGB(0, 0, 0))
    , m_WndSize(0)
    , m_bUpdate(false)
    , m_bIsShow(false)
{
}

CShadowUI::~CShadowUI(void)
{
}

bool CShadowUI::Initialize(HINSTANCE hInstance)
{
    if (s_bHasInit) { return false; }

    // Register window class for shadow window
    WNDCLASSEX wcex;

    memset(&wcex, 0, sizeof(wcex));

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = DefWindowProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = NULL;
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = strWndClassName;
    wcex.hIconSm        = NULL;

    RegisterClassEx(&wcex);

    s_bHasInit = true;
    return true;
}

void CShadowUI::Create(CPaintManagerUI *pPaintManager)
{
    if (!m_bIsShow) { return; }

    // Already initialized
    _ASSERT(CPaintManagerUI::GetInstance() != INVALID_HANDLE_VALUE);
    _ASSERT(pPaintManager != NULL);
    m_pManager = pPaintManager;
    HWND hParentWnd = m_pManager->GetPaintWindow();
    // Add parent window - shadow pair to the map
    _ASSERT(GetShadowMap().find(hParentWnd) == GetShadowMap().end());   // Only one shadow for each window
    GetShadowMap()[hParentWnd] = this;

    // Determine the initial show state of shadow according to parent window's state
    LONG lParentStyle = GetWindowLongPtr(hParentWnd, GWL_STYLE);

    // Create the shadow window
    LONG styleValue = lParentStyle & WS_CAPTION;
    m_hWnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, strWndClassName, NULL,
                            styleValue | WS_POPUPWINDOW | WS_DISABLED,
                            CW_USEDEFAULT, 0, 0, 0, hParentWnd, NULL, CPaintManagerUI::GetInstance(), NULL);

    if (!(WS_VISIBLE & lParentStyle))   // Parent invisible
    { m_Status = SS_ENABLED; }
    else if ((WS_MAXIMIZE | WS_MINIMIZE) & lParentStyle) // Parent visible but does not need shadow
    { m_Status = SS_ENABLED | SS_PARENTVISIBLE; }
    else    // Show the shadow
    {
        m_Status = SS_ENABLED | SS_VISABLE | SS_PARENTVISIBLE;
        ::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
        Update(hParentWnd);
    }

    // Replace the original WndProc of parent window to steal messages
    m_OriParentProc = GetWindowLongPtr(hParentWnd, GWLP_WNDPROC);

#pragma warning(disable: 4311)  // temporrarily disable the type_cast warning in Win32
    SetWindowLongPtr(hParentWnd, GWLP_WNDPROC, (LONG_PTR)ParentProc);
#pragma warning(default: 4311)
}

std::map<HWND, CShadowUI *> &CShadowUI::GetShadowMap()
{
    static std::map<HWND, CShadowUI *> s_Shadowmap;
    return s_Shadowmap;
}

LRESULT CALLBACK CShadowUI::ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    _ASSERT(GetShadowMap().find(hwnd) != GetShadowMap().end()); // Shadow must have been attached

    CShadowUI *pThis = GetShadowMap()[hwnd];

    switch (uMsg)
    {
    case WM_MOVE:
        if (pThis->m_Status & SS_VISABLE)
        {
            RECT WndRect;
            GetWindowRect(hwnd, &WndRect);

            if (!pThis->m_diImgShadow.sDrawString.IsEmpty())
            {
                SetWindowPos(pThis->m_hWnd, 0,
                             //WndRect.left - pThis->m_rcShadowCorner.left, WndRect.top - pThis->m_rcShadowCorner.top,
                             WndRect.left - pThis->m_diImgShadow.rcScale9.left,
                             WndRect.top - pThis->m_diImgShadow.rcScale9.top,
                             0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }
            else
            {
                SetWindowPos(pThis->m_hWnd, 0,
                             WndRect.left + pThis->m_nxOffset - pThis->m_nSize, WndRect.top + pThis->m_nyOffset - pThis->m_nSize,
                             0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
            }
        }

        break;

    case WM_SIZE:

        // 2019-10-11 || 后的条件，用于解决创建的隐藏窗口，显示后没有阴影的问题
        if ((pThis->m_Status & SS_ENABLED) || (0 == pThis->m_WndSize && !(pThis->m_Status & SS_ENABLED)))
        {
            if (SIZE_MAXIMIZED == wParam || SIZE_MINIMIZED == wParam)
            {
                ::ShowWindow(pThis->m_hWnd, SW_HIDE);
                pThis->m_Status &= ~SS_VISABLE;
            }
            else if (pThis->m_Status & SS_PARENTVISIBLE) // Parent maybe resized even if invisible
            {
                // Awful! It seems that if the window size was not decreased
                // the window region would never be updated until WM_PAINT was sent.
                // So do not Update() until next WM_PAINT is received in this case
                if (LOWORD(lParam) > LOWORD(pThis->m_WndSize) || HIWORD(lParam) > HIWORD(pThis->m_WndSize))
                { pThis->m_bUpdate = true; }
                else
                { pThis->Update(hwnd); }

                if (!(pThis->m_Status & SS_VISABLE))
                {
                    ::ShowWindow(pThis->m_hWnd, SW_SHOWNA);
                    pThis->m_Status |= SS_VISABLE;
                }
            }

            // 2019-10-11 || 后的条件，用于解决创建的隐藏窗口，显示后没有阴影的问题
            if (0 == pThis->m_WndSize && !(pThis->m_Status & SS_ENABLED)) { ::ShowWindow(pThis->m_hWnd, SW_HIDE); }

            pThis->m_WndSize = lParam;
        }

        break;

    case WM_PAINT:
        {
            if (pThis->m_bUpdate)
            {
                pThis->Update(hwnd);
                pThis->m_bUpdate = false;
            }

            //return hr;
            break;
        }

    // In some cases of sizing, the up-right corner of the parent window region would not be properly updated
    // Update() again when sizing is finished
    case WM_EXITSIZEMOVE:
        if (pThis->m_Status & SS_VISABLE)
        {
            pThis->Update(hwnd);
        }

        break;

    case WM_SHOWWINDOW:

        // 2019-10-11 || 后的条件，用于解决创建的隐藏窗口，显示后没有阴影的问题
        if ((pThis->m_Status & SS_ENABLED) || (0 == pThis->m_WndSize && !(pThis->m_Status & SS_ENABLED)))
        {
            if (!wParam) // the window is being hidden
            {
                ::ShowWindow(pThis->m_hWnd, SW_HIDE);
                pThis->m_Status &= ~(SS_VISABLE | SS_PARENTVISIBLE);
            }
            else if (!(pThis->m_Status & SS_PARENTVISIBLE))
            {
                //pThis->Update(hwnd);
                pThis->m_bUpdate = true;
                ::ShowWindow(pThis->m_hWnd, SW_SHOWNA);
                pThis->m_Status |= SS_VISABLE | SS_PARENTVISIBLE;
            }
        }

        break;

    case WM_DESTROY:
        DestroyWindow(pThis->m_hWnd);   // Destroy the shadow
        break;

    case WM_NCDESTROY:
        GetShadowMap().erase(hwnd); // Remove this window and shadow from the map
        break;

    case WM_MOUSEACTIVATE:
        return MA_NOACTIVATE;
    }


#pragma warning(disable: 4312)  // temporrarily disable the type_cast warning in Win32
    // Call the default(original) window procedure for other messages or messages processed but not returned
    return ((WNDPROC)pThis->m_OriParentProc)(hwnd, uMsg, wParam, lParam);
#pragma warning(default: 4312)

}

void CShadowUI::Update(HWND hParent)
{
    RECT WndRect;
    GetWindowRect(hParent, &WndRect);
    int nShadWndWid;
    int nShadWndHei;

    if (!m_diImgShadow.sDrawString.IsEmpty())
    {
        if (m_diImgShadow.sImageName.IsEmpty())
        {
            CDuiString sResType;
            DWORD dwMask = 0;
            bool bHSL = false;
            CRenderEngine::ParseDrawInfo(m_diImgShadow, sResType, dwMask, bHSL);
            const TImageInfo *data = m_pManager->GetImageEx((LPCTSTR)m_diImgShadow.sImageName, (LPCTSTR)sResType, 0, false);

            if (!data) { return; }

            m_diImgShadow.pImageInfo = data;
        }

        nShadWndWid = WndRect.right - WndRect.left + m_diImgShadow.rcScale9.left + m_diImgShadow.rcScale9.right;
        nShadWndHei = WndRect.bottom - WndRect.top + m_diImgShadow.rcScale9.top + m_diImgShadow.rcScale9.bottom;
    }
    else
    {
        if (m_nSize == 0) { return; }

        nShadWndWid = WndRect.right - WndRect.left + m_nSize * 2;
        nShadWndHei = WndRect.bottom - WndRect.top + m_nSize * 2;
    }

    // Create the alpha blending bitmap
    BITMAPINFO bmi;        // bitmap header

    ZeroMemory(&bmi, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nShadWndWid;
    bmi.bmiHeader.biHeight = nShadWndHei;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;         // four 8-bit components
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = nShadWndWid * nShadWndHei * 4;

    BYTE *pvBits;          // pointer to DIB section
    HBITMAP hbitmap = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (void **)&pvBits, NULL, 0);
    HDC hMemDC = CreateCompatibleDC(NULL);
    HBITMAP hOriBmp = (HBITMAP)SelectObject(hMemDC, hbitmap);

    if (!m_diImgShadow.sDrawString.IsEmpty())
    {
        RECT rcPaint = {0, 0, nShadWndWid, nShadWndHei};
        const TImageInfo *data = m_diImgShadow.pImageInfo;
        RECT rcCorner = m_diImgShadow.rcScale9;


        RECT rcBmpPart = {0};
        rcBmpPart.right = m_diImgShadow.pImageInfo->nX;
        rcBmpPart.bottom = m_diImgShadow.pImageInfo->nY;

        CRenderEngine::DrawImage(hMemDC, m_diImgShadow.pImageInfo->hBitmap, rcPaint, rcPaint, rcBmpPart,
                                 rcCorner, m_diImgShadow.pImageInfo->bAlpha, 0xFF, true, false, false);
    }
    else
    {
        ZeroMemory(pvBits, bmi.bmiHeader.biSizeImage);
        MakeShadow((UINT32 *)pvBits, hParent, &WndRect);
    }

    POINT ptDst;

    if (!m_diImgShadow.sDrawString.IsEmpty())
    {
        ptDst.x = WndRect.left - m_diImgShadow.rcScale9.left;
        ptDst.y = WndRect.top - m_diImgShadow.rcScale9.top;
    }
    else
    {
        ptDst.x = WndRect.left + m_nxOffset - m_nSize;
        ptDst.y = WndRect.top + m_nyOffset - m_nSize;
    }

    POINT ptSrc = {0, 0};
    SIZE WndSize = {nShadWndWid, nShadWndHei};
    BLENDFUNCTION blendPixelFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

    MoveWindow(m_hWnd, ptDst.x, ptDst.y, nShadWndWid, nShadWndHei, FALSE);

    BOOL bRet = ::UpdateLayeredWindow(m_hWnd, NULL, &ptDst, &WndSize, hMemDC,
                                      &ptSrc, 0, &blendPixelFunction, ULW_ALPHA);

    _ASSERT(bRet); // something was wrong....

    // Delete used resources
    SelectObject(hMemDC, hOriBmp);
    DeleteObject(hbitmap);
    DeleteDC(hMemDC);

}

void CShadowUI::MakeShadow(UINT32 *pShadBits, HWND hParent, RECT *rcParent)
{
    // The shadow algorithm:
    // Get the region of parent window,
    // Apply morphologic erosion to shrink it into the size (ShadowWndSize - Sharpness)
    // Apply modified (with blur effect) morphologic dilation to make the blurred border
    // The algorithm is optimized by assuming parent window is just "one piece" and without "wholes" on it

    // Get the region of parent window,
    HRGN hParentRgn = CreateRectRgn(0, 0, rcParent->right - rcParent->left, rcParent->bottom - rcParent->top);
    GetWindowRgn(hParent, hParentRgn);

    // Determine the Start and end point of each horizontal scan line
    SIZE szParent = {rcParent->right - rcParent->left, rcParent->bottom - rcParent->top};
    SIZE szShadow = {szParent.cx + 2 * m_nSize, szParent.cy + 2 * m_nSize};
    // Extra 2 lines (set to be empty) in ptAnchors are used in dilation
    int nAnchors = max(szParent.cy, szShadow.cy);   // # of anchor points pares
    int (*ptAnchors)[2] = new int[nAnchors + 2][2];
    int (*ptAnchorsOri)[2] = new int[szParent.cy][2];   // anchor points, will not modify during erosion
    ptAnchors[0][0] = szParent.cx;
    ptAnchors[0][1] = 0;
    ptAnchors[nAnchors + 1][0] = szParent.cx;
    ptAnchors[nAnchors + 1][1] = 0;

    if (m_nSize > 0)
    {
        // Put the parent window anchors at the center
        for (int i = 0; i < m_nSize; i++)
        {
            ptAnchors[i + 1][0] = szParent.cx;
            ptAnchors[i + 1][1] = 0;
            ptAnchors[szShadow.cy - i][0] = szParent.cx;
            ptAnchors[szShadow.cy - i][1] = 0;
        }

        ptAnchors += m_nSize;
    }

    for (int i = 0; i < szParent.cy; i++)
    {
        // find start point
        int j;

        for (j = 0; j < szParent.cx; j++)
        {
            if (PtInRegion(hParentRgn, j, i))
            {
                ptAnchors[i + 1][0] = j + m_nSize;
                ptAnchorsOri[i][0] = j;
                break;
            }
        }

        if (j >= szParent.cx)   // Start point not found
        {
            ptAnchors[i + 1][0] = szParent.cx;
            ptAnchorsOri[i][1] = 0;
            ptAnchors[i + 1][0] = szParent.cx;
            ptAnchorsOri[i][1] = 0;
        }
        else
        {
            // find end point
            for (j = szParent.cx - 1; j >= ptAnchors[i + 1][0]; j--)
            {
                if (PtInRegion(hParentRgn, j, i))
                {
                    ptAnchors[i + 1][1] = j + 1 + m_nSize;
                    ptAnchorsOri[i][1] = j + 1;
                    break;
                }
            }
        }
    }

    if (m_nSize > 0) { ptAnchors -= m_nSize; }  // Restore pos of ptAnchors for erosion

    int (*ptAnchorsTmp)[2] = new int[nAnchors + 2][2];  // Store the result of erosion
    // First and last line should be empty
    ptAnchorsTmp[0][0] = szParent.cx;
    ptAnchorsTmp[0][1] = 0;
    ptAnchorsTmp[nAnchors + 1][0] = szParent.cx;
    ptAnchorsTmp[nAnchors + 1][1] = 0;
    // int nEroTimes = 0;
    int nMaxOffset = std::max<int>(abs(m_nxOffset), abs(m_nyOffset));

    // morphologic erosion
    //for (int i = 0; i < m_nSharpness - m_nSize; i++)
    for (int i = 0; i < nMaxOffset; i++)
    {
        // nEroTimes++;

        //ptAnchorsTmp[1][0] = szParent.cx;
        //ptAnchorsTmp[1][1] = 0;
        //ptAnchorsTmp[szParent.cy + 1][0] = szParent.cx;
        //ptAnchorsTmp[szParent.cy + 1][1] = 0;
        for (int j = 1; j < nAnchors + 1; j++)
        {
            ptAnchorsTmp[j][0] = max(ptAnchors[j - 1][0], max(ptAnchors[j][0], ptAnchors[j + 1][0])) + 1;
            ptAnchorsTmp[j][1] = min(ptAnchors[j - 1][1], min(ptAnchors[j][1], ptAnchors[j + 1][1])) - 1;
        }

        // Exchange ptAnchors and ptAnchorsTmp;
        int (*ptAnchorsXange)[2] = ptAnchorsTmp;
        ptAnchorsTmp = ptAnchors;
        ptAnchors = ptAnchorsXange;
    }

    // morphologic dilation
    ptAnchors += (m_nSize < 0 ? -m_nSize : 0) + 1;  // now coordinates in ptAnchors are same as in shadow window
    // Generate the kernel
    // int nKernelSize = m_nSize > m_nSharpness ? m_nSize : m_nSharpness;
    // int nCenterSize = m_nSize > m_nSharpness ? (m_nSize - m_nSharpness) : 0;
    int nKernelSize = m_nSize + nMaxOffset;
    int nCenterSize = 0;
    UINT32 *pKernel = new UINT32[(2 * nKernelSize + 1) * (2 * nKernelSize + 1)];
    UINT32 *pKernelIter = pKernel;

    for (int i = 0; i <= 2 * nKernelSize; i++)
    {
        for (int j = 0; j <= 2 * nKernelSize; j++)
        {
            double dLength = sqrt((i - nKernelSize) * (i - nKernelSize) + (j - nKernelSize) * (double)(j - nKernelSize));

            if (dLength < nCenterSize)
            { *pKernelIter = m_nDarkness << 24 | PreMultiply(m_Color, m_nDarkness); }
            else if (dLength <= nKernelSize)
            {
                //UINT32 nFactor = ((UINT32)((1 - (dLength - nCenterSize) / (m_nSharpness + 1)) * m_nDarkness));
                double x = 1 - dLength / (nKernelSize + 1);
                UINT32 nFactor = (UINT32)(x * x * m_nDarkness);
                *pKernelIter = nFactor << 24 | PreMultiply(m_Color, nFactor);
            }
            else
            { *pKernelIter = 0; }

            //TRACE("%d ", *pKernelIter >> 24);
            pKernelIter ++;
        }

        //TRACE("\n");
    }

    // Generate blurred border
    for (int i = nKernelSize; i < szShadow.cy - nKernelSize; i++)
    {
        int j;

        if (ptAnchors[i][0] < ptAnchors[i][1])
        {
            // Start of line
            for (j = ptAnchors[i][0];
                 j < min(max(ptAnchors[i - 1][0], ptAnchors[i + 1][0]) + 1, ptAnchors[i][1]);
                 j++)
            {
                for (int k = 0; k <= 2 * nKernelSize; k++)
                {
                    UINT32 *pPixel = pShadBits +
                                     (szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
                    UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);

                    for (int l = 0; l <= 2 * nKernelSize; l++)
                    {
                        if (*pPixel < *pKernelPixel) { *pPixel = *pKernelPixel; }

                        pPixel++;
                        pKernelPixel++;
                    }
                }
            }   // for() start of line

            // End of line
            for (j = max(j, min(ptAnchors[i - 1][1], ptAnchors[i + 1][1]) - 1);
                 j < ptAnchors[i][1];
                 j++)
            {
                for (int k = 0; k <= 2 * nKernelSize; k++)
                {
                    UINT32 *pPixel = pShadBits +
                                     (szShadow.cy - i - 1 + nKernelSize - k) * szShadow.cx + j - nKernelSize;
                    UINT32 *pKernelPixel = pKernel + k * (2 * nKernelSize + 1);

                    for (int l = 0; l <= 2 * nKernelSize; l++)
                    {
                        if (*pPixel < *pKernelPixel) { *pPixel = *pKernelPixel; }

                        pPixel++;
                        pKernelPixel++;
                    }
                }
            }   // for() end of line

        }
    }   // for() Generate blurred border

    // Erase unwanted parts and complement missing
    UINT32 clCenter = m_nDarkness << 24 | PreMultiply(m_Color, m_nDarkness);

    for (int i = min(nKernelSize, max(m_nSize - m_nyOffset, 0));
         i < max(szShadow.cy - nKernelSize, min(szParent.cy + m_nSize - m_nyOffset, szParent.cy + 2 * m_nSize));
         i++)
    {
        UINT32 *pLine = pShadBits + (szShadow.cy - i - 1) * szShadow.cx;

        if (i - m_nSize + m_nyOffset < 0 || i - m_nSize + m_nyOffset >=
            szParent.cy) // Line is not covered by parent window
        {
            for (int j = ptAnchors[i][0]; j < ptAnchors[i][1]; j++)
            {
                *(pLine + j) = clCenter;
            }
        }
        else
        {
            for (int j = ptAnchors[i][0];
                 j < min(ptAnchorsOri[i - m_nSize + m_nyOffset][0] + m_nSize - m_nxOffset, ptAnchors[i][1]);
                 j++)
            { *(pLine + j) = clCenter; }

            for (int j = max(ptAnchorsOri[i - m_nSize + m_nyOffset][0] + m_nSize - m_nxOffset, 0);
                 j < min(ptAnchorsOri[i - m_nSize + m_nyOffset][1] + m_nSize - m_nxOffset, szShadow.cx);
                 j++)
            { *(pLine + j) = 0; }

            for (int j = max(ptAnchorsOri[i - m_nSize + m_nyOffset][1] + m_nSize - m_nxOffset, ptAnchors[i][0]);
                 j < ptAnchors[i][1];
                 j++)
            { *(pLine + j) = clCenter; }
        }
    }

    // Delete used resources
    delete[](ptAnchors - (m_nSize < 0 ? -m_nSize : 0) - 1);
    delete[] ptAnchorsTmp;
    delete[] ptAnchorsOri;
    delete[] pKernel;
    DeleteObject(hParentRgn);
}

void CShadowUI::SetShow(bool bShow)
{
    m_bIsShow = bShow;
}

bool CShadowUI::IsShow() const
{
    return m_bIsShow;
}

void CShadowUI::SetShadowShow(bool bShow)
{
    if (bShow) { m_Status |= SS_ENABLED;  }
    else       { m_Status &= ~SS_ENABLED; }

    ShowWindow(m_hWnd, bShow ? SW_NORMAL : SW_HIDE);
}

bool CShadowUI::SetSize(unsigned int NewSize)
{
    m_nSize = (signed char)NewSize;
    m_nSize = (m_nSize > 20) ? 20 : m_nSize;
    m_nSize = (m_nSize <= 1) ? 1 : m_nSize;

    if (m_hWnd != NULL && (SS_VISABLE & m_Status)) { Update(GetParent(m_hWnd)); }

    return true;
}

// bool CShadowUI::SetSharpness(unsigned int NewSharpness)
// {
//     if (NewSharpness > 20) { return false; }
//
//     m_nSharpness = (unsigned char)NewSharpness;
//
//     if (m_hWnd != NULL && (SS_VISABLE & m_Status)) { Update(GetParent(m_hWnd)); }
//
//     return true;
// }

bool CShadowUI::SetDarkness(unsigned int NewDarkness)
{
    if (NewDarkness > 255) { return false; }

    m_nDarkness = (unsigned char)NewDarkness;

    if (m_hWnd != NULL && (SS_VISABLE & m_Status)) { Update(GetParent(m_hWnd)); }

    return true;
}

bool CShadowUI::SetPosition(int NewXOffset, int NewYOffset)
{
    if (NewXOffset > 20 || NewXOffset < -20 || NewYOffset > 20 || NewYOffset < -20) { return false; }

    m_nxOffset = (signed char)NewXOffset;
    m_nyOffset = (signed char)NewYOffset;

    if (m_hWnd != NULL && (SS_VISABLE & m_Status)) { Update(GetParent(m_hWnd)); }

    return true;
}

bool CShadowUI::SetColor(COLORREF NewColor)
{
    m_Color = NewColor;

    if (m_hWnd != NULL && (SS_VISABLE & m_Status)) { Update(GetParent(m_hWnd)); }

    return true;
}

bool CShadowUI::SetImage(LPCTSTR szImage)
{
    if (szImage == NULL || m_diImgShadow.sDrawString == szImage) { return false; }

    m_diImgShadow.Clear();
    m_diImgShadow.sDrawString = szImage;

    if (m_hWnd != NULL && (SS_VISABLE & m_Status)) { Update(GetParent(m_hWnd)); }

    return true;
}

bool CShadowUI::CopyShadow(CShadowUI *pShadow)
{
    if (!m_diImgShadow.sDrawString.IsEmpty())
    {
        pShadow->SetImage(m_diImgShadow.sDrawString);
    }
    else
    {
        pShadow->SetSize((int)m_nSize);
        //pShadow->SetSharpness((unsigned int)m_nSharpness);
        pShadow->SetDarkness((unsigned int)m_nDarkness);
        pShadow->SetColor(m_Color);
        pShadow->SetPosition((int)m_nxOffset, (int)m_nyOffset);
    }

    pShadow->SetShow(m_bIsShow);
    return true;
}
} //namespace DuiLib
