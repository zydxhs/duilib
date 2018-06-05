//
// main_frame.cpp
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2011 achellies (achellies at 163 dot com)
//
// This code may be used in compiled form in any way you desire. This
// source file may be redistributed by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name is included.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#include "stdafx.h"
#include <windows.h>
#if !defined(UNDER_CE)
    #include <shellapi.h>
#endif

#include "resource.h"
#include "main_frame.hpp"

const TCHAR *const kTitleControlName = _T("apptitle");
const TCHAR *const kCloseButtonControlName = _T("btnclose");
const TCHAR *const kMinButtonControlName = _T("btnmin");
const TCHAR *const kMaxButtonControlName = _T("btnmax");
const TCHAR *const kRestoreButtonControlName = _T("btnrestore");

MainFrame::MainFrame()
{}

MainFrame::~MainFrame()
{
    PostQuitMessage(0);
}

LPCTSTR MainFrame::GetWindowClassName() const
{
    return _T("TXGuiFoundation");
}

CControlUI *MainFrame::CreateControl(LPCTSTR pstrClass)
{
    return NULL;
}

void MainFrame::OnFinalMessage(HWND hWnd)
{
    CWndImplBase::OnFinalMessage(hWnd);
    delete this;
}

CDuiString MainFrame::GetSkinFile()
{
    TCHAR szBuf[MAX_PATH] = {0};
    _stprintf_s(szBuf, MAX_PATH - 1, _T("%d"), IDR_SKINXML);
    return szBuf;
}

CDuiString MainFrame::GetSkinFolder()
{
    return _T("");
}

UILIB_RESOURCETYPE MainFrame::GetResourceType() const
{
    return UILIB_RESOURCE;
}

LRESULT MainFrame::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
    BOOL bZoomed = ::IsZoomed(m_hWnd);
    LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);

    if (::IsZoomed(m_hWnd) != bZoomed)
    {
        if (!bZoomed)
        {
            CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(kMaxButtonControlName));

            if (pControl) { pControl->SetVisible(false); }

            pControl = static_cast<CControlUI *>(m_pm.FindControl(kRestoreButtonControlName));

            if (pControl) { pControl->SetVisible(true); }
        }
        else
        {
            CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(kMaxButtonControlName));

            if (pControl) { pControl->SetVisible(true); }

            pControl = static_cast<CControlUI *>(m_pm.FindControl(kRestoreButtonControlName));

            if (pControl) { pControl->SetVisible(false); }
        }
    }

#else
    return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

    return 0;
}

LRESULT MainFrame::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT MainFrame::ResponseDefaultKeyEvent(WPARAM wParam)
{
    if (wParam == VK_RETURN)
    {
        return FALSE;
    }
    else if (wParam == VK_ESCAPE)
    {
        return TRUE;
    }

    return FALSE;
}

void MainFrame::OnTimer(TNotifyUI &msg)
{
}

void MainFrame::OnExit(TNotifyUI &msg)
{
    Close();
}

void MainFrame::InitWindow()
{}

void MainFrame::OnPrepare(TNotifyUI &msg)
{}

void MainFrame::Notify(TNotifyUI &msg)
{
    if (_tcsicmp(msg.sType, DUI_MSGTYPE_WINDOWINIT) == 0)
    {
        OnPrepare(msg);
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_CLICK) == 0)
    {
        if (_tcsicmp(msg.pSender->GetName(), kCloseButtonControlName) == 0)
        {
            OnExit(msg);
        }
        else if (_tcsicmp(msg.pSender->GetName(), kMinButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MINIMIZE);
#else
            SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), kMaxButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_MAXIMIZE);
            CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(kMaxButtonControlName));

            if (pControl) { pControl->SetVisible(false); }

            pControl = static_cast<CControlUI *>(m_pm.FindControl(kRestoreButtonControlName));

            if (pControl) { pControl->SetVisible(true); }

#else
            SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), kRestoreButtonControlName) == 0)
        {
#if defined(UNDER_CE)
            ::ShowWindow(m_hWnd, SW_RESTORE);
            CControlUI *pControl = static_cast<CControlUI *>(m_pm.FindControl(kMaxButtonControlName));

            if (pControl) { pControl->SetVisible(true); }

            pControl = static_cast<CControlUI *>(m_pm.FindControl(kRestoreButtonControlName));

            if (pControl) { pControl->SetVisible(false); }

#else
            SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
#endif
        }
        else if (_tcsicmp(msg.pSender->GetName(), _T("btn_menu")) == 0)
        {
            CDuiPoint point = msg.ptMouse;
            ClientToScreen(m_hWnd, &point);
            STRINGorID xml(IDR_XML_MENU);
            CMenuWnd *pMenu = CMenuWnd::CreateMenu(NULL, xml, _T("xml"), point, &m_pm);

            if (NULL != pMenu)
            {
                CMenuUI *pRootMenu = pMenu->GetMenuUI();

                // 添加一级菜单项
                CMenuElementUI *pNew = pRootMenu->NewMenuItem();
                pNew->SetName(_T("Menu_Dynamic_1"));
                pNew->SetText(_T("动态一级菜单"));
                pRootMenu->Add(pNew);

                // 添加二级菜单项
                CMenuElementUI *pItem1 = pRootMenu->FindMenuItem(_T("menu1"));
                pNew = pItem1->NewMenuItem();
                pNew->SetName(_T("Menu_Dynamic_2"));
                pNew->SetText(_T("动态二级菜单_1"));
                pItem1->Add(pNew);

                // 添加二级菜单项：原父菜单项没有子菜单
                CMenuElementUI *pItem4 = pRootMenu->FindMenuItem(_T("menu4"));
                pItem4->SetExpandable(true);    // 添加可展开属性
                pNew = pItem4->NewMenuItem();
                pNew->SetName(_T("Menu_Dynamic_3"));
                pNew->SetText(_T("动态二级菜单_3"));
                pItem4->Add(pNew);

            }

            pMenu->ResizeMenu();
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_TIMER) == 0)
    {
        return OnTimer(msg);
    }
    else if (msg.sType == DUI_MSGTYPE_MENUITEM_CLICK)
    {
        CDuiString strTxt;
        strTxt.Format(_T("你单击了菜单项：%s"), CMenuWnd::s_strName.GetData());
        MessageBox(GetHWND(), strTxt, _T("提示"), MB_OK);
    }
}

LRESULT MainFrame::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    return 0;
}