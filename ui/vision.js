/* vision.js — mathematical art that listens: cymatics, harmonograph, attractor, scope (+ the orrery) */
const VISIONS = ['AUTO', 'CYMATICS', 'HARMONOGRAPH', 'ATTRACTOR', 'SCOPE', 'ORRERY'];
const AUTO_SEQ = ['CYMATICS', 'HARMONOGRAPH', 'ATTRACTOR', 'SCOPE', 'HARMONOGRAPH', 'CYMATICS', 'ATTRACTOR', 'SCOPE'];
const PAL = {   // two printed-paper moods: pale fog, and rust/umber dark
  fog: { bg0: [214, 215, 210], bg1: [150, 160, 163], ink: [32, 36, 42], acc: [150, 78, 44], cool: [66, 96, 116], grain: 0.13, dark: 0 },
  rust: { bg0: [46, 36, 29], bg1: [12, 10, 8], ink: [230, 218, 200], acc: [204, 120, 70], cool: [146, 170, 180], grain: 0.10, dark: 1 }
};
const DARK_MOODS = new Set(['rust', 'abyss', 'static', 'orbit', 'lullaby']);
const JI = [1, 16 / 15, 9 / 8, 6 / 5, 5 / 4, 4 / 3, 45 / 32, 3 / 2, 8 / 5, 5 / 3, 9 / 5, 15 / 8];
const VX = { mode: null, prev: null, fade: 0, kick: 0, flash: 0, pal: null, cap: '', t: 0 };
const mixc = (a, b, t) => a.map((x, i) => x + (b[i] - x) * t);
const rgba = (c, a) => `rgba(${c[0] | 0},${c[1] | 0},${c[2] | 0},${a})`;
function visionName() { return UI.vision && UI.vision !== 'AUTO' ? UI.vision : AUTO_SEQ[PJ.cur]; }
function palTarget() { return DARK_MOODS.has(MOODS[PJ.mood].id) ? PAL.rust : PAL.fog; }

/* ---- film grain tile ---- */
let GRAIN = null;
function grainTile() {
  const c = document.createElement('canvas'); c.width = c.height = 256; const g = c.getContext('2d'), d = g.createImageData(256, 256);
  for (let i = 0; i < d.data.length; i += 4) { const v = Math.random() * 255; d.data[i] = d.data[i + 1] = d.data[i + 2] = v; d.data[i + 3] = Math.random() < 0.5 ? 40 : 0; }
  g.putImageData(d, 0, 0); return c;
}

/* ---- the chord as numbers ---- */
function chordSemis() { const sc = curScale(), d = TR.playing ? TR.chord : SC().prog[0]; return [0, 2, 4].map(k => scaleNote(sc, d + k) - scaleNote(sc, d)); }

