#ifndef SHORTCUTINPUTER_H
#define SHORTCUTINPUTER_H

#include <QPushButton>

class ShortCutInputer : public QPushButton
{
    Q_OBJECT
public:
    explicit ShortCutInputer(QWidget *parent = nullptr);

    bool eventFilter(QObject* oj,QEvent* ev)override;

    void setCombo(QKeyCombination keys);
    static QString fromKeyCombo(QKeyCombination keys);
    static QKeyCombination toKeyCombo(QString str);
public slots:
    void startCapture();
signals:
    void result(QKeyCombination keys);
private:
    Qt::KeyboardModifiers current_modifiers;
    int current_key = Qt::Key_unknown;

    void finishCapture();
    void updateComboView();
};

#endif // SHORTCUTINPUTER_H
