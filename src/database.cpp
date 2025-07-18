#include "database.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSqlDriver>
#include <QFile>

// DEBUG_RESET_DB tanımını kaldırıyoruz
// #define DEBUG_RESET_DB

const QString Database::DATABASE_NAME = "passwords.db";

Database::Database(QObject *parent)
    : QObject(parent)
    , currentUserId(-1)
{
    QString dbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dbPath);
    
    QString fullDbPath = dbPath + "/" + DATABASE_NAME;
    
    // Check if we need to delete the existing database due to schema changes
    bool needsReset = false;
    
#ifdef DEBUG_RESET_DB
    // For debugging purposes, we'll delete the database if it exists
    if (QFile::exists(fullDbPath)) {
        qDebug() << "Existing database found at:" << fullDbPath;
        needsReset = true;
    }
    
    if (needsReset) {
        qDebug() << "Removing existing database to ensure schema compatibility";
        QFile::remove(fullDbPath);
    }
#endif
    
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(fullDbPath);
    
    if (!initialize()) {
        qCritical() << "Failed to initialize database:" << db.lastError().text();
    }
}

Database::~Database()
{
    if (db.isOpen()) {
        db.close();
    }
}

bool Database::initialize()
{
    if (!db.open()) {
        qCritical() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    
    // Enable foreign keys
    QSqlQuery query;
    query.exec("PRAGMA foreign_keys = ON");
    
    return upgradeDatabase() && createTables() && initializeEncryption();
}

bool Database::upgradeDatabase()
{
    QSqlQuery query;
    
    // Check if the passwords table exists
    query.exec("SELECT sql FROM sqlite_master WHERE type='table' AND name='passwords'");
    
    if (query.next()) {
        QString tableSchema = query.value(0).toString();
        
        // Check if the table has the name and note columns
        if (!tableSchema.contains("name TEXT") || !tableSchema.contains("note TEXT")) {
            qDebug() << "Upgrading database schema...";
            
            // Start a transaction
            if (!db.transaction()) {
                qWarning() << "Failed to start transaction for database upgrade:" << db.lastError().text();
                return false;
            }
            
            // Create a backup of the old table
            if (!query.exec("CREATE TABLE passwords_backup AS SELECT * FROM passwords")) {
                qWarning() << "Failed to create backup table:" << query.lastError().text();
                db.rollback();
                return false;
            }
            
            // Drop the old table
            if (!query.exec("DROP TABLE passwords")) {
                qWarning() << "Failed to drop old table:" << query.lastError().text();
                db.rollback();
                return false;
            }
            
            // Create the new table with updated schema
            if (!query.exec("CREATE TABLE passwords ("
                          "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                          "user_id INTEGER NOT NULL,"
                          "name TEXT NOT NULL,"
                          "url TEXT NOT NULL,"
                          "username TEXT NOT NULL,"
                          "password BLOB NOT NULL,"
                          "note TEXT,"
                          "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                          "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                          "FOREIGN KEY (user_id) REFERENCES users(id))")) {
                qWarning() << "Failed to create new passwords table:" << query.lastError().text();
                db.rollback();
                return false;
            }
            
            // Copy data from backup to new table
            if (!query.exec("INSERT INTO passwords (id, user_id, name, url, username, password, created_at, updated_at) "
                          "SELECT id, user_id, url, url, username, password, created_at, updated_at FROM passwords_backup")) {
                qWarning() << "Failed to copy data from backup:" << query.lastError().text();
                db.rollback();
                return false;
            }
            
            // Drop the backup table
            if (!query.exec("DROP TABLE passwords_backup")) {
                qWarning() << "Failed to drop backup table:" << query.lastError().text();
                db.rollback();
                return false;
            }
            
            // Create index on user_id
            if (!query.exec("CREATE INDEX IF NOT EXISTS idx_passwords_user_id ON passwords(user_id)")) {
                qWarning() << "Failed to create index on user_id:" << query.lastError().text();
                db.rollback();
                return false;
            }
            
            // Commit the transaction
            if (!db.commit()) {
                qWarning() << "Failed to commit transaction for database upgrade:" << db.lastError().text();
                db.rollback();
                return false;
            }
            
            qDebug() << "Database schema upgrade completed successfully";
        }
    }
    
    return true;
}

bool Database::initializeEncryption()
{
    // Initialize OpenSSL
    OpenSSL_add_all_algorithms();
    return true;
}

bool Database::createTables()
{
    QSqlQuery query;
    
    // Users table
    if (!query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "username TEXT UNIQUE NOT NULL,"
                   "password TEXT NOT NULL,"
                   "salt BLOB NOT NULL,"
                   "created_at DATETIME DEFAULT CURRENT_TIMESTAMP)")) {
        qCritical() << "Failed to create users table:" << query.lastError().text();
        return false;
    }
    
    // Passwords table - simplified schema
    if (!query.exec("CREATE TABLE IF NOT EXISTS passwords ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "user_id INTEGER NOT NULL,"
                   "name TEXT NOT NULL,"
                   "url TEXT NOT NULL,"
                   "username TEXT NOT NULL,"
                   "password BLOB NOT NULL,"
                   "note TEXT,"
                   "created_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                   "updated_at DATETIME DEFAULT CURRENT_TIMESTAMP,"
                   "FOREIGN KEY (user_id) REFERENCES users(id))")) {
        qCritical() << "Failed to create passwords table:" << query.lastError().text();
        return false;
    }
    
    // Create index on user_id
    if (!query.exec("CREATE INDEX IF NOT EXISTS idx_passwords_user_id ON passwords(user_id)")) {
        qCritical() << "Failed to create index on user_id:" << query.lastError().text();
        return false;
    }
    
    return true;
}

