#include "gameroom.h"
#include "gameselect.h"
GameRoom::GameRoom(QWidget *parent) : QWidget(parent)
{
    this->setWindowIcon(QIcon(":res/ico.png"));
    this->setWindowTitle("游戏房间");
    this->setFixedSize(1000,800);
    //初始化
    snakeList.push_back(QRectF(800*0.9,800*0.5,\
                               kSnakeNodeWidth,kSnakeNodeHeight));
    moveUp();
    moveUp();
    //创建食物
    creatNewFood();
    timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        int cnt = 1;
        if(snakeList.front().intersects(foodRect)){
            creatNewFood();
            cnt++;
        }
        //蛇吃豆子，豆子占一个，移动占一格，一共走2格
        while(cnt--){
            switch (moveDirect) {
            case SnackDirect::UP:
                moveUp();
                break;
            case SnackDirect::DOWN:
                moveDown();
                break;
            case SnackDirect::LEFT:
                moveLeft();
                break;
            case SnackDirect::RIGHT:
                moveRight();
                break;
            }
        }
        snakeList.pop_back();//删除尾部
        update();
    });

    //开始游戏 暂停游戏
    QFont font("楷体",20);
    QPushButton* start_button = new QPushButton(this);
    QPushButton* stop_button = new QPushButton(this);
    start_button->move(830,150);
    start_button->setText("开始");
    start_button->setFont(font);
    stop_button->setText("暂停");
    stop_button->move(830,240);
    stop_button->setFont(font);


    connect(start_button,&QPushButton::clicked,this,[=](){
        isGameStart = true;
        timer->start(moveTimerOut);//启动定时器
        sound = new QSound(":res/bgm.wav");
        sound->play();
        sound->setLoops(-1);//表示一直循环
    });

    connect(stop_button,&QPushButton::clicked,this,[=](){
        isGameStart = false;
        timer->stop();
        sound->stop();
    });

    // 方向控制
    QPushButton* up = new QPushButton(this);
    QPushButton* down = new QPushButton(this);
    QPushButton* left = new QPushButton(this);
    QPushButton* right = new QPushButton(this);

    up->move(865,395);
    down->move(865,485);
    left->move(800,440);
    right->move(930,440);
    up->setText("↑");
    down->setText("↓");
    left->setText("←");
    right->setText("→");

    //去掉边框
    up->setStyleSheet("QPushButton{border:0px;}");
    down->setStyleSheet("QPushButton{border:0px;}");
    left->setStyleSheet("QPushButton{border:0px;}");
    right->setStyleSheet("QPushButton{border:0px;}");

    QFont ft("宋体",30);
    up->setFont(ft);
    down->setFont(ft);
    left->setFont(ft);
    right->setFont(ft);

    connect(up,&QPushButton::clicked,this,[=](){
        if(moveDirect!=SnackDirect::DOWN){
            moveDirect = SnackDirect::UP;
        }
    });
    connect(down,&QPushButton::clicked,this,[=](){
        if(moveDirect!=SnackDirect::UP){
            moveDirect = SnackDirect::DOWN;
        }
    });
    connect(right,&QPushButton::clicked,this,[=](){
        if(moveDirect!=SnackDirect::LEFT){
            moveDirect = SnackDirect::RIGHT;
        }
    });
    connect(left,&QPushButton::clicked,this,[=](){
        if(moveDirect!=SnackDirect::RIGHT){
            moveDirect = SnackDirect::LEFT;
        }
    });

    //退出游戏
    QPushButton* exit_button = new QPushButton(this);
    exit_button->move(830,700);
    exit_button->setText("退出");
    exit_button->setFont(font);

    QMessageBox* msg = new QMessageBox(this);
    QPushButton* ok = new QPushButton("ok");
    QPushButton* cancel = new QPushButton("cancel");

    msg->addButton(ok,QMessageBox::AcceptRole);
    msg->addButton(cancel,QMessageBox::RejectRole);

    msg->setWindowTitle("退出游戏");
    msg->setText("确认退出游戏吗?");

    connect(exit_button,&QPushButton::clicked,this,[=](){
        msg->show();
        msg->exec();//事件轮询
        // 退出gameroom界面是回到gameselect界面
        GameSelect* sl = new GameSelect;
        QSound::play(":res/click.wav");
        if(msg->clickedButton() == ok){
            this->close();
            sl->show();

        }
        else{
            msg->close();
        }
    });
}

