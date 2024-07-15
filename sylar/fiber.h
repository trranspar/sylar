#ifndef __SYLAR_FIBER_H__
#define __SYLAR_FIBER_H__

#include<memory>
#include<functional>
#include<ucontext.h>
#include"thread.h"

namespace sylar {
class Scheduler;
class Fiber : public std::enable_shared_from_this<Fiber> {
friend Scheduler;
public:
    typedef std::shared_ptr<Fiber> ptr;

    enum State {
        INIT,
        HOLD,
        EXEC,
        TERM,
        READY,
        EXCEPT
    };
private:
    Fiber();

public:
    Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);
    ~Fiber();

    //重置协程函数，并重置状态
    //INIT，TERM
    void reset(std::function<void()> cb);
    //切换到当前协程执行
    void swapIn();
    //切换到后台执行
    void swapOut();

    void call();
    void back();

    uint64_t getId() const { return m_id; }
    State getState() const { return m_state; }
public:
    //将当前协程赋给thread_local变量t_fiber
    static void SetThis(Fiber* f);
    //返回当前协程或初始化主协程
    static std::shared_ptr<Fiber> GetThis();
    //协程切换到后台，并且设置为Ready状态
    static void YieldToready();
    //协程切换到后台，并且设置为Hold状态
    static void YieldToHold();
    //总协程数
    static uint64_t TotalFibers();

    static void MainFunc();
    static void callerMainFunc();
    static uint64_t GetFiberId();
private:
    uint64_t m_id = 0;  //协程id
    uint32_t m_stacksize = 0;   //协程栈大小
    State m_state = INIT;   //状态

    ucontext_t m_ctx;   //上下文
    void* m_stack = nullptr;    //协程栈指针

    std::function<void()> m_cb; //协程执行方法
};



}
#endif