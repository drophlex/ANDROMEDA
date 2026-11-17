// ANDROMEDA engine · fx.h — Space Tape (three-head tape echo + spring), Cosmos hall, the live FX rack, the master
#pragma once
#include "voices.h"

namespace andro {

// ================================================================ SPACE TAPE
struct SpaceTape {
  static constexpr int N = 1 << 18;          // ~5.4 s at 48 kHz
  Delay<N> tape; float sr = 48000;
  // controls (0..1 unless noted), set every block by the engine
  int mode = 8; bool on = true, freeze = false; float rate = .55f, intensity = .47f, echoVol = .72f, revVol = .44f, bass = .5f, treble = .47f, inVol = .5f, wow = .32f, flutter = .24f, age = .47f;
  bool sync = false; float syncSec = 0.25f;
  // state
  float T1 = 0.3f, T1t = 0.3f, motorK = 0, spinMul = 1, spinT = 0;
  Smooth headG[3], inG, fbG, onG, sprG, outG; float wph = 0, fph = 0, drift = 0, driftT = 0; Rng rng;
  OnePole fbLP, fbHP, ageLP; Shelf eqB, eqT, eqB2, eqT2; float peak = 0, headLvl[3] = {0, 0, 0};
  // spring: dispersive all-passes + two combs per side
  float sprLvl = 0; float apBuf[8][700]; Allpass ap[8]; Delay<4096> cL, cR; OnePole cLP, cRLP; float kick = 0; float dropT = 0, drop = 1;
  void init(float rate_) {
    sr = rate_; motorK = 1.f - expf_(-1.f / (0.22f * sr));
    for (auto& h : headG) h.time(0.03f, sr); inG.time(0.05f, sr); fbG.time(0.04f, sr); onG.time(0.03f, sr); sprG.time(0.05f, sr); outG.time(0.05f, sr);
    static const int L[8] = {113, 157, 199, 251, 293, 347, 409, 467};
    for (int i = 0; i < 8; i++) { for (int j = 0; j < 700; j++) apBuf[i][j] = 0; ap[i].buf = apBuf[i]; ap[i].n = (int)(L[i] * sr / 48000.f) + 1; if (ap[i].n > 699) ap[i].n = 699; ap[i].g = 0.62f; }
    cLP.setLP(4200, sr); cRLP.setLP(3900, sr); fbHP.setLP(90, sr);
  }
  float headTime() const { return sync ? clampf(syncSec, 0.02f, 1.8f) : 0.045f * powf_(20.f, rate); }
  void spin(float dir) { spinT = dir > 0 ? 0.35f : 2.2f; }
  void springKick() { kick = 1.f; }
  // in: mono send; outL/outR accumulate
  inline void process(float in, float& outL, float& outR) {
    const EchoMode& md = ECHO_MODES[clampi(mode, 0, 11)];
    // motor: tape speed follows the knob with inertia (that is where the pitch swoops come from)
    T1t = headTime() * spinMul; T1 += (T1t - T1) * motorK;
    spinMul += ((spinT > 0 ? spinT : 1.f) - spinMul) * 0.00012f; if (spinT > 0 && absf(spinMul - spinT) < 0.02f) spinT = 0;
    // wow (slow), flutter (fast), drift (random)
    wph += (0.45f + wow * 0.5f) / sr; wph -= floorf_(wph); fph += (6.1f + flutter * 2.f) / sr; fph -= floorf_(fph);
    if (rng.uni() < 0.0005f) driftT = rng.next(); drift += (driftT - drift) * 0.00005f;
    float mod = 1.f + sinp(wph) * wow * 0.0035f + sinp(fph) * flutter * 0.0009f + drift * wow * 0.002f;
    // tape age: random small dropouts
    if (age > 0.3f && rng.uni() < age * 0.00002f) dropT = 1.f; drop += ((dropT > 0 ? 0.55f : 1.f) - drop) * 0.002f; dropT *= 0.9995f; if (dropT < 0.01f) dropT = 0;
    int nh = (md.heads & 1) + ((md.heads >> 1) & 1) + ((md.heads >> 2) & 1); float hgN = nh ? 0.8f / sqrtf_((float)nh) : 0;
    float sum = 0;
    for (int h = 0; h < 3; h++) {
      headG[h].set(on && (md.heads >> h) & 1 ? hgN : 0.f); float g = headG[h].tick();
      float y = g > 1e-4f ? tape.read(T1 * HEAD_RATIO[h] * mod * sr) : 0.f; sum += y * g; headLvl[h] = maxf(headLvl[h] * 0.9995f, absf(y * g));
    }
    sum *= drop;
    // record: input volume drives the tape; feedback gets darker and saturated at each pass, never runs away
    inG.set(on && !freeze ? 0.4f + inVol * 1.4f : 0.f); float x = in * inG.tick(); peak = maxf(peak * 0.9997f, absf(x));
    float fbTarget = freeze ? (nh ? 1.f / (hgN * nh) : 0.f) : (nh ? intensity * 1.1f / (hgN * nh) : 0.f); fbG.set(fbTarget);
    ageLP.setLP(freeze ? 15000.f : 1400.f * powf_(8.f, 1.f - age), sr);
    float fb = ageLP.lp(sum); fb = fbHP.hp(fb); fb = softclip(fb, 0.75f);
    float rec = softclip(x, 0.9f) + fb * fbG.tick() + (rng.next() * age * 0.0006f);
    tape.push(rec);
    // output EQ (bass / treble, ±12 dB) on the echoes
    eqB.set(true, 220, (bass - 0.5f) * 24.f, sr); eqT.set(false, 3200, (treble - 0.5f) * 24.f, sr);
    outG.set(on ? echoVol * 1.6f : 0.f); float e = eqT.tick(eqB.tick(sum)) * outG.tick();
    // spring
    sprG.set(on && md.spring ? revVol * 1.2f : 0.f); float sg = sprG.tick();
    float sIn = (x * 0.6f + sum * 0.4f) + kick * (rng.next()) * 0.8f; kick *= 0.9985f;
    float sL = 0, sR = 0;
    if (sg > 1e-4f || kick > 1e-3f) {
      float d = sIn; for (int i = 0; i < 8; i++) d = ap[i].tick(d);
      float cl = cL.tap((int)(0.0331f * sr)), cr = cR.tap((int)(0.0397f * sr));
      cL.push(d + cLP.lp(cl) * 0.78f); cR.push(d * 0.9f + cRLP.lp(cr) * 0.76f);
      sL = cl * sg; sR = cr * sg; sprLvl = maxf(sprLvl * 0.9995f, absf(sL));
    } else { cL.push(0); cR.push(0); }
    // stereo: head 1 left, head 2 right, head 3 centre
    float l = e * 0.9f, r = e * 0.9f; (void)headLvl;
    outL += l + sL; outR += r + sR;
  }
};

// ================================================================ COSMOS (8-line FDN hall)
struct Cosmos {
  static constexpr int N = 1 << 14; Delay<N> d[8]; float len[8], g[8]; OnePole damp[8]; Delay<1 << 15> pre; float sr = 48000; float size = .79f, dampV = .47f, preV = .24f, vol = .72f;
  float apB[2][1200]; Allpass ap[2]; float lph = 0; Smooth sz; float lastSize = -1, lastDamp = -1;
  void init(float r) { sr = r; static const float base[8] = {29.7f, 37.1f, 41.1f, 43.7f, 50.3f, 57.1f, 63.7f, 71.3f};
    for (int i = 0; i < 8; i++) { len[i] = base[i]; g[i] = 0.8f; }
    for (int i = 0; i < 2; i++) { for (int j = 0; j < 1200; j++) apB[i][j] = 0; ap[i].buf = apB[i]; ap[i].n = (int)((i ? 12.7f : 4.7f) * sr / 1000.f); ap[i].g = 0.65f; }
    sz.time(0.8f, sr); sz.snap(size); update(); }
  void update() { float sc = 1.f + sz.v * 2.3f, rt = 1.2f + sz.v * 12.f, fc = 1800.f * powf_(6.7f, 1.f - dampV);
    for (int i = 0; i < 8; i++) { float dl = len[i] * sc / 1000.f; g[i] = minf(0.97f, powf_(10.f, -3.f * dl / rt)); damp[i].setLP(fc, sr); } }
  inline void process(float in, float& oL, float& oR, int i0) {
    sz.set(size); float s = sz.tick(); if ((i0 & 63) == 0 && (absf(s - lastSize) > 1e-4f || dampV != lastDamp)) { lastSize = s; lastDamp = dampV; update(); }
    pre.push(in); float x = pre.read(1.f + preV * 0.25f * sr); x = ap[1].tick(ap[0].tick(x));
    lph += 0.11f / sr; lph -= floorf_(lph); float sc = 1.f + s * 2.3f;
    float o[8], sum = 0;
    for (int i = 0; i < 8; i++) { float m = (i == 0 ? sinp(lph) : i == 3 ? cosp(lph) : 0.f) * 0.0008f * sr; o[i] = damp[i].lp(d[i].read(len[i] * sc * sr / 1000.f + m)); sum += o[i]; }
    float h = sum * 0.25f;   // Householder: y = x - 2/N * sum
    for (int i = 0; i < 8; i++) d[i].push(x * 0.35f + (o[i] - h) * g[i]);
    float l = o[0] - o[2] + o[4] - o[6], r = o[1] - o[3] + o[5] - o[7];
    oL += l * vol * 0.5f; oR += r * vol * 0.5f;
  }
};

// ================================================================ ENSEMBLE (BBD-style stereo chorus)
struct Ensemble { Delay<4096> dl; float ph = 0, sr = 48000;
  inline void process(float x, float amt, float rate, float& l, float& r) {
    dl.push(x); ph += rate / sr; ph -= floorf_(ph);
    float t = absf(2.f * ph - 1.f), base = 0.0045f * sr, dep = 0.0028f * sr;
    float a = dl.read(base + dep * t), b = dl.read(base + dep * (1.f - t));
    l = x * (1.f - amt * 0.5f) + a * amt * 0.7f; r = x * (1.f - amt * 0.5f) + b * amt * 0.7f; }
};

// ================================================================ LIVE FX RACK (stereo insert on the master)
struct FxRack {
  float sr = 48000; int type = 0, cur = 0; bool on = false; float x = .5f, y = .5f, mix = 1, beatSec = 0.5f; double beatPos = 0;
  Smooth wet, sx, sy; Ladder ld[2]; SVF sv[2], fm[2][3]; float ap1[2][6] = {{0}}; float lfo = 0, envF = 0;
  Delay<8192> fl[2]; Ensemble ens[2]; static constexpr int RN = 1 << 18; float rep[2][RN]; int repLen = 0, repPos = 0, repRec = 0, repW = 0; bool repLock = false;
  float crushH[2] = {0, 0}; int crushC = 0; static constexpr int HN = 1 << 18; float hb[2][HN]; int hw = 0; double hr = 0; float hs = 1; float rph = 0;
  void init(float r) { sr = r; wet.time(0.06f, sr); sx.time(0.05f, sr); sy.time(0.05f, sr); for (auto& e : ens) e.sr = sr; for (int c = 0; c < 2; c++) { for (int i = 0; i < RN; i++) rep[c][i] = 0; for (int i = 0; i < HN; i++) hb[c][i] = 0; } }
  void setOn(bool o) { if (o && !on) { repLock = false; repRec = 0; repPos = 0; } on = o; }
  inline void process(float& L, float& R) {
    if (type != cur) { if (wet.v < 0.002f) cur = type; }   // change type only when faded out: no clicks
    wet.set(on && type == cur ? mix : 0.f); float w = wet.tick(); float X = sx.tick(), Y = sy.tick(); sx.set(x); sy.set(y);
    // half-speed keeps its buffer running so it can engage smoothly
    hb[0][hw] = L; hb[1][hw] = R; hw = (hw + 1) & (HN - 1);
    if (w < 1e-4f) { hr = hw; hs = 1; repLock = false; repRec = 0; return; }
    float in[2] = {L, R}, out[2];
    lfo += 1.f / sr;
    for (int c = 0; c < 2; c++) {
      float s = in[c], o = s;
      switch (cur) {
        case 0: ld[c].set(80.f * powf_(200.f, X), Y * 0.95f, sr); o = ld[c].tick(s * 0.8f) * 1.25f; break;
        case 1: { envF = maxf(envF * 0.9995f, absf(s)); ld[c].set(minf(120.f * powf_(100.f, X) * (1.f + envF * 3.f), sr * 0.4f), 0.6f + Y * 0.38f, sr); o = softclip(ld[c].tick(softclip(s * 2.2f, 1.f)) * 1.1f, 0.8f); break; }
        case 2: { if ((crushC & 3) == 0) sv[c].set(60.f * powf_(250.f, X), 0.7f + Y * 10.f, sr); float l, b, h; sv[c].tick(softclip(s * 1.8f, 0.9f), l, b, h); o = softclip(l + b * Y * 0.3f, 0.75f); break; }
        case 3: if ((crushC & 3) == 0) sv[c].set(20.f * powf_(400.f, X), 0.6f + Y * 6.f, sr); o = sv[c].hp(s); break;
        case 4: if ((crushC & 3) == 0) sv[c].set(150.f * powf_(60.f, X), 0.8f + Y * 9.f, sr); o = sv[c].bp(s) * (1.f + Y); break;
        case 5: { float fi = X * 3.999f; int a = (int)fi, b2 = a + 1 > 4 ? 4 : a + 1; float fr = fi - a; o = 0;
          for (int k = 0; k < 3; k++) { if ((crushC & 7) == 0) fm[c][k].set(lerpf(VOWELS[a][k], VOWELS[b2][k], fr), 6.f + Y * 10.f, sr); o += fm[c][k].bp(s) * (k == 0 ? 1.f : k == 1 ? 0.6f : 0.35f); } o *= 2.2f; break; }
        case 6: { envF = maxf(envF * 0.9993f, absf(s)); if ((crushC & 3) == 0) sv[c].set(minf(200.f + envF * 9000.f * (0.2f + X), sr * 0.4f), 1.f + Y * 8.f, sr); o = sv[c].bp(s) * 1.6f + sv[c].lp(0) * 0; break; }
        case 7: { float f = 120.f * powf_(60.f, clampf(X + 0.25f * sinp(lfo * (0.05f + Y * 2.f)), 0, 1)); if ((crushC & 3) == 0) sv[c].set(f, 1.2f, sr); float l, b, h; sv[c].tick(s, l, b, h); o = l + h; break; }
        case 8: { float m = 0.5f + 0.5f * sinp(lfo * (0.05f + X * 2.f) + c * 0.25f); float a = 0.15f + m * 0.8f; float v = s + ap1[c][5] * Y * 0.7f;
          for (int k = 0; k < 6; k++) { float z = ap1[c][k]; float y2 = -a * v + z; ap1[c][k] = v + a * y2; v = y2; } o = (s + v) * 0.7f; break; }
        case 9: { fl[c].push(s + fl[c].tap(1) * 0); float m = 0.5f + 0.5f * sinp(lfo * (0.03f + X * 0.8f) + c * 0.1f); float d = (0.0006f + m * 0.007f) * sr; float dd = fl[c].read(d); o = (s + dd) * 0.6f;
          fl[c].b[(fl[c].w + 8191) & 8191] += dd * Y * 0.85f; break; }
        case 10: { float l2, r2; ens[c].process(s, 0.4f + Y * 0.6f, 0.4f + X * 5.f, l2, r2); o = c ? r2 : l2; break; }
        case 11: { // REPEAT: grab 1/4..1/32 at the moment it engages (engine quantizes the start), loop it, Y fades the repeats
          int divs[4] = {1, 2, 4, 8}; int len = (int)(beatSec * sr / divs[clampi((int)(X * 3.999f), 0, 3)]); len = clampi(len, 64, RN - 1);
          if (!repLock) { if (c == 0) repRec++; rep[c][repRec % RN] = s; if (c == 1 && repRec >= len) { repLock = true; repLen = len; repPos = 0; } o = s; }
          else { int p = repPos % repLen; float fade = p < 64 ? p / 64.f : p > repLen - 64 ? (repLen - p) / 64.f : 1.f; o = rep[c][p + 1] * fade * (1.f - Y * 0.5f * (repPos / repLen > 0 ? 1.f : 0.f)); if (c == 1) repPos++; }
          break; }
        case 12: { float bits = 16.f - X * 12.f, q = exp2f_(bits - 1.f); int down = 1 + (int)(Y * 15.f); if (crushC % down == 0) crushH[c] = floorf_(s * q + 0.5f) / q; o = crushH[c]; break; }
        case 13: { // HALF-SPEED: read the recent past slower and slower (X: how slow, down to a stop)
          if (c == 0) { float tgt = 1.f - X * 0.98f; hs += (tgt - hs) * 0.00008f; hr += hs; if (hw - hr > HN - 1000) hr = hw - 2000; }
          double p = hr - 1; int i = ((int)p) & (HN - 1); float f = (float)(p - floorf_((float)p)); o = hb[c][i] + (hb[c][(i + 1) & (HN - 1)] - hb[c][i]) * f; break; }
        case 14: { // GATE: 1/16 trance gate, X = pattern, Y = depth
          static const uint16_t PAT[4] = {0xFFFF, 0xEEEE, 0xB6B6, 0x9249}; double ph = beatPos * 4.0; int st = ((int)ph) & 15; float f = (float)(ph - floorf_((float)ph));
          bool g = (PAT[clampi((int)(X * 3.999f), 0, 3)] >> (15 - st)) & 1; float env = g ? (f < 0.05f ? f / 0.05f : f > 0.7f ? (1.f - f) / 0.3f : 1.f) : 0.f; o = s * (1.f - Y + Y * env); break; }
        case 15: { rph += 30.f * powf_(60.f, X) / sr; rph -= floorf_(rph); o = s * (1.f - Y) + s * sinp(rph) * Y * 1.4f; break; }
      }
      out[c] = o;
    }
    crushC++;
    L = L + (out[0] - L) * w; R = R + (out[1] - R) * w;
  }
};

// ================================================================ MASTER
struct Master {
  float sr = 48000; Smooth haze, grav, vol, stop; SVF lp[2], hp[2], hz[2]; float env = 0, gr = 1; DCBlock dc[2]; float crush[2] = {0, 0}; int cc = 0;
  static constexpr int SN = 1 << 17; float sb[2][SN]; int sw = 0; double srd = 0; float spd = 1, spdT = 1;
  void init(float r) { sr = r; haze.time(0.08f, sr); grav.time(0.06f, sr); vol.time(0.05f, sr); for (int c = 0; c < 2; c++) for (int i = 0; i < SN; i++) sb[c][i] = 0; }
  void tapeStop(bool on) { spdT = on ? 0.f : 1.f; }
  inline void process(float& L, float& R) {
    float h = haze.tick(), g = grav.tick(), v = vol.tick();
    // tape stop: varispeed read of the recent past (motor slows to a halt, then spins back up)
    sb[0][sw] = L; sb[1][sw] = R; sw = (sw + 1) & (SN - 1);
    spd += (spdT - spd) * (spdT < spd ? 0.00004f : 0.0003f);
    if (spd < 0.999f || srd != 0) { srd += 1.0 - spd; if (spd > 0.998f && spdT >= 1.f) { srd *= 0.995; if (srd < 1) srd = 0; }
      if (srd > SN - 100) srd = SN - 100; double p = sw - 1 - srd; int i = ((int)(p + SN * 4)) & (SN - 1); float f = (float)(p - floorf_((float)p));
      L = sb[0][i] + (sb[0][(i + 1) & (SN - 1)] - sb[0][i]) * f; R = sb[1][i] + (sb[1][(i + 1) & (SN - 1)] - sb[1][i]) * f; float ga = clampf(spd * 1.5f, 0.f, 1.f); L *= ga; R *= ga; }
    float x[2] = {L, R};
    bool sv = (cc & 7) == 0;
    for (int c = 0; c < 2; c++) {
      float s = x[c];
      if (h > 0.003f) { if (sv) hz[c].set(20000.f * powf_(0.18f, h), 0.5f, sr); s = hz[c].lp(s); s = softclip(s * (1.f + h * 1.6f), 0.92f) / (1.f + h * 1.1f);
        float crush = maxf(0.f, (h - 0.5f) * 2.f); if (crush > 0) { int down = 1 + (int)(crush * 5); if (cc % down == 0) { float q = exp2f_(13.f - crush * 7.f); crush_hold(c, floorf_(s * q + .5f) / q); } s = s + (this->crush[c] - s) * crush * 0.8f; } }
      if (g < 0.47f) { if (sv) lp[c].set(20000.f * powf_(140.f / 20000.f, (0.47f - g) / 0.47f), 0.75f, sr); s = lp[c].lp(s); }
      if (g > 0.53f) { if (sv) hp[c].set(10.f * powf_(250.f, (g - 0.53f) / 0.47f), 0.75f, sr); s = hp[c].hp(s); }
      x[c] = dc[c].tick(s);
    }
    cc++;
    // glue: gentle 2:1 above -18 dB, slow, musical
    float lvl = maxf(absf(x[0]), absf(x[1])); env += (lvl - env) * (lvl > env ? 0.002f : 0.00008f);
    float over = env > 0.126f ? env / 0.126f : 1.f; float target = over > 1.f ? 1.f / sqrtf_(over) : 1.f; gr += (target - gr) * 0.0005f;
    L = softclip(x[0] * gr * 1.25f * v, 0.72f); R = softclip(x[1] * gr * 1.25f * v, 0.72f);   // soft ceiling ≈ −3 dBFS
  }
  inline void crush_hold(int c, float v) { crush[c] = v; }
};

} // namespace andro
