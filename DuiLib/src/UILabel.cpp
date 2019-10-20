#include "StdAfx.h"

namespace DuiLib {

#ifdef USE_GDIPLUS
static Color ARGB2Color(DWORD dwColor)
{
    return Color(HIBYTE((dwColor) >> 16), GetBValue(dwColor), GetGValue(dwColor), GetRValue(dwColor));
}
#endif // USE_GDIPLUS

CLabelUI::CLabelUI()
    : m_pWideText(0)
    , m_uTextStyle(DT_SINGLELINE | DT_VCENTER)
    , m_iFont(-1)
    , m_bShowHtml(false)
    , m_bNeedEstimateSize(true)
    , m_bEnableEffect(false)
#ifdef USE_GDIPLUS
    , m_bEnableLuminous(false)
    , m_fLuminousFuzzy(3)
    , m_dwTextColor1(-1)
    , m_dwTextShadowColorA(0xff000000)
    , m_dwTextShadowColorB(-1)
    , m_GradientAngle(0)
    , m_EnabledStroke(false)
    , m_dwStrokeColor(0)
    , m_EnabledShadow(false)
    , m_GradientLength(0)
#endif // USE_GDIPLUS
{
#ifdef USE_GDIPLUS
    m_ShadowOffset.X        = 0.0f;
    m_ShadowOffset.Y        = 0.0f;
    m_ShadowOffset.Width    = 0.0f;
    m_ShadowOffset.Height   = 0.0f;
#endif // USE_GDIPLUS
    m_cxyFixedLast.cx = m_cxyFixedLast.cy = 0;
    m_szAvailableLast.cx = m_szAvailableLast.cy = 0;
}

CLabelUI::~CLabelUI()
{
#ifdef _UNICODE

    if (m_pWideText && m_pWideText != m_sText.GetData()) { delete[] m_pWideText; }

#else

    if (m_pWideText) { delete[] m_pWideText; }

#endif // _UNICODE
}

LPCTSTR CLabelUI::GetClass() const
{
    return DUI_CTR_LABEL;
}

LPVOID CLabelUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_LABEL) == 0) { return static_cast<CLabelUI *>(this); }

    return CControlUI::GetInterface(pstrName);
}

void CLabelUI::SetFixedWidth(int cx)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetFixedWidth(cx);
}

void CLabelUI::SetFixedHeight(int cy)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetFixedHeight(cy);
}

void CLabelUI::SetText(LPCTSTR pstrText)
{
    m_bNeedEstimateSize = true;
    CControlUI::SetText(pstrText);

    if (m_bAutoWidth) { NeedParentUpdate(); }

    if (m_bEnableEffect)
    {
#ifdef USE_GDIPLUS
#ifdef _UNICODE
        m_pWideText = (LPWSTR)m_sText.GetData();
#else
        int iLen = _tcslen(pstrText);

        if (m_pWideText) { delete[] m_pWideText; }

        m_pWideText = new WCHAR[iLen + 1];
        ::ZeroMemory(m_pWideText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, pstrText, -1, (LPWSTR)m_pWideText, iLen);
#endif // _UNICODE
#endif // USE_GDIPLUS
    }
}

void CLabelUI::SetTextStyle(UINT uStyle)
{
    m_uTextStyle = uStyle;
    m_bNeedEstimateSize = true;

    if (m_bAutoWidth) { NeedParentUpdate(); }

    Invalidate();
}

UINT CLabelUI::GetTextStyle() const
{
    return m_uTextStyle;
}

bool CLabelUI::IsMultiLine()
{
    return (m_uTextStyle & DT_SINGLELINE) == 0;
}

void CLabelUI::SetMultiLine(bool bMultiLine)
{
    if (bMultiLine)
    {
        m_uTextStyle  &= ~DT_SINGLELINE;
        m_uTextStyle |= DT_WORDBREAK;
    }
    else { m_uTextStyle |= DT_SINGLELINE; }

    m_bNeedEstimateSize = true;
}

void CLabelUI::SetFont(int index)
{
    m_iFont = index;
    m_bNeedEstimateSize = true;
    Invalidate();

    if (m_bAutoWidth) { NeedParentUpdate(); }
}

int CLabelUI::GetFont() const
{
    return m_iFont;
}

void CLabelUI::SetPadding(RECT rc)
{
    m_rcPadding = rc;
    m_bNeedEstimateSize = true;
    Invalidate();

    if (m_bAutoWidth) { NeedParentUpdate(); }
}

