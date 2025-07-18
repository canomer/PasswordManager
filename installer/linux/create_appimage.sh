#!/bin/bash

# Script to create an AppImage for Password Manager

# Exit on error
set -e

# Configuration
APP_NAME="PasswordManager"
APP_VERSION="1.0.0"

# Create AppDir structure
APPDIR="AppDir"
mkdir -p "${APPDIR}/usr/bin"
mkdir -p "${APPDIR}/usr/lib"
mkdir -p "${APPDIR}/usr/share/applications"
mkdir -p "${APPDIR}/usr/share/icons/hicolor/256x256/apps"

# Copy executable
echo "Copying executable..."
cp "../../build/PasswordManager" "${APPDIR}/usr/bin/"
chmod +x "${APPDIR}/usr/bin/PasswordManager"

# Copy shared libraries
echo "Copying shared libraries..."
# Use ldd to find dependencies and copy them
for lib in $(ldd ../../build/PasswordManager | grep "=> /" | awk '{print $3}'); do
  cp -L "$lib" "${APPDIR}/usr/lib/"
done

# Copy Qt plugins
echo "Copying Qt plugins..."
QT_PLUGIN_PATH=$(qmake -query QT_INSTALL_PLUGINS)
mkdir -p "${APPDIR}/usr/plugins/platforms"
mkdir -p "${APPDIR}/usr/plugins/sqldrivers"
cp "${QT_PLUGIN_PATH}/platforms/libqxcb.so" "${APPDIR}/usr/plugins/platforms/"
cp "${QT_PLUGIN_PATH}/sqldrivers/libqsqlite.so" "${APPDIR}/usr/plugins/sqldrivers/"

# Copy OpenSSL libraries if not included in dependencies
if [ ! -f "${APPDIR}/usr/lib/libssl.so.3" ]; then
  echo "Copying OpenSSL libraries..."
  cp "/usr/lib/x86_64-linux-gnu/libssl.so.3" "${APPDIR}/usr/lib/" || true
  cp "/usr/lib/x86_64-linux-gnu/libcrypto.so.3" "${APPDIR}/usr/lib/" || true
fi

# Create a default icon
echo "Creating default application icon..."
cat > "${APPDIR}/usr/share/icons/hicolor/256x256/apps/${APP_NAME}.png" << EOF
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

# Create desktop file
cat > "${APPDIR}/usr/share/applications/${APP_NAME}.desktop" << EOF
[Desktop Entry]
Name=Password Manager
Comment=Secure password manager application
Exec=PasswordManager
Icon=${APP_NAME}
Terminal=false
Type=Application
Categories=Utility;Security;
Keywords=password;security;
EOF

# Create AppRun script
cat > "${APPDIR}/AppRun" << 'EOF'
#!/bin/bash
APPDIR="$(dirname "$(readlink -f "$0")")"
export LD_LIBRARY_PATH="${APPDIR}/usr/lib:${LD_LIBRARY_PATH}"
export QT_PLUGIN_PATH="${APPDIR}/usr/plugins"
export QML_IMPORT_PATH="${APPDIR}/usr/qml"
export QML2_IMPORT_PATH="${APPDIR}/usr/qml"
export XDG_DATA_DIRS="${APPDIR}/usr/share:${XDG_DATA_DIRS}"
exec "${APPDIR}/usr/bin/PasswordManager" "$@"
EOF

chmod +x "${APPDIR}/AppRun"

# Create symlinks for icon and desktop file in AppDir root
ln -sf "./usr/share/icons/hicolor/256x256/apps/${APP_NAME}.png" "${APPDIR}/${APP_NAME}.png"
ln -sf "./usr/share/applications/${APP_NAME}.desktop" "${APPDIR}/${APP_NAME}.desktop"

# Download linuxdeploy and the Qt plugin
echo "Downloading linuxdeploy..."
wget -c "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
wget -c "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
chmod +x linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-plugin-qt-x86_64.AppImage

# Build the AppImage
echo "Building AppImage..."
export OUTPUT="${APP_NAME}-${APP_VERSION}-x86_64.AppImage"
./linuxdeploy-x86_64.AppImage --appdir="${APPDIR}" --plugin=qt --output=appimage

echo "AppImage created: ${OUTPUT}"

# Clean up
echo "Cleaning up..."
rm -rf "${APPDIR}" linuxdeploy-x86_64.AppImage linuxdeploy-plugin-qt-x86_64.AppImage 