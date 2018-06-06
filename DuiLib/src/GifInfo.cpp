#include "StdAfx.h"
#include "GifInfo.h"

namespace DuiLib {
CGifInfo::CGifInfo(int nFrameCnt) : vGifFrame(nFrameCnt), nCurFrame(0), bIsDeleting(false)
{
}

CGifInfo::~CGifInfo(void)
{
    bIsDeleting = true;

    for (int i = 0; i < vGifFrame.GetSize(); i++)
    {
        CRenderEngine::FreeImage((TImageInfo *)vGifFrame.GetAt(i));
    }
}

int CGifInfo::GetFrameCount(void)
{
    return vGifFrame.GetSize();
}

void CGifInfo::AddFrame(TImageInfo *ptFrame)
{
    if (NULL != ptFrame) { vGifFrame.Add(ptFrame); }
}

void CGifInfo::SetCurFrame(int nCurFrame)
{
    if (nCurFrame < vGifFrame.GetSize())
    {
        this->nCurFrame = nCurFrame;
    }
}

TImageInfo *CGifInfo::GetCurFrame(void)
{
    if (bIsDeleting == false)
    {
        return (TImageInfo *)vGifFrame.GetAt(nCurFrame);
    }

    return NULL;
}

TImageInfo *CGifInfo::GetNextFrame(void)
{
    if (bIsDeleting == false)
    {
        int n = nCurFrame++;

        if (nCurFrame >= vGifFrame.GetSize())
        {
            nCurFrame = 0;
        }

        return (TImageInfo *)vGifFrame.GetAt(n);
    }

    return NULL;
}

TImageInfo *CGifInfo::GetFrame(int nIdx)
{
    if (bIsDeleting == false && nIdx < vGifFrame.GetSize())
    {
        return (TImageInfo *)vGifFrame.GetAt(nIdx);
    }

    return NULL;
}

}
