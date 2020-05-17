# Elevator
My first Qt appiliaction
1  分析
1.1 背景分析
每个电梯里面设置必要功能键：如数字键、关门键、开门键、上行键、下行键、报警键、当前电梯的楼层数、上升及下降状态等。
每层楼的每部电梯门口，应该有上行和下行按钮和当前电梯状态的数码显示器。
五部电梯门口的按钮是互联结的，即当一个电梯按钮按下去时，其他电梯的相应按钮也就同时点亮，表示也按下去了。
所有电梯初始状态都在第一层。每个电梯如果在它的上层或者下层没有相应请求情况下，则应该在原地保持不动。
从要求来看是要将每部电梯作为一个独立的线程，不断处理它内部外部请求。从实际中考虑，一个人可以在楼道里的不同楼层按下按钮，同时在电梯内部也可以按下多个按钮（虽然他不一定使用）。因此我在设计过程中不去考虑乘客这个类，只从请求的角度出发，不去考虑这种请求是否合理，只保证电梯内部逻辑正确。
我使用Qt开发，c++语言，使用到了多线程技术。共享内存进行数据操作，但实际中通过逻辑判断避免冲突（没有去使用🔒）。
1.2 功能分析
这个系统应当能及时响应内外部按钮请求，并分配一个电梯去响应。
每部电梯应当有自己的可移动模块，内部显示板（包括楼层按钮、警报、开关门和显示器）。外部楼层每层由上下按钮。所有的按钮在被按下后会变色，在得到响应后变为原来的颜色。同时又输出提示可以去查看。
	当按下故障键以后，电梯会暂停运行，再点击消除故障，会恢复原来的行动。
1.3 算法思想
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
2	设计
2.1 数据结构设计
总体上一个电梯类，楼道类，一个全局子线程类，一个局部子线程类。所有的显示都放在一个MainWindow主窗口类上。
2.2 类结构成员
电梯类包括的按钮部分：内部20个楼层按钮（作为继承自QPushbutton的Ele_in_button）,开关门按钮（继承自QPushbutton的openclose），故障按钮（继承自QPushbutton的Warning类），电梯自身按钮（继承自QPushbutton的elevatorself），一个计数器面板（QLCDNumber），一个面板集合控件（继承自QWidget）一个目标队列（保存下一个目标楼层）。
电梯类包括的数据部分：乘坐人数、现在楼层、电梯上升或下降键.初始在第零层暂停，电梯楼层信息：内部数字键1表示有，-1表示没有请求，以及请求的发出者

楼道类的按钮部分：20个外层楼道按钮（自定义控件【包含两个上升、下降按钮、一个楼层名称标签】）.
楼道部分数据部分：20层楼的层信息（包含上下）。

线程类无论全局子线程还是局部子线程都很类似。实现有一个私有成员isDone用来控制rum()函数的运行。全局线程还需要标识所选出的电梯，所以有一个整数成员 liftid. 除此之外他们都有指向电梯类，楼道类的指针方便之后共享内存。



2.3 类操作设计
1.电梯类	
void setmode(int value) { direction = value; }//	用于调整电梯上下停状态
    void setfloor(int value) { now_floor = value; }
    void setswitch(int value) { liftswitch.switchs = value; }//	调整电梯门开关

    void inoperation(int value);		//将乘客接入电梯,同时消除楼层按钮颜色。
                                        //只能在mainwindow里面实现后面的细节

    void Delivered(int value);	//	乘客送达修改内部按钮和人员记录。同时消除电梯内部按钮颜色。

    void update_des(int flag,int value);      //更新目标楼层队列。

    bool SpaceDes(){return nextDes.empty();}        //检查队列是否为空。

    void gradual_vary(int value);       //	用于更改现在电梯楼层,子线程发出信号以后的调用。

同时补充说明组成控件的函数和connect操作。
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
这是inButtons添加点击响应
     //警告响应
     connect(&warning,&Ele_in_button::clicked,
             [=]()
     {
         warning.setStyleSheet("color:red");
         setmode(ElePause);
     });
     //消除警告
     connect(&warning2,&Ele_in_button::clicked,
             [=]()
     {
         warning.setStyleSheet("color:black");
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
对于每个按钮他们的构造函数里面我只去设置他们的固定大小。在这里不再显示。对于每个按钮的颜色、位置、文本这些全部写道电梯类的构造函数里面。
2.楼道类
void outoperation(int value, int direction);//请求发出
void delivered(int value);	//	送达修改内外记录。
同时位上行，下降按钮绑定事件
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
与电梯类类似单个按钮的大小在他们自己的构造函数里面实现。
3.主窗口类
private slots:
    void dealisOver();//处理全局子线程的销毁
    void dealvary(int floor);//处理子线程对应楼层操作。
    void test();//测试所有类函数
public:
    Ui::MainWindow *ui;
    mythread *myt;//全局线程！！
    singleT *singlet;//局部线程。
    int tempid=0;//标记算法选出的执行者
    int tempfloor=0;//标记要执行操作的楼层。

    QMutex printm;
    building *Building=new building;
    elevator *lifts=new elevator[TotalElevator + 1];
析构函数值得注意：（要确保点击关闭按钮的时候所有的子线程随之停止）
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
2.4 系统流程设计
1.所有子线程的初始化：

 
共享内存。
2.电梯和楼道的初始化：
    
3.启动线程：
 
4.按钮出发后的全局线程操作：
 
当有按钮按下isSelect 部分可以进入，之后选出一个电梯去将对应的请求分配。（注意这里的请求只是考虑楼道中的请求）【电梯内部请求已经绑定在了电梯内部按钮点击事件上，电梯内部采用先到先服务原则。】
5.局部子线程运行：
 
 

3运行情况
见打包项目的运行结果。

