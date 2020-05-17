#ifndef ELEVATOR_H
#define ELEVATOR_H

#include <QObject>
#include <QWidget>
#include<QPushButton>
#include<QMutex>
#include<QLabel>
#include<QLCDNumber>
#include<QVector>
#define Warnlift 99
#define UnWarnlift 100
#define TotalFloor 20
#define TotalElevator 5
#define EleUp 1
#define EleDown 0
#define ElePause -1
#define InLift 1
#define OutLift 0
#define OpenLift 1
#define CloseLift 0
#define FirstChange 0
#define LastChange 1

/*
    building 的队列还未完成更新
*/


//按钮类
class Ele_in_button : public::QPushButton
{
    Q_OBJECT
public:
    int id;
public:
    int getid();
    explicit Ele_in_button(QWidget *parent = nullptr);

signals:

public slots:
};

class elevatorself : public::QPushButton
{
    Q_OBJECT
public:
    explicit elevatorself(QWidget *parent = nullptr);
    void mygradual_vary();

signals:

public slots:
};

class Floor_out_button : public::QWidget
{
    Q_OBJECT
public:
    QPushButton Lup,Ldown;
    QLabel floors;

    int id;
    int direction;
public:
    void getid_dir();
    explicit Floor_out_button(QWidget *parent = nullptr);

signals:

public slots:
};

class Warning : public::QPushButton
{
    Q_OBJECT
public:
    int isWarning;
public:
    explicit Warning(QWidget *parent = nullptr);

signals:

public slots:
};

class openclose : public::QPushButton
{
    Q_OBJECT
public:
    int switchs;
public:
    explicit openclose(QWidget *parent = nullptr);

signals:

public slots:
};

//主类
class elevator : public QObject
{
    Q_INVOKABLE
    Q_OBJECT
public:
    int next_des=0;

    int num_customer = 0;//	乘坐人数
    int now_floor = 0;//	现在楼层
    int direction = ElePause;//	电梯上升或下降键.初始在第零层暂停。
    int destination[TotalFloor + 2][2]; //		内部数字键1表示有，-1表示没有请求，以及请求的发出者

    QVector<int> nextDes;
    //按钮成员
    QWidget Groupmy;
    Ele_in_button inButtons[TotalFloor + 2];

    Warning warning;	//		1表示按响警报器。
    Warning warning2;
    openclose liftswitch;//	电梯开关键。
    openclose liftswitch2;

    QLCDNumber number1;
    elevatorself Elevatorself;

    QMutex elevator_mtx;
public:
    explicit elevator(QObject *parent = nullptr);
    void setmode(int value) { direction = value; }//	用于调整电梯上下停状态
    void setfloor(int value) { now_floor = value; }
    void setswitch(int value) { liftswitch.switchs = value; }//	调整电梯门开关

    void inoperation(int value);		//将乘客接入电梯,同时消除楼层按钮颜色。
                                        //只能在mainwindow里面实现后面的细节

    void Delivered(int value);	//	乘客送达修改内部按钮和人员记录。同时消除电梯内部按钮颜色。

    void update_des(int flag,int value);      //更新目标楼层队列。

    bool SpaceDes(){return nextDes.empty();}        //检查队列是否为空。

    void gradual_vary(int value);       //	用于更改现在电梯楼层,子线程发出信号以后的调用。

signals:

public slots:


};

class building : public QObject
{
    Q_OBJECT
public:
    friend class elevator;
    QVector<int> TotalDes;


    int counts = 0;
    int floor[TotalFloor + 2][3];//			所有外部楼层的请求,用1表示向上的请求，用0表示向下的请求,-1表示没有请求
                                //			同时记录乘客id
                                //			记录乘客最终目的地
    Floor_out_button outButton[TotalFloor +2];
    QMutex floor_mtx;
public:
     void outoperation(int value, int direction);
     void delivered(int value);	//	乘客送达修改内外记录。
public:
    explicit building(QObject *parent = nullptr);

signals:

public slots:
};


#endif // ELEVATOR_H


