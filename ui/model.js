/* model.js — the project as the UI sees it (decoded from the engine's bytes; same layout as engine/api.h) */
const TRACKS = [
  { id: 'drums', n: 'DRUMS', kind: 'drum', planet: 'MARS', col: '#c4714a', base: 36, lo: 0, hi: 127 },
  { id: 'bass', n: 'BASS', kind: 'synth', planet: 'SATURN', col: '#a8906a', base: 36, lo: 28, hi: 57 },
  { id: 'pad', n: 'PAD', kind: 'synth', planet: 'VENUS', col: '#d9cbb3', base: 60, lo: 40, hi: 86 },
  { id: 'voice', n: 'VOICE', kind: 'synth', planet: 'MOON', col: '#b7b3c9', base: 60, lo: 45, hi: 86 },
  { id: 'bells', n: 'BELLS', kind: 'synth', planet: 'MERCURY', col: '#8fb0c0', base: 72, lo: 52, hi: 98 },
  { id: 'lead', n: 'LEAD', kind: 'synth', planet: 'JUPITER', col: '#d0a35c', base: 72, lo: 48, hi: 93 },
  { id: 'tex', n: 'TEXTURE', kind: 'tex', planet: 'NEPTUNE', col: '#7f9a8a', base: 48, lo: 0, hi: 127 },
  { id: 'input', n: 'INPUT', kind: 'input', planet: 'PLUTO', col: '#c98b86', base: 60, lo: 0, hi: 127 }
];
const RATES = [{ n: '1/1', s: 16 }, { n: '1/2', s: 8 }, { n: '1/4', s: 4 }, { n: '1/8', s: 2 }, { n: '1/16', s: 1 }];
const DIRS = ['FWD', 'REV', 'PING', 'RAND', 'DRIFT'];
const KEYS = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B'];
const ENGINES = ['PAD', 'CHOIR', 'FM', 'BASS', 'LEAD', 'BOWED'];
const ECHO_MODES = [{ h: 1, r: 0 }, { h: 2, r: 0 }, { h: 4, r: 0 }, { h: 6, r: 0 }, { h: 1, r: 1 }, { h: 2, r: 1 }, { h: 4, r: 1 }, { h: 3, r: 1 }, { h: 6, r: 1 }, { h: 5, r: 1 }, { h: 7, r: 1 }, { h: 0, r: 1 }];
const CMD = { PLAY: 1, STOP: 2, BPM10: 3, KEY: 4, SCALE: 5, SWING: 6, QUEUE_SCENE: 7, SET_SCENE_NOW: 8, QUANT: 9, STEP: 20, DSTEP: 21, TRACK: 22, LANE_VOICE: 23, MUTE: 24, PROG: 25, CB: 26, CLEAR_TRACK: 27, COPY_SCENE: 28, CLEAR_SCENE: 29,
  PATCH_VAL: 40, PRESET: 41, KIT: 42, TEX_TYPE: 43, TEX_LVL: 44, TEX_TONE: 45, TEX_MOTION: 46, ALIGN: 60, GEN_TRACK: 61, MUTATE: 62, SEED_RETURN: 63, MOOD: 64, CHAOS: 65, SONG_LEN: 70, SONG_ENTRY: 71, SONG_ON: 72,
  NOTE: 80, MIDI: 81, PERF: 82, LEARN: 83, PAD_CHANNEL: 84, FILE_PLAY: 85, NEW_PROJECT: 86 };
const PERF = { FREEZE: 0, STOP: 1, SPIN: 2, FILL: 3, NOVA: 4, ECLIPSE: 5, KICK: 6, ALIGN: 7, AUTO: 8, ALIGN_SONG: 9, SPIN_BACK: 10 };
const scaleNote = (sc, deg) => { const L = sc.length, o = Math.floor(deg / L), d = ((deg % L) + L) % L; return sc[d] + 12 * o; };
const clamp = (v, a, b) => (v < a ? a : v > b ? b : v);
const lerp = (a, b, t) => a + (b - a) * t;
const rnd = (a = 1, b) => (b === undefined ? Math.random() * a : a + Math.random() * (b - a));
const rint = (a, b) => Math.floor(rnd(a, b + 1));
const pick = arr => arr[Math.floor(Math.random() * arr.length)];
const chance = p => Math.random() < p;
const u = v => clamp(v, 0, 127) / 127;
const $ = (s, r = document) => r.querySelector(s);
const $$ = (s, r = document) => [...r.querySelectorAll(s)];

let PJ = null, PARAMS = new Float32Array(PARAM_META.length);
PARAM_META.forEach((m, i) => (PARAMS[i] = 0.5));
const SC = () => PJ.scenes[PJ.cur];
const curScale = () => SCALES[PJ.scale].i;

