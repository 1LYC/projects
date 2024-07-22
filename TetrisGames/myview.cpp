#include "myview.h"
#include"box.h"
#include<QIcon>
#include<QPropertyAnimation>
#include<QGraphicsBlurEffect>
#include<QTimer>
#include<QPushButton>
#include<QGraphicsProxyWidget>
#include<QApplication>
#include<QLabel>
#include<QFileInfo>

static const qreal INITSSPEED = 500;//游戏的初始化速度

MyView::MyView(QWidget *parent)
{
    init_view();
}

void MyView::start_game()
{
    game_welcome_text->hide();
    start_button->hide();
    option_button->hide();
    help_button->hide();
    exit_button->hide();
    mask_widget->hide();
    init_game();
}

void MyView::clear_full_rows()
{
    for(int y = 429; y > 50; y -= 20) {
        //每隔20行取一个item出来，括号里面的参数不能弄错，否则没有方块消失的效果
        QList<QGraphicsItem *> list = scene()->items(199, y, 202, 22, Qt::ContainsItemShape,Qt::DescendingOrder);//返回指定区域内所有可见的item
        if(list.count() == 10) {   //如果一行已满，则销毁该行的所有小方块
            foreach(QGraphicsItem *item, list) {
                OneBox *box = (OneBox *) item;
//                box->deleteLater();
                //先为小方块使用了模糊图形效果，然后为其添加了放大再缩小的属性动画，等动画指定结束后才调用deleteLater()槽
                QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
                box->setGraphicsEffect(blurEffect);
                QPropertyAnimation *animation = new QPropertyAnimation(box,"scale");
                animation->setEasingCurve(QEasingCurve::OutBounce);
                animation->setDuration(250);
                animation->setStartValue(4);
                animation->setEndValue(0.25);
                animation->start(QAbstractAnimation::DeleteWhenStopped);
                //connect(animation,SIGNAL(finished()),box,SLOT(deleteLater()));
                connect(animation,&QPropertyAnimation::finished,box,&OneBox::deleteLater);
            }
            rows << y;//将满行的行号保存到rows中
        }
    }

    //如果满行，则下移上面的方块
    if(rows.count()>0) {
//     move_box();
        QTimer::singleShot(400,this,SLOT(move_box()));
    }
    else {
        //没有满行，则新出现提示方块，且提示方块出更新新的提示方块
        box_group->create_box(QPointF(300, 70), next_box_group->getCurrentShape());
        next_box_group->clear_box_group(true);
        next_box_group->create_box(QPointF(500, 70));//
    }

}

void MyView::move_box()
{
    for(int i = rows.count(); i > 0; --i) {
        int row = rows.at(i-1);//取出满行的行号,从最上面的位置开始
        //取出从区域上边界到当前满行之间所形成的矩形区域
        foreach(QGraphicsItem *item, scene()->items(199, 49, 202, row-47, Qt::ContainsItemShape,Qt::DescendingOrder)) {
            item->moveBy(0, 20);
        }
    }
    //更新分数
    update_score(rows.count());
    //出现新的方块组
    rows.clear();
    box_group->create_box(QPointF(300, 70), next_box_group->getCurrentShape());
    next_box_group->clear_box_group(true);
    next_box_group->create_box(QPoint(500, 70));
}

void MyView::game_over()
{
    //游戏结束
    pause_button->hide();
    show_menu_button->hide();
    mask_widget->show();
    game_over_text->show();
    restart_button->setPos(370, 200);
    finish_button->show();
}

void MyView::restartGame()
{
    mask_widget->hide();
    game_over_text->hide();
    finish_button->hide();
    restart_button->setPos(600, 150);

    //销毁当前方块组和当前方块中的所有小方块
    next_box_group->clear_box_group(true);
    box_group->clear_box_group();
    box_group->hide();
    foreach(QGraphicsItem *item, scene()->items(199, 49, 202, 402,Qt::ContainsItemBoundingRect,Qt::DescendingOrder)) {
        scene()->removeItem(item);
        OneBox *box = (OneBox*)item;
        box->deleteLater();
    }
    init_game();
}