bool CLabelUI::IsShowHtml()
{
    return m_bShowHtml;
}

void CLabelUI::SetShowHtml(bool bShowHtml)
{
    if (m_bShowHtml == bShowHtml) { return; }

    m_bShowHtml = bShowHtml;
    m_bNeedEstimateSize = true;
    Invalidate();

    if (m_bAutoWidth) { NeedParentUpdate(); }
}

SIZE CLabelUI::EstimateSize(SIZE szAvailable)
{
    // if (m_cxyFixed.cx > 0 && m_cxyFixed.cy > 0) { return m_cxyFixed; }

    if ((m_uTextStyle & DT_SINGLELINE) == 0 &&
        (szAvailable.cx != m_szAvailableLast.cx || szAvailable.cy != m_szAvailableLast.cy))
    {
        m_bNeedEstimateSize = true;
    }

    if (m_bNeedEstimateSize)
    {
        m_bNeedEstimateSize = false;
        m_szAvailableLast = szAvailable;
        m_cxyFixedLast = m_cxyFixed;

        if ((m_uTextStyle & DT_SINGLELINE) != 0)
        {
            if (m_cxyFixedLast.cy == 0)
            {
                m_cxyFixedLast.cy = m_pManager->GetFontInfo(m_iFont)->tm.tmHeight + 8;
                m_cxyFixedLast.cy += m_rcPadding.top + m_rcPadding.bottom + m_rcBorderSize.top + m_rcBorderSize.bottom;
            }

            if (m_cxyFixedLast.cx == 0 || m_bAutoWidth)
            {
                RECT rcText = { 0, 0, MAX_CTRL_WIDTH, m_cxyFixedLast.cy };

                if (m_bShowHtml)
                {
                    CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, NULL,
                                                m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }
                else
                {
                    CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0,
                                            m_iFont,  DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
                }

                m_cxyFixedLast.cx = rcText.right - rcText.left + m_rcPadding.left + m_rcPadding.right;
                m_cxyFixedLast.cx += m_rcBorderSize.left + m_rcBorderSize.right;
                //if (m_bAutoWidth) { m_cxyFixed.cx = m_cxyFixedLast.cx; }
                //if (m_bAutoHeight)
                //{
                //    m_cxyFixedLast.cy = rcText.bottom - rcText.top + m_rcPadding.top + m_rcPadding.bottom;
                //    m_cxyFixed.cy = m_cxyFixedLast.cy;
                //}
            }

            m_cxyFixed.cx = m_bAutoWidth ? m_cxyFixedLast.cx : m_cxyFixed.cx;
        }
        else
        {
            m_cxyFixedLast.cx = (m_cxyFixedLast.cx == 0) ? szAvailable.cx : m_cxyFixedLast.cx;
            m_cxyFixedLast.cy = (m_cxyFixedLast.cy == 0) ? szAvailable.cy : m_cxyFixedLast.cy;

            RECT rcText = { 0, 0, m_cxyFixedLast.cx, MAX_CTRL_WIDTH };

            if (m_bShowHtml)
            {
                CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, NULL,
                                            m_iFont, DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }
            else
            {
                CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, m_iFont,
                                        DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER);
            }

            //m_cxyFixedLast.cx = rcText.right - rcText.left + m_rcTextPadding.left + m_rcTextPadding.right;
            m_cxyFixedLast.cy = rcText.bottom - rcText.top + m_rcPadding.top + m_rcPadding.bottom;
            m_cxyFixedLast.cy += m_rcBorderSize.top + m_rcBorderSize.bottom;

            m_cxyFixed.cy = m_bAutoHeight ? m_cxyFixedLast.cy : m_cxyFixed.cy;

            if (m_bAutoWidth)
            {
                m_cxyFixedLast.cx = rcText.right - rcText.left + m_rcPadding.left + m_rcPadding.right;
                m_cxyFixedLast.cx += m_rcBorderSize.left + m_rcBorderSize.right;
                m_cxyFixed.cx = m_cxyFixedLast.cx;
            }
        }
    }

    return m_cxyFixedLast;
}

void CLabelUI::DoEvent(TEventUI &event)
{
    if (event.Type == UIEVENT_SETFOCUS)
    {
        m_bFocused = true;
        return;
    }

    if (event.Type == UIEVENT_KILLFOCUS)
    {
        m_bFocused = false;
        return;
    }

    CControlUI::DoEvent(event);
}

void CLabelUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("align")) == 0)
    {
        CDuiString str = ParseString(pstrValue);

        if (str == _T("left"))
        {
            m_uTextStyle &= ~(DT_CENTER | DT_RIGHT);
            m_uTextStyle |= DT_LEFT;
        }
        else if (str == _T("center"))
        {
            m_uTextStyle &= ~(DT_LEFT | DT_RIGHT);
            m_uTextStyle |= DT_CENTER;
        }
        else if (str == _T("right"))
        {
            m_uTextStyle &= ~(DT_LEFT | DT_CENTER);
            m_uTextStyle |= DT_RIGHT;
        }
    }
    else if (_tcscmp(pstrName, _T("valign")) == 0)
    {
        CDuiString str = ParseString(pstrValue);

        if (str == _T("top"))
        {
            m_uTextStyle &= ~(DT_BOTTOM | DT_VCENTER);
            m_uTextStyle |= DT_TOP;
        }
        else if (str == _T("center"))
        {
            m_uTextStyle &= ~(DT_TOP | DT_BOTTOM);
            m_uTextStyle |= DT_VCENTER;
        }
        else  if (str == _T("bottom"))
        {
            m_uTextStyle &= ~(DT_TOP | DT_VCENTER);
            m_uTextStyle |= DT_BOTTOM;
        }
    }
    else if (_tcscmp(pstrName, _T("endellipsis")) == 0)
    {
        if (ParseBool(pstrValue)) { m_uTextStyle |= DT_END_ELLIPSIS; }
        else { m_uTextStyle &= ~DT_END_ELLIPSIS; }
    }
    else if (_tcscmp(pstrName, _T("font")) == 0) { SetFont(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("textpadding")) == 0 || _tcscmp(pstrName, _T("padding")) == 0)
    {
        RECT rt = ParseRect(pstrValue);
        SetPadding(rt);
    }
    else if (_tcscmp(pstrName, _T("multiline")) == 0) { SetMultiLine(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("showhtml")) == 0) { SetShowHtml(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("enabledeffect")) == 0) { SetEnabledEffect(ParseBool(pstrValue)); }

