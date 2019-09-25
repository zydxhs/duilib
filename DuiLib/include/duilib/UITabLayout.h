#ifndef __UITABLAYOUT_H__
#define __UITABLAYOUT_H__

#pragma once

namespace DuiLib {
class DUILIB_API CTabLayoutUI : public CContainerUI
{
public:
    CTabLayoutUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual bool Add(CControlUI *pControl) override;
    virtual bool AddAt(CControlUI *pControl, int iIndex) override;
    virtual bool Remove(CControlUI *pControl, bool bDoNotDestroy = false) override;
    virtual void RemoveAll() override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    int GetCurSel() const;
    bool SelectItem(int iIndex, bool bTriggerEvent = true);
    bool SelectItem(CControlUI *pControl,  bool bTriggerEvent = true);

protected:
    int m_iCurSel;
};
}
#endif // __UITABLAYOUT_H__