void MyView::finishGame()
{
    game_over_text->hide();
    finish_button->hide();
    restart_button->setPos(600, 150);
    restart_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    gameScoreText->hide();
    gameLevelText->hide();
    top_line->hide();
    bottom_line->hide();
    left_line->hide();
    right_line->hide();

    next_box_group->clear_box_group(true);
    box_group->clear_box_group();
    box_group->hide();
    foreach(QGraphicsItem *item, scene()->items(199, 49, 202, 402,Qt::ContainsItemBoundingRect,Qt::DescendingOrder)) {
        scene()->removeItem(item);
        OneBox *box = (OneBox*)item;
        box->deleteLater();
    }

    mask_widget->show();
    game_welcome_text->show();
    start_button->show();
    option_button->show();
    help_button->show();
    exit_button->show();
    scene()->setBackgroundBrush(QPixmap(":/"));
}

void MyView::pauseGame()
{
    box_group->stop_timer();//中断游戏最主要的是停止方块下移的定时器工作
    restart_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    mask_widget->show();
    game_pause_text->show();
    return_button->show();
}

void MyView::returnGame()
{
    return_button->hide();
    game_pause_text->hide();
    mask_widget->hide();
    restart_button->show();
    pause_button->show();
    show_menu_button->show();
    box_group->startTimer(game_speed);
}



void MyView::init_view()
{
    setRenderHint(QPainter::Antialiasing);//使用抗锯齿的方式渲染
    setCacheMode(CacheBackground);//设置缓存背景，这样可以加快渲染速度
    setWindowTitle(tr("Teris游戏"));
    setWindowIcon(QIcon(":/images/icon.png"));//设置标题处的图标
    setMinimumSize(810, 510);  //2者设置成一样说明视图尺寸不能再更改
    setMaximumSize(810, 510);
    QGraphicsScene *scene = new QGraphicsScene;//新建场景指针
    scene->setSceneRect(5, 5, 800, 500);//场景大小
    scene->setBackgroundBrush(QPixmap(":/"));
    setScene(scene);//设置场景

    //俄罗斯方块可移动区域外界的4条线,与外界预留3个像素是为了方便进行碰撞检测
    top_line = scene->addLine(197, 27, 403, 27);
    bottom_line = scene->addLine(197, 453, 403, 453);
    left_line = scene->addLine(197, 27, 197, 453);
    right_line = scene->addLine(403, 27, 403, 453);

    //添加当前方块组
    box_group = new BoxGroup;//通过新建BoxGroup对象间接达到调用box的2个类
    connect(box_group, SIGNAL(need_new_box()), this, SLOT(clear_full_rows()));
    connect(box_group, SIGNAL(game_finished()), this, SLOT(game_over()));
    scene->addItem(box_group);

    //添加提示方块组
    next_box_group = new BoxGroup;
    scene->addItem(next_box_group);

    gameScoreText = new QGraphicsTextItem();//文本的父item为对应的场景
    gameScoreText->setFont(QFont("Times", 50, QFont::Bold));//为文本设置字体
    gameScoreText->setPos(450, 350);//分数在场景中出现的位置

    gameLevelText = new QGraphicsTextItem();
    gameLevelText->setFont(QFont("Times", 50, QFont::Bold));
    gameLevelText->setPos(20, 150);

    scene->addItem(gameLevelText);
    scene->addItem(gameScoreText);

    //开始游戏
   //start_game();
    //设置初始为隐藏状态
    top_line->hide();
    bottom_line->hide();
    left_line->hide();
    right_line->hide();
    gameScoreText->hide();
    gameLevelText->hide();

    //黑色遮罩
    QWidget *mask = new QWidget;
    mask->setAutoFillBackground(true);
    mask->setPalette(QPalette(QColor(0, 0, 0, 50)));//alpha为不透明度
    mask->resize(900, 600);
    //addWidget()函数的返回值是QGraphicsProxyWidget，如果不添加相应的头文件，则此处会报错
    mask_widget = scene->addWidget(mask);
    mask_widget->setPos(-50, -50);
    mask_widget->setZValue(1);//该层薄纱放在原图的上面，这里有点类似于opengl中的3维绘图

    //选项面板
    QWidget *option = new QWidget;
    //将关闭按钮放在option上
    QPushButton *option_close_button = new QPushButton(tr("关  闭"), option);//第2个参数为按钮所在的widget
    //设置按钮的字体颜色是白色
    QPalette palette;
    palette.setColor(QPalette::ButtonText, Qt::black);//第一个参数调色版的role，这里指的是按钮字体颜色
    option_close_button->setPalette(palette);
    //设置关闭按钮的位置，和单击后的响应
    option_close_button->move(120, 300);
    connect(option_close_button, SIGNAL(clicked()), option, SLOT(hide()));//单击后消失

    option->setAutoFillBackground(true);
    option->setPalette(QPalette(QColor(0, 0, 0, 180)));
    option->resize(300, 400);
    QGraphicsWidget *option_widget = scene->addWidget(option);
    option_widget->setPos(250, 50);
    option_widget->setZValue(3);
    option_widget->hide();


    //帮助面板
    QWidget *help = new QWidget;
    QPushButton *help_close_button = new QPushButton(tr("帮  助"), help);
    help_close_button->setPalette(palette);
    help_close_button->move(120, 300);
    connect(help_close_button, SIGNAL(clicked()), help, SLOT(hide()));

    help->setAutoFillBackground(true);
    help->setPalette(QPalette(QColor(0, 0, 0, 180)));
    help->resize(300, 400);
    QGraphicsWidget *help_widget = scene->addWidget(help);
    help_widget->setPos(250, 50);
    help_widget->setZValue(3);
    help_widget->hide();

    //游戏欢迎文本
    game_welcome_text = new QGraphicsTextItem();//第一个参数为文本内容，第二个参数为父item
    game_welcome_text->setHtml(tr("<font color=green>Tetris游戏</font>"));
    game_welcome_text->setFont(QFont("Times", 40, QFont::Bold));
    game_welcome_text->setPos(300, 100);
    game_welcome_text->setZValue(2);//放在第2层


    //游戏暂停文本
    game_pause_text = new QGraphicsTextItem();//第一个参数为文本内容，第二个参数为父item
    game_pause_text->setHtml(tr("<font color=green>游戏暂停中！</font>"));
    game_pause_text->setFont(QFont("Times", 40, QFont::Bold));
    game_pause_text->setPos(300, 100);
    game_pause_text->setZValue(2);//放在第2层
    game_pause_text->hide();

    //游戏结束文本
    game_over_text = new QGraphicsTextItem();//第一个参数为文本内容，第二个参数为父item
    game_over_text->setHtml(tr("<font color=green>GAME　OVER！</font>"));
    game_over_text->setFont(QFont("Times", 40, QFont::Bold));
    game_over_text->setPos(300, 100);
    game_over_text->setZValue(2);//放在第2层
    game_over_text->hide();

    scene->addItem(game_welcome_text);
    scene->addItem(game_pause_text);
    scene->addItem(game_over_text);
    // 游戏中使用的按钮

    QPushButton *button1 = new QPushButton(tr("开    始"));
    QPushButton *button2 = new QPushButton(tr("选    项"));
    QPushButton *button3 = new QPushButton(tr("帮    助"));
    QPushButton *button4 = new QPushButton(tr("退    出"));
    QPushButton *button5 = new QPushButton(tr("重新开始"));
    QPushButton *button6 = new QPushButton(tr("暂    停"));
    QPushButton *button7 = new QPushButton(tr("主 菜 单"));
    QPushButton *button8 = new QPushButton(tr("返回游戏"));
    QPushButton *button9 = new QPushButton(tr("结束游戏"));

    connect(button1, SIGNAL(clicked()), this, SLOT(start_game()));
    connect(button2, SIGNAL(clicked()), option, SLOT(show()));
    connect(button3, SIGNAL(clicked()), help, SLOT(show()));
    connect(button4, SIGNAL(clicked()), qApp, SLOT(quit()));//此处槽函数的接收对象为应用程序本身
    connect(button5, SIGNAL(clicked()), this, SLOT(restartGame()));
    connect(button6, SIGNAL(clicked()), this, SLOT(pauseGame()));
    connect(button7, SIGNAL(clicked()), this, SLOT(show_menu_button()));
    connect(button8, SIGNAL(clicked()), this, SLOT(returnGame()));//返回主菜单
    connect(button9, SIGNAL(clicked()), this, SLOT(finishGame()));

    start_button = scene->addWidget(button1);//restart_button并不是QPushbutton类型，而是QGraphicsItem类型,后面的类似
    option_button = scene->addWidget(button2);
    help_button = scene->addWidget(button3);
    exit_button = scene->addWidget(button4);
    restart_button = scene->addWidget(button5);
    pause_button = scene->addWidget(button6);
    show_menu_button = scene->addWidget(button7);
    return_button = scene->addWidget(button8);
    finish_button = scene->addWidget(button9);

    //设置位置
    start_button->setPos(370, 200);
    option_button->setPos(370, 250);
    help_button->setPos(370, 300);
    exit_button->setPos(370, 350);
    restart_button->setPos(600, 150);
    pause_button->setPos(600, 200);
    show_menu_button->setPos(600, 250);
    return_button->setPos(370, 200);
    finish_button->setPos(370, 250);

    //将这些按钮都放在z方向的第二层
    start_button->setZValue(2);
    option_button->setZValue(2);
    help_button->setZValue(2);
    exit_button->setZValue(2);
    restart_button->setZValue(2);
    return_button->setZValue(2);
    finish_button->setZValue(2);

    //一部分按钮隐藏起来
    restart_button->hide();
    finish_button->hide();
    pause_button->hide();
    show_menu_button->hide();
    return_button->hide();

}

void MyView::init_game()
{
    box_group->create_box(QPointF(300, 70)); //创建方块组,在中间位置处出现
    box_group->setFocus();//设置人机交互焦点，这样就可以使用键盘来控制它
    box_group->startTimer(INITSSPEED);//启动定时器
    game_speed = INITSSPEED;//游戏速度，暂停时需要用到
    next_box_group->create_box(QPoint(500, 70));//创建提示方块组

    scene()->setBackgroundBrush(QPixmap(":/"));
    gameScoreText->setHtml(tr("<font color=red>00</font>"));
    gameLevelText->setHtml(tr("<font color=white>第<br>一<br>幕</font>"));

    restart_button->show();
    pause_button->show();
    show_menu_button->show();
    gameScoreText->show();
    gameLevelText->show();
    top_line->show();
    bottom_line->show();
    left_line->show();
    right_line->show();
    // 可能以前返回主菜单时隐藏了boxGroup
    box_group->show();
}

void MyView::update_score(const int full_row_num)
{
    //更新分数
    int score = full_row_num *100;
    int currentScore =gameScoreText->toPlainText().toInt();
    currentScore+=score;
    //显示当前分数
    gameScoreText->setHtml(tr("<font color=red>%1</font>").arg(currentScore));
    //判断级别
    if(currentScore<500)
    {
        //第一级，什么都不用做

    }
    else if(currentScore <1000)
    {
        //第二级
        gameLevelText->setHtml(tr("<font color=white>第<br>二<br>幕</font>"));
        scene()->setBackgroundBrush(QPixmap(":/"));
        game_speed =300;
        box_group->stop_timer();
        box_group->startTimer(game_speed);
    }
    else {
        //添加下一个级别的设置
    }
}
