#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // 设置窗口图标
    setWindowIcon(QIcon(QStringLiteral(":/res/mainframe.ico")));

    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
