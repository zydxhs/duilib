#ifndef __UICONTROL_H__
#define __UICONTROL_H__

#pragma once

namespace DuiLib {

/////////////////////////////////////////////////////////////////////////////////////
//

typedef CControlUI *(CALLBACK *FINDCONTROLPROC)(CControlUI *, LPVOID);
class COleDataHelper;

class DUILIB_API CControlUI : public IEffectCallBackUI
{
public:
    CControlUI();
    virtual void Delete();

protected:
    virtual ~CControlUI();

public:
    virtual CDuiString GetName() const;
    virtual void SetName(LPCTSTR pstrName);
    virtual LPCTSTR GetClass() const;
    virtual LPVOID GetInterface(LPCTSTR pstrName);
    virtual UINT GetControlFlags() const;
    virtual HWND GetNativeWindow() const;

    virtual bool Activate();
    virtual CPaintManagerUI *GetManager() const;
    virtual void SetManager(CPaintManagerUI *pManager, CControlUI *pParent, bool bInit = true);
    virtual CControlUI *GetParent() const;
    virtual CControlUI *GetCover() const;
    virtual void SetCover(CControlUI *pControl);

    // 文本相关
    virtual CDuiString GetText() const;
    virtual void SetText(LPCTSTR pstrText);
    virtual void ReloadText(void);

    // 图形相关
    DWORD GetBkColor() const;
    void SetBkColor(DWORD dwBackColor);
    DWORD GetBkColor2() const;
    void SetBkColor2(DWORD dwBackColor);
    DWORD GetBkColor3() const;
    void SetBkColor3(DWORD dwBackColor);
    void SetDirection(bool bHorizonal);
    bool GetDirection(void);
    LPCTSTR GetBkImage();
    void SetBkImage(LPCTSTR pStrImage);
    bool IsColorHSL() const;
    void SetColorHSL(bool bColorHSL);
    SIZE GetBorderRound() const;
    void SetBorderRound(SIZE cxyRound);
    bool DrawImage(HDC hDC, TDrawInfo &drawInfo);

    //边框相关
    DWORD GetBorderColor() const;
    void SetBorderColor(DWORD dwBorderColor);
    DWORD GetHotBorderColor() const;
    void SetHotBorderColor(DWORD dwBorderColor);
    DWORD GetFocusBorderColor() const;
    void SetFocusedBorderColor(DWORD dwBorderColor);
    DWORD GetSelectedBorderColor() const;
    void SetSelectedBorderColor(DWORD dwSelectedColor);
    RECT GetBorderSize() const;
    void SetBorderSize(RECT rc);
    void SetBorderSize(int iSize);
    int GetBorderStyle() const;
    void SetBorderStyle(int nStyle);

    // 位置相关
    virtual RECT GetMargin() const;
    virtual void SetMargin(RECT rcMargin);      // 设置外边距，由上层窗口绘制
#define GetPadding GetMargin
#define SetPadding SetMargin
    virtual const RECT &GetPos() const;
    virtual RECT GetRelativePos() const; // 相对(父控件)位置
    virtual RECT GetClientPos() const;   // 返回控件矩形区域
    // 只有控件为float的时候，外部调用SetPos和Move才是有效的，位置参数是相对父控件的位置
    virtual void SetPos(RECT rc, bool bNeedInvalidate = true);
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true);
    virtual int GetWidth() const;
    virtual int GetHeight() const;
    virtual int GetX() const;
    virtual int GetY() const;
    virtual SIZE GetFixedXY() const;            // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedXY(SIZE szXY);         // 仅float为true时有效
    virtual TPercentInfo GetFloatPercent() const;
    virtual void SetFloatPercent(TPercentInfo piFloatPercent);
    virtual int GetFixedWidth() const;          // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedWidth(int cx);         // 预设的参考值
    virtual int GetFixedHeight() const;         // 实际大小位置使用GetPos获取，这里得到的是预设的参考值
    virtual void SetFixedHeight(int cy);        // 预设的参考值
    virtual int GetMinWidth() const;
    virtual void SetMinWidth(int cx);
    virtual int GetMaxWidth() const;
    virtual void SetMaxWidth(int cx);
    virtual int GetMinHeight() const;
    virtual void SetMinHeight(int cy);
    virtual int GetMaxHeight() const;
    virtual void SetMaxHeight(int cy);

