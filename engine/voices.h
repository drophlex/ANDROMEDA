// ANDROMEDA engine · voices.h — synth voices, drum voices, textures (click-free: every start and steal is enveloped)
#pragma once
#include "data.h"

namespace andro {

struct SynthVoice {
  bool on = false; int note = 0, age = 0; float freq = 440, f0 = 440, glideK = 1, vel = 1, gain = 0.2f; bool chordTop = false;
  Env amp, fenv; Osc o[5]; SVF filt; SVF res[3]; float mph = 0, cph = 0, idx = 0, idxK = 0, idx0 = 0, pw = 0.5f, pwph = 0;
  OnePole nlp; uint32_t ns = 1;
  const uint8_t* P = nullptr; float sr = 48000; int nIdx = 0;
  inline float noise() { ns ^= ns << 13; ns ^= ns >> 17; ns ^= ns << 5; return (float)(ns >> 8) * (1.f / 16777216.f) * 2.f - 1.f; }
  void start(const uint8_t* patch, int midi, float velocity, float rate, int noteIdx, float glideFrom, uint32_t seed) {
    P = patch; sr = rate; nIdx = noteIdx; note = midi; vel = velocity; age = 0; ns = seed | 1;
    int oct = (int)P[PF_OCT] - 64; f0 = mtof((float)(midi + 12 * oct));
    freq = glideFrom > 0 ? glideFrom : f0; glideK = 1.f - expf_(-1.f / maxf(1.f, (0.004f + tSec(P[PF_GLIDE]) * 0.12f) * sr));
    amp.setSR(sr); fenv.setSR(sr);
    amp.set(tSec(P[PF_A]), tSec(P[PF_D]), u127(P[PF_S]), tSec(P[PF_R])); amp.v = 0; amp.on();
    fenv.set(0.002f + tSec(P[PF_FA]) * 0.6f, tSec(P[PF_FD]), u127(P[PF_S]) * 0.4f, tSec(P[PF_R])); fenv.v = 0; fenv.on();
    for (int i = 0; i < 5; i++) { o[i].ph = (float)((seed >> (i * 5)) & 31) / 32.f; o[i].tri = 0; }
    filt.reset(); for (auto& r : res) r.reset();
    int eng = P[PF_ENG];
    gain = eng == E_FM ? 0.30f : eng == E_CHOIR ? 0.34f : eng == E_BOWED ? 0.9f : 0.22f;
    idx0 = (0.3f + u127(P[PF_INDEX]) * 6.f); idx = idx0 * (0.6f + velocity * 0.5f); idxK = 1.f - expf_(-1.f / maxf(1.f, (tSec(P[PF_IDEC]) * 0.33f + 0.002f) * sr));
    mph = cph = 0; on = true; nlp.setLP(eng == E_CHOIR ? 6000.f : minf(f0 * 4.f, 12000.f), sr);
  }
  void release() { amp.off(); fenv.off(); }
  void kill() { amp.kill(); }
  // render adds mono into out; cents = pitch drift for this block (shared track LFO)
  void render(float* out, int n, float cents, float cutMul) {
    if (!on) return; const uint8_t* p = P; int eng = p[PF_ENG];
    float pr = exp2f_(cents / 1200.f);
    float fc0 = expMap(p[PF_CUT], 40.f, 16000.f) * powf_(f0 / 261.6f, 0.35f) * cutMul, envOct = u127(p[PF_ENV]) * 5.5f;
    float q = 0.6f + u127(p[PF_RES]) * u127(p[PF_RES]) * 14.f;
    float det = u127(p[PF_DET]) * 0.014f, mix2 = eng == E_BASS ? 0 : u127(p[PF_MIX]) * 0.5f, sub = nIdx == 0 ? u127(p[PF_SUB]) * 0.6f : 0, nz = nIdx == 0 ? u127(p[PF_NOISE]) * (eng == E_CHOIR ? 0.3f : eng == E_BOWED ? 1.f : 0.16f) : 0;
    int w1 = p[PF_W1], w2 = p[PF_W2], uni = nIdx > 0 ? 1 : clampi(p[PF_UNI], 1, 2);
    float pwmD = u127(p[PF_PWM]) * 0.4f;
    for (int i = 0; i < n; i++) {
      freq += (f0 - freq) * glideK; float f = freq * pr, dt = f / sr;
      float a = amp.tick(); if (!amp.active()) { on = false; return; }
      float fe = fenv.tick(), y = 0;
      if (eng == E_FM) {
        idx += (idx0 * 0.04f - idx) * idxK;
        float ratio = FM_RATIOS[(int)(u127(p[PF_RATIO]) * 15.f)], mf = minf(f * ratio, sr * 0.45f);
        float m = sinp(mph) * minf(idx * 0.8f, 3000.f / maxf(f, 1.f)); mph += mf / sr; mph -= floorf_(mph);
        y = sinp(cph + m * 0.159f); cph += dt; cph -= floorf_(cph);
      } else if (eng == E_BOWED) {
        float ex = nlp.lp(noise()) * nz + o[0].tick(W_SAW, dt) * 0.04f;
        int set = (int)(u127(p[PF_RATIO]) * 3.99f); float rq = 20.f + u127(p[PF_RES]) * 140.f;
        if ((age & 31) == 0) for (int k = 0; k < 3; k++) res[k].set(minf(f * BOW_SETS[set][k], sr * 0.44f), rq / (1.f + k), sr);
        y = res[0].bp(ex) * 1.f + res[1].bp(ex) * 0.6f + res[2].bp(ex) * 0.35f;
      } else {
        pwph += 0.37f / sr; pwph -= floorf_(pwph); pw = 0.5f + pwmD * sinp(pwph + nIdx * 0.21f) * (w1 == W_PULSE || w2 == W_PULSE ? 1.f : 0.f);
        if (uni == 1) y = o[0].tick(w1, dt, pw) * 0.5f; else y = (o[0].tick(w1, dt * (1.f + det), pw) + o[1].tick(w1, dt * (1.f - det), 1.f - pw)) * 0.25f;
        if (mix2 > 0.01f) y += o[2].tick(w2, dt * (1.f - det * 0.6f), pw) * mix2;
        if (sub > 0.01f) y += o[3].tick(W_SINE, dt * 0.5f) * sub;
        if (nz > 0.001f) y += nlp.lp(noise()) * nz;
      }
      if (eng != E_BOWED) { if ((age & 7) == 0) filt.set(fc0 * exp2f_(envOct * fe), q, sr); y = filt.lp(y); }
      out[i] += y * a * gain * vel;
      age++;
    }
  }
};

// ------------------------------------------------------------------ drums
struct DrumVoice {
  bool on = false; int v = 0; float t = 0, sr = 48000, vel = 1, len = 0.5f; uint32_t ns = 7; const Kit* k = nullptr;
  float ph = 0, ph2 = 0, mph[6] = {0, 0, 0, 0, 0, 0}; SVF f1, f2; float pitch = 0;
  inline float noise() { ns ^= ns << 13; ns ^= ns >> 17; ns ^= ns << 5; return (float)(ns >> 8) * (1.f / 16777216.f) * 2.f - 1.f; }
  void start(int voice, const Kit* kit, float velocity, float rate, uint32_t seed) {
    v = voice; k = kit; vel = velocity; sr = rate; t = 0; ns = seed | 1; ph = ph2 = 0; for (auto& m : mph) m = 0; f1.reset(); f2.reset(); on = true;
    float tone = u127(k->tone), ht = u127(k->ht);
    switch (v) {
      case 1: f1.set(1800 + tone * 3500, 1.2f, sr); len = 0.12f + u127(k->sn) * 0.25f; break;
      case 2: f1.set(7000 + ht * 3000, 0.9f, sr); len = 0.05f + ht * 0.04f; break;
      case 3: f1.set(6500 + ht * 2500, 0.9f, sr); len = 0.35f + ht * 0.2f; break;
      case 4: f1.set(1300, 2.5f, sr); len = 0.25f; break;
      case 5: f1.set(1700, 6, sr); len = 0.05f; break;
      case 7: f1.set(2400, 3, sr); len = 0.3f; break;
      case 8: f1.set(7000, 1.6f, sr); len = 0.09f; break;
      case 10: f1.set(800, 3, sr); len = 0.25f; break;
      default: len = 0.5f;
    }
    if (v == 9) pitch = 900 + velocity * 1600;
  }
  // adds into out; returns false when finished
  void render(float* out, int n) {
    if (!on) return; const float isr = 1.f / sr; float kp = u127(k->kp), kd = u127(k->kd), kc = u127(k->kc), st = u127(k->st), metal = u127(k->metal);
    for (int i = 0; i < n; i++) {
      float y = 0, e;
      switch (v) {
        case 0: { float lo = 38 + kp * 26, d = 0.16f + kd * 0.95f; float f = lo + lo * 2.6f * expf_(-t / (0.035f + kd * 0.04f)); ph += f * isr; ph -= floorf_(ph);
          e = expf_(-t / (d * 0.32f)); y = sinp(ph) * e * 0.95f; if (t < 0.012f) y += noise() * kc * 0.45f * (1.f - t / 0.012f); if (t > d * 1.8f) on = false; break; }
        case 1: { float tf = 150 + st * 120; float f = tf + tf * 0.6f * expf_(-t / 0.03f); ph += f * isr; ph -= floorf_(ph);
          y = (2.f * absf(2.f * ph - 1.f) - 1.f) * 0.45f * expf_(-t / 0.04f) + f1.bp(noise()) * 1.4f * expf_(-t / (len * 0.3f)); if (t > len * 2.f) on = false; break; }
        case 2: case 3: { float m = 0; static const float MF[6] = {205.3f, 304.4f, 369.6f, 522.7f, 540.f, 800.f};
          for (int q = 0; q < 6; q++) { mph[q] += MF[q] * 1.7f * isr; mph[q] -= floorf_(mph[q]); m += mph[q] < 0.5f ? 1.f : -1.f; }
          float src = noise() * (1.f - metal * 0.6f) + m * 0.16f * metal; y = f1.hp(src) * 0.55f * expf_(-t / (len * 0.3f)) * (t < 0.002f ? t / 0.002f : 1.f); if (t > len * 2.f) on = false; break; }
        case 4: { float g = 0; const float o[3] = {0, 0.011f, 0.023f}; for (int q = 0; q < 3; q++) if (t >= o[q]) g += (q == 2 ? 0.9f : 0.6f) * expf_(-(t - o[q]) / (q == 2 ? 0.06f : 0.005f));
          y = f1.bp(noise()) * g * 1.6f; if (t > 0.4f) on = false; break; }
        case 5: { ph += 1700 * isr; ph2 += 480 * isr; ph -= floorf_(ph); ph2 -= floorf_(ph2); y = f1.bp((2 * absf(2 * ph - 1) - 1) * 0.6f + (ph2 < .5f ? .3f : -.3f)) * 3.f * expf_(-t / 0.012f); if (t > 0.08f) on = false; break; }
        case 6: { float tf = 80 + st * 60; float f = tf + tf * 1.2f * expf_(-t / 0.05f); ph += f * isr; ph -= floorf_(ph); y = sinp(ph) * 0.8f * expf_(-t / 0.1f); if (t > 0.5f) on = false; break; }
        case 7: { float cf = 240 + u127(k->tone) * 200; mph[0] += cf * 3.14f * isr; mph[0] -= floorf_(mph[0]); ph += (cf + sinp(mph[0]) * 900) * isr; ph -= floorf_(ph);
          y = f1.bp(ph < 0.5f ? 1.f : -1.f) * 1.2f * expf_(-t / 0.06f) + noise() * 0.1f * expf_(-t / 0.03f); if (t > 0.35f) on = false; break; }
        case 8: { float a = t < 0.012f ? t / 0.012f : expf_(-(t - 0.012f) / 0.03f); y = f1.bp(noise()) * a * 0.9f; if (t > 0.15f) on = false; break; }
        case 9: { float f = pitch * (1.f + 1.2f * minf(1.f, t / 0.04f)); ph += f * isr; ph -= floorf_(ph); y = sinp(ph) * 0.4f * expf_(-t / 0.015f); if (t > 0.1f) on = false; break; }
        case 10: { ph += 540 * isr; ph2 += 800 * isr; ph -= floorf_(ph); ph2 -= floorf_(ph2); y = f1.bp((ph < .5f ? 1.f : -1.f) + (ph2 < .5f ? 1.f : -1.f)) * 0.35f * expf_(-t / 0.08f); if (t > 0.4f) on = false; break; }
      }
      out[i] += y * vel * 0.5f; t += isr;
      if (!on) return;
    }
  }
};

// ------------------------------------------------------------------ textures (continuous beds + events)
struct Texture {
  int type = 0; float sr = 48000, t = 0, swell = 1, swellT = 1, mot = 0.5f; uint32_t ns = 99; SVF f1, f2; OnePole lp, lp2; float ph[4] = {0, 0, 0, 0};
  float drops[8][3]; int key = 2;
  inline float noise() { ns ^= ns << 13; ns ^= ns >> 17; ns ^= ns << 5; return (float)(ns >> 8) * (1.f / 16777216.f) * 2.f - 1.f; }
  Texture() { for (auto& d : drops) d[0] = d[1] = d[2] = 0; }
  void event(float vel) { swellT = 1.4f + vel; if (type == 1 || type == 2) for (int i = 0; i < 4; i++) addDrop(); }
  void addDrop() { for (auto& d : drops) if (d[1] <= 0) { d[0] = 1800 + (noise() * 0.5f + 0.5f) * 2400; d[1] = 0.02f + (noise() * .5f + .5f) * 0.05f; d[2] = 0; return; } }
  void render(float* out, int n, float level, float tone) {
    const float isr = 1.f / sr;
    for (int i = 0; i < n; i++) {
      swell += (swellT - swell) * 0.00005f; swellT += (1.f - swellT) * 0.00002f;
      float y = 0; t += isr;
      switch (type) {
        case 0: { float s = 0.05f + mot * 0.1f; ph[0] += s * isr; ph[1] += 0.031f * isr; if ((i & 15) == 0) f1.set(600 + 400 * sinp(ph[0]) + 250 * sinp(ph[1]), 1.6f, sr); y = f1.bp(noise()) * (0.55f + 0.45f * sinp(ph[0] * 1.7f)); break; }
        case 1: { if ((i & 15) == 0) f1.set(3000, 0.6f, sr); y = f1.lp(noise() - lp.lp(noise())) * 0.18f; if (((ns >> 3) & 4095) < 2) addDrop(); break; }
        case 2: { float c = 0; if (((ns >> 5) & 8191) < 2) c = noise() * 0.5f; ph[2] = ph[2] * 0.6f + c; ph[3] += (ph[2] - ph[3]) * 0.35f; y = ph[3] * 0.05f + lp.lp(noise()) * 0.22f; if (t < isr * 2) lp.setLP(90, sr); break; }   // soft dust, never sharp
        case 3: { if (t < isr * 2) { lp.setLP(11000, sr); lp2.setLP(2500, sr); } float x = noise(); y = (lp.lp(x) - lp2.lp(x)) * 0.16f * (0.96f + 0.04f * sinp(t * 0.4f)); break; }
        case 4: { float w = 0.06f + mot * 0.1f; ph[0] += w * isr; float s = 0.5f + 0.5f * sinp(ph[0]); if ((i & 15) == 0) f1.set(300 + 1100 * s, 0.6f, sr); y = f1.lp(noise()) * (0.15f + 0.6f * s); break; }
        case 5: { ph[0] += (0.07f + mot * 0.2f) * isr; if ((i & 15) == 0) f1.set(1400 + 900 * sinp(ph[0]), 7, sr); ph[1] += (1100 + 700 * sinp(ph[0] * 0.7f)) * isr; ph[1] -= floorf_(ph[1]);
          y = f1.bp(noise()) * 0.8f + sinp(ph[1]) * 0.035f * (0.6f + 0.4f * sinp(t * 7)); break; }
        case 6: { float r = mtof(33.f + key); float s = 0; for (int k = 0; k < 3; k++) { ph[k] += r * (k == 0 ? 1 : k == 1 ? 1.5f : 2.f) * (1.f + 0.003f * k) * isr; ph[k] -= floorf_(ph[k]); s += (2 * ph[k] - 1) * 0.12f / (k + 1); }
          ph[3] += (0.04f + mot * 0.08f) * isr; if ((i & 15) == 0) f1.set(260 + 140 * sinp(ph[3]), 2, sr); y = f1.lp(s); break; }
      }
      for (auto& d : drops) if (d[1] > 0) { d[2] += d[0] * isr; d[0] *= 0.99997f; y += sinp(d[2]) * d[1]; d[1] *= 0.9985f; if (d[1] < 1e-4f) d[1] = 0; }
      if ((i & 15) == 0) f2.set(400 * powf_(45.f, tone), 0.5f, sr);
      out[i] += f2.lp(y) * level * swell;
    }
  }
};

} // namespace andro
