#!/bin/bash

# Script to create a .deb package for Password Manager

# Exit on error
set -e

# Configuration
APP_NAME="passwordmanager"
APP_VERSION="1.0.0"
MAINTAINER="Your Name <your.email@example.com>"
DESCRIPTION="Secure password manager application"
ARCHITECTURE="amd64"
DEPENDS="libqt6core6, libqt6gui6, libqt6widgets6, libqt6sql6, libqt6sql6-sqlite, libssl3"

# Create temporary directory structure
TEMP_DIR="$(mktemp -d)"
PKG_DIR="${TEMP_DIR}/${APP_NAME}_${APP_VERSION}"
mkdir -p "${PKG_DIR}/DEBIAN"
mkdir -p "${PKG_DIR}/usr/bin"
mkdir -p "${PKG_DIR}/usr/share/${APP_NAME}"
mkdir -p "${PKG_DIR}/usr/share/applications"
mkdir -p "${PKG_DIR}/usr/share/icons/hicolor/256x256/apps"

# Create control file
cat > "${PKG_DIR}/DEBIAN/control" << EOF
Package: ${APP_NAME}
Version: ${APP_VERSION}
Section: utils
Priority: optional
Architecture: ${ARCHITECTURE}
Depends: ${DEPENDS}
Maintainer: ${MAINTAINER}
Description: ${DESCRIPTION}
 A secure password manager application that allows users to store,
 manage, and import passwords with features like auto-fill functionality.
 .
 Features include:
  * Secure password storage using AES-256 encryption
  * CSV import functionality
  * Auto-fill for websites
  * User-friendly Qt-based interface
EOF

# Copy executable and resources
echo "Copying executable and resources..."
cp "../../build/PasswordManager" "${PKG_DIR}/usr/bin/${APP_NAME}"
chmod +x "${PKG_DIR}/usr/bin/${APP_NAME}"

