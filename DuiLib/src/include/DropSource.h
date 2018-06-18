/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file dropsource.h
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
#pragma once
#include "oleidl.h"

namespace DuiLib {

HRESULT CreateDropSource(IDropSource **pDropSource);

class CDropSource : public IDropSource
{
public:
    CDropSource(void);
    virtual ~CDropSource(void);

    virtual HRESULT __stdcall QueryInterface(REFIID iid, void **ppvObject);
    virtual ULONG   __stdcall AddRef(void);
    virtual ULONG   __stdcall Release(void);

    virtual HRESULT __stdcall QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState);
    virtual HRESULT __stdcall GiveFeedback(DWORD dwEffect);
protected:
private:
    LONG       m_lRefCount;
};
}
