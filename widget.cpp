#include "widget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QTimer>
#include <windows.h>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTime>
#include <QClipboard>
#include <QColorDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QScreen* screen=QGuiApplication::primaryScreen();
    this->move(screen->geometry().center());
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    this->resize(1,1);

    loadFromJson();

    QSystemTrayIcon* tray=new QSystemTrayIcon(this);
    QMenu* menu=new QMenu(this);
    QAction* act_exit=new QAction(menu);
    QAction* act_set=new QAction(menu);
    act_exit->setText("关闭");
    act_set->setText("设置");
    menu->addAction(act_exit);
    menu->addAction(act_set);
    connect(act_exit,&QAction::triggered,this,[=](){
        qApp->quit();
    });
    connect(act_set,&QAction::triggered,this,[=](){
        if(setWidget){
            delete setWidget;
        }
        setWidget=new SetWidget;
        setWidget->autoStartBox()->setChecked(config.autoStart);
        setWidget->updateColor(config.color);
        setWidget->savePathEdit()->setText(config.savePath);

        connect(setWidget->hotKeyButton(),&QPushButton::clicked,this,[=](){

        });
        connect(setWidget->savePathButton(),&QPushButton::clicked,this,[=](){
            QString path = QFileDialog::getExistingDirectory(this);
            config.savePath = path;
            setWidget->savePathEdit()->setText(path);
        });
        connect(setWidget->savePathEdit(),&QLineEdit::returnPressed,this,[=](){
            QString path = setWidget->savePathEdit()->text();
            if(!QDir(path).exists()){
                QMessageBox::warning(this,tr("提示"),tr("路径不存在"));
                return;
            }
            config.savePath = path;
        });
        connect(setWidget->colorButton(),&QPushButton::clicked,this,[=](){
            QColor result = QColorDialog::getColor(pen_color,this,tr("选择线条颜色"));
            config.color = result;
            setWidget->updateColor(result);
        });
        connect(setWidget->saveButton(),&QPushButton::clicked,this,[=](){
            saveToJson();
            setWidget->close();
        });
        connect(setWidget->autoStartBox(),&QCheckBox::checkStateChanged,this,[=](Qt::CheckState state){
            if(state == Qt::Checked){
                config.autoStart = true;
                addToStartup();
            }else{
                config.autoStart = false;
                removeFromStartup();
            }
        });
        setWidget->show();
    });
    tray->setIcon(QIcon(":res/cut.png"));
    tray->setContextMenu(menu);
    tray->show();

    bool autoStart = addToStartup();
    qDebug()<<"autoStart"<<autoStart;

    this->setVisible(false);

    static int num_try=0;
    QTimer* timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){

        HWND hwnd = reinterpret_cast<HWND>(this->winId());
        if(!hwnd){
            QMessageBox::critical(this,tr("错误"),tr("窗口句柄为空，程序退出"));
            qApp->exit();
        }

        this->setFocus();
        bool result=RegisterHotKey(hwnd,1,MOD_CONTROL|MOD_SHIFT,'C');
        if(result){
            this->move(0,0);
            this->resize(screen->size());
            timer->stop();
            timer->deleteLater();
        }else{
            if(num_try >= 5){
                QMessageBox::critical(this,tr("提示"),tr("失败次数过多，程序退出"));
                qApp->exit();
            }else{
                num_try++;
                QMessageBox::warning(this,tr("提示"),QString(tr("热键申请失败:%1,点击确认以重新申请")).arg(GetLastError()));
            }
        }
    });
    timer->start(500);
}

Widget::~Widget()
{
    if(setWidget){
        delete setWidget;
    }
}

void Widget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);
    QPainter painter(this);
    painter.setPen(QPen(config.color,3));
    painter.setBrush(Qt::NoBrush);
    if(working){
        if(!m_pxp.isNull()){
            painter.drawPixmap(0,0,m_pxp);
        }else if(this->isVisible()){
            this->hide();
        }
        if(this->x_first > 0){
            painter.drawRect(x_first,y_first,x_moving-x_first,y_moving-y_first);
        }
    }
}

bool Widget::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    if(eventType=="windows_generic_MSG"){
        MSG* msg=static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY && msg->wParam == 1) {
            qDebug()<<"截屏热键";
            if(!working){
                working=true;
                m_pxp=QGuiApplication::primaryScreen()->grabWindow();
                // 降低亮度，不过操作耗时较多
                // QPixmap pxp=QGuiApplication::primaryScreen()->grabWindow();
                // QImage image=pxp.toImage();
                // for(int i=0;i<image.height();++i)
                //     for(int j=0;j<image.width();++j){
                //         QColor color=image.pixel(j,i);
                //         int r=qBound(0,color.red()-10,255);
                //         int g=qBound(0,color.green()-10,255);
                //         int b=qBound(0,color.blue()-10,255);
                //         color=qRgb(r,g,b);
                //         image.setPixel(j,i,qRgb(r,g,b));
                //     }
                // m_pxp=QPixmap::fromImage(image);

                this->show();
                update();
            }
            return true;
        }
    }
    return false;
    Q_UNUSED(result);
}