    // 鼠标提示
    virtual CDuiString GetToolTip() const;
    virtual void SetToolTip(LPCTSTR pstrText);
    virtual void ReloadToolTip(void);
    virtual void SetToolTipWidth(int nWidth);
    virtual int   GetToolTipWidth(void);    // 多行ToolTip单行最长宽度

    // 快捷键
    virtual TCHAR GetShortcut() const;
    virtual bool IsNeedCtrl(void) const;
    virtual bool IsNeedShift(void) const;
    virtual bool IsNeedAlt(void) const;
    virtual void SetShortcut(LPCTSTR pstrText);

    // 菜单
    virtual bool IsContextMenuUsed() const;
    virtual void SetContextMenuUsed(bool bMenuUsed);

    // 用户属性
    virtual const CDuiString &GetUserData(); // 辅助函数，供用户使用
    virtual void SetUserData(LPCTSTR pstrText); // 辅助函数，供用户使用
    virtual UINT_PTR GetTag() const; // 辅助函数，供用户使用
    virtual void SetTag(UINT_PTR pTag); // 辅助函数，供用户使用

    // 一些重要的属性
    virtual bool IsVisible() const;
    virtual bool SetVisible(bool bVisible = true);
    // .仅供内部调用，有些UI拥有窗口句柄，需要重写此函数
    virtual void SetInternVisible(bool bVisible = true);
    virtual bool IsEnabled() const;
    virtual void SetEnabled(bool bEnable = true);
    virtual bool IsMouseEnabled() const;
    virtual void SetMouseEnabled(bool bEnable = true);
    virtual bool IsKeyboardEnabled() const;
    virtual void SetKeyboardEnabled(bool bEnable = true);
    bool IsHot() const;
    virtual bool IsFocused() const;
    virtual void SetFocus();
    virtual bool IsFloat() const;
    virtual void SetFloat(bool bFloat = true);

    // 权重属性
    void SetWeight(int nWeight);
    int GetWeight(void);

    // 自定义(未处理的)属性
    void AddCustomAttribute(LPCTSTR pstrName, LPCTSTR pstrAttr);
    LPCTSTR GetCustomAttribute(LPCTSTR pstrName) const;
    bool RemoveCustomAttribute(LPCTSTR pstrName);
    void RemoveAllCustomAttribute();

    virtual CControlUI *FindControl(FINDCONTROLPROC Proc, LPVOID pData, UINT uFlags);

    void Invalidate();
    bool IsUpdateNeeded() const;
    void NeedUpdate();
    void NeedParentUpdate();
    DWORD GetAdjustColor(DWORD dwColor);

    virtual void Init();
    virtual void DoInit();

    virtual void Event(TEventUI &event);
    virtual void DoEvent(TEventUI &event);

    virtual CDuiString GetAttribute(LPCTSTR pstrName);
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    virtual CDuiString GetAttributeList(bool bIgnoreDefault = true);
    virtual void SetAttributeList(LPCTSTR pstrList);

    virtual SIZE EstimateSize(SIZE szAvailable);

    // 返回要不要继续绘制
    virtual bool Paint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl = NULL);
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl);
    virtual void PaintBkColor(HDC hDC);
    virtual void PaintBkImage(HDC hDC);
    virtual void PaintStatusImage(HDC hDC);
    virtual void PaintText(HDC hDC);
    virtual void PaintBorder(HDC hDC);

    virtual void DoPostPaint(HDC hDC, const RECT &rcPaint);

    //虚拟窗口参数
    void SetVirtualWnd(LPCTSTR pstrValue);
    CDuiString GetVirtualWnd() const;

    // 拖拽相关
    void SetDropEnable(bool bDropEnable);
    bool GetDropEnable(void);
    void SetDragEnable(bool bDragEnable);
    bool GetDragEnable(void);
    virtual void OnDragEnter(COleDataHelper *pDataHelper);
    virtual void OnDragOver(COleDataHelper *pDataHelper);
    virtual void OnDragLeave(void);
    virtual void OnDragDrop(COleDataHelper *pDataHelper);

    void SetCapture(void);
    void ReleaseCapture(void);

    void SetAutoWidth(bool bAutoWidth);
    bool GetAutoWidth(void);
    void SetAutoHeight(bool bAutoHeight);
    bool GetAutoHeight(void);

    // 2018-08-18 zhuyadong 添加特效
    bool AddEffect(BYTE byTrigger, BYTE byEffect, WORD wElapse = 150, bool bDirection = true, bool bLoop = false);
    void DelEffect(BYTE byTrigger);                 // 删除动画触发器
    bool StartEffect(BYTE byTrigger);               // 开始播放动画
    void StopEffect(void);                          // 停止播放动画
    BYTE GetEffectTrigger(void);                    // 获取当前特效触发器ID

    virtual void OnEffectBegin(TAniParam &data);    // 每一个特效开始时回调
    virtual void OnEffectEnd(TAniParam &data);      // 每一个特效结束时回调
    virtual void OnEffectDraw(TAniParam &data);     // 每一个特效帧绘制时回调

    // 设置 Cover 的大小
    // LONG nPersent    : Cover 相对于控件的百分比大小
    // bool bHorizontal : 方向， true 表示水平，false 表示垂直
    // bool bTopLeft    : Cover位置，bHorizontal=true  时：true/false 表示靠近上/下
    //                               bHorizontal=false 时：true/false 表示靠近左/右
    void SetCoverSize(LONG nPersent, bool bHorizontal, bool bTopLeft);

