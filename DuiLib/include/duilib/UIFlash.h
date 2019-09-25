/*
    创建日期： 2012/11/05 15:09:48
    作者：     daviyang35@gmail.com
    描述：     FlashUI
*/
#ifndef __UIFLASH_H__
#define __UIFLASH_H__
#pragma once


namespace ShockwaveFlashObjects {
struct IShockwaveFlash;
}

namespace DuiLib {
class CFlashEvents;

class DUILIB_API CFlashUI : public CActiveXUI, public ITranslateAccelerator
{
public:
    CFlashUI(void);
    virtual ~CFlashUI(void);

    ShockwaveFlashObjects::IShockwaveFlash *GetShockwaveFlash(void);

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    // 属性
    CDuiString GetAlign(void);
    void SetAlign(CDuiString sAlign);
    CDuiString GetWMode(void);
    void SetWMode(CDuiString sWMode);
    CDuiString GetMovie(void);
    void SetMovie(CDuiString sMovie);
    CDuiString GetBase(void);
    void SetBase(CDuiString sBase);
    CDuiString GetScale(void);
    void SetScale(CDuiString sScale);

    bool IsPlaying(void);


    // 消息通知参数
    int GetState(void)    { return m_nState; }          // 返回当前状态
    int GetProgress(void) { return m_nProgress; }       // 返回当前进度
    CDuiString GetCommand(void)  { return m_sCmd; }     // 返回命令
    CDuiString GetArgument(void) { return m_sArg; }     // 返回参数

protected:
    virtual bool DoCreateControl() override;

private:
    // 影片状态变化通知，可能的值：
    // 0 ——正在载入
    // 1 ——未初始化
    // 2 ——已载入
    // 3 ——正在交互
    // 4 ——完成 例子
    HRESULT OnReadyStateChange(long newState);
    // 播放进度
    HRESULT OnProgress(long percentDone);
    // fscommand 命令
    // 全屏命令          "fullscreen","true"
    // 屏蔽右键菜单       "showmenu","false"
    // 禁止影片缩放       "allowscale","false"
    // 使键盘出入无效     "trapallkeys","true"
    // 调用可执行文件     "exec","需要打开的文件路径"
    // 关闭播放器        "quit"
    // 保存变量到文本文件  "save","arg"
    HRESULT FSCommand(CDuiString &command, CDuiString &args);
    // FSCommand 事件差不多，与 FSCommand不同的是，这个事件可以有返回值。参数是一个XML格式的字符串，格式如下
    // "<invoke name='%s'returntype='xml'><arguments><string>%s</string></arguments></invoke>"
    HRESULT FlashCall(CDuiString &request);

    virtual void ReleaseControl() override;
    HRESULT RegisterEventHandler(BOOL inAdvise);

    // ITranslateAccelerator
    // Duilib消息分发给Flash
    virtual LRESULT TranslateAccelerator(MSG *pMsg) override;

private:
    friend class CFlashEvents;
    DWORD m_dwCookie;
    ShockwaveFlashObjects::IShockwaveFlash *m_pFlash;
    CFlashEvents       *m_pFlashEvents;

    // 属性
    CDuiString  m_sAlign;       // 对齐属性：L=Left T=Top R=Right B=Bottom 各字符的组合
    CDuiString  m_sWMode;       // 控件的窗口模式
    CDuiString  m_sMovie;       // 影片路径（URL / 相对资源目录的路径）
    CDuiString  m_sBase;        // 指定所有影片相对路径的基地址。当影片与其它需要的其它文件不在同一目录时特别有用。默认为当前影片所在路径。
    CDuiString  m_sScale;       // 影片缩放模式

    int         m_nProgress;
    int         m_nState;
    CDuiString  m_sCmd;
    CDuiString  m_sArg;
};
}

#endif // __UIFLASH_H__
