// App.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "flash11.tlh"

class CFrameWnd : public CWndImplBase
{
public:
    CFrameWnd() { };
    LPCTSTR GetWindowClassName() const { return _T("UIFrame"); };
    UINT GetClassStyle() const { return UI_CLASSSTYLE_DIALOG; };
    virtual CDuiString GetSkinFolder() { return _T(""); }
    virtual CDuiString GetSkinFile() { return _T("ui.xml"); }
    void OnFinalMessage(HWND /*hWnd*/)
    {
        delete this;
        PostQuitMessage(0);
    };

    void OnInitWindow()
    {
        CWndImplBase::OnInitWindow();
    }

    void Notify(TNotifyUI &msg)
    {
        if (msg.sType == _T("showactivex"))
        {
            if (msg.pSender->GetName() != _T("flash")) { return; }

            ShockwaveFlashObjects::IShockwaveFlash *pFlash = NULL;
            //#define FLASH
#ifdef FLASH
            CFlashUI *pCtrl = static_cast<CFlashUI *>(msg.pSender);
            pFlash = pCtrl->GetShockwaveFlash();
#else
            CActiveXUI *pActiveX = static_cast<CActiveXUI *>(msg.pSender);
            pActiveX->GetControl(IID_IUnknown, (void **)&pFlash);
#endif

            if (pFlash != NULL)
            {
                pFlash->put_WMode(_bstr_t(_T("Transparent")));
                pFlash->put_Movie(_bstr_t(CPaintManagerUI::GetInstancePath() + _T("\\skin\\FlashRes\\test.swf")));
                pFlash->DisableLocalSecurity();
                pFlash->put_AllowScriptAccess(L"always");
                BSTR response;
                pFlash->CallFunction(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>",
                                     &response);
#ifndef FLASH
                pFlash->Release();
#endif
            }

            return;
        }

        CWndImplBase::Notify(msg);
    }

public:
    CPaintManagerUI m_pm;
};


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
    CPaintManagerUI::SetInstance(hInstance);
    CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin\\FlashRes"));

    CFrameWnd *pFrame = new CFrameWnd();

    if (pFrame == NULL) { return 0; }

    pFrame->Create(NULL, NULL, UI_WNDSTYLE_DIALOG, 0);
    pFrame->CenterWindow();
    pFrame->ShowWindow(true);
    CPaintManagerUI::MessageLoop();
    CPaintManagerUI::Term();

    return 0;
}
