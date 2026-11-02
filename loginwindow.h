#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

#include "netclient.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(NetClient *net, QWidget *parent = nullptr);
    ~LoginWindow();

private:
    void setupUI();

    Ui::LoginWindow *m_ui;

    NetClient *m_net;

    bool m_registerMode = false;

private slots:
    void on_auth();
    void on_switchMode();
    void on_showError(const QString &msg);

signals:
    void authSuccess(const QJsonObject &user);
};

#endif // LOGINWINDOW_H
