#include "shortcutinputer.h"

#include <QApplication>
#include <QEvent>
#include <QKeyEvent>

ShortCutInputer::ShortCutInputer(QWidget *parent)
    : QPushButton{parent}
{

}

bool ShortCutInputer::eventFilter(QObject *oj, QEvent *ev)
{
    Q_UNUSED(oj);
    if(ev->type() == QEvent::KeyPress){
        QKeyEvent* event = dynamic_cast<QKeyEvent*>(ev);
        if(event){
            this->current_modifiers = event->modifiers();

            int key=event->key();
            if(key != Qt::Key_Control && key != Qt::Key_Shift &&
                key != Qt::Key_Alt && key != Qt::Key_Meta){

                this->current_key = event->key();

            }
            updateComboView();
        }
        return true;
    }
    if(ev->type() == QEvent::KeyRelease){
        QKeyEvent* event = dynamic_cast<QKeyEvent*>(ev);
        if(event){
            int key = event->key();
            bool modifier = false;
            if(key == Qt::Key_Control){
                this->current_modifiers.setFlag(Qt::KeyboardModifier::ControlModifier,false);
                modifier = true;
            }
            if(key == Qt::Key_Shift){
                this->current_modifiers.setFlag(Qt::KeyboardModifier::ShiftModifier,false);
                modifier = true;
            }
            if(key == Qt::Key_Alt){
                this->current_modifiers.setFlag(Qt::KeyboardModifier::AltModifier,false);
                modifier = true;
            }
            if(key == Qt::Key_Meta){
                this->current_modifiers.setFlag(Qt::KeyboardModifier::MetaModifier,false);
                modifier = true;
            }
            if(!modifier){
                finishCapture();
            }
        }
        return true;
    }
    return false;
}

void ShortCutInputer::setCombo(QKeyCombination keys)
{
    this->current_modifiers = keys.keyboardModifiers();
    this->current_key = keys.key();
    updateComboView();
}

QString ShortCutInputer::fromKeyCombo(QKeyCombination keys)
{
    Qt::KeyboardModifiers modifiers = keys.keyboardModifiers();
    int key = keys.key();
    qDebug()<<"modifiers"<<keys.keyboardModifiers();
    QStringList list;
    if(modifiers & Qt::KeyboardModifier::ControlModifier){
        list<<"Ctrl";
    }
    if(modifiers & Qt::KeyboardModifier::ShiftModifier){
        list<<"Shift";
    }
    if(modifiers & Qt::KeyboardModifier::AltModifier){
        list<<"Alt";
    }
    if(modifiers & Qt::KeyboardModifier::MetaModifier){
        list<<"Meta";
    }
    if(key>0){
        list<<QKeySequence(key).toString();
    }
    return list.join('+');
}

QKeyCombination ShortCutInputer::toKeyCombo(QString str)
{
    QStringList list = str.split('+');
    Qt::KeyboardModifiers modifiers;
    if(list.contains("Ctrl")){
        modifiers.setFlag(Qt::KeyboardModifier::ControlModifier);
    }
    if(list.contains("Shift")){
        modifiers.setFlag(Qt::KeyboardModifier::ShiftModifier);
    }
    if(list.contains("Alt")){
        modifiers.setFlag(Qt::KeyboardModifier::AltModifier);
    }
    if(list.contains("Meta")){
        modifiers.setFlag(Qt::KeyboardModifier::MetaModifier);
    }
    QKeyCombination keys(modifiers,static_cast<Qt::Key>( list.last()[0].unicode() ));

    return keys;
}

void ShortCutInputer::startCapture()
{
    // this->setText("");
    this->setFocus();
    this->current_modifiers = Qt::KeyboardModifiers();
    this->current_key = Qt::Key_unknown;
    updateComboView();
    QApplication::instance()->installEventFilter(this);
}

void ShortCutInputer::finishCapture()
{
    QApplication::instance()->removeEventFilter(this);
    QKeyCombination combo(current_modifiers,static_cast<Qt::Key>(current_key));
    emit result(combo);
}

void ShortCutInputer::updateComboView()
{
    // QStringList list;
    // if(current_modifiers & Qt::KeyboardModifier::ControlModifier){
    //     list<<"Ctrl";
    // }
    // if(current_modifiers & Qt::KeyboardModifier::ShiftModifier){
    //     list<<"Shift";
    // }
    // if(current_modifiers & Qt::KeyboardModifier::AltModifier){
    //     list<<"Alt";
    // }
    // if(current_modifiers & Qt::KeyboardModifier::MetaModifier){
    //     list<<"Meta";
    // }
    // if(current_key>0){
    //     list<<QKeySequence(current_key).toString();
    // }
    // this->setText(list.join('+'));
    QKeyCombination combo(current_modifiers,static_cast<Qt::Key>(current_key));
    this->setText(fromKeyCombo(combo));
}
