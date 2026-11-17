# ANDROMEDA + Arturia KeyLab Essential 49 (Ableton Live)

## Setup / Configuración
1. Live → Settings → Link, Tempo & MIDI: Control Surface = **KeyLab Essential mk3** (or *KeyLab Essential* for mk1/mk2),
   Input/Output = KeyLab Essential **DAW** port. Turn on **Track** and **Remote** for the KeyLab **MIDI** port.
2. Select the ANDROMEDA track. The KeyLab screen shows the plug-in's parameters, 8 per bank, on the 8 knobs.
3. Move between banks with the KeyLab's bank/page navigation (or Live's device bank arrows).

## The 10 banks (8 knobs each) / Los 10 bancos
| Bank | Knobs 1–8 |
|---|---|
| 1 PERFORM | Drift · Haze · Bloom · Gravity · Auto · Auto Amount · Energy · FX Mix |
| 2 SPACE TAPE | Mode · Repeat Rate · Intensity · Echo Vol · Reverb Vol · Bass · Treble · Input Vol |
| 3 TAPE / COSMOS | Wow · Flutter · Tape Age · Echo On · Cosmos Size · Damp · Pre-delay · Cosmos Vol |
| 4 VOLUMES | Drums · Bass · Pad · Voice · Bells · Lead · Texture · Input |
| 5 ECHO SENDS | same 8 tracks → Space Tape |
| 6 COSMOS SENDS | same 8 tracks → Cosmos |
| 7 SOUND (selected track) | Cutoff · Resonance · Attack · Release · Warble · Drive · Level · Filter Env |
| 8 FX | FX Type · X · Y · On · Scene · Mood · Chaos · Swing |
| 9 EXTRAS | Master · Tape Sync · Tape Division · Tape Freeze · Input Filter · Input Reso · Input Drive · Input Motion |
| 10 | Track (which track bank 7 edits) · Key Follow (RAW / SNAP / SHIFT) |

## Keys, pads, wheels / Teclas, pads, ruedas
- **Keys** play the selected track. KEYS mode: RAW (as played) · SNAP (into the scale) · SHIFT (a note outside the scale moves the music to the nearest scale on the next beat).
- **Pads** (send notes on MIDI channel 10, notes 36–51 — set them to that in Arturia MIDI Control Center if needed; the channel can be changed in INPUT · MIDI):
  - Pads 1–8 → scenes A–H (they change on the next bar)
  - Pads 9–16 → FREEZE · TAPE STOP · SPIN · FILL · SUPERNOVA · ECLIPSE · SPRING KICK · FX on/off
- **Mod wheel** → FX X. **Sustain pedal** → FREEZE.
- **Faders**: in DAW mode Live uses them for track volumes. For ANDROMEDA, use INPUT · MIDI → LEARN: tap a knob on screen, move a fader.

Any other MIDI controller works the same way: keys, pads on channel 10, and LEARN for everything else.
