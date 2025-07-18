#include "passwordmanager.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QTemporaryFile>
#include <QProcess>
#include <QCryptographicHash>
#include <QTextStream>
#include <QFileInfo>

#ifdef Q_OS_WIN
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")
#include <dpapi.h>
#endif

PasswordManager::PasswordManager(Database *db, QObject *parent)
    : QObject(parent)
    , db(db)
{
}

PasswordManager::~PasswordManager()
{
}

bool PasswordManager::importFromChrome()
{
    auto passwords = readChromePasswords();
    return db->importPasswords(passwords);
}

bool PasswordManager::importFromFirefox()
{
    auto passwords = readFirefoxPasswords();
    return db->importPasswords(passwords);
}

bool PasswordManager::importFromEdge()
{
    auto passwords = readEdgePasswords();
    return db->importPasswords(passwords);
}

bool PasswordManager::importFromCsv(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open CSV file:" << filePath;
        return false;
    }

    QTextStream in(&file);
    QList<QPair<QString, QPair<QString, QString>>> passwords;
    
    // CSV başlık satırını oku ve sütun indekslerini belirle
    QString headerLine = in.readLine();
    QStringList headers = parseCSVLine(headerLine);
    
    int nameIndex = headers.indexOf("name");
    int urlIndex = headers.indexOf("url");
    int usernameIndex = headers.indexOf("username");
    int passwordIndex = headers.indexOf("password");
    int noteIndex = headers.indexOf("note");
    
    if (nameIndex < 0 || urlIndex < 0 || usernameIndex < 0 || passwordIndex < 0) {
        qWarning() << "CSV file does not have the required columns (name, url, username, password)";
        return false;
    }
    
    qDebug() << "CSV import: Found columns - name:" << nameIndex << "url:" << urlIndex 
             << "username:" << usernameIndex << "password:" << passwordIndex;
    
    // Satırları oku
    int importedCount = 0;
    int skippedCount = 0;
    bool success = true;
    
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.trimmed().isEmpty()) {
            continue;
        }
        
        // CSV alanlarını ayır
        QStringList fields = parseCSVLine(line);
        
        // Gerekli alanların mevcut olduğundan emin ol
        if (fields.size() <= qMax(qMax(nameIndex, urlIndex), qMax(usernameIndex, passwordIndex))) {
            qWarning() << "Skipping invalid CSV line with insufficient fields";
            continue;
        }
        
        QString name = fields[nameIndex];
        QString url = fields[urlIndex];
        QString username = fields[usernameIndex];
        QString password = fields[passwordIndex];
        QString note = noteIndex >= 0 && noteIndex < fields.size() ? fields[noteIndex] : QString();
        
        // Boş alanları kontrol et ve varsayılan değerler ata
        if (name.isEmpty() && url.isEmpty()) {
            qWarning() << "Skipping entry with empty name and URL";
            continue;
        }
        
        // Boş kullanıcı adı için varsayılan değer
        if (username.isEmpty()) {
            username = "imported_user";
            qDebug() << "Using default username for entry:" << (!url.isEmpty() ? url : name);
        }
        
        // Duplicate kontrolü
        if (passwordExists(url, username)) {
            qDebug() << "Skipping duplicate entry for URL:" << url << "and username:" << username;
            skippedCount++;
            continue;
        }
        
        // Şifreyi doğrudan ekle
        if (db->addPassword(name, url, username, password, note)) {
            importedCount++;
        } else {
            qWarning() << "Failed to add password for URL:" << url;
            success = false;
        }
    }
    
    file.close();
    qDebug() << "CSV import: Successfully imported" << importedCount << "passwords, skipped" << skippedCount << "duplicates";
    
    return success && importedCount > 0;
}

