#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QString>
#include<QLCDNumber>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    myt=new mythread(this);
    myt->lifts=this->lifts;       //让全局子线程拥有电梯和楼梯间的信息
    //myt->Building=this->Building;   //选择电梯并且整个调度写道其他子线程里面。

    myt->mytestEleself=&(lifts[1].Elevatorself);

    singlet=new singleT[TotalFloor + 1];

    for (int i=1;i<=TotalElevator;++i) {
        singlet[i].setParent(this);

        singlet[i].lifts=&(this->lifts[i]);     //对于子线程我们只要拥有对应的电梯信息即可
        singlet[i].Building=this->Building;     //也可放到主窗口操作。

        connect(&singlet[i],&singleT::isOver,
                [=]()
        {
            singlet[i].stopImmediately();
        });//直接调用，安全退出

    }


    //主窗口的构造函数里布局，并且确定基本的功能
   //初始化电梯所有按钮。
   for(int j=1;j<=TotalElevator;j++)
   {
       //电梯内部按钮设计。
       lifts[j].setParent(this);
       lifts[j].Groupmy.setParent(this);//必须！
       lifts[j].Groupmy.move(150*j-100,560);
       lifts[j].warning.setParent(&lifts[j].Groupmy);
       lifts[j].warning2.setParent(&lifts[j].Groupmy);
       lifts[j].liftswitch.setParent(&lifts[j].Groupmy);
       lifts[j].liftswitch2.setParent(&lifts[j].Groupmy);

       //出发按钮和计时器
       lifts[j].Elevatorself.setParent(this);
       lifts[j].number1.setParent(this);

       if(j==1){lifts[j].Elevatorself.setText("电梯1");}
       if(j==2){lifts[j].Elevatorself.setText("电梯2");}
       if(j==3){lifts[j].Elevatorself.setText("电梯3");}
       if(j==4){lifts[j].Elevatorself.setText("电梯4");}
       if(j==5){lifts[j].Elevatorself.setText("电梯5");}
       lifts[j].Elevatorself.setGeometry(115*j-110,500,110,25);
       //lifts[j].Elevatorself.move(115*j-110,500);
       lifts[j].number1.move(115*j-110,530);


       //楼层按钮.
       for (int i=1;i<=TotalFloor;i++) {

           lifts[j].inButtons[i].setParent(&lifts[j].Groupmy);
           QString tempfloor=QString::number(i);
           lifts[j].inButtons[i].setText(tempfloor);
           //lifts[j].inButtons[i].setFixedSize(80,40);
           if(i<=10)
          {
               lifts[j].inButtons[i].move(40,42*i-40);
           }
           else
           {
               lifts[j].inButtons[i].move(120,42*(i-10)-40);
           }
       }
        lifts[j].Groupmy.hide();//避免一开始同时多个电梯界面重合。
   }
    //楼梯间设置。
    Building->setParent(this);
    //
    for (int i=1;i<=TotalFloor;i++) {
        Building->outButton[i].setParent(this);
        Building->outButton[i].Lup.setParent(this);
        Building->outButton[i].Ldown.setParent(this);
        Building->outButton[i].floors.setParent(this);
        Building->outButton[i].move(600-30,500-23*i);
        Building->outButton[i].Lup.move(610-30,500-23*i);
        Building->outButton[i].Ldown.move(680-30,500-23*i);
        Building->outButton[i].floors.move(740-30,500-23*i);
        //如果不需要Building和Elevator之间的信息传递，connect函数全部写到构造函数中
        connect(&Building->outButton[i].Lup,&QPushButton::clicked,
                [=]()
        {
            Building->outButton[i].Lup.setStyleSheet("background-color: rgb(175,238,238)");
            //在内部绑定更新楼层队列
            //在主窗口调用全局线程改变各个电梯的目标队列。
            myt->dealSelect(i);//启动主线程的搜索分配。
            QThread::msleep(1000);//等待搜索结果
                                  //若结果未等到则tempid的值未知，无法进行下去。
                                  //全局线程的等待事件设为100ms,子线程的事件最多等待500ms
                                  //所以留给算法的时间约为400ms.
                                  //每次按钮触发的调度时间则远比这个时间要长。
            tempid=myt->liftid;
            qDebug()<<tempid<<"选出电梯号："<<endl;
        });
        connect(&Building->outButton[i].Ldown,&QPushButton::clicked,
                [=]()
        {
            Building->outButton[i].Ldown.setStyleSheet("background-color: rgb(175,238,238)");
             myt->dealSelect(i);
             QThread::msleep(600);
             tempid=myt->liftid;
             qDebug()<<tempid<<"选出电梯号："<<endl;
        });

        Building->outButton[i].show();
    }
    //完成初始化以后开启线程myt，运行run函数.
    connect(myt,&mythread::isOver,this,&MainWindow::dealisOver);
    //在楼梯层的按钮被按下后交给全局线程去选择一个合适接收者。

    for (int i=1;i<=TotalElevator;++i) {
        connect(&singlet[i],&singleT::vary,this,&MainWindow::dealvary);
        //将按钮改变交给主窗口处理?
    }
    myt->start();
    //主线程只改变子线程内部run()函数的运行情况而不会直接调用run函数。
    //局部子线程即改变整个按钮移动响应，全程不停止扫描更新。
    for (int i=1;i<=TotalElevator;++i) {
        singlet[i].start();
    }
    //connect(ui->Test,&QPushButton::clicked,this,&MainWindow::test);
}