/* ================================================== CYMATICS (Chladni) === */
const CY = { n: 3, m: 5, on: 3, om: 5, mix: 1, N: 0, P: null, rot: 0 };
function cyInit(N) { CY.N = N; CY.P = new Float32Array(N * 2); for (let i = 0; i < N * 2; i++) CY.P[i] = Math.random(); }
function cySet(n, m) { if (n === m) m++; if (n === CY.n && m === CY.m) return; CY.on = CY.n; CY.om = CY.m; CY.n = n; CY.m = m; CY.mix = 0; }
const chl = (x, y, n, m) => Math.cos(n * Math.PI * x) * Math.cos(m * Math.PI * y) - Math.cos(m * Math.PI * x) * Math.cos(n * Math.PI * y);
function cyDraw(g, W, H, dt, P, lite) {
  const N = lite ? 1800 : 8500; if (CY.N !== N) cyInit(N);
  CY.mix = Math.min(1, CY.mix + dt * 0.6); const mx = CY.mix * CY.mix * (3 - 2 * CY.mix);
  const amp = (0.0025 + SKY.energy * 0.012 + VX.kick * 0.03) * (LIVE.freeze ? 0.2 : 1);
  const p = CY.P;
  for (let i = 0; i < CY.N; i++) {
    const x = p[i * 2], y = p[i * 2 + 1];
    const f = Math.abs(chl(x, y, CY.n, CY.m) * mx + chl(x, y, CY.on, CY.om) * (1 - mx));
    const s = amp * (f * 1.6 + 0.015);
    let nx = x + (Math.random() - 0.5) * s * 2, ny = y + (Math.random() - 0.5) * s * 2;
    if (nx < 0) nx = -nx; if (nx > 1) nx = 2 - nx; if (ny < 0) ny = -ny; if (ny > 1) ny = 2 - ny;
    p[i * 2] = nx; p[i * 2 + 1] = ny;
  }
  const S = Math.min(W, H) * 0.74, cx = W / 2, cy = H / 2;
  CY.rot += dt * 0.01 * (0.3 + u(PJ.mac.drift));
  g.save(); g.translate(cx, cy); g.rotate(Math.sin(CY.rot) * 0.08);
  // the plate
  g.fillStyle = rgba(P.ink, P.dark ? 0.04 : 0.05); g.fillRect(-S / 2 - S * 0.03, -S / 2 - S * 0.03, S * 1.06, S * 1.06);
  g.strokeStyle = rgba(P.ink, 0.35); g.lineWidth = Math.max(1, S / 700); g.strokeRect(-S / 2 - S * 0.03, -S / 2 - S * 0.03, S * 1.06, S * 1.06);
  // sand
  const z = Math.max(1, S / (lite ? 260 : 520)); g.fillStyle = rgba(P.ink, P.dark ? 0.8 : 0.78);
  for (let i = 0; i < CY.N; i++) g.fillRect(p[i * 2] * S - S / 2, p[i * 2 + 1] * S - S / 2, z, z);
  if (VX.flash > 0.02) { g.fillStyle = rgba(P.acc, VX.flash * 0.9); for (let i = 0; i < CY.N; i += 7) g.fillRect(p[i * 2] * S - S / 2, p[i * 2 + 1] * S - S / 2, z * 1.4, z * 1.4); }
  g.restore();
  VX.cap = `cos(${CY.n}πx)·cos(${CY.m}πy) − cos(${CY.m}πx)·cos(${CY.n}πy) = 0`;
}

/* ===================================================== HARMONOGRAPH === */
const HG = { acc: null, t: 0, P: null, last: null, life: 0, ratio: '' };
function hgNew() {
  if (!HG.acc) { HG.acc = document.createElement('canvas'); HG.acc.width = HG.acc.height = 1100; }
  const s = chordSemis(), dr = 0.003 + u(PJ.mac.drift) * 0.02, iv = ((s[1] % 12) + 12) % 12;
  const pairs = { 3: [5, 6], 4: [4, 5], 5: [3, 4], 7: [2, 3] }, pq = chance(0.45) ? [2, 3] : pairs[iv] || pick([[1, 2], [2, 3], [3, 4]]), [p, q] = pq, d = rnd(0.008, 0.012);
  HG.P = [{ A: 0.24, f: p, p: rnd(0, 6.28), d }, { A: 0.22, f: q + dr, p: rnd(0, 6.28), d: d * 1.1 },
          { A: 0.24, f: q, p: rnd(0, 6.28), d: d * 0.95 }, { A: 0.22, f: p - dr * 0.7, p: rnd(0, 6.28), d }];
  HG.ratio = `${p} : ${q}  (+${dr.toFixed(3)})`;
  HG.t = 0; HG.last = null; HG.life = 1;
  const g = HG.acc.getContext('2d'); g.globalCompositeOperation = 'destination-out'; g.fillStyle = 'rgba(0,0,0,0.82)'; g.fillRect(0, 0, 1100, 1100); g.globalCompositeOperation = 'source-over';
}
const hgAt = t => { const P = HG.P, e = k => Math.exp(-P[k].d * t); return [P[0].A * Math.sin(P[0].f * t + P[0].p) * e(0) + P[1].A * Math.sin(P[1].f * t + P[1].p) * e(1), P[2].A * Math.sin(P[2].f * t + P[2].p) * e(2) + P[3].A * Math.sin(P[3].f * t + P[3].p) * e(3)]; };
function hgDraw(g, W, H, dt, P, lite) {
  if (!HG.P) hgNew();
  const a = HG.acc.getContext('2d'), Z = 1100, steps = lite ? 40 : 90, speed = (LIVE.freeze ? 0.12 : 1) * (0.55 + PJ.bpm / 170);
  a.lineWidth = 1.5; a.lineJoin = 'round';
  a.strokeStyle = rgba(VX.flash > 0.1 ? mixc(P.ink, P.acc, VX.flash) : P.ink, 0.13 + SKY.energy * 0.22);
  a.beginPath(); let p0 = HG.last || hgAt(HG.t); a.moveTo(Z / 2 + p0[0] * Z, Z / 2 + p0[1] * Z);
  for (let k = 0; k < steps; k++) { HG.t += dt * speed * 9 / steps; const q = hgAt(HG.t); a.lineTo(Z / 2 + q[0] * Z, Z / 2 + q[1] * Z); p0 = q; }
  a.stroke(); HG.last = p0;
  if (Math.exp(-HG.P[0].d * HG.t) < 0.05) hgNew();
  const S = Math.min(W, H) * 0.9; g.drawImage(HG.acc, (W - S) / 2, (H - S) / 2, S, S);
  const x = W / 2 + p0[0] * S, y = H / 2 + p0[1] * S; g.fillStyle = rgba(P.acc, 0.9); g.beginPath(); g.arc(x, y, Math.max(2, S / 260), 0, 6.283); g.fill();
  VX.cap = `x(t) = Σ Aᵢ·sin(fᵢt + φᵢ)·e^(−dᵢt)    f = ${HG.ratio}`;
}

