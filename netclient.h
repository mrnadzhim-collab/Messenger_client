#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QByteArray>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QTcpSocket>

class NetClient : public QObject
{
    Q_OBJECT
public:
    NetClient(QObject *parent = nullptr);

    void connectToServer(const QString &host, quint16 port);
    void login(const QString &username, const QString &password);
    void registerUser(const QString &username, const QString &password, const QString &name);
    void requestUsers();
    void requestHistory(qint64 peer);
    void sendMessage(qint64 to, const QString &text, const QString &fileName,
                     const QByteArray &fileData, qint64 cid);
    void sendRead(qint64 peer, const QVector<qint64> &ids);

private:
    void parse(const QJsonObject &obj);
    void send(const QJsonObject &obj);
    void flushAuth();

    QTcpSocket *m_socket;
    QJsonObject m_auth;
    QByteArray m_buffer;
    bool m_connected = false;

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onSocketError();

signals:
    void connectedToServer();
    void connectionFailed(const QString &error);
    void authOk(const QJsonObject &user);
    void serverError(const QString &msg);
    void usersReceived(const QJsonArray &users);
    void historyReceived(qint64 peer, const QJsonArray &msgs);
    void incomingMessage(const QJsonObject &msg);
    void messageAck(qint64 cid, qint64 id, qint64 ts, bool delivered, qint64 to);
    void readReceipt(qint64 fromId, const QJsonArray &ids);
    void presenceChanged(qint64 userId, bool online);
    void deliveredReceipt(const QJsonArray &ids);
    void pendingMessages(const QJsonArray &msgs);
};

#endif // NETCLIENT_H