function decodeProject(b) {
  let n = 4; const B = () => b[n++], S = () => { const v = b[n++]; return v > 127 ? v - 256 : v; }, str = len => { const u = b.subarray(n, n + len); n += len; const z = u.indexOf(0); return new TextDecoder().decode(u.subarray(0, z < 0 ? len : z)); };
  const P = {}; P.bpm = (B() | (B() << 8)) / 10;
  ['swing', 'key', 'scale', 'kit', 'texType', 'texLvl', 'texTone', 'texMotion', 'cur', 'mood', 'chaos', 'songOn', 'songLen'].forEach(k => (P[k] = B()));
  P.song = []; for (let i = 0; i < 32; i++) P.song.push({ s: B(), b: B() });
  P.patches = []; for (let i = 0; i < 8; i++) { const p = { n: str(24), cat: str(8), v: [] }; for (let k = 0; k < 30; k++) p.v.push(B()); P.patches.push(p); }
  P.scenes = [];
  for (let s = 0; s < 8; s++) {
    const sc = { prog: [S(), S(), S(), S()], cb: B(), used: B(), mutes: [], tracks: [] }; for (let i = 0; i < 8; i++) sc.mutes.push(B());
    for (let i = 0; i < 8; i++) {
      const t = { len: B(), rate: B(), dir: B(), follow: B(), chord: B(), oct: S(), steps: [], lanes: [] };
      for (let q = 0; q < 32; q++) t.steps.push({ on: B(), deg: S(), vel: B(), prob: B(), gate4: B(), oct: S(), rat: B() });
      for (let l = 0; l < 4; l++) { const ln = { v: B(), steps: [] }; for (let q = 0; q < 32; q++) ln.steps.push({ on: B(), vel: B(), prob: B(), rat: B() }); t.lanes.push(ln); }
      sc.tracks.push(t);
    }
    P.scenes.push(sc);
  }
  // fields the visuals expect
  P.mix = TRACKS.map(() => ({ mute: 0, solo: 0 })); P.mac = { get drift() { return PARAMS[P_.DRIFT] * 127; } };
  return P;
}
const P_ = { DRIFT: P.P_DRIFT };
function encodeProject(P, params) {
  const out = []; const B = v => out.push(v & 255), str = (s, len) => { const e = new TextEncoder().encode(s); for (let i = 0; i < len; i++) out.push(i < e.length && i < len - 1 ? e[i] : 0); };
  out.push(65, 78, 68, 2); const b10 = Math.round(P.bpm * 10); B(b10); B(b10 >> 8);
  ['swing', 'key', 'scale', 'kit', 'texType', 'texLvl', 'texTone', 'texMotion', 'cur', 'mood', 'chaos', 'songOn', 'songLen'].forEach(k => B(P[k]));
  P.song.forEach(e => { B(e.s); B(e.b); });
  P.patches.forEach(p => { str(p.n, 24); str(p.cat, 8); p.v.forEach(B); });
  P.scenes.forEach(sc => { sc.prog.forEach(B); B(sc.cb); B(sc.used); sc.mutes.forEach(B);
    sc.tracks.forEach(t => { [t.len, t.rate, t.dir, t.follow, t.chord, t.oct].forEach(B); t.steps.forEach(s => [s.on, s.deg, s.vel, s.prob, s.gate4, s.oct, s.rat].forEach(B)); t.lanes.forEach(l => { B(l.v); l.steps.forEach(s => [s.on, s.vel, s.prob, s.rat].forEach(B)); }); }); });
  if (params) { B(params.length); params.forEach(v => B(Math.round(v * 255))); } else B(0);
  return new Uint8Array(out);
}
function keyRoot(i) { const k = PJ.key; return TRACKS[i].base + (k > 6 ? k - 12 : k); }
function degToMidi(i, deg, oct = 0) { let m = keyRoot(i) + 12 * oct + scaleNote(curScale(), deg); const t = TRACKS[i]; while (m > t.hi) m -= 12; while (m < t.lo) m += 12; return m; }
const noteName = m => KEYS[((m % 12) + 12) % 12] + (Math.floor(m / 12) - 1);
const ROMAN = ['I', 'II', 'III', 'IV', 'V', 'VI', 'VII'];
function chordName(deg) {
  const sc = curScale(), L = sc.length, d = ((deg % L) + L) % L, third = scaleNote(sc, deg + 2) - scaleNote(sc, deg), fifth = scaleNote(sc, deg + 4) - scaleNote(sc, deg);
  const root = KEYS[(PJ.key + scaleNote(sc, deg) + 120) % 12];
  if (L !== 7) return { r: String(d + 1), root };
  let r = ROMAN[d]; if (third === 3) r = r.toLowerCase(); if (fifth === 6) r += '°'; if (fifth === 8) r += '+';
  return { r, root: root + (third === 3 ? 'm' : '') };
}
