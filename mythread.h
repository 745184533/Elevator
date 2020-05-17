#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include<QThread>
#include<QPushButton>
#include<QMutex>
#include"elevator.h"

class mythread : public QThread
{
    Q_OBJECT
private:
    QMutex m_lock;
    bool isDone;
    bool isSelect=false;
    int nextfloor;

public:
    int liftid;
    building *Building;
    elevator *lifts;
    QPushButton *test1;

     Ele_in_button *mytestElein;
     elevatorself *mytestEleself;
     Floor_out_button *mytestFloorout;

public:
    explicit mythread(QObject *parent = nullptr);
    void run();//线程处理函数。
    void stopImmediately();

    int SelectLift(int floor);
signals:
    void isOver();

public slots:
    void dealSelect(int floor);


};

class singleT : public QThread
{
    //事实上对这个子线程的运行有三重评判条件
    //1.主窗口给结束，他也结束
    //2.全局子线程没有传参时，可以运行！！
    //3.对应电梯的目标队列为空的时候停止运行。
    Q_OBJECT
private:
    QMutex m_lock;
    bool isDone;
    int nextfloor;
public:
    int selfid;
    building *Building;
    elevator *lifts;
    //
    Ele_in_button *mytestElein;
    elevatorself *mytestEleself;
    Floor_out_button *mytestFloorout;

public:

    explicit singleT(QObject *parent = nullptr);
    void run();//线程处理函数。
    void stopImmediately();

signals:
    void isOver();
    void vary(int floor);

public slots:

};




#endif // MYTHREAD_H