// CSV satırını alanlarına ayırır, tırnak işaretlerini ve virgülleri doğru şekilde ele alır
QStringList PasswordManager::parseCSVLine(const QString &line)
{
    QStringList fields;
    bool inQuotes = false;
    QString field;
    
    for (int i = 0; i < line.length(); i++) {
        QChar c = line.at(i);
        
        if (c == '"') {
            // Tırnak işareti içindeki tırnak işaretlerini kontrol et (örn. "field""with""quotes")
            if (i + 1 < line.length() && line.at(i + 1) == '"') {
                field.append('"');
                i++; // İkinci tırnak işaretini atla
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            // Virgül, tırnak içinde değilse alan sonu
            fields.append(field);
            field.clear();
        } else {
            field.append(c);
        }
    }
    
    // Son alanı ekle
    fields.append(field);
    
    return fields;
}

bool PasswordManager::addPassword(const QString &name, const QString &url, const QString &username, const QString &password, const QString &note)
{
    return db->addPassword(name, url, username, password, note);
}

bool PasswordManager::updatePassword(int id, const QString &name, const QString &url, const QString &username, const QString &password, const QString &note)
{
    return db->updatePassword(id, name, url, username, password, note);
}

bool PasswordManager::deletePassword(int id)
{
    return db->deletePassword(id);
}

QList<QPair<QString, QPair<QString, QString>>> PasswordManager::searchPasswords(const QString &query)
{
    QList<QPair<QString, QPair<QString, QString>>> results;
    QSqlQuery sqlQuery = db->getPasswords(query);
    
    while (sqlQuery.next()) {
        QString name = sqlQuery.value("name").toString();
        QString url = sqlQuery.value("url").toString();
        QString username = sqlQuery.value("username").toString();
        QByteArray encryptedPassword = sqlQuery.value("password").toByteArray();
        
        // Decrypt the password
        QString password = db->decryptPassword(encryptedPassword);
        
        // Use URL as the first element, and username/password as the pair
        results.append({url, {username, password}});
    }
    
    return results;
}

bool PasswordManager::passwordExists(const QString &url, const QString &username)
{
    QSqlQuery sqlQuery = db->getPasswords();
    
    while (sqlQuery.next()) {
        QString storedUrl = sqlQuery.value("url").toString();
        QString storedUsername = sqlQuery.value("username").toString();
        
        if (storedUrl == url && storedUsername == username) {
            return true;
        }
    }
    
    return false;
}

QList<QPair<QString, QPair<QString, QString>>> PasswordManager::readChromePasswords()
{
    QList<QPair<QString, QPair<QString, QString>>> passwords;
    QString dbPath = getChromePasswordFile();
    
    qDebug() << "Looking for Chrome password database at:" << dbPath;
    
    if (dbPath.isEmpty() || !QFile::exists(dbPath)) {
        qWarning() << "Chrome password database not found at:" << dbPath;
        return passwords;
    }
    
    qDebug() << "Chrome password database found. Attempting to read...";
    
#ifdef Q_OS_WIN
    try {
        // Copy the Login Data file (it's locked while Chrome is running)
        QTemporaryFile tempDb;
        if (!tempDb.open()) {
            qWarning() << "Could not create temporary file";
            return passwords;
        }
        
        tempDb.close();
        
        qDebug() << "Copying Chrome database to temporary file:" << tempDb.fileName();
        
        if (!QFile::copy(dbPath, tempDb.fileName())) {
            qWarning() << "Could not copy Chrome database to temporary file";
            return passwords;
        }
        
        // Open the database
        QSqlDatabase chromeSqlDb = QSqlDatabase::addDatabase("QSQLITE", "chrome_import");
        chromeSqlDb.setDatabaseName(tempDb.fileName());
        
        qDebug() << "Opening Chrome database...";
        
        if (!chromeSqlDb.open()) {
            qWarning() << "Could not open Chrome database:" << chromeSqlDb.lastError().text();
            return passwords;
        }
        
        qDebug() << "Getting Chrome encryption key...";
        
        // Get the encryption key from Local State file
        QByteArray encryptionKey = getChromeEncryptionKey();
        if (encryptionKey.isEmpty()) {
            qWarning() << "Could not get Chrome encryption key";
            chromeSqlDb.close();
            return passwords;
        }
        
        qDebug() << "Chrome encryption key obtained. Querying database...";
        
        // Query the database
        QSqlQuery query(chromeSqlDb);
        if (query.exec("SELECT origin_url, username_value, password_value FROM logins")) {
            qDebug() << "Query executed successfully. Processing results...";
            
            int count = 0;
            while (query.next()) {
                QString url = query.value(0).toString();
                QString username = query.value(1).toString();
                QByteArray encryptedPassword = query.value(2).toByteArray();
                
                qDebug() << "Found entry for URL:" << url << "Username:" << username;
                
                // Decrypt the password
                QString password = decryptChromePassword(encryptedPassword, encryptionKey);
                
                if (!url.isEmpty() && !password.isEmpty()) {
                    passwords.append({url, {username, password}});
                    count++;
                } else {
                    qDebug() << "Skipping entry with empty URL or password";
                }
            }
            
            qDebug() << "Successfully read" << count << "passwords from Chrome";
        } else {
            qWarning() << "Failed to query Chrome database:" << query.lastError().text();
        }
        
        chromeSqlDb.close();
        QSqlDatabase::removeDatabase("chrome_import");
    } catch (const std::exception& e) {
        qWarning() << "Exception while reading Chrome passwords:" << e.what();
    }
#else
    qWarning() << "Chrome password import not implemented for this platform";
#endif
    
    return passwords;
}

QList<QPair<QString, QPair<QString, QString>>> PasswordManager::readFirefoxPasswords()
{
    QList<QPair<QString, QPair<QString, QString>>> passwords;
    QString profilePath = getFirefoxProfilePath();
    
    if (profilePath.isEmpty()) {
        qWarning() << "Firefox profile not found";
        return passwords;
    }
    
    // TODO: Implement Firefox password reading
    // This requires:
    // 1. Reading logins.json
    // 2. Using Firefox's key4.db for decryption
    // 3. Implementing NSS (Network Security Services) decryption
    
    return passwords;
}

QList<QPair<QString, QPair<QString, QString>>> PasswordManager::readEdgePasswords()
{
    QList<QPair<QString, QPair<QString, QString>>> passwords;
    QString dbPath = getEdgePasswordFile();
    
    qDebug() << "Looking for Edge password database at:" << dbPath;
    
    if (dbPath.isEmpty() || !QFile::exists(dbPath)) {
        qWarning() << "Edge password database not found at:" << dbPath;
        return passwords;
    }
    
    qDebug() << "Edge password database found. Attempting to read...";
    
#ifdef Q_OS_WIN
    try {
        // Copy the Login Data file (it's locked while Edge is running)
        QTemporaryFile tempDb;
        if (!tempDb.open()) {
            qWarning() << "Could not create temporary file";
            return passwords;
        }
        
        tempDb.close();
        
        qDebug() << "Copying Edge database to temporary file:" << tempDb.fileName();
        
        if (!QFile::copy(dbPath, tempDb.fileName())) {
            qWarning() << "Could not copy Edge database to temporary file";
            return passwords;
        }
        
        // Open the database
        QSqlDatabase edgeSqlDb = QSqlDatabase::addDatabase("QSQLITE", "edge_import");
        edgeSqlDb.setDatabaseName(tempDb.fileName());
        
        qDebug() << "Opening Edge database...";
        
        if (!edgeSqlDb.open()) {
            qWarning() << "Could not open Edge database:" << edgeSqlDb.lastError().text();
            return passwords;
        }
        
        qDebug() << "Getting Edge encryption key...";
        
        // Get the encryption key from Local State file
        QByteArray encryptionKey = getEdgeEncryptionKey();
        if (encryptionKey.isEmpty()) {
            qWarning() << "Could not get Edge encryption key";
            edgeSqlDb.close();
            return passwords;
        }
        
        qDebug() << "Edge encryption key obtained. Querying database...";
        
        // Query the database
        QSqlQuery query(edgeSqlDb);
        if (query.exec("SELECT origin_url, username_value, password_value FROM logins")) {
            qDebug() << "Query executed successfully. Processing results...";
            
            int count = 0;
            while (query.next()) {
                QString url = query.value(0).toString();
                QString username = query.value(1).toString();
                QByteArray encryptedPassword = query.value(2).toByteArray();
                
                qDebug() << "Found entry for URL:" << url << "Username:" << username;
                
                // Decrypt the password (same algorithm as Chrome)
                QString password = decryptChromePassword(encryptedPassword, encryptionKey);
                
                if (!url.isEmpty() && !password.isEmpty()) {
                    passwords.append({url, {username, password}});
                    count++;
                } else {
                    qDebug() << "Skipping entry with empty URL or password";
                }
            }
            
            qDebug() << "Successfully read" << count << "passwords from Edge";
        } else {
            qWarning() << "Failed to query Edge database:" << query.lastError().text();
        }
        
        edgeSqlDb.close();
        QSqlDatabase::removeDatabase("edge_import");
    } catch (const std::exception& e) {
        qWarning() << "Exception while reading Edge passwords:" << e.what();
    }
#else
    qWarning() << "Edge password import not implemented for this platform";
#endif
    
    return passwords;
}

QString PasswordManager::getChromePasswordFile()
{
#ifdef Q_OS_WIN
    // On Windows, the path is %LOCALAPPDATA%\Google\Chrome\User Data\Default\Login Data
    QString localAppData = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    
    // First try the direct path
    QString directPath = QDir(localAppData).filePath("Google/Chrome/User Data/Default/Login Data");
    if (QFile::exists(directPath)) {
        return directPath;
    }
    
    // If that doesn't work, try the legacy path
    QString legacyPath = QDir(localAppData).filePath("../Local/Google/Chrome/User Data/Default/Login Data");
    if (QFile::exists(legacyPath)) {
        return legacyPath;
    }
    
    // Try one more path format
    QString altPath = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).filePath("AppData/Local/Google/Chrome/User Data/Default/Login Data");
    if (QFile::exists(altPath)) {
        return altPath;
    }
    
    // Return the most likely path even if it doesn't exist
    return directPath;
#else
    QString configHome = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    return QDir(configHome).filePath("google-chrome/Default/Login Data");
#endif
}

