#ifndef __WKE_BROWSER_H
#define __WKE_BROWSER_H
#pragma once


namespace DuiLib {

class DUILIB_API CWkeBrowserUI : public CControlUI
{
public:
    enum
    {
        ENTY_TITLE      = 1,    // 网页标题改变
        ENTY_URL,               // 网页URL 改变
        ENTY_ALERTBOX,          // 网页弹框 提醒
        ENTY_CONFIRMBOX,        // 网页弹框 确认
        ENTY_PROMPTBOX,         // 网页弹框 提示
    };
public:
    CWkeBrowserUI(void);
    virtual ~CWkeBrowserUI(void);

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;
    virtual void DoInit() override;


    void LoadUrl(LPCTSTR szUrl);
    void LoadFile(LPCTSTR szFile);
    void Reload(void);

    CDuiString RunJS(LPCTSTR strValue);
    bool CanGoBack();
    void GoBack();
    bool CanGoForward();
    void GoForward();

    const CDuiString &GetTitle(void) { return m_sTitle; }
    const CDuiString &GetURL(void) { return m_sURL; }

    //ENTY_ALERTBOX ENTY_CONFIRMBOX ENTY_PROMPTBOX
    const CDuiString &GetMsg(void) { return m_sMsg; }
    // ENTY_PROMPTBOX
    const CDuiString &GetDefRet(void) { return m_sDefRet; }
    const CDuiString &GetRet(void) { return m_sRet; }


    bool SendNotify(void *pWebView, int nFlag, LPCTSTR sMsg, LPCTSTR sDefRet = NULL, LPCTSTR sRet = NULL);
protected:
    void InitBrowser(void);
    void PaintWebContent(HDC hDC, const RECT &rcPaint);

protected:
    wkeWebView  m_pWeb;

    // 回调通知数据
    void       *m_pView;    //
    CDuiString  m_sTitle;   // 当前网页标题
    CDuiString  m_sURL;     // 当前网页标题
    CDuiString  m_sMsg;
    CDuiString  m_sDefRet;
    CDuiString  m_sRet;

};

}

#endif // __WKE_BROWSER_H

