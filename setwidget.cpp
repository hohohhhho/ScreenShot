#include "setwidget.h"
#include "ui_setwidget.h"

SetWidget::SetWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SetWidget)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("设置"));
    this->setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setVisible(false);
    this->setStyleSheet("*{"
                        "font-size:15px;"
                        "font-family:微软雅黑;"
                        "color:red;"
                        "}");
}

SetWidget::~SetWidget()
{
    delete ui;
}

QPushButton *SetWidget::hotKeyButton()
{
    return ui->btn_hotKey;
}

QPushButton *SetWidget::savePathButton()
{
    return ui->btn_savePath;
}

QPushButton *SetWidget::colorButton()
{
    return ui->btn_color;
}

QPushButton *SetWidget::saveButton()
{
    return ui->btn_save;
}

QCheckBox *SetWidget::autoStartBox()
{
    return ui->box_autoStart;
}

QLineEdit *SetWidget::hotKeyEdit()
{
    return ui->edit_hotKey;
}

QLineEdit *SetWidget::savePathEdit()
{
    return ui->edit_savePath;
}

void SetWidget::updateColor(QColor color)
{
    QString style=QString("*{"
                        "font-size:15px;"
                        "font-family:微软雅黑;"
                        "color:black;"
                        "}"
                        "QPushButton#btn_color{"
                        "font-size:15px;"
                        "font-family:微软雅黑;"
                        "color:rgb(%1,%2,%3);"
                        "}").arg(color.red()).arg(color.green()).arg(color.blue());

    qDebug()<<"style"<<style;
    this->setStyleSheet("");//先设置为空，删除原样式表缓存
    this->setStyleSheet(style);
}
