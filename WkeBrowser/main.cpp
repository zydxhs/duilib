#include "UIlib.h"

//#define DUILIB_API
//#include "wke_dll.h"
//#include "UIWkeBrowser.h"

#pragma comment(lib, "imm32.lib")
using namespace DuiLib;


class CMainDlg : public CWndImplBase
{
public:
    virtual void Notify(TNotifyUI &msg);

protected:
    virtual LPCTSTR GetWindowClassName(void) const { return _T("WkeBrowser"); }
    virtual CDuiString GetSkinFolder() { return _T("skin\\WkeBrowser"); }
    virtual CDuiString GetSkinFile() { return _T("WkeBrowser.xml"); }
    virtual UILIB_RESOURCETYPE GetResourceType() const { return UILIB_FILE; }
    virtual void OnFinalMessage(HWND hWnd)
    {
        CWndImplBase::OnFinalMessage(hWnd);
        PostQuitMessage(0);
    }
    virtual void OnInitWindow(void);
    virtual void OnPrepare(void);
    //virtual CControlUI *CreateControl(LPCTSTR pstrClass)
    //{
    //    if (_tcscmp(pstrClass, _T("WkeBrowser")) == 0)
    //    {
    //        m_pWke = new CWkeBrowserUI;
    //    }
    //
    //    return m_pWke;
    //}

private:
    CWkeBrowserUI   *m_pWke = NULL;
    CEditUI         *m_pURLEdit;
    CLabelUI        *m_pLblStatusBar;
};

jsValue JS_CALL js_msgBox(jsExecState es)
{
    int n = jsArgCount(es);
    jsType t0 = jsArgType(es, 0);
    jsType t1 = jsArgType(es, 1);
    const wchar_t *text = jsToTempStringW(es, jsArgValue(es, 0));
    const wchar_t *title = jsToTempStringW(es, jsArgValue(es, 1));

    return jsUndefined(es);
}

void CMainDlg::OnInitWindow(void)
{
    CWndImplBase::OnInitWindow();
    m_pURLEdit = static_cast<CEditUI *>(m_pm.FindControl(_T("urlEdit")));
    m_pLblStatusBar = static_cast<CLabelUI *>(m_pm.FindControl(_T("statusbar")));

    m_pWke = static_cast<CWkeBrowserUI *>(m_pm.FindControl(_T("ie")));

    if (m_pWke)
    {
        jsBindFunction("msgBox", js_msgBox, 2);//这里绑定js函数，让js主动调用c++函数
    }
}

void CMainDlg::Notify(TNotifyUI &msg)
{
    if (msg.sType == DUI_MSGTYPE_CLICK)
    {
        if (msg.pSender->GetName() == _T("btnBack"))
        {
            m_pWke->GoBack();
        }
        else if (msg.pSender->GetName() == _T("btnforward"))
        {
            m_pWke->GoForward();
        }
        else if (msg.pSender->GetName() == _T("btnGo"))
        {
            m_pWke->LoadUrl(m_pURLEdit->GetText().GetData());
        }
        else if (msg.pSender->GetName() == _T("btnRefresh"))
        {
            wstring strRet = m_pWke->RunJS(m_pURLEdit->GetText().GetData());
            MessageBox(m_hWnd, strRet.c_str(), _T(""), 0);
        }
    }
    else if (msg.sType == DUI_MSGTYPE_WEBNOTIFY)
    {
        if (CWkeBrowserUI::ENTY_URL == msg.wParam)
        {
            m_pURLEdit->SetText(m_pWke->GetURL());
        }

        msg.dwTimestamp = 12;
    }

    CWndImplBase::Notify(msg);
}



void CMainDlg::OnPrepare(void)
{
    //m_pWke->LoadFile(_T("file:///d:/CodeBlocks/sdk/duilib/bin/Html/index.html"));
    m_pWke->LoadUrl(_T("http://www.baidu.com"));
}

static CMainDlg *s_pDlg = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetCurrentPath(CPaintManagerUI::GetInstancePath());

    s_pDlg = new CMainDlg;
    s_pDlg->Create(NULL, _T("Redrain wke demo"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
    s_pDlg->CenterWindow();
    s_pDlg->ShowWindow(true);
    CPaintManagerUI::MessageLoop();
    CPaintManagerUI::Term();

    return 0;
}