/* ========================================= ATTRACTOR (Clifford map) === */
const AT_PRE = [[-1.4, 1.6, 1.0, 0.7], [1.91, -1.81, 1.43, -0.84], [-1.49, -1.4, 1.62, 1.23], [1.53, 1.87, -1.12, 1.81], [-1.82, 1.34, 1.57, 0.51], [-1.59, 1.34, -0.86, 1.74], [-1.75, -0.89, 1.87, -1.5], [-1.15, 1.9, -1.43, -1.79], [-1.86, -1.75, 1.68, -0.97], [-1.8, -2.0, -0.5, -0.9], [1.5, -1.8, 1.6, 2.0]];   // all checked: chaotic under the drift
const AT = { R: 0, D: null, img: null, cv: null, x: 0.1, y: 0.1, t: rnd(0, 100), target: 0, p: AT_PRE[0].slice() };
function atDraw(g, W, H, dt, P, lite) {
  const R = lite ? 170 : 340;
  if (AT.R !== R) { AT.R = R; AT.D = new Float32Array(R * R); AT.cv = document.createElement('canvas'); AT.cv.width = AT.cv.height = R; AT.img = AT.cv.getContext('2d').createImageData(R, R); }
  const sp = (LIVE.freeze ? 0.1 : 1) * (0.25 + u(PJ.mac.drift) * 1.2);
  AT.t += dt * sp;
  AT.p = AT_PRE[AT.target];   // a new figure when the progression comes round; the old one dissolves
  const w = 0.035 * (0.4 + u(PJ.mac.drift));
  AT.a = AT.p[0] + w * Math.sin(AT.t * 0.21); AT.b = AT.p[1] + w * Math.sin(AT.t * 0.17 + 1); AT.c = AT.p[2] + w * Math.sin(AT.t * 0.13 + 2) + VX.kick * 0.03; AT.d = AT.p[3] + w * Math.sin(AT.t * 0.19 + 3);
  const D = AT.D; for (let i = 0; i < D.length; i++) D[i] *= 0.86;
  let x = AT.x, y = AT.y; const n = lite ? 6000 : 30000, ext = 1 + Math.max(Math.abs(AT.c), Math.abs(AT.d)), sc = R / (2 * ext + 0.4), o = R / 2;
  for (let i = 0; i < n; i++) {
    const nx = Math.sin(AT.a * y) + AT.c * Math.cos(AT.a * x), ny = Math.sin(AT.b * x) + AT.d * Math.cos(AT.b * y); x = nx; y = ny;
    const px = (x * sc + o) | 0, py = (y * sc + o) | 0; if (px >= 0 && px < R && py >= 0 && py < R) D[py * R + px] += 1;
  }
  AT.x = x; AT.y = y;
  let lit = 0; for (let i = 0; i < D.length; i += 7) if (D[i] > 0.5) lit++;
  AT.dead = lit < D.length / 7 * 0.012 ? (AT.dead || 0) + dt : 0; if (AT.dead > 1) { AT.target = (AT.target + 1) % AT_PRE.length; AT.dead = 0; }
  const im = AT.img.data, gain = (lite ? 0.6 : 0.22) * (0.7 + SKY.energy), col = VX.flash > 0.05 ? mixc(P.ink, P.acc, Math.min(1, VX.flash)) : P.ink;
  for (let i = 0; i < D.length; i++) { const v = 1 - Math.exp(-D[i] * gain), j = i * 4; im[j] = col[0]; im[j + 1] = col[1]; im[j + 2] = col[2]; im[j + 3] = v * 235; }
  AT.cv.getContext('2d').putImageData(AT.img, 0, 0);
  const S = Math.min(W, H) * 0.92; g.imageSmoothingEnabled = true; g.drawImage(AT.cv, (W - S) / 2, (H - S) / 2, S, S);
  VX.cap = `xₙ₊₁ = sin(${AT.a.toFixed(2)}·yₙ) + ${AT.c.toFixed(2)}·cos(${AT.a.toFixed(2)}·xₙ)    yₙ₊₁ = sin(${AT.b.toFixed(2)}·xₙ) + ${AT.d.toFixed(2)}·cos(${AT.b.toFixed(2)}·yₙ)`;
}

