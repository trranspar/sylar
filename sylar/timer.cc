#include "timer.h"
#include "util.h"

namespace sylar {
    bool Timer::Comparator::operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const {
        if(!lhs && !rhs) {
            return false;
        }
        if(!lhs) {
            return true;
        }
        if(!rhs) {
            return false;
        }
        if(lhs->m_next < rhs->m_next) {
            return true;
        }
        if(rhs->m_next < lhs->m_next) {
            return false;
        }
        return lhs.get() < rhs.get();
    }

    Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager)
        :m_recurring(recurring)
        ,m_ms(ms)
        ,m_cb(cb)
        ,m_manager(manager) {
        m_next = sylar::GetCurrentMS() + m_ms;
    }

    Timer::Timer(uint64_t next) : m_next(next) {
    }

    bool Timer::cancel() {
        TimerManager::RWMMutexType::writeLock lock(m_manager->m_mutex);
        if(m_cb) {
            m_cb = nullptr;
            auto it = m_manager->m_timers.find(shared_from_this());
            m_manager->m_timers.erase(it);
            return true;
        }
        return false;
    }

    bool Timer::refresh() {
        TimerManager::RWMMutexType::writeLock lock(m_manager->m_mutex);
        if(!m_cb) {
            return false;
        }
        auto it = m_manager->m_timers.find(shared_from_this());
        if(it == m_manager->m_timers.end()) {
            return false;
        }
        m_manager->m_timers.erase(it);
        m_next = sylar::GetCurrentMS() + m_ms;
        m_manager->m_timers.insert(shared_from_this());
        return true;
    }

    bool Timer::reset(uint64_t ms, bool from_now) {
        if(ms == m_ms && !from_now) {
            return true;
        }
        TimerManager::RWMMutexType::writeLock lock(m_manager->m_mutex);
        if(!m_cb) {
            return false;
        }
        auto it = m_manager->m_timers.find(shared_from_this());
        if(it == m_manager->m_timers.end()) {
            return false;
        }
        m_manager->m_timers.erase(it);

        uint64_t start = 0;
        if(from_now) {
            start = sylar::GetCurrentMS();
        } else {
            start = m_next - m_ms;
        }
        m_ms = ms;
        m_next = start + m_ms;
        m_manager->addTimer(shared_from_this(), lock);
        return true;
    }





    TimerManager::TimerManager() {
        m_previousTime = sylar::GetCurrentMS();
    }

    TimerManager::~TimerManager() {
    }

    Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
        // 创建timer
        Timer::ptr timer(new Timer(ms, cb, recurring, this));
        RWMMutexType::writeLock lock(m_mutex);
        // 添加到set中
        addTimer(timer, lock);
        return timer;
    }

    static void OnTimer(std::weak_ptr<void> weak_cond, std::function<void()> cb) {
        std::shared_ptr<void> tmp = weak_cond.lock();
        if(tmp) {
            cb();
        }
    }

    Timer::ptr TimerManager::addConditionTimer(uint64_t ms, std::function<void()> cb
                                ,std::weak_ptr<void> weak_cond
                                ,bool recurring) {
        // 在定时器触发时调用OnTimer函数，判断条件对象是否存在，若存在则调用回调函数cb
        return addTimer(ms, std::bind(&OnTimer, weak_cond, cb), recurring);
    }

    uint64_t TimerManager::getNextTimer() {
        RWMMutexType::ReadLock lock(m_mutex);
        m_tickled = false;
        if(m_timers.empty()) {
            return ~0ull;
        }
        // 获取第一个定时器的执行时间与当前时间比较
        const Timer::ptr& next = *m_timers.begin();
        uint64_t now_ms = sylar::GetCurrentMS();
        if(now_ms >= next->m_ms) {
            return 0;
        } else {
            return next->m_ms - now_ms;
        }
    }

    void TimerManager::listExpiredCb(std::vector<std::function<void()> >& cbs) {
        uint64_t now_ms = sylar::GetCurrentMS();
        std::vector<Timer::ptr> expired;
        {
            RWMMutexType::ReadLock lock(m_mutex);
            if(m_timers.empty()) {
                return;
            }
        }
        RWMMutexType::writeLock lock(m_mutex);
        bool rollover = detectClockRollover(now_ms);
        if(!rollover && ((*m_timers.begin())->m_next > now_ms)) {
            return;
        }

        Timer::ptr now_Timer(new Timer(now_ms));
        // 若系统时间回滚，则将m_timers内的所有timer视为过期
        // 否则找到第一个m_next > now_ms的timmer，在此之前所有的timer都已超时
        auto it = rollover ? m_timers.end() : m_timers.lower_bound(now_Timer);
        while(it != m_timers.end() && (*it)->m_next == now_ms) {
            ++it;
        }
        expired.insert(expired.begin(), m_timers.begin(), it);
        m_timers.erase(m_timers.begin(), it);
        cbs.reserve(expired.size());

        for(auto& timer : expired) {
            cbs.push_back(timer->m_cb);
            if(timer->m_recurring) {
                // 若为循环定时器，则将timer重新加入到m_timers
                timer->m_next = now_ms + timer->m_ms;
                m_timers.insert(timer);
            } else {
                timer->m_cb = nullptr;
            }
        }
    }

    void TimerManager::addTimer(Timer::ptr val, RWMMutexType::writeLock& lock) {
        auto it = m_timers.insert(val).first;
        bool at_front = (it == m_timers.begin()) && !m_tickled;
        if(at_front) {
            m_tickled = true;
        }
        lock.unlock();

        if(at_front) {
            onTimerInsertedAtFront();
        }
    }

    bool TimerManager::detectClockRollover(uint64_t now_ms) {
        bool rollover = false;
        if(now_ms < m_previousTime && now_ms < (m_previousTime - 60 * 60 * 1000)) {
            rollover = true;
        }
        m_previousTime = now_ms;
        return rollover;
    }

    bool TimerManager::hasTimer() {
        RWMMutexType::ReadLock lock(m_mutex);
        return !m_timers.empty();
    }

}