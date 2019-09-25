#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once

struct IOleObject;


namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

class CActiveXCtrl;

template< class T >
class CSafeRelease
{
public:
    CSafeRelease(T *p) : m_p(p) { };
    ~CSafeRelease() { if (m_p != NULL) m_p->Release(); };
    T *Detach() { T *t = m_p; m_p = NULL; return t; };
    T *m_p;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CActiveXUI : public CControlUI, public IMessageFilterUI
{
    friend class CActiveXCtrl;
public:
    CActiveXUI();
    virtual ~CActiveXUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void DoInit() override;
    virtual UINT GetControlFlags() const override;
    virtual HWND GetNativeWindow() const override;

    bool IsDelayCreate() const;
    void SetDelayCreate(bool bDelayCreate = true);

    void SetCLSID(const LPCTSTR pstrCLSID);
    HRESULT GetControl(const IID iid, LPVOID *ppRet);
    CLSID GetClisd() const;
    CDuiString GetModuleName() const;
    void SetModuleName(LPCTSTR pstrText);

    virtual bool SetVisible(bool bVisible = true) override;
    virtual void SetInternVisible(bool bVisible = true) override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;

    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

protected:
    virtual void ReleaseControl();
    virtual bool DoCreateControl();
    bool CreateControl(void);
    bool CreateControl(LPCTSTR pstrCLSID);

protected:
    CLSID m_clsid;
    CDuiString m_sModuleName;
    bool m_bCreated;
    bool m_bDelayCreate;
    IOleObject *m_pUnk;
    CActiveXCtrl *m_pControl;
    HWND m_hwndHost;
};

} // namespace DuiLib

#endif // __UIACTIVEX_H__
