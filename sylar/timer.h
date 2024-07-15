#ifndef __SYLAR_TIMER_H__
#define __SYLAR_TIMER_H__

#include <memory>
#include <vector>
#include <set>
#include "thread.h"

namespace sylar {

class TimerManager;
class Timer : public std::enable_shared_from_this<Timer> {
friend TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;
    bool cancel();
    bool refresh();
    bool reset(uint64_t ms, bool from_now);    
private:
    /**
     * @brief 构造函数
     * @param[in] ms 定时器执行间隔时间
     * @param[in] cb 回调函数
     * @param[in] recurring 是否循环
     * @param[in] manager 定时器管理器
     */
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager* manager);

    /**
     * @brief 构造函数
     * @param[in] next 执行的时间戳(毫秒)
     */
    Timer(uint64_t next);
private:
    bool m_recurring = false;   //是否循环定时器
    uint64_t m_ms = 0;          //执行周期
    uint64_t m_next = 0;        //精确的执行时间
    std::function<void()> m_cb; //回调函数
    TimerManager* m_manager = nullptr;  //定时器管理器
private:
    //定时器比较仿函数
    struct Comparator {
        bool operator()(const Timer::ptr& lhs, const Timer::ptr& rhs) const;
    };
};



class TimerManager {
friend class Timer;
public:
    typedef RWMutex RWMMutexType;
    TimerManager();
    virtual ~TimerManager();

    /**
     * @brief 添加定时器
     * @param[in] ms 定时器执行间隔时间
     * @param[in] cb 定时器回调函数
     * @param[in] recurring 是否循环定时器
     */
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

    /**
     * @brief 添加条件定时器
     * @param[in] ms 定时器执行间隔时间
     * @param[in] cb 定时器回调函数
     * @param[in] weak_cond 条件
     * @param[in] recurring 是否循环
     */
    Timer::ptr addConditionTimer(uint64_t ms, std::function<void()> cb
                                ,std::weak_ptr<void> weak_cond
                                ,bool recurring = false);
    
    
    // @brief 到最近一个定时器执行的时间间隔(毫秒)
    uint64_t getNextTimer();

    /**
     * @brief 获取需要执行的定时器的回调函数列表
     * @param[out] cbs 回调函数数组
     */
    void listExpiredCb(std::vector<std::function<void()> >& cbs);

    // @brief 是否有定时器
    bool hasTimer();
protected:
    // @brief 当有新的定时器插入到定时器的首部,执行该函数
    virtual void onTimerInsertedAtFront() = 0;

    // @brief 将定时器添加到管理器中
    void addTimer(Timer::ptr val, RWMMutexType::writeLock& lock);
private:
    // @brief 检测服务器时间是否需要回滚
    bool detectClockRollover(uint64_t now_ms);
private:
    RWMMutexType m_mutex;   // Mutex
    std::set<Timer::ptr, Timer::Comparator> m_timers;   // 定时器集合
    bool m_tickled = false; // 是否触发onTimerInsertedAtFront
    uint64_t m_previousTime = 0;    // 上次执行时间
};


}
#endif