void GameRoom::paintEvent(QPaintEvent *event)
{
    QPainter p(this);//画家
    QPixmap pix;//绘画设备
    //游戏运行区
    pix.load(":res/game_room.png");
    p.drawPixmap(0,0,800,800,pix);
    //游戏控制区
    pix.load(":res/bg1.png");
    p.drawPixmap(800,0,200,800,pix);

    //渲染蛇
    //step1:蛇头
    if(moveDirect==SnackDirect::UP){
        pix.load(":res/up.png");
    }
    else if(moveDirect==SnackDirect::RIGHT){
        pix.load(":res/right.png");
    }
    else if(moveDirect==SnackDirect::LEFT){
        pix.load(":res/left.png");
    }
    else{
        pix.load(":res/down.png");
    }
    QRectF front = snakeList.front();
    p.drawPixmap(front.x(),front.y(),front.width(),front.height(),pix);

    //step2:蛇身
    pix.load(":res/Bd.png");
    for(int i = 1;i < snakeList.size()-1;i++){
      QRectF body = snakeList.at(i);
      p.drawPixmap(body.x(),body.y(),body.width(),body.height(),pix);
    }
    //step:蛇尾
    QRectF tail = snakeList.back();
    p.drawPixmap(tail.x(),tail.y(),tail.width(),tail.height(),pix);

    //渲染食物
    pix.load(":res/food.png");
    p.drawPixmap(foodRect.x(),foodRect.y(),foodRect.width(),foodRect.height(),pix);

    //渲染分数
    pix.load(":res/score_bg.png");
    p.drawPixmap(this->width()*0.86,this->height()*0.06,90,40,pix);

    QPen pen;
    pen.setColor(Qt::red);
    p.setPen(pen);
    QFont font("方正舒体",21);
    p.setFont(font);
    p.drawText(this->width()*0.9,this->height()*0.11,QString("%1").arg(snakeList.size()));

    //往文件中写分数
    int c = snakeList.size();
    QFile file("C:/Users/46285/Desktop/Greedy_Snake/Snake/1.txt");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QTextStream out(&file);
        int num = c;
        out << num;
        file.close();//关闭文件
    }

    //渲染游戏失败效果
    if(checkFail()){
        pen.setColor(Qt::red);
        p.setPen(pen);
        QFont font("方正舒体",50);
        p.setFont(font);
        p.drawText(this->width()*0.1,this->height()*0.5,QString("GAME OVER!"));
        timer->stop();
        sound->stop();
    }
}

//向上移动
void GameRoom::moveUp()
{
    QPointF leftTop;//左上角
    QPointF rightBottom;// 右下角坐标

    QRectF snakeNode = snakeList.front();//头节点
    int headX = snakeNode.x();
    int headY = snakeNode.y();

    //考虑穿墙问题
    if(headY < 0){//穿墙了
        leftTop = QPointF(headX,800-kSnakeNodeHeight);
    }
    else{
        leftTop = QPointF(headX,headY-kSnakeNodeHeight);
    }
    rightBottom = leftTop + QPointF(kSnakeNodeWidth,kSnakeNodeHeight);

    snakeList.push_front(QRectF(leftTop,rightBottom));
}

//向下移动
void GameRoom::moveDown()
{
    QPointF leftTop;//左上角
    QPointF rightBottom;// 右下角坐标

    QRectF snakeNode = snakeList.front();//头节点
    int headX = snakeNode.x();
    int headY = snakeNode.y();

    //考虑穿墙问题
    if(headY > 800){//穿墙了
        leftTop = QPointF(headX,0);
    }
    else{
        //leftTop = QPointF(headX,headY+kSnakeNodeHeight);
        leftTop = snakeNode.bottomLeft();
    }
    rightBottom = leftTop+QPointF(kSnakeNodeWidth,kSnakeNodeHeight);
    snakeList.push_front(QRectF(leftTop,rightBottom));
}

void GameRoom::moveLeft()
{
    QPointF leftTop;//左上角
    QPointF rightBottom;// 右下角坐标

    QRectF snakeNode = snakeList.front();//头节点
    int headX = snakeNode.x();
    int headY = snakeNode.y();

    //考虑穿墙问题
    if(headX < 0){//穿墙了
        leftTop = QPointF(800-kSnakeNodeWidth,headY);
    }
    else{
        leftTop = QPointF(headX-kSnakeNodeWidth,headY);
    }
    rightBottom = leftTop+QPointF(kSnakeNodeWidth,kSnakeNodeHeight);
    snakeList.push_front(QRectF(leftTop,rightBottom));
}

void GameRoom::moveRight()
{
    QPointF leftTop;//左上角
    QPointF rightBottom;// 右下角坐标

    QRectF snakeNode = snakeList.front();//头节点
    int headX = snakeNode.x();
    int headY = snakeNode.y();

    //考虑穿墙问题
    if(headX > 760){//穿墙了
        leftTop = QPointF(0,headY);
    }
    else{
        //leftTop = QPointF(headX+kSnakeNodeWidth,headY);
        leftTop = snakeNode.topRight();
    }
    rightBottom = leftTop+QPointF(kSnakeNodeWidth,kSnakeNodeHeight);
    snakeList.push_front(QRectF(leftTop,rightBottom));
}

bool GameRoom::checkFail()
{
    for(int i = 0;i < snakeList.size();i++){
        for(int j = i+1;j < snakeList.size();j++){
            if(snakeList.at(i) == snakeList.at(j)){
                return true;//表示游戏失败
            }
        }
    }
    return false;
}

void GameRoom::creatNewFood()
{
    foodRect = QRectF(qrand()%760,qrand()%800,\
                      kSnakeNodeWidth,kSnakeNodeHeight);
}
