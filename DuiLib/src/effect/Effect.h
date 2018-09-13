#pragma once
#include "graph_def.h"
#include "graph_rect.h"
#include "image_process.h"
#include <vector>

namespace DuiLib {

class CFlipEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CMosaicEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int      m_nWidth;                  // 图片宽
    int      m_nHeight;                 // 图片高
    int      m_nMosaicW;                // 马赛克宽
    int      m_nMosaicH;                // 马赛克高
    int      m_nWidthCount;             // 马赛克横向个数
    int      m_nHeightCount;            // 马赛克纵向个数
    int     *m_pMosaicArray;            // 每个马赛克是否执行完
};

class CSlideEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
protected:
    void  CalculateFrame(int length);
protected:
    int      m_step;                    // 每次滑动的距离(speed)
};

class CBandSlideEffect : public CSlideEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int     m_bandNum;                  // 带状个数
    int     m_bandSize;                 // 带状的宽或者高
};

class CBlindsEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
protected:
    int     m_bandNum;                  // 百叶个数
    int     m_bandSize;                 // 百叶的宽或者高
};

class CSepcialBlindsEffect : public CBlindsEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    void CalculateFrame(int length);
private:
    std::vector<int>    m_specialBandSize;      // 百叶的宽或者高
    int                 m_nowSumRowCol;         // 当前计算起始行或者列
};

class CScanEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
protected:
    void  CalculateFrame(int length);
protected:
    int      m_step;                    // 每次扫描的距离(speed)
};

class CInertiaSlideEffect : public CSlideEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::real m_tInertiaTime;         // 惯性运动当前时间
    void   *m_inertiaInstance;          // 惯性辅助类
    Graph::real m_tInertiacoeffecient;  // 惯性运动速率调节参数
};

class CFadeEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CZoomEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::CdRect               m_rcNow;        // 变换完当前的大小
};

class CShrinkEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::CdRect               m_rcNow;        // 变换完当前的大小
};

class CSwingEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CFunnelEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CNoiseEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int                         m_u;            // 方差
};

class CCubeEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CGradualEraseEffect : public CScanEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::real     m_alphaStep;            // alpha渐变值
    int             m_lastStart;            // 上一次渐变起始行或者列
    int             m_lastEnd;              // 上一次渐变结束行或者列
};

class CPushEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CDissolveEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    BYTE           *m_pDissolveArray;       // 标示是否已被溶解
    int             m_size;                 // 所有像素数量
    int             m_framePixel;           // 每帧溶解的个数
};

class CCurtainEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CTriangularprismEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CCubeInsideEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CSpatialmotionEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int         m_xyOffset;                     // x或y轴步长
    int         m_zOffset;                      // z轴步长
};

class CPatchFlipEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CSpatialRotateEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CDoorEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CWaveEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    CWater          *m_pWater;                      // 水波粒子系统
};

class CScatterEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int             m_vx[4];                        // 水平速度
    int             m_vy[4];                        // 垂直速度
};

class CColorFadeEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CDampSlideEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    Graph::real m_tInertiaTime;         // 惯性运动当前时间
    void       *m_inertiaInstance;      // 惯性辅助类
    Graph::real m_tInertiacoeffecient;  // 惯性运动速率调节参数
    Graph::real m_ratio;                // 图像的长宽比
};

class CWhirlPoolEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
};

class CClockWipeEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:

    Bitmap     *m_pBitmap;                  // 数据
    BYTE       *m_pBits;                    // 数据
};

class CPinWheelEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:

    Bitmap     *m_pBitmap;                  // 数据
    BYTE       *m_pBits;                    // 数据
};

class CPagePeelEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:

    Bitmap     *m_pBitmap;                  // 数据
    BYTE       *m_pBits;                    // 数据
    real        m_nK;                       // 系数
    BOOL        m_intersectTop;             // 与顶部相交
};

class CBlurEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
};

class CZoomBlurEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
};

class CFoldEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int                     m_foldNum;                  // 折叠效果的折数
    std::vector<CdRect>     m_foldRect;                 // 每个折的RECT
    std::vector<WarpRect>   m_warpRect;                 // 扭曲的rect
};

class CStackEffect : public IEffect
{
public:
    // IEffect
    void ComputeOneFrame(TAniParam *internalParam);
    void InitEffectParam(TAniParam *internalParam);
    void ReleaseEffectParam();
private:
    int                 m_bandNum;                  // 带状的条数
    int                 m_bandSize;                 // 带状的宽或者高
    std::vector<int>    m_bandFrameNum;             // 每条的帧数
    int                 m_bandAnimating;            // 当前运动的带状序号
    int                 m_bandFrameSum;             // 在当前带状前面的带状累积完成的帧数
    int                 m_bandSubFrame;             // 当前带状的执行帧数
};

/////////////////////////////////////////////////////////////////////////////////////////////
//特效工厂
IEffect *CreateEffect(DWORD animationEffect);


}
/////////////////////////////////////////////////////////////////////////////////////////////
