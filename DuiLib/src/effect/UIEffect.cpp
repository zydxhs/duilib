#include "stdafx.h"
#include "Effect.h"
#include <time.h>

namespace DuiLib {
/*
CEffectUI::CEffectUI(void) : m_pControl(NULL), m_cAryAniContainer(sizeof(TAniParamEx), 1)
{
}

CEffectUI::~CEffectUI(void)
{
    ClearAllAnimation();
}

BOOL CEffectUI::AddAnimation(TAniParam &tParam)
{
    TAniParamEx tInnerParam;
    memcpy(&tInnerParam.param, &tParam, sizeof(tParam));
    DWORD dataSize = tInnerParam.param.bmpSize.cx * tInnerParam.param.bmpSize.cy * 4;
    tInnerParam.bmpDataCopy = new BYTE[dataSize];
    tInnerParam.pEffect = CreateEffect(tParam.aniEffect);

    if (tInnerParam.bmpDataCopy == NULL || NULL == tInnerParam.pEffect)
    {
        delete tInnerParam.bmpDataCopy;
        delete tInnerParam.pEffect;
        return FALSE;
    }

    memcpy(tInnerParam.bmpDataCopy, tParam.pBmpData, dataSize);
    tInnerParam.frameNow = 0;
    tInnerParam.bLastFrame = FALSE;

    tInnerParam.pEffect->InitEffectParam(&tInnerParam);
    m_cAryAniContainer.Add(&tInnerParam);
    return TRUE;
}

BOOL CEffectUI::DelAnimation(DWORD animationEffect)
{
    BOOL bFind = FALSE;
    TAniParamEx *ptItem = NULL;

    for (int i = 0; i < m_cAryAniContainer.GetSize(); ++i)
    {
        ptItem = (TAniParamEx *)m_cAryAniContainer.GetAt(i);

        if (ptItem->param.aniEffect == animationEffect)
        {
            delete[ ]ptItem->bmpDataCopy;
            ptItem->pEffect->ReleaseEffectParam();
            delete ptItem->pEffect;
            m_cAryAniContainer.Remove(i);
            bFind = TRUE;
            break;
        }
    }

    return  bFind;
}

BOOL CEffectUI::ClearAllAnimation()
{
    // 清理资源
    for (int i = 0; i < m_cAryAniContainer.GetSize(); ++i)
    {
        TAniParamEx *pElem = (TAniParamEx *)m_cAryAniContainer.GetAt(i);
        // 删除拷贝的内存
        delete[ ]pElem->bmpDataCopy;
        pElem->bmpDataCopy = NULL;
        // 删除特效类
        pElem->pEffect->ReleaseEffectParam();
        delete pElem->pEffect;
        pElem->pEffect = NULL;
    }

    m_cAryAniContainer.Empty();
    return TRUE;
}*/
/*
BOOL CEffectUI::ExecAnimation(IEffectCallBackUI *iDrawEffect, DWORD frameSpin)
{
    if (iDrawEffect == NULL) { return FALSE; }

    DWORD time_start = clock();
    DWORD delta_time = 0;

    // 队列非空，执行动画
    while (!m_cAryAniContainer.IsEmpty())
    {
        ComputeAnimation(iDrawEffect, delta_time);

        iDrawEffect->OnEffectDraw();

        CleanFinishedAnimation(iDrawEffect);

        Sleep(1);

        delta_time = clock() - time_start;
    }

    return TRUE;
}

void CEffectUI::ComputeAnimation(IEffectCallBackUI *iDrawEffect, DWORD timeElapse)
{
    TInnerAniParam *ptItem = NULL;
    IEffect *pEffect = NULL;

    // 此循环过程中不能插入
    for (int i = 0; i < m_cAryAniContainer.GetSize(); ++i)
    {
        ptItem = (TInnerAniParam *)m_cAryAniContainer.GetAt(i);
        pEffect = ptItem->pEffect;

        if ((!ptItem->frameNow))
        {
            // 第一帧
            ptItem->frameNow++;
            pEffect->ComputeOneFrame(ptItem);
            // 通知绘制模块
            iDrawEffect->OnEffectBegin(ptItem->param.context, ptItem->param.aniEffect);
        }
        else if (timeElapse / ptItem->param.aniFrequency >= ptItem->frameNow)
        {
            // 符合下一帧条件，重新计算
            ptItem->frameNow++;
            pEffect->ComputeOneFrame(ptItem);
        }
    }
}

void CEffectUI::CleanFinishedAnimation(IEffectCallBackUI *iDrawEffect)
{
    // 绘制完成后删除完成的动画
    TInnerAniParam *ptItem = NULL;

    for (int i = 0; i < m_cAryAniContainer.GetSize(); ++i)
    {
        ptItem = (TInnerAniParam *)m_cAryAniContainer.GetAt(i);

        if (ptItem->bLastFrame)
        {
            iDrawEffect->OnEffectEnd(ptItem->param.context, ptItem->param.aniEffect);
            delete[ ] ptItem->bmpDataCopy;
            IEffect *pEffect = ptItem->pEffect;
            pEffect->ReleaseEffectParam();
            delete pEffect;
            m_cAryAniContainer.Remove(i);
            break;
        }
    }
}
*/
//////////////////////////////////////////////////////////////////////////
void RestoreAlphaColor(LPBYTE pBits, int bitsWidth, PRECT rc)
{
    for (int i = rc->top; i < rc->bottom; ++i)
    {
        for (int j = rc->left; j < rc->right; ++j)
        {
            if ((pBits[3] == 0) && (pBits[0] != 0 || pBits[1] != 0 || pBits[2] != 0)) { pBits[3] = 255; }

            pBits += 4;
            // int x = (i * bitsWidth + j) * 4;
            //
            // if ((pBits[x + 3] == 0) && (pBits[x + 0] != 0 || pBits[x + 1] != 0 || pBits[x + 2] != 0))
            // {
            //     pBits[x + 3] = 255;
            // }
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
    RECT rcRestore = rcCtrl;
    RestoreAlphaColor((LPBYTE)bmDst.bmBits, bmDst.bmWidth, &rcRestore);

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
        m_pControl->OnEffectEnd(*pData);

        if (pData->bLoop)
        {
            pData->byCurFrame = 0;
        }
        else
        {
            m_pControl->GetManager()->KillTimer(m_pControl, pData->byTrigger);
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

TAniParam *CEffectUI::GetTriggerById(BYTE byTrigger)
{
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
