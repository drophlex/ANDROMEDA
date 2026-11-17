#include "api.h"
#include <cstdio>
#include <cmath>
using namespace andro;
int main(int c, char** v) {
  float sr = c > 1 ? atof(v[1]) : 96000;
  static Engine E; E.init(sr); int a[16] = {}; command(E, C_PLAY, a);
  static float L[128], R[128];
  for (int b = 0; b < (int)(sr * 20 / 128); b++) { E.process(L, R, nullptr, nullptr, 128);
    for (int i = 0; i < 128; i++) if (!std::isfinite(L[i]) || !std::isfinite(R[i])) { printf("NaN at block %d (%.2fs)\n", b, b * 128 / sr); return 1; } }
  printf("ok %f\n", E.tele[11]); return 0; }
