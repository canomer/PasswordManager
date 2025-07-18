#ifndef PASSWORDMANAGER_H
#define PASSWORDMANAGER_H

#include <QObject>
#include <QList>
#include <QPair>
#include "database.h"

class PasswordManager : public QObject
{
    Q_OBJECT

public:
    explicit PasswordManager(Database *db, QObject *parent = nullptr);
    ~PasswordManager();

    // Browser password import
    bool importFromChrome();
    bool importFromFirefox();
    bool importFromEdge();
    bool importFromCsv(const QString &filePath); // CSV dosyasından içe aktarma için yeni metot

    // Password operations
    bool addPassword(const QString &name, const QString &url, const QString &username, const QString &password, const QString &note = QString());
    bool updatePassword(int id, const QString &name, const QString &url, const QString &username, const QString &password, const QString &note = QString());
    bool deletePassword(int id);
    QList<QPair<QString, QPair<QString, QString>>> searchPasswords(const QString &query = QString());

    // Check if password already exists
    bool passwordExists(const QString &url, const QString &username);

private:
    Database *db;
    
    // Browser-specific import helpers
    QList<QPair<QString, QPair<QString, QString>>> readChromePasswords();
    QList<QPair<QString, QPair<QString, QString>>> readFirefoxPasswords();
    QList<QPair<QString, QPair<QString, QString>>> readEdgePasswords();
    
    // CSV parsing helper
    QStringList parseCSVLine(const QString &line);
    
    // Platform-specific paths
    QString getChromePasswordFile();
    QString getFirefoxProfilePath();
    QString getEdgePasswordFile();
    
#ifdef Q_OS_WIN
    // Windows-specific decryption methods
    QByteArray getChromeEncryptionKey();
    QByteArray getEdgeEncryptionKey();
    QString decryptChromePassword(const QByteArray &encryptedPassword, const QByteArray &key);
#endif
};

#endif // PASSWORDMANAGER_H 