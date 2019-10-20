#pragma once

namespace DuiLib {

// 动画触发器ID
enum EMTrigger
{
    TRIGGER_NONE = 0,
    TRIGGER_ENTER,                  // 鼠标移入
    TRIGGER_LEAVE,                  // 鼠标移出
    TRIGGER_CLICK,                  // 鼠标单击
    TRIGGER_SHOW,                   // 显示
    TRIGGER_HIDE,                   // 隐藏

    TRIGGER_COUNT,                  // 触发器数量
    TRIGGER_ALL = 255,              // 所有触发器
};

// 动画类型ID
enum EMEffectID
{
    EFFECT_FLIPLEFT = 0,                    // 向左翻转
    EFFECT_FLIPRIGHT,                       // 向右翻转
    EFFECT_FLIPTOP,                         // 向上翻转
    EFFECT_FLIPDOWN,                        // 向下翻转
    EFFECT_MOSAIC = 4,                      // 马赛克
    EFFECT_SLIDELEFT,                       // 向左滑动
    EFFECT_SLIDERIGHT,                      // 向右滑动
    EFFECT_SLIDETOP,                        // 向上滑动
    EFFECT_SLIDEDOWN,                       // 向下滑动
    EFFECT_BANDSLIDEH = 9,                  // 水平带状滑动
    EFFECT_BANDSLIDEV,                      // 垂直带状滑动
    EFFECT_BLINDSH,                         // 水平百叶
    EFFECT_BLINDSV,                         // 垂直百叶
    EFFECT_SEPCIALBLINDSH,                  // 不等高水平百叶
    EFFECT_SEPCIALBLINDSV = 14,             // 不等宽垂直百叶
    EFFECT_SCANLEFT,                        // 向左扫描
    EFFECT_SCANRIGHT,                       // 向右扫描
    EFFECT_SCANTOP,                         // 向上扫描
    EFFECT_SCANDOWN,                        // 向下扫描
    EFFECT_INERTIASLIDELEFT = 19,           // 向左惯性滑动
    EFFECT_INERTIASLIDERIGHT,               // 向右惯性滑动
    EFFECT_INERTIASLIDETOP,                 // 向上惯性滑动
    EFFECT_INERTIASLIDEDOWN,                // 向下惯性滑动
    EFFECT_FADE,                            // 淡出/淡入
    EFFECT_ZOOM = 24,                       // 放大
    EFFECT_SHRINK,                          // 缩小
    EFFECT_SWING,                           // 摆出/摆入
    EFFECT_FUNNEL,                          // 漏斗
    EFFECT_NOISE,                           // 噪声
    EFFECT_CUBELEFT = 29,                   // 左立方体
    EFFECT_CUBERIGHT,                       // 右立方体
    EFFECT_CUBETOP,                         // 上立方体
    EFFECT_CUBEDOWN,                        // 下立方体
    EFFECT_GRADUALERASELEFT,                // 向左渐变擦除
    EFFECT_GRADUALERASERIGHT = 34,          // 向右渐变擦除
    EFFECT_GRADUALERASETOP,                 // 向上渐变擦除
    EFFECT_GRADUALERASEDOWN,                // 向下渐变擦除
    EFFECT_PUSHLEFT,                        // 向左推挤
    EFFECT_PUSHRIGHT,                       // 向右推挤
    EFFECT_PUSHTOP = 39,                    // 向上推挤
    EFFECT_PUSHDOWN,                        // 向下推挤
    EFFECT_DISSOLVE,                        // 溶解
    EFFECT_CURTAIN,                         // 幕
    EFFECT_TRIANGULARPRISMLEFT,             // 左三棱柱
    EFFECT_TRIANGULARPRISMRIGHT = 44,       // 右三棱柱
    EFFECT_TRIANGULARPRISMTOP,              // 上三棱柱
    EFFECT_TRIANGULARPRISMDOWN,             // 下三棱柱
    EFFECT_CUBEINSIDELEFT,                  // 左内立方体
    EFFECT_CUBEINSIDERIGHT,                 // 右内立方体
    EFFECT_CUBEINSIDETOP = 49,              // 上内立方体
    EFFECT_CUBEINSIDEDOWN,                  // 下内立方体
    EFFECT_SPATIALMOTIONLEFT,               // 空间左移
    EFFECT_SPATIALMOTIONRIGHT,              // 空间右移
    EFFECT_SPATIALMOTIONTOP,                // 空间上移
    EFFECT_SPATIALMOTIONDOWN = 54,          // 空间下移
    EFFECT_PATCHRANDOMFLIP,                 // 分块翻转
    EFFECT_SPATIALROTATELEFT,               // 空间向左旋转
    EFFECT_SPATIALROTATERIGHT,              // 空间向右旋转
    EFFECT_SPATIALROTATETOP,                // 空间向上旋转
    EFFECT_SPATIALROTATEDOWN = 59,          // 空间向下旋转
    EFFECT_DOOR,                            // 开/关门
    EFFECT_WHIRLPOOL,                       // 漩涡
    EFFECT_SCATTER,                         // 打散
    EFFECT_COLORDADE,                       // 褪色
    EFFECT_DAMPSLIDE_DOWNLEFT = 64,         // 左下角(起始方向)阻尼滑动
    EFFECT_DAMPSLIDE_DOWNRIGHT,             // 右下角(起始方向)阻尼滑动
    EFFECT_DAMPSLIDE_TOPLEFT,               // 左上角(起始方向)阻尼滑动
    EFFECT_DAMPSLIDE_TOPRIGHT,              // 右上角(起始方向)阻尼滑动
    EFFECT_DAMPSLIDELEFT,                   // 向左阻尼滑动
    EFFECT_DAMPSLIDERIGHT = 69,             // 向右阻尼滑动
    EFFECT_DAMPSLIDETOP,                    // 向上阻尼滑动
    EFFECT_DAMPSLIDEDOWN,                   // 向下阻尼滑动
    EFFECT_CLOCKWIPE,                       // 环形擦除
    EFFECT_PINWHEEL,                        // 风车
    EFFECT_PAGEPEEL = 74,                   // 卷页
    EFFECT_GAUSSBLUR,                       // 模糊淡入/出
    EFFECT_WAVE,                            // 波浪化
    EFFECT_ZOOMBLUR,                        // 拖尾
    EFFECT_FOLDV,                           // 垂直折叠
    EFFECT_FOLDH = 79,                      // 水平折叠
    EFFECT_STACKLEFT,                       // 向左堆积
    EFFECT_STACKRIGHT,                      // 向右堆积
    EFFECT_STACKTOP,                        // 向上堆积
    EFFECT_STACKDOWN = 83,                  // 向下堆积

