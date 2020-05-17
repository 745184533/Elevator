#include "elevator.h"
#include<QDebug>

building::building(QObject *parent) : QObject(parent)
{
    memset(floor, -1, TotalFloor*12);
    for (int i=1;i<=TotalFloor;i++) {

        outButton[i].id=i;
        outButton[i].getid_dir();//设置楼层号字样
        outButton[i].setFixedSize(250,19);  //设置总按钮的大小。
        /*outButton[i].Lup.setStyleSheet("background-color: #f5f5f5");
        outButton[i].Ldown.setStyleSheet("background-color: #f5f5f5");*/


        connect(&outButton[i].Lup,&QPushButton::clicked,
                [=]()
        {

            outoperation(i,EleUp);
            TotalDes.push_back(i);
        });
        connect(&outButton[i].Ldown,&QPushButton::clicked,
                [=]()
        {

            outoperation(i,EleDown);
            TotalDes.push_back(i);
        });
    }

}

void building::outoperation(int value, int direction)
{
    counts++;
    qDebug()<<"在楼梯间第："<<value<<"层发出请求:";
    if(direction==EleUp){qDebug()<<"向上"<<endl;}
    if(direction==EleDown){qDebug()<<"向下"<<endl;}
    floor[value][0] = direction;
}

void building::delivered(int value)
{
    qDebug()<<"第"<<value<<"层的请求，已经被电梯接受"<<endl;
    //模拟乘客走进楼梯间此时要将按钮颜色调整回来。
    outButton[value].Lup.setStyleSheet("background-color: #f5f5f5");
    outButton[value].Ldown.setStyleSheet("background-color: #f5f5f5");
    floor[value][0]=-1;
}

Floor_out_button::Floor_out_button(QWidget *parent) : QWidget(parent)
{

    Lup.setParent(this);
    Ldown.setParent(this);
    floors.setParent(this);
    //定义集合按钮时仍要注意这个指定各按钮的父类为我们定义的widget
    //但到主窗口仍要写一遍！！！！吐了
    Lup.setText("up");//设置向上按钮
    Ldown.setText("down");//设置向下按钮
    Lup.setFixedSize(50,19);
    Ldown.setFixedSize(50,19);
    floors.setFixedSize(50,19);
    connect(&Lup,&QPushButton::clicked,
            [=]()
    {
        Lup.setStyleSheet("background-color: rgb(175,238,238)");
        direction=EleUp;
    });
    connect(&Ldown,&QPushButton::clicked,
            [=]()
    {
        Ldown.setStyleSheet("background-color: rgb(175,238,238)");
        direction=EleDown;
    });
}

void Floor_out_button::getid_dir()
{
    QString temp=QString::number(id);
    floors.setText(temp);
}