QString PasswordManager::getFirefoxProfilePath()
{
#ifdef Q_OS_WIN
    QString appData = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    // Remove the "AppData/Local/YourOrganization/Password Manager" part
    appData = QDir::cleanPath(appData + "/../..");
    QString profilesPath = QDir(appData).filePath("Roaming/Mozilla/Firefox/Profiles");
#else
    QString configHome = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString profilesPath = QDir(configHome).filePath("firefox");
#endif

    QDir profilesDir(profilesPath);
    if (!profilesDir.exists()) {
        qWarning() << "Firefox profiles directory not found at:" << profilesPath;
        return QString();
    }
    
    // Find the default profile (ends with .default-release)
    QStringList profiles = profilesDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QString &profile : profiles) {
        if (profile.endsWith(".default-release")) {
            return profilesDir.filePath(profile);
        }
    }
    
    // If no .default-release profile found, try to find any profile
    if (!profiles.isEmpty()) {
        return profilesDir.filePath(profiles.first());
    }
    
    return QString();
}

QString PasswordManager::getEdgePasswordFile()
{
#ifdef Q_OS_WIN
    // On Windows, the path is %LOCALAPPDATA%\Microsoft\Edge\User Data\Default\Login Data
    QString localAppData = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    
    // First try the direct path
    QString directPath = QDir(localAppData).filePath("Microsoft/Edge/User Data/Default/Login Data");
    if (QFile::exists(directPath)) {
        return directPath;
    }
    
    // If that doesn't work, try the legacy path
    QString legacyPath = QDir(localAppData).filePath("../Local/Microsoft/Edge/User Data/Default/Login Data");
    if (QFile::exists(legacyPath)) {
        return legacyPath;
    }
    
    // Try one more path format
    QString altPath = QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).filePath("AppData/Local/Microsoft/Edge/User Data/Default/Login Data");
    if (QFile::exists(altPath)) {
        return altPath;
    }
    
    // Return the most likely path even if it doesn't exist
    return directPath;
