#include "StdAfx.h"
#include <vector>

namespace DuiLib {

namespace sub {
class CSubjectImpl
{
public:
    CSubjectImpl(CSubjectBase *pSub) : m_pSub(pSub), m_bForward(true)
    {
        ::InitializeCriticalSection(&m_cs);
    }
    ~CSubjectImpl(void)
    {
        ::DeleteCriticalSection(&m_cs);
        m_vecObs.clear();
    }

    void AddObserver(IObserver *o)
    {
        if (NULL != o)
        {
            ::EnterCriticalSection(&m_cs);
            CVecObserver::iterator it = std::find(m_vecObs.begin(), m_vecObs.end(), o);

            if (it == m_vecObs.end())
            {
                m_vecObs.push_back(o);
            }

            ::LeaveCriticalSection(&m_cs);
        }
    }
    void RemoveObserver(IObserver *o)
    {
        ::EnterCriticalSection(&m_cs);
        CVecObserver::iterator it = std::find(m_vecObs.begin(), m_vecObs.end(), o);

        if (it != m_vecObs.end())
        {
            m_vecObs.erase(it);
        }

        ::LeaveCriticalSection(&m_cs);
    }

    void NotifyObserver(WPARAM p1, WPARAM p2, LPARAM p3, IObserver *o)
    {
        ::EnterCriticalSection(&m_cs);

        if (m_bForward)
        {
            std::vector<IObserver *>::iterator it(m_vecObs.begin());

            if (NULL != o)
            {
                for (; it != m_vecObs.end(); ++it)
                {
                    if (*it == o)
                    {
                        (*it)->OnSubjectUpdate(p1, p2, p3, m_pSub);
                        break;
                    }
                }
            }
            else
            {
                for (; it != m_vecObs.end(); ++it)
                {
                    // 返回 false，不再通知其它观察者
                    if (!(*it)->OnSubjectUpdate(p1, p2, p3, m_pSub)) { break; }
                }
            }
        }
        else
        {
            CVecObserver::reverse_iterator it(m_vecObs.rbegin());

            if (NULL != o)
            {
                for (; it != m_vecObs.rend(); ++it)
                {
                    if (*it == o)
                    {
                        (*it)->OnSubjectUpdate(p1, p2, p3, m_pSub);
                        break;
                    }
                }
            }
            else
            {
                for (; it != m_vecObs.rend(); ++it)
                {
                    // 返回 false，不再通知其它观察者
                    if (!(*it)->OnSubjectUpdate(p1, p2, p3, m_pSub)) { break; }
                }
            }
        }

        ::LeaveCriticalSection(&m_cs);
    }

    // 通知发送方式：正向/逆向 遍历观察者
    void SetNotifyDirection(bool bForward)
    {
        m_bForward = bForward;
    }
    // 遍历观察者，遍历过程禁止添加/删除观察者，因为可能引起程序崩溃
    IObserver *GetFirst(void)
    {
        IObserver *pObs = NULL;
        ::EnterCriticalSection(&m_cs);

        if (m_bForward)
        {
            m_itCur = m_vecObs.begin();
            pObs = (m_itCur != m_vecObs.end()) ? *m_itCur : NULL;
        }
        else
        {
            m_ritCur = m_vecObs.rbegin();
            pObs = (m_ritCur != m_vecObs.rend()) ? *m_ritCur : NULL;
        }

        ::LeaveCriticalSection(&m_cs);
        return pObs;
    }
    IObserver *GetNext(void)
    {
        IObserver *pObs = NULL;
        ::EnterCriticalSection(&m_cs);

        if (m_bForward)
        {
            m_itCur = (m_itCur != m_vecObs.end()) ? ++m_itCur : m_itCur;
            pObs = (m_itCur != m_vecObs.end()) ? *m_itCur : NULL;
        }
        else
        {
            m_ritCur = (m_ritCur != m_vecObs.rend()) ? ++m_ritCur : m_ritCur;
            pObs = (m_ritCur != m_vecObs.rend()) ? *m_ritCur : NULL;
        }

        ::LeaveCriticalSection(&m_cs);
        return pObs;
    }
private:
    typedef std::vector<IObserver *>    CVecObserver;
    CSubjectBase                   *m_pSub;
    CVecObserver                    m_vecObs;
    CVecObserver::iterator          m_itCur;
    CVecObserver::reverse_iterator  m_ritCur;
    bool                            m_bForward;     // 通知发送方向：true 从头到尾；false 相反
    CRITICAL_SECTION                m_cs;           // 保护 m_vecObs
};

}

CSubjectBase::CSubjectBase(void) : m_pSub(NULL)
{
    m_pSub = new sub::CSubjectImpl(this);
}

CSubjectBase::~CSubjectBase(void)
{
    if (NULL != m_pSub)
    {
        delete m_pSub;
        m_pSub = NULL;
    }
}

void CSubjectBase::AddObserver(IObserver *o)
{
    if (NULL != m_pSub) { m_pSub->AddObserver(o); }
}

void CSubjectBase::RemoveObserver(IObserver *o)
{
    if (NULL != m_pSub) { m_pSub->RemoveObserver(o); }
}

void CSubjectBase::NotifyObserver(WPARAM p1, WPARAM p2, LPARAM p3, IObserver *o)
{
    if (NULL != m_pSub) { m_pSub->NotifyObserver(p1, p2, p3, o); }
}

void CSubjectBase::SetNotifyDirection(bool bForward)
{
    if (NULL != m_pSub) { m_pSub->SetNotifyDirection(bForward); }
}

IObserver *CSubjectBase::GetFirst(void)
{
    return (NULL != m_pSub) ? m_pSub->GetFirst() : NULL;
}

IObserver *CSubjectBase::GetNext(void)
{
    return (NULL != m_pSub) ? m_pSub->GetNext() : NULL;
}

}