#ifdef USE_GDIPLUS
    else if (_tcscmp(pstrName, _T("enabledluminous")) == 0) { SetEnabledLuminous(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("luminousfuzzy")) == 0) { SetLuminousFuzzy(ParseFloat(pstrValue)); }
    else if (_tcscmp(pstrName, _T("gradientangle")) == 0) { SetGradientAngle(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("enabledstroke")) == 0) { SetEnabledStroke(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("enabledshadow")) == 0) { SetEnabledShadow(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("gradientlength")) == 0) { SetGradientLength(ParseInt(pstrValue)); }
    else if (_tcscmp(pstrName, _T("shadowoffset")) == 0)
    {
        SIZE sz = ParseSize(pstrValue);
        SetShadowOffset(sz.cx, sz.cy);
    }
    else if (_tcscmp(pstrName, _T("textcolor1")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetTextColor1(clr);
    }
    else if (_tcscmp(pstrName, _T("textshadowcolora")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetTextShadowColorA(clr);
    }
    else if (_tcscmp(pstrName, _T("textshadowcolorb")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetTextShadowColorB(clr);
    }
    else if (_tcscmp(pstrName, _T("strokecolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetStrokeColor(clr);
    }

#endif // USE_GDIPLUS
    else { CControlUI::SetAttribute(pstrName, pstrValue); }
}

void CLabelUI::DoInit()
{
    CControlUI::DoInit();

    // 2018-09-17 zhuyadong 默认字体颜色
    if (m_dwTextColor == 0 && NULL != m_pManager)
    {
        m_dwTextColor = m_pManager->GetDefaultFontColor();
    }

    if (m_dwDisabledTextColor == 0 && NULL != m_pManager)
    {
        m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor();
    }
}

void CLabelUI::PaintText(HDC hDC)
{
    if (m_dwTextColor == 0) { m_dwTextColor = m_pManager->GetDefaultFontColor(); }

    if (m_dwDisabledTextColor == 0) { m_dwDisabledTextColor = m_pManager->GetDefaultDisabledColor(); }

    RECT rc = m_rcItem;
    rc.left += (m_rcBorderSize.left + m_rcPadding.left);
    rc.right -= (m_rcBorderSize.right + m_rcPadding.right);
    rc.top += (m_rcBorderSize.top + m_rcPadding.top);
    rc.bottom -= (m_rcBorderSize.bottom + m_rcPadding.bottom);

    if (GetEnabledEffect())
    {
        PaintTextEffect(hDC, rc);
    }
    else
    {
        if (m_sText.IsEmpty()) { return; }

        if (!(m_uTextStyle & DT_SINGLELINE) && ((m_uTextStyle & DT_VCENTER) || (m_uTextStyle & DT_BOTTOM)))
        {
            RECT rcTxt = GetTextRect(rc);

            if (m_uTextStyle & DT_VCENTER)
            {
                rc.top += ((rc.bottom - rc.top) - (rcTxt.bottom - rcTxt.top)) / 2;
            }
            else if (m_uTextStyle & DT_BOTTOM)
            {
                rc.top += (rc.bottom - rc.top) - (rcTxt.bottom - rcTxt.top);
            }
        }

        if (IsEnabled())
        {
            if (m_bShowHtml)
                CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwTextColor,
                                            GetRectLinks(), GetStringLinks(), GetLinksNum(),
                                            m_iFont, m_uTextStyle);
            else
            {
                CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwTextColor,
                                        m_iFont, m_uTextStyle);
            }
        }
        else
        {
            if (m_bShowHtml)
                CRenderEngine::DrawHtmlText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor,
                                            GetRectLinks(), GetStringLinks(), GetLinksNum(),
                                            m_iFont, m_uTextStyle);
            else
                CRenderEngine::DrawText(hDC, m_pManager, rc, m_sText, m_dwDisabledTextColor,
                                        m_iFont, m_uTextStyle);
        }
    }
}

void CLabelUI::SetEnabledEffect(bool _EnabledEffect)
{
    m_bEnableEffect = _EnabledEffect;

    if (m_bEnableEffect)
    {
#ifdef _UNICODE
        m_pWideText = (LPWSTR)m_sText.GetData();
#else
        int iLen = m_sText.GetLength();

        if (m_pWideText) { delete[ ] m_pWideText; }

        m_pWideText = new WCHAR[iLen + 1];
        ::ZeroMemory(m_pWideText, (iLen + 1) * sizeof(WCHAR));
        ::MultiByteToWideChar(CP_ACP, 0, m_sText.GetData(), -1, (LPWSTR)m_pWideText, iLen);
#endif // _UNICODE
    }

    Invalidate();
}

bool CLabelUI::GetEnabledEffect()
{
    return m_bEnableEffect;
}

void CLabelUI::PaintTextEffect(HDC hDC, RECT rt)
{
#ifdef USE_GDIPLUS
    Gdiplus::Font nFont(hDC, m_pManager->GetFont(GetFont()));
    Graphics nGraphics(hDC);
    nGraphics.SetTextRenderingHint(TextRenderingHintAntiAlias);

    StringFormat format;
    StringAlignment sa = StringAlignment::StringAlignmentNear;

    if ((m_uTextStyle & DT_VCENTER) != 0) { sa = StringAlignment::StringAlignmentCenter; }
    else if ((m_uTextStyle & DT_BOTTOM) != 0) { sa = StringAlignment::StringAlignmentFar; }

    format.SetLineAlignment((StringAlignment)sa);
    sa = StringAlignment::StringAlignmentNear;

    if ((m_uTextStyle & DT_CENTER) != 0) { sa = StringAlignment::StringAlignmentCenter; }
    else if ((m_uTextStyle & DT_RIGHT) != 0) { sa = StringAlignment::StringAlignmentFar; }

    format.SetAlignment((StringAlignment)sa);

    RectF nRc((float)rt.left, (float)rt.top, (float)rt.right - rt.left, (float)rt.bottom - rt.top);
    RectF nShadowRc = nRc;
    nShadowRc.X += m_ShadowOffset.X;
    nShadowRc.Y += m_ShadowOffset.Y;

    int nGradientLength = GetGradientLength();

    if (nGradientLength == 0)
    {
        nGradientLength = (rt.bottom - rt.top);
    }

    LinearGradientBrush nLineGrBrushA(Point(GetGradientAngle(), 0), Point(0, nGradientLength),
                                      ARGB2Color(GetTextShadowColorA()),
                                      ARGB2Color(GetTextShadowColorB() == -1 ? GetTextShadowColorA() : GetTextShadowColorB()));
    LinearGradientBrush nLineGrBrushB(Point(GetGradientAngle(), 0), Point(0, nGradientLength),
                                      ARGB2Color(GetTextColor()), ARGB2Color(GetTextColor1() == -1 ? GetTextColor() : GetTextColor1()));

    if (GetEnabledLuminous())
    {
        // from http://bbs.csdn.net/topics/390346428
        int iFuzzyWidth = (int)(nRc.Width / GetLuminousFuzzy());

        if (iFuzzyWidth < 1) { iFuzzyWidth = 1; }

        int iFuzzyHeight = (int)(nRc.Height / GetLuminousFuzzy());

        if (iFuzzyHeight < 1) { iFuzzyHeight = 1; }

        RectF nTextRc(0.0f, 0.0f, nRc.Width, nRc.Height);

        Bitmap Bit1((INT)nRc.Width, (INT)nRc.Height);
        Graphics g1(&Bit1);
        g1.SetSmoothingMode(SmoothingModeAntiAlias);
        g1.SetTextRenderingHint(TextRenderingHintAntiAlias);
        g1.SetCompositingQuality(CompositingQualityAssumeLinear);
        Bitmap Bit2(iFuzzyWidth, iFuzzyHeight);
        Graphics g2(&Bit2);
        g2.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g2.SetPixelOffsetMode(PixelOffsetModeNone);

        FontFamily ftFamily;
        nFont.GetFamily(&ftFamily);
        int iLen = wcslen(m_pWideText);
        g1.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushB);

        g2.DrawImage(&Bit1, 0, 0, (int)iFuzzyWidth, (int)iFuzzyHeight);
        g1.Clear(Color(0));
        g1.DrawImage(&Bit2, (int)m_ShadowOffset.X, (int)m_ShadowOffset.Y, (int)nRc.Width, (int)nRc.Height);
        g1.SetTextRenderingHint(TextRenderingHintAntiAlias);

        nGraphics.DrawImage(&Bit1, nRc.X, nRc.Y);
    }

    if (GetEnabledStroke() && GetStrokeColor() > 0)
    {
        LinearGradientBrush nLineGrBrushStroke(Point(GetGradientAngle(), 0), Point(0, rt.bottom - rt.top + 2),
                                               ARGB2Color(GetStrokeColor()), ARGB2Color(GetStrokeColor()));
#ifdef _UNICODE
        nRc.Offset(-1, 0);
        nGraphics.DrawString(m_sText, m_sText.GetLength(), &nFont, nRc, &format, &nLineGrBrushStroke);
        nRc.Offset(2, 0);
        nGraphics.DrawString(m_sText, m_sText.GetLength(), &nFont, nRc, &format, &nLineGrBrushStroke);
        nRc.Offset(-1, -1);
        nGraphics.DrawString(m_sText, m_sText.GetLength(), &nFont, nRc, &format, &nLineGrBrushStroke);
        nRc.Offset(0, 2);
        nGraphics.DrawString(m_sText, m_sText.GetLength(), &nFont, nRc, &format, &nLineGrBrushStroke);
        nRc.Offset(0, -1);
#else
        int iLen = wcslen(m_pWideText);
        nRc.Offset(-1, 0);
        nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushStroke);
        nRc.Offset(2, 0);
        nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushStroke);
        nRc.Offset(-1, -1);
        nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushStroke);
        nRc.Offset(0, 2);
        nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushStroke);
        nRc.Offset(0, -1);
