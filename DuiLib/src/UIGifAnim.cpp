#include "StdAfx.h"
#include "gifinfo.h"

///////////////////////////////////////////////////////////////////////////////////////

namespace DuiLib {
#define EVENT_TIEM_ID   100
/*
CGifAnimUI::CGifAnimUI(void)
{
    m_pGifImage         =   NULL;
    m_pPropertyItem     =   NULL;
    m_nFrameCount       =   0;
    m_nFramePosition    =   0;
    m_bIsAutoPlay       =   true;
    m_bIsAutoSize       =   false;
    m_bIsPlaying        =   false;

}


CGifAnimUI::~CGifAnimUI(void)
{
    DeleteGif();
    m_pManager->KillTimer(this, EVENT_TIEM_ID);

}

LPCTSTR CGifAnimUI::GetClass() const
{
    return DUI_CTR_GIFANIM;
}

LPVOID CGifAnimUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_GIFANIM) == 0) { return static_cast<CGifAnimUI *>(this); }

    return CControlUI::GetInterface(pstrName);
}

void CGifAnimUI::DoInit()
{
    InitGifImage();
}

bool CGifAnimUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    if (NULL == m_pGifImage)
    {
        InitGifImage();
    }

    DrawFrame(hDC);
    return true;
}

void CGifAnimUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_TIMER)
    { OnTimer((UINT_PTR)event.wParam); }
}

void CGifAnimUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);

    if (bVisible)
    { PlayGif(); }
    else
    { StopGif(); }
}

void CGifAnimUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("bkimage")) == 0) { SetBkImage(pstrValue); }
    else if (_tcscmp(pstrName, _T("autoplay")) == 0)
    {
        SetAutoPlay(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else if (_tcscmp(pstrName, _T("autosize")) == 0)
    {
        SetAutoSize(_tcscmp(pstrValue, _T("true")) == 0);
    }
    else
    { CControlUI::SetAttribute(pstrName, pstrValue); }
}

void CGifAnimUI::SetBkImage(LPCTSTR pStrImage)
{
    if (m_sBkImage == pStrImage || NULL == pStrImage) { return; }

    m_sBkImage = pStrImage;

    StopGif();
    DeleteGif();

    Invalidate();

}

LPCTSTR CGifAnimUI::GetBkImage()
{
    return m_sBkImage.GetData();
}

void CGifAnimUI::SetAutoPlay(bool bIsAuto)
{
    m_bIsAutoPlay = bIsAuto;
}

bool CGifAnimUI::IsAutoPlay() const
{
    return m_bIsAutoPlay;
}

void CGifAnimUI::SetAutoSize(bool bIsAuto)
{
    m_bIsAutoSize = bIsAuto;
}

bool CGifAnimUI::IsAutoSize() const
{
    return m_bIsAutoSize;
}

void CGifAnimUI::PlayGif()
{
    if (m_bIsPlaying || m_pGifImage == NULL)
    {
        return;
    }

    long lPause = ((long *) m_pPropertyItem->value)[m_nFramePosition] * 10;

    if (lPause == 0) { lPause = 100; }

    m_pManager->SetTimer(this, EVENT_TIEM_ID, lPause);

    m_bIsPlaying = true;
}

void CGifAnimUI::PauseGif()
{
    if (!m_bIsPlaying || m_pGifImage == NULL)
    {
        return;
    }

    m_pManager->KillTimer(this, EVENT_TIEM_ID);
    this->Invalidate();
    m_bIsPlaying = false;
}

void CGifAnimUI::StopGif()
{
    if (!m_bIsPlaying)
    {
        return;
    }

    m_pManager->KillTimer(this, EVENT_TIEM_ID);
    m_nFramePosition = 0;
    this->Invalidate();
    m_bIsPlaying = false;
}

void CGifAnimUI::InitGifImage()
{
    m_pGifImage = LoadGifFromFile(GetBkImage());

    if (NULL == m_pGifImage) { return; }

    UINT nCount = 0;
    nCount  =   m_pGifImage->GetFrameDimensionsCount();
    GUID *pDimensionIDs =   new GUID[ nCount ];
    m_pGifImage->GetFrameDimensionsList(pDimensionIDs, nCount);
    m_nFrameCount   =   m_pGifImage->GetFrameCount(&pDimensionIDs[0]);
    int nSize       =   m_pGifImage->GetPropertyItemSize(PropertyTagFrameDelay);
    m_pPropertyItem = (Gdiplus::PropertyItem *) malloc(nSize);
    m_pGifImage->GetPropertyItem(PropertyTagFrameDelay, nSize, m_pPropertyItem);
    delete  pDimensionIDs;
    pDimensionIDs = NULL;

    if (m_bIsAutoSize)
    {
        SetFixedWidth(m_pGifImage->GetWidth());
        SetFixedHeight(m_pGifImage->GetHeight());
    }

    if (m_bIsAutoPlay && nSize > 0)
    {
        PlayGif();
    }
}

void CGifAnimUI::DeleteGif()
{
    if (m_pGifImage != NULL)
    {
        delete m_pGifImage;
        m_pGifImage = NULL;
    }

    if (m_pPropertyItem != NULL)
    {
        free(m_pPropertyItem);
        m_pPropertyItem = NULL;
    }

    m_nFrameCount       =   0;
    m_nFramePosition    =   0;
}

void CGifAnimUI::OnTimer(UINT_PTR idEvent)
{
    if (idEvent != EVENT_TIEM_ID)
    { return; }

    m_pManager->KillTimer(this, EVENT_TIEM_ID);
    this->Invalidate();

    m_nFramePosition = (++m_nFramePosition) % m_nFrameCount;

    long lPause = ((long *) m_pPropertyItem->value)[m_nFramePosition] * 10;

    if (lPause == 0) { lPause = 100; }

    m_pManager->SetTimer(this, EVENT_TIEM_ID, lPause);
}

void CGifAnimUI::DrawFrame(HDC hDC)
{
    if (NULL == hDC || NULL == m_pGifImage) { return; }

    GUID pageGuid = Gdiplus::FrameDimensionTime;
    Gdiplus::Graphics graphics(hDC);
    graphics.DrawImage(m_pGifImage, (int)m_rcItem.left, (int)m_rcItem.top,
                       int(m_rcItem.right - m_rcItem.left), int(m_rcItem.bottom - m_rcItem.top));
    m_pGifImage->SelectActiveFrame(&pageGuid, m_nFramePosition);
}

Gdiplus::Image *CGifAnimUI::LoadGifFromFile(LPCTSTR pstrGifPath)
{
    LPBYTE pData = NULL;
    DWORD dwSize = 0;

    do
    {
        CDuiString sFile = CPaintManagerUI::GetResourcePath();

        if (CPaintManagerUI::GetResourceZip().IsEmpty())
        {
            sFile += pstrGifPath;
            HANDLE hFile = ::CreateFile(sFile.GetData(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
                                        FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile == INVALID_HANDLE_VALUE) { break; }

            dwSize = ::GetFileSize(hFile, NULL);

            if (dwSize == 0) { break; }

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

            if (FindZipItem(hz, pstrGifPath, true, &i, &ze) != 0) { break; }

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

    } while (0);

    while (!pData)
    {
        //读不到图片, 则直接去读取bitmap.m_lpstr指向的路径
        HANDLE hFile = ::CreateFile(pstrGifPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, \
                                    FILE_ATTRIBUTE_NORMAL, NULL);

        if (hFile == INVALID_HANDLE_VALUE) { break; }

        dwSize = ::GetFileSize(hFile, NULL);

        if (dwSize == 0) { break; }

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
        return NULL;
    }

    Gdiplus::Image *pImage = LoadGifFromMemory(pData, dwSize);
    delete pData;
    return pImage;
}

Gdiplus::Image *CGifAnimUI::LoadGifFromMemory(LPVOID pBuf, size_t dwSize)
{
    HGLOBAL hMem = ::GlobalAlloc(GMEM_FIXED, dwSize);
    BYTE *pMem = (BYTE *)::GlobalLock(hMem);

    memcpy(pMem, pBuf, dwSize);

    IStream *pStm = NULL;
    ::CreateStreamOnHGlobal(hMem, TRUE, &pStm);
    Gdiplus::Image *pImg = Gdiplus::Image::FromStream(pStm);

    if (!pImg || pImg->GetLastStatus() != Gdiplus::Ok)
    {
        pStm->Release();
        ::GlobalUnlock(hMem);
        return 0;
    }

    return pImg;
}
*/
//////////////////////////////////////////////////////////////////////////
CGifAnimUI::CGifAnimUI(void) : m_pcGifInfo(NULL), m_bIsAutoPlay(true), m_bIsAutoSize(true),
    m_bIsPlaying(false)
{

}

CGifAnimUI::~CGifAnimUI(void)
{
    DeleteGif();
    m_pManager->KillTimer(this, EVENT_TIEM_ID);
}

LPCTSTR CGifAnimUI::GetClass() const
{
    return DUI_CTR_GIFANIM;
}

LPVOID CGifAnimUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_GIFANIM) == 0) { return static_cast<CGifAnimUI *>(this); }

    return CControlUI::GetInterface(pstrName);
}

