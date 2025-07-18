#include "mainwindow.h"
#include "passworddialog.h"
#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QHeaderView>
#include <QScreen>
#include <QApplication>
#include <QStyle>
#include <QFileDialog>
#include <QDir>
#include <QUrl>
#include <QTimer>
#include <QSqlQuery>
#include <QGuiApplication>
#include <QClipboard>

MainWindow::MainWindow(Database *db, PasswordManager *passwordManager, QWidget *parent)
    : QMainWindow(parent)
    , db(db)
    , passwordManager(passwordManager)
{
    setupUI();
    createMenuBar();
    createToolBar();
    setupConnections();
    setupTrayIcon();
    setupAutofillMonitor();
    positionWindowAtBottomRight();
    
    setWindowTitle(tr("Password Manager"));
    setWindowIcon(QIcon(style()->standardIcon(QStyle::SP_DriveFDIcon)));
    setMinimumSize(800, 600);
    
    refreshPasswordList();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::setupUI()
{
    // Set dark theme
    QString darkStyle = 
        "QWidget { background-color: #1E1E1E; color: #FFFFFF; }"
        "QMainWindow { background-color: #1E1E1E; }"
        "QDialog { background-color: #1E1E1E; }"
        "QMenuBar { background-color: #2D2D2D; color: #FFFFFF; }"
        "QMenuBar::item { background-color: #2D2D2D; color: #FFFFFF; }"
        "QMenuBar::item:selected { background-color: #3E3E3E; }"
        "QMenu { background-color: #2D2D2D; color: #FFFFFF; border: 1px solid #3E3E3E; }"
        "QMenu::item:selected { background-color: #3E3E3E; }"
        "QToolBar { background-color: #2D2D2D; border: none; }"
        "QStatusBar { background-color: #2D2D2D; color: #FFFFFF; }"
        "QPushButton { background-color: #0078D7; color: white; border: none; padding: 5px 10px; border-radius: 2px; }"
        "QPushButton:hover { background-color: #1C97EA; }"
        "QPushButton:pressed { background-color: #00559B; }"
        "QLineEdit { background-color: #333333; color: #FFFFFF; border: 1px solid #3E3E3E; padding: 2px; border-radius: 2px; }"
        "QTableWidget { background-color: #252526; color: #FFFFFF; gridline-color: #3E3E3E; }"
        "QHeaderView::section { background-color: #2D2D2D; color: #FFFFFF; padding: 4px; border: 1px solid #3E3E3E; }"
        "QTableWidget::item:selected { background-color: #0078D7; }";
    
    qApp->setStyleSheet(darkStyle);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Search box
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchBox = new QLineEdit(this);
    searchBox->setPlaceholderText(tr("Search passwords..."));
    searchLayout->addWidget(searchBox);
    
    // Password table
    passwordTable = new QTableWidget(this);
    passwordTable->setColumnCount(5);
    passwordTable->setHorizontalHeaderLabels({tr("name"), tr("url"), tr("username"), tr("password"), tr("note")});
    passwordTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    passwordTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    passwordTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addButton = new QPushButton(tr("Add"), this);
    deleteButton = new QPushButton(tr("Delete"), this);
    editButton = new QPushButton(tr("Edit"), this);
    importCsvButton = new QPushButton(tr("Import from CSV"), this);
    
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(importCsvButton);
    
    mainLayout->addLayout(searchLayout);
    mainLayout->addWidget(passwordTable);
    mainLayout->addLayout(buttonLayout);
    
    setCentralWidget(centralWidget);
}

void MainWindow::createMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Import from CSV"), this, &MainWindow::importFromCsv);
    fileMenu->addSeparator();
    
    // Replace Exit with Hide to Tray
    fileMenu->addAction(tr("&Hide to Tray"), this, &QWidget::hide);
    fileMenu->addAction(tr("&Exit"), qApp, &QApplication::quit);
    
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Add Password"), this, &MainWindow::addPassword);
    editMenu->addAction(tr("&Delete Password"), this, &MainWindow::deletePassword);
    editMenu->addAction(tr("&Edit Password"), this, &MainWindow::editPassword);
}