MainWindow::~MainWindow()
{
    qDebug() << "start destroy widget";
    //调试信息确保线程安全退出。
    //在主窗口被删除前结束子线程。
    for (int i=1;i<=TotalElevator;++i) {
        singlet[i].stopImmediately();
        singlet[i].wait();
    }

    myt->stopImmediately();
    myt->wait();
    delete ui;
    qDebug() << "end destroy widget";
}


void MainWindow::dealisOver()
{
    myt->stopImmediately();
    //这个是线程的非run函数，他运行在主线程上，所以主线程可以立马对其操作通过bool变量来结束子线程。
}


void MainWindow::dealvary(int floor)
{
    //电梯内部按钮的响应,检测其非空所以触发信号vary,此时参数floor。

    qDebug()<<"主线程处理按钮移动";
    tempfloor=floor;
    //qDebug()<<"目标楼层："<<tempfloor<<endl;全部无误。
    if(tempfloor<=0||tempfloor>TotalFloor||tempid==0)//证明是电梯内部请求
    {
        for (int i=1;i<=TotalElevator;++i) {
            qDebug()<<"检测电梯"<<i;
            if(lifts[i].nextDes.count())
            {
                //因为是有线程检测到队列不空，所以我们只要找到有有值的那一个,
                tempid=i;break;
            }
        }
    }
    qDebug()<<"目标电梯："<<tempid<<endl;

    while(lifts[tempid].now_floor<tempfloor)
    {
        QThread::msleep(1000);
        qDebug()<<"+1";
        lifts[tempid].gradual_vary(tempfloor);
    }

    while(lifts[tempid].now_floor>tempfloor)
    {
        QThread::msleep(1000);
        qDebug()<<"-1";
        lifts[tempid].gradual_vary(tempfloor);
    }

    if(!Building->TotalDes.empty())
    {
        for(QVector<int>::iterator it = Building->TotalDes.begin(); it != Building->TotalDes.end(); ++it)
        {
            if(tempfloor==*it)
            {
              Building->TotalDes.erase(it);
              qDebug()<<"Yes";
              break;
            }
        }
    }
    lifts[tempid].inButtons[tempfloor].setStyleSheet("background-color: #f5f5f5");
    Building->outButton[tempfloor].setStyleSheet("background-color: #f5f5f5");
    qDebug()<<tempfloor<<"--------";
    qDebug()<<tempid<<"-------";
    if(lifts[tempid].nextDes.count())
    {
        lifts[tempid].nextDes.pop_front();
        qDebug()<<lifts[tempid].nextDes.count()<<"队列中的剩余个数"<<endl;
    }
}


void MainWindow::test()
{
    /*int i=1;
    qDebug()<<"动不了！！";
    lifts[1].gradual_vary(20);
    while(i<20)
    {
        QThread::sleep(1);
        QPoint temp=lifts[1].Elevatorself.pos();//将电梯自身按钮上下移动。
        lifts[1].Elevatorself.move(temp.x(),temp.y()-24);
        ++i;
    }*/
}





