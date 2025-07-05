#ifndef SETWIDGET_H
#define SETWIDGET_H

#include "shortcutinputer.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

namespace Ui {
class SetWidget;
}

class SetWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SetWidget(QWidget *parent = nullptr);
    ~SetWidget();
    ShortCutInputer* hotKeyButton();
    QPushButton* savePathButton();
    QPushButton* colorButton();
    QPushButton* saveButton();
    QCheckBox* autoStartBox();
    QLineEdit* savePathEdit();
    void updateColor(QColor color);
    void setCombo(QString combo);

private:
    Ui::SetWidget *ui;
};

#endif // SETWIDGET_H