void MainWindow::createToolBar()
{
    QToolBar *toolBar = addToolBar(tr("Main Toolbar"));
    toolBar->addAction(tr("Add"), this, &MainWindow::addPassword);
    toolBar->addAction(tr("Delete"), this, &MainWindow::deletePassword);
    toolBar->addAction(tr("Edit"), this, &MainWindow::editPassword);
    toolBar->addSeparator();
    toolBar->addAction(tr("Import from CSV"), this, &MainWindow::importFromCsv);
}

void MainWindow::setupConnections()
{
    connect(addButton, &QPushButton::clicked, this, &MainWindow::addPassword);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deletePassword);
    connect(editButton, &QPushButton::clicked, this, &MainWindow::editPassword);
    connect(importCsvButton, &QPushButton::clicked, this, &MainWindow::importFromCsv);
    connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::searchPasswords);
}

void MainWindow::setupTrayIcon()
{
    // Create tray icon
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(style()->standardIcon(QStyle::SP_DriveFDIcon)));
    
    // Create tray menu
    trayIconMenu = new QMenu(this);
    
    showHideAction = new QAction(tr("Show"), this);
    connect(showHideAction, &QAction::triggered, this, &MainWindow::showHideWindow);
    
    quitAction = new QAction(tr("Exit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    
    trayIconMenu->addAction(showHideAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayIconActivated);
    
    trayIcon->show();
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        showHideWindow();
    }
}

void MainWindow::showHideWindow()
{
    if (isVisible()) {
        hide();
        showHideAction->setText(tr("Show"));
    } else {
        show();
        positionWindowAtBottomRight();
        raise();
        activateWindow();
        showHideAction->setText(tr("Hide"));
    }
}

void MainWindow::positionWindowAtBottomRight()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    
    int x = screenGeometry.width() - width();
    int y = screenGeometry.height() - height();
    
    move(x, y);
}

void MainWindow::addPassword()
{
    PasswordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString website = dialog.getWebsite();
        QString username = dialog.getUsername();
        QString password = dialog.getPassword();
        
        // Extract name and URL from website field
        QString name = website;
        QString url = website;
        
        // If website contains a URL, separate name and URL
        if (website.contains("://")) {
            QUrl websiteUrl(website);
            name = websiteUrl.host();
            if (name.isEmpty()) {
                name = website;
            } else if (name.startsWith("www.")) {
                name = name.mid(4);
            }
        }
        
        if (passwordManager->addPassword(name, url, username, password)) {
            refreshPasswordList();
            statusBar()->showMessage(tr("Password added successfully"), 3000);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to add password"));
        }
    }
}

void MainWindow::deletePassword()
{
    QModelIndexList selection = passwordTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a password to delete"));
        return;
    }
    
    int row = selection.first().row();
    int id = passwordTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString name = passwordTable->item(row, 0)->text();
    QString url = passwordTable->item(row, 1)->text();
    
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Confirm Delete"),
        tr("Are you sure you want to delete the password for %1 (%2)?").arg(name).arg(url),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        if (passwordManager->deletePassword(id)) {
            refreshPasswordList();
            statusBar()->showMessage(tr("Password deleted successfully"), 3000);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to delete password"));
        }
    }
}

