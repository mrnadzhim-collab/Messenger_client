#include "mainwindow.h"
#include "loginwindow.h"

#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    NetClient *net = new NetClient;
    LoginWindow *login = new LoginWindow(net);
    QObject::connect(login, &LoginWindow::authSuccess, [net, login](const QJsonObject &user){
        MainWindow *w = new MainWindow(net, user);
        w->show();
        login->deleteLater();
    });
    login->show();

    return a.exec();
}
