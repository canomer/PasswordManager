#include "passworddialog.h"
#include <QGridLayout>
#include <QRandomGenerator>

PasswordDialog::PasswordDialog(QWidget *parent, bool isEdit)
    : QDialog(parent)
    , isEditMode(isEdit)
{
    setupUI();
    setupConnections();
    
    setWindowTitle(isEditMode ? tr("Edit Password") : tr("Add Password"));
    setFixedSize(400, 300);
}

PasswordDialog::~PasswordDialog()
{
}

void PasswordDialog::setupUI()
{
    // Set dark theme
    QString darkStyle = 
        "QDialog { background-color: #1E1E1E; color: #FFFFFF; }"
        "QLabel { color: #FFFFFF; }"
        "QPushButton { background-color: #0078D7; color: white; border: none; padding: 5px 10px; border-radius: 2px; }"
        "QPushButton:hover { background-color: #1C97EA; }"
        "QPushButton:pressed { background-color: #00559B; }"
        "QPushButton:disabled { background-color: #333333; color: #666666; }"
        "QPushButton:checked { background-color: #00559B; }"
        "QLineEdit { background-color: #333333; color: #FFFFFF; border: 1px solid #3E3E3E; padding: 5px; border-radius: 2px; }";
    
    setStyleSheet(darkStyle);

    QGridLayout *mainLayout = new QGridLayout(this);
    
    // Title
    QLabel *titleLabel = new QLabel(isEditMode ? tr("Edit Password") : tr("Add New Password"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    // Website
    QLabel *websiteLabel = new QLabel(tr("Website:"), this);
    websiteEdit = new QLineEdit(this);
    websiteEdit->setPlaceholderText(tr("Enter website URL"));
    
    // Username
    QLabel *usernameLabel = new QLabel(tr("Username:"), this);
    usernameEdit = new QLineEdit(this);
    usernameEdit->setPlaceholderText(tr("Enter username or email"));
    
    // Password
    QLabel *passwordLabel = new QLabel(tr("Password:"), this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setPlaceholderText(tr("Enter password"));
    
    // Password visibility toggle
    toggleVisibilityButton = new QPushButton(tr("Show"), this);
    toggleVisibilityButton->setCheckable(true);
    toggleVisibilityButton->setFixedWidth(60);
    
    // Generate password button
    generateButton = new QPushButton(tr("Generate"), this);
    
    // Status label
    statusLabel = new QLabel(this);
    statusLabel->setStyleSheet("QLabel { color: #FF6347; }");
    
    // Dialog buttons
    okButton = new QPushButton(isEditMode ? tr("Update") : tr("Add"), this);
    cancelButton = new QPushButton(tr("Cancel"), this);
    okButton->setEnabled(false);
    
    // Layout
    mainLayout->addWidget(titleLabel, 0, 0, 1, 3);
    mainLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed), 1, 0);
    
    mainLayout->addWidget(websiteLabel, 2, 0);
    mainLayout->addWidget(websiteEdit, 2, 1, 1, 2);
    
    mainLayout->addWidget(usernameLabel, 3, 0);
    mainLayout->addWidget(usernameEdit, 3, 1, 1, 2);
    
    mainLayout->addWidget(passwordLabel, 4, 0);
    mainLayout->addWidget(passwordEdit, 4, 1);
    mainLayout->addWidget(toggleVisibilityButton, 4, 2);
    
    mainLayout->addWidget(generateButton, 5, 1);
    mainLayout->addWidget(statusLabel, 6, 0, 1, 3);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout, 7, 0, 1, 3);
    
    mainLayout->setRowStretch(8, 1);
}

void PasswordDialog::setupConnections()
{
    connect(websiteEdit, &QLineEdit::textChanged, this, &PasswordDialog::validateInput);
    connect(usernameEdit, &QLineEdit::textChanged, this, &PasswordDialog::validateInput);
    connect(passwordEdit, &QLineEdit::textChanged, this, &PasswordDialog::validateInput);
    
    connect(generateButton, &QPushButton::clicked, this, &PasswordDialog::generatePassword);
    connect(toggleVisibilityButton, &QPushButton::toggled, this, &PasswordDialog::togglePasswordVisibility);
    
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void PasswordDialog::validateInput()
{
    bool isValid = !websiteEdit->text().isEmpty() &&
                   !usernameEdit->text().isEmpty() &&
                   !passwordEdit->text().isEmpty();
    
    okButton->setEnabled(isValid);
    
    if (isValid) {
        statusLabel->clear();
    } else {
        statusLabel->setText(tr("Please fill in all fields"));
    }
}

void PasswordDialog::generatePassword()
{
    QString newPassword = generateRandomPassword();
    passwordEdit->setText(newPassword);
    toggleVisibilityButton->setChecked(true);
    passwordEdit->setEchoMode(QLineEdit::Normal);
}

void PasswordDialog::togglePasswordVisibility()
{
    if (toggleVisibilityButton->isChecked()) {
        passwordEdit->setEchoMode(QLineEdit::Normal);
        toggleVisibilityButton->setText(tr("Hide"));
    } else {
        passwordEdit->setEchoMode(QLineEdit::Password);
        toggleVisibilityButton->setText(tr("Show"));
    }
}

QString PasswordDialog::generateRandomPassword(int length) const
{
    const QString chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_+-=[]{}|;:,.<>?";
    QString result;
    
    for (int i = 0; i < length; ++i) {
        int index = QRandomGenerator::global()->bounded(chars.length());
        result.append(chars.at(index));
    }
    
    return result;
}

QString PasswordDialog::getWebsite() const
{
    return websiteEdit->text();
}

QString PasswordDialog::getUsername() const
{
    return usernameEdit->text();
}

QString PasswordDialog::getPassword() const
{
    return passwordEdit->text();
}

void PasswordDialog::setWebsite(const QString &website)
{
    websiteEdit->setText(website);
}

void PasswordDialog::setUsername(const QString &username)
{
    usernameEdit->setText(username);
}

void PasswordDialog::setPassword(const QString &password)
{
    passwordEdit->setText(password);
} 