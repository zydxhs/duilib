#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib {
class DUILIB_API CTextUI : public CLabelUI
{
public:
    CTextUI();
    virtual ~CTextUI();

    virtual LPCTSTR GetClass() const override;
    virtual UINT GetControlFlags() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual int *GetLinksNum(void) override;
    CDuiString *GetLinkContent(int iIndex);

    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual void PaintText(HDC hDC) override;

protected:
    virtual RECT *GetRectLinks(void) override;
    virtual CDuiString *GetStringLinks(void) override;

protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
    RECT m_rcLinks[MAX_LINK];
    CDuiString m_sLinks[MAX_LINK];
    int m_nHoverLink;
};

} // namespace DuiLib

#endif //__UITEXT_H__