#endif // _UNICODE
    }

#ifdef _UNICODE

    if (GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))
    {
        nGraphics.DrawString(m_sText, m_sText.GetLength(), &nFont, nShadowRc, &format, &nLineGrBrushA);
    }

    nGraphics.DrawString(m_sText, m_sText.GetLength(), &nFont, nRc, &format, &nLineGrBrushB);
#else
    int iLen = wcslen(m_pWideText);

    if (GetEnabledShadow() && (GetTextShadowColorA() > 0 || GetTextShadowColorB() > 0))
    {
        nGraphics.DrawString(m_pWideText, iLen, &nFont, nShadowRc, &format, &nLineGrBrushA);
    }

    nGraphics.DrawString(m_pWideText, iLen, &nFont, nRc, &format, &nLineGrBrushB);
#endif // _UNICODE

#endif // USE_GDIPLUS
}

RECT CLabelUI::GetTextRect(RECT rc)
{
    UINT dwTextStyle = DT_CALCRECT | m_uTextStyle & ~DT_RIGHT & ~DT_CENTER;

    if (IsEnabled())
    {
        if (m_bShowHtml)
            CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rc, m_sText, m_dwTextColor,
                                        GetRectLinks(), GetStringLinks(), GetLinksNum(),
                                        m_iFont, dwTextStyle);
        else
        {
            CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rc, m_sText, m_dwTextColor,
                                    m_iFont, dwTextStyle);
        }
    }
    else
    {
        if (m_bShowHtml)
            CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rc, m_sText, m_dwDisabledTextColor,
                                        GetRectLinks(), GetStringLinks(), GetLinksNum(),
                                        m_iFont, dwTextStyle);
        else
            CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rc, m_sText, m_dwDisabledTextColor,
                                    m_iFont, dwTextStyle);
    }

    return rc;
}

