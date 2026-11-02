#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();

    Ui::MainWindow *m_ui;

    bool m_registerMode = false;

private slots:
    void on_auth();
    void on_switchMode();
    void on_showError(const QString &msg);
};
#endif // MAINWINDOW_H
