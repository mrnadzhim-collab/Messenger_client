#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "netclient.h"
#include "messegedata.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class NetClient;
class QLineEdit;
class QListWidget;
class QLabel;
class QTextEdit;
class QPushButton;
class QJsonObject;
class QJsonArray;

struct Chat
{
    qint64 id = -1;
    QString name;
    QString username;
    bool online = false;
    QList<Message> msgs;
    int unread = 0;
    qint64 lastTs = 0;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(NetClient *net, const QJsonObject &user, QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();

    Chat &ensureChat(qint64 id);
    void addUserEntry(qint64 id, const QString &name, const QString &username, bool online);
    QString chatPreview(const Chat &c) const;
    void rebuildChats();
    void rebuildMessages();
    void updateHeader();

    Ui::MainWindow *m_ui;

    NetClient *m_net;
    qint64 m_myId;
    QString m_myName;
    QMap<qint64, Chat> m_chats;
    qint64 m_current = -1;

    QString m_filesDir;

private slots:
    void onUsersReceived(const QJsonArray &users);
    void serverError(const QString &msg);
    void openChat(qint64 peer);
    void onHistoryReceived(qint64 peer, const QJsonArray &msgs);
};
#endif // MAINWINDOW_H