#else
    QString configHome = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    return QDir(configHome).filePath("microsoft-edge/Default/Login Data");
#endif
}

#ifdef Q_OS_WIN
QByteArray PasswordManager::getChromeEncryptionKey()
{
    // On Windows, the path is %LOCALAPPDATA%\Google\Chrome\User Data\Local State
    QString localAppData = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    
    // Try multiple possible paths
    QStringList possiblePaths = {
        QDir(localAppData).filePath("Google/Chrome/User Data/Local State"),
        QDir(localAppData).filePath("../Local/Google/Chrome/User Data/Local State"),
        QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).filePath("AppData/Local/Google/Chrome/User Data/Local State")
    };
    
    QString statePath;
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            statePath = path;
            break;
        }
    }
    
    if (statePath.isEmpty()) {
        qWarning() << "Could not find Chrome Local State file. Tried paths:" << possiblePaths;
        return QByteArray();
    }
    
    qDebug() << "Found Chrome Local State file at:" << statePath;
    
    QFile stateFile(statePath);
    if (!stateFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open Chrome Local State file at:" << statePath;
        return QByteArray();
    }
    
    QByteArray data = stateFile.readAll();
    stateFile.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid Chrome Local State file format";
        return QByteArray();
    }
    
    QJsonObject root = doc.object();
    QJsonObject os_crypt = root["os_crypt"].toObject();
    QByteArray encryptedKey = QByteArray::fromBase64(os_crypt["encrypted_key"].toString().toUtf8());
    
    // Remove DPAPI prefix "DPAPI"
    encryptedKey = encryptedKey.mid(5);
    
    DATA_BLOB encryptedBlob, decryptedBlob;
    encryptedBlob.pbData = reinterpret_cast<BYTE*>(encryptedKey.data());
    encryptedBlob.cbData = encryptedKey.size();
    
    if (!CryptUnprotectData(&encryptedBlob, nullptr, nullptr, nullptr, nullptr, 0, &decryptedBlob)) {
        qWarning() << "CryptUnprotectData failed with error code:" << GetLastError();
        return QByteArray();
    }
    
    QByteArray key(reinterpret_cast<char*>(decryptedBlob.pbData), decryptedBlob.cbData);
    LocalFree(decryptedBlob.pbData);
    
    return key;
}

