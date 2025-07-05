#ifndef SETWIDGET_H
#define SETWIDGET_H

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
    QPushButton* hotKeyButton();
    QPushButton* savePathButton();
    QPushButton* colorButton();
    QPushButton* saveButton();
    QCheckBox* autoStartBox();
    QLineEdit* hotKeyEdit();
    QLineEdit* savePathEdit();
    void updateColor(QColor color);

private:
    Ui::SetWidget *ui;
};

#endif // SETWIDGET_H
