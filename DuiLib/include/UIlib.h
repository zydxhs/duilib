// Copyright (c) 2010-2011, duilib develop team(www.duilib.com).
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted provided that the
// following conditions are met.
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials
// provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
// CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#ifndef __UILIB_H__
#define __UILIB_H__

#ifdef UILIB_STATIC
    #define DUILIB_API
#else
    #if defined(UILIB_EXPORTS)
        #if defined(_MSC_VER)
            #define DUILIB_API __declspec(dllexport)
        #else
            #define DUILIB_API
        #endif
    #else
        #if defined(_MSC_VER)
            #define DUILIB_API __declspec(dllimport)
        #else
            #define DUILIB_API
        #endif
    #endif
#endif

//µ¼Èë¿â
#if defined(_MSC_VER) && !defined(UILIB_STATIC)
    #ifndef UILIB_EXPORTS
        #ifdef _DEBUG
            #ifdef _UNICODE
                #pragma comment(lib, "duilib_ud.lib")
            #else
                #pragma comment(lib, "duilib_d.lib")
            #endif
        #else
            #ifdef _UNICODE
                #pragma comment(lib, "duilib_u.lib")
            #else
                #pragma comment(lib, "duilib.lib")
            #endif
        #endif
    #endif
#endif //_MSC_VER

#ifdef UILIB_STATIC
    #define INLINE
#else
    #define INLINE inline
#endif // UILIB_STATIC

#define UILIB_COMDAT __declspec(selectany)

#if defined _M_IX86
    #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
    #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
    #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
    #pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <stddef.h>
#include <richedit.h>
#include <tchar.h>
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <stdio.h>


#ifdef USE_GDIPLUS
    #include <GdiPlus.h>
    #pragma comment( lib, "GdiPlus.lib" )
    using namespace Gdiplus;
    class DUILIB_API Gdiplus::RectF;
#endif // USE_GDIPLUS

#include "duilib/Utils.h"
#include "duilib/UIDelegate.h"
#include "duilib/UIDefine.h"
#include "duilib/UIManager.h"
#include "duilib/UIBase.h"
#include "duilib/UIControl.h"
#include "duilib/UIContainer.h"
#include "duilib/UIMarkup.h"
#include "duilib/UIDlgBuilder.h"
#include "duilib/UIRender.h"
#include "duilib/WinImplBase.h"
#include "duilib/Subject.h"

#include "duilib/UIVerticalLayout.h"
#include "duilib/UIHorizontalLayout.h"
#include "duilib/UITileLayout.h"
#include "duilib/UITabLayout.h"
#include "duilib/UIChildLayout.h"
#include "duilib/UIHWeightLayout.h"

#include "duilib/UIList.h"
#include "duilib/UICombo.h"
#include "duilib/UIScrollBar.h"
#include "duilib/UITreeView.h"

#include "duilib/UILabel.h"
#include "duilib/UIText.h"
#include "duilib/UIEdit.h"

#include "duilib/UIButton.h"
#include "duilib/UIOption.h"
#include "duilib/UICheckBox.h"
#include "duilib/UIRadioBox.h"

#include "duilib/UIProgress.h"
#include "duilib/UISlider.h"

#include "duilib/UIRichEdit.h"
#include "duilib/UIDateTime.h"

#include "duilib/UIActiveX.h"
#include "duilib/UIFlash.h"
//#include "duilib/WebBrowserEventHandler.h"
//#include "duilib/UIWebBrowser.h"
#include "duilib/UIGifAnim.h"

#include "duilib/UIMenu.h"
#include "duilib/UIHotKey.h"
#include "duilib/UIPwdCheck.h"
#include "duilib/UIIPAddress.h"

#include "duilib/OleDataHelper.h"

#endif //__UILIB_H__
