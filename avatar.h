#ifndef AVATAR_H
#define AVATAR_H

#include <QPixmap>
#include <QPainter>
#include <QColor>

inline QPixmap makeAvatar(const QString &name, bool online, int size = 44)
{
    QPixmap pm(size, size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);
    static const QColor colors[] = {
        QColor("#e53935"), QColor("#8e24aa"), QColor("#3949ab"), QColor("#039be5"),
        QColor("#00897b"), QColor("#7cb342"), QColor("#fb8c00"), QColor("#6d4c41")
    };
    uint h = 0;
    for (QChar ch : name) h = h * 31 + ch.unicode();
    p.setPen(Qt::NoPen);
    p.setBrush(colors[h % 8]);
    p.drawEllipse(0, 0, size, size);
    p.setPen(Qt::white);
    QFont f = p.font();
    f.setBold(true);
    f.setPointSize(qMax(size / 3, 8));
    p.setFont(f);
    p.drawText(pm.rect(), Qt::AlignCenter, name.left(1).toUpper());
    if (online) {
        int d = qMax(size / 4, 8);
        p.setBrush(QColor("#17212b"));
        p.setPen(Qt::NoPen);
        p.drawEllipse(size - d, size - d, d, d);
        p.setBrush(QColor("#4caf50"));
        p.drawEllipse(size - d + 2, size - d + 2, d - 4, d - 4);
    }
    return pm;
}

#endif // AVATAR_H
