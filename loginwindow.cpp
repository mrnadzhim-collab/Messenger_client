#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(NetClient *net, QWidget *parent)
    : QWidget(parent),
    m_ui(new Ui::LoginWindow),
    m_net(net)
{
    m_ui->setupUi(this);

    setupUI();

    connect(m_net, &NetClient::authOk, this, [this](const QJsonObject &user){
        emit authSuccess(user);
        close();
    });
    connect(m_net, &NetClient::serverError, this, &LoginWindow::on_showError);
    connect(m_net, &NetClient::connectionFailed, this, &LoginWindow::on_showError);
}

LoginWindow::~LoginWindow()
{
    delete m_ui;
}

void LoginWindow::setupUI()
{
    setFixedSize(360, 480);

    m_ui->lineEdit_name->setVisible(false);
    m_ui->label_error->setVisible(false);

    connect(m_ui->pushButton_action, SIGNAL(clicked(bool)), this, SLOT(on_auth()));
    connect(m_ui->pushButton_switch, SIGNAL(clicked(bool)), this, SLOT(on_switchMode()));
}

void LoginWindow::on_auth()
{
    const QString username = m_ui->lineEdit_username->text().trimmed();
    const QString password = m_ui->lineEdit_password->text();
    if (username.isEmpty() || password.isEmpty()) {
        on_showError(QString("Введите логин и пароль"));
        return;
    }
    if (m_registerMode && m_ui->lineEdit_name->text().trimmed().isEmpty()) {
        on_showError(QString("Введите имя"));
        return;
    }
    m_ui->label_error->setVisible(false);
    m_ui->pushButton_action->setEnabled(false);
    m_ui->pushButton_action->setText(QString("Подключение..."));
    m_net->connectToServer(m_ui->lineEdit_host->text().trimmed(), (quint16)m_ui->lineEdit_port->text().toUShort());
    if (m_registerMode)
        m_net->registerUser(username, password, m_ui->lineEdit_name->text().trimmed());
    else
        m_net->login(username, password);
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
