#!/bin/bash
# ANDROMEDA installer — copies the plugins to your user plug-in folders and clears the download quarantine.
cd "$(dirname "$0")"
echo ""
echo "  A N D R O M E D A"
echo "  an ambient instrument · Dark Funk Durcal design · Polaris Sound Lab"
echo ""
VST3="$HOME/Library/Audio/Plug-Ins/VST3"; AU="$HOME/Library/Audio/Plug-Ins/Components"
mkdir -p "$VST3" "$AU"
rm -rf "$VST3/ANDROMEDA.vst3" "$AU/ANDROMEDA.component"
cp -R ANDROMEDA.vst3 "$VST3/" && cp -R ANDROMEDA.component "$AU/"
[ -d ANDROMEDA.app ] && rm -rf "/Applications/ANDROMEDA.app" 2>/dev/null; cp -R ANDROMEDA.app /Applications/ 2>/dev/null || cp -R ANDROMEDA.app "$HOME/Applications/" 2>/dev/null
xattr -dr com.apple.quarantine "$VST3/ANDROMEDA.vst3" "$AU/ANDROMEDA.component" /Applications/ANDROMEDA.app "$HOME/Applications/ANDROMEDA.app" 2>/dev/null
killall -9 AudioComponentRegistrar 2>/dev/null
echo "  ✓ VST3  → $VST3"
echo "  ✓ AU    → $AU"
echo "  ✓ App   → Applications"
echo ""
echo "  Ableton Live: Settings → Plug-ins → turn on 'Use VST3 Plug-in System Folders' → Rescan."
echo "  Ableton Live: Ajustes → Plug-ins → activa 'Usar carpetas de sistema VST3' → Volver a escanear."
echo ""
read -n 1 -s -r -p "  Press any key / Presiona una tecla…"
