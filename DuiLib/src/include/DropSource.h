/// \copyright Copyright(c) 2018, SuZhou Keda Technology Co., All rights reserved.
/// \file dropsource.h
/// \brief �Ϸ�Դ
///
///
/// \author zhuyadong
/// Contact: zhuyadong@kedacom.com
/// \date 2018-04-27
/// \note
/// -----------------------------------------------------------------------------
/// �޸ļ�¼��
/// ��  ��        �汾        �޸���        �߶���    �޸�����
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