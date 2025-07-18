<<<<<<< HEAD
# Password Manager

Güvenli ve kullanıcı dostu bir şifre yönetici uygulaması. Tarayıcılardan (Chrome, Firefox, Edge) şifreleri otomatik olarak içe aktarabilir ve güvenli bir şekilde saklayabilir.

## Özellikler

- Kullanıcı girişi ve kayıt sistemi
- Şifrelerin güvenli bir şekilde saklanması
- Tarayıcılardan şifre aktarımı (Chrome, Firefox, Edge)
- Şifre ekleme, düzenleme, silme ve arama
- Modern Qt arayüzü
- Çapraz platform desteği (Windows ve Linux)

## Gereksinimler

- CMake (>= 3.16)
- Qt6
- SQLite3
- C++17 uyumlu derleyici
- Windows için:
  - Visual Studio 2019 veya üzeri
  - Windows SDK
- Linux için:
  - GCC/Clang
  - Qt6 geliştirme paketleri
  - SQLite3 geliştirme paketleri

## Kurulum

### Windows

1. Qt6'yı [Qt'nin resmi sitesinden](https://www.qt.io/download) indirin ve kurun
2. Visual Studio'yu kurun
3. CMake'i kurun
4. SQLite3'ü kurun
5. Projeyi derleyin:
```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

### Linux (Ubuntu/Debian)

1. Gerekli paketleri kurun:
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev libsqlite3-dev
```

2. Projeyi derleyin:
```bash
mkdir build
cd build
cmake ..
make
```

## Kullanım

1. Uygulamayı başlatın
2. Yeni bir hesap oluşturun veya mevcut hesabınızla giriş yapın
3. Ana şifrenizi güvenli bir yerde saklayın
4. Tarayıcılardan şifrelerinizi içe aktarın veya manuel olarak ekleyin
5. Şifrelerinizi güvenli bir şekilde yönetin

## Güvenlik

- Tüm şifreler AES-256 ile şifrelenerek saklanır
- Ana parola yerel olarak hash'lenir (SHA-256)
- Hiçbir veri dışarıya gönderilmez
- Tüm veriler yerel SQLite veritabanında saklanır

## Katkıda Bulunma

1. Bu depoyu fork edin
2. Yeni bir branch oluşturun (`git checkout -b feature/amazing-feature`)
3. Değişikliklerinizi commit edin (`git commit -m 'Add some amazing feature'`)
4. Branch'inizi push edin (`git push origin feature/amazing-feature`)
5. Bir Pull Request oluşturun

## Lisans

Bu proje MIT lisansı altında lisanslanmıştır. Detaylar için [LICENSE](LICENSE) dosyasına bakın. 

###############################################################################################
# Password Manager

A Qt-based secure password manager application that allows users to store, manage, and import passwords with features like auto-fill functionality.

## Project Structure

```
PasswordManager/
├── CMakeLists.txt              # Main CMake configuration file
├── src/                        # Source code directory
│   ├── CMakeLists.txt          # Source-specific CMake configuration
│   ├── main.cpp                # Application entry point
│   ├── database.h/cpp          # Database management and encryption
│   ├── passwordmanager.h/cpp   # Core password management functionality
│   ├── mainwindow.h/cpp        # Main application window UI
│   ├── loginwindow.h/cpp       # Login/registration UI
│   ├── passworddialog.h/cpp    # Dialog for adding/editing passwords
│   └── resources/              # Application resources
│       └── resources.qrc       # Qt resource file
├── build/                      # Build directory (created during build)
│   ├── Debug/                  # Debug build output
│   └── Release/                # Release build output
```

## Key Files and Responsibilities

### Core Components

1. **database.h/cpp**
   - Handles SQLite database operations
   - Manages user authentication and registration
   - Implements password encryption/decryption using OpenSSL
   - Manages schema upgrades and database initialization

2. **passwordmanager.h/cpp**
   - Core business logic for password management
   - Provides interface between UI and database
   - Handles CSV import functionality
   - Implements password search and auto-fill features

3. **mainwindow.h/cpp**
   - Main application window UI
   - Displays password list and management options
   - Handles clipboard monitoring for auto-fill
   - Manages user interactions with the password list

4. **loginwindow.h/cpp**
   - Handles user login and registration
   - Validates user credentials
   - Creates new user accounts

### Supporting Components

5. **passworddialog.h/cpp**
   - Dialog for adding or editing password entries
   - Form validation for password entries

6. **main.cpp**
   - Application entry point
   - Sets up application settings and launches the login window

## Database Schema

The application uses an SQLite database with the following structure:

1. **users table**
   - id: INTEGER PRIMARY KEY
   - username: TEXT
   - password: TEXT (hashed)
   - salt: BLOB

2. **passwords table**
   - id: INTEGER PRIMARY KEY
   - user_id: INTEGER (foreign key to users.id)
   - name: TEXT (name/title of the entry)
   - url: TEXT (website URL)
   - username: TEXT (username for the website)
   - password: BLOB (encrypted password)
   - note: TEXT (additional notes)

## Features

1. **Secure Password Storage**
   - Passwords are encrypted using OpenSSL
   - User passwords are hashed with salt for secure storage

2. **CSV Import**
   - Import passwords from CSV files
   - Support for standard CSV format with headers
   - Duplicate detection during import

3. **Auto-fill Functionality**
   - Monitors clipboard for copied URLs
   - Matches URLs with stored credentials
   - Two-step clipboard copying (username, then password)

4. **Search Functionality**
   - Search across all password entries
   - Filter by URL, username, or name

## Build Instructions

### Prerequisites

1. **Qt 6.8.3 or later**
   - Download from [Qt website](https://www.qt.io/download)
   - Install Qt with MSVC 2022 64-bit components

2. **OpenSSL**
   - Install OpenSSL for Windows (version 3.x recommended)
   - Default location: C:/Program Files/OpenSSL-Win64

3. **Visual Studio Build Tools 2022**
   - Install with C++ desktop development workload

4. **CMake 3.16 or later**
   - Usually included with Qt installation

### Build Steps

1. **Create Build Directory**
   ```powershell
   mkdir build
   cd build
   ```

2. **Configure with CMake**
   ```powershell
   cmake .. -DOPENSSL_SSL_LIBRARY="C:/Program Files/OpenSSL-Win64/lib/libssl.lib" ^
            -DOPENSSL_CRYPTO_LIBRARY="C:/Program Files/OpenSSL-Win64/lib/libcrypto.lib" ^
            -DOPENSSL_INCLUDE_DIR="C:/Program Files/OpenSSL-Win64/include" ^
            -DOPENSSL_ROOT_DIR="C:/Program Files/OpenSSL-Win64" ^
            -DCMAKE_PREFIX_PATH="<path-to-qt>/Qt/6.8.3/msvc2022_64"
   ```
   Replace `<path-to-qt>` with your Qt installation path.

3. **Build the Project**
   ```powershell
   cmake --build . --config Release
   ```
   Use `--config Debug` for debug builds.

4. **Deploy Qt Dependencies**
   ```powershell
   <path-to-qt>/Qt/6.8.3/msvc2022_64/bin/windeployqt.exe ./Release/PasswordManager.exe
   ```
   This copies all required Qt DLLs to the executable directory.

5. **Deploy OpenSSL Dependencies**
   ```powershell
   copy "C:\Program Files\OpenSSL-Win64\bin\libcrypto-3-x64.dll" ".\Release\"
   copy "C:\Program Files\OpenSSL-Win64\bin\libssl-3-x64.dll" ".\Release\"
   ```

6. **Run the Application**
   ```powershell
   ./Release/PasswordManager.exe
   ```

## Troubleshooting

1. **Missing DLL Errors**
   - Ensure Qt and OpenSSL DLLs are in the same directory as the executable
   - Run windeployqt again if Qt DLLs are missing

2. **Database Errors**
   - If you encounter database schema issues, you may need to delete the existing database file
   - Located in: `%APPDATA%\PasswordManager\passwords.db`

3. **Build Errors**
   - Verify Qt and OpenSSL paths in CMake configuration
   - Ensure Visual Studio Build Tools are properly installed

## Development Notes

- The application stores its database in the user's AppData directory
- Password encryption uses AES-256 with OpenSSL
- User passwords are hashed using SHA-256 with a random salt
- CSV import expects columns: name, url, username, password, note (header required) 
=======
# PasswordManager
Basic, Secure and user-friendly password manager.
>>>>>>> 0c1bc2e57f30131035034c9d454c347c1e1fd2e1