bool Database::createUser(const QString &username, const QString &password)
{
    qDebug() << "Creating user:" << username;
    
    // Generate a random salt
    QByteArray salt = QByteArray(SALT_SIZE, 0);
    if (RAND_bytes(reinterpret_cast<unsigned char*>(salt.data()), SALT_SIZE) != 1) {
        qWarning() << "Failed to generate salt";
        return false;
    }
    
    qDebug() << "Generated salt of size:" << salt.size();
    
    // Calculate password hash
    QString passwordHash = hashPassword(password + salt.toHex());
    qDebug() << "Generated password hash:" << passwordHash.left(10) + "...";
    
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, salt) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(passwordHash);
    query.addBindValue(salt);
    
    if (!query.exec()) {
        qWarning() << "Failed to create user:" << query.lastError().text();
        return false;
    }
    
    // Set the current user ID
    currentUserId = query.lastInsertId().toInt();
    currentUsername = username;
    
    qDebug() << "User created with ID:" << currentUserId;
    
    // Set the master key
    if (!setMasterKey(password)) {
        qWarning() << "Failed to set master key for new user";
        return false;
    }
    
    qDebug() << "User creation complete, master key set";
    return true;
}

bool Database::validateUser(const QString &username, const QString &password)
{
    qDebug() << "Validating user:" << username;
    
    QSqlQuery query;
    query.prepare("SELECT id, password, salt FROM users WHERE username = ?");
    query.addBindValue(username);
    
    if (!query.exec()) {
        qWarning() << "Failed to execute user validation query:" << query.lastError().text();
        return false;
    }
    
    if (!query.next()) {
        qWarning() << "User not found:" << username;
        return false;
    }
    
    int userId = query.value(0).toInt();
    QString storedHash = query.value(1).toString();
    QByteArray salt = query.value(2).toByteArray();
    
    qDebug() << "Found user with ID:" << userId;
    qDebug() << "Salt size:" << salt.size();
    
    // Calculate hash with the same method as when creating the user
    QString calculatedHash = hashPassword(password + salt.toHex());
    
    qDebug() << "Stored hash:" << storedHash.left(10) + "...";
    qDebug() << "Calculated hash:" << calculatedHash.left(10) + "...";
    
    bool valid = (storedHash == calculatedHash);
    
    if (valid) {
        qDebug() << "User validation successful";
        currentUserId = userId;
        currentUsername = username;
        
        // Set the master key
        if (!setMasterKey(password)) {
            qWarning() << "Failed to set master key";
            return false;
        }
    } else {
        qWarning() << "Password validation failed for user:" << username;
    }
    
    return valid;
}

