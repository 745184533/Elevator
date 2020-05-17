#include "elevator.h"
#include<QDebug>
#include<QThread>
#include<QPoint>
elevator::elevator(QObject *parent) : QObject(parent)
{
     memset(destination, -1, TotalFloor*2);
    number1.display(now_floor);
     warning.setParent(&Groupmy);
     liftswitch.setParent(&Groupmy);
     warning2.setParent(&Groupmy);
     liftswitch2.setParent(&Groupmy);

     Groupmy.setFixedSize(600,500);


     warning.isWarning=Warnlift;
     liftswitch.switchs=CloseLift;
    // update_lcd();


     for (int i=1;i<=TotalFloor;i++) {
         connect(&inButtons[i],&Ele_in_button::clicked,
                 [=]()
         {
             inButtons[i].setParent(&Groupmy);
             Groupmy.hide();
             inButtons[i].setStyleSheet("background-color: rgb(175,238,238)");
             inoperation(i);
             //只是电梯内部请求
             nextDes.push_back(i);//立马更新我们的目标队列。
             //电梯内部我们是先到先服务。
         });
     }
     //警告响应
     connect(&warning,&Ele_in_button::clicked,
             [=]()
     {
         warning.setStyleSheet("color:red");
         setmode(Warnlift);
     });
     //消除警告
     connect(&warning2,&Ele_in_button::clicked,
             [=]()
     {
         warning.setStyleSheet("color:black");
         setmode(UnWarnlift);
     });

     //实际上是控制面板显示
     connect(&liftswitch,&Ele_in_button::clicked,
             [=]()
     {
         setmode(OpenLift);
     });
     connect(&liftswitch2,&Ele_in_button::clicked,
             [=]()
     {
         setmode(CloseLift);
     });
    //显示隐藏响应
     connect(&Elevatorself,&elevatorself::clicked,
             [=]()
     {
        Groupmy.show();
     });
     connect(&liftswitch2,&openclose::clicked,
             [=]()
     {
        Groupmy.hide();
     });

     //warning.setFixedSize(150,100);
     warning.setText("故障");
     warning.move(230,100-40);
     //warning2.setFixedSize(150,100);
     warning2.setText("消除故障");
     warning2.move(230,250-40);
     //liftswitch.setFixedSize(100,50);
     liftswitch.setText("开门");
     liftswitch.move(230,350-40);
    // liftswitch2.setFixedSize(100,50);
     liftswitch2.setText("关门");
     liftswitch2.move(320,350-40);

     //Elevatorself.setFixedSize(110,25);
     number1.setFixedSize(110,100);

}

void elevator::inoperation(int value)
{
    qDebug() << "电梯内部第：" << value<<"层发出请求"<<endl;
    destination[value][0] = 1; //	电梯内部按钮。   
    num_customer++;
}

void elevator::Delivered(int value)
{
    //清理部分人以及按钮。
    destination[value][1] = -1;
    destination[value][0] = -1;
    qDebug() << "目标楼层，第：" << value << "层到达。" << endl;
    inButtons[value].setStyleSheet("background-color: #f5f5f5");
    num_customer--;
    //每次到达目的地后不一定会暂停。
    //只有检查到目标队列为空的时候才会暂停。

}

void elevator::gradual_vary(int value)
{
   QPoint temp=Elevatorself.pos();//将电梯自身按钮上下移动。
    //完全动不起来？坐标获取没有问题问题在Move函数。层数也没问题一直跟着变。
    if(direction==EleUp)
    {
        qDebug()<<"操作向上";
        qDebug()<<"开始";
        now_floor++;
        qDebug()<<now_floor<<"现在层数";
        qDebug()<<"移动一次"<<endl;
        Elevatorself.move(temp.x(),temp.y()-24);
        number1.display(now_floor);
    }
    if(direction==EleDown)
    {
        now_floor--;
        Elevatorself.move(temp.x(),temp.y()+24);
        number1.display(now_floor);
    }
    //一秒一秒变化。随时间变化写到子线程里面。
}

void elevator::update_des(int flag, int value)
{
    if(flag==FirstChange)
    {
        nextDes.push_front(value);
    }
    if(flag==LastChange)
    {
        nextDes.push_back(value);
    }
}

void elevatorself::mygradual_vary()
{
    qDebug()<<"我太累了";
    QPoint temp=this->pos();//将电梯自身按钮上下移动。
    //this->move(temp.x(),temp.y()-24);
}

Ele_in_button::Ele_in_button(QWidget *parent) : QPushButton(parent)
{
    this->setStyleSheet("background-color: #f5f5f5");
    this->setFixedSize(80,40);
    getid();
}

int Ele_in_button::getid()
{
    QString temp=this->text();
    id=temp.toInt();
    return id;
}

Warning::Warning(QWidget *parent) : QPushButton(parent)
{
    this->setFixedSize(150,100);
}

openclose::openclose(QWidget *parent) : QPushButton(parent)
{
    this->setFixedSize(100,50);
}

elevatorself::elevatorself(QWidget *parent) : QPushButton(parent)
{
    this->setFixedSize(110,23);
}
