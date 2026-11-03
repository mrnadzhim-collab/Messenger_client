#ifndef MESSAGEWIDGET_H
#define MESSAGEWIDGET_H

#include <QWidget>
#include "messegedata.h"

class QLabel;

class MessageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageWidget(const Message &msg, QWidget *parent = nullptr);

private:
    Message m_msg;
};

#endif // MESSAGEWIDGET_H