QByteArray PasswordManager::getEdgeEncryptionKey()
{
    // On Windows, the path is %LOCALAPPDATA%\Microsoft\Edge\User Data\Local State
    QString localAppData = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
    
    // Try multiple possible paths
    QStringList possiblePaths = {
        QDir(localAppData).filePath("Microsoft/Edge/User Data/Local State"),
        QDir(localAppData).filePath("../Local/Microsoft/Edge/User Data/Local State"),
        QDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).filePath("AppData/Local/Microsoft/Edge/User Data/Local State")
    };
    
    QString statePath;
    for (const QString &path : possiblePaths) {
        if (QFile::exists(path)) {
            statePath = path;
            break;
        }
    }
    
    if (statePath.isEmpty()) {
        qWarning() << "Could not find Edge Local State file. Tried paths:" << possiblePaths;
        return QByteArray();
    }
    
    qDebug() << "Found Edge Local State file at:" << statePath;
    
    QFile stateFile(statePath);
    if (!stateFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open Edge Local State file at:" << statePath;
        return QByteArray();
    }
    
    QByteArray data = stateFile.readAll();
    stateFile.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid Edge Local State file format";
        return QByteArray();
    }
    
    QJsonObject root = doc.object();
    QJsonObject os_crypt = root["os_crypt"].toObject();
    QByteArray encryptedKey = QByteArray::fromBase64(os_crypt["encrypted_key"].toString().toUtf8());
    
    // Remove DPAPI prefix "DPAPI"
    encryptedKey = encryptedKey.mid(5);
    
    DATA_BLOB encryptedBlob, decryptedBlob;
    encryptedBlob.pbData = reinterpret_cast<BYTE*>(encryptedKey.data());
    encryptedBlob.cbData = encryptedKey.size();
    
    if (!CryptUnprotectData(&encryptedBlob, nullptr, nullptr, nullptr, nullptr, 0, &decryptedBlob)) {
        qWarning() << "CryptUnprotectData failed with error code:" << GetLastError();
        return QByteArray();
    }
    
    QByteArray key(reinterpret_cast<char*>(decryptedBlob.pbData), decryptedBlob.cbData);
    LocalFree(decryptedBlob.pbData);
    
    return key;
}

