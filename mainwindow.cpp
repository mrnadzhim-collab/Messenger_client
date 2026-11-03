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

MainWindow::MainWindow(NetClient *net, const QJsonObject &user, QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow),
    m_net(net)
{
    m_ui->setupUi(this);

    m_myId = user["id"].toVariant().toLongLong();
    m_myName = user["name"].toString();

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

    connect(m_net, &NetClient::usersReceived, this, &MainWindow::onUsersReceived);
    connect(m_net, &NetClient::serverError, this, &MainWindow::serverError);
}


void MainWindow::onUsersReceived(const QJsonArray &users)
{
    for (const QJsonValue &v : users) {
        QJsonObject o = v.toObject();
        addUserEntry(o["id"].toVariant().toLongLong(),
                     o["name"].toString(),
                     o["username"].toString(),
                     o["online"].toBool());
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

