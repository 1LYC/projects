#include "box.h"
#include<QPainter>
#include<QTimer>
#include<QKeyEvent>

//OneBox是从QGraphicsObject继承而来的
OneBox::OneBox(const QColor &color) : brushColor(color) {

}


//该函数为指定后面的绘图区域的外边框
QRectF OneBox::boundingRect() const {

    qreal pen_width = 1;
    //小方块的边长为20.5像素
    return QRectF(-10-pen_width/2, -10-pen_width/2, 20+pen_width, 20+pen_width);

}

void OneBox::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){

    //贴图，看起来有质感,否则单独用颜色去看，会感觉那些方块颜色很单一
    painter->drawPixmap(-10, -10, 20, 20, QPixmap(":/images/box.gif"));
    painter->setBrush(brushColor);//设置画刷颜色
    QColor penColor = brushColor;
    penColor.setAlpha(20);//将颜色的透明度减小，使方框边界和填充色直接能区分开
    painter->setPen(penColor);//色绘制画笔
    //这里画矩形框，框内填充部分用画刷画，框外线条用画笔画
    painter->drawRect(-10, -10, 20, 20);//画矩形框
}


//在局部坐标点上返回item的shape,但是好像没有其它地方调用了该函数
QPainterPath OneBox::shape() const{

    //QPainterPath是一个绘图操作的容器
    QPainterPath path;
    path.addRect(-9.5, -9.5, 19, 19);
    return path;
}


//BoxGroup是从QGraphicsItemGroup，QObject继承而来的
BoxGroup::BoxGroup() {

    setFlags(QGraphicsItem::ItemIsFocusable);//允许设置输入焦点
    old_transform = transform();//返回当前item的变换矩阵,当BoxGroup进行旋转后，可以使用它来进行恢复
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(move_one_step()));
    current_shape = RandomShape;
}


QRectF BoxGroup::boundingRect() const {

    qreal pen_width = 1;
    return QRectF(-40-pen_width/2, -40-pen_width/2, 80+pen_width, 80+pen_width);//2*2个小方块组成一个小方块组
}


void BoxGroup::keyPressEvent(QKeyEvent *event) {

    static qreal angle = 0;
    switch(event->key())
    {
        //向下键位坠落键
        case Qt::Key_Down:{
            moveBy(0, 20);//moveBy是系统自带的函数，不需要我们自己去实现
            while(!isColliding()) {
                moveBy(0, 20);
            }
            moveBy(0, -20);//往回跳
            clear_box_group();//到达底部后就将当前方块组的4个item移除,不销毁方块组
            emit need_new_box();//发射信号，在MyView中接收
            break;}
        case Qt::Key_Left:{
            moveBy(-20, 0);
            if(isColliding()) {
                moveBy(20, 0);
            }
            break;}
        case Qt::Key_Right:{
            moveBy(20, 0);
            if(isColliding()) {
                moveBy(-20, 0);
            }
            break;}
         //实现小方块组变形
         case Qt::Key_Up:{
            setRotation(angle+90.0);
            angle = angle+90.0;
            //rotation();
            if(isColliding())
            {
                setRotation(angle-90);
                angle = angle-90.0;
                //rotation();
            }
            break;}
    }
}


//检测是否有碰撞
bool BoxGroup::isColliding() {

    QList<QGraphicsItem *> item_list = childItems();//返回子item列表
    QGraphicsItem *item;
    foreach(item, item_list) {
        if(item->collidingItems().count()>1)//collidingItems返回与当前item碰撞的子item列表
            return true;//代表至少有一个item发生了碰撞
    }
    return false;
}


//将方块组从视图中移除掉，如果有需要(即参数为true的情况下)则销毁掉
//其本质是将所有的小方块从方块组中移除掉，达到从视图中将方块组移除的目的
void BoxGroup::clear_box_group(bool destroy_box) {

    QList<QGraphicsItem *> item_list = childItems();
    QGraphicsItem *item;
    foreach(item, item_list) {
        removeFromGroup(item);//将item从方块组中移除掉
        if(destroy_box) {
            OneBox *box = (OneBox*)item;
            box->deleteLater();//当控制返回到事件循环时，该目标被删除，即销毁
        }
    }
}


