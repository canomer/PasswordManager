#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include "database.h"

class LoginWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWindow(Database *db, QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void handleLogin();
    void handleRegister();
    void switchToRegister();
    void switchToLogin();

private:
    void setupUI();
    void setupConnections();
    bool validateInput();
    
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *switchButton;
    QLabel *statusLabel;
    
    Database *db;
    bool isLoginMode;
};

#endif // LOGINWINDOW_H 