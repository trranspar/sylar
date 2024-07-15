#ifndef __SYLAR_SCHEDULER_H__
#define __SYLAR_SCHEDULER_H__

#include<memory>
#include<vector>
#include<list>
#include"fiber.h"
#include"thread.h"



namespace sylar {

class Scheduler {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef Mutex MutexType;

    //threads:线程池大小
    //user_caller:将执行了Scheduler构造函数的线程放入Scheduler中 name:线程池名称
    Scheduler(size_t threads = 1, bool user_caller = true, const std::string& name = "");

    virtual ~Scheduler();

    const std::string& getName() const { return m_name; }

    static Scheduler* GetThis();
    static Fiber* GetMainFiber();   //协程调度器的主协程

    void start();
    void stop();

    template<class FiberOrCb>
    void schedule(FiberOrCb fc, int thread = -1) {  //加入任务
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        
        if(need_tickle) {
            tickle();
        }
    }

    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end) {
        bool need_tickle = false;
        {
            MutexType::Lock lock(m_mutex);
            while (begin != end) {
                need_tickle = scheduleNoLock(&*begin, -1) || need_tickle;
                ++begin;
            }
        }
        if(need_tickle) {
            tickle();
        }
    }

protected:
    virtual void tickle();  //唤醒线程
    void run();     //协程调度
    virtual bool stopping();
    virtual void idle();

    void setThis();

    /**
     * @return 是否有空闲调度线程
     */
    bool hasIdleThreads() { return m_idleThreadCount > 0; }
private:
    template<class FiberOrCb>
    bool scheduleNoLock(FiberOrCb fc, int thread) {
        bool need_tickle = m_fibers.empty();
        FiberAndThread ft(fc, thread);
        if(ft.fiber || ft.cb) {
            m_fibers.push_back(ft);
        }
        return need_tickle;
    }

private:
    struct FiberAndThread {
        Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;     //协程在指定线程中运行的线程id

        FiberAndThread(Fiber::ptr f, int thr) : fiber(f), thread(thr) {

        }

        FiberAndThread(Fiber::ptr* f, int thr) : thread(thr) {
            fiber.swap(*f); //swap 智能指针引用数量减一
        }

        FiberAndThread(std::function<void()> f, int thr) : cb(f), thread(thr) {

        }

        FiberAndThread(std::function<void()>* f, int thr) : thread(thr) {
            cb.swap(*f);
        }

        //将一个类用到STL中必须要有默认构造函数，否则无法进行初始化
        FiberAndThread() : thread(-1) {

        }

        //重置
        void reset() {
            fiber = nullptr;
            cb =  nullptr;
            thread = -1;
        }
    };
    
private:
    MutexType m_mutex;
    std::vector<Thread::ptr> m_threads; //线程池
    std::list<FiberAndThread> m_fibers; //等待执行的协程队列
    Fiber::ptr m_rootFiber; //主协程
    std::string m_name;

protected:
    std::vector<int> m_threadIds;   //记录线程池的全部线程id
    size_t m_threadCount = 0;       //线程数量
    std::atomic<size_t> m_activeThreadCount = {0}; //活跃线程数量
    std::atomic<size_t> m_idleThreadCount = {0};   //空闲线程数量
    bool m_stopping = true; //运行状态
    bool m_autoStop = false;
    int m_rootThread = 0;   //主线程id use_caller
};



}


#endif