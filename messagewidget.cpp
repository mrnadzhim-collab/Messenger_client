#include "messagewidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QFileInfo>
#include <QPixmap>
#include <QEvent>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>

static QString statusText(int status)
{
    switch (status) {
    case MessageStatus::Pended: return QString("…");
    case MessageStatus::Sended: return QString("✓");
    case MessageStatus::Delivered: return QString("✓✓");
    case MessageStatus::Readed: return QString("✓✓");
    }
    return QString();
}

static QString statusColor(int status)
{
    return status == MessageStatus::Readed ? QString("#4fc3f7") : QString("#7f91a4");
}

MessageWidget::MessageWidget(const Message &msg, QWidget *parent)
    : QWidget(parent), m_msg(msg)
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(10, 3, 10, 3);

    QFrame *bubble = new QFrame;
    bubble->setObjectName(QString("bubble"));
    QVBoxLayout *bl = new QVBoxLayout(bubble);
    bl->setContentsMargins(10, 7, 10, 6);
    bl->setSpacing(4);

    if (!msg.text.isEmpty()) {
        QLabel *txt = new QLabel(msg.text);
        txt->setWordWrap(true);
        txt->setTextInteractionFlags(Qt::TextSelectableByMouse);
        txt->setMaximumWidth(380);
        txt->setStyleSheet(QStringLiteral("color:#ffffff; background:transparent;"));
        bl->addWidget(txt);
    }

    QHBoxLayout *bottom = new QHBoxLayout;
    bottom->setSpacing(4);
    QLabel *time = new QLabel(fmtMsgTime(msg.ts));
    time->setStyleSheet(QStringLiteral("color:#7f91a4; font-size:10px; background:transparent;"));
    bottom->addStretch(1);
    bottom->addWidget(time);
    if (msg.outgoing) {
        QLabel *st = new QLabel(statusText(msg.status));
        st->setStyleSheet(QStringLiteral("color:%1; font-size:10px; background:transparent;")
                              .arg(statusColor(msg.status)));
        bottom->addWidget(st);
    }
    bl->addLayout(bottom);

    bubble->setMaximumWidth(440);
    if (msg.outgoing) {
        lay->addStretch(1);
        lay->addWidget(bubble, 0, Qt::AlignTop);
        bubble->setStyleSheet(QStringLiteral(
            "QFrame#bubble { background:#2b5278; border-radius:10px; }"));
    } else {
        lay->addWidget(bubble, 0, Qt::AlignTop);
        lay->addStretch(1);
        bubble->setStyleSheet(QStringLiteral(
            "QFrame#bubble { background:#182533; border-radius:10px; }"));
    }
}