void MainWindow::editPassword()
{
    QModelIndexList selection = passwordTable->selectionModel()->selectedRows();
    if (selection.isEmpty()) {
        QMessageBox::warning(this, tr("Warning"), tr("Please select a password to edit"));
        return;
    }
    
    int row = selection.first().row();
    int id = passwordTable->item(row, 0)->data(Qt::UserRole).toInt();
    QString name = passwordTable->item(row, 0)->text();
    QString url = passwordTable->item(row, 1)->text();
    QString username = passwordTable->item(row, 2)->text();
    QString password = passwordTable->item(row, 3)->text();
    QString note = passwordTable->item(row, 4)->text();
    
    PasswordDialog dialog(this, true);
    dialog.setWebsite(url);
    dialog.setUsername(username);
    dialog.setPassword(password);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString newWebsite = dialog.getWebsite();
        QString newUsername = dialog.getUsername();
        QString newPassword = dialog.getPassword();
        
        // Extract name and URL from website field
        QString newName = newWebsite;
        QString newUrl = newWebsite;
        
        // If website contains a URL, separate name and URL
        if (newWebsite.contains("://")) {
            QUrl websiteUrl(newWebsite);
            newName = websiteUrl.host();
            if (newName.isEmpty()) {
                newName = newWebsite;
            } else if (newName.startsWith("www.")) {
                newName = newName.mid(4);
            }
        }
        
        if (passwordManager->updatePassword(id, newName, newUrl, newUsername, newPassword, note)) {
            refreshPasswordList();
            statusBar()->showMessage(tr("Password updated successfully"), 3000);
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Failed to update password"));
        }
    }
}

void MainWindow::searchPasswords()
{
    QString searchText = searchBox->text();
    QSqlQuery query = db->getPasswords(searchText);
    
    passwordTable->setRowCount(0);
    
    while (query.next()) {
        int id = query.value("id").toInt();
        QString name = query.value("name").toString();
        QString url = query.value("url").toString();
        QString username = query.value("username").toString();
        QByteArray encryptedPassword = query.value("password").toByteArray();
        QString note = query.value("note").toString();
        
        // Decrypt the password
        QString password = db->decryptPassword(encryptedPassword);
        
        int row = passwordTable->rowCount();
        passwordTable->insertRow(row);
        
        QTableWidgetItem *nameItem = new QTableWidgetItem(name);
        QTableWidgetItem *urlItem = new QTableWidgetItem(url);
        QTableWidgetItem *usernameItem = new QTableWidgetItem(username);
        QTableWidgetItem *passwordItem = new QTableWidgetItem(password);
        QTableWidgetItem *noteItem = new QTableWidgetItem(note);
        
        // Store the password ID for later use
        nameItem->setData(Qt::UserRole, id);
        
        passwordTable->setItem(row, 0, nameItem);
        passwordTable->setItem(row, 1, urlItem);
        passwordTable->setItem(row, 2, usernameItem);
        passwordTable->setItem(row, 3, passwordItem);
        passwordTable->setItem(row, 4, noteItem);
    }
}

void MainWindow::importFromBrowsers()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        tr("Import Passwords"),
        tr("Do you want to import passwords from installed browsers?\nThis may take a few moments."),
        QMessageBox::Yes | QMessageBox::No
    );
    
    if (reply == QMessageBox::Yes) {
        bool imported = false;
        
        if (passwordManager->importFromChrome()) {
            imported = true;
            statusBar()->showMessage(tr("Passwords imported from Chrome"), 3000);
        }
        
        if (passwordManager->importFromFirefox()) {
            imported = true;
            statusBar()->showMessage(tr("Passwords imported from Firefox"), 3000);
        }
        
        if (passwordManager->importFromEdge()) {
            imported = true;
            statusBar()->showMessage(tr("Passwords imported from Edge"), 3000);
        }
        
        if (imported) {
            refreshPasswordList();
        } else {
            QMessageBox::warning(this, tr("Import Failed"),
                               tr("Failed to import passwords from browsers.\nMake sure browsers are installed and passwords are saved."));
        }
    }
}

void MainWindow::importFromCsv()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Import Passwords from CSV"),
        QDir::homePath(),
        tr("CSV Files (*.csv);;All Files (*)")
    );
    
    if (filePath.isEmpty()) {
        return;
    }
    
    if (passwordManager->importFromCsv(filePath)) {
        refreshPasswordList();
        statusBar()->showMessage(tr("Passwords imported from CSV successfully"), 3000);
    } else {
        QMessageBox::warning(this, tr("Import Failed"),
                           tr("Failed to import passwords from CSV file.\nMake sure the file format is correct."));
    }
}

