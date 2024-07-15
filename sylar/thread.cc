#include"thread.h"
#include"log.h"
#include"util.h"
namespace sylar {

    //thread_local修饰变量具有线程周期，线程隔离
    static thread_local Thread* t_thread = nullptr; //当前线程
    static thread_local std::string t_thread_name = "UNKNOW";   //当前线程名称
    static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");



    Semaphore::Semaphore(uint32_t count) {
        if(sem_init(&m_semaphore, 0, count)) {
            throw std::logic_error("sem_init error");
        }
    }

    Semaphore::~Semaphore() {
        sem_destroy(&m_semaphore);
    }

    void Semaphore::wait() {
        //sem_wait 阻塞当前线程，调用成功return 0
        if(sem_wait(&m_semaphore)) {
            throw std::logic_error("sem_wait error");
        }        
    }

    void Semaphore::notify() {
        //sem_post增加信号量的值
        if(sem_post(&m_semaphore)) {
            throw std::logic_error("sem_post error");
        }
    }



    Thread* Thread::GetThis() {
        return t_thread;
    }

    const std::string& Thread::GetName() {
        return t_thread_name;
    }

    void Thread::SetName(const std::string& name) {
        if(t_thread) {
            t_thread->m_name = name;
        }
        t_thread_name = name;
    }

    Thread::Thread(std::function<void()> cb, const std::string& name) 
        :m_cb(cb)
        ,m_name(name) {
        if(name.empty()) {
            m_name = "UNKNOW";
        }
        //创建线程，成功返回0
        int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
        if(rt) {
            SYLAR_LOG_ERROR(g_logger) << "pthread_create thread fail, rt=" << rt << " name=" << name;
            throw std::logic_error("pthread_create error");
        }
        
        m_semaphore.wait(); //阻塞，直到被创建的线程已经准备好执行
    }

    Thread::~Thread() {
        if(m_thread) {
            pthread_detach(m_thread);
        }
    }

    void Thread::join() {
        if(m_thread) {
            int rt = pthread_join(m_thread, nullptr);
            if(rt) {
                SYLAR_LOG_ERROR(g_logger) << "pthread_join thread fail, rt=" << rt << " name=" << m_name;
                throw std::logic_error("pthread_join error");
            }
            m_thread = 0;
        }
    }

    void* Thread::run(void* arg) {
        Thread* thread = (Thread*)arg;
        t_thread = thread;
        t_thread_name = thread->m_name;
        //获取Linux top中显示的线程id
        thread->m_id = sylar::GetThreadId();
        pthread_setname_np(pthread_self(), thread->m_name.substr(0,15).c_str());

        std::function<void()> cb;
        cb.swap(thread->m_cb);

        thread->m_semaphore.notify();

        cb();
        return 0;
    }
}