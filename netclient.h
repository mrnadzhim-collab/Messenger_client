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
};

#endif // NETCLIENT_H