/* ======================================================= SCOPE (L×R) === */
const SCO = { acc: null, gain: 3 };
function scDraw(g, W, H, dt, P, lite) {
  if (!SCO.acc) { SCO.acc = document.createElement('canvas'); SCO.acc.width = SCO.acc.height = 900; }
  const a = SCO.acc.getContext('2d'), Z = 900;
  a.globalCompositeOperation = 'destination-out'; a.fillStyle = `rgba(0,0,0,${LIVE.freeze ? 0.04 : 0.16})`; a.fillRect(0, 0, Z, Z); a.globalCompositeOperation = 'source-over';
  if (SCOPE.L) {
    const L = SCOPE.L, Rr = SCOPE.R;
    let pk = 0; for (let i = 0; i < L.length; i += 4) pk = Math.max(pk, Math.abs(L[i]), Math.abs(Rr[i]));
    SCO.gain = lerp(SCO.gain, clamp(0.36 / Math.max(0.02, pk), 0.5, 14), 0.05);
    a.strokeStyle = rgba(VX.flash > 0.1 ? mixc(P.ink, P.acc, VX.flash) : P.ink, 0.5); a.lineWidth = 1.3; a.beginPath();
    for (let i = 0; i < L.length; i += lite ? 4 : 1) { const m = (L[i] + Rr[i]) * 0.7071, s = (L[i] - Rr[i]) * 0.7071, x = Z / 2 + s * SCO.gain * Z * 0.9, y = Z / 2 - m * SCO.gain * Z * 0.9; i ? a.lineTo(x, y) : a.moveTo(x, y); }
    a.stroke();
  }
  const S = Math.min(W, H) * 0.86, ox = (W - S) / 2, oy = (H - S) / 2;
  g.strokeStyle = rgba(P.ink, 0.16); g.lineWidth = 1; g.beginPath(); g.arc(W / 2, H / 2, S * 0.45, 0, 6.283); g.moveTo(W / 2 - S * 0.48, H / 2); g.lineTo(W / 2 + S * 0.48, H / 2); g.moveTo(W / 2, H / 2 - S * 0.48); g.lineTo(W / 2, H / 2 + S * 0.48); g.stroke();
  for (let k = 0; k < 24; k++) { const an = k / 24 * 6.283, r0 = S * 0.45, r1 = S * (k % 6 ? 0.465 : 0.48); g.beginPath(); g.moveTo(W / 2 + Math.cos(an) * r0, H / 2 + Math.sin(an) * r0); g.lineTo(W / 2 + Math.cos(an) * r1, H / 2 + Math.sin(an) * r1); g.stroke(); }
  g.drawImage(SCO.acc, ox, oy, S, S);
  VX.cap = 'mid = (L+R)/√2    side = (L−R)/√2';
}

