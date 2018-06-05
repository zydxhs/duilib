#ifndef __UICHILDLAYOUT_H__
#define __UICHILDLAYOUT_H__

#pragma once

namespace DuiLib {
// 不推荐使用。可以用 Include 标签代替
class DUILIB_API CChildLayoutUI : public CContainerUI
{
public:
    CChildLayoutUI();

    void Init();
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void SetChildLayoutXML(CDuiString pXML);
    CDuiString GetChildLayoutXML();
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;

private:
    CDuiString m_pstrXMLFile;
};
} // namespace DuiLib
#endif // __UICHILDLAYOUT_H__