protected:
    void OnDoDragDrop(TEventUI &evt);

public:
    CEventSource OnInit;
    CEventSource OnDestroy;
    CEventSource OnSize;
    CEventSource OnEvent;
    CEventSource OnNotify;
    CEventSource OnPaint;
    CEventSource OnPostPaint;

protected:
    CPaintManagerUI *m_pManager;
    CControlUI *m_pParent;
    CControlUI *m_pCover;
    CEffectUI *m_pEffect;     // 动画特效对象
    CDuiString m_sVirtualWnd;
    CDuiString m_sName;
    bool m_bUpdateNeeded;
    bool m_bMenuUsed;
    bool m_bAsyncNotify;
    BYTE m_byEffectTrigger; // 正在播放的动画的触发器ID
    RECT m_rcItem;          // 控件包含边框的矩形
    RECT m_rcMargin;        // 外边距
    SIZE m_cXY;
    SIZE m_cxyFixed;
    SIZE m_cxyMin;
    SIZE m_cxyMax;
    bool m_bVisible;
    bool m_bInternVisible;
    bool m_bEnabled;
    bool m_bMouseEnabled;
    bool m_bKeyboardEnabled;
    bool m_bSetPos; // 防止SetPos循环调用
    bool m_bFloat;
    TPercentInfo m_piFloatPercent;

    CDuiString m_sText;
    CDuiString m_sTextOrig;     // 翻译前的文本
    CDuiString m_sToolTip;
    CDuiString m_sToolTipOrig;  // 翻译前的文本
    TCHAR m_chShortcut;
    bool  m_bNeedCtrl;
    bool  m_bNeedShift;
    bool  m_bNeedAlt;
    CDuiString m_sUserData;
    UINT_PTR m_pTag;

    DWORD m_dwBackColor;
    DWORD m_dwBackColor2;
    DWORD m_dwBackColor3;
    bool  m_bDirection;         // 渐变方向
    bool m_bColorHSL;
    bool m_bHot;                // 鼠标悬停
    bool m_bFocused;            // 焦点
    TDrawInfo m_diBk;
    TDrawInfo m_diFore;
    DWORD m_dwBorderColor;          // 边框颜色
    DWORD m_dwHotBorderColor;       // 鼠标悬停状态边框颜色
    DWORD m_dwFocusedBorderColor;   // 焦点状态边框颜色
    DWORD m_dwSelectedBorderColor;  // 选中状态边框颜色
    int m_nBorderStyle;
    int m_nTooltipWidth;
    SIZE m_cxyBorderRound;
    RECT m_rcPaint;
    RECT m_rcBorderSize;
    CDuiStringPtrMap m_mCustomAttrHash;
    int m_nWeight;                      // 权重，默认100。值越小，权重越小，空间不足时优先裁减其大小直到隐藏
    TDrawInfo m_diDrag;                 // 拖拽时显示的图片
    bool m_bDropEnable;                 // 是否接受拖放(拖放目标）
    bool m_bDragEnable;                 // 是否允许拖动（拖放源）
    bool m_bCapture;                    // true 表示捕获鼠标
    bool m_bAutoWidth;                  // 自动计算宽度
    bool m_bAutoHeight;                 // 自动计算高度
};

} // namespace DuiLib

#endif // __UICONTROL_H__
