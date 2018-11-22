#include "ThreadTaskPool.h"

ITask::ITask()
    :arg_(NULL)
{
}


ITask::~ITask()
{
}

void ITask::setArg(TASKARG arg)
{
    arg_ = arg;
}

void ITask::setName(const std::string& strName)
{
    taskName_ = strName;
}

void ITask::SetPriority(int priority)
{
    priority_ = priority;
}

int ITask::GetPriority() const
{
    return priority_;
}

void ITask::SetAutoRelease(bool bAutoRelease)
{
    bAutoRelease_ = bAutoRelease;
}

bool ITask::IsAutoRelease()
{
    return bAutoRelease_;
}

void TaskContainer::push(ITask * t)
{
    task_container_.push(t);
}

ITask* TaskContainer::top()
{
    return task_container_.top();
}

void TaskContainer::pop()
{
    task_container_.pop();
}

bool TaskContainer::empty()
{
    return task_container_.empty();
}

std::priority_queue<ITask*>::size_type TaskContainer::size()
{
    return task_container_.size();
}

ThreadTaskPool::ThreadTaskPool(int threadNum, int wait_seconds)
    :seconds_wait(wait_seconds)
{
    if (threadNum <= 0)
    {
        threadNum = std::thread::hardware_concurrency();
        threadNum = (threadNum == 0 ? 2 : threadNum);
    }

    //创建空闲线程
    for (int i = 0; i != threadNum; ++i)
    {
        CThreadTask* pthread = new CThreadTask(this);
        idle_threads_.push(pthread);
    }
}

ThreadTaskPool::~ThreadTaskPool()
{
    stop();
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void ThreadTaskPool::start()
{
    //创建线程池管理线程
    thread_ = std::thread(&ThreadTaskPool::run, this);
    thread_.detach();
}

void ThreadTaskPool::run()
{
    while (true)
    {
        if (stop_.load())
            break;

        {
            std::unique_lock<std::mutex> ulk(task_mutex_);
            this->cond_.wait_for(ulk, seconds_wait, [this]{ return/* stop_.load() ||*/ !this->tasks_.empty(); });

            if (stop_.load())
                break;


            {
                std::unique_lock<std::mutex> ulk(thread_mutex_);
                if (idle_threads_.empty() || tasks_.empty())
                {
                    continue;
                }

                CThreadTask* p = idle_threads_.top();
                idle_threads_.pop();
                ITask* task = tasks_.top();
                tasks_.pop();
                p->Assign(task);
                p->Start();
            }
        }
    }
    printf("exit loop\n");
}

void ThreadTaskPool::addTask(ITask* task)
{   
    if (task)
    {
        std::unique_lock<std::mutex> ulk(task_mutex_);
        tasks_.push(task);
        cond_.notify_all();
    } 
}

size_t ThreadTaskPool::taskNum()
{
    size_t sz;
    {
        std::unique_lock<std::mutex> ulk(task_mutex_);
        sz = tasks_.size();
    }
    return sz;
}

size_t ThreadTaskPool::idlThreadNum()
{
    size_t sz;
    {
        std::unique_lock<std::mutex> ulk(thread_mutex_);
        sz = idle_threads_.size();
    }
    return sz;
}

void ThreadTaskPool::stop()
{
    stop_.store(true);
    cond_.notify_all();
    release();
}

void ThreadTaskPool::release()
{
    //空闲的线程立刻销毁,运行中的线程在其运行完成后销毁
    std::unique_lock<std::mutex> ulk(thread_mutex_);
    CThreadTask* pTaskthread = NULL;
    do
    {
        if (idle_threads_.empty())
            break;

        pTaskthread = idle_threads_.top();
        if (pTaskthread){
            delete pTaskthread;
            idle_threads_.pop();
        }
        else{
            idle_threads_.pop();
        }
    } while (pTaskthread);
}

void ThreadTaskPool::SetThreadidl(CThreadTask *myThread)
{
    if (stop_)
    {
        delete myThread;
    }
    else
    {
        std::unique_lock<std::mutex> ulk(thread_mutex_);
        idle_threads_.push(myThread);
        cond_.notify_all();
    }
   
}

int CThreadTask::s_threadnumber = 0;

CThreadTask::CThreadTask(ThreadTaskPool *pool)
    :threadpool_(pool), isdetach_(true)
{
    s_threadnumber++;
    threadid_ = s_threadnumber;
}

CThreadTask::~CThreadTask()
{

}

void CThreadTask::Assign(ITask *task)
{
    task_ = task;
}

void CThreadTask::Run()
{
    task_->run();
    if (task_->IsAutoRelease())
    {
        task_->release();
    }
    threadpool_ ->SetThreadidl(this);
}

void CThreadTask::Start()
{
    thread_ = std::thread(&CThreadTask::Run, this);
    if (isdetach_ == true)
        thread_.detach();
    else
        thread_.join();
}
