# Password Manager Installer

This directory contains scripts and configuration files to create installation packages for both Windows and Linux platforms.

## Directory Structure

```
installer/
├── README.md                   # This file
├── windows/                    # Windows installer files
│   └── PasswordManager.nsi     # NSIS installer script
└── linux/                      # Linux installer files
    ├── create_deb.sh           # Script to create Debian package
    └── create_appimage.sh      # Script to create AppImage
```

## Windows Installer (Setup.exe)

The Windows installer is created using NSIS (Nullsoft Scriptable Install System).

### Prerequisites

1. Install NSIS from [nsis.sourceforge.net](https://nsis.sourceforge.net/Download)
2. Build the Password Manager application in Release mode

### Creating the Windows Installer

1. Open NSIS
2. Load the `windows/PasswordManager.nsi` script
3. Click "Compile NSI" or use the command line:
   ```
   makensis installer/windows/PasswordManager.nsi
   ```
4. The installer will be created as `installer/windows/PasswordManager_Setup.exe`

### Customizing the Windows Installer

You can modify the `PasswordManager.nsi` file to:
- Change the application name, version, and publisher information
- Modify installation directory
- Add additional files to the installer
- Change the license file
- Customize the installer appearance

## Linux Installers

### Debian Package (.deb)

The `create_deb.sh` script creates a Debian package suitable for Ubuntu, Debian, and other Debian-based distributions.

#### Prerequisites

1. Build the Password Manager application
2. Install the `dpkg-deb` tool:
   ```
   sudo apt-get install dpkg-dev
   ```

#### Creating the Debian Package

1. Navigate to the `installer/linux` directory
2. Make the script executable:
   ```
   chmod +x create_deb.sh
   ```
3. Run the script:
   ```
   ./create_deb.sh
   ```
4. The Debian package will be created as `passwordmanager_1.0.0_amd64.deb`

### AppImage

The `create_appimage.sh` script creates an AppImage that can run on most Linux distributions without installation.

#### Prerequisites

1. Build the Password Manager application
2. Install required tools:
   ```
   sudo apt-get install wget fuse libfuse2
   ```

#### Creating the AppImage

1. Navigate to the `installer/linux` directory
2. Make the script executable:
   ```
   chmod +x create_appimage.sh
   ```
3. Run the script:
   ```
   ./create_appimage.sh
   ```
4. The AppImage will be created as `PasswordManager-1.0.0-x86_64.AppImage`

## Notes

- Make sure to build the application in Release mode before creating installers
- Update version numbers in the scripts when releasing new versions
- Test the installers on clean systems to ensure all dependencies are properly included 