# Create a default icon if the original is not available
echo "Creating default application icon..."
cat > "${PKG_DIR}/usr/share/icons/hicolor/256x256/apps/${APP_NAME}.png" << EOF
iVBORw0KGgoAAAANSUhEUgAAAQAAAAEACAYAAABccqhmAAAACXBIWXMAAA7EAAAOxAGVKw4bAAAF
EWlUWHRYTUw6Y29tLmFkb2JlLnhtcAAAAAAAPD94cGFja2V0IGJlZ2luPSLvu78i iglkPSJXNU0w
TXBDZWhpSHpyZVN6TlRjemtjOWQiPz4gPHg6eG1wbWV0YSB4bWxuczp4PSJhZG9iZTpuczptZXRh
LyIgeDp4bXB0az0iQWRvYmUgWE1QIENvcmUgNS42LWMxNDIgNzkuMTYwOTI0LCAyMDE3LzA3LzEz
LTAxOjA2OjM5ICAgICAgICAiPiA8cmRmOlJERiB4bWxuczpypmY9Imh0dHA6Ly93d3cudzMub3Jn
LzE5OTkvMDIvMjItcmRmLXN5bnRheC1ucyMiPiA8cmRmOkRlc2NyaXB0aW9uIHJkZjphYm91dD0i
IiB4bWxuczp4bXA9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC8iIHhtbG5zOmRjPSJodHRw
Oi8vcHVybC5vcmcvZGMvZWxlbWVudHMvMS4xLyIgeG1sbnM6cGhvdG9zaG9wPSJodHRwOi8vbnMu
YWRvYmUuY29tL3Bob3Rvc2hvcC8xLjAvIiB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNv
bS94YXAvMS4wL21tLyIgeG1sbnM6c3RFdnQ9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9z
VHlwZS9SZXNvdXJjZUV2ZW50IyIgeG1wOkNyZWF0b3JUb29sPSJBZG9iZSBQaG90b3Nob3AgQ0Mg
MjAxOCAoV2luZG93cykiIHhtcDpDcmVhdGVEYXRlPSIyMDIzLTA3LTEzVDEyOjM5OjEwKzAzOjAw
IiB4bXA6TW9kaWZ5RGF0ZT0iMjAyMy0wNy0xM1QxMjo0MDowNSswMzowMCIgeG1wOk1ldGFkYXRh
RGF0ZT0iMjAyMy0wNy0xM1QxMjo0MDowNSswMzowMCIgZGM6Zm9ybWF0PSJpbWFnZS9wbmciIHBo
b3Rvc2hvcDpDb2xvck1vZGU9IjMiIHBob3Rvc2hvcDpJQ0NQcm9maWxlPSJzUkdCIElFQzYxOTY2
LTIuMSIgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDpmZDM1YTk2Yi03YWMwLTRkNDItOGU0ZS0z
NmM4ZWU2YTQzODAiIHhtcE1NOkRvY3VtZW50SUQ9InhtcC5kaWQ6ZmQzNWE5NmItN2FjMC00ZDQy
LThlNGUtMzZjOGVlNmE0MzgwIiB4bXBNTTpPcmlnaW5hbERvY3VtZW50SUQ9InhtcC5kaWQ6ZmQz
NWE5NmItN2FjMC00ZDQyLThlNGUtMzZjOGVlNmE0MzgwIj4gPHhtcE1NOkhpc3Rvcnk+IDxyZGY6
U2VxPiA8cmRmOmxpIHN0RXZ0OmFjdGlvbj0iY3JlYXRlZCIgc3RFdnQ6aW5zdGFuY2VJRD0ieG1w
LmlpZDpmZDM1YTk2Yi03YWMwLTRkNDItOGU0ZS0zNmM4ZWU2YTQzODAiIHN0RXZ0OndoZW49IjIw
MjMtMDctMTNUMTI6Mzk6MTArMDM6MDAiIHN0RXZ0OnNvZnR3YXJlQWdlbnQ9IkFkb2JlIFBob3Rv
c2hvcCBDQyAyMDE4IChXaW5kb3dzKSIvPiA8L3JkZjpTZXE+IDwveG1wTU06SGlzdG9yeT4gPC9y
ZGY6RGVzY3JpcHRpb24+IDwvcmRmOlJERj4gPC94OnhtcG1ldGE+IDw/eHBhY2tldCBlbmQ9InIi
Pz4NCjxzdmcgdmlld0JveD0iMCAwIDI1NiAyNTYiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8y
MDAwL3N2ZyI+PHJlY3QgZmlsbD0iIzMzNjZDQyIgd2lkdGg9IjI1NiIgaGVpZ2h0PSIyNTYiIHJ4
PSIzMiIgcnk9IjMyIi8+PHBhdGggZD0iTTEyOCA2NEE2NCA2NCAwIDAgMCA2NCAxMjhBNjQgNjQg
MCAwIDAgMTI4IDE5MkE2NCA2NCAwIDAgMCAxOTIgMTI4QTY0IDY0IDAgMCAwIDEyOCA2NFoiIGZp
bGw9IiNGRkYiLz48cmVjdCB4PSIxMjAiIHk9IjEwMCIgd2lkdGg9IjE2IiBoZWlnaHQ9IjU2IiBm
aWxsPSIjMzM2NkNDIi8+PHJlY3QgeD0iMTAwIiB5PSIxMjAiIHdpZHRoPSI1NiIgaGVpZ2h0PSIx
NiIgZmlsbD0iIzMzNjZDQyIvPjwvc3ZnPg==
EOF

# Create desktop entry
cat > "${PKG_DIR}/usr/share/applications/${APP_NAME}.desktop" << EOF
[Desktop Entry]
Name=Password Manager
Comment=Secure password manager application
Exec=${APP_NAME}
Icon=${APP_NAME}
Terminal=false
Type=Application
Categories=Utility;Security;
Keywords=password;security;
EOF

# Build the package
echo "Building .deb package..."
dpkg-deb --build "${PKG_DIR}" .

# Clean up
echo "Cleaning up..."
rm -rf "${TEMP_DIR}"

echo "Package created: ${APP_NAME}_${APP_VERSION}_${ARCHITECTURE}.deb" 