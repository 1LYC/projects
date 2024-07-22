#include<QApplication>
#include"myview.h"
#include<QTextCodec>
#include<QTime>

int main(int argc, char* argv[]) {

    QApplication  app(argc, argv);
    QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
    //QTime提供了闹钟功能
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));//secsTo()为返回当前的秒数

    MyView view;//主函数是直接调用的视图类
    view.show();

    return app.exec();
}