void MainWindow::refreshPasswordList()
{
    searchBox->clear();
    searchPasswords();
} 

void MainWindow::setupAutofillMonitor()
{
    clipboardMonitorTimer = new QTimer(this);
    connect(clipboardMonitorTimer, &QTimer::timeout, this, &MainWindow::checkClipboardForLoginForms);
    clipboardMonitorTimer->start(1000); // Check every second
    lastClipboardText = QApplication::clipboard()->text();
}

void MainWindow::checkClipboardForLoginForms()
{
    QString clipboardText = QApplication::clipboard()->text();
    
    // If clipboard hasn't changed, do nothing
    if (clipboardText == lastClipboardText) {
        return;
    }
    
    lastClipboardText = clipboardText;
    
    // Check if the clipboard contains a URL
    if (clipboardText.contains("://") && (clipboardText.startsWith("http://") || clipboardText.startsWith("https://"))) {
        QUrl url(clipboardText);
        QString urlString = url.toString();
        
        // Check if we have credentials for this URL
        QSqlQuery query = db->getPasswords();
        QList<QPair<QString, QPair<QString, QString>>> matchingCredentials;
        
        while (query.next()) {
            QString storedUrl = query.value("url").toString();
            QString username = query.value("username").toString();
            QByteArray encryptedPassword = query.value("password").toByteArray();
            
            // Check if the URL matches
            if (urlString.contains(storedUrl, Qt::CaseInsensitive) || 
                storedUrl.contains(url.host(), Qt::CaseInsensitive)) {
                
                // Decrypt the password
                QString password = db->decryptPassword(encryptedPassword);
                matchingCredentials.append({storedUrl, {username, password}});
            }
        }
        
        // If we have matching credentials, ask the user if they want to autofill
        if (!matchingCredentials.isEmpty()) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Autofill Credentials"));
            msgBox.setText(tr("Would you like to autofill credentials for %1?").arg(url.host()));
            msgBox.setIcon(QMessageBox::Question);
            
            QPushButton *cancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
            
            QList<QPushButton*> credentialButtons;
            for (const auto &cred : matchingCredentials) {
                QString buttonText = tr("Use %1").arg(cred.second.first);
                QPushButton *credButton = msgBox.addButton(buttonText, QMessageBox::AcceptRole);
                credentialButtons.append(credButton);
            }
            
            msgBox.exec();
            
            // Find which button was clicked
            QAbstractButton *clickedButton = msgBox.clickedButton();
            if (clickedButton != cancelButton) {
                int index = credentialButtons.indexOf(qobject_cast<QPushButton*>(clickedButton));
                if (index >= 0 && index < matchingCredentials.size()) {
                    const auto &selectedCred = matchingCredentials[index];
                    autofillCredentials(urlString, selectedCred.second.first, selectedCred.second.second);
                }
            }
        }
    }
}

void MainWindow::autofillCredentials(const QString &url, const QString &username, const QString &password)
{
    // Copy username to clipboard
    QApplication::clipboard()->setText(username);
    
    // Show notification
    QMessageBox::information(
        this,
        tr("Autofill"),
        tr("Username '%1' has been copied to clipboard.\n\nPress Ctrl+V to paste it into the username field, then press Tab to move to the password field.\n\nThe password will be automatically copied to clipboard in 3 seconds.").arg(username)
    );
    
    // Schedule password copy after a delay
    QTimer::singleShot(3000, [this, password]() {
        QApplication::clipboard()->setText(password);
        QMessageBox::information(
            this,
            tr("Autofill"),
            tr("Password has been copied to clipboard.\n\nPress Ctrl+V to paste it into the password field.")
        );
    });
} 