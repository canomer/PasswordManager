#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>

class PasswordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PasswordDialog(QWidget *parent = nullptr, bool isEdit = false);
    ~PasswordDialog();

    QString getWebsite() const;
    QString getUsername() const;
    QString getPassword() const;

    void setWebsite(const QString &website);
    void setUsername(const QString &username);
    void setPassword(const QString &password);

private slots:
    void generatePassword();
    void togglePasswordVisibility();
    void validateInput();

private:
    void setupUI();
    void setupConnections();
    QString generateRandomPassword(int length = 16) const;

    QLineEdit *websiteEdit;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QPushButton *generateButton;
    QPushButton *toggleVisibilityButton;
    QPushButton *okButton;
    QPushButton *cancelButton;
    QLabel *statusLabel;

    bool isEditMode;
};

#endif // PASSWORDDIALOG_H 