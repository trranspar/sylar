//基于Epoll的IO协程调度器
#ifndef __SYLAR_IOMANAGER_H__
#define __SYLAR_IOMANAGER_H__

#include "scheduler.h"
#include "timer.h"

namespace sylar {

class IOManager : public Scheduler, public TimerManager {
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    /**
     * @brief IO事件，继承自epoll对事件的定义
     * @details 这里只关心socket fd的读和写事件，其他epoll事件会归类到这两类事件中
     */
    enum Event {
        NONE    = 0X0,
        READ    = 0X1,  //EPOLLIN
        WRITE   = 0X4   //EPOLLOUT
    };
private:
    /**
     * @brief socket fd上下文类
     * @details 每个socket fd都对应一个FdContext，包括fd的值，fd上的事件，以及fd的读写事件上下文
     */
    struct FdContext {
        typedef Mutex MutexType;

        /**
         * @brief 事件上下文类
         * @details fd的每个事件都有一个事件上下文，保存这个事件的回调函数以及执行回调函数的调度器
         *          对fd事件做了简化，只预留了读事件和写事件，所有的事件都被归类到这两类事件中
         */
        struct EventContext {
            Scheduler* scheduler = nullptr; //事件执行的scheduler
            Fiber::ptr fiber;               //事件协程
            std::function<void()> cb;       //事件的回调函数
        };

        /**
         * @brief 获取事件上下文类
         * @param[in] event 事件类型
         * @return 返回对应事件的上下文
         */
        EventContext& getContext(Event event);

        /**
         * @brief 重置事件上下文
         * @param[in, out] ctx 待重置的事件上下文对象
         */
        void resetContext(EventContext& ctx);

        /**
         * @brief 触发事件
         * @details 根据事件类型调用对应上下文结构中的调度器去调度回调协程或回调函数
         * @param[in] event 事件类型
         */
        void triggerEvent(Event event);

        EventContext read;      //读事件
        EventContext write;     //写事件
        int fd = 0;             //事件关联的句柄
        Event events = NONE;    //已经注册的事件
        MutexType mutex;
        
    };

public:
    /**
     * @brief 构造函数
     * @param[in] threads 线程数量
     * @param[in] use_caller 是否将调用线程包含进去
     * @param[in] name 调度器的名称
     */
    IOManager(size_t threads = 1, bool user_caller = true, const std::string& name = "");

    /**
     * @brief 析构函数
     * @details 等待Scheduler调度完所有的任务
     *          然后再关闭epoll句柄和pipe句柄，然后释放所有的FdContext
     */
    ~IOManager();

    /**
     * @brief 添加事件
     * @details fd描述符发生了event事件时执行cb函数
     * @param[in] fd socket句柄
     * @param[in] event 事件类型
     * @param[in] cb 事件回调函数，如果为空，则默认把当前协程作为回调执行体
     * @return 添加成功返回0,失败返回-1
     */
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

    /**
     * @brief 删除事件
     * @param[in] fd socket句柄
     * @param[in] event 事件类型
     * @attention 不会触发事件
     * @return 是否删除成功
     */
    bool delEvent(int fd, Event event);

    /**
     * @brief 取消事件
     * @param[in] fd socket句柄
     * @param[in] event 事件类型
     * @attention 如果该事件被注册过回调，那就触发一次回调事件
     * @return 是否删除成功
     */
    bool cancelEvent(int fd, Event event);

    /**
     * @brief 取消所有事件
     * @details 所有被注册的回调事件在cancel之前都会被执行一次
     * @param[in] fd socket句柄
     * @return 是否删除成功
     */
    bool cancelAll(int fd);

    /**
     * @brief 获取当前IO调度器
     */
    static IOManager* GetThis();

protected:
    /**
     * @brief 通知调度器有任务要调度
     * @details 写pipe让idle协程从epoll_wait退出，待idle协程yield之后Scheduler::run就可以调度其他任务
     * 如果当前没有空闲调度线程，那就没必要发通知
     */
    void tickle() override;

    /**
     * @brief 结束IO调度
     */
    bool stopping() override;
    bool stopping(uint64_t& timeout);

    /**
     * @brief idle协程
     * @details IO协程调度应阻塞在等待IO事件上，epoll_wait返回时退出idle，即tickle或注册的IO事件就绪
     * 调度器无调度任务时会阻塞idle协程上，对IO调度器而言，idle状态应该关注两件事
     * 一是如果有新的调度任务，对应Schduler::schedule()，那应该立即退出idle状态，并执行对应的任务；
     * 二是关注当前注册的所有IO事件有没有触发，如果有触发，那么应该执行IO事件对应的回调函数
     */
    void idle() override;
    void onTimerInsertedAtFront() override;
    
    void contextResize(size_t size);
    
private:
    int m_epfd = 0;     // epoll文件句柄
    int m_tickleFds[2]; // pipe文件句柄，其中fd[0]表示读端，fd[1]表示写端

    std::atomic<size_t> m_pendingEventCount =  {0}; //等待执行的事件数量
    RWMutexType m_mutex;
    std::vector<FdContext*> m_fdContexts;   //socket事件上下文容器
};



}

#endif