void CGifAnimUI::DoInit()
{
    CControlUI::DoInit();
    InitGifImage();
}

bool CGifAnimUI::DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl)
{
    if (NULL == m_pcGifInfo) { InitGifImage(); }

    DrawFrame(hDC);
    return true;
}

void CGifAnimUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_TIMER) { OnTimer((UINT_PTR)event.wParam); }
}

bool CGifAnimUI::SetVisible(bool bVisible /*= true*/)
{
    // 2018-08-18 zhuyadong 添加特效
    if (!CControlUI::SetVisible(bVisible)) { return false; }

    if (bVisible) { PlayGif(); }
    else { StopGif(); }

    return true;
}

void CGifAnimUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("bkimage")) == 0) { SetBkImage(ParseString(pstrValue)); }
    else if (_tcscmp(pstrName, _T("autoplay")) == 0)
    {
        SetAutoPlay(ParseBool(pstrValue));
    }
    else if (_tcscmp(pstrName, _T("autosize")) == 0)
    {
        SetAutoSize(ParseBool(pstrValue));
    }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CControlUI::SetAttribute(pstrName, pstrValue); }
}

void CGifAnimUI::SetBkImage(LPCTSTR pStrImage)
{
    if (m_diBk.sDrawString == pStrImage && m_diBk.pImageInfo != NULL) { return; }

    m_diBk.Clear();
    m_diBk.sDrawString = pStrImage;

    if (m_bFloat && m_cxyFixed.cx == 0 && m_cxyFixed.cy == 0 && m_diBk.pImageInfo)
    {
        m_cxyFixed.cx = m_diBk.pImageInfo->nX;
        m_cxyFixed.cy = m_diBk.pImageInfo->nY;
    }

    StopGif();
    DeleteGif();
    Invalidate();
}

