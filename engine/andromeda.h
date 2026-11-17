// ANDROMEDA engine · andromeda.h — the instrument: transport, harmony, voices, mix, MUSE, live play, state
// One engine for the web (WebAssembly), the VST3/AU plugin and the FM-1 firmware.
// an ambient instrument · Dark Funk Durcal design · Polaris Sound Lab · Mérida, Yucatán, México
#pragma once
#include "project.h"

namespace andro {

constexpr int SUB = 32;                 // control block: events and smoothing run every 32 samples
constexpr int MAXV = 12, MAXD = 20, NEV = 512, NUI = 512;

enum Perf { PERF_FREEZE = 0, PERF_STOP, PERF_SPIN, PERF_FILL, PERF_NOVA, PERF_ECLIPSE, PERF_KICK, PERF_ALIGN, PERF_AUTO, PERF_ALIGN_SONG, PERF_SPIN_BACK };
enum UiEv { UI_NOTE = 1, UI_BAR, UI_SCENE, UI_MOD, UI_NOVA, UI_EVOLVE, UI_PROJECT, UI_PERF };
enum KeyMode { KM_RAW = 0, KM_SNAP, KM_SHIFT };

struct TrackRT {
  SynthVoice v[MAXV]; DrumVoice d[MAXD]; float buf[SUB];
  Smooth vol, pan, e, r, mute; SVF fmt[3]; Smooth fq[3]; Ensemble ens; OnePole dlp; float crushH = 0; int crushC = 0;
  float lastF = 0; float lastEnd = 0; int walk = 0; int voicing[4] = {0, 0, 0, 0}; int nVoicing = 0; float flare = 0;
};

struct Engine {
  // ----- state
  float sr = 48000; float params[P_COUNT]; Project PJ; TrackPat seed[8][8]; Rng rng; uint32_t seedCtr = 1;
  TrackRT T[8]; Texture tex; SpaceTape st; Cosmos cos; FxRack fx; Master mst;
  // transport
  bool playing = false, hostSync = false; double hostPpq = 0; float hostBpm = 120; bool hostPlaying = false;
  long tick = 0, sceneT = 0; double toNext = 0; int barInScene = 0, chordIdx = 0, chordDeg = 0, queued = -1, quant = 1, songIdx = 0, songBar = 0;
  bool fill = false; long fillEnd = 0; int novaLeft = 0, novaLen = 0; float novaPh = 0; SVF novaF; float boom = 0, boomPh = 0; bool eclipse = false;
  int pendKey = -1, pendScale = -1; bool reharm = false; int motif[8]; int motifN = 0; int lastLive[128];
  bool fxWant = false; int fxStartIn = -1; float lastFxOn = 0; bool autoArr = true; int sinceUser = 0;
  // events (note-offs and swung notes)
  struct Ev { int at; uint8_t type, tr; int16_t note; uint8_t vel; int8_t voice; float dur; uint8_t idx; } ev[NEV]; int nev = 0;
  // ui telemetry
  struct UiE { int32_t type, a, b, c; } ui[NUI]; int uiW = 0;
  float tele[48]; float scope[1024 * 2]; int scopeW = 0; float rmsL = 0, rmsR = 0, low = 0, high = 0; OnePole lowF, highF; int projVersion = 1;
  // input (host audio in or a loaded file)
  const float* fileL = nullptr; const float* fileR = nullptr; int fileLen = 0; double filePos = 0; bool filePlay = false; float fileGain = 1;
  SVF inF[2]; Smooth inGate; float inMot = 0; float inPeak = 0;
  // midi learn
  int16_t ccMap[16][128]; int learnParam = -1; int padChannel = 9;
  float lph[3] = {0, 0, 0};

  // ============================================================ setup
  void init(float rate) {
    sr = rate; for (int i = 0; i < P_COUNT; i++) params[i] = PARAMS[i].def;
    for (auto& c : ccMap) for (auto& x : c) x = -1;
    for (auto& t : T) { t.vol.time(0.05f, sr); t.pan.time(0.05f, sr); t.e.time(0.05f, sr); t.r.time(0.05f, sr); t.mute.time(0.6f, sr); t.mute.snap(1); for (auto& f : t.fq) f.time(0.2f, sr); t.ens.sr = sr; t.dlp.setLP(12000, sr); for (auto& v : t.v) { v.amp.setSR(sr); } }
    tex.sr = sr; st.init(sr); cos.init(sr); fx.init(sr); mst.init(sr); inGate.time(0.004f, sr); inGate.snap(1); lowF.setLP(150, sr); highF.setLP(4000, sr);
    for (auto& x : lastLive) x = -1;
    mst.vol.snap(params[P_MASTER]); newProject(); align(PJ.mood, 0, true);
  }
  void newProject() {
    PJ = Project(); const PresetBank& B = presets(); const char* def[8] = {nullptr, "Deep Field", "Polar Dawn", "Breath of Io", "Glass Bell", "Lonely Probe", nullptr, nullptr};
    for (int i = 0; i < 8; i++) PJ.patches[i] = B.p[def[i] ? B.find(def[i]) : 0];
    for (auto& s : PJ.scenes) { s = Scene(); for (int i = 0; i < 8; i++) defaultTrack(s.t[i], i); }
    PJ.song[0] = {0, 8}; PJ.songLen = 1; snapshotSeeds(); projVersion++;
  }
  void defaultTrack(TrackPat& t, int i) { t = TrackPat(); t.rate = (uint8_t)(i == 2 || i == 3 || i == 6 || i == 7 ? 1 : i == 1 ? 2 : i == 0 ? 4 : 3); t.follow = (i == 4 || i == 5) ? 0 : 1; t.chord = i == 2; const int L[4] = {0, 1, 2, 5}; for (int l = 0; l < 4; l++) t.lanes[l].v = (uint8_t)L[l]; }
  void snapshotSeeds() { for (int s = 0; s < 8; s++) for (int i = 0; i < 8; i++) seed[s][i] = PJ.scenes[s].t[i]; }
  void push(int type, int a = 0, int b = 0, int c = 0) { ui[uiW % NUI] = {type, a, b, c}; uiW++; }
  Scene& SC() { return PJ.scenes[PJ.cur & 7]; }
  float p(int id) const { return params[id]; }
  int pStep(int id) const { int n = PARAMS[id].steps; return n > 1 ? clampi((int)(params[id] * (n - 1) + 0.5f), 0, n - 1) : 0; }
  void setParam(int id, float v) { if (id < 0 || id >= P_COUNT) return; v = clampf(v, 0, 1); float old = params[id]; params[id] = v; onParam(id, old); }
  float samplesPer16() const { return 60.f / (hostSync ? hostBpm : PJ.bpm) / 4.f * sr; }
  float beatSec() const { return 60.f / (hostSync ? hostBpm : PJ.bpm); }

