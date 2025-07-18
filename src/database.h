#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCryptographicHash>
#include <QDateTime>
#include <QByteArray>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database();

    bool initialize();
    bool createTables();
    
    // User management
    bool createUser(const QString &username, const QString &password);
    bool validateUser(const QString &username, const QString &password);
    bool setMasterKey(const QString &password);
    int getCurrentUserId() const { return currentUserId; }
    
    // Password management
    bool addPassword(const QString &name, const QString &url, const QString &username, const QString &password, const QString &note = QString());
    bool updatePassword(int id, const QString &name, const QString &url, const QString &username, const QString &password, const QString &note = QString());
    bool deletePassword(int id);
    QSqlQuery getPasswords(const QString &search = QString());
    
    // Browser import
    bool importPasswords(const QList<QPair<QString, QPair<QString, QString>>> &passwords);
    
    // Encryption/Decryption
    QByteArray encryptPassword(const QString &password);
    QString decryptPassword(const QByteArray &encryptedPassword);

private:
    QString hashPassword(const QString &password);
    bool initializeEncryption();
    QByteArray generateIV();
    bool upgradeDatabase();
    
    QSqlDatabase db;
    static const QString DATABASE_NAME;
    
    // Encryption related members
    QByteArray masterKey;
    static const int KEY_SIZE = 32; // 256 bits
    static const int IV_SIZE = 16;  // 128 bits
    static const int SALT_SIZE = 32;
    
    // Current user info
    int currentUserId;
    QString currentUsername;
};

#endif // DATABASE_H 