void CGifAnimUI::SetAutoPlay(bool bIsAuto)
{
    m_bIsAutoPlay = bIsAuto;
}

bool CGifAnimUI::IsAutoPlay() const
{
    return m_bIsAutoPlay;
}

void CGifAnimUI::SetAutoSize(bool bIsAuto)
{
    m_bIsAutoSize = bIsAuto;
}

bool CGifAnimUI::IsAutoSize() const
{
    return m_bIsAutoSize;
}

void CGifAnimUI::PlayGif()
{
    if (m_bIsPlaying || NULL == m_pcGifInfo) { return; }

    TImageInfo *pFrame = m_pcGifInfo->GetCurFrame();
    DWORD dwPause = (0 != pFrame->dwMask) ? pFrame->dwMask : 100;
    m_pManager->SetTimer(this, EVENT_TIEM_ID, dwPause);
    m_bIsPlaying = true;
}

void CGifAnimUI::PauseGif()
{
    if (!m_bIsPlaying || NULL == m_pcGifInfo) { return; }

    m_pManager->KillTimer(this, EVENT_TIEM_ID);
    //this->Invalidate();
    m_bIsPlaying = false;
}

void CGifAnimUI::StopGif()
{
    if (!m_bIsPlaying || NULL == m_pcGifInfo) { return; }

    m_pManager->KillTimer(this, EVENT_TIEM_ID);
    m_bIsPlaying = false;
    m_pcGifInfo->SetCurFrame(0);
    this->Invalidate();
}

void CGifAnimUI::InitGifImage()
{
    if (NULL != m_pcGifInfo) { return; }

    m_pcGifInfo = CRenderEngine::LoadGif(m_diBk);

    if (NULL == m_pcGifInfo) { return; }

    TImageInfo *pFrame = m_pcGifInfo->GetCurFrame();

    if (m_bIsAutoSize && NULL != pFrame)
    {
        SetFixedWidth(pFrame->nX);
        SetFixedHeight(pFrame->nY);
    }

    if (m_bIsAutoPlay && m_pcGifInfo->GetFrameCount() > 0)
    {
        PlayGif();
    }
}

void CGifAnimUI::DeleteGif()
{
    if (m_pcGifInfo != NULL)
    {
        delete m_pcGifInfo;
        m_pcGifInfo = NULL;
    }
}

void CGifAnimUI::OnTimer(UINT_PTR idEvent)
{
    if (idEvent != EVENT_TIEM_ID || NULL == m_pcGifInfo) { return; }

    m_pManager->KillTimer(this, idEvent);
    this->Invalidate();

    TImageInfo *pFrame = m_pcGifInfo->GetNextFrame();
    DWORD dwPause = (0 != pFrame->dwMask) ? pFrame->dwMask : 100;
    m_pManager->SetTimer(this, idEvent, dwPause);
}

void CGifAnimUI::DrawFrame(HDC hDC)
{
    if (NULL == hDC || NULL == m_pcGifInfo) { return; }

    TImageInfo *pFrame = m_pcGifInfo->GetCurFrame();
    RECT rcBmpPart = { 0, 0, pFrame->nX, pFrame->nY };
    RECT rcScale9 = { 0 };
    CRenderEngine::DrawImage(hDC, pFrame->hBitmap, m_rcItem, m_rcPaint, rcBmpPart, rcScale9, pFrame->bAlpha);
}

}
