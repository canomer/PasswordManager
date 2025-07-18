# Password Manager Project Structure

## Directory Hierarchy

```
PasswordManager/
├── CMakeLists.txt              # Main CMake configuration file for the project
├── README.md                   # Project documentation and build instructions
├── Structure.md                # This file - detailed project structure information
├── src/                        # Source code directory containing all application code
│   ├── CMakeLists.txt          # CMake configuration for the source directory
│   ├── main.cpp                # Application entry point, initializes Qt application
│   ├── database.h              # Database class declaration for SQLite operations
│   ├── database.cpp            # Database class implementation with encryption/decryption logic
│   ├── passwordmanager.h       # PasswordManager class declaration, business logic interface
│   ├── passwordmanager.cpp     # PasswordManager implementation, handles password operations
│   ├── mainwindow.h            # MainWindow UI class declaration
│   ├── mainwindow.cpp          # MainWindow UI implementation, main application window
│   ├── loginwindow.h           # LoginWindow UI class declaration
│   ├── loginwindow.cpp         # LoginWindow implementation, handles user authentication
│   ├── passworddialog.h        # PasswordDialog class declaration for adding/editing passwords
│   ├── passworddialog.cpp      # PasswordDialog implementation
│   ├── resources/              # Directory containing application resources
│   │   └── resources.qrc       # Qt resource collection file
│   │       ├── icons/          # Application icons
│   │       │   ├── app.ico     # Application icon
│   │       │   └── ...         # Other icons used in the application
│   │       └── styles/         # Application stylesheets
│   │           └── style.qss   # Qt stylesheet for application theming
├── build/                      # Build directory (created during build process)
│   ├── Debug/                  # Debug build output directory
│   │   ├── PasswordManager.exe # Debug executable
│   │   ├── Qt6Core.dll         # Qt Core library
│   │   ├── Qt6Gui.dll          # Qt GUI library
│   │   ├── Qt6Widgets.dll      # Qt Widgets library
│   │   ├── Qt6Sql.dll          # Qt SQL library
│   │   ├── libcrypto-3-x64.dll # OpenSSL crypto library
│   │   ├── libssl-3-x64.dll    # OpenSSL SSL library
│   │   └── ...                 # Other DLLs and build artifacts
│   └── Release/                # Release build output directory
│       ├── PasswordManager.exe # Release executable
│       ├── Qt6Core.dll         # Qt Core library
│       ├── Qt6Gui.dll          # Qt GUI library
│       ├── Qt6Widgets.dll      # Qt Widgets library
│       ├── Qt6Sql.dll          # Qt SQL library
│       ├── libcrypto-3-x64.dll # OpenSSL crypto library
│       ├── libssl-3-x64.dll    # OpenSSL SSL library
│       └── ...                 # Other DLLs and build artifacts
```

## Important System Files

### Application Data

The application stores its data in the user's AppData directory:

```
%APPDATA%\PasswordManager\
└── passwords.db               # SQLite database file containing all user data and passwords
```

On Windows, this typically resolves to:
`C:\Users\<username>\AppData\Roaming\PasswordManager\passwords.db`

### Database Structure

The SQLite database (`passwords.db`) contains two main tables:

1. **users table**
   - Stores user authentication information
   - Contains: id, username, password (hashed), salt

2. **passwords table**
   - Stores encrypted password entries
   - Contains: id, user_id, name, url, username, password (encrypted), note

### Build System Files

During the build process, CMake generates several files:

```
build/
├── CMakeCache.txt             # CMake cache file
├── CMakeFiles/                # CMake build files
├── cmake_install.cmake        # CMake installation script
├── PasswordManager.sln        # Visual Studio solution file (on Windows)
└── src/
    ├── CMakeFiles/            # Source-specific CMake build files
    ├── PasswordManagerLib.vcxproj  # Visual Studio project for the library
    └── PasswordManagerLib.vcxproj.filters  # Visual Studio filters
```

## Key Component Relationships

```
main.cpp
  └── Creates LoginWindow
       └── On successful login, creates MainWindow
            ├── Uses PasswordManager for business logic
            │    └── Uses Database for data operations
            └── Creates PasswordDialog for adding/editing passwords
```

## File Responsibilities

### Core Components

1. **database.h/cpp**
   - Manages SQLite database connection and operations
   - Handles user authentication (login/registration)
   - Implements password encryption/decryption using OpenSSL
   - Manages database schema and upgrades
   - Responsible for secure storage of all sensitive information

2. **passwordmanager.h/cpp**
   - Provides high-level password management operations
   - Acts as an interface between UI and database
   - Handles CSV import functionality
   - Implements password searching and filtering
   - Manages clipboard operations for auto-fill functionality
   - Checks for duplicate passwords during import

3. **mainwindow.h/cpp**
   - Implements the main application window UI
   - Displays the list of stored passwords in a table
   - Handles user interactions (add, edit, delete passwords)
   - Manages clipboard monitoring for auto-fill
   - Implements context menu for password operations
   - Handles CSV import UI interaction

4. **loginwindow.h/cpp**
   - Implements the login and registration UI
   - Validates user input for login/registration
   - Communicates with Database for authentication
   - Manages transition to the main application window

### Supporting Components

5. **passworddialog.h/cpp**
   - Implements dialog for adding/editing password entries
   - Validates password entry fields
   - Generates random passwords when requested
   - Communicates with PasswordManager for CRUD operations

6. **main.cpp**
   - Application entry point
   - Sets up Qt application settings
   - Initializes resources
   - Creates the login window to start the application

## Runtime Data Flow

1. User enters credentials in LoginWindow
2. LoginWindow passes credentials to Database for validation
3. On successful login, MainWindow is created
4. MainWindow uses PasswordManager to retrieve passwords
5. PasswordManager queries Database for encrypted passwords
6. Database decrypts passwords and returns them to PasswordManager
7. PasswordManager passes decrypted passwords to MainWindow for display
8. When user adds/edits a password via PasswordDialog:
   - Data flows from PasswordDialog → MainWindow → PasswordManager → Database
9. When importing from CSV:
   - Data flows from MainWindow → PasswordManager → Database
10. When using auto-fill:
    - Clipboard events flow from system → MainWindow → PasswordManager → clipboard 