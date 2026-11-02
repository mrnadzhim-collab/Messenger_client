#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(NetClient *net, QWidget *parent)
    : QWidget(parent),
    m_ui(new Ui::LoginWindow),
    m_net(net)
{
    m_ui->setupUi(this);

    setupUI();
}

LoginWindow::~LoginWindow()
{
    delete m_ui;
}

void LoginWindow::setupUI()
{
    setFixedSize(360, 480);

    m_ui->lineEdit_name->setVisible(false);

    connect(m_ui->pushButton_action, SIGNAL(clicked(bool)), this, SLOT(on_auth()));
    connect(m_ui->pushButton_switch, SIGNAL(clicked(bool)), this, SLOT(on_switchMode()));
}

void LoginWindow::on_auth()
{

}

void LoginWindow::on_switchMode()
{
    m_registerMode = !m_registerMode;
    m_ui->lineEdit_name->setVisible(m_registerMode);
    m_ui->pushButton_action->setText(m_registerMode ? QString("Зарегистрироваться") : QString("Войти"));
    m_ui->pushButton_switch->setText(m_registerMode ? QString("Уже есть аккаунт? Войти")
                                                    : QString("Нет аккаунта? Зарегистрироваться"));
    m_ui->label_error->setVisible(false);
}

void LoginWindow::on_showError(const QString &msg)
{
    m_ui->label_error->setText(msg);
    m_ui->label_error->setVisible(true);
    m_ui->pushButton_action->setEnabled(true);
    m_ui->pushButton_action->setText(m_registerMode ? QString("Зарегистрироваться") : QString("Войти"));
}
