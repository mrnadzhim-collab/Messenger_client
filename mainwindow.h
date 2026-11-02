#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "netclient.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(NetClient *net, const QJsonObject &user, QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *m_ui;

    NetClient *m_net;
};
#endif // MAINWINDOW_H
