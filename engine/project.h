// ANDROMEDA engine · project.h — patterns, scenes, presets, generators (moods) and the MUSE (auto-composer)
#pragma once
#include "fx.h"

namespace andro {

struct Step { uint8_t on = 0, vel = 100, prob = 127, gate4 = 4, rat = 1; int8_t deg = 0, oct = 0; };
struct DStep { uint8_t on = 0, vel = 100, prob = 127, rat = 1; };
struct Lane { uint8_t v = 0; DStep s[32]; };
struct TrackPat { uint8_t len = 16, rate = 3, dir = 0, follow = 1, chord = 0; int8_t oct = 0; Step s[32]; Lane lanes[4]; };
struct Scene { int8_t prog[4] = {0, 5, 3, 4}; uint8_t cb = 2, used = 0; uint8_t mutes[8] = {0, 0, 0, 0, 0, 0, 0, 0}; TrackPat t[8]; };
struct SongEntry { uint8_t s = 0, b = 8; };
struct Project {
  float bpm = 84; uint8_t swing = 0, key = 2, scale = 0, kit = 1, texType = 0, texLvl = 90, texTone = 64, texMotion = 60, cur = 0, mood = 1, chaos = 40, songOn = 0, songLen = 1;
  SongEntry song[32]; Patch patches[8]; Scene scenes[8];
};

// ---------------------------------------------------------------- presets (parsed once from the text table)
struct PresetBank { Patch p[NPRESETS]; int n = 0;
  static int toInt(const char*& s) { int v = 0, neg = 0; if (*s == '-') { neg = 1; s++; } while (*s >= '0' && *s <= '9') v = v * 10 + (*s++ - '0'); return neg ? -v : v; }
  static bool eq(const char* a, const char* b, int n) { for (int i = 0; i < n; i++) if (a[i] != b[i]) return false; return b[n] == 0; }
  PresetBank() {
    for (int i = 0; i < NPRESETS; i++) {
      Patch& P = p[n++]; for (int k = 0; k < PF_COUNT; k++) P.v[k] = PF_DEF[k];
      const char* s = PRESET_SRC[i]; int j = 0; while (*s && *s != '|' && j < 23) P.name[j++] = *s++; P.name[j] = 0; while (*s && *s != '|') s++; s++;
      j = 0; while (*s && *s != '|' && j < 7) P.cat[j++] = *s++; P.cat[j] = 0; s++;
      const char* e = s; while (*e && *e != '|') e++; int el = (int)(e - s);
      static const char* EN[6] = {"pad", "choir", "fm", "bass", "lead", "bowed"}; for (int k = 0; k < 6; k++) if (eq(s, EN[k], el)) P.v[PF_ENG] = (uint8_t)k;
      s = *e ? e + 1 : e;
      while (*s) { while (*s == ' ') s++; const char* kb = s; while (*s && *s != '=') s++; int kl = (int)(s - kb); if (!*s) break; s++; int v = toInt(s);
        for (int k = 0; k < PF_COUNT; k++) if (eq(kb, PF_NAMES[k], kl)) P.v[k] = (uint8_t)clampi(v, 0, 127); }
    }
  }
  int find(const char* name) const { for (int i = 0; i < n; i++) { const char* a = p[i].name; const char* b = name; while (*a && *a == *b) { a++; b++; } if (!*a && !*b) return i; } return 0; }
};
inline const PresetBank& presets() { static PresetBank B; return B; }

// ---------------------------------------------------------------- helpers
inline void clearTrack(TrackPat& t) { for (auto& s : t.s) s = Step(); for (auto& l : t.lanes) for (auto& d : l.s) d = DStep(); }
inline void setStep(TrackPat& t, int i, int deg, int gate4 = 4, int vel = 100, int prob = 127, int oct = 0) { Step& s = t.s[i & 31]; s.on = 1; s.deg = (int8_t)deg; s.gate4 = (uint8_t)clampi(gate4, 1, 128); s.vel = (uint8_t)vel; s.prob = (uint8_t)prob; s.oct = (int8_t)oct; s.rat = 1; }
inline void dOn(TrackPat& t, int l, int i, int vel = 100, int prob = 127, int rat = 1) { DStep& d = t.lanes[l].s[i & 31]; d.on = 1; d.vel = (uint8_t)clampi(vel, 1, 127); d.prob = (uint8_t)prob; d.rat = (uint8_t)rat; }
inline int euclidHit(int k, int n, int rot, int i) { int j = ((i - rot) % n + n) % n; return ((j * k) % n) < k; }
inline int scaleIndex(const char* id) { for (int i = 0; i < NSCALES; i++) { const char* a = SCALES[i].id; const char* b = id; while (*a && *a == *b) { a++; b++; } if (!*a && !*b) return i; } return 0; }
static const int8_t PROGS[12][4] = {{0, 3, 0, 4}, {0, 5, 3, 4}, {0, 2, 3, 1}, {0, 3, 5, 3}, {5, 3, 0, 4}, {0, -1, -2, -3}, {0, 4, 5, 3}, {0, 0, 3, 3}, {0, 6, 5, 3}, {0, 3, 1, 4}, {3, 4, 0, 0}, {0, 1, 0, -1}};
inline bool streq(const char* a, const char* b) { while (*a && *a == *b) { a++; b++; } return *a == *b; }

// ---------------------------------------------------------------- generators (one per part, styles from the moods)
struct Gen {
  Rng& r; explicit Gen(Rng& rr) : r(rr) {}
  void drums(TrackPat& t, const char* style, float ch) {
    clearTrack(t); t.rate = 4; t.len = 16;
    int lanes[4] = {0, 1, 2, 5};
    if (streq(style, "soft")) { int v[4] = {0, 1, 2, 8}; for (int i = 0; i < 4; i++) lanes[i] = v[i]; }
    else if (streq(style, "tick")) { int v[4] = {5, 2, 9, 8}; for (int i = 0; i < 4; i++) lanes[i] = v[i]; }
    else if (streq(style, "clank")) { int v[4] = {0, 7, 2, 6}; for (int i = 0; i < 4; i++) lanes[i] = v[i]; }
    else if (streq(style, "break")) { int v[4] = {0, 1, 2, 3}; for (int i = 0; i < 4; i++) lanes[i] = v[i]; }
    else if (streq(style, "idm")) { int v[4] = {0, 1, 2, 9}; for (int i = 0; i < 4; i++) lanes[i] = v[i]; }
    else if (streq(style, "pulse")) { int v[4] = {0, 2, 8, 5}; for (int i = 0; i < 4; i++) lanes[i] = v[i]; }
    else if (streq(style, "pulse4")) { int v[4] = {0, 4, 2, 3}; for (int i = 0; i < 4; i++) lanes[i] = v[i]; }
    for (int i = 0; i < 4; i++) t.lanes[i].v = (uint8_t)lanes[i];
    auto j = [&]() { return (int)(r.next() * ch * 30); };
    if (streq(style, "none")) return;
    if (streq(style, "soft")) { dOn(t, 0, 0, 110); dOn(t, 0, 8, 80, 90); dOn(t, 0, 10, 70, 50 + (int)(ch * 40)); dOn(t, 1, 4, 64); dOn(t, 1, 12, 70); for (int i = 0; i < 16; i += 2) dOn(t, 2, i, 50 + (i % 4 ? 0 : 20) + j(), 110); for (int i = 1; i < 16; i += 2) if (r.chance(.3f + ch * .3f)) dOn(t, 3, i, 40, 70); }
    if (streq(style, "tick")) { int a = r.range(0, 3), b = r.range(0, 7); for (int i = 0; i < 16; i++) { if (euclidHit(5, 16, a, i)) dOn(t, 0, i, 70 + j(), 100); if (euclidHit(7, 16, 2, i)) dOn(t, 1, i, 50, 90); if (euclidHit(3, 16, b, i)) dOn(t, 2, i, 60, 70); if (r.chance(.25f)) dOn(t, 3, i, 35, 80); } }
    if (streq(style, "clank")) { dOn(t, 0, 0, 120); dOn(t, 0, 7, 90, 70); dOn(t, 0, 10, 100); dOn(t, 1, 4, 90); dOn(t, 1, 12, 100); for (int i = 0; i < 16; i++) if (euclidHit(3, 16, 5, i) && !t.lanes[1].s[i].on) dOn(t, 1, i, 60, 50); for (int i = 2; i < 16; i += 4) dOn(t, 2, i, 60, 100); if (r.chance(.6f)) dOn(t, 3, 14, 80, 60); }
    if (streq(style, "break")) { dOn(t, 0, 0, 115); dOn(t, 0, 7, 85); dOn(t, 0, 10, 100); if (r.chance(ch)) dOn(t, 0, 13, 70, 60); dOn(t, 1, 4, 100); dOn(t, 1, 12, 105); dOn(t, 1, 9, 40, 70); dOn(t, 1, 15, 35, 40); for (int i = 0; i < 16; i += 2) dOn(t, 2, i, 70 + (i % 4 ? -20 : 0) + j()); dOn(t, 3, 14, 60, 60); }
    if (streq(style, "idm")) { int a = r.range(0, 3), b = r.range(0, 4), c = r.range(0, 6); for (int i = 0; i < 16; i++) { if (euclidHit(5, 16, a, i)) dOn(t, 0, i, 110); if (euclidHit(3, 16, 4, i)) dOn(t, 1, i, 100); if (euclidHit(11, 16, b, i)) dOn(t, 2, i, 60 + j(), 100, r.chance(.15f + ch * .2f) ? r.range(2, 4) : 1); if (euclidHit(7, 16, c, i)) dOn(t, 3, i, 80, 60 + (int)(ch * 50)); } }
    if (streq(style, "pulse")) { dOn(t, 0, 0, 110); dOn(t, 0, 3, 70); dOn(t, 0, 8, 100, 90); for (int i = 2; i < 16; i += 4) dOn(t, 1, i, 40); for (int i = 0; i < 16; i++) if (r.chance(.2f)) dOn(t, 2, i, 30, 90); if (r.chance(.5f)) dOn(t, 3, 12, 60, 70); }
    if (streq(style, "pulse4")) { for (int i = 0; i < 16; i += 4) dOn(t, 0, i, 104); dOn(t, 1, 4, 80); dOn(t, 1, 12, 86); for (int i = 2; i < 16; i += 4) dOn(t, 3, i, 60, 100); for (int i = 0; i < 16; i++) if (i % 4 != 2 && r.chance(.6f)) dOn(t, 2, i, 40 + j(), 100); }
  }
  void bass(TrackPat& t, const char* st, float ch) {
    clearTrack(t); t.follow = 1; t.chord = 0; t.oct = 0; t.dir = 0;
    if (streq(st, "drone")) { t.rate = 0; t.len = 1; setStep(t, 0, 0, 4); return; }
    if (streq(st, "pulse")) { t.rate = 3; t.len = 16; for (int n = 0; n < 4; n++) setStep(t, n * 4, n == 3 && r.chance(.5f) ? 4 : 0, n == 0 ? 12 : 8, n == 0 ? 110 : 80); if (r.chance(ch)) setStep(t, 14, 4, 4, 70, 80); return; }
    if (streq(st, "walk")) { t.rate = 3; t.len = 16; const int D[8] = {0, 0, 4, 2, -3, 0, 5, -1}; const int P[6] = {0, 3, 6, 8, 11, 14}; for (int n = 0; n < 6; n++) if (n == 0 || r.chance(.7f)) setStep(t, P[n], n == 0 ? 0 : D[r.range(0, 7)], 4 * r.range(1, 3), 90 + r.range(-10, 10)); return; }
    t.rate = 4; t.len = 16; const int D[9] = {0, 0, 7, 0, 3, 4, -3, 0, 2};
    for (int i = 0; i < 16; i++) if (i == 0 || r.chance(.45f + ch * .2f)) setStep(t, i, i == 0 ? 0 : D[r.range(0, 8)], r.chance(.25f) ? 8 : 4, r.chance(.3f) ? 120 : 85, i == 0 ? 127 : 100, r.chance(.15f) ? 1 : 0);
  }
  void pad(TrackPat& t, int cb, float ch, const char* st) {
    clearTrack(t); t.follow = 1; t.chord = 1; t.oct = 0; t.dir = 0;
    if (streq(st, "pulse")) { t.rate = 2; t.len = (uint8_t)(4 * cb); for (int i = 0; i < t.len; i += 2) setStep(t, i, 0, 6, i % 4 ? 80 : 100); return; }
    t.rate = 0; t.len = (uint8_t)cb; setStep(t, 0, 0, 4 * cb, r.chance(ch * .5f) ? 115 : 100);
  }
  void voice(TrackPat& t, int cb) {
    clearTrack(t); t.follow = 1; t.chord = 0; t.oct = 0; t.rate = 0; t.len = (uint8_t)(cb * 2); t.dir = 0;
    const int A[3] = {4, 2, 7}, B[3] = {2, 4, 6}; setStep(t, 0, A[r.range(0, 2)], 4 * cb, 90); if (r.chance(.6f)) setStep(t, cb, B[r.range(0, 2)], 4 * cb, 80, 100);
  }
  void arp(TrackPat& t, const char* st, float ch) {
    clearTrack(t); t.follow = 1; t.chord = 0; t.oct = 0; t.dir = 0; const int T[7] = {0, 2, 4, 7, 9, 11, 14};
    if (streq(st, "arp")) { t.rate = r.chance(.5f) ? 4 : 3; const int L[4] = {12, 16, 14, 10}; t.len = (uint8_t)L[r.range(0, 3)];
      static const int PAT[5][4] = {{0, 1, 2, 3}, {0, 2, 1, 3}, {3, 2, 1, 0}, {0, 1, 2, 1}, {0, 2, 4, 2}}; int pi = r.range(0, 4);
      for (int i = 0; i < t.len; i++) if (r.chance(.75f)) setStep(t, i, T[clampi(PAT[pi][i % 4] + ((i / 4) % 2), 0, 6)], 4, 60 + r.range(0, 40), 90 + r.range(0, 37)); return; }
    if (streq(st, "sparkle")) { t.rate = 3; const int L[3] = {16, 13, 11}; t.len = (uint8_t)L[r.range(0, 2)]; for (int i = 0; i < t.len; i++) if (r.chance(.3f + ch * .2f)) setStep(t, i, T[r.range(0, 6)] + (r.chance(.3f) ? 7 : 0), 4, 50 + r.range(0, 50), 70 + r.range(0, 57)); return; }
    t.rate = 2; t.len = 16; for (int i = 0; i < 16; i += 4) if (r.chance(.55f)) setStep(t, i + r.range(0, 1), T[r.range(0, 6)], 12, 70 + r.range(0, 30));
  }
  void lead(TrackPat& t, const char* st, float ch) {
    clearTrack(t); t.follow = 1; t.chord = 0; t.oct = 0; t.dir = 0; t.rate = streq(st, "fast") ? 4 : 3; t.len = 16;
    const int strong[4] = {0, 2, 4, 7}; int motif[8]; int d = 2 * r.range(0, 2);
    for (int i = 0; i < 8; i++) {
      if (i > 0 && r.chance(streq(st, "sparse") ? .55f : .3f)) { motif[i] = -99; continue; }
      const int mv[8] = {-2, -1, -1, 1, 1, 2, 0, 3}; d += mv[r.range(0, 7)] * (r.chance(ch) ? 2 : 1); d = clampi(d, -3, 9);
      if (i % 2 == 0 && r.chance(.7f)) { int best = 0; for (int s : strong) if (absf((float)(s - d)) < absf((float)(best - d))) best = s; d = best; }
      motif[i] = d;
    }
    for (int i = 0; i < 16; i++) { int x = i < 8 ? motif[i] : motif[i - 8]; if (x == -99) continue; if (i >= 8) x = i >= 14 ? strong[r.range(0, 3)] : x + (r.chance(.3f) ? (r.chance(.5f) ? -1 : 1) : 0);
      setStep(t, i, x, r.chance(.35f) ? 4 * r.range(2, 3) : 4, 80 + r.range(-15, 25), r.chance(ch * .5f) ? 90 : 127); }
  }
  void tex(TrackPat& t, int cb) { clearTrack(t); t.rate = 0; t.len = (uint8_t)(cb * 2); setStep(t, 0, 0, 4, 90); if (r.chance(.5f)) setStep(t, cb, 0, 4, 70); }
};

} // namespace andro
