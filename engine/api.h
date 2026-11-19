// ANDROMEDA engine · api.h — project bytes (the same layout for web, plugin and firmware) and the command set used by every UI
#pragma once
#include "andromeda.h"

namespace andro {

// ---------------------------------------------------------------- project <-> bytes (little endian, versioned)
struct W { uint8_t* p; int n = 0, cap; void b(int v) { if (n < cap) p[n] = (uint8_t)v; n++; } void s(const char* x, int len) { for (int i = 0; i < len; i++) b(x[i]); } };
struct Rd { const uint8_t* p; int n = 0, len; bool ok = true; int b() { if (n >= len) { ok = false; return 0; } return p[n++]; } int sb() { return (int8_t)b(); } };
constexpr int PROJ_VERSION = 2;
inline int writeProject(const Project& P, const float* params, uint8_t* out, int cap) {
  W w{out, 0, cap}; w.s("AND", 3); w.b(PROJ_VERSION);
  int bpm10 = (int)(P.bpm * 10); w.b(bpm10 & 255); w.b(bpm10 >> 8);
  w.b(P.swing); w.b(P.key); w.b(P.scale); w.b(P.kit); w.b(P.texType); w.b(P.texLvl); w.b(P.texTone); w.b(P.texMotion); w.b(P.cur); w.b(P.mood); w.b(P.chaos); w.b(P.songOn); w.b(P.songLen);
  for (int i = 0; i < 32; i++) { w.b(P.song[i].s); w.b(P.song[i].b); }
  for (int i = 0; i < 8; i++) { w.s(P.patches[i].name, 24); w.s(P.patches[i].cat, 8); for (int k = 0; k < PF_COUNT; k++) w.b(P.patches[i].v[k]); }
  for (int s = 0; s < 8; s++) { const Scene& S = P.scenes[s]; for (int k = 0; k < 4; k++) w.b((uint8_t)S.prog[k]); w.b(S.cb); w.b(S.used); for (int i = 0; i < 8; i++) w.b(S.mutes[i]);
    for (int i = 0; i < 8; i++) { const TrackPat& t = S.t[i]; w.b(t.len); w.b(t.rate); w.b(t.dir); w.b(t.follow); w.b(t.chord); w.b((uint8_t)t.oct);
      for (int q = 0; q < 32; q++) { const Step& x = t.s[q]; w.b(x.on); w.b((uint8_t)x.deg); w.b(x.vel); w.b(x.prob); w.b(x.gate4); w.b((uint8_t)x.oct); w.b(x.rat); }
      for (int l = 0; l < 4; l++) { w.b(t.lanes[l].v); for (int q = 0; q < 32; q++) { const DStep& d = t.lanes[l].s[q]; w.b(d.on); w.b(d.vel); w.b(d.prob); w.b(d.rat); } } } }
  if (params) { w.b(P_COUNT); for (int i = 0; i < P_COUNT; i++) w.b((int)(params[i] * 255.f + .5f)); } else w.b(0);
  return w.n;
}
inline bool readProject(Project& P, float* params, const uint8_t* in, int len) {
  Rd r{in, 0, len}; if (len < 8 || in[0] != 'A' || in[1] != 'N' || in[2] != 'D') return false; r.n = 3; int ver = r.b(); if (ver < 1 || ver > PROJ_VERSION) return false;
  Project Q; int lo = r.b(), hi = r.b(); Q.bpm = clampf((lo | (hi << 8)) / 10.f, 30.f, 220.f);
  Q.swing = (uint8_t)r.b(); Q.key = (uint8_t)(r.b() % 12); Q.scale = (uint8_t)(r.b() % NSCALES); Q.kit = (uint8_t)(r.b() % NKITS); Q.texType = (uint8_t)(r.b() % 7); Q.texLvl = (uint8_t)r.b(); Q.texTone = (uint8_t)r.b(); Q.texMotion = (uint8_t)r.b();
  Q.cur = (uint8_t)(r.b() & 7); Q.mood = (uint8_t)(r.b() % NMOODS); Q.chaos = (uint8_t)r.b(); Q.songOn = (uint8_t)r.b(); Q.songLen = (uint8_t)clampi(r.b(), 0, 32);
  for (int i = 0; i < 32; i++) { Q.song[i].s = (uint8_t)(r.b() & 7); Q.song[i].b = (uint8_t)maxi(1, r.b()); }
  for (int i = 0; i < 8; i++) { for (int k = 0; k < 24; k++) Q.patches[i].name[k] = (char)r.b(); Q.patches[i].name[23] = 0; for (int k = 0; k < 8; k++) Q.patches[i].cat[k] = (char)r.b(); Q.patches[i].cat[7] = 0;
    for (int k = 0; k < PF_COUNT; k++) Q.patches[i].v[k] = (uint8_t)clampi(r.b(), 0, 127); if (Q.patches[i].v[PF_ENG] > 5) Q.patches[i].v[PF_ENG] = 0; }
  for (int s = 0; s < 8; s++) { Scene& S = Q.scenes[s]; for (int k = 0; k < 4; k++) S.prog[k] = (int8_t)r.sb(); S.cb = (uint8_t)clampi(r.b(), 1, 8); S.used = (uint8_t)r.b(); for (int i = 0; i < 8; i++) S.mutes[i] = (uint8_t)(r.b() ? 1 : 0);
    for (int i = 0; i < 8; i++) { TrackPat& t = S.t[i]; t.len = (uint8_t)clampi(r.b(), 1, 32); t.rate = (uint8_t)clampi(r.b(), 0, 4); t.dir = (uint8_t)clampi(r.b(), 0, 4); t.follow = (uint8_t)r.b(); t.chord = (uint8_t)r.b(); t.oct = (int8_t)clampi(r.sb(), -2, 2);
      for (int q = 0; q < 32; q++) { Step& x = t.s[q]; x.on = (uint8_t)r.b(); x.deg = (int8_t)r.sb(); x.vel = (uint8_t)clampi(r.b(), 1, 127); x.prob = (uint8_t)clampi(r.b(), 0, 127); x.gate4 = (uint8_t)clampi(r.b(), 1, 128); x.oct = (int8_t)clampi(r.sb(), -2, 2); x.rat = (uint8_t)clampi(r.b(), 1, 4); }
      for (int l = 0; l < 4; l++) { t.lanes[l].v = (uint8_t)(r.b() % 11); for (int q = 0; q < 32; q++) { DStep& d = t.lanes[l].s[q]; d.on = (uint8_t)r.b(); d.vel = (uint8_t)clampi(r.b(), 1, 127); d.prob = (uint8_t)clampi(r.b(), 0, 127); d.rat = (uint8_t)clampi(r.b(), 1, 4); } } } }
  if (!r.ok) return false;
  int np = r.b(); if (params && r.ok) for (int i = 0; i < np && i < P_COUNT; i++) { int v = r.b(); if (r.ok) params[i] = v / 255.f; }
  P = Q; return true;
}

// ---------------------------------------------------------------- commands (UI -> engine), all integers
enum Cmd {
  C_PLAY = 1, C_STOP, C_BPM10, C_KEY, C_SCALE, C_SWING, C_QUEUE_SCENE, C_SET_SCENE_NOW, C_QUANT,
  C_STEP = 20,        // scene, track, step, on, deg, vel, prob, gate4, oct, rat
  C_DSTEP,            // scene, lane, step, on, vel, prob, rat
  C_TRACK,            // scene, track, len, rate, dir, follow, chord, oct
  C_LANE_VOICE,       // scene, lane, voice
  C_MUTE,             // scene, track, muted
  C_PROG,             // scene, idx, deg
  C_CB,               // scene, bars
  C_CLEAR_TRACK,      // scene, track
  C_COPY_SCENE,       // from, to
  C_CLEAR_SCENE,      // scene
  C_PATCH_VAL = 40,   // track, field, value
  C_PRESET,           // track, preset index
  C_KIT, C_TEX_TYPE, C_TEX_LVL, C_TEX_TONE, C_TEX_MOTION,
  C_ALIGN = 60,       // mood, keep flags, song(0/1)
  C_GEN_TRACK,        // track
  C_MUTATE,           // track, amount 0..127
  C_SEED_RETURN,      // scene (bring every part back to your pattern)
  C_MOOD, C_CHAOS,
  C_SONG_LEN = 70, C_SONG_ENTRY, C_SONG_ON,
  C_NOTE = 80,        // track, midi, vel (0 = off)
  C_MIDI,             // status, d1, d2
  C_PERF,             // id, down
  C_LEARN,            // param id (-1 cancel)
  C_PAD_CHANNEL,      // channel 0..15
  C_FILE_PLAY,        // on, gain 0..127
  C_NEW_PROJECT,
};
inline void command(Engine& E, int op, const int* a) {
  Project& P = E.PJ; auto sc = [&](int s) -> Scene& { return P.scenes[s & 7]; };
  switch (op) {
    case C_PLAY: E.play(); E.ownPlay = true; break; case C_STOP: E.stop(); E.ownPlay = false; break;
    case C_BPM10: P.bpm = clampf(a[0] / 10.f, 30.f, 220.f); break;
    case C_KEY: P.key = (uint8_t)(((a[0] % 12) + 12) % 12); E.projVersion++; break;
    case C_SCALE: P.scale = (uint8_t)clampi(a[0], 0, NSCALES - 1); E.projVersion++; break;
    case C_SWING: P.swing = (uint8_t)clampi(a[0], 0, 127); E.params[P_SWING] = P.swing / 127.f; break;
    case C_QUEUE_SCENE: E.queueScene(a[0]); break;
    case C_SET_SCENE_NOW: E.switchScene(a[0]); break;
    case C_QUANT: E.quant = clampi(a[0], 1, 4); break;
    case C_STEP: { Step& x = sc(a[0]).t[a[1] & 7].s[a[2] & 31]; x.on = (uint8_t)a[3]; x.deg = (int8_t)clampi(a[4], -21, 28); x.vel = (uint8_t)clampi(a[5], 1, 127); x.prob = (uint8_t)clampi(a[6], 0, 127); x.gate4 = (uint8_t)clampi(a[7], 1, 128); x.oct = (int8_t)clampi(a[8], -2, 2); x.rat = (uint8_t)clampi(a[9], 1, 4);
      E.seed[a[0] & 7][a[1] & 7] = sc(a[0]).t[a[1] & 7]; sc(a[0]).used = 1; break; }
    case C_DSTEP: { DStep& d = sc(a[0]).t[0].lanes[a[1] & 3].s[a[2] & 31]; d.on = (uint8_t)a[3]; d.vel = (uint8_t)clampi(a[4], 1, 127); d.prob = (uint8_t)clampi(a[5], 0, 127); d.rat = (uint8_t)clampi(a[6], 1, 4); E.seed[a[0] & 7][0] = sc(a[0]).t[0]; sc(a[0]).used = 1; break; }
    case C_TRACK: { TrackPat& t = sc(a[0]).t[a[1] & 7]; t.len = (uint8_t)clampi(a[2], 1, 32); t.rate = (uint8_t)clampi(a[3], 0, 4); t.dir = (uint8_t)clampi(a[4], 0, 4); t.follow = (uint8_t)(a[5] != 0); t.chord = (uint8_t)(a[6] != 0); t.oct = (int8_t)clampi(a[7], -2, 2); E.seed[a[0] & 7][a[1] & 7] = t; break; }
    case C_LANE_VOICE: sc(a[0]).t[0].lanes[a[1] & 3].v = (uint8_t)clampi(a[2], 0, 10); E.seed[a[0] & 7][0] = sc(a[0]).t[0]; break;
    case C_MUTE: sc(a[0]).mutes[a[1] & 7] = (uint8_t)(a[2] != 0); break;
    case C_PROG: sc(a[0]).prog[a[1] & 3] = (int8_t)clampi(a[2], -7, 7); break;
    case C_CB: sc(a[0]).cb = (uint8_t)clampi(a[1], 1, 8); break;
    case C_CLEAR_TRACK: clearTrack(sc(a[0]).t[a[1] & 7]); E.seed[a[0] & 7][a[1] & 7] = sc(a[0]).t[a[1] & 7]; break;
    case C_COPY_SCENE: P.scenes[a[1] & 7] = P.scenes[a[0] & 7]; for (int i = 0; i < 8; i++) E.seed[a[1] & 7][i] = E.seed[a[0] & 7][i]; break;
    case C_CLEAR_SCENE: { Scene& S = sc(a[0]); S = Scene(); for (int i = 0; i < 8; i++) { E.defaultTrack(S.t[i], i); E.seed[a[0] & 7][i] = S.t[i]; } break; }
    case C_PATCH_VAL: P.patches[a[0] & 7].v[clampi(a[1], 0, PF_COUNT - 1)] = (uint8_t)clampi(a[2], 0, 127); if (a[1] != PF_ENG) {} E.syncSoundParams(); break;
    case C_PRESET: { const PresetBank& B = presets(); P.patches[a[0] & 7] = B.p[clampi(a[1], 0, B.n - 1)]; E.syncSoundParams(); E.projVersion++; break; }
    case C_KIT: P.kit = (uint8_t)clampi(a[0], 0, NKITS - 1); break;
    case C_TEX_TYPE: P.texType = (uint8_t)clampi(a[0], 0, 6); break;
    case C_TEX_LVL: P.texLvl = (uint8_t)clampi(a[0], 0, 127); break; case C_TEX_TONE: P.texTone = (uint8_t)clampi(a[0], 0, 127); break; case C_TEX_MOTION: P.texMotion = (uint8_t)clampi(a[0], 0, 127); break;
    case C_ALIGN: E.align(a[0], a[1], a[2] != 0); break;
    case C_GEN_TRACK: E.genTrack(a[0] & 7); break;
    case C_MUTATE: E.museVary(P.cur, a[0] & 7, a[1] / 127.f); E.projVersion++; break;
    case C_SEED_RETURN: for (int i = 0; i < 8; i++) sc(a[0]).t[i] = E.seed[a[0] & 7][i]; E.projVersion++; break;
    case C_MOOD: P.mood = (uint8_t)clampi(a[0], 0, NMOODS - 1); E.params[P_MOOD] = P.mood / (float)(NMOODS - 1); break;
    case C_CHAOS: P.chaos = (uint8_t)clampi(a[0], 0, 127); E.params[P_CHAOS] = P.chaos / 127.f; break;
    case C_SONG_LEN: P.songLen = (uint8_t)clampi(a[0], 0, 32); break;
    case C_SONG_ENTRY: P.song[a[0] & 31] = {(uint8_t)(a[1] & 7), (uint8_t)clampi(a[2], 1, 128)}; break;
    case C_SONG_ON: P.songOn = (uint8_t)(a[0] != 0); E.songIdx = 0; E.songBar = 0; break;
    case C_NOTE: E.liveNote(a[0], a[1] & 127, a[2]); break;
    case C_MIDI: E.midi((uint8_t)a[0], (uint8_t)a[1], (uint8_t)a[2]); break;
    case C_PERF: E.perf(a[0], a[1] != 0); break;
    case C_LEARN: E.learnParam = a[0]; break;
    case C_PAD_CHANNEL: E.padChannel = clampi(a[0], 0, 15); break;
    case C_FILE_PLAY: E.filePlay = a[0] != 0; E.fileGain = a[1] / 100.f; if (!E.filePlay) E.filePos = 0; break;
    case C_NEW_PROJECT: E.newProject(); break;
  }
}

} // namespace andro
