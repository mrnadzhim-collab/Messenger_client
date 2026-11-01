#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "chatitemwidget.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QStandardPaths>
#include <QDir>
#include <QMessageBox>

#include "types.h"
#include "messagewidget.h"

MainWindow::MainWindow(NetClient *net, const QJsonObject &user, QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow),
    m_net(net)
{
    m_ui->setupUi(this);

    m_myId = user[JSON_FIELDS::F_ID].toVariant().toLongLong();
    m_myName = user[JSON_FIELDS::F_NAME].toString();

    m_filesDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/files";
    QDir().mkpath(m_filesDir);

    m_net->requestUsers();

    setupUI();
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::setupUI()
{
    setWindowTitle(QString("Чат для общения — %1").arg(m_myName));
    resize(980, 620);
    setMinimumSize(760, 480);

    m_ui->attachRow->setVisible(false);

    connect(m_net, &NetClient::usersReceived, this, &MainWindow::onUsersReceived);
    connect(m_net, &NetClient::serverError, this, &MainWindow::serverError);
    connect(m_net, &NetClient::historyReceived, this, &MainWindow::onHistoryReceived);
    connect(m_net, &NetClient::messageAck, this, &MainWindow::onAck);
    connect(m_net, &NetClient::incomingMessage, this, &MainWindow::onIncoming);
    connect(m_net, &NetClient::readReceipt, this, &MainWindow::onReadReceipt);
    connect(m_net, &NetClient::presenceChanged, this, &MainWindow::onPresence);
    connect(m_net, &NetClient::deliveredReceipt, this, &MainWindow::onDelivered);
    connect(m_net, &NetClient::pendingMessages, this, &MainWindow::onPending);

    connect(m_ui->pushButton_clearAttach, &QPushButton::clicked, this, &MainWindow::clearAttach);
    connect(m_ui->pushButton_send, &QPushButton::clicked, this, &MainWindow::sendCurrent);

    connect(m_ui->listWidget_chatList, &QListWidget::itemClicked, this, [this](QListWidgetItem *item){
        openChat(item->data(Qt::UserRole).toLongLong());
    });
}

void MainWindow::onUsersReceived(const QJsonArray &users)
{
    for (const QJsonValue &v : users) {
        QJsonObject o = v.toObject();
        addUserEntry(o[JSON_FIELDS::F_ID].toVariant().toLongLong(),
                     o[JSON_FIELDS::F_NAME].toString(),
                     o[JSON_FIELDS::F_USERNAME].toString(),
                     o[JSON_FIELDS::F_ONLINE].toBool());
    }
    rebuildChats();
}

void MainWindow::serverError(const QString &msg)
{
    QMessageBox::warning(this, QString("Ошибка сервера"), msg);
}

void MainWindow::addUserEntry(qint64 id, const QString &name, const QString &username, bool online)
{
    Chat &c = ensureChat(id);
    c.name = name;
    c.username = username;
    c.online = online;
}

void MainWindow::rebuildChats()
{
    const QString filter = m_ui->lineEdit_search->text().trimmed().toLower();
    m_ui->listWidget_chatList->clear();
    QList<qint64> ids = m_chats.keys();
    std::sort(ids.begin(), ids.end(), [this](qint64 a, qint64 b) {
        return m_chats[a].lastTs > m_chats[b].lastTs;
    });
    for (qint64 id : ids) {
        const Chat &c = m_chats[id];
        if (!filter.isEmpty() && !c.name.toLower().contains(filter) &&
            !c.username.toLower().contains(filter))
            continue;
        QListWidgetItem *item = new QListWidgetItem(m_ui->listWidget_chatList);
        item->setData(Qt::UserRole, QVariant((qlonglong)id));
        ChatItemWidget *w = new ChatItemWidget(c.name, chatPreview(c),
                                               fmtMsgTime(c.lastTs), c.unread, c.online);
        w->setSelected(id == m_current);
        item->setSizeHint(w->sizeHint());
        m_ui->listWidget_chatList->addItem(item);
        m_ui->listWidget_chatList->setItemWidget(item, w);
    }
}

Chat &MainWindow::ensureChat(qint64 id)
{
    if (!m_chats.contains(id)) {
        Chat c;
        c.id = id;
        c.name = QString("Пользователь #%1").arg(id);
        m_chats.insert(id, c);
    }
    return m_chats[id];
}

QString MainWindow::chatPreview(const Chat &c) const
{
    if (c.msgs.isEmpty()) {
        return c.username.isEmpty() ? QString("Нет сообщений")
                                    : QString("@") + c.username;
    }
    const Message &m = c.msgs.last();
    QString p = m.hasFile ? (QString("[файл] ") + m.fileName) : m.text;
    p.replace('\n', ' ');
    if (m.outgoing)
        p = QString("Вы: ") + p;
    return p;
}

void MainWindow::openChat(qint64 peer)
{
    m_current = peer;
    Chat &c = ensureChat(peer);
    c.unread = 0;
    updateHeader();
    rebuildChats();
    m_net->requestHistory(peer);
}

void MainWindow::updateHeader()
{
    if (m_current < 0 || !m_chats.contains(m_current))
        return;
    const Chat &c = m_chats[m_current];
    m_ui->headerAvatar->setPixmap(makeAvatar(c.name, c.online, 40));
    m_ui->headerName->setText(c.name);
    if (c.online) {
        m_ui->headerStatus->setText(QString("в сети"));
        m_ui->headerStatus->setStyleSheet(QString("color:#4fc3f7; font-size:12px; background:transparent;"));
    } else {
        m_ui->headerStatus->setText(QString("не в сети"));
        m_ui->headerStatus->setStyleSheet(QString("color:#7f91a4; font-size:12px; background:transparent;"));
    }
}

void MainWindow::onHistoryReceived(qint64 peer, const QJsonArray &msgs)
{
    Chat &c = ensureChat(peer);
    c.msgs.clear();
    for (const QJsonValue &v : msgs) {
        QJsonObject o = v.toObject();
        Message m;
        m.id = o[JSON_FIELDS::F_TYPE].toVariant().toLongLong();
        m.from = o[JSON_FIELDS::F_FROM].toVariant().toLongLong();
        m.outgoing = (m.from == m_myId);
        m.text = o[JSON_FIELDS::F_TEXT].toString();
        m.fileName = o[JSON_FIELDS::F_FILENAME].toString();
        m.hasFile = o[JSON_FIELDS::F_HASFILE].toBool();
        m.ts = o[JSON_FIELDS::F_TS].toVariant().toLongLong();
        if (m.outgoing) {
            if (o[JSON_FIELDS::F_READ].toInt()) m.status = MessageStatus::Readed;
            else if (o[JSON_FIELDS::F_DELIVERED].toInt()) m.status = MessageStatus::Delivered;
            else m.status = MessageStatus::Sended;
        }
        if (m.hasFile) {
            QString candidate = m_filesDir + "/" + QString::number(m.id) + "_" + m.fileName;
            if (QFile::exists(candidate))
                m.localPath = candidate;
        }
        c.msgs.append(m);
        if (m.ts > c.lastTs) c.lastTs = m.ts;
    }
    c.unread = 0;
    if (m_current == peer)
        rebuildMessages();
    rebuildChats();
}

void MainWindow::rebuildMessages()
{
    m_ui->listWidget_msgList->clear();
    if (m_current < 0 || !m_chats.contains(m_current))
        return;
    const Chat &c = m_chats[m_current];
    for (const Message &m : c.msgs) {
        QListWidgetItem *item = new QListWidgetItem(m_ui->listWidget_msgList);
        MessageWidget *w = new MessageWidget(m);
        item->setSizeHint(w->sizeHint());
        m_ui->listWidget_msgList->addItem(item);
        m_ui->listWidget_msgList->setItemWidget(item, w);
    }
    m_ui->listWidget_msgList->scrollToBottom();
}

void MainWindow::sendCurrent()
{
    if (m_current < 0)
        return;

    const QString text = m_ui->lineEdit_input->text().trimmed();

    QString fileName = "";
    QByteArray fileData;;

    if (text.isEmpty())
        return;

    Message m;
    m.cid = m_nextCid++;
    m.from = m_myId;
    m.outgoing = true;
    m.text = text;
    m.fileName = fileName;
    m.hasFile = !fileName.isEmpty();
    m.ts = QDateTime::currentSecsSinceEpoch();
    m.status = 0;
    if (m.hasFile)
        m.localPath = m_attachPath;

    Chat &c = ensureChat(m_current);
    c.msgs.append(m);
    if (m.ts > c.lastTs) c.lastTs = m.ts;

    m_net->sendMessage(m_current, text, fileName, fileData, m.cid);
    m_ui->lineEdit_input->clear();
    clearAttach();
    rebuildMessages();
    rebuildChats();
}

void MainWindow::clearAttach()
{
    m_attachPath.clear();
    m_ui->attachLabel->clear();
    m_ui->attachRow->setVisible(false);
}

void MainWindow::onIncoming(const QJsonObject &msg)
{
    addIncomingMessage(msg, true);
}

void MainWindow::onAck(qint64 cid, qint64 id, qint64 ts, bool delivered, qint64 to)
{
    if (!m_chats.contains(to))
        return;
    Chat &c = m_chats[to];
    for (int i = 0; i < c.msgs.size(); ++i) {
        if (c.msgs[i].cid == cid) {
            c.msgs[i].id = id;
            c.msgs[i].ts = ts;
            c.msgs[i].status = delivered ? 2 : 1;
            break;
        }
    }
    if (m_current == to)
        rebuildMessages();
    else
        rebuildChats();
}

void MainWindow::addIncomingMessage(const QJsonObject &o, bool countUnread)
{
    const qint64 from = o[JSON_FIELDS::F_FROM].toVariant().toLongLong();
    Chat &c = ensureChat(from);
    Message m;
    m.id = o[JSON_FIELDS::F_ID].toVariant().toLongLong();
    m.from = from;
    m.text = o[JSON_FIELDS::F_TEXT].toString();
    m.fileName = o[JSON_FIELDS::F_FILENAME].toString();
    m.hasFile = o[JSON_FIELDS::F_HASFILE].toBool();
    m.ts = o[JSON_FIELDS::F_TS].toVariant().toLongLong();
    if (m.hasFile) {
        QString candidate = m_filesDir + "/" + QString::number(m.id) + "_" + m.fileName;
        if (QFile::exists(candidate))
            m.localPath = candidate;
    }
    c.msgs.append(m);
    if (m.ts > c.lastTs) c.lastTs = m.ts;

    if (m_current == from) {
        m_net->sendRead(from, QVector<qint64>() << m.id);
        rebuildMessages();
    } else if (countUnread) {
        c.unread++;
    }
    rebuildChats();
}

void MainWindow::onReadReceipt(qint64 fromId, const QJsonArray &ids)
{
    if (!m_chats.contains(fromId))
        return;
    QSet<qint64> idSet;
    for (const QJsonValue &v : ids)
        idSet.insert(v.toVariant().toLongLong());
    Chat &c = m_chats[fromId];
    for (Message &m : c.msgs)
        if (m.outgoing && idSet.contains(m.id))
            m.status = 3;
    if (m_current == fromId)
        rebuildMessages();
    else
        rebuildChats();
}

void MainWindow::onPresence(qint64 userId, bool online)
{
    if (!m_chats.contains(userId)) {
        m_net->requestUsers();
        return;
    }
    Chat &c = m_chats[userId];
    c.online = online;
    if (m_current == userId)
        updateHeader();
    rebuildChats();
}

void MainWindow::onDelivered(const QJsonArray &ids)
{
    QSet<qint64> idSet;
    for (const QJsonValue &v : ids)
        idSet.insert(v.toVariant().toLongLong());
    bool currentChanged = false;
    for (auto it = m_chats.begin(); it != m_chats.end(); ++it) {
        bool changed = false;
        for (Message &m : it.value().msgs) {
            if (m.outgoing && idSet.contains(m.id) && m.status < 2) {
                m.status = 2;
                changed = true;
            }
        }
        if (changed && m_current == it.key())
            currentChanged = true;
    }
    if (currentChanged)
        rebuildMessages();
    else
        rebuildChats();
}

void MainWindow::onPending(const QJsonArray &msgs)
{
    for (const QJsonValue &v : msgs)
        addIncomingMessage(v.toObject(), true);
}

