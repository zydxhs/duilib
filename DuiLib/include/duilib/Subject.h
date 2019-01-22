#ifndef __OBSERVER_PATTERN_H__
#define __OBSERVER_PATTERN_H__
#pragma once

namespace DuiLib {

//观察者
class CSubjectBase;
class DUILIB_API IObserver
{
public:
    /** \brief
     *
     * \param p1 WPARAM                     参数1
     * \param NULL WPARAM p2=               参数2
     * \param NULL LPARAM p3=               参数3
     * \param NULL CSubjectBase *pSub=      主题对象指针
     * \return virtual bool                 true表示继续通知其它观察者，否则终止
     *
     */
    virtual bool OnSubjectUpdate(WPARAM p1, WPARAM p2 = NULL, LPARAM p3 = NULL, CSubjectBase *pSub = NULL) = 0;
};

//主题
namespace sub {
class CSubjectImpl;
}

class DUILIB_API CSubjectBase
{
public:
    CSubjectBase(void);
    virtual ~CSubjectBase(void);
    void AddObserver(IObserver *o);
    void RemoveObserver(IObserver *o);
    // 参数说明：
    // p1/p2/p3  通知内容。使用指针，可以传递32/64位整数，或结构体/类对象指针，32/64位系统通用
    // o         观察者指针，用于定向通知
    void NotifyObserver(WPARAM p1, WPARAM p2 = NULL, LPARAM p3 = NULL, IObserver *o = NULL);

    // 通知发送方向：正向/逆向 遍历观察者
    void SetNotifyDirection(bool bForward = true);
    // 遍历观察者，遍历过程禁止添加/删除观察者，因为可能引起程序崩溃
    IObserver *GetFirst(void);
    IObserver *GetNext(void);

private:
    sub::CSubjectImpl  *m_pSub;
};


}

#endif // __OBSERVER_PATTERN_H__