  // ============================================================ harmony
  const Scale& scale() const { return SCALES[PJ.scale % NSCALES]; }
  int keyRoot(int i) const { int k = PJ.key; return TRACKS[i].base + (k > 6 ? k - 12 : k); }
  int degToMidi(int i, int deg, int oct) const { int m = keyRoot(i) + 12 * oct + scaleNote(scale(), deg); while (m > TRACKS[i].hi) m -= 12; while (m < TRACKS[i].lo) m += 12; return m; }
  int snapToScale(int midi) const { for (int d = 0; d < 7; d++) { if (inScale(scale(), PJ.key, midi - d)) return midi - d; if (inScale(scale(), PJ.key, midi + d)) return midi + d; } return midi; }
  // nearest scale (same type first, then sister modes) that holds this pitch class and shares the most notes with the current one
  void findShift(int pc, int& key, int& sc) {
    const Scale& cur = scale(); int best = -1, bk = PJ.key, bs = PJ.scale;
    for (int s = 0; s < NSCALES; s++) { if (SCALES[s].n != cur.n && s != PJ.scale) continue;
      for (int k = 0; k < 12; k++) { if (!inScale(SCALES[s], k, pc)) continue; int common = 0; for (int q = 0; q < 12; q++) if (inScale(SCALES[s], k, q) && inScale(cur, PJ.key, q)) common++;
        int score = common * 10 - (s == PJ.scale ? 0 : 3) - (int)absf((float)(((k - PJ.key) * 7 % 12 + 18) % 12 - 6)); if (score > best) { best = score; bk = k; bs = s; } } }
    key = bk; sc = bs;
  }
  // chord voicing with smooth voice leading (each note moves as little as possible)
  int voiceChord(int tr, int deg, int oct, int sevenths, int* out) {
    int n = sevenths ? 4 : 3; int center = 0; TrackRT& R = T[tr];
    if (R.nVoicing) { for (int i = 0; i < R.nVoicing; i++) center += R.voicing[i]; center /= R.nVoicing; } else center = keyRoot(tr) + 12 * oct + 5;
    for (int k = 0; k < n; k++) { int m = degToMidi(tr, deg + 2 * k, oct); while (m - center > 6) m -= 12; while (center - m > 6) m += 12; out[k] = clampi(m, TRACKS[tr].lo, TRACKS[tr].hi); }
    for (int a = 0; a < n; a++) for (int b = a + 1; b < n; b++) if (out[b] < out[a]) { int t = out[a]; out[a] = out[b]; out[b] = t; }
    for (int k = 1; k < n; k++) if (out[k] == out[k - 1]) out[k] += 12;
    R.nVoicing = n; for (int k = 0; k < n; k++) R.voicing[k] = out[k];
    return n;
  }

  // ============================================================ voices
  int noteOn(int tr, int midi, float vel, int nIdx = 0) {
    const TrackDef& td = TRACKS[tr]; if (td.kind != K_SYNTH) return -1; TrackRT& R = T[tr]; const Patch& P = PJ.patches[tr];
    int eng = P.v[PF_ENG]; bool mono = eng == E_BASS || eng == E_LEAD; int maxv = mono ? 2 : (tr == 2 ? 8 : 6);
    int act = 0, oldest = -1, oa = -1; for (int i = 0; i < MAXV; i++) if (R.v[i].on) { act++; if (R.v[i].age > oa && R.v[i].amp.st != Env::FAST) { oa = R.v[i].age; oldest = i; } }
    if (act >= maxv && oldest >= 0) R.v[oldest].kill();          // 4 ms fade, never a click
    if (mono) for (int i = 0; i < MAXV; i++) if (R.v[i].on && R.v[i].amp.st != Env::FAST && R.v[i].amp.st != Env::REL) R.v[i].release();
    int slot = -1; for (int i = 0; i < MAXV; i++) if (!R.v[i].on) { slot = i; break; }
    if (slot < 0) { float q = 9; for (int i = 0; i < MAXV; i++) if (R.v[i].amp.v < q) { q = R.v[i].amp.v; slot = i; } }
    float glideFrom = mono && P.v[PF_GLIDE] > 0 && R.lastF > 0 ? R.lastF : 0;
    R.v[slot].start(P.v, midi, vel, sr, nIdx, glideFrom, (seedCtr++) * 2654435761u); R.lastF = R.v[slot].f0; R.flare = 1;
    return slot;
  }
  void noteOffSlot(int tr, int slot, int midi) { if (slot < 0) return; SynthVoice& v = T[tr].v[slot]; if (v.on && v.note == midi) v.release(); }
  void drumHit(int voice, float vel) { TrackRT& R = T[0]; int slot = -1, oldest = 0; float ot = -1; for (int i = 0; i < MAXD; i++) { if (!R.d[i].on) { slot = i; break; } if (R.d[i].t > ot) { ot = R.d[i].t; oldest = i; } }
    if (slot < 0) slot = oldest; R.d[slot].start(clampi(voice, 0, 10), &KITS[PJ.kit % NKITS], vel, sr, (seedCtr++) * 2246822519u); R.flare = 1; }
  void addEv(int at, int type, int tr, int note, int vel, int voice, float dur, int idx = 0) { if (nev >= NEV) return; ev[nev++] = {at, (uint8_t)type, (uint8_t)tr, (int16_t)note, (uint8_t)vel, (int8_t)voice, dur, (uint8_t)idx}; }