#ifdef USE_GDIPLUS
void CLabelUI::SetShadowOffset(int _offset, int _angle)
{
    if (_angle > 180 || _angle < -180) { return; }

    RECT rc = m_rcItem;

    if (_angle >= 0 && _angle <= 180) { rc.top -= _offset; }
    else if (_angle > -180 && _angle < 0) { rc.top += _offset; }

    if (_angle > -90 && _angle <= 90) { rc.left -= _offset; }
    else if (_angle > 90 || _angle < -90) { rc.left += _offset; }

    m_ShadowOffset.X = (float)rc.top;
    m_ShadowOffset.Y = (float)rc.left;
    Invalidate();
}

RectF CLabelUI::GetShadowOffset()
{
    return m_ShadowOffset;
}

void CLabelUI::SetEnabledLuminous(bool bEnableLuminous)
{
    m_bEnableLuminous = bEnableLuminous;
    Invalidate();
}

bool CLabelUI::GetEnabledLuminous()
{
    return m_bEnableLuminous;
}

void CLabelUI::SetLuminousFuzzy(float fFuzzy)
{
    if (fFuzzy < 0.0001f) { return; }

    m_fLuminousFuzzy = fFuzzy;
    Invalidate();
}

float CLabelUI::GetLuminousFuzzy()
{
    return m_fLuminousFuzzy;
}

void CLabelUI::SetTextColor1(DWORD _TextColor1)
{
    m_dwTextColor1 = _TextColor1;
    Invalidate();
}

DWORD CLabelUI::GetTextColor1()
{
    return m_dwTextColor1;
}

void CLabelUI::SetTextShadowColorA(DWORD _TextShadowColorA)
{
    m_dwTextShadowColorA = _TextShadowColorA;
    Invalidate();
}

DWORD CLabelUI::GetTextShadowColorA()
{
    return m_dwTextShadowColorA;
}

void CLabelUI::SetTextShadowColorB(DWORD _TextShadowColorB)
{
    m_dwTextShadowColorB    = _TextShadowColorB;
    Invalidate();
}

DWORD CLabelUI::GetTextShadowColorB()
{
    return m_dwTextShadowColorB;
}

void CLabelUI::SetGradientAngle(int _SetGradientAngle)
{
    m_GradientAngle = _SetGradientAngle;
    Invalidate();
}

int CLabelUI::GetGradientAngle()
{
    return m_GradientAngle;
}

void CLabelUI::SetEnabledStroke(bool _EnabledStroke)
{
    m_EnabledStroke = _EnabledStroke;
    Invalidate();
}

bool CLabelUI::GetEnabledStroke()
{
    return m_EnabledStroke;
}

void CLabelUI::SetStrokeColor(DWORD _StrokeColor)
{
    m_dwStrokeColor = _StrokeColor;
    Invalidate();
}

DWORD CLabelUI::GetStrokeColor()
{
    return m_dwStrokeColor;
}

void CLabelUI::SetEnabledShadow(bool _EnabledShadow)
{
    m_EnabledShadow = _EnabledShadow;
    Invalidate();
}

bool CLabelUI::GetEnabledShadow()
{
    return m_EnabledShadow;
}

void CLabelUI::SetGradientLength(int _GradientLength)
{
    m_GradientLength    = _GradientLength;
    Invalidate();
}

int CLabelUI::GetGradientLength()
{
    return m_GradientLength;
}
#endif // USE_GDIPLUS
}
