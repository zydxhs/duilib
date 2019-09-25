#ifndef __UIRADIOBUTTON_H__
#define __UIRADIOBUTTON_H__
#pragma once

namespace DuiLib {
/// 最普通的单选按钮控件，只有是、否两种结果，可以用 COptionUI 代替，添加 group 属性
/// 派生于COptionUI，只是每组只有一个按钮而已，组名为空，配置文件默认属性举例：
/// <RadioBtn name="rdTest" value="height='20' align='left' textpadding='24,0,0,0'
///  selnormalimage="file='chb_sel_nor.png' dest='0,4,16,20'" selhotimage="file='chb_sel_hover.png' dest='0,4,16,20'"
///  selfocusedimage="file='chb_sel_hover.png' dest='0,4,16,20'" selpushedimage="file='chb_sel_down.png' dest='0,4,16,20'"
///  seldisabledimage="file='chb_sel_disable.png' dest='0,4,16,20'" unselnormalimage="file='chb_unsel_nor.png' dest='0,4,16,20'"
///  unselhotimage="file='chb_unsel_hover.png' dest='0,4,16,20'" unselfocusedimage="file='chb_unsel_hover.png' dest='0,4,16,20'"
///  unselpushedimage="file='chb_unsel_down.png' dest='0,4,16,20'" unseldisabledimage="file='chb_unsel_disable.png' dest='0,4,16,20'" />

class DUILIB_API CRadioBoxUI : public COptionUI
{
public:
    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

};

}

#endif // __UIRADIOBUTTON_H__