void Widget::mousePressEvent(QMouseEvent *ev)
{
    this->x_first=ev->globalPosition().x();
    this->y_first=ev->globalPosition().y();
    this->x_moving=ev->globalPosition().x();
    this->y_moving=ev->globalPosition().y();
    update();
}

void Widget::mouseMoveEvent(QMouseEvent *ev)
{
    //qDebug()<<"mouseMoveEvent";
    this->x_moving=ev->globalPosition().x();
    this->y_moving=ev->globalPosition().y();
    update();
}

void Widget::mouseReleaseEvent(QMouseEvent *ev)
{
    Q_UNUSED(ev);
    qDebug()<<"x_first:"<<x_first<<"y_first:"<<y_first;
    qDebug()<<"x_moving:"<<x_first<<"y_moving:"<<y_moving;
    qreal ratio=QGuiApplication::primaryScreen()->devicePixelRatio();//根据物理像素摘下来的屏幕截图
    int x=qRound(x_first*ratio);
    int y=qRound(y_first*ratio);
    int w=qRound((x_moving-x_first)*ratio);
    int h=qRound((y_moving-y_first)*ratio);
    QPixmap pxp;
    if(w>0 && h>0){
        //复制这个屏幕截图对应区域时也是要用物理像素，然而通过鼠标等获取到的坐标都是逻辑像素，所以才需要用DPR像素转换
        pxp=this->m_pxp.copy(x,y,w,h);
    }else{
        pxp=this->m_pxp;
    }

    QDir dir(config.savePath);
    if(!dir.exists()){//如果配置里的路径不存在则默认一个保存路径
        dir =(QDir::currentPath() + "/pxp");
        if(!dir.exists()){
            dir.mkpath(".");
        }
    }

    QDateTime now = QDateTime::currentDateTime();
    QString filename = now.toString("yyyy-MM-dd_HHmmss") + ".png";
    QString finaldir = dir.filePath(filename);
    // QString finaldir(dir.absolutePath()+"/"+now.toString().replace(":","-").replace(" ","_")+".png");
    qDebug()<<"finaldir"<<finaldir;
    if(!pxp.save(finaldir)){
        QMessageBox::critical(this,tr("提示"),tr("图片保存失败"));
    }
    QImage image=pxp.toImage();
    QClipboard* clip=QGuiApplication::clipboard();
    clip->setImage(image);

    this->x_first=-1;
    this->y_first=-1;
    this->working=false;
    this->hide();
}

bool Widget::addToStartup() {
    //获取当前可执行文件路径
    TCHAR path[MAX_PATH];
    GetModuleFileName(nullptr, path, MAX_PATH);

    //注册表路径
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER,
                               TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                               0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    //设置注册表值
    result = RegSetValueEx(hKey,
                           TEXT("ScreenCut"),
                           0,
                           REG_SZ,
                           (BYTE*)path,
                           (DWORD)((wcslen(path) + 1) * sizeof(wchar_t))
                           );
    RegCloseKey(hKey);

    return result == ERROR_SUCCESS;
}

bool Widget::removeFromStartup() {
    HKEY hKey;
    LONG result = RegOpenKeyEx(HKEY_CURRENT_USER,
                               TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
                               0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    result = RegDeleteValue(hKey, TEXT("ScreenCut"));
    RegCloseKey(hKey);

    return result == ERROR_SUCCESS;
}

void Widget::loadFromJson()
{
    QString filename=QDir(QDir::currentPath()).filePath("user.json");
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){//没有配置文件则直接跳过
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    if(document.isObject()){
        QJsonObject oj = document.object();
        config.hotKey = oj["hotKey"].toString();
        config.savePath = oj["savePath"].toString();
        QStringList rgb = oj["color"].toString().split(',');
        config.color = QColor( rgb[0].toInt() , rgb[1].toInt() , rgb[2].toInt() );
        config.autoStart = oj["autoStart"].toBool();
    }else{
        qDebug()<<"document is not a object";
    }
}

void Widget::saveToJson()
{
    QDir dir(QDir::currentPath());
    if(!dir.exists()){
        dir.mkpath(".");
    }
    QString filename=dir.filePath("user.json");

    QColor config_color=config.color;
    QJsonObject oj;
    oj["hotKey"] = config.hotKey;
    oj["savePath"] = config.savePath;
    oj["color"] = QString("%1,%2,%3")
                      .arg(config_color.red())
                      .arg(config_color.green())
                      .arg(config_color.blue());
    oj["autoStart"] = config.autoStart;

    QJsonDocument document(oj);

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)){
        QMessageBox::critical(this,tr("错误"),tr("配置保存失败"));
        return;
    }
    file.write(document.toJson());
    file.close();
}
