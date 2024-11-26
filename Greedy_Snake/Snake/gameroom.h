#ifndef GAMEROOM_H
#define GAMEROOM_H

#include <QWidget>
#include "common.h"

// 枚举蛇的移动方向
enum SnackDirect{
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
};

class GameRoom : public QWidget
{
    Q_OBJECT
public:
    explicit GameRoom(QWidget *parent = nullptr);

    // 重写绘图事件
    void paintEvent(QPaintEvent *event);

    void moveUp();//向上移动
    void moveDown();//向下移动
    void moveLeft();//向左移动
    void moveRight();//向右移动

    bool checkFail();//判断游戏是否结束
    void creatNewFood();//随机生成食物
    void setTimeout(int timeout){moveTimerOut=timeout;}
private:
    const int kSnakeNodeWidth = 20;//蛇的宽度
    const int kSnakeNodeHeight = 20;//蛇的高度
    const int kDefaultTimeout = 200;
    //QRectF表示矩形
    QList<QRectF> snakeList;//贪吃蛇链表
    QRectF foodRect;//食物节点
    QSound* sound;//bgm
    SnackDirect moveDirect = SnackDirect::UP;//蛇的默认方向
    QTimer*timer;//定时器

    int moveTimerOut = kDefaultTimeout;
    bool isGameStart = false;//是否开始游戏
};

#endif // GAMEROOM_H