  // ============================================================ transport
  void play() { if (playing) return; playing = true; tick = 0; sceneT = 0; barInScene = 0; toNext = 0; songIdx = 0; songBar = 0; for (auto& t : T) t.walk = 0; if (PJ.songOn && PJ.songLen) switchScene(PJ.song[0].s); }
  void stop() { playing = false; queued = -1; nev = 0; for (auto& t : T) for (auto& v : t.v) if (v.on) v.release(); }
  void switchScene(int n) { PJ.cur = (uint8_t)(n & 7); sceneT = tick; barInScene = 0; SC().used = 1; for (auto& t : T) t.walk = 0; push(UI_SCENE, PJ.cur); }
  void queueScene(int n) { if (!playing) { switchScene(n); return; } queued = n & 7; push(UI_SCENE, PJ.cur, queued); }
  int stepIndex(const TrackPat& t, long c) { int L = maxi(1, t.len); if (t.dir == 1) return L - 1 - (int)(c % L); if (t.dir == 2) { int P = L > 1 ? 2 * L - 2 : 1; int x = (int)(c % P); return x < L ? x : P - x; } return (int)(c % L); }

  void barStart(long k) {
    if (fill && k / 16 >= fillEnd) fill = false;
    barInScene = (int)((k - sceneT) / 16);
    if (PJ.songOn && PJ.songLen && k > 0) { songBar++; if (songBar >= PJ.song[songIdx % PJ.songLen].b) { songBar = 0; songIdx = (songIdx + 1) % PJ.songLen; queued = -1; switchScene(PJ.song[songIdx].s); } }
    if (queued >= 0 && barInScene % maxi(1, quant) == 0) { int q = queued; queued = -1; switchScene(q); }
    Scene& S = SC(); chordIdx = (barInScene / maxi(1, S.cb)) & 3; chordDeg = S.prog[chordIdx];
    if (p(P_AUTO) > 0.5f && barInScene > 0) muse();
    if (novaLeft > 0 && novaLeft <= novaLen) {}
    push(UI_BAR, chordIdx, barInScene, chordDeg);
  }
  void beatStart() {
    if (pendKey >= 0) { PJ.key = (uint8_t)pendKey; PJ.scale = (uint8_t)pendScale; pendKey = pendScale = -1; reharm = true; push(UI_MOD, PJ.key, PJ.scale); projVersion++; }
  }
  void doTick(long k, int offset) {
    float s16 = samplesPer16(); Scene& S = SC(); long kS = k - sceneT;
    if (k % 16 == 0) barStart(k);
    if (k % 4 == 0) beatStart();
    if (fxWant && fxStartIn < 0) { int div = pStep(P_FX_TYPE) == 11 ? 2 : 1; if (k % div == 0) { fx.setOn(true); fxWant = false; } }
    int sw = (k % 2) ? (int)(PJ.swing / 127.f * 0.32f * s16) : 0;
    float energy = p(P_AUTO) > 0.5f ? 0.55f + 0.45f * p(P_ENERGY) : 1.f;
    for (int i = 0; i < 8; i++) {
      TrackPat& t = S.t[i]; int rs = RATE_STEPS[clampi(t.rate, 0, 4)];
      if (kS % rs) { if (i == 0 && fill) fillHit(kS, offset + sw); continue; }
      long c = kS / rs; int stp;
      if (t.dir == 3) stp = rng.range(0, maxi(1, t.len) - 1);
      else if (t.dir == 4) { T[i].walk = (T[i].walk + (rng.chance(.7f) ? 1 : rng.chance(.5f) ? -1 : 2) + t.len) % maxi(1, t.len); stp = T[i].walk; }
      else stp = stepIndex(t, c);
      tele[16 + i] = (float)stp;
      const TrackDef& td = TRACKS[i];
      if (td.kind == K_DRUM) {
        for (int l = 0; l < 4; l++) { const DStep& d = t.lanes[l].s[stp]; if (!d.on) continue; float pr = d.prob / 127.f * (l >= 2 ? energy : 1.f); if (!rng.chance(pr)) continue;
          int r = maxi(1, d.rat); for (int q = 0; q < r; q++) addEv(offset + sw + (int)(q * s16 * rs / r), 3, 0, t.lanes[l].v, q ? (int)(d.vel * (0.55f + 0.35f * q / r)) : d.vel, -1, 0);
          push(UI_NOTE, 0, t.lanes[l].v, d.vel); }
        if (fill) fillHit(kS, offset + sw);
        continue;
      }
      const Step& s = t.s[stp];
      if (td.kind == K_INPUT) { bool any = false; for (int q = 0; q < t.len; q++) if (t.s[q].on) { any = true; break; } inGate.set(any ? (s.on ? 1.f : 0.08f) : 1.f); continue; }
      if (!s.on) { if (reharm && i >= 1 && i <= 3) reharmTrack(i, t, offset); continue; }
      float pr = s.prob / 127.f * (stp == 0 ? 1.f : energy); if (!rng.chance(pr)) continue;
      if (td.kind == K_TEX) { tex.event(s.vel / 127.f); push(UI_NOTE, 6, 60, s.vel); continue; }
      float dur = maxf(0.03f, s.gate4 / 4.f * rs * s16 / sr * 0.97f); int d = s.deg + (t.follow ? chordDeg : 0), oct = t.oct + s.oct;
      if (t.chord) { int notes[4]; int n = voiceChord(i, d, oct, s.vel > 110 && scale().n >= 7, notes); for (int q = 0; q < n; q++) addEv(offset + sw + q * 24, 1, i, notes[q], s.vel, -1, dur, q); push(UI_NOTE, i, notes[0], s.vel); }
      else { int m = degToMidi(i, d, oct); addEv(offset + sw, 1, i, m, s.vel, -1, dur, 0); push(UI_NOTE, i, m, s.vel); }
    }
    reharm = false;
    if (tex.type == 1 && rng.chance(0.3f)) tex.addDrop();
  }
  void reharmTrack(int i, TrackPat& t, int offset) {    // after a key change, sustaining parts move to the new scale on the beat
    bool sounding = false; for (auto& v : T[i].v) if (v.on && v.amp.st != Env::REL && v.amp.st != Env::FAST) { sounding = true; break; } if (!sounding) return;
    for (auto& v : T[i].v) if (v.on) v.release();
    float dur = beatSec() * 4.f * maxi(1, SC().cb) * 0.9f; int d = chordDeg;
    if (t.chord) { int notes[4]; int n = voiceChord(i, d, t.oct, 0, notes); for (int q = 0; q < n; q++) addEv(offset + q * 24, 1, i, notes[q], 90, -1, dur, q); }
    else addEv(offset, 1, i, degToMidi(i, d + (i == 3 ? 4 : 0), t.oct), 90, -1, dur, 0);
  }
  void fillHit(long kS, int off) { int pos = (int)(kS % 16); addEv(off, 3, 0, 2, 60 + rng.range(0, 40), -1, 0); if (pos >= 12) { addEv(off, 3, 0, 1, 50 + (pos - 12) * 18, -1, 0); addEv(off + (int)(samplesPer16() / 2), 3, 0, 1, 40 + (pos - 12) * 15, -1, 0); } if (pos == 8) addEv(off, 3, 0, 6, 90, -1, 0); }

