#ifndef TYPES_H
#define TYPES_H

#include <QString>

// Описание типов сообщений
namespace JSON_MESSAGES
{
    const QString M_AUTH_OK         = QString("auth_ok");       // успешная авторизация
    const QString M_ERROR           = QString("error");         // ошибка
    const QString M_LOGIN           = QString("login");         // попытка авторизации
    const QString M_REGISTER        = QString("register");      // регистрация
}

// Описание полей JSON-документа
namespace JSON_FIELDS
{
    const QString F_TYPE            = QString("type");          // тип сообщения
    const QString F_USER            = QString("user");          // пользователь
    const QString F_MSG             = QString("msg");           // сообщение
    const QString F_USERNAME        = QString("username");      // логин
    const QString F_PASSWORD        = QString("password");      // пароль
    const QString F_NAME            = QString("name");          // имя пользователя
}

#endif // TYPES_H
