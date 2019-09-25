#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib {
// 不推荐使用。可以用 Include 标签代替
class DUILIB_API CChildLayoutUI : public CContainerUI
{
public:
    CChildLayoutUI();

    virtual void Init() override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual LPCTSTR GetClass() const override;

    void SetChildLayoutXML(CDuiString pXML);
    CDuiString GetChildLayoutXML();

private:
    CDuiString m_pstrXMLFile;
};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__
