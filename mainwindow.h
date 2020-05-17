#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"elevator.h"
#include"mythread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
//第一点：当信号与槽函数的参数数量相同时，它们参数类型要完全一致。
/*第二点：当信号的参数与槽函数的参数数量不同时，
 * 只能是信号的参数数量多于槽函数的参数数量，且前面相同数量的参数类型应一致，
 * 信号中多余的参数会被忽略。
 * 此外，在不进行参数传递时，信号槽绑定时也是要求信号的参数数量大于等于槽函数的参数数量。
 * 这种情况一般是一个带参数的信号去绑定一个无参数的槽函数。
*/
signals:

private slots:
    void dealisOver();
    void dealvary(int floor);//处理子线程对应楼层操作。
    void test();//测试所有类函数真的是中看不中用
public:
    Ui::MainWindow *ui;
    mythread *myt;//全局线程！！
    singleT *singlet;//局部线程。
    int tempid=0;//标记算法选出的执行者
    int tempfloor=0;//标记要执行操作的楼层。

    QMutex printm;
    building *Building=new building;
    elevator *lifts=new elevator[TotalElevator + 1];

};

#endif // MAINWINDOW_H
