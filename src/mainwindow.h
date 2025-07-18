#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QTimer>
#include <QClipboard>
#include "database.h"
#include "passwordmanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Database *db, PasswordManager *passwordManager, QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addPassword();
    void deletePassword();
    void editPassword();
    void searchPasswords();
    void importFromBrowsers();
    void importFromCsv(); // CSV dosyasından içe aktarma için yeni slot
    void refreshPasswordList();
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void showHideWindow();
    void checkClipboardForLoginForms(); // Pano kontrolü için yeni slot
    void autofillCredentials(const QString &url, const QString &username, const QString &password); // Otomatik doldurma için yeni slot

private:
    void setupUI();
    void createMenuBar();
    void createToolBar();
    void setupConnections();
    void setupTrayIcon();
    void positionWindowAtBottomRight();
    void setupAutofillMonitor(); // Otomatik doldurma izleyicisi kurulumu

    QTableWidget *passwordTable;
    QLineEdit *searchBox;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *editButton;
    QPushButton *importButton;
    QPushButton *importCsvButton; // CSV içe aktarma butonu
    
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *showHideAction;
    QAction *quitAction;
    
    Database *db;
    PasswordManager *passwordManager;
    
    QTimer *clipboardMonitorTimer; // Pano izleme zamanlayıcısı
    QString lastClipboardText; // Son pano metni
};

#endif // MAINWINDOW_H 