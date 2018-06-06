#ifndef __GIFINFO_H__
#define __GIFINFO_H__

#pragma once

namespace DuiLib {

class CGifInfo
{
public:
    explicit CGifInfo(int nFrameCnt);
    ~CGifInfo(void);

    int GetFrameCount(void);                // 返回总帧数
    void AddFrame(TImageInfo *ptFrame);     // 添加一帧图像
    void SetCurFrame(int nCurFrame);        // 设置当前帧
    TImageInfo *GetCurFrame(void);          // 返回当前帧
    TImageInfo *GetNextFrame(void);         // 返回下一帧
    TImageInfo *GetFrame(int nIdx);         // 返回指定帧

private:
    CDuiPtrArray    vGifFrame;              // 存储 GIF 所有帧信息的指针
    int             nCurFrame;              // 当前播放到第几帧
    bool            bIsDeleting;
};
}

#endif // __GIFINFO_H__
