#include "loginwindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QScreen>
#include <QStyle>

LoginWindow::LoginWindow(Database *db, QWidget *parent)
    : QDialog(parent)
    , db(db)
    , isLoginMode(true)
{
    setupUI();
    setupConnections();
    setWindowTitle(tr("Password Manager - Login"));
    setFixedSize(400, 300);
    
    // Center the login window on screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            qApp->primaryScreen()->availableGeometry()
        )
    );
}

LoginWindow::~LoginWindow()
{
}

void LoginWindow::setupUI()
{
    // Set dark theme
    QString darkStyle = 
        "QDialog { background-color: #1E1E1E; color: #FFFFFF; }"
        "QLabel { color: #FFFFFF; }"
        "QPushButton { background-color: #0078D7; color: white; border: none; padding: 5px 10px; border-radius: 2px; }"
        "QPushButton:hover { background-color: #1C97EA; }"
        "QPushButton:pressed { background-color: #00559B; }"
        "QLineEdit { background-color: #333333; color: #FFFFFF; border: 1px solid #3E3E3E; padding: 5px; border-radius: 2px; }";
    
    setStyleSheet(darkStyle);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // App logo or title
    QLabel *titleLabel = new QLabel(tr("Password Manager"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // Username
    QLabel *usernameLabel = new QLabel(tr("Username:"), this);
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText(tr("Enter username"));
    
    // Password
    QLabel *passwordLabel = new QLabel(tr("Password:"), this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(tr("Enter password"));
    
    // Confirm Password (initially hidden)
    QLabel *confirmLabel = new QLabel(tr("Confirm Password:"), this);
    confirmPasswordEdit = new QLineEdit(this);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setPlaceholderText(tr("Confirm password"));
    confirmLabel->hide();
    confirmPasswordEdit->hide();
    
    // Buttons
    loginButton = new QPushButton(tr("Login"), this);
    registerButton = new QPushButton(tr("Register"), this);
    switchButton = new QPushButton(tr("Switch to Register"), this);
    registerButton->hide();
    
    // Status Label
    statusLabel = new QLabel(this);
    statusLabel->setStyleSheet("QLabel { color: #FF6347; }");
    
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(usernameLabel);
    mainLayout->addWidget(usernameEdit);
    mainLayout->addWidget(passwordLabel);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(confirmLabel);
    mainLayout->addWidget(confirmPasswordEdit);
    mainLayout->addWidget(statusLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(registerButton);
    mainLayout->addWidget(switchButton);
    mainLayout->addStretch();
}

void LoginWindow::setupConnections()
{
    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::handleRegister);
    connect(switchButton, &QPushButton::clicked, this, [this]() {
        if (isLoginMode) {
            switchToRegister();
        } else {
            switchToLogin();
        }
    });
}

void LoginWindow::handleLogin()
{
    if (!validateInput()) {
        return;
    }
    
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    
    // Implement actual login logic with database
    if (db->validateUser(username, password)) {
        accept(); // Close dialog with Accepted result
    } else {
        statusLabel->setText(tr("Invalid username or password"));
    }
}

void LoginWindow::handleRegister()
{
    if (!validateInput()) {
        return;
    }
    
    QString username = usernameEdit->text();
    QString password = passwordEdit->text();
    QString confirmPassword = confirmPasswordEdit->text();
    
    if (password != confirmPassword) {
        statusLabel->setText(tr("Passwords do not match"));
        return;
    }
    
    // Implement actual registration logic with database
    if (db->createUser(username, password)) {
        QMessageBox::information(this, tr("Success"), tr("Registration successful. You can now login."));
        switchToLogin();
    } else {
        statusLabel->setText(tr("Username already exists or registration failed"));
    }
}

void LoginWindow::switchToRegister()
{
    isLoginMode = false;
    setWindowTitle(tr("Password Manager - Register"));
    loginButton->hide();
    registerButton->show();
    confirmPasswordEdit->parentWidget()->show();
    confirmPasswordEdit->show();
    switchButton->setText(tr("Switch to Login"));
    statusLabel->clear();
}

void LoginWindow::switchToLogin()
{
    isLoginMode = true;
    setWindowTitle(tr("Password Manager - Login"));
    registerButton->hide();
    loginButton->show();
    confirmPasswordEdit->parentWidget()->hide();
    confirmPasswordEdit->hide();
    switchButton->setText(tr("Switch to Register"));
    statusLabel->clear();
}

bool LoginWindow::validateInput()
{
    if (usernameEdit->text().isEmpty()) {
        statusLabel->setText(tr("Username cannot be empty"));
        return false;
    }
    
    if (passwordEdit->text().isEmpty()) {
        statusLabel->setText(tr("Password cannot be empty"));
        return false;
    }
    
    if (!isLoginMode && confirmPasswordEdit->text().isEmpty()) {
        statusLabel->setText(tr("Please confirm your password"));
        return false;
    }
    
    return true;
} 