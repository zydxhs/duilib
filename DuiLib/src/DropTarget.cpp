/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file droptarget.cpp
/// \brief 拖放目标
///
///
/// \author zhuyadong@kedacom.com
/// \date 2018-04-26
/// \note
/// -----------------------------------------------------------------------------
/// 修改记录：
/// 日  期        版本        修改人        走读人    修改内容
///
#include "stdafx.h"
#include <ShlGuid.h>
#include <Shlwapi.h>
#include "droptarget.h"

namespace DuiLib {

CDropTarget::CDropTarget(void)
    : m_hWnd(NULL)
    , m_piDropHelper(NULL)
    , m_bUseDnDHelper(NULL)
    , m_dAcceptKeyState(0)
    , m_lRefCount(0)
    , m_pDuiDropTarget(NULL)
{
    m_lRefCount = 1;

    // Create an instance of the shell DnD helper object.
    if (SUCCEEDED(CoCreateInstance(CLSID_DragDropHelper, NULL,
                                   CLSCTX_INPROC_SERVER,
                                   IID_IDropTargetHelper,
                                   (void **)&m_piDropHelper)))
    {
        m_bUseDnDHelper = true;
    }
}


CDropTarget::~CDropTarget(void)
{
    if (m_piDropHelper) { m_piDropHelper->Release(); }

    m_bUseDnDHelper = false;
    m_lRefCount = 0;
}

bool CDropTarget::DragDropRegister(IDuiDropTarget *pDuiDropTarget, HWND hWnd, DWORD AcceptKeyState)
{

    if (!IsWindow(hWnd)) { return false; }

    m_pDuiDropTarget = pDuiDropTarget;
    m_dAcceptKeyState = AcceptKeyState;
    HRESULT s = ::RegisterDragDrop(hWnd, this);
    m_hWnd = hWnd;
    return SUCCEEDED(s) ? true : false;
}

bool CDropTarget::DragDropRevoke(HWND hWnd)
{
    if (!IsWindow(hWnd)) { return false; }

    HRESULT s = ::RevokeDragDrop(hWnd);
    return SUCCEEDED(s);
}

HRESULT STDMETHODCALLTYPE CDropTarget::QueryInterface(REFIID riid, __RPC__deref_out void **ppvObject)
{
    static const QITAB qit[ ] =
    {
        QITABENT(CDropTarget, IDropTarget), { 0 }
    };

    return QISearch(this, qit, riid, ppvObject);
}

ULONG STDMETHODCALLTYPE CDropTarget::AddRef()
{
    return InterlockedIncrement(&m_lRefCount);
}

ULONG STDMETHODCALLTYPE CDropTarget::Release()
{
    ULONG lRef = InterlockedDecrement(&m_lRefCount);

    if (0 == lRef) { delete this;    return 0; }

    return lRef;
}

//进入
HRESULT STDMETHODCALLTYPE CDropTarget::DragEnter(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState,
                                                 POINTL pt, __RPC__inout DWORD *pdwEffect)
{
    if (m_bUseDnDHelper) { m_piDropHelper->DragEnter(m_hWnd, pDataObj, (LPPOINT)&pt, *pdwEffect); }

    return m_pDuiDropTarget->OnDragEnter(pDataObj, grfKeyState, pt, pdwEffect);
}

//移动
HRESULT STDMETHODCALLTYPE CDropTarget::DragOver(DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect)
{
    if (m_bUseDnDHelper) { m_piDropHelper->DragOver((LPPOINT)&pt, *pdwEffect); }

    return m_pDuiDropTarget->OnDragOver(grfKeyState, pt, pdwEffect);
}

//离开
HRESULT STDMETHODCALLTYPE CDropTarget::DragLeave()
{
    if (m_bUseDnDHelper) { m_piDropHelper->DragLeave(); }

    return m_pDuiDropTarget->OnDragLeave();
}

//释放
HRESULT STDMETHODCALLTYPE CDropTarget::Drop(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState,
                                            POINTL pt, __RPC__inout DWORD *pdwEffect)
{
    m_piDropHelper->Drop(pDataObj, (LPPOINT)&pt, *pdwEffect);
    return m_pDuiDropTarget->OnDragDrop(pDataObj, grfKeyState, pt, pdwEffect);
}

}