  // ============================================================ MUSE: music that writes itself from your pattern
  void museVary(int sc, int i, float a) {
    TrackPat& t = PJ.scenes[sc].t[i]; const TrackPat& s0 = seed[sc][i]; const TrackDef& td = TRACKS[i];
    if (td.kind == K_DRUM) {
      for (int l = 0; l < 4; l++) for (int q = 0; q < 32; q++) t.lanes[l].s[q] = s0.lanes[l].s[q];
      int nch = 1 + (int)(a * 4); for (int k = 0; k < nch; k++) { int l = rng.range(2, 3), q = rng.range(0, maxi(1, t.len) - 1); DStep& d = t.lanes[l].s[q]; if (d.on && rng.chance(.5f)) d.on = 0; else { d.on = 1; d.vel = (uint8_t)rng.range(35, 95); d.prob = (uint8_t)rng.range(70, 127); d.rat = rng.chance(a * .3f) ? (uint8_t)rng.range(2, 3) : 1; } }
      if (rng.chance(a * .5f)) { int q = rng.range(1, 15); if (!s0.lanes[1].s[q].on && q % 4 != 0) dOn(t, 1, q, 35, 70); }   // ghost note
      return;
    }
    if (td.kind != K_SYNTH) return;
    if (t.chord) { for (int q = 0; q < 32; q++) t.s[q] = s0.s[q]; for (int q = 0; q < t.len; q++) if (t.s[q].on && rng.chance(a * .4f)) t.s[q].vel = rng.chance(.5f) ? 116 : 96; return; }
    int L = maxi(1, s0.len), on[32], n = 0, iv[32], ni = 0, prev = -999;
    for (int q = 0; q < L; q++) if (s0.s[q].on) { on[n++] = q; if (prev != -999) iv[ni++] = s0.s[q].deg - prev; prev = s0.s[q].deg; }
    for (int q = 0; q < 32; q++) t.s[q] = s0.s[q];
    if (!n) return;
    // rhythm: keep the skeleton, nudge a few notes, add a few where the seed already likes to play
    for (int k = 1; k < n; k++) { int q = on[k]; if (rng.chance(a * .25f)) { int nq = clampi(q + (rng.chance(.5f) ? 1 : -1), 1, L - 1); if (!t.s[nq].on) { t.s[nq] = t.s[q]; t.s[q] = Step(); on[k] = nq; } } else if (rng.chance(a * .15f)) { t.s[q].on = 0; } }
    int adds = (int)(a * 2.5f + rng.uni()); for (int k = 0; k < adds; k++) { int q = rng.range(0, L - 1); if (t.s[q].on) continue; bool parity = (on[rng.range(0, n - 1)] & 1) == (q & 1); if (!parity && rng.chance(.6f)) continue; t.s[q] = s0.s[on[rng.range(0, n - 1)]]; t.s[q].on = 1; }
    // pitch: keep most seed notes, otherwise walk with the seed's own intervals; strong beats land on chord tones
    int last = s0.s[on[0]].deg; const int CT[5] = {0, 2, 4, 7, -3};
    for (int q = 0; q < L; q++) { Step& st_ = t.s[q]; if (!st_.on) continue;
      if (rng.chance(1.f - a * .6f) && s0.s[q].on) { last = st_.deg = s0.s[q].deg; continue; }
      int step_ = ni ? iv[rng.range(0, ni - 1)] : (rng.chance(.5f) ? 1 : -1) * rng.range(1, 2); int d = clampi(last + step_, -5, 12);
      if (q % 4 == 0) { int b = CT[0]; for (int c : CT) if (absf((float)(c - d)) < absf((float)(b - d))) b = c; d = b; }
      st_.deg = (int8_t)d; st_.vel = (uint8_t)clampi(st_.vel + rng.range(-12, 12), 40, 127); last = d; }
    // call & response: quote what you played on the keyboard
    if ((i == 4 || i == 5) && motifN >= 3 && rng.chance(0.35f + a * .3f)) { int k = 0; for (int q = L / 2; q < L && k < motifN; q++) if (t.s[q].on) t.s[q].deg = (int8_t)motif[k++]; }
  }
  void muse() {
    float a = p(P_AUTO_AMT); int phrase = a > 0.66f ? 2 : 4; int sc = PJ.cur & 7; Scene& S = SC();
    if (barInScene % phrase == 0) {
      int cand[8], nc = 0; for (int i = 0; i < 6; i++) if (!S.mutes[i]) cand[nc++] = i;
      int changes = a > 0.6f ? 2 : 1; for (int k = 0; k < changes && nc; k++) museVary(sc, cand[rng.range(0, nc - 1)], a);
      push(UI_EVOLVE, sc); projVersion++;
    }
    if (barInScene % 8 == 0 && autoArr && a > 0.3f) {   // arrangement breathes with ENERGY: parts come and go with a slow fade
      float e = p(P_ENERGY); int i = rng.range(0, 5); bool want = rng.chance(0.25f + e * 0.6f);
      if ((i == 0 || i == 5) && e < 0.35f) want = false; if (i == 2) want = true;
      if (S.mutes[i] != (uint8_t)!want) { S.mutes[i] = (uint8_t)!want; projVersion++; }
    }
    if (barInScene % 16 == 0 && rng.chance(a * .35f)) { int k = rng.range(1, 3); const int8_t opts[6] = {0, 1, 3, 4, 5, -1}; S.prog[k] = opts[rng.range(0, 5)]; projVersion++; }
  }

