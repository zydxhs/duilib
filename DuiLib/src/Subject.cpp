#include "StdAfx.h"
#include <vector>

namespace DuiLib {

namespace sub {
class CSubjectImpl
{
public:
    CSubjectImpl(CSubjectBase *pSub) : m_pSub(pSub) {}
    ~CSubjectImpl(void)
    {
        m_vecObs.clear();
    }

    void AddObserver(IObserver *o)
    {
        if (NULL != o)
        {
            CVecObserver::iterator it = std::find(m_vecObs.begin(), m_vecObs.end(), o);

            if (it == m_vecObs.end())
            {
                m_vecObs.push_back(o);
            }
        }
    }
    void RemoveObserver(IObserver *o)
    {
        CVecObserver::iterator it = std::find(m_vecObs.begin(), m_vecObs.end(), o);

        if (it != m_vecObs.end())
        {
            m_vecObs.erase(it);
        }
    }

    void NotifyObserver(WPARAM p1, WPARAM p2, LPARAM p3, IObserver *o)
    {
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
                    (*it)->OnSubjectUpdate(p1, p2, p3, m_pSub);
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
                    (*it)->OnSubjectUpdate(p1, p2, p3, m_pSub);
                }
            }
        }
    }

    // 通知发送方式：正向/逆向 遍历观察者
    void SetNotifyDirection(bool bForward)
    {
        m_bForward = bForward;
    }
    // 遍历观察者
    IObserver *GetFirst(void)
    {
        if (m_bForward)
        {
            m_itCur = m_vecObs.begin();
            return (m_itCur != m_vecObs.end()) ? *m_itCur : NULL;
        }
        else
        {
            m_ritCur = m_vecObs.rbegin();
            return (m_ritCur != m_vecObs.rend()) ? *m_ritCur : NULL;
        }
    }
    IObserver *GetNext(void)
    {
        if (m_bForward)
        {
            m_itCur = (m_itCur != m_vecObs.end()) ? ++m_itCur : m_itCur;
            return (m_itCur != m_vecObs.end()) ? *m_itCur : NULL;
        }
        else
        {
            m_ritCur = (m_ritCur != m_vecObs.rend()) ? ++m_ritCur : m_ritCur;
            return (m_ritCur != m_vecObs.rend()) ? *m_ritCur : NULL;
        }
    }
private:
    typedef std::vector<IObserver *>    CVecObserver;
    CSubjectBase                   *m_pSub;
    CVecObserver                    m_vecObs;
    CVecObserver::iterator          m_itCur;
    CVecObserver::reverse_iterator  m_ritCur;
    bool                            m_bForward; // 通知发送方向：true 从头到尾；false 相反
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
