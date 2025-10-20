/********************************************************************************
** Form generated from reading UI file 'server_mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SERVER_MAINWINDOW_H
#define UI_SERVER_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *dbStatusLabel;
    QLabel *serverStatusLabel;
    QPushButton *serverStartButton;
    QTextEdit *logTextEdit;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(474, 464);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        dbStatusLabel = new QLabel(centralwidget);
        dbStatusLabel->setObjectName("dbStatusLabel");
        dbStatusLabel->setGeometry(QRect(12, 2, 441, 41));
        serverStatusLabel = new QLabel(centralwidget);
        serverStatusLabel->setObjectName("serverStatusLabel");
        serverStatusLabel->setGeometry(QRect(10, 40, 441, 51));
        serverStartButton = new QPushButton(centralwidget);
        serverStartButton->setObjectName("serverStartButton");
        serverStartButton->setGeometry(QRect(10, 390, 101, 26));
        logTextEdit = new QTextEdit(centralwidget);
        logTextEdit->setObjectName("logTextEdit");
        logTextEdit->setGeometry(QRect(10, 90, 451, 281));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 474, 23));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        dbStatusLabel->setText(QCoreApplication::translate("MainWindow", "DB", nullptr));
        serverStatusLabel->setText(QCoreApplication::translate("MainWindow", "SERVER", nullptr));
        serverStartButton->setText(QCoreApplication::translate("MainWindow", "start", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SERVER_MAINWINDOW_H