  // ============================================================ moods
  void applyMoodSound(const Mood& m, int keep) {
    const PresetBank& B = presets();
    if (!(keep & 1)) { for (int n = 0; n < 5; n++) PJ.patches[n + 1] = B.p[B.find(m.p[n])]; PJ.kit = (uint8_t)m.kit; PJ.texType = (uint8_t)m.tex; tex.type = m.tex; }
    if (!(keep & 2)) PJ.bpm = (float)rng.range(m.bpmLo, m.bpmHi);
    if (!(keep & 4)) { PJ.scale = (uint8_t)scaleIndex(m.scales[rng.range(0, 2)]); PJ.key = (uint8_t)rng.range(0, 11); }
    setParam(P_ST_MODE, m.echoMode / 11.f); setParam(P_ST_RATE, m.echoRate / 127.f); setParam(P_ST_INT, m.echoInt / 127.f); setParam(P_ST_WOW, m.echoWow / 127.f); setParam(P_ST_ON, 1); setParam(P_COS_SIZE, m.cosmos / 127.f);
    PJ.swing = (uint8_t)(streq(m.drums, "break") ? 34 : streq(m.drums, "soft") ? 14 : 0);
  }
  void genScene(Scene& S, const Mood& m, float ch) {
    Gen g(rng); S.cb = (uint8_t)m.chordBars; const int8_t* pr = PROGS[rng.chance(ch * .5f) ? rng.range(0, 11) : rng.range(0, 8)]; for (int k = 0; k < 4; k++) S.prog[k] = pr[k];
    g.drums(S.t[0], m.drums, ch); g.bass(S.t[1], m.bass, ch); g.pad(S.t[2], m.chordBars, ch, m.pad); g.voice(S.t[3], m.chordBars); g.arp(S.t[4], m.bells, ch); g.lead(S.t[5], m.lead, ch); g.tex(S.t[6], m.chordBars); clearTrack(S.t[7]);
    float* d = (float*)m.dens; S.mutes[0] = streq(m.drums, "none"); S.mutes[1] = !rng.chance(d[0]); S.mutes[2] = 0; S.mutes[3] = !rng.chance(d[2]); S.mutes[4] = !rng.chance(d[3]); S.mutes[5] = !rng.chance(d[4]); S.mutes[6] = !rng.chance(d[5]); S.mutes[7] = 0; S.used = 1;
  }
  void align(int mood, int keep, bool song) {
    PJ.mood = (uint8_t)clampi(mood, 0, NMOODS - 1); const Mood& m = MOODS[PJ.mood]; float ch = PJ.chaos / 127.f;
    applyMoodSound(m, keep);
    if (!song) { genScene(SC(), m, ch); for (int i = 0; i < 8; i++) seed[PJ.cur][i] = SC().t[i]; }
    else {
      Scene base; genScene(base, m, ch); for (int i = 0; i < 8; i++) base.mutes[i] = (i == 0 && streq(m.drums, "none")) ? 1 : 0;
      const uint8_t M[6][8] = {{1, 1, 0, 1, 1, 1, 0, 0}, {1, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 1, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 1, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}, {1, 1, 0, 0, 0, 1, 0, 0}};
      Gen g(rng);
      for (int s = 0; s < 6; s++) { PJ.scenes[s] = base; for (int i = 0; i < 8; i++) PJ.scenes[s].mutes[i] = M[s][i] | base.mutes[i]; }
      g.arp(PJ.scenes[3].t[4], "chime", ch); clearTrack(PJ.scenes[3].t[1]); PJ.scenes[3].t[1].rate = 0; PJ.scenes[3].t[1].len = 1; setStep(PJ.scenes[3].t[1], 0, 0, 4);
      g.lead(PJ.scenes[4].t[5], m.lead, minf(1.f, ch + .2f)); g.voice(PJ.scenes[5].t[3], m.chordBars);
      for (int s = 6; s < 8; s++) { PJ.scenes[s] = Scene(); for (int i = 0; i < 8; i++) defaultTrack(PJ.scenes[s].t[i], i); }
      int cb = m.chordBars; auto L = [&](int n) { return (uint8_t)maxi(cb * 2, n); };
      const uint8_t SB[6] = {8, 8, 16, 8, 16, 8}; for (int s = 0; s < 6; s++) PJ.song[s] = {(uint8_t)s, L(SB[s])}; PJ.songLen = 6; PJ.songOn = 1; PJ.cur = 0;
      snapshotSeeds();
    }
    params[P_MOOD] = PJ.mood / (float)(NMOODS - 1); projVersion++; push(UI_PROJECT);
  }
  void genTrack(int i) {
    const Mood& m = MOODS[PJ.mood]; Gen g(rng); float ch = PJ.chaos / 127.f; Scene& S = SC(); TrackPat& t = S.t[i];
    switch (i) { case 0: g.drums(t, streq(m.drums, "none") ? "tick" : m.drums, ch); break; case 1: g.bass(t, m.bass, ch); break; case 2: g.pad(t, S.cb, ch, m.pad); break;
      case 3: g.voice(t, S.cb); break; case 4: g.arp(t, m.bells, ch); break; case 5: g.lead(t, m.lead, ch); break; case 6: g.tex(t, S.cb); break;
      default: clearTrack(t); t.rate = 4; t.len = 16; for (int q = 0; q < 16; q++) if (euclidHit(5, 16, 0, q)) setStep(t, q, 0); }
    S.mutes[i] = 0; seed[PJ.cur][i] = t; projVersion++;
  }

