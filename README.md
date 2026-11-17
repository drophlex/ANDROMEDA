# ANDROMEDA

**An ambient instrument.** *Un instrumento ambient.*
Dark Funk Durcal design · Polaris Sound Lab

▶ **Web:** https://drophlex.github.io/ANDROMEDA/
⤓ **Mac plug-in (VST3 · AU · App):** [ANDROMEDA-mac.zip](https://github.com/drophlex/ANDROMEDA/releases/latest/download/ANDROMEDA-mac.zip) — or install with one line in Terminal / o instala con una línea en Terminal:

```
curl -fsSL https://drophlex.github.io/ANDROMEDA/install.sh | bash
```

📖 **Manual (PDF):** [English + Español](manual/ANDROMEDA-Manual-EN-ES.pdf) · [Solo español](manual/ANDROMEDA-Manual-ES.pdf) · **with the FM-1 / con el FM-1:** [EN + ES](manual/ANDROMEDA-Manual-Completo-EN-ES.pdf) · [ES](manual/ANDROMEDA-Manual-Completo-ES.pdf)

🎛 **M-VAVE FM-1 firmware:** [drophlex/ANDROMEDAFM1](https://github.com/drophlex/ANDROMEDAFM1)

![ANDROMEDA](img/andromeda-main.jpg)

---

## English

Warm, drifting, nostalgic ambient music: breathing pulse-width pads, choirs, music boxes, FM bells, bowed metal, lo-fi drum machines, dust and wind. **Everything is always in key.**

- **Draw a pattern, then turn on ✦ AUTO**: ANDROMEDA keeps writing from *your* pattern (your intervals, your rhythms), always in key and in time, with parts fading in and out by ENERGY.
- **Play over it on any keyboard.** KEYS = SHIFT: a note outside the scale glides the music to the nearest scale on the next beat; what you play becomes a motif the music answers.
- **13 moods**, 53 sounds, 7 drum kits, 7 textures, scenes A–H, song mode.
- **Space Tape**: three-head tape echo with the 12-position mode selector, repeat rate, intensity, bass, treble, input level with peak light, wow, flutter, tape age, spring reverb (kick it!), freeze, spin, tempo sync.
- **16 FX & filters** that enter on the beat and leave with a fade: ladder, acid, MS, high sweep, band, vowel, envelope, notch, phaser, jet, ensemble, repeat, dust, half-speed, gate, metal. XY pad.
- **Audio in**: mic, line, an audio file or (in the plug-in) a sidechain, through its own filter, the tape and the Cosmos reverb.
- Every change is smooth: scenes on the bar, mutes with fades, effects on the 1/16.
- MIDI in, pads, MIDI learn, export **.mid**, record **WAV**, save **.andromeda** (same file in web and plug-in).

### Mac plug-in
Download the zip above, unzip, right-click **Install ANDROMEDA.command → Open**. In Ableton: Settings → Plug-ins → VST3 system folders on → Rescan. It follows Live's tempo; 74 parameters in banks of 8 appear on any controller screen. Arturia KeyLab Essential 49 guide: [installer/KeyLab-Essential-49.md](installer/KeyLab-Essential-49.md).

---

## Español

Música ambient cálida, flotante y nostálgica. **Todo suena siempre en tono.**

- **Dibuja un patrón y enciende ✦ AUTO**: ANDROMEDA sigue escribiendo a partir de *tu* patrón (tus intervalos, tus ritmos), siempre en tono y en tiempo; las partes entran y salen con fundido según la ENERGÍA.
- **Toca encima con cualquier teclado.** TECLAS = MODULAR: una nota fuera de la escala lleva la música a la escala más cercana en el siguiente tiempo, y lo que tocas se vuelve un motivo al que la música contesta.
- **13 estados**, 53 sonidos, 7 kits, 7 texturas, escenas A–H, modo canción.
- **Space Tape**: eco de cinta de tres cabezas con el selector de 12 modos, velocidad, intensidad, graves, agudos, volumen de entrada con luz de pico, wow, flutter, desgaste, resorte (¡golpéalo!), congelar, girar y sincronía.
- **16 efectos y filtros** que entran a tiempo y salen con fundido. Pad XY.
- **Entrada de audio**: micrófono, línea, un archivo o (en el plug-in) sidechain.
- Todo cambia suave: escenas al compás, silencios con fundido, efectos al 1/16.

### Plug-in para Mac
Descarga el zip, descomprímelo, clic derecho en **Install ANDROMEDA.command → Abrir**. En Ableton: Ajustes → Plug-ins → carpetas de sistema VST3 → Volver a escanear. Guía del KeyLab Essential 49: [installer/KeyLab-Essential-49.md](installer/KeyLab-Essential-49.md).

---

`engine/` C++ engine (same code in web as WebAssembly and in the plug-in) · `ui/` interface · `plugin/` JUCE wrapper · `tools/build_web.py` builds the web page.
