#include <QApplication>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include "mainwindow.h"
#include "loginwindow.h"
#include "database.h"
#include "passwordmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Check if system tray is available
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(nullptr, "Password Manager",
                            "System tray is not available on this system.");
        return 1;
    }
    
    // Allow the application to run in the background when all windows are closed
    QApplication::setQuitOnLastWindowClosed(false);
    
    // Set application information
    QApplication::setApplicationName("Password Manager");
    QApplication::setApplicationVersion("1.0.0");
    QApplication::setOrganizationName("YourOrganization");
    QApplication::setOrganizationDomain("yourorganization.com");
    
    // Initialize database
    Database db;
    
    // Initialize password manager
    PasswordManager passwordManager(&db);
    
    // Show login window
    LoginWindow loginWindow(&db);
    if (loginWindow.exec() != QDialog::Accepted) {
        return 0;
    }
    
    // Show main window
    MainWindow mainWindow(&db, &passwordManager);
    mainWindow.show();
    
    return app.exec();
} 