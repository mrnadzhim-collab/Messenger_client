#ifndef MESSEGEDATA_H
#define MESSEGEDATA_H

#include <QString>
#include <QDateTime>

struct Message
{
    qint64 id = -1;       // id в базе сервера (-1 пока не подтверждено)
    qint64 cid = -1;      // временный id клиента (для подтверждения)
    qint64 from = -1;
    bool outgoing = false;
    QString text;
    QString fileName;
    bool hasFile = false;
    qint64 ts = 0;
    int status = 0;       // 0 — ожидает, 1 — отправлено, 2 — доставлено, 3 — прочитано
    QString localPath;    // путь к скачанному/отправленному файлу на диске
};

inline QString fmtMsgTime(qint64 ts)
{
    if (ts <= 0) return QString();
    QDateTime dt = QDateTime::fromSecsSinceEpoch(ts);
    if (dt.date() == QDate::currentDate())
        return dt.toString(QStringLiteral("hh:mm"));
    return dt.toString(QStringLiteral("dd.MM.yy hh:mm"));
}

#endif // MESSEGEDATA_H
