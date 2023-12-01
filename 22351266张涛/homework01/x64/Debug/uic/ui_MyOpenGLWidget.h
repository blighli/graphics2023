/********************************************************************************
** Form generated from reading UI file 'MyOpenGLWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYOPENGLWIDGET_H
#define UI_MYOPENGLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyOpenGLWidgetClass
{
public:

    void setupUi(QWidget *MyOpenGLWidgetClass)
    {
        if (MyOpenGLWidgetClass->objectName().isEmpty())
            MyOpenGLWidgetClass->setObjectName(QString::fromUtf8("MyOpenGLWidgetClass"));
        MyOpenGLWidgetClass->resize(600, 400);

        retranslateUi(MyOpenGLWidgetClass);

        QMetaObject::connectSlotsByName(MyOpenGLWidgetClass);
    } // setupUi

    void retranslateUi(QWidget *MyOpenGLWidgetClass)
    {
        MyOpenGLWidgetClass->setWindowTitle(QCoreApplication::translate("MyOpenGLWidgetClass", "MyOpenGLWidget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyOpenGLWidgetClass: public Ui_MyOpenGLWidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYOPENGLWIDGET_H
