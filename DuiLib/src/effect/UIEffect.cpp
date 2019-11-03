#include "stdafx.h"
#include "Effect.h"
#include <time.h>

namespace DuiLib {

//////////////////////////////////////////////////////////////////////////
void RestoreAlphaColor(LPBYTE pBits, int bitsWidth, const RECT &rc, BYTE byAlpha)
{
    for (int i = rc.top; i < rc.bottom; ++i)
    {
        for (int j = rc.left; j < rc.right; ++j)
        {
            if ((pBits[3] == 0) && (pBits[0] != 0 || pBits[1] != 0 || pBits[2] != 0)) { pBits[3] = byAlpha; }

            pBits += 4;
        }
    }
}

// 动画参数
TAniParam::TAniParam()
    : byTrigger(TRIGGER_NONE)
    , byEffect(EFFECT_FLIPLEFT)
    , wFrequency(150)
    , bDirection(true)
    , bLoop(false)
    , bLastFrame(false)
    , byCurFrame(0)
    , hBitmap(NULL)
    , hMemDC(NULL)
    , szMemDC(SIZE { 0, 0 })
    , pBmpData(NULL)
    , pBmpDataCopy(NULL)
    , pEffect(NULL)
{

}

bool TAniParam::Init(CControlUI *pCtrl)
{
    ASSERT(pCtrl);

    if (NULL == pCtrl) { return false; }

    hMemDC = ::CreateCompatibleDC(pCtrl->GetManager()->GetPaintDC());
    RECT rcCtrl = pCtrl->GetPos();
    hBitmap = CRenderEngine::GenerateBitmap(pCtrl->GetManager(), pCtrl, rcCtrl);
    pEffect = CreateEffect(byEffect);
    int nBmpBytes = (rcCtrl.right - rcCtrl.left) * (rcCtrl.bottom - rcCtrl.top) * 4;
    pBmpDataCopy = (BYTE *)malloc(nBmpBytes);


    if (NULL == hMemDC || NULL == hBitmap || NULL == pEffect || NULL == pBmpDataCopy) { return false; }


    HBITMAP hOldBmp = (HBITMAP) ::SelectObject(hMemDC, hBitmap);
    ::DeleteObject(hOldBmp);

    BITMAP bmDst;
    ::GetObject(hBitmap, sizeof(bmDst), &bmDst);
    szMemDC = { bmDst.bmWidth, bmDst.bmHeight };
    pBmpData = (BYTE *)bmDst.bmBits;

    //修补一下Alpha通道,一些控件(Richedit)会让Alpha为0
    BYTE byAlpha = 255;

    if (0 == _tcscmp(DUI_CTR_RICHEDIT, pCtrl->GetClass()))
    {
        byAlpha = (pCtrl->GetBkColor() >> 24);
    }

    //RECT rcRestore = rcCtrl;
    //RestoreAlphaColor((LPBYTE)bmDst.bmBits, bmDst.bmWidth, &rcRestore);
    rcCtrl.left = rcCtrl.top = 0;
    rcCtrl.right = bmDst.bmWidth;
    rcCtrl.bottom = bmDst.bmHeight;
    RestoreAlphaColor((LPBYTE)bmDst.bmBits, bmDst.bmWidth, rcCtrl, byAlpha);

    memcpy(pBmpDataCopy, pBmpData, nBmpBytes);
    pEffect->InitEffectParam(this);
    return true;
}

void TAniParam::Release(void)
{
    bLastFrame = false;
    byCurFrame = 0;

    if (hBitmap) { ::DeleteObject(hBitmap); hBitmap = NULL; }

    if (hMemDC) { ::DeleteObject(hMemDC); hMemDC = NULL; }

    if (pEffect) { pEffect->ReleaseEffectParam(); delete pEffect;  pEffect = NULL; }

    if (pBmpDataCopy) { delete[ ]pBmpDataCopy; pBmpDataCopy = NULL; }
}

bool TAniParam::IsRunning(void)
{
    return (NULL != hBitmap && NULL != pBmpData && NULL != pBmpDataCopy && NULL != pEffect) ? true : false;
}

CEffectUI::CEffectUI(CControlUI *pCtrl) : m_pControl(pCtrl), m_aryAniParam(sizeof(TAniParam), 1)
{
}

CEffectUI::~CEffectUI(void)
{
    if (!m_aryAniParam.IsEmpty())
    {
        Stop(TRIGGER_ALL);
        Del(TRIGGER_ALL);
    }

    m_pControl = NULL;
}

bool CEffectUI::Add(BYTE byTrigger, BYTE byEffect, WORD wFrequency, bool bDirection, bool bLoop)
{
    TAniParam *pData = GetTriggerById(byTrigger);
    ASSERT(TRIGGER_NONE < byTrigger && TRIGGER_COUNT > byTrigger && EFFECT_COUNT > byEffect && NULL == pData);

    if (TRIGGER_NONE == byTrigger || TRIGGER_COUNT <= byTrigger || EFFECT_COUNT <= byEffect || NULL != pData)
    {
        return false;
    }

    if (0 >= wFrequency) { wFrequency = 150; }

    TAniParam data;
    data.byTrigger  = byTrigger;            // 特效触发器ID
    data.byEffect   = byEffect;             // 动画ID
    data.wFrequency = wFrequency;           // 动画间隔
    data.bDirection      = bDirection;           // 动画顺序
    data.bLoop      = bLoop;                // 是否循环播放特效
    return m_aryAniParam.Add(&data);
}

void CEffectUI::Del(BYTE byTrigger)
{
    if (TRIGGER_NONE == byTrigger) { return; }

    Stop(byTrigger);
    RemoveTriggerById(byTrigger);
}

bool CEffectUI::Start(BYTE byTrigger)
{
    TAniParam *pData = GetTriggerById(byTrigger);

    if (NULL == m_pControl || NULL == pData) { return false; }

    if (!pData->Init(m_pControl)) { pData->Release(); return false; }

    return m_pControl->GetManager()->SetTimer(m_pControl, byTrigger, pData->wFrequency);
}

void CEffectUI::Stop(BYTE byTrigger)
{
    if (m_pControl == NULL) { return; }

    if (TRIGGER_NONE < byTrigger && TRIGGER_COUNT > byTrigger)
    {
        TAniParam *pData = GetTriggerById(byTrigger);

        if (NULL != pData)
        {
            m_pControl->GetManager()->KillTimer(m_pControl, byTrigger);
            pData->Release();
        }
    }
    else if (TRIGGER_ALL == byTrigger)
    {
        for (int i = 0; i < m_aryAniParam.GetSize(); ++i)
        {
            TAniParam *pData = (TAniParam *)m_aryAniParam[i];
            m_pControl->GetManager()->KillTimer(m_pControl, pData->byTrigger);
            pData->Release();
        }
    }
}

bool CEffectUI::IsRunning(BYTE byTrigger)
{
    TAniParam *pData = GetTriggerById(byTrigger);
    return (pData && pData->IsRunning()) ? true : false;
}

bool CEffectUI::HasEffectTrigger(BYTE byTrigger)
{
    return GetTriggerById(byTrigger) ? true : false;
}

void CEffectUI::OnElapse(BYTE byTrigger)
{
    if (NULL == m_pControl) { return; }

    TAniParam *pData = GetTriggerById(byTrigger);

    if (NULL == pData || NULL == pData->pEffect) { return; }

    pData->byCurFrame += 1;
    pData->pEffect->ComputeOneFrame(pData);

    if (pData->byCurFrame == 1)
    {
        m_pControl->OnEffectBegin(*pData);
    }

    m_pControl->OnEffectDraw(*pData);

    if (pData->bLastFrame)
    {
        if (pData->bLoop)
        {
            pData->byCurFrame = 0;
            pData->bLastFrame = false;
        }
        else
        {
            m_pControl->GetManager()->KillTimer(m_pControl, pData->byTrigger);
            m_pControl->OnEffectEnd(*pData);
            pData->Release();
        }
    }
}

HDC CEffectUI::GetMemHDC(BYTE byTrigger)
{
    TAniParam *pData = GetTriggerById(byTrigger);
    return pData ? pData->hMemDC : NULL;
}

BYTE CEffectUI::GetCurFrame(BYTE byTrigger)
{
    TAniParam *pData = GetTriggerById(byTrigger);
    return pData ? pData->byCurFrame : 0;
}

bool CEffectUI::IsLastFrame(BYTE byTrigger)
{
    TAniParam *pData = GetTriggerById(byTrigger);
    return pData ? pData->bLastFrame && !pData->bLoop : false;
}

TAniParam *CEffectUI::GetTriggerById(BYTE byTrigger)
{
    if (TRIGGER_NONE == byTrigger || TRIGGER_COUNT <= byTrigger) { return NULL; }

    for (int i = 0; i < m_aryAniParam.GetSize(); ++i)
    {
        TAniParam *pAniParam = (TAniParam *)m_aryAniParam[i];

        if (byTrigger == pAniParam->byTrigger)
        {
            return pAniParam;
        }
    }

    return NULL;
}

void CEffectUI::RemoveTriggerById(BYTE byTrigger)
{
    if (TRIGGER_ALL == byTrigger)
    {
        m_aryAniParam.Empty();
    }
    else if (TRIGGER_COUNT > byTrigger)
    {
        TAniParam *pData = NULL;

        for (int i = 0; i < m_aryAniParam.GetSize(); ++i)
        {
            pData = (TAniParam *)m_aryAniParam[i];

            if (byTrigger == pData->byTrigger)
            {
                m_aryAniParam.Remove(i);
            }
        }
    }
}

}