QString PasswordManager::decryptChromePassword(const QByteArray &encryptedPassword, const QByteArray &key)
{
    if (encryptedPassword.size() <= 3) {
        return QString();
    }
    
    // Check if the password is encrypted with AES-GCM
    if (encryptedPassword.startsWith("v10") || encryptedPassword.startsWith("v11")) {
        // Extract components: version(3) + nonce(12) + ciphertext(variable) + tag(16)
        QByteArray nonce = encryptedPassword.mid(3, 12);
        QByteArray ciphertext = encryptedPassword.mid(15, encryptedPassword.size() - 15 - 16);
        QByteArray tag = encryptedPassword.right(16);
        
        // AES-GCM decryption using OpenSSL
        EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
        if (!ctx) {
            qWarning() << "Failed to create OpenSSL cipher context";
            return QString();
        }
        
        // Initialize the decryption operation
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) {
            qWarning() << "Failed to initialize AES-GCM decryption";
            EVP_CIPHER_CTX_free(ctx);
            return QString();
        }
        
        // Set IV length
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, nonce.size(), nullptr) != 1) {
            qWarning() << "Failed to set IV length";
            EVP_CIPHER_CTX_free(ctx);
            return QString();
        }
        
        // Initialize key and IV
        if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, 
                             reinterpret_cast<const unsigned char*>(key.constData()),
                             reinterpret_cast<const unsigned char*>(nonce.constData())) != 1) {
            qWarning() << "Failed to set key and IV";
            EVP_CIPHER_CTX_free(ctx);
            return QString();
        }
        
        // Set expected tag value
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag.size(),
                              const_cast<char*>(tag.constData())) != 1) {
            qWarning() << "Failed to set authentication tag";
            EVP_CIPHER_CTX_free(ctx);
            return QString();
        }
        
        // Decrypt the ciphertext
        QByteArray plaintext;
        plaintext.resize(ciphertext.size());
        int len = 0;
        
        if (EVP_DecryptUpdate(ctx, 
                           reinterpret_cast<unsigned char*>(plaintext.data()), 
                           &len, 
                           reinterpret_cast<const unsigned char*>(ciphertext.constData()),
                           ciphertext.size()) != 1) {
            qWarning() << "Failed to decrypt data";
            EVP_CIPHER_CTX_free(ctx);
            return QString();
        }
        
        // Finalize the decryption
        int finalLen = 0;
        if (EVP_DecryptFinal_ex(ctx, 
                             reinterpret_cast<unsigned char*>(plaintext.data() + len),
                             &finalLen) != 1) {
            qWarning() << "Failed to finalize decryption";
            EVP_CIPHER_CTX_free(ctx);
            return QString();
        }
        
        EVP_CIPHER_CTX_free(ctx);
        
        return QString::fromUtf8(plaintext.left(len + finalLen));
    } else {
        // Legacy DPAPI encryption
        DATA_BLOB encryptedBlob, decryptedBlob;
        encryptedBlob.pbData = reinterpret_cast<BYTE*>(const_cast<char*>(encryptedPassword.data()));
        encryptedBlob.cbData = encryptedPassword.size();
        
        if (!CryptUnprotectData(&encryptedBlob, nullptr, nullptr, nullptr, nullptr, 0, &decryptedBlob)) {
            qWarning() << "CryptUnprotectData failed with error code:" << GetLastError();
            return QString();
        }
        
        QString password = QString::fromUtf16(reinterpret_cast<const ushort*>(decryptedBlob.pbData), decryptedBlob.cbData / 2);
        LocalFree(decryptedBlob.pbData);
        
        return password;
    }
}
#endif 