#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

#include "threadTaskPool.h"

class CMyTask : public ITask
{
public:
    int run()
    {
        for (int i = 0; i < 10;++i)
        {
            cout << taskName_ << endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        return 0;
    }
};

int main(int argc, char**argv)
{
    ThreadTaskPool pool(2,100);
    
    CMyTask *one = new CMyTask;
    one->setName("1");
    pool.addTask(one);

    //one = new CMyTask;
    //one->setName("2");
    //pool.addTask(one);

    //one = new CMyTask;
    //one->setName("3");
    //pool.addTask(one);

    //one = new CMyTask;
    //one->setName("4");
    //pool.addTask(one);


    pool.start();

    //std::this_thread::sleep_for(std::chrono::seconds(5));
    //pool.stop();
    //pool.release();
    system("pause");
    return 0;
}