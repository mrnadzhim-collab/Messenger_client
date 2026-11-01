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
    void addIncomingMessage(const QJsonObject &o, bool countUnread);
    void rebuildChats();
    void rebuildMessages();
    void updateHeader();
    void clearAttach();

    Ui::MainWindow *m_ui;

    NetClient *m_net;
    qint64 m_myId;
    QString m_myName;
    QMap<qint64, Chat> m_chats;
    qint64 m_current = -1;
    qint64 m_nextCid = 1;
    QString m_attachPath;
    QString m_filesDir;

private slots:
    void onUsersReceived(const QJsonArray &users);
    void serverError(const QString &msg);
    void openChat(qint64 peer);
    void onHistoryReceived(qint64 peer, const QJsonArray &msgs);
    void sendCurrent();
    void onIncoming(const QJsonObject &msg);
    void onAck(qint64 cid, qint64 id, qint64 ts, bool delivered, qint64 to);
    void onReadReceipt(qint64 fromId, const QJsonArray &ids);
    void onPresence(qint64 userId, bool online);
    void onDelivered(const QJsonArray &ids);
    void onPending(const QJsonArray &msgs);
};
#endif // MAINWINDOW_H