/* ====================================================== events + frame === */
function visionEvent(e) {
  if (e.bar) {
    if (e.barN === 0 || e.chord !== VX.lastChord) {
      const d = TR.chord, n = 3 + (((d % 5) + 5) % 5), m = n + 1 + ((PJ.scale + PJ.cur + e.chord) % 4);
      cySet(n, m); if (e.chord === 0) AT.target = (AT.target + 1 + rint(0, 2)) % AT_PRE.length; if (e.barN === 0 && visionName() === 'HARMONOGRAPH') hgNew();
    }
    VX.lastChord = e.chord; return;
  }
  if (e.nova) { VX.kick = 1.5; VX.flash = 1; cySet(CY.n + 1, CY.m + 2); return; }
  if (e.i === 0) { VX.kick = Math.min(1.5, VX.kick + (e.l === 0 ? 0.55 : 0.12) * e.vel / 127); return; }
  if (e.i === 4 || e.i === 5) VX.flash = Math.min(1, VX.flash + 0.25 * (e.vel || 100) / 127);
}
function drawVision(g, W, H, opt = {}) {
  const name = visionName();
  if (name === 'ORRERY') { drawSky(g, W, H, opt); VX.cap = 'the planets turn once per loop of their track'; finishVision(g, W, H, PAL.rust, name); return; }
  const now = AU.ready ? ctx.currentTime : 0, pn = performance.now(), dt = Math.min(0.05, (pn - (VX.lastT || pn)) / 1000); VX.lastT = pn;
  skyAudio(); skyEvents(now);
  // palette follows the mood, slowly
  const tg = palTarget(); if (!VX.pal) VX.pal = JSON.parse(JSON.stringify(tg));
  ['bg0', 'bg1', 'ink', 'acc', 'cool'].forEach(k => (VX.pal[k] = mixc(VX.pal[k], tg[k], 0.02))); VX.pal.dark = tg.dark; VX.pal.grain = tg.grain;
  const P = VX.pal, lite = W < 300 || ENGINE_LIGHT();
  const bg = g.createRadialGradient(W / 2, H * 0.45, 0, W / 2, H / 2, Math.max(W, H) * 0.75); bg.addColorStop(0, rgba(P.bg0, 1)); bg.addColorStop(1, rgba(P.bg1, 1)); g.fillStyle = bg; g.fillRect(0, 0, W, H);
  if (VX.mode !== name) { if (VX.mode) { VX.prev = document.createElement('canvas'); VX.prev.width = W; VX.prev.height = H; VX.prev.getContext('2d').drawImage(g.canvas, 0, 0); VX.fade = 1; } VX.mode = name; }
  ({ CYMATICS: cyDraw, HARMONOGRAPH: hgDraw, ATTRACTOR: atDraw, SCOPE: scDraw })[name](g, W, H, dt, P, lite);
  if (VX.prev && VX.fade > 0) { g.globalAlpha = VX.fade; g.drawImage(VX.prev, 0, 0, W, H); g.globalAlpha = 1; VX.fade -= dt * 0.6; if (VX.fade <= 0) VX.prev = null; }
  VX.kick *= Math.pow(0.04, dt); VX.flash *= Math.pow(0.08, dt);
  finishVision(g, W, H, P, name, lite);
}
function finishVision(g, W, H, P, name, lite) {
  if (!GRAIN) GRAIN = grainTile();
  g.save(); g.globalAlpha = P.grain; g.globalCompositeOperation = P.dark ? 'screen' : 'multiply';
  const ox = Math.random() * 256, oy = Math.random() * 256, sz = 256 * Math.max(1, W / 900);
  for (let x = -ox; x < W; x += sz) for (let y = -oy; y < H; y += sz) g.drawImage(GRAIN, x, y, sz, sz);
  g.restore();
  const vg = g.createRadialGradient(W / 2, H / 2, Math.min(W, H) * 0.35, W / 2, H / 2, Math.max(W, H) * 0.75);
  vg.addColorStop(0, 'rgba(0,0,0,0)'); vg.addColorStop(1, P.dark ? 'rgba(0,0,0,0.55)' : 'rgba(40,40,36,0.28)'); g.fillStyle = vg; g.fillRect(0, 0, W, H);
  if (!lite) {
    const fs = Math.max(10, Math.round(Math.min(W, H) / 52));
    g.fillStyle = rgba(P.ink, 0.62); g.font = `italic ${fs}px Georgia, "Times New Roman", serif`; g.textAlign = 'left';
    g.fillText(`fig. ${VISIONS.indexOf(name)} — ${name.toLowerCase()}`, fs * 1.4, fs * 4.4);
    g.font = `${Math.round(fs * 0.92)}px Georgia, "Times New Roman", serif`; g.fillStyle = rgba(P.ink, 0.5);
    g.fillText(VX.cap, fs * 1.4, fs * 5.9);
  }
  const box = g.canvas.parentElement; if (box && box.classList) box.classList.toggle('light', !P.dark);
}
