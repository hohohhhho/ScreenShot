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
#include <QApplication>

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
        setWidget->setCombo(config.hotKey);
        setWidget->autoStartBox()->setChecked(config.autoStart);
        setWidget->updateColor(config.color);
        setWidget->savePathEdit()->setText(config.savePath);

        connect(setWidget->hotKeyButton(),&ShortCutInputer::result,this,[=](QKeyCombination keys){
            config.hotKey = ShortCutInputer::fromKeyCombo(keys);
            createNewHotKey();
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

        bool result = createNewHotKey();

        if(result){
            this->move(0,0);
            this->resize(screen->size());
            timer->stop();
            timer->deleteLater();
      }else{
            if(num_try >= 3){
                QMessageBox::critical(this,tr("提示"),tr("失败次数过多，程序退出"));
                qApp->exit();
            }else{
                num_try++;
                QMessageBox::warning(this,tr("提示"),QString(tr("热键申请失败:%1,点击确认以重新申请")).arg(GetLastError()));
            }
        }
        // if(result && loaded){
        //     timer->stop();
        //     timer->deleteLater();
        // }

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
    if(!dir.exists() || config.savePath.isEmpty()){//如果配置里的路径不存在则默认一个保存路径
        dir =(QApplication::applicationDirPath() + "/pxp");
        if(!dir.exists()){
            dir.mkpath(dir.absolutePath());
        }
    }


    QDateTime now = QDateTime::currentDateTime();
    QString filename = now.toString("yyyy-MM-dd_HHmmss") + ".png";
    QString finaldir = dir.filePath(filename);
    // QString finaldir(dir.absolutePath()+"/"+now.toString().replace(":","-").replace(" ","_")+".png");
    qDebug()<<"finaldir"<<finaldir;
    if(!pxp.save(finaldir)){
        QMessageBox::critical(this,tr("提示"),QString( tr("图片保存失败").append(finaldir) ));
    }
    QImage image=pxp.toImage();
    QClipboard* clip=QGuiApplication::clipboard();
    clip->setImage(image);

    this->x_first=-1;
    this->y_first=-1;
    this->working=false;
    this->hide();
}

bool Widget::createNewHotKey()
{
    HWND hwnd = reinterpret_cast<HWND>(this->winId());
    if(!hwnd){
        QMessageBox::critical(this,tr("错误"),tr("窗口句柄为空，程序退出"));
        qApp->exit();
    }

    UnregisterHotKey(hwnd, 1);//注销可能存在的旧热键

    QKeyCombination combo = ShortCutInputer::toKeyCombo(config.hotKey);
    // 转换修饰键
    uint modifiers = 0;
    Qt::KeyboardModifiers qtMods = combo.keyboardModifiers();

    if (qtMods & Qt::ControlModifier) modifiers |= MOD_CONTROL;
    if (qtMods & Qt::AltModifier) modifiers |= MOD_ALT;
    if (qtMods & Qt::ShiftModifier) modifiers |= MOD_SHIFT;
    if (qtMods & Qt::MetaModifier) modifiers |= MOD_WIN;

    // 转换键值
    UINT key = 0;
    Qt::Key qtKey = combo.key();

    // 处理特殊键值映射
    switch (qtKey) {
    case Qt::Key_Escape: key = VK_ESCAPE; break;
    case Qt::Key_Tab: key = VK_TAB; break;
    case Qt::Key_Backspace: key = VK_BACK; break;
    case Qt::Key_Return: key = VK_RETURN; break;
    case Qt::Key_Enter: key = VK_RETURN; break; // 小键盘回车
    case Qt::Key_Insert: key = VK_INSERT; break;
    case Qt::Key_Delete: key = VK_DELETE; break;
    case Qt::Key_Pause: key = VK_PAUSE; break;
    case Qt::Key_Print: key = VK_PRINT; break;
    case Qt::Key_Home: key = VK_HOME; break;
    case Qt::Key_End: key = VK_END; break;
    case Qt::Key_Left: key = VK_LEFT; break;
    case Qt::Key_Up: key = VK_UP; break;
    case Qt::Key_Right: key = VK_RIGHT; break;
    case Qt::Key_Down: key = VK_DOWN; break;
    case Qt::Key_PageUp: key = VK_PRIOR; break;
    case Qt::Key_PageDown: key = VK_NEXT; break;
    case Qt::Key_CapsLock: key = VK_CAPITAL; break;
    case Qt::Key_NumLock: key = VK_NUMLOCK; break;
    case Qt::Key_ScrollLock: key = VK_SCROLL; break;
    case Qt::Key_F1: key = VK_F1; break;
    case Qt::Key_F2: key = VK_F2; break;
    case Qt::Key_F3: key = VK_F3; break;
    case Qt::Key_F4: key = VK_F4; break;
    case Qt::Key_F5: key = VK_F5; break;
    case Qt::Key_F6: key = VK_F6; break;
    case Qt::Key_F7: key = VK_F7; break;
    case Qt::Key_F8: key = VK_F8; break;
    case Qt::Key_F9: key = VK_F9; break;
    case Qt::Key_F10: key = VK_F10; break;
    case Qt::Key_F11: key = VK_F11; break;
    case Qt::Key_F12: key = VK_F12; break;
    case Qt::Key_Space: key = VK_SPACE; break;
    case Qt::Key_Plus: key = VK_ADD; break;
    case Qt::Key_Minus: key = VK_SUBTRACT; break;
    case Qt::Key_Asterisk: key = VK_MULTIPLY; break;
    case Qt::Key_Slash: key = VK_DIVIDE; break;
    case Qt::Key_Period: key = VK_DECIMAL; break;
    default:
        // 处理字母和数字键
        if (qtKey >= Qt::Key_A && qtKey <= Qt::Key_Z) {
            key = 'A' + (qtKey - Qt::Key_A);
        } else if (qtKey >= Qt::Key_0 && qtKey <= Qt::Key_9) {
            key = '0' + (qtKey - Qt::Key_0);
        } else if (qtKey >= Qt::Key_F1 && qtKey <= Qt::Key_F24) {
            key = VK_F1 + (qtKey - Qt::Key_F1);
        } else {
            // 无法识别的键值
            modifiers = MOD_CONTROL | MOD_SHIFT;
            key = 'C';
        }
    }

    // 注册热键
    return RegisterHotKey(hwnd, 1, modifiers, key);
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
    /*QDir::currentPath()).filePath("user.json"*/
    QString filename=QDir( QApplication::applicationDirPath() ).filePath("user.json");
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)){//没有配置文件则直接跳过
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(data);
    if(document.isObject()){
        // loaded = true;
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
    QDir dir(QApplication::applicationDirPath());
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
