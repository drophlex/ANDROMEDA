// ANDROMEDA · WebAssembly entry (no libc): the same engine as the plugin
#include "api.h"
extern "C" {
void* memset(void* d, int c, unsigned long n) { unsigned char* p = (unsigned char*)d; while (n--) *p++ = (unsigned char)c; return d; }
void* memcpy(void* d, const void* s, unsigned long n) { unsigned char* a = (unsigned char*)d; const unsigned char* b = (const unsigned char*)s; while (n--) *a++ = *b++; return d; }
void* memmove(void* d, const void* s, unsigned long n) { unsigned char* a = (unsigned char*)d; const unsigned char* b = (const unsigned char*)s; if (a < b) while (n--) *a++ = *b++; else { a += n; b += n; while (n--) *--a = *--b; } return d; }
int memcmp(const void* a, const void* b, unsigned long n) { const unsigned char* x = (const unsigned char*)a; const unsigned char* y = (const unsigned char*)b; for (; n; n--, x++, y++) if (*x != *y) return *x - *y; return 0; }
}
using namespace andro;
static Engine E;
static float inL[128], inR[128], outL[128], outR[128];
static int args[16];
static uint8_t proj[1 << 16];
constexpr int FILE_MAX = 48000 * 50;
static float fileL[FILE_MAX], fileR[FILE_MAX];
#define EXPORT extern "C" __attribute__((visibility("default")))
EXPORT void andro_init(float sr) { E.init(sr); }
EXPORT void andro_process(int n, int hasIn) { E.process(outL, outR, hasIn ? inL : nullptr, hasIn ? inR : nullptr, n); }
EXPORT float* andro_in_l() { return inL; }
EXPORT float* andro_in_r() { return inR; }
EXPORT float* andro_out_l() { return outL; }
EXPORT float* andro_out_r() { return outR; }
EXPORT int* andro_args() { return args; }
EXPORT void andro_cmd(int op) { command(E, op, args); }
EXPORT void andro_param(int id, float v) { E.setParam(id, v); }
EXPORT float andro_param_get(int id) { return id >= 0 && id < P_COUNT ? E.params[id] : 0.f; }
EXPORT int andro_param_count() { return P_COUNT; }
EXPORT float* andro_tele() { return E.tele; }
EXPORT float* andro_scope() { return E.scope; }
EXPORT int andro_scope_w() { return E.scopeW; }
EXPORT int* andro_ui() { return (int*)E.ui; }
EXPORT int andro_ui_w() { return E.uiW; }
EXPORT uint8_t* andro_proj() { return proj; }
EXPORT int andro_proj_write() { return writeProject(E.PJ, E.params, proj, sizeof proj); }
EXPORT int andro_proj_read(int len) { Project P; if (!readProject(P, E.params, proj, len)) return 0; E.PJ = P; E.snapshotSeeds(); E.syncSoundParams(); E.projVersion++; return 1; }
EXPORT int andro_preset_count() { return presets().n; }
EXPORT const char* andro_preset_name(int i) { return presets().p[i].name; }
EXPORT const char* andro_preset_cat(int i) { return presets().p[i].cat; }
EXPORT float* andro_file_l() { return fileL; }
EXPORT float* andro_file_r() { return fileR; }
EXPORT int andro_file_max() { return FILE_MAX; }
EXPORT void andro_file_set(int len, int stereo) { E.fileL = fileL; E.fileR = stereo ? fileR : nullptr; E.fileLen = len < FILE_MAX ? len : FILE_MAX; E.filePos = 0; }
EXPORT void andro_host(int playing, float bpm, double ppq, int sync) { E.hostSync = sync; E.hostPlaying = playing; E.hostBpm = bpm; if (sync) E.hostPpq = ppq; }