//创建俄罗斯方块组，根据形状参数选择方块组的颜色和形状
void BoxGroup::create_box(const QPointF &point, BoxShape shape) {

    static const QColor color_table[7] = {
        QColor(200, 0, 0, 100), QColor(255, 200, 0, 100), QColor(0, 0, 200, 100),
        QColor(0, 200, 0, 100), QColor(0, 200, 255, 100), QColor(200, 0, 255, 100),
        QColor(150, 100, 100, 100)
    };
    int shape_id = shape; //Box_Shape是枚举型，其实也是整型，因为它相当于整型的宏定义
    if(shape == RandomShape) {
        shape_id = qrand()%7;//随机取一个颜色
    }
    QColor color = color_table[shape_id];//根据id选颜色
    QList<OneBox *> list;
    setTransform(old_transform);//恢复方块组前的变换矩阵
    for(int i = 0; i < 4; ++i) { //4个小方块组成一个方块组
        OneBox *temp  = new OneBox(color);
        list << temp;//将小方块加入list列表
        addToGroup(temp);
    }
    switch(shape_id) {
        case IShape:
        current_shape = IShape;//横着的一杆
        list.at(0)->setPos(-30, -10);
        list.at(1)->setPos(-10, -10);
        list.at(2)->setPos(10, -10);
        list.at(3)->setPos(30, -10);
        break;
        case JShape:
        current_shape = JShape;//J型
        list.at(0)->setPos(10, -10);
        list.at(1)->setPos(10, 10);
        list.at(2)->setPos(10, 30);
        list.at(3)->setPos(-10, 30);
        break;
        case LShape:
        current_shape = LShape;//L型的方块组
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(-10, 10);
        list.at(2)->setPos(-10, 30);
        list.at(3)->setPos(10, 30);
        break;
        case OShape://田字型
        current_shape = OShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(-10, 10);
        list.at(3)->setPos(10, 10);
        break;
        case SShape://S型
        current_shape = SShape;
        list.at(0)->setPos(10, -10);
        list.at(1)->setPos(30, -10);
        list.at(2)->setPos(-10, 10);
        list.at(3)->setPos(10, 10);
        break;
        case TShape: //土子型
        current_shape = TShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(30, -10);
        list.at(3)->setPos(10, 10);
        break;
        case ZShape://Z字型
        current_shape = ZShape;
        list.at(0)->setPos(-10, -10);
        list.at(1)->setPos(10, -10);
        list.at(2)->setPos(10, 10);
        list.at(3)->setPos(30, 10);
        break;
        default: break;
    }
    setPos(point);//将准备好的俄罗斯方块放入指定的位置，然后进行碰撞检测
    if(isColliding()) {
        //如果俄罗斯方块一出现后就发生了碰撞，因为它是从中间出来的，所以一开始不可能是与左右两边发生碰撞，
        //只能是与下面碰撞，因此如果发生了碰撞，说明游戏已经结束，就可以发送游戏结束信号了，且定时器停止。
        stop_timer();
        emit game_finished();
    }
}


//这个是系统里的函数，本程序中是在主函数中启动的
//其实是该子类中的timeEvent()函数调用的
void BoxGroup::startTimer(int interval) {
    timer->start(interval);//启动定时器并且设置定时器间隔，然后在BoxGroup()的构造函数中设置了该定时器的信号与槽函数
}


//每当定时器到时间了，小方块组就向下移一步
void BoxGroup::move_one_step() {
    moveBy(0, 20);//该函数是父类的函数，这里指向下移动一个单位，因为向下为正坐标
    if(isColliding()) {//发生碰撞的情况下
        moveBy(0, -20);
        clear_box_group();//将方块组移除视图
        emit need_new_box();//发生信号通知程序需要新的方块组出现
    }
}



void BoxGroup::stop_timer() {

    timer->stop();//定时器停止
}
