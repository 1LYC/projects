#ifndef BOX_H
#define BOX_H
#include <QGraphicsObject>
#include<QGraphicsItemGroup>

class OneBox :public QGraphicsObject
{
public:
    OneBox(const QColor &color = Qt::red);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QPainterPath shape() const;

private:
    QColor brushColor;

};


class BoxGroup : public QObject,public QGraphicsItemGroup
{
    Q_OBJECT
public:
    enum BoxShape {IShape, JShape, LShape, OShape, SShape, TShape, ZShape, RandomShape};//8中俄罗斯方框形状
    BoxGroup();
    QRectF boundingRect() const;//在函数后用const表示不能改变类的成员
    void clear_box_group(bool destroy_box = false);
    void create_box(const QPointF &point, BoxShape shape = RandomShape);//在函数的申明处可以将参数设定为默认值，定义处不需要
    bool isColliding();
    BoxShape getCurrentShape() {return current_shape;}//获得当前俄罗斯方块的形状


protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void need_new_box();
    void game_finished();

public slots:
    void move_one_step();
    void startTimer(int interval);
    void stop_timer();

private:
    BoxShape current_shape;
    QTransform old_transform;
    QTimer *timer;

};

#endif // BOX_H