  // ============================================================ live input (keyboard, pads, controllers)
  void liveNote(int tr, int midi, int vel) {
    tr = clampi(tr, 0, 7); const TrackDef& td = TRACKS[tr];
    if (vel <= 0) { int m = lastLive[midi & 127]; if (m >= 0) for (auto& v : T[tr].v) if (v.on && v.note == m) v.release(); lastLive[midi & 127] = -1; return; }
    if (td.kind == K_DRUM) { static const int GM[16] = {0, 5, 1, 4, 1, 2, 6, 2, 6, 3, 6, 6, 7, 6, 8, 10}; int idx = midi - 36; drumHit(idx >= 0 && idx < 16 ? GM[idx] : (midi % 11), vel / 127.f); push(UI_NOTE, 0, midi, vel); return; }
    if (td.kind == K_TEX) { tex.event(vel / 127.f); return; }
    if (td.kind != K_SYNTH) return;
    int mode = pStep(P_KEYMODE), m = midi;
    if (mode == KM_SNAP) m = snapToScale(midi);
    else if (mode == KM_SHIFT && !inScale(scale(), PJ.key, midi % 12)) { int k, s; findShift(midi % 12, k, s); pendKey = k; pendScale = s; }
    // remember what you played: the MUSE answers it later
    int deg = 0; for (int d = -7; d < 14; d++) if (degToMidi(tr, d, 0) % 12 == m % 12) { deg = d; break; }
    if (motifN < 8) motif[motifN++] = deg; else { for (int q = 0; q < 7; q++) motif[q] = motif[q + 1]; motif[7] = deg; }
    const TrackPat& t = SC().t[tr];
    if (t.chord) { int notes[4]; int base = 0, bdd = 99; for (int d = -7; d < 14; d++) { int dm = degToMidi(tr, d, t.oct); if (dm % 12 == m % 12) { base = d; bdd = 0; break; } } (void)bdd; int n = voiceChord(tr, base, t.oct, 0, notes); for (int q = 0; q < n; q++) noteOn(tr, notes[q], vel / 127.f, q); lastLive[midi & 127] = notes[0]; }
    else { noteOn(tr, m, vel / 127.f, 0); lastLive[midi & 127] = m; }
    push(UI_NOTE, tr, m, vel); sinceUser = 0;
  }
  void perf(int id, bool down) {
    switch (id) {
      case PERF_FREEZE: if (down) { setParam(P_ST_FREEZE, p(P_ST_FREEZE) > .5f ? 0.f : 1.f); } break;
      case PERF_STOP: mst.tapeStop(down); break;
      case PERF_SPIN: if (down) st.spin(1); break;
      case PERF_SPIN_BACK: if (down) st.spin(-1); break;
      case PERF_FILL: if (down && playing) { fill = true; fillEnd = tick / 16 + 1; } break;
      case PERF_NOVA: if (down && playing) { int toBar = (int)((16 - tick % 16) % 16); novaLen = (int)(2 * 16 * samplesPer16()); novaLeft = novaLen + (int)(toBar * samplesPer16()); } break;
      case PERF_ECLIPSE: eclipse = down; break;
      case PERF_KICK: if (down) st.springKick(); break;
      case PERF_ALIGN: if (down) align(PJ.mood, 0, false); break;
      case PERF_ALIGN_SONG: if (down) align(PJ.mood, 0, true); break;
      case PERF_AUTO: if (down) setParam(P_AUTO, p(P_AUTO) > .5f ? 0.f : 1.f); break;
    }
    push(UI_PERF, id, down);
  }
  // raw MIDI from any controller (KeyLab pads on channel 10 play the live moves)
  void midi(uint8_t s, uint8_t d1, uint8_t d2) {
    int cmd = s & 0xF0, ch = s & 15;
    if (cmd == 0xB0) {
      if (learnParam >= 0) { for (auto& c : ccMap) for (auto& x : c) if (x == learnParam) x = -1; ccMap[ch][d1] = (int16_t)learnParam; learnParam = -1; push(UI_PROJECT); return; }
      int pid = ccMap[ch][d1]; if (pid >= 0) setParam(pid, d2 / 127.f);
      else if (d1 == 1) setParam(P_FX_X, d2 / 127.f);           // mod wheel = FX X
      else if (d1 == 64) { if (d2 >= 64) perf(PERF_FREEZE, true); }
      return;
    }
    bool on = cmd == 0x90 && d2 > 0, off = cmd == 0x80 || (cmd == 0x90 && d2 == 0);
    if (ch == padChannel && d1 >= 36 && d1 <= 51) {          // 16 pads: scenes A–H, then the live moves
      int pad = d1 - 36;
      if (pad < 8) { if (on) queueScene(pad); return; }
      static const int PM[8] = {PERF_FREEZE, PERF_STOP, PERF_SPIN, PERF_FILL, PERF_NOVA, PERF_ECLIPSE, PERF_KICK, -2};
      int id = PM[pad - 8]; if (id == -2) { if (on) { setParam(P_FX_ON, p(P_FX_ON) > .5f ? 0.f : 1.f); } return; }
      if (on) perf(id, true); else if (off && (id == PERF_STOP || id == PERF_ECLIPSE)) perf(id, false);
      return;
    }
    if (on || off) liveNote(pStep(P_TRACK), d1, on ? d2 : 0);
  }

  // ============================================================ parameters → engine
  void onParam(int id, float old) {
    float v = params[id];
    switch (id) {
      case P_ST_FREEZE: break;
      case P_SCENE: { int s = pStep(P_SCENE); if (s != PJ.cur) queueScene(s); break; }
      case P_MOOD: PJ.mood = (uint8_t)pStep(P_MOOD); break;
      case P_CHAOS: PJ.chaos = (uint8_t)(v * 127); break;
      case P_SWING: PJ.swing = (uint8_t)(v * 127); break;
      case P_FX_ON: if (v > .5f && old <= .5f) { fxWant = true; fxStartIn = -1; } else if (v <= .5f) { fxWant = false; fx.setOn(false); } break;
      case P_TRACK: syncSoundParams(); break;
      case P_S_CUT: case P_S_RES: case P_S_ATT: case P_S_REL: case P_S_WARBLE: case P_S_DRIVE: case P_S_LEVEL: case P_S_ENV: {
        int tr = pStep(P_TRACK); if (TRACKS[tr].kind != K_SYNTH) break; static const int MAP[8] = {PF_CUT, PF_RES, PF_A, PF_R, PF_WARBLE, PF_DRIVE, PF_LVL, PF_ENV};
        PJ.patches[tr].v[MAP[id - P_S_CUT]] = (uint8_t)(v * 127 + .5f); break; }
      default: break;
    }
  }
  void syncSoundParams() { int tr = pStep(P_TRACK); if (TRACKS[tr].kind != K_SYNTH) return; static const int MAP[8] = {PF_CUT, PF_RES, PF_A, PF_R, PF_WARBLE, PF_DRIVE, PF_LVL, PF_ENV}; for (int k = 0; k < 8; k++) params[P_S_CUT + k] = PJ.patches[tr].v[MAP[k]] / 127.f; }

