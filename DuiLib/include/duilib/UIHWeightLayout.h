#ifndef __UIHWEIGHTLAYOUT_H__
#define __UIHWEIGHTLAYOUT_H__

#pragma once

namespace DuiLib {
class DUILIB_API CHWeightLayoutUI : public CHorizontalLayoutUI
{
public:
    CHWeightLayoutUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;

protected:
    void ResetWeightCtrlState(void);    // 带权重且被隐藏的控件，重置其 InternVisible 属性
    bool HideMinHeightCtrl(int nIndex); // 隐藏指定索引位置的控件

};
}
#endif // __UIHWEIGHTLAYOUT_H__
