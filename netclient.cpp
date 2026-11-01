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
    } else if (type == JSON_MESSEGES::M_USERS) {
        emit usersReceived(obj[JSON_FIELDS::F_LIST].toArray());
    } else if (type == JSON_MESSEGES::M_HISTORY) {
        emit historyReceived(obj[JSON_FIELDS::F_PEER].toVariant().toLongLong(), obj[JSON_FIELDS::F_MSGS].toArray());
    } else if (type == JSON_MESSEGES::M_MSG) {
        emit incomingMessage(obj[JSON_FIELDS::F_MSG].toObject());
    } else if (type == JSON_MESSEGES::M_MSG_ACK) {
        emit messageAck(obj[JSON_FIELDS::F_CID].toVariant().toLongLong(),
                        obj[JSON_FIELDS::F_ID].toVariant().toLongLong(),
                        obj[JSON_FIELDS::F_TS].toVariant().toLongLong(),
                        obj[JSON_FIELDS::F_DELIVERED].toBool(),
                        obj[JSON_FIELDS::F_TO].toVariant().toLongLong());
    } else if (type == JSON_MESSEGES::M_READ) {
        emit readReceipt(obj[JSON_FIELDS::F_PEER].toVariant().toLongLong(), obj[JSON_FIELDS::F_IDS].toArray());
    } else if (type == JSON_MESSEGES::M_PRESENCE) {
        emit presenceChanged(obj[JSON_FIELDS::F_ID].toVariant().toLongLong(), obj[JSON_FIELDS::F_ONLINE].toBool());
    } else if (type == JSON_MESSEGES::M_DELIVERED) {
        emit deliveredReceipt(obj[JSON_FIELDS::F_IDS].toArray());
    } else if (type == JSON_MESSEGES::M_PENDING) {
        emit pendingMessages(obj[JSON_FIELDS::F_MSGS].toArray());
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

void NetClient::requestUsers()
{
    QJsonObject o;
    o[JSON_FIELDS::F_TYPE] = JSON_MESSEGES::M_USERS;
    send(o);
}

void NetClient::requestHistory(qint64 peer)
{
    QJsonObject o;
    o[JSON_FIELDS::F_TYPE] = JSON_MESSEGES::M_HISTORY;
    o[JSON_FIELDS::F_PEER] = (double)peer;
    send(o);
}

void NetClient::sendMessage(qint64 to, const QString &text, const QString &fileName, const QByteArray &fileData, qint64 cid)
{
    QJsonObject o;
    o[JSON_FIELDS::F_TYPE] = JSON_MESSEGES::M_MSG;
    o[JSON_FIELDS::F_TO] = (double)to;
    o[JSON_FIELDS::F_TEXT] = text;
    o[JSON_FIELDS::F_FILENAME] = fileName;
    o[JSON_FIELDS::F_FILE_B64] = fileData.isEmpty() ? QString() : QString::fromUtf8(fileData.toBase64());
    o[JSON_FIELDS::F_CID] = (double)cid;
    send(o);
}

void NetClient::sendRead(qint64 peer, const QVector<qint64> &ids)
{
    QJsonArray a;
    for (qint64 id : ids) a.append((double)id);
    QJsonObject o;
    o[JSON_FIELDS::F_TYPE] = JSON_MESSEGES::M_READ;
    o[JSON_FIELDS::F_PEER] = (double)peer;
    o[JSON_FIELDS::F_IDS] = a;
    send(o);
}