  // ============================================================ audio
  void process(float* outL, float* outR, const float* inL, const float* inR, int n) {
    for (int off = 0; off < n; off += SUB) {
      int m = minf(SUB, n - off);
      controlBlock(m);
      renderBlock(outL + off, outR + off, inL ? inL + off : nullptr, inR ? inR + off : nullptr, m);
    }
  }
  void controlBlock(int m) {
    // transport: find 16th boundaries inside this block
    if (hostSync) {
      if (hostPlaying && !playing) { playing = true; double pos = hostPpq * 4.0; tick = (long)pos; sceneT = 0; toNext = (1.0 - (pos - tick)) * samplesPer16(); if (pos - tick < 1e-6) toNext = 0; }
      if (!hostPlaying && playing) stop();
    }
    if (playing) {
      while (toNext < m) { doTick(tick, (int)maxf(0, toNext)); tick++; toNext += samplesPer16(); }
      toNext -= m; if (hostSync) hostPpq += m / sr / beatSec();
    }
    // events
    int w = 0;
    for (int e = 0; e < nev; e++) { Ev& E = ev[e]; if (E.at < m) {
        if (E.type == 1) { int slot = noteOn(E.tr, E.note, E.vel / 127.f, E.idx); addEv(E.at + (int)(E.dur * sr), 2, E.tr, E.note, 0, slot, 0); }
        else if (E.type == 2) noteOffSlot(E.tr, E.voice, E.note);
        else if (E.type == 3) drumHit(E.note, E.vel / 127.f);
        E.at = -1; } }
    for (int e = 0; e < nev; e++) if (ev[e].at >= 0) { ev[w] = ev[e]; ev[w].at -= m; w++; } nev = w;
    // live values
    float bloom = 0.3f + p(P_BLOOM) * 1.4f; bool soloOn = false;
    for (int i = 0; i < 8; i++) { TrackRT& R = T[i]; float mute = SC().mutes[i] ? 0.f : 1.f; if (eclipse && (i == 0 || i == 1 || i == 5)) mute = 0;
      R.vol.set(powf_(p(P_VOL1 + i), 1.6f) * 1.3f); R.e.set(p(P_ECH1 + i) * bloom * (eclipse ? 1.6f : 1.f)); R.r.set(p(P_REV1 + i) * bloom * (eclipse ? 1.6f : 1.f)); R.mute.set(mute);
      R.mute.time(beatSec() * (mute > 0 ? 0.5f : 0.35f), sr); R.flare *= 0.9f; }
    (void)soloOn;
    st.mode = pStep(P_ST_MODE); st.rate = p(P_ST_RATE); st.intensity = p(P_ST_INT); st.echoVol = p(P_ST_ECHO); st.revVol = p(P_ST_REVERB); st.bass = p(P_ST_BASS); st.treble = p(P_ST_TREBLE); st.inVol = p(P_ST_INPUT);
    st.wow = p(P_ST_WOW) * (0.3f + p(P_DRIFT) * 1.4f); st.flutter = p(P_ST_FLUTTER); st.age = p(P_ST_AGE); st.on = p(P_ST_ON) > .5f; st.freeze = p(P_ST_FREEZE) > .5f; st.sync = p(P_ST_SYNC) > .5f; st.syncSec = SYNC_DIVS[pStep(P_ST_DIV)] * beatSec();
    cos.size = p(P_COS_SIZE); cos.dampV = p(P_COS_DAMP); cos.preV = p(P_COS_PRE); cos.vol = p(P_COS_VOL) * 1.4f;
    fx.type = pStep(P_FX_TYPE); fx.x = p(P_FX_X); fx.y = p(P_FX_Y); fx.mix = p(P_FX_MIX); fx.beatSec = beatSec(); fx.beatPos = playing ? (tick + (1.0 - toNext / samplesPer16())) / 4.0 : fx.beatPos + m / sr / beatSec();
    mst.haze.set(p(P_HAZE)); mst.grav.set(p(P_GRAVITY)); mst.vol.set(p(P_MASTER) * 1.25f);
    tex.type = PJ.texType % 7; tex.mot = PJ.texMotion / 127.f; tex.key = PJ.key;
  }
  void renderBlock(float* outL, float* outR, const float* inL, const float* inR, int m) {
    float mixL[SUB], mixR[SUB], eS[SUB], rS[SUB]; for (int i = 0; i < m; i++) mixL[i] = mixR[i] = eS[i] = rS[i] = 0;
    float drift = 0.3f + p(P_DRIFT) * 1.7f; lph[0] += m * 0.21f / sr; lph[1] += m * 0.34f / sr; lph[2] += m * 5.6f / sr;
    for (int i = 0; i < 8; i++) {
      TrackRT& R = T[i]; const TrackDef& td = TRACKS[i]; for (int q = 0; q < m; q++) R.buf[q] = 0;
      const uint8_t* P = PJ.patches[i].v;
      if (td.kind == K_SYNTH) { float cents = (u127(P[PF_WARBLE]) * (14.f * sinp(lph[i & 1]) + 2.5f * sinp(lph[2])) * drift) + u127(P[PF_VIB]) * 14.f * sinp(lph[2] * 0.93f);
        float cutMul = 1.f; for (auto& v : R.v) v.render(R.buf, m, cents, cutMul); }
      else if (td.kind == K_DRUM) { for (auto& d : R.d) d.render(R.buf, m); const Kit& k = KITS[PJ.kit % NKITS]; float cr = u127(k.crunch) * 0.45f; R.dlp.setLP(expMap(k.lp, 1800, 20000), sr);
        for (int q = 0; q < m; q++) { float x = R.buf[q]; if (cr > 0.02f) { float steps = 2.f + (1.f - cr) * (1.f - cr) * 400.f; x = softclip(x * (1.f + cr * 3.f), 0.9f); x = floorf_(x * steps + 0.5f) / steps; } R.buf[q] = R.dlp.lp(x); } }
      else if (td.kind == K_TEX) tex.render(R.buf, m, PJ.texLvl / 127.f * 0.55f, PJ.texTone / 127.f);
      else if (td.kind == K_INPUT) {
        float fc = 60.f * powf_(330.f, p(P_IN_FILTER)), q = 0.6f + p(P_IN_RES) * 12.f, dr = 1.f + p(P_IN_DRIVE) * 6.f; inMot += m * (0.05f + p(P_IN_MOTION) * 0.6f) / sr; float mod = exp2f_(sinp(inMot) * p(P_IN_MOTION) * 2.f);
        inF[0].set(minf(fc * mod, sr * 0.45f), q, sr);
        for (int s = 0; s < m; s++) { float x = 0; if (inL) x += (inL[s] + (inR ? inR[s] : inL[s])) * 0.5f;
          if (filePlay && fileL && fileLen > 0) { int fi = (int)filePos; x += (fileL[fi] + (fileR ? fileR[fi] : fileL[fi])) * 0.5f * fileGain; filePos += 1.0; if (filePos >= fileLen) filePos = 0; }
          inPeak = maxf(inPeak * 0.9995f, absf(x)); x = inF[0].lp(x); x = softclip(x * dr, 0.9f) / sqrtf_(dr); R.buf[s] = x * inGate.tick(); } }
      // per-track colour: choir formants, ensemble width
      if (td.kind == K_SYNTH && P[PF_ENG] == E_CHOIR) { float fi = u127(P[PF_VOWEL]) * 3.999f; int a = (int)fi; float fr = fi - a;
        for (int k = 0; k < 3; k++) { R.fq[k].set(lerpf(VOWELS[a][k], VOWELS[a + 1][k], fr)); R.fmt[k].set(R.fq[k].tick(), 9.f, sr); }
        for (int q = 0; q < m; q++) { float x = R.buf[q]; R.buf[q] = (R.fmt[0].bp(x) + R.fmt[1].bp(x) * 0.55f + R.fmt[2].bp(x) * 0.3f) * 3.2f; } }
      float chorus = td.kind == K_SYNTH ? u127(P[PF_CHORUS]) : 0.f, drv = td.kind == K_SYNTH ? u127(P[PF_DRIVE]) * 0.7f : 0.f;
      for (int q = 0; q < m; q++) {
        float x = R.buf[q]; if (drv > 0.02f) x = softclip(x * (1.f + drv * 3.f), 1.f) / (1.f + drv * 1.5f);
        float l = x, r = x; if (chorus > 0.02f) R.ens.process(x, chorus, 0.5f, l, r);
        float g = R.vol.tick() * R.mute.tick(), pan = 0.f; float gl = g * (1.f - pan), gr = g * (1.f + pan);
        l *= gl; r *= gr; mixL[q] += l; mixR[q] += r; float mono = (l + r) * 0.5f; eS[q] += mono * R.e.tick(); rS[q] += mono * R.r.tick();
      }
    }
    // supernova: a two-bar riser that ends in an impact on the downbeat
    if (novaLeft > 0) for (int q = 0; q < m; q++) { if (novaLeft <= novaLen && novaLeft > 0) { float pr = 1.f - novaLeft / (float)novaLen; novaF.set(200.f * powf_(45.f, pr), 3.f, sr); float nz = novaF.bp(rng.next()) * pr * 0.5f; mixL[q] += nz; mixR[q] += nz; eS[q] += nz; rS[q] += nz; }
        novaLeft--; if (novaLeft == 0) { boom = 1; push(UI_NOVA); } }
    if (boom > 1e-4f) for (int q = 0; q < m; q++) { boomPh += (32.f + 90.f * boom * boom) / sr; boomPh -= floorf_(boomPh); float b = sinp(boomPh) * boom * 0.8f; boom *= 0.99985f; mixL[q] += b; mixR[q] += b; rS[q] += b * 0.5f; }
    for (int q = 0; q < m; q++) {
      float L = mixL[q], R = mixR[q], el = 0, er = 0;
      st.process(eS[q], el, er); L += el; R += er;
      float cl = 0, cr = 0; cos.process(rS[q] + (el + er) * 0.06f, cl, cr, q); L += cl; R += cr;
      fx.process(L, R); mst.process(L, R);
      outL[q] = L; outR[q] = R;
      scope[scopeW * 2] = L; scope[scopeW * 2 + 1] = R; scopeW = (scopeW + 1) & 1023;
      rmsL += (L * L - rmsL) * 0.0005f; rmsR += (R * R - rmsR) * 0.0005f; float lo = lowF.lp(L + R); low += (lo * lo - low) * 0.001f; float hi = (L + R) - highF.lp(L + R); high += (hi * hi - high) * 0.001f;
    }
    // telemetry for the screens
    tele[0] = playing; tele[1] = hostSync ? hostBpm : PJ.bpm; tele[2] = (float)barInScene; tele[3] = (float)(tick % 16); tele[4] = PJ.cur; tele[5] = (float)queued; tele[6] = (float)chordIdx; tele[7] = (float)chordDeg;
    tele[8] = PJ.key; tele[9] = PJ.scale; tele[10] = (float)songIdx; tele[11] = sqrtf_(rmsL); tele[12] = sqrtf_(rmsR); tele[13] = sqrtf_(low); tele[14] = sqrtf_(high); tele[15] = st.T1;
    for (int i = 0; i < 8; i++) tele[24 + i] = T[i].flare;
    tele[32] = st.headLvl[0]; tele[33] = st.headLvl[1]; tele[34] = st.headLvl[2]; tele[35] = st.peak; tele[36] = inPeak; tele[37] = p(P_ST_FREEZE); tele[38] = eclipse; tele[39] = p(P_AUTO);
    tele[40] = (float)projVersion; tele[41] = fx.on ? 1.f : 0.f; tele[42] = (float)pendKey; tele[43] = mst.spd; tele[44] = st.sprLvl;
  }
};


} // namespace andro
