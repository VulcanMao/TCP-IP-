#pragma once

/*
*fuc:C++ 11 任务线程池封装类
*date:2017.09.14
*author:maozhen
*/

#include <thread>
#include <mutex>

#include <list>
#include <atomic>
#include <deque>
#include <stack>
#include <queue>
#include <iostream>
#include <condition_variable>

typedef void* TASKARG;

class ITask
{
public:
    enum Priority
    {
        Idle = 0,       //空闲
        Normal = 5,     //普通
        High = 10,       //高
        Realtime = 50,   //实时
    };
    //重载预算符
    friend bool operator<(ITask  &lv, ITask &rv)
    {
        return lv.priority_ < rv.priority_;
    }
    ITask();
    virtual ~ITask();
    void setArg(TASKARG arg);
    void setName(const std::string& strName);
    virtual int run() = 0;
    void SetPriority(int priority);
    int GetPriority() const;
    void SetAutoRelease(bool bAutoRelease);
    bool IsAutoRelease();
    virtual void release(){ delete this; }
    
protected:
    int   priority_ = Normal;        //任务优先级
    TASKARG     arg_;                //任务参数
    std::string taskName_;           //任务名称
    bool bAutoRelease_ = true;       //是否自动删除*
};

struct PCmp
{
    bool operator () (ITask const *x, ITask const *y)
    {
        return x->GetPriority() < y->GetPriority();
    }
};

//任务容器
class TaskContainer
{
public:
    TaskContainer(){};
    ~TaskContainer(){};
    void push(ITask *);
    ITask* top();
    void pop();
    bool empty();
    std::priority_queue<ITask*, std::vector<ITask*>, PCmp >::size_type size();
private:
    std::priority_queue<ITask*, std::vector<ITask*>, PCmp > task_container_;
};

class ThreadTaskPool;

class CThreadTask
{
public:
    ~CThreadTask();
public:
    CThreadTask(ThreadTaskPool *pool);
    void Assign(ITask *task);
    void Run();
    void Start();
    void setDetach(bool isdetach);
private:
    static int  s_threadnumber;
    ThreadTaskPool *threadpool_;
    int threadid_ = 0;
    bool isdetach_;
    ITask *task_;
    std::thread thread_;
};



class ThreadTaskPool
{
public:
    explicit ThreadTaskPool(int threadNum = 0, int wait_seconds = 1);
    ~ThreadTaskPool();
public:
    void start();
    void run();
    void addTask(ITask* task);
    size_t taskNum();
    size_t idlThreadNum();
    void stop();
    void release();
    void SetThreadidl(CThreadTask *myThread);
private:
    ThreadTaskPool& operator=(const ThreadTaskPool&);
    ThreadTaskPool(const ThreadTaskPool&);
private:
    std::atomic<bool>           stop_ = false;  // 是否关闭提交
    std::mutex                  task_mutex_;    // 任务队列锁
    std::mutex                  thread_mutex_;  // 线程队列锁
    std::condition_variable     cond_;          // 条件阻塞
    TaskContainer               tasks_;         // 任务队列
    std::thread                 thread_;        // 调度线程
    std::stack<CThreadTask*>    idle_threads_;  // 空闲线程队列

    typedef std::chrono::duration<int> seconds_type;
    seconds_type seconds_wait;
};

