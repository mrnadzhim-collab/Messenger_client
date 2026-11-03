#ifndef CHATITEMWIDGET_H
#define CHATITEMWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>

#include "avatar.h"

class ChatItemWidget : public QWidget
{
public:
    ChatItemWidget(const QString &name, const QString &preview, const QString &timeStr,
                   int unread, bool online, QWidget *parent = nullptr) : QWidget(parent)
    {
        setAttribute(Qt::WA_StyledBackground, true);
        setFixedHeight(64);

        QHBoxLayout *lay = new QHBoxLayout(this);
        lay->setContentsMargins(10, 8, 10, 8);
        lay->setSpacing(10);

        QLabel *avatar = new QLabel;
        avatar->setPixmap(makeAvatar(name, online));
        avatar->setFixedSize(44, 44);
        lay->addWidget(avatar);

        QVBoxLayout *mid = new QVBoxLayout;
        mid->setSpacing(2);
        QLabel *nameL = new QLabel(name);
        nameL->setStyleSheet(QString("color:#ffffff; font-weight:bold; font-size:13px; background:transparent;"));
        QLabel *prevL = new QLabel;
        prevL->setStyleSheet(QString("color:#7f91a4; font-size:12px; background:transparent;"));
        prevL->setText(prevL->fontMetrics().elidedText(preview, Qt::ElideRight, 150));
        mid->addWidget(nameL);
        mid->addWidget(prevL);
        lay->addLayout(mid, 1);

        QVBoxLayout *rightLay = new QVBoxLayout;
        rightLay->setSpacing(4);
        rightLay->setAlignment(Qt::AlignTop | Qt::AlignRight);
        QLabel *timeL = new QLabel(timeStr);
        timeL->setAlignment(Qt::AlignRight);
        timeL->setStyleSheet(QString("color:#5b6b7c; font-size:11px; background:transparent;"));
        rightLay->addWidget(timeL);
        if (unread > 0) {
            QLabel *badge = new QLabel(unread > 99 ? QString("99+") : QString::number(unread));
            badge->setAlignment(Qt::AlignCenter);
            badge->setFixedSize(20, 20);
            badge->setStyleSheet(QStringLiteral(
                "background:#5288c1; color:#ffffff; border-radius:10px; font-size:11px; font-weight:bold;"));
            rightLay->addWidget(badge, 0, Qt::AlignRight);
        }
        lay->addLayout(rightLay);

        setSelected(false);
    }


    void setSelected(bool selected)
    {
        if (selected)
            setStyleSheet(QString("ChatItemWidget { background:#2b5278; border-radius:8px; }"));
        else
            setStyleSheet(QString("ChatItemWidget { background:transparent; }"));
    }
};

#endif // CHATITEMWIDGET_H
