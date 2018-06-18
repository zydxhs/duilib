/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file dropsource.cpp
/// \brief 拖放源
///
///
/// \author zhuyadong@kedacom.com
/// \date 2018-04-27
/// \note
/// -----------------------------------------------------------------------------
/// 修改记录：
/// 日  期        版本        修改人        走读人    修改内容
///
#include "stdafx.h"
#include "DropSource.h"

namespace DuiLib {


CDropSource::CDropSource(void)
{
    m_lRefCount = 1;
}

CDropSource::~CDropSource(void)
{

}

HRESULT __stdcall CDropSource::QueryInterface(REFIID iid, void **ppvObject)
{
    if (iid == IID_IDropSource || iid == IID_IUnknown)
    {
        AddRef();
        *ppvObject = this;
        return S_OK;
    }
    else
    {
        *ppvObject = 0;
        return E_NOINTERFACE;
    }
}

ULONG __stdcall CDropSource::AddRef(void)
{
    return InterlockedIncrement(&m_lRefCount);
}

ULONG __stdcall CDropSource::Release(void)
{
    LONG count = InterlockedDecrement(&m_lRefCount);

    if (count == 0) { delete this; }

    return count;
}

HRESULT __stdcall CDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
{
    // if the <Escape> key has been pressed since the last call, cancel the drop
    if (fEscapePressed == TRUE) { return DRAGDROP_S_CANCEL; }

    // if the <LeftMouse> button has been released, then do the drop!
    if ((grfKeyState & MK_LBUTTON) == 0) { return DRAGDROP_S_DROP; }

    // continue with the drag-drop
    return S_OK;
}

HRESULT __stdcall CDropSource::GiveFeedback(DWORD dwEffect)
{
    return DRAGDROP_S_USEDEFAULTCURSORS;
}

HRESULT CreateDropSource(IDropSource **pDropSource)
{
    if (pDropSource == NULL) { return E_INVALIDARG; }

    *pDropSource = new CDropSource();

    return (*pDropSource) ? S_OK : E_OUTOFMEMORY;
}

}
