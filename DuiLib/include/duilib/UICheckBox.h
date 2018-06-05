#ifndef __UICHECKBOX_H__
#define __UICHECKBOX_H__

#pragma once

namespace DuiLib {
/// 最普通的复选按钮控件，只有是、否两种结果，可以用 COptionUI 代替，不添加 group 属性
/// 派生于COptionUI，只是每组只有一个按钮而已，组名为空，配置文件默认属性举例：
/// <CheckBox name="chbTest" value="height='20' align='left' textpadding='24,0,0,0'
///  selnormalimg="file='chb_sel_nor.png' dest='0,4,16,20'" selhotimg="file='chb_sel_hover.png' dest='0,4,16,20'"
///  selfocusedimg="file='chb_sel_hover.png' dest='0,4,16,20'" selpushedimg="file='chb_sel_down.png' dest='0,4,16,20'"
///  seldisabledimg="file='chb_sel_disable.png' dest='0,4,16,20'" unselnormalimg="file='chb_unsel_nor.png' dest='0,4,16,20'"
///  unselhotimg="file='chb_unsel_hover.png' dest='0,4,16,20'" unselfocusedimg="file='chb_unsel_hover.png' dest='0,4,16,20'"
///  unselpushedimg="file='chb_unsel_down.png' dest='0,4,16,20'" unseldisabledimg="file='chb_unsel_disable.png' dest='0,4,16,20'" />

class DUILIB_API CCheckBoxUI : public COptionUI
{
public:
    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetCheck(bool bCheck, bool bTriggerEvent = true);
    bool GetCheck() const;

    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

};

}

#endif // __UICHECKBOX_H__
