#ifndef __UICHECKBOX_H__
#define __UICHECKBOX_H__

#pragma once

namespace DuiLib {
/// 最普通的复选按钮控件，只有是、否两种结果，可以用 COptionUI 代替，不添加 group 属性
/// 派生于COptionUI，只是每组只有一个按钮而已，组名为空，配置文件默认属性举例：
/// <CheckBox name="chbTest" value="height='20' align='left' textpadding='24,0,0,0'
///  selnormalimage="file='chb_sel_nor.png' dest='0,4,16,20'" selhotimage="file='chb_sel_hover.png' dest='0,4,16,20'"
///  selfocusedimage="file='chb_sel_hover.png' dest='0,4,16,20'" selpushedimage="file='chb_sel_down.png' dest='0,4,16,20'"
///  seldisabledimage="file='chb_sel_disable.png' dest='0,4,16,20'" unselnormalimage="file='chb_unsel_nor.png' dest='0,4,16,20'"
///  unselhotimage="file='chb_unsel_hover.png' dest='0,4,16,20'" unselfocusedimage="file='chb_unsel_hover.png' dest='0,4,16,20'"
///  unselpushedimage="file='chb_unsel_down.png' dest='0,4,16,20'" unseldisabledimage="file='chb_unsel_disable.png' dest='0,4,16,20'" />

class DUILIB_API CCheckBoxUI : public COptionUI
{
public:
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    void SetCheck(bool bCheck, bool bTriggerEvent = true);
    bool GetCheck() const;

    void SetSelText(LPCTSTR pstrValue);
    CDuiString GetSelText(void);

    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void PaintText(HDC hDC) override;
    virtual void ReloadText(void) override;

protected:
    CDuiString  m_sSelText;     // 选中状态显示的文本。用于状态切换时自动改变显示文本。
    CDuiString  m_sSelTextOrig;
};

}

#endif // __UICHECKBOX_H__
