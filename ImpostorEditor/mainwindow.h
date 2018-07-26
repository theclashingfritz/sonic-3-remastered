#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "highlighter.h"
#include "customtextedit.h"

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