bool Database::setMasterKey(const QString &password)
{
    qDebug() << "Setting master key for password";
    
    // Store the password directly as the master key
    // This is simpler and more reliable for now
    masterKey = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    
    qDebug() << "Master key set, size:" << masterKey.size();
    return true;
}

QByteArray Database::generateIV()
{
    QByteArray iv(IV_SIZE, 0);
    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), IV_SIZE);
    return iv;
}

QByteArray Database::encryptPassword(const QString &password)
{
    if (masterKey.isEmpty()) {
        qWarning() << "Master key not set";
        return QByteArray();
    }
    
    QByteArray iv = generateIV();
    QByteArray plaintext = password.toUtf8();
    QByteArray ciphertext;
    
    // Create and initialize the context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return QByteArray();
    }
    
    // Initialize the encryption operation
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                          reinterpret_cast<const unsigned char*>(masterKey.constData()),
                          reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    
    // Encrypt the plaintext
    ciphertext.resize(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len;
    if (EVP_EncryptUpdate(ctx,
                         reinterpret_cast<unsigned char*>(ciphertext.data()),
                         &len,
                         reinterpret_cast<const unsigned char*>(plaintext.constData()),
                         plaintext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    
    // Finalize the encryption
    int finalLen;
    if (EVP_EncryptFinal_ex(ctx,
                           reinterpret_cast<unsigned char*>(ciphertext.data() + len),
                           &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    
    // Get the tag
    unsigned char tag[16];
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QByteArray();
    }
    
    EVP_CIPHER_CTX_free(ctx);
    
    // Combine IV + Ciphertext + Tag
    QByteArray result;
    result.append(iv);
    result.append(ciphertext.left(len + finalLen));
    result.append(reinterpret_cast<char*>(tag), 16);
    
    return result;
}

QString Database::decryptPassword(const QByteArray &encryptedData)
{
    if (masterKey.isEmpty() || encryptedData.size() < IV_SIZE + 16) {
        return QString();
    }
    
    // Extract IV, ciphertext and tag
    QByteArray iv = encryptedData.left(IV_SIZE);
    QByteArray tag = encryptedData.right(16);
    QByteArray ciphertext = encryptedData.mid(IV_SIZE, encryptedData.size() - IV_SIZE - 16);
    
    // Create and initialize the context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        return QString();
    }
    
    // Initialize the decryption operation
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr,
                          reinterpret_cast<const unsigned char*>(masterKey.constData()),
                          reinterpret_cast<const unsigned char*>(iv.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QString();
    }
    
    // Set the tag
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16,
                           const_cast<char*>(tag.constData())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QString();
    }
    
    // Decrypt the ciphertext
    QByteArray plaintext;
    plaintext.resize(ciphertext.size());
    int len;
    if (EVP_DecryptUpdate(ctx,
                         reinterpret_cast<unsigned char*>(plaintext.data()),
                         &len,
                         reinterpret_cast<const unsigned char*>(ciphertext.constData()),
                         ciphertext.size()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QString();
    }
    
    // Finalize the decryption
    int finalLen;
    if (EVP_DecryptFinal_ex(ctx,
                           reinterpret_cast<unsigned char*>(plaintext.data() + len),
                           &finalLen) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        return QString();
    }
    
    EVP_CIPHER_CTX_free(ctx);
    
    return QString::fromUtf8(plaintext.left(len + finalLen));
}

QString Database::hashPassword(const QString &password)
{
    qDebug() << "Hashing password of length:" << password.length();
    
    // Use SHA-256 for consistent hashing
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
    );
    
    QString hexHash = hash.toHex();
    qDebug() << "Generated hash of length:" << hexHash.length();
    
    return hexHash;
}

bool Database::addPassword(const QString &name, const QString &url, const QString &username, const QString &password, const QString &note)
{
    if (currentUserId <= 0) {
        qWarning() << "No user is logged in";
        return false;
    }
    
    qDebug() << "Adding password for name:" << name << "url:" << url << "username:" << username << "user_id:" << currentUserId;
    
    QByteArray encryptedData = encryptPassword(password);
    if (encryptedData.isEmpty()) {
        qWarning() << "Failed to encrypt password";
        return false;
    }
    
    qDebug() << "Password encrypted successfully. Encrypted data size:" << encryptedData.size();
    
    QSqlQuery query;
    query.prepare("INSERT INTO passwords (user_id, name, url, username, password, note) VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(currentUserId);
    query.addBindValue(name);
    query.addBindValue(url);
    query.addBindValue(username);
    query.addBindValue(encryptedData);
    query.addBindValue(note);
    
    if (!query.exec()) {
        qWarning() << "Failed to add password. SQL error:" << query.lastError().text();
        qDebug() << "SQL driver error code:" << query.lastError().nativeErrorCode();
        qDebug() << "SQL query:" << query.lastQuery();
        return false;
    }
    
    qDebug() << "Password added successfully for name:" << name << "url:" << url;
    return true;
}

bool Database::updatePassword(int id, const QString &name, const QString &url, const QString &username, const QString &password, const QString &note)
{
    if (currentUserId <= 0) {
        qWarning() << "No user is logged in";
        return false;
    }
    
    QByteArray encryptedData = encryptPassword(password);
    if (encryptedData.isEmpty()) {
        qWarning() << "Failed to encrypt password";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("UPDATE passwords SET name = ?, url = ?, username = ?, password = ?, note = ?, updated_at = CURRENT_TIMESTAMP "
                 "WHERE id = ? AND user_id = ?");
    query.addBindValue(name);
    query.addBindValue(url);
    query.addBindValue(username);
    query.addBindValue(encryptedData);
    query.addBindValue(note);
    query.addBindValue(id);
    query.addBindValue(currentUserId);
    
    if (!query.exec()) {
        qWarning() << "Failed to update password:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

bool Database::deletePassword(int id)
{
    if (currentUserId <= 0) {
        qWarning() << "No user is logged in";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("DELETE FROM passwords WHERE id = ? AND user_id = ?");
    query.addBindValue(id);
    query.addBindValue(currentUserId);
    
    if (!query.exec()) {
        qWarning() << "Failed to delete password:" << query.lastError().text();
        return false;
    }
    
    return query.numRowsAffected() > 0;
}

QSqlQuery Database::getPasswords(const QString &search)
{
    QSqlQuery query;
    
    if (currentUserId <= 0) {
        qWarning() << "No user is logged in";
        return query;
    }
    
    if (search.isEmpty()) {
        query.prepare("SELECT id, name, url, username, password, note FROM passwords "
                     "WHERE user_id = ? "
                     "ORDER BY name ASC");
        query.addBindValue(currentUserId);
    } else {
        query.prepare("SELECT id, name, url, username, password, note FROM passwords "
                     "WHERE user_id = ? AND (name LIKE ? OR url LIKE ? OR username LIKE ?) "
                     "ORDER BY name ASC");
        query.addBindValue(currentUserId);
        query.addBindValue("%" + search + "%");
        query.addBindValue("%" + search + "%");
        query.addBindValue("%" + search + "%");
    }
    
    if (!query.exec()) {
        qWarning() << "Failed to get passwords:" << query.lastError().text();
    }
    
    return query;
}

bool Database::importPasswords(const QList<QPair<QString, QPair<QString, QString>>> &passwords)
{
    if (currentUserId <= 0) {
        qWarning() << "No user is logged in";
        return false;
    }
    
    db.transaction();
    
    for (const auto &entry : passwords) {
        // Use the URL as name, and empty note
        if (!addPassword(entry.first, entry.first, entry.second.first, entry.second.second, QString())) {
            db.rollback();
            return false;
        }
    }
    
    return db.commit();
} 