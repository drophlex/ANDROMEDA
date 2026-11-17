// native test: render every mood (song), check levels, clicks and CPU
#include "../api.h"
#include <cstdio>
#include <cmath>
#include <chrono>
#include <vector>
#include <cstring>
using namespace andro;
static void wav(const char* fn, const std::vector<float>& L, const std::vector<float>& R, int sr) {
  FILE* f = fopen(fn, "wb"); int n = (int)L.size(); int data = n * 4; auto u32 = [&](uint32_t v) { fwrite(&v, 4, 1, f); }; auto u16 = [&](uint16_t v) { fwrite(&v, 2, 1, f); };
  fwrite("RIFF", 1, 4, f); u32(36 + data); fwrite("WAVEfmt ", 1, 8, f); u32(16); u16(1); u16(2); u32(sr); u32(sr * 4); u16(4); u16(16); fwrite("data", 1, 4, f); u32(data);
  for (int i = 0; i < n; i++) { int16_t a = (int16_t)(fmaxf(-1, fminf(1, L[i])) * 32767), b = (int16_t)(fmaxf(-1, fminf(1, R[i])) * 32767); fwrite(&a, 2, 1, f); fwrite(&b, 2, 1, f); }
  fclose(f);
}
int main(int argc, char** argv) {
  const int sr = 48000; float secs = argc > 1 ? atof(argv[1]) : 20; int only = argc > 2 ? atoi(argv[2]) : -1;
  static Engine E;
  for (int m = 0; m < NMOODS; m++) {
    if (only >= 0 && m != only) continue;
    E.init(sr); E.rng.s = 1234 + m; E.align(m, 0, true); E.PJ.songOn = 0; E.switchScene(2); E.play();
    int N = (int)(secs * sr), B = 256; std::vector<float> L(N), R(N);
    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; i += B) E.process(&L[i], &R[i], nullptr, nullptr, std::min(B, N - i));
    double ms = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - t0).count();
    // stats: peak, rms, clicks = jumps far above the local slope
    float pk = 0; double s2 = 0; int clicks = 0, nan = 0;
    for (int i = 2; i < N; i++) { float x = L[i]; if (!std::isfinite(x)) { nan++; continue; } pk = fmaxf(pk, fabsf(x)); s2 += x * x;
      float d1 = L[i] - L[i - 1], d0 = L[i - 1] - L[i - 2]; if (fabsf(d1 - d0) > 0.25f) clicks++; }
    printf("%-8s bpm %5.1f  load %5.1f%%  peak %.2f  rms %.3f  clicks %d  nan %d\n", MOODS[m].name, E.PJ.bpm, ms / (secs * 1000) * 100, pk, sqrt(s2 / N), clicks, nan);
    char fn[64]; snprintf(fn, 64, "out_%s.wav", MOODS[m].id); wav(fn, L, R, sr);
  }
  // live: keyboard intervention, out-of-scale note shifts the key on the beat; perf moves
  E.init(sr); E.align(2, 0, false); E.play(); std::vector<float> L(sr * 12), R(sr * 12);
  for (int i = 0, k = 0; i < (int)L.size(); i += 256, k++) {
    if (k == 400) { int a[3] = {5, 61, 100}; command(E, C_NOTE, a); }
    if (k == 420) { int a[3] = {5, 61, 0}; command(E, C_NOTE, a); }
    if (k == 600) { E.setParam(P_FX_TYPE, 11 / 15.f); E.setParam(P_FX_ON, 1); }
    if (k == 900) E.setParam(P_FX_ON, 0);
    if (k == 1000) E.perf(PERF_NOVA, true);
    if (k == 1500) E.perf(PERF_FREEZE, true);
    if (k == 1800) E.perf(PERF_STOP, true);
    if (k == 1900) E.perf(PERF_STOP, false);
    E.process(&L[i], &R[i], nullptr, nullptr, 256);
  }
  float pk = 0; for (float x : L) pk = fmaxf(pk, fabsf(x)); printf("live test peak %.2f key now %d scale %d\n", pk, E.PJ.key, E.PJ.scale);
  wav("out_live.wav", L, R, sr);
  // state round trip
  static uint8_t buf[1 << 16]; int n = writeProject(E.PJ, E.params, buf, sizeof buf); Project Q; float pr[P_COUNT]; bool ok = readProject(Q, pr, buf, n);
  printf("project bytes %d, read %s, same bpm %d\n", n, ok ? "ok" : "FAIL", Q.bpm == E.PJ.bpm);
}
