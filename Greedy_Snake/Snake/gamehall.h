#ifndef GAMEHALL_H
#define GAMEHALL_H

#include <QWidget>
#include "common.h"

QT_BEGIN_NAMESPACE
namespace Ui { class GameHall; }
QT_END_NAMESPACE

class GameHall : public QWidget
{
    Q_OBJECT

public:
    GameHall(QWidget *parent = nullptr);
    ~GameHall();

    //重写绘图事件
    void paintEvent(QPaintEvent *event);
private:
    Ui::GameHall *ui;
};
#endif // GAMEHALL_H
