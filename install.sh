#!/bin/bash
# ANDROMEDA one-line installer for Mac:  curl -fsSL https://drophlex.github.io/ANDROMEDA/install.sh | bash
set -e
echo ""; echo "  A N D R O M E D A  ·  installing / instalando…"; echo ""
T="$(mktemp -d)"; cd "$T"
curl -fsSL -o A.zip https://github.com/drophlex/ANDROMEDA/releases/latest/download/ANDROMEDA-mac.zip
ditto -x -k A.zip .
V="$HOME/Library/Audio/Plug-Ins/VST3"; C="$HOME/Library/Audio/Plug-Ins/Components"; mkdir -p "$V" "$C" "$HOME/Applications"
rm -rf "$V/ANDROMEDA.vst3" "$C/ANDROMEDA.component" "$HOME/Applications/ANDROMEDA.app"
cp -R ANDROMEDA/ANDROMEDA.vst3 "$V/"; cp -R ANDROMEDA/ANDROMEDA.component "$C/"; cp -R ANDROMEDA/ANDROMEDA.app "$HOME/Applications/"
xattr -dr com.apple.quarantine "$V/ANDROMEDA.vst3" "$C/ANDROMEDA.component" "$HOME/Applications/ANDROMEDA.app" 2>/dev/null || true
killall -9 AudioComponentRegistrar 2>/dev/null || true
cd /; rm -rf "$T"
echo "  ✓ VST3 → ~/Library/Audio/Plug-Ins/VST3"
echo "  ✓ AU   → ~/Library/Audio/Plug-Ins/Components"
echo "  ✓ App  → ~/Applications (Aplicaciones de tu usuario)"
echo ""; echo "  ✓ ANDROMEDA instalado / installed. Abre Ableton → Ajustes → Plug-ins → Volver a escanear."; echo ""
