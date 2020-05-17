#include "mythread.h"
#include<QDebug>
#include<QVector>
#include<QTime>
mythread::mythread(QObject *parent) : QThread(parent)
{

}

/*一个电梯是一个线程
//全局线程只是帮助选出对应线程。
//我们需要一个总的请求队列。

//整体逻辑：每当内外有一个按钮被按下的时候，调用SelectLift选择电梯并且跟新对应电梯的下一个目标。
//每次响应总会加入到一个电梯的目标队列中
//所以对于每部电梯来说只需要关注目标队列即可。
//每个按钮的改变和调试信息的输出是在电梯的目标完成时函数实现。
//这个全局线程负责启动各电梯子线程。

//那么临时电梯子线程做的事就是不断的将目标队列里的东西清楚。
//为了实现这个目标每个电梯都应该有一个他自己的目标队列。
*/

/*
全局线程算法：
//1.电梯的空闲当且仅当目标队列为空。
//电梯调度优先选取空闲的电梯。并更新它的目标队列。

//对于运行电梯，选择算法当注意应当有一定的楼层差。
//2.对于正在运行的电梯只有响应处于目标楼层和本楼层之间，才会把它更新为自己的目标楼层。【最前】
//不考虑距离各个电梯比较远近。随便选一个。
//出于安全考虑我们把楼层差而被筛掉的，放到第三种情况里面。
//3.上述两种情况任没有电梯相应。考虑对于正在运行的电梯，随便一个将其添加到它的目标队列中。【最后】
//针对2.3.情况避免全部都分配到同一个电梯。

//这是在简化了楼层按钮方向的考虑后的顺便服务策略。
*/

/*
子线程逻辑：
//子线程一直处于运行状态！！
//对于响应一个目标队列的请求后紧接着响应下一个：
//外部：首先移动函数（主线程）执行完毕，进行一系列操作。
//然后改变该层按钮的颜色（电梯内部的话需要显示面板）（主/子线程），同时调用输出打印信息（主/子线程），
//将目标队列中的相应楼层清除（主线程/子线程）
//（子线程）同时将下一个目标取出，打包信号。
//之后向主线程发出信号（包括目标楼层和方向）让电梯动起来，向目标楼层前进。
//不断循环直到目标队列为空。

//电梯内部响应应当绑定到按键上，一旦有点击立马更新相应的队列

//相应分为两种电梯内部和楼层间的。对于全局线程两者并无区别。
//在目标队列中加以区分，对两种目标完成的调试信息以及按钮改变各不相同。
//

*/


void mythread::stopImmediately()
{
    QMutexLocker locker(&m_lock);
    isDone=false;
}

void mythread::dealSelect(int floor)
{
    isSelect=true;
    nextfloor=floor;
}

int mythread::SelectLift(int floor)
{

    //一阶段。
    for (int i=1;i<=TotalElevator;++i) {
        if(lifts[i].direction==ElePause)
        {
           lifts[i].nextDes.push_back(floor);//直接更新其目标队列
           liftid=i;
           return liftid;//直接退出。
        }
    }
    //二阶段正好顺路
    for (int i=1;i<=TotalElevator;++i)
    {
        if(lifts[i].nextDes.count())
        {
            if(lifts[i].nextDes.front()>floor+1&&floor-1>lifts[i].now_floor)
            {
                lifts[i].nextDes.push_front(floor);
                liftid=i;
                return liftid;//直接退出。
            }
        }
        if(lifts[i].nextDes.count())
        {
            if(lifts[i].nextDes.front()<floor-1&&floor+1<lifts[i].now_floor)
            {
                lifts[i].nextDes.push_front(floor);
                liftid=i;
                return liftid;//直接退出。
            }
        }

    }
    //最后阶段
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    int n = qrand() % 5;    //产生5以内的随机数
    if(n==0){++n;}
    liftid=n;
    lifts[liftid].nextDes.push_back(floor);
    return liftid;
}

void mythread::run()
{
    isDone=true;

    while (isDone) {
        msleep(100);//注意匹配我们的算法选择事件不要太长也不要太短。
        //我试过了凡是涉及到了按钮移动的全部报错，但是按钮修改他的名称，修改计数器这些不会报错
        //postEvent: 可以给别的线程发送事件。事件会在目的对象所属的线程中运行。这是一个异步接口。
        //sendEvent: 仅用于同一个线程之间发送事件。目的对象必须与当前线程一样。这是一个同步接口。假如发送给属于另一个线程的对象，会报错：
        //

        /*int i=1;
        qDebug()<<"动不了！！";
        lifts[1].gradual_vary(20);
        while(i<20)
        {
            QPoint temp=lifts[1].Elevatorself.pos();//将电梯自身按钮上下移动。
            lifts[1].Elevatorself.move(temp.x(),temp.y()-24);
            ++i;
        }*/

        /*test1->setText("sdgdsdges");
        QPoint temp=test1->pos();
        test1->move(temp.x(),temp.y()-24);*/

        if(isSelect)
        {

            liftid = SelectLift(nextfloor);
            /*qDebug()<<"Yes?";
            mytestEleself->mygradual_vary();
            qDebug()<<"Yes?";
            msleep(5000);*/
            //确定电梯的同时，更新其目标队列。
            //让主线程明确取修改哪个电梯
            //qDebug()<<liftid<<endl;电梯选择没有问题。

        }
        isSelect=false;//当主线程不要求时我们就不启动select函数，只让它空循环。
    }
    qDebug()<<"子线程运行结束。";
}

singleT::singleT(QObject *parent) : QThread(parent)
{

}

void singleT::stopImmediately()
{
    QMutexLocker locker(&m_lock);
    isDone=false;
}

void singleT::run()
{
    qDebug()<<"条线程开始";
    isDone=true;
    while (isDone) {
        msleep(500);
        //只要电梯的目标队列不空就开始运行
        if(lifts->nextDes.count())
        {
            qDebug()<<"是否为空1？"<<lifts->nextDes.empty();
            int temp=lifts->nextDes.front();
            if(lifts->now_floor>temp)
            {
                lifts->setmode(EleDown);//在外部提前改变方向。
            }
            if(lifts->now_floor<temp)
            {
                lifts->setmode(EleUp);//在外部提前改变方向。
            }
            int tempsd=qAbs(lifts->now_floor-temp);
            int tempdirection=lifts->direction;
            while(tempsd--)
            {
                if(lifts->direction==Warnlift)
                {
                    tempsd++;
                    //当警报响起，电梯在这里空循环
                }
                else
                {
                    lifts->direction=tempdirection;
                    //警报消除，或者没有任何中断，不做处理，按原方向运行。
                }
                msleep(1000);
                lifts->gradual_vary(temp);
            }

            //这一句可能出问题。移动一些控件不能写道子线程里面？

            //在在线程里面只能访问主窗口里面对象的值，但是无法修改？
            //emit vary(temp);


            //由于原本操作被移动到主窗口，我们要估算这段时间，让线程休眠。
            /*int aboutsleep=0;
            aboutsleep=qAbs(lifts->now_floor-temp);
            QThread::sleep(aboutsleep);*/

            qDebug()<<"是否为空2？"<<lifts->nextDes.empty()<<endl;
            //同时如果是楼梯层的请求我们还应该更新它的目标队列。
            //为了方便我们直接在其队列中找到改元素并且直接删除。
            for(QVector<int>::iterator it = Building->TotalDes.begin(); it != Building->TotalDes.end(); ++it)
            {
                if(temp==*it)
                {
                    Building->TotalDes.erase(it);
                    break;
                }
            }
            lifts->inButtons[temp].setStyleSheet("background-color: #f5f5f5");
            Building->outButton[temp].Lup.setStyleSheet("background-color: #f5f5f5");
            Building->outButton[temp].Ldown.setStyleSheet("background-color: #f5f5f5");
            if(lifts->nextDes.count())
            {
                lifts->nextDes.pop_front();
            }
        }
        //qDebug()<<"是否为空？外部"<<lifts->nextDes.empty()<<endl;
        //qDebug()<<"完成一次运行";
        if(!lifts->nextDes.count())
        {
            lifts->setmode(ElePause);//如果目标队列为空设为暂停状态。
        }
        /*emit isOver();*/
    }
    qDebug()<<"局部子线程运行结束。";
}



