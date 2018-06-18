/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file droptarget.h
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
#pragma once
#include <OleIdl.h>
#include <Shobjidl.h>

namespace DuiLib {

class CDropTarget : public IDropTarget
{
public:
    CDropTarget(void);

    bool DragDropRegister(IDuiDropTarget *pDuiDropTarget, HWND hWnd, DWORD AcceptKeyState = MK_LBUTTON);
    bool DragDropRevoke(HWND hWnd);
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, __RPC__deref_out void **ppvObject);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();
    //进入
    HRESULT STDMETHODCALLTYPE DragEnter(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt,
                                        __RPC__inout DWORD *pdwEffect);
    //移动
    HRESULT STDMETHODCALLTYPE DragOver(DWORD grfKeyState, POINTL pt, __RPC__inout DWORD *pdwEffect);
    //离开
    HRESULT STDMETHODCALLTYPE DragLeave();
    //释放
    HRESULT STDMETHODCALLTYPE Drop(__RPC__in_opt IDataObject *pDataObj, DWORD grfKeyState, POINTL pt,
                                   __RPC__inout DWORD *pdwEffect);

private:
    ~CDropTarget(void);

    HWND                m_hWnd;
    IDropTargetHelper  *m_piDropHelper;
    bool                m_bUseDnDHelper;
    DWORD               m_dAcceptKeyState;
    ULONG               m_lRefCount;
    IDuiDropTarget     *m_pDuiDropTarget;
};

}