    EFFECT_COUNT,                           // 特效数量
};

class IEffect;

// 动画参数
struct TAniParam
{
    BYTE        byTrigger;          // 触发特效的条件          需要用户指定 默认 TRIGGGER_NONE
    BYTE        byEffect;           // 动画ID。EMEffectID      需要用户指定 默认 EFFECT_FLIPLEFT
    WORD        wFrequency;         // 动画帧时间，单位：毫秒  需要用户指定 默认 150 毫秒，值越大，动画越慢。
    bool        bDirection;         // 动画顺序                需要用户指定 默认 true
    bool        bLoop;              // 是否循环播放特效        需要用户指定 默认 false
    bool        bLastFrame;         // 是否是最后一帧
    BYTE        byCurFrame;         // 当前显示帧

    HBITMAP     hBitmap;            // 动画图片资源
    HDC         hMemDC;             // 动画图片DC
    SIZE        szMemDC;            // 动画图片尺寸
    BYTE       *pBmpData;           // 动画图片内存地址
    BYTE       *pBmpDataCopy;       // 外部动画图片资源备份
    IEffect    *pEffect;            // 特效类实例指针

    TAniParam(void);
    bool Init(CControlUI *pCtrl);
    void Release(void);
    bool IsRunning(void);
};

// 特效执行过程中的回调接口
class _declspec(novtable) IEffectCallBackUI
{
public:
    // 每一 个 动画开始时回调
    virtual void OnEffectBegin(TAniParam &data) = 0;
    // 每一 个 动画结束时回调
    virtual void OnEffectEnd(TAniParam &data) = 0;
    // 每一 帧 动画绘制时回调
    virtual void OnEffectDraw(TAniParam &data) = 0;
};

class DUILIB_API CEffectUI
{
public:
    CEffectUI(CControlUI *pCtrl);
    ~CEffectUI(void);

    // 添加动画
    // 参数1：动画触发器    参数2：特效ID    参数3：特效帧间隔    参数4：动画顺序    参数5：是否循环
    bool Add(BYTE byTrigger, BYTE byEffect, WORD wFrequency = 150, bool bDirection = true, bool bLoop = false);
    // 删除动画。如果正在播放，则自动关闭
    void Del(BYTE byTrigger);

    // 开始播放指定触发器的特效
    bool Start(BYTE byTrigger);
    // 停止播放特效
    // 参数: 动画触发器
    //************************************
    void Stop(BYTE byTrigger = TRIGGER_ALL);

    // 返回是否正在播放指定的特效
    bool IsRunning(BYTE byTrigger = TRIGGER_NONE);

    bool HasEffectTrigger(BYTE byTrigger);

    // 特效步进
    void OnElapse(BYTE byTrigger);

    // 返回指定特效的内存HDC，用于绘制
    HDC GetMemHDC(BYTE byTrigger);
    BYTE GetCurFrame(BYTE byTrigger);
    bool IsLastFrame(BYTE byTrigger);
protected:
    TAniParam *GetTriggerById(BYTE byTrigger);
    void RemoveTriggerById(BYTE byTrigger);

protected:
    CControlUI         *m_pControl;         // 启用动画的控件指针
    CDuiValArray        m_aryAniParam;      // 动画参数容器
};

//////////////////////////////////////////////////////////////////////////
// 自定义特效接口，用于实现自定义特效。
class _declspec(novtable) IEffect
{
public:
    // 计算一帧
    virtual void ComputeOneFrame(TAniParam *pData) = 0;
    // 初始化特效参数
    virtual void InitEffectParam(TAniParam *pData) = 0;
    // 清理特效参数
    virtual void ReleaseEffectParam() = 0;

protected:
    int      m_nFrame;
};

}