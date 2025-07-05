#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QScreen>
#include <windows.h>
#include <QPointer>
#include "setwidget.h"
//#include <QAbstractNativeEventFilter>

struct UserSet{
    QString hotKey;//热键
    QString savePath;//图片保存路径
    QColor color;//线条颜色
    bool autoStart;//自启动
};

class Widget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(bool working READ getWorking WRITE setWorking NOTIFY workStateChanged FINAL)

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();


    bool getWorking(){return working;};
    void setWorking(bool state){if(state!=working){working=state;emit workStateChanged();}};

    void paintEvent(QPaintEvent* ev)override;
    bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
    void mousePressEvent(QMouseEvent* ev)override;
    void mouseMoveEvent(QMouseEvent* ev)override;
    void mouseReleaseEvent(QMouseEvent* ev)override;

public slots:

signals:
    void workStateChanged();

private:
    bool working=false;
    QPixmap m_pxp;
    QColor pen_color=QColor(20,160,180);
    QPointer<SetWidget> setWidget;
    UserSet config;

    int x_first=-1,y_first=-1;
    int x_moving=-1,y_moving=-1;

    bool addToStartup();
    bool removeFromStartup();
    void loadFromJson();
    void saveToJson();
    bool createNewHotKey();
};
#endif // WIDGET_H
