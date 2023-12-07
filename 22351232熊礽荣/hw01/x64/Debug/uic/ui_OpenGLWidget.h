/********************************************************************************
** Form generated from reading UI file 'OpenGLWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.2.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OPENGLWIDGET_H
#define UI_OPENGLWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OpenGLWidgetClass
{
public:

    void setupUi(QWidget *OpenGLWidgetClass)
    {
        if (OpenGLWidgetClass->objectName().isEmpty())
            OpenGLWidgetClass->setObjectName(QString::fromUtf8("OpenGLWidgetClass"));
        OpenGLWidgetClass->resize(600, 400);

        retranslateUi(OpenGLWidgetClass);

        QMetaObject::connectSlotsByName(OpenGLWidgetClass);
    } // setupUi

    void retranslateUi(QWidget *OpenGLWidgetClass)
    {
        OpenGLWidgetClass->setWindowTitle(QCoreApplication::translate("OpenGLWidgetClass", "OpenGLWidget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OpenGLWidgetClass: public Ui_OpenGLWidgetClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OPENGLWIDGET_H
