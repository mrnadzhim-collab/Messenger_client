#include "netclient.h"

#include "protocol.h"
#include "types.h"

NetClient::NetClient(QObject *parent)
    : QObject(parent),
    m_socket(new QTcpSocket(this))
{
    connect(m_socket, &QTcpSocket::readyRead, this, &NetClient::onReadyRead);
    connect(m_socket, &QTcpSocket::connected, this, &NetClient::onConnected);
    connect(m_socket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
            this, &NetClient::onSocketError);
    connect(m_socket, &QTcpSocket::disconnected, this, &NetClient::onDisconnected);
}

void NetClient::parse(const QJsonObject &obj)
{
    const QString type = obj[JSON_FIELDS::F_TYPE].toString();
    if (type == JSON_MESSEGES::M_AUTH_OK) {
        emit authOk(obj[JSON_FIELDS::F_USER].toObject());
    } else if (type == JSON_MESSEGES::M_ERROR) {
        emit serverError(obj[JSON_FIELDS::F_MSG].toString());
    }
}

void NetClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());
    const QList<QJsonObject> packets = Protocol::decode(m_buffer);
    for (const QJsonObject &obj : packets)
        parse(obj);
}

void NetClient::onConnected()
{
    m_connected = true;
    emit connectedToServer();
    flushAuth();
}

void NetClient::onDisconnected()
{
    if (m_connected) {
        m_connected = false;
        emit connectionFailed(QString("Соединение с сервером разорвано"));
    }
}

void NetClient::onSocketError()
{
    if (!m_connected)
        emit connectionFailed(m_socket->errorString());
}

void NetClient::send(const QJsonObject &obj)
{
    m_socket->write(Protocol::encode(obj));
}

void NetClient::flushAuth()
{
    if (!m_auth.isEmpty() && m_socket->state() == QAbstractSocket::ConnectedState) {
        send(m_auth);
        m_auth = QJsonObject();
    }
}

void NetClient::connectToServer(const QString &host, quint16 port)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        flushAuth();
        return;
    }
    if (m_socket->state() != QAbstractSocket::ConnectingState)
        m_socket->connectToHost(host, port);
}

void NetClient::login(const QString &username, const QString &password)
{
    QJsonObject o;
    o[JSON_FIELDS::F_TYPE] = JSON_MESSEGES::M_LOGIN;
    o[JSON_FIELDS::F_USERNAME] = username;
    o[JSON_FIELDS::F_PASSWORD] = password;
    m_auth = o;
    flushAuth();
}

void NetClient::registerUser(const QString &username, const QString &password, const QString &name)
{
    QJsonObject o;
    o[JSON_FIELDS::F_TYPE] = JSON_MESSEGES::M_REGISTER;
    o[JSON_FIELDS::F_USERNAME] = username;
    o[JSON_FIELDS::F_PASSWORD] = password;
    o[JSON_FIELDS::F_NAME] = name;
    m_auth = o;
    flushAuth();
}

