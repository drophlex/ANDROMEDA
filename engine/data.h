// ANDROMEDA engine · data.h — scales, sounds, kits, moods, parameters
#pragma once
#include "dsp.h"

namespace andro {

struct Scale { const char* id; const char* name; int n; int iv[7]; };
static const Scale SCALES[] = {
  {"lydian", "LYDIAN", 7, {0, 2, 4, 6, 7, 9, 11}}, {"major", "MAJOR", 7, {0, 2, 4, 5, 7, 9, 11}}, {"mixo", "MIXOLYDIAN", 7, {0, 2, 4, 5, 7, 9, 10}},
  {"dorian", "DORIAN", 7, {0, 2, 3, 5, 7, 9, 10}}, {"aeolian", "MINOR", 7, {0, 2, 3, 5, 7, 8, 10}}, {"phrygian", "PHRYGIAN", 7, {0, 1, 3, 5, 7, 8, 10}},
  {"harmmin", "HARMONIC MIN", 7, {0, 2, 3, 5, 7, 8, 11}}, {"penta", "PENTATONIC", 5, {0, 2, 4, 7, 9}}, {"mpenta", "MINOR PENTA", 5, {0, 3, 5, 7, 10}},
  {"hira", "HIRAJOSHI", 5, {0, 2, 3, 7, 8}}, {"whole", "WHOLE TONE", 6, {0, 2, 4, 6, 8, 10}}, {"insen", "IN SEN", 5, {0, 1, 5, 7, 10}}};
constexpr int NSCALES = 12;
inline int scaleNote(const Scale& s, int deg) { int L = s.n, o = deg >= 0 ? deg / L : -((-deg + L - 1) / L), d = deg - o * L; return s.iv[d] + 12 * o; }
inline bool inScale(const Scale& s, int key, int pc) { pc = ((pc - key) % 12 + 12) % 12; for (int i = 0; i < s.n; i++) if (s.iv[i] == pc) return true; return false; }

enum TrackKind { K_DRUM, K_SYNTH, K_TEX, K_INPUT };
struct TrackDef { const char* name; TrackKind kind; int base; int lo, hi; };
static const TrackDef TRACKS[8] = {{"DRUMS", K_DRUM, 36, 0, 127}, {"BASS", K_SYNTH, 36, 28, 57}, {"PAD", K_SYNTH, 60, 40, 86}, {"VOICE", K_SYNTH, 60, 45, 86},
  {"BELLS", K_SYNTH, 72, 52, 98}, {"LEAD", K_SYNTH, 72, 48, 93}, {"TEXTURE", K_TEX, 48, 0, 127}, {"INPUT", K_INPUT, 60, 0, 127}};
static const int RATE_STEPS[5] = {16, 8, 4, 2, 1};

// ---- synth patches: every value 0..127 (same data as the web prototype and the FM-1)
enum Eng { E_PAD, E_CHOIR, E_FM, E_BASS, E_LEAD, E_BOWED };
enum PF { PF_ENG, PF_W1, PF_W2, PF_DET, PF_MIX, PF_UNI, PF_SUB, PF_NOISE, PF_CUT, PF_RES, PF_ENV, PF_A, PF_D, PF_S, PF_R, PF_FA, PF_FD, PF_WARBLE, PF_VIB,
  PF_RATIO, PF_INDEX, PF_IDEC, PF_VOWEL, PF_GLIDE, PF_DRIVE, PF_LVL, PF_SPREAD, PF_OCT, PF_PWM, PF_CHORUS, PF_COUNT };
static const char* PF_NAMES[PF_COUNT] = {"eng", "w1", "w2", "det", "mix", "uni", "sub", "noise", "cut", "res", "env", "a", "d", "s", "r", "fa", "fd", "warble", "vib",
  "ratio", "index", "idec", "vowel", "glide", "drive", "lvl", "spread", "oct", "pwm", "chorus"};
static const uint8_t PF_DEF[PF_COUNT] = {0, 0, 0, 20, 90, 1, 0, 0, 70, 20, 30, 60, 70, 100, 80, 60, 70, 30, 10, 20, 40, 60, 0, 0, 10, 100, 60, 64, 0, 40};
struct Patch { char name[24]; char cat[8]; uint8_t v[PF_COUNT]; };
static const float FM_RATIOS[16] = {0.5f, 1, 2, 3, 4, 3.5f, 5, 7, 1.41f, 2.76f, 3.14f, 4.2f, 5.4f, 7.13f, 9.33f, 11.7f};
static const float BOW_SETS[4][3] = {{1.f, 2.f, 3.f}, {1.f, 2.76f, 5.4f}, {1.f, 1.59f, 2.14f}, {1.f, 3.01f, 4.95f}};
static const float VOWELS[5][3] = {{800, 1150, 2900}, {400, 1600, 2700}, {270, 2290, 3010}, {450, 800, 2830}, {325, 700, 2530}};

// name|cat|engine|values  (oct is stored +64)
static const char* PRESET_SRC[] = {
  // pads, warm and floating
  "Polar Dawn|PAD|pad|w1=0 w2=0 det=26 uni=2 cut=58 res=18 env=22 a=92 d=90 s=110 r=98 fa=96 fd=100 warble=44 vib=6",
  "Gossamer Saw|PAD|pad|w1=0 w2=0 det=34 uni=2 cut=50 res=24 env=46 a=86 d=104 s=96 r=100 fa=104 fd=110 warble=52 chorus=90",
  "Tidal Glass|PAD|pad|w1=1 w2=2 det=14 cut=66 res=34 env=40 a=80 d=96 s=90 r=96 fa=90 fd=104 warble=36 noise=8",
  "Velvet Orbit|PAD|pad|w1=0 w2=3 det=34 uni=2 cut=52 res=10 env=16 a=98 s=120 r=104 warble=52 spread=100",
  "Pulse Width Dream|PAD|pad|w1=4 w2=4 det=18 pwm=90 cut=62 res=20 env=30 a=84 s=110 r=100 warble=40 chorus=100",
  "Analog Strings|PAD|pad|w1=0 w2=0 det=40 uni=2 cut=74 res=6 env=10 a=76 s=118 r=92 warble=30 vib=12 chorus=110",
  "Cold Plateau|PAD|pad|w1=2 w2=2 det=40 cut=74 res=44 env=36 a=88 s=100 r=100 fa=104 fd=110 warble=30 noise=14",
  "Amber Fog|PAD|pad|w1=0 w2=1 det=18 uni=2 cut=44 res=26 env=30 a=100 s=118 r=108 warble=60 drive=30",
  "Low Sun|PAD|pad|w1=1 w2=1 det=22 cut=40 res=12 env=12 a=96 s=124 r=106 sub=50 warble=40 oct=63",
  "Saturn Hum|PAD|pad|w1=0 w2=0 det=8 cut=34 res=60 env=18 a=104 s=127 r=110 sub=70 warble=20 drive=46 oct=63",
  "Weightless|PAD|pad|w1=3 w2=2 det=30 cut=90 res=0 env=0 a=104 s=127 r=112 warble=24 vib=14 spread=110",
  "Paper Moon|PAD|pad|w1=2 w2=1 det=52 cut=56 res=22 env=26 a=70 d=84 s=80 r=90 warble=74 drive=22 noise=20",
  "Static Bloom|PAD|pad|w1=0 w2=1 det=60 uni=3 cut=62 res=48 env=50 a=94 d=104 s=70 r=104 fa=110 fd=116 warble=50 noise=30",
  "Distant Horn|PAD|pad|w1=0 w2=4 pwm=50 det=12 cut=38 res=14 env=28 a=90 s=110 r=100 warble=34 drive=20",
  // choirs and breath
  "Breath of Io|CHOIR|choir|w1=0 det=20 uni=2 vowel=20 a=96 s=120 r=104 warble=36 noise=30 cut=96",
  "Choir of Tides|CHOIR|choir|w1=0 det=26 uni=2 vowel=4 a=88 s=118 r=104 warble=40 vib=30 noise=24 cut=104 chorus=80",
  "Hollow Choir|CHOIR|choir|w1=0 det=32 uni=3 vowel=92 a=92 s=120 r=100 warble=30 noise=14 cut=88",
  "Ghost Vowel|CHOIR|choir|w1=1 det=12 vowel=60 a=104 s=110 r=110 warble=64 noise=44 cut=80",
  "Cathedral Air|CHOIR|choir|w1=0 det=40 uni=3 vowel=110 a=100 s=124 r=112 warble=20 noise=20 cut=100 spread=110",
  "Cathedral Drone|CHOIR|choir|w1=0 det=10 vowel=100 a=110 s=127 r=116 warble=16 noise=10 cut=70 sub=40 oct=63",
  "Whisper Field|CHOIR|choir|w1=2 det=24 vowel=40 a=90 s=100 r=106 warble=44 noise=80 cut=110",
  // bells, keys, metal (FM)
  "Glass Bell|BELL|fm|ratio=70 index=60 idec=70 a=0 d=96 s=0 r=96 warble=16 lvl=92 cut=112",
  "Music Box|BELL|fm|ratio=40 index=46 idec=50 a=0 d=84 s=0 r=84 warble=30 lvl=96 cut=112",
  "Heliotrope|BELL|fm|ratio=16 index=56 idec=46 a=0 d=80 s=10 r=80 warble=24 lvl=100 cut=120 chorus=70",
  "Satellite|BELL|fm|ratio=100 index=80 idec=60 a=0 d=100 s=10 r=100 warble=10 lvl=80 cut=112",
  "Ice Marimba|BELL|fm|ratio=52 index=54 idec=34 a=0 d=66 s=0 r=66 warble=8 lvl=104 cut=112",
  "Tin Gamelan|BELL|fm|ratio=88 index=70 idec=64 a=0 d=92 s=0 r=92 warble=22 lvl=90 cut=112",
  "Tape Piano|BELL|fm|ratio=8 index=44 idec=58 a=0 d=96 s=30 r=88 warble=60 vib=6 lvl=100 cut=100 chorus=60",
  "Electric Haze|BELL|fm|ratio=20 index=36 idec=56 a=4 d=92 s=50 r=84 warble=40 vib=8 lvl=96 cut=112",
  "Prepared Piano|BELL|fm|ratio=82 index=62 idec=30 a=0 d=74 s=0 r=70 warble=12 lvl=100 cut=100",
  "Clockwork|BELL|fm|ratio=112 index=90 idec=20 a=0 d=48 s=0 r=48 warble=4 lvl=92 cut=112",
  "Singing Bowl|BELL|fm|ratio=76 index=30 idec=100 a=20 d=110 s=40 r=112 warble=30 vib=12 lvl=84 cut=112",
  "Comet Pluck|BELL|fm|ratio=30 index=70 idec=30 a=0 d=70 s=0 r=76 warble=26 lvl=100 cut=112",
  "Iron Bell|BELL|fm|ratio=94 index=48 idec=96 a=0 d=112 s=0 r=112 warble=14 lvl=86 cut=100",
  // bowed / resonant metal (the darker, grey textures)
  "Rusted Bowl|METAL|bowed|ratio=40 noise=90 cut=90 res=100 a=100 s=120 r=110 warble=24 lvl=96",
  "Hollow Metal|METAL|bowed|ratio=90 noise=80 cut=80 res=110 a=90 s=110 r=112 warble=30 lvl=96",
  "Grey Water|METAL|bowed|ratio=110 noise=110 cut=70 res=90 a=110 s=127 r=116 warble=50 lvl=96",
  "Glass Harmonica|METAL|bowed|ratio=4 noise=60 cut=110 res=116 a=80 s=120 r=104 warble=20 vib=16 lvl=100",
  // basses
  "Deep Field|BASS|bass|w1=0 sub=80 cut=40 res=30 env=40 a=6 d=76 s=70 r=60 fa=0 fd=64 glide=20 drive=20",
  "Rubber Ball|BASS|bass|w1=3 sub=60 cut=60 res=40 env=70 a=0 d=60 s=60 r=50 fd=40 glide=60 drive=30",
  "Rubber Moon|BASS|bass|w1=1 sub=60 cut=34 res=64 env=64 a=2 d=60 s=40 r=50 fd=54 glide=30 drive=34",
  "Sub Gravity|BASS|bass|w1=3 sub=110 cut=60 res=0 env=0 a=20 d=90 s=110 r=80 glide=40",
  "Acid Dust|BASS|bass|w1=0 sub=10 cut=30 res=104 env=90 a=0 d=50 s=20 r=40 fd=46 glide=50 drive=56",
  "Warm Pulse|BASS|bass|w1=4 pwm=60 sub=70 cut=46 res=20 env=30 a=10 d=80 s=90 r=70 fd=80 glide=10 drive=14",
  // leads and arps
  "Lonely Probe|LEAD|lead|w1=2 w2=3 det=10 cut=76 res=20 env=30 a=30 d=80 s=90 r=86 warble=40 vib=30 glide=30",
  "Ice Bubble|LEAD|lead|w1=4 w2=3 pwm=40 det=6 cut=40 res=100 env=90 a=0 d=50 s=0 r=60 fd=40 warble=20 lvl=96",
  "Heliosphere Arp|LEAD|lead|w1=4 w2=0 pwm=70 det=12 cut=70 res=40 env=50 a=0 d=64 s=20 r=70 fd=56 warble=24 chorus=80",
  "Pulsewidth|LEAD|lead|w1=4 w2=4 pwm=110 det=8 cut=80 res=16 env=20 a=12 d=80 s=90 r=80 warble=30 vib=18",
  "Wobble Lead|LEAD|lead|w1=0 w2=1 det=16 cut=60 res=40 env=40 a=20 d=70 s=80 r=80 warble=90 vib=20 glide=40",
  "Sine Flute|LEAD|lead|w1=3 w2=2 det=6 cut=100 res=0 env=0 a=44 s=110 r=84 warble=30 vib=40 noise=24 glide=20",
  "Thin Signal|LEAD|lead|w1=1 w2=1 det=4 cut=84 res=30 env=20 a=10 d=60 s=60 r=70 warble=20 vib=16",
  "Dream Saw|LEAD|lead|w1=0 w2=0 det=30 uni=2 cut=64 res=30 env=36 a=40 s=100 r=90 warble=54 vib=10 glide=24",
  "Square Kid|LEAD|lead|w1=1 w2=3 det=8 cut=70 res=10 env=24 a=6 d=64 s=70 r=64 warble=30 vib=22",
};
constexpr int NPRESETS = sizeof(PRESET_SRC) / sizeof(PRESET_SRC[0]);

// ---- drum kits (synth recipes; voices: 0 kick 1 snare 2 hat 3 open hat 4 clap 5 rim 6 tom 7 clank 8 shaker 9 blip 10 cowbell)
static const char* DRUM_NAMES[11] = {"KICK", "SNARE", "HAT", "OPEN HAT", "CLAP", "RIM", "TOM", "CLANK", "SHAKER", "BLIP", "COWBELL"};
struct Kit { const char* name; const char* desc; uint8_t kp, kd, kc, st, sn, ht, crunch, lp, tone, metal; };
static const Kit KITS[] = {
  {"DUST", "lo-fi box, soft and crunchy", 52, 52, 30, 60, 70, 90, 70, 74, 60, 30},
  {"VELVET", "round, deep and gentle", 44, 80, 10, 46, 54, 76, 10, 64, 40, 20},
  {"RUST", "dark, metallic, industrial", 40, 64, 60, 70, 90, 100, 96, 86, 90, 90},
  {"TIN", "tiny vintage rhythm box", 64, 34, 20, 80, 60, 110, 40, 100, 70, 40},
  {"STEEL", "tight, for broken beats", 48, 60, 70, 64, 84, 100, 30, 120, 60, 60},
  {"BOOM", "long analog boom, metallic hats", 36, 110, 20, 54, 70, 96, 20, 110, 50, 100},
  {"GRIT", "punchy, driven, a little dirty", 50, 70, 80, 70, 100, 104, 80, 116, 70, 70}};
constexpr int NKITS = 7;
static const char* TEX_NAMES[7] = {"WIND", "RAIN", "VINYL", "TAPE HISS", "OCEAN", "RADIO", "DEEP HUM"};

// ---- moods
struct Mood { const char* id; const char* name; const char* desc; int bpmLo, bpmHi; const char* scales[3]; const char* drums; int kit, tex;
  const char* p[5]; float dens[6]; int echoMode, echoRate, echoInt, echoWow, cosmos, chordBars; const char* bass; const char* pad; const char* bells; const char* lead; };
static const Mood MOODS[] = {
  {"drift", "DRIFT", "beatless, floating clouds", 66, 82, {"lydian", "major", "penta"}, "none", 1, 0, {"Sub Gravity", "Polar Dawn", "Breath of Io", "Glass Bell", "Sine Flute"}, {.5f, 1, .8f, .7f, .3f, 1}, 9, 76, 62, 50, 110, 4, "drone", "hold", "sparkle", "sparse"},
  {"tidal", "TIDAL", "slow pulse, warm soft beat", 84, 98, {"dorian", "mixo", "major"}, "soft", 1, 4, {"Warm Pulse", "Velvet Orbit", "Hollow Choir", "Electric Haze", "Lonely Probe"}, {1, 1, .5f, .7f, .5f, .8f}, 8, 60, 56, 40, 90, 2, "pulse", "hold", "chime", "sparse"},
  {"haze", "HAZE", "bright nostalgic arps, breakbeat", 100, 118, {"major", "lydian", "mixo"}, "break", 0, 3, {"Rubber Ball", "Gossamer Saw", "Choir of Tides", "Heliotrope", "Heliosphere Arp"}, {1, 1, .6f, 1, .9f, .8f}, 4, 52, 48, 50, 80, 2, "walk", "hold", "arp", "normal"},
  {"glass", "GLASS", "music box arps and frost", 90, 112, {"major", "lydian", "penta"}, "tick", 3, 3, {"Sub Gravity", "Weightless", "Cathedral Air", "Music Box", "Ice Bubble"}, {.7f, .9f, .4f, 1, .8f, .9f}, 10, 54, 50, 30, 96, 2, "drone", "hold", "arp", "sparse"},
  {"pulse", "PULSE", "pulse-width dreams, rubber bass", 104, 124, {"dorian", "aeolian", "mixo"}, "pulse4", 6, 3, {"Rubber Moon", "Pulse Width Dream", "Ghost Vowel", "Tape Piano", "Pulsewidth"}, {1, 1, .4f, .8f, .9f, .6f}, 3, 46, 44, 36, 76, 2, "acid", "pulse", "sparkle", "normal"},
  {"rust", "RUST", "dark drones, clanks, machines", 70, 92, {"phrygian", "insen", "aeolian"}, "clank", 2, 6, {"Rubber Moon", "Saturn Hum", "Ghost Vowel", "Tin Gamelan", "Thin Signal"}, {.9f, 1, .6f, .6f, .3f, 1}, 11, 84, 74, 64, 120, 4, "walk", "hold", "chime", "sparse"},
  {"breath", "BREATH", "choirs and long sighs", 60, 76, {"aeolian", "dorian", "lydian"}, "none", 1, 0, {"Sub Gravity", "Amber Fog", "Breath of Io", "Singing Bowl", "Sine Flute"}, {.6f, .9f, 1, .5f, .2f, .7f}, 9, 90, 60, 54, 120, 4, "drone", "hold", "chime", "sparse"},
  {"lullaby", "LULLABY", "nostalgic melody, lo-fi breaks", 96, 116, {"aeolian", "dorian", "major"}, "break", 0, 3, {"Deep Field", "Paper Moon", "Hollow Choir", "Electric Haze", "Dream Saw"}, {1, 1, .4f, .7f, 1, 1}, 4, 50, 46, 56, 70, 2, "walk", "pulse", "sparkle", "normal"},
  {"orbit", "ORBIT", "polyrhythms, braindance lite", 112, 132, {"dorian", "lydian", "mpenta"}, "idm", 4, 3, {"Acid Dust", "Tidal Glass", "Ghost Vowel", "Clockwork", "Square Kid"}, {1, .8f, .3f, 1, .7f, .5f}, 3, 44, 48, 30, 70, 2, "acid", "hold", "arp", "fast"},
  {"abyss", "ABYSS", "deep, dark, very slow", 54, 70, {"phrygian", "harmmin", "hira"}, "pulse", 1, 6, {"Sub Gravity", "Low Sun", "Cathedral Drone", "Iron Bell", "Thin Signal"}, {1, 1, .7f, .4f, .2f, 1}, 11, 100, 70, 60, 127, 4, "drone", "hold", "chime", "sparse"},
  {"chrome", "CHROME", "cold metal, bowed and grey", 64, 84, {"insen", "phrygian", "whole"}, "clank", 5, 5, {"Sub Gravity", "Grey Water", "Cathedral Drone", "Prepared Piano", "Distant Horn"}, {.7f, 1, .5f, .7f, .3f, 1}, 10, 80, 66, 50, 116, 4, "drone", "hold", "chime", "sparse"},
  {"dawn", "DAWN", "bright, hopeful, sunrise", 88, 104, {"lydian", "major", "mixo"}, "soft", 3, 4, {"Warm Pulse", "Analog Strings", "Choir of Tides", "Comet Pluck", "Lonely Probe"}, {.9f, 1, .6f, .9f, .7f, .7f}, 8, 62, 52, 40, 96, 2, "pulse", "hold", "arp", "normal"},
  {"static", "STATIC", "radio ghosts and textures", 72, 90, {"whole", "hira", "insen"}, "tick", 2, 5, {"Deep Field", "Static Bloom", "Whisper Field", "Satellite", "Wobble Lead"}, {.5f, .8f, .7f, .7f, .4f, 1}, 10, 70, 66, 70, 110, 4, "walk", "hold", "sparkle", "sparse"},
};
constexpr int NMOODS = sizeof(MOODS) / sizeof(MOODS[0]);

// ---- Space Tape: 12 modes like a classic three-head echo (heads bitmask + spring)
struct EchoMode { uint8_t heads, spring; };
static const EchoMode ECHO_MODES[12] = {{1, 0}, {2, 0}, {4, 0}, {6, 0}, {1, 1}, {2, 1}, {4, 1}, {3, 1}, {6, 1}, {5, 1}, {7, 1}, {0, 1}};
static const float HEAD_RATIO[3] = {1.f, 1.94f, 2.9f};
static const float SYNC_DIVS[8] = {0.125f, 0.25f, 0.375f, 1.f / 3.f, 0.5f, 0.75f, 1.f, 1.5f};

// ---- live FX rack (filters and effects you can play)
static const char* FX_NAMES[16] = {"LADDER", "ACID", "MS", "HIGH SWEEP", "BAND", "VOWEL", "ENVELOPE", "NOTCH",
                                   "PHASER", "JET", "ENSEMBLE", "REPEAT", "DUST", "HALF-SPEED", "GATE", "METAL"};
constexpr int NFX = 16;

// ---- host parameters: 8 banks of 8 (the KeyLab / any controller shows them bank by bank)
enum Param {
  // 1 PERFORM
  P_DRIFT, P_HAZE, P_BLOOM, P_GRAVITY, P_AUTO, P_AUTO_AMT, P_ENERGY, P_FX_MIX,
  // 2 SPACE TAPE
  P_ST_MODE, P_ST_RATE, P_ST_INT, P_ST_ECHO, P_ST_REVERB, P_ST_BASS, P_ST_TREBLE, P_ST_INPUT,
  // 3 TAPE + COSMOS
  P_ST_WOW, P_ST_FLUTTER, P_ST_AGE, P_ST_ON, P_COS_SIZE, P_COS_DAMP, P_COS_PRE, P_COS_VOL,
  // 4..6 MIX
  P_VOL1, P_VOL2, P_VOL3, P_VOL4, P_VOL5, P_VOL6, P_VOL7, P_VOL8,
  P_ECH1, P_ECH2, P_ECH3, P_ECH4, P_ECH5, P_ECH6, P_ECH7, P_ECH8,
  P_REV1, P_REV2, P_REV3, P_REV4, P_REV5, P_REV6, P_REV7, P_REV8,
  // 7 SOUND (selected track)
  P_S_CUT, P_S_RES, P_S_ATT, P_S_REL, P_S_WARBLE, P_S_DRIVE, P_S_LEVEL, P_S_ENV,
  // 8 FX + SCENE
  P_FX_TYPE, P_FX_X, P_FX_Y, P_FX_ON, P_SCENE, P_MOOD, P_CHAOS, P_SWING,
  // not in banks (still automatable)
  P_MASTER, P_ST_SYNC, P_ST_DIV, P_ST_FREEZE, P_IN_FILTER, P_IN_RES, P_IN_DRIVE, P_IN_MOTION, P_TRACK, P_KEYMODE,
  P_COUNT
};
struct ParamMeta { const char* name; float def; int steps; };   // def 0..1, steps>0 = stepped
static const ParamMeta PARAMS[P_COUNT] = {
  {"Drift", .24f, 0}, {"Haze", .08f, 0}, {"Bloom", .5f, 0}, {"Gravity", .5f, 0}, {"Auto", 0, 2}, {"Auto Amount", .4f, 0}, {"Energy", .6f, 0}, {"FX Mix", 1, 0},
  {"Tape Mode", 8 / 11.f, 12}, {"Repeat Rate", .55f, 0}, {"Intensity", .47f, 0}, {"Echo Volume", .72f, 0}, {"Reverb Volume", .44f, 0}, {"Tape Bass", .5f, 0}, {"Tape Treble", .47f, 0}, {"Input Volume", .5f, 0},
  {"Wow", .32f, 0}, {"Flutter", .24f, 0}, {"Tape Age", .47f, 0}, {"Echo On", 1, 2}, {"Cosmos Size", .79f, 0}, {"Cosmos Damp", .47f, 0}, {"Cosmos Pre", .24f, 0}, {"Cosmos Volume", .72f, 0},
  {"Drums Vol", .82f, 0}, {"Bass Vol", .79f, 0}, {"Pad Vol", .72f, 0}, {"Voice Vol", .66f, 0}, {"Bells Vol", .66f, 0}, {"Lead Vol", .66f, 0}, {"Texture Vol", .63f, 0}, {"Input Vol", .79f, 0},
  {"Drums Echo", .08f, 0}, {"Bass Echo", .08f, 0}, {"Pad Echo", .24f, 0}, {"Voice Echo", .31f, 0}, {"Bells Echo", .5f, 0}, {"Lead Echo", .44f, 0}, {"Texture Echo", .16f, 0}, {"Input Echo", .55f, 0},
  {"Drums Cosmos", .16f, 0}, {"Bass Cosmos", .16f, 0}, {"Pad Cosmos", .55f, 0}, {"Voice Cosmos", .63f, 0}, {"Bells Cosmos", .55f, 0}, {"Lead Cosmos", .47f, 0}, {"Texture Cosmos", .47f, 0}, {"Input Cosmos", .39f, 0},
  {"Cutoff", .5f, 0}, {"Resonance", .5f, 0}, {"Attack", .5f, 0}, {"Release", .5f, 0}, {"Warble", .5f, 0}, {"Drive", .5f, 0}, {"Level", .5f, 0}, {"Filter Env", .5f, 0},
  {"FX Type", 0, NFX}, {"FX X", .5f, 0}, {"FX Y", .5f, 0}, {"FX On", 0, 2}, {"Scene", 0, 8}, {"Mood", 1.f / (NMOODS - 1), NMOODS}, {"Chaos", .31f, 0}, {"Swing", 0, 0},
  {"Master", .8f, 0}, {"Tape Sync", 0, 2}, {"Tape Division", 4 / 7.f, 8}, {"Tape Freeze", 0, 2}, {"Input Filter", 1, 0}, {"Input Reso", .1f, 0}, {"Input Drive", .1f, 0}, {"Input Motion", 0, 0}, {"Track", 2 / 7.f, 8}, {"Key Follow", 1, 3},
};

} // namespace andro
