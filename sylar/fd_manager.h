#ifndef __FD_MANAGER_H__
#define __FD_MANAGER_H__

#include<memory>
#include<vector>
#include"thread.h"
#include"singleton.h"

namespace sylar{


    /**
     * @brief 文件句柄上下文类
     * @details 管理文件句柄类型(是否socket),是否阻塞,是否关闭,读/写超时时间
     */
    class FdCtx : public std::enable_shared_from_this<FdCtx> {
    public:
        typedef std::shared_ptr<FdCtx> ptr;
        FdCtx(int fd);
        ~FdCtx();

        bool init();
        bool isInit() const { return m_isInit; }
        bool isSocket() const { return m_isSocket; }
        bool isClose() const { return m_isClosed; }

        void setUserNonblock(bool v) { m_userNonblock = v; }
        bool getUserNonblock() const { return m_userNonblock; }

        void setSysNonblock(bool v) { m_sysNonblock = v; }
        bool getSysNonblock() const { return m_sysNonblock; }

        /// @brief 设置超时时间
        /// @param type 套接字选项，接收SO_RCVTIMEO 或 发送SO_SNDTIMEO
        /// @param v 超时时间
        void setTimeout(int type, uint64_t v);

        /// @brief 获取超时时间
        /// @param type 套接字选项，接收SO_RCVTIMEO 或 发送SO_SNDTIMEO
        /// @return 超时时间
        uint64_t getTimeout(int type);
    private:
        /// @brief 是否初始化
        bool m_isInit: 1;
        /// @brief 是否socket
        bool m_isSocket: 1;
        /// @brief 是否hook非阻塞
        bool m_sysNonblock: 1;
        /// @brief 是否用户设置非阻塞
        bool m_userNonblock: 1;
        /// @brief 是否关闭
        bool m_isClosed: 1;
        /// @brief 文件句柄
        int m_fd;
        /// @brief 读超时毫秒数
        uint64_t m_recvTimeout;
        /// @brief 写超时毫秒数
        uint64_t m_sendTimeout;
    };

    /**
     * @brief 文件句柄管理类
     */
    class FdManager {
    public:
        typedef RWMutex RWMutexType;;
        FdManager();
        
        /**
         * @brief 获取/创建文件句柄类FdCtx
         * @param[in] fd 文件句柄
         * @param[in] auto_create 是否自动创建
         * @return 返回对应文件句柄类FdCtx::ptr
         */
        FdCtx::ptr get(int fd, bool auto_create = false);

        
        /**
         * @brief 删除文件句柄类
         * @param[in] fd 文件句柄
         */
        void del(int fd);

    private:
        RWMutexType m_mutex;
        /// @brief 文件句柄集合
        std::vector<FdCtx::ptr> m_datas;
    };

    typedef Singleton<FdManager> FdMgr; 

}


#endif