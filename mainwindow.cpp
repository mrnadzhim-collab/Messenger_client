#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(NetClient *net, const QJsonObject &user, QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow),
    m_net(net)
{
    m_ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}
