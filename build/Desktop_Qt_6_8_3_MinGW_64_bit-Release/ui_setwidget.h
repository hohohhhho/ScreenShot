/********************************************************************************
** Form generated from reading UI file 'setwidget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETWIDGET_H
#define UI_SETWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "shortcutinputer.h"

QT_BEGIN_NAMESPACE

class Ui_SetWidget
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    ShortCutInputer *btn_hotKey;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *btn_savePath;
    QLineEdit *edit_savePath;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *btn_color;
    QCheckBox *box_autoStart;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_6;
    QPushButton *btn_save;
    QSpacerItem *horizontalSpacer_5;

    void setupUi(QWidget *SetWidget)
    {
        if (SetWidget->objectName().isEmpty())
            SetWidget->setObjectName("SetWidget");
        SetWidget->resize(240, 140);
        verticalLayout = new QVBoxLayout(SetWidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton = new QPushButton(SetWidget);
        pushButton->setObjectName("pushButton");

        horizontalLayout->addWidget(pushButton);

        btn_hotKey = new ShortCutInputer(SetWidget);
        btn_hotKey->setObjectName("btn_hotKey");

        horizontalLayout->addWidget(btn_hotKey);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");
        btn_savePath = new QPushButton(SetWidget);
        btn_savePath->setObjectName("btn_savePath");

        horizontalLayout_4->addWidget(btn_savePath);

        edit_savePath = new QLineEdit(SetWidget);
        edit_savePath->setObjectName("edit_savePath");
        edit_savePath->setAlignment(Qt::AlignmentFlag::AlignCenter);

        horizontalLayout_4->addWidget(edit_savePath);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        btn_color = new QPushButton(SetWidget);
        btn_color->setObjectName("btn_color");

        horizontalLayout_2->addWidget(btn_color);

        box_autoStart = new QCheckBox(SetWidget);
        box_autoStart->setObjectName("box_autoStart");
        box_autoStart->setChecked(true);

        horizontalLayout_2->addWidget(box_autoStart);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_6);

        btn_save = new QPushButton(SetWidget);
        btn_save->setObjectName("btn_save");

        horizontalLayout_5->addWidget(btn_save);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        verticalLayout->addLayout(horizontalLayout_5);


        retranslateUi(SetWidget);

        QMetaObject::connectSlotsByName(SetWidget);
    } // setupUi

    void retranslateUi(QWidget *SetWidget)
    {
        SetWidget->setWindowTitle(QCoreApplication::translate("SetWidget", "Form", nullptr));
        pushButton->setText(QCoreApplication::translate("SetWidget", "\347\202\271\345\207\273\346\233\264\346\224\271\347\203\255\351\224\256", nullptr));
        btn_hotKey->setText(QString());
        btn_savePath->setText(QCoreApplication::translate("SetWidget", "\345\233\276\347\211\207\344\277\235\345\255\230\350\267\257\345\276\204", nullptr));
        btn_color->setText(QCoreApplication::translate("SetWidget", "\346\210\252\345\261\217\347\272\277\346\235\241\351\242\234\350\211\262", nullptr));
        box_autoStart->setText(QCoreApplication::translate("SetWidget", "\345\274\200\346\234\272\350\207\252\345\220\257\345\212\250", nullptr));
        btn_save->setText(QCoreApplication::translate("SetWidget", "\344\277\235\345\255\230", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SetWidget: public Ui_SetWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETWIDGET_H
