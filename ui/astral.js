/* astral.js — the living sky chart and the Nebula Echo tape drawing */
const ZOD = ['♈', '♉', '♊', '♋', '♌', '♍', '♎', '♏', '♐', '♑', '♒', '♓'].map(c => c + '︎');
const PGLYPH = { MARS: '♂', SATURN: '♄', VENUS: '♀', MOON: '☽', MERCURY: '☿', JUPITER: '♃', NEPTUNE: '♆', PLUTO: '♇' };
const ASPECTS = [{ a: 0, c: '242,200,120', n: 'conjunction' }, { a: 60, c: '127,211,224', n: 'sextile' }, { a: 90, c: '217,138,168', n: 'square' }, { a: 120, c: '143,214,227', n: 'trine' }, { a: 180, c: '224,122,95', n: 'opposition' }];
const SKY = {
  t0: performance.now(), last: performance.now(), time: 0, rot: 0, wheel: 0, wheelTo: 0, energy: 0, low: 0, high: 0,
  planets: TRACKS.map((t, i) => ({ a: i * 0.8, flare: 0, r: 0.6 + (i % 4) * 0.035, last: null })),
  rings: [], comets: [], stars: [], cons: [], shock: [], transit: null, chordFlash: 0, sun: 0
};
(function makeStars() { for (let i = 0; i < 320; i++) SKY.stars.push({ r: Math.sqrt(Math.random()) * 0.75, a: Math.random() * 6.283, s: Math.random() < .08 ? rnd(1.4, 2.4) : rnd(0.4, 1.2), p: rnd(0, 6.28), w: rnd(0.5, 3), hue: pick(['233,228,212', '200,220,255', '255,220,190', '210,200,255']) }); })();
let NEB = null;
function nebulaTex() {                 // soft clouds, drawn once
  const c = document.createElement('canvas'); c.width = c.height = 512; const g = c.getContext('2d');
  g.globalCompositeOperation = 'lighter';
  const cols = ['120,90,190', '60,140,190', '200,110,150', '210,150,90', '70,80,180', '40,160,170'];
  for (let i = 0; i < 70; i++) {
    const x = 256 + rnd(-200, 200) * (i % 3 ? 1 : 0.5), y = 256 + rnd(-200, 200) * (i % 2 ? 0.6 : 1), r = rnd(40, 170), c0 = pick(cols);
    const gr = g.createRadialGradient(x, y, 0, x, y, r); gr.addColorStop(0, `rgba(${c0},${rnd(0.05, 0.13)})`); gr.addColorStop(1, `rgba(${c0},0)`);
    g.fillStyle = gr; g.beginPath(); g.arc(x, y, r, 0, 6.283); g.fill();
  }
  // a faint spiral arm
  for (let k = 0; k < 2; k++) for (let i = 0; i < 260; i++) {
    const t = i / 260, a = t * 7 + k * Math.PI, r = 20 + t * 220, x = 256 + Math.cos(a) * r, y = 256 + Math.sin(a) * r * 0.62;
    g.fillStyle = `rgba(220,210,255,${0.05 * (1 - t)})`; g.beginPath(); g.arc(x + rnd(-8, 8), y + rnd(-8, 8), rnd(2, 9), 0, 6.283); g.fill();
  }
  return c;
}
function skyAudio() {                  // levels come from the engine (telemetry), whatever runs it
  const rms = Math.min(1, (TELE[11] + TELE[12]) * 1.4);
  SKY.low = lerp(SKY.low, Math.min(1, TELE[13] * 3), 0.25); SKY.high = lerp(SKY.high, Math.min(1, TELE[14] * 6), 0.3); SKY.energy = lerp(SKY.energy, rms, 0.12);
}
function skyEvents(now) {             // note events scheduled ahead: consume when they sound
  while (VIS.length && VIS[0].t <= now) {
    const e = VIS.shift(); if (typeof visionEvent === 'function') visionEvent(e);
    if (e.bar) { if (e.barN === 0 || e.chord !== SKY.lastChord) { SKY.wheelTo += Math.PI / 6 * (e.barN === 0 ? 2 : 1); SKY.chordFlash = 1; } SKY.lastChord = e.chord; continue; }
    if (e.nova) { SKY.shock.push({ r: 0, a: 1 }); SKY.flash = 1; continue; }
    if (e.evolve) { SKY.rings.push({ i: -1, r: 0.05, a: 0.5 }); continue; }
    const p = SKY.planets[e.i]; if (!p) continue;
    p.flare = Math.min(1.6, p.flare + 0.5 + e.vel / 200);
    SKY.rings.push({ i: e.i, r: 0, a: 0.7 * e.vel / 127 });
    if (e.i === 0 && (e.l === 0)) SKY.sun = 1;
    if (e.i === 0 && e.l >= 2 && chance(0.12)) SKY.comets.push({ x: rnd(-.6, .6), y: -0.7, vx: rnd(-.2, .2), vy: rnd(.4, .8), life: 1 });
    if (e.midi !== undefined || e.i >= 6) {    // constellations: stars connect as the music plays
      const ang = p.a + rnd(-0.25, 0.25), rr = clamp(0.12 + (((e.midi || 60) - 36) / 60) * 0.45 + rnd(-0.04, 0.04), 0.1, 0.56);
      const st = { x: Math.cos(ang) * rr, y: Math.sin(ang) * rr, i: e.i, life: 1, prev: p.last };
      SKY.cons.push(st); p.last = st; if (SKY.cons.length > 90) SKY.cons.shift();
    }
  }
}
function drawSky(g, W, H, opt = {}) {
  const now = AU.ready ? ctx.currentTime : 0, pnow = performance.now(), dt = opt.passive ? 0 : Math.min(0.05, (pnow - SKY.last) / 1000);
  if (!opt.passive) { SKY.last = pnow; skyAudio(); skyEvents(now); }
  const slow = LIVE.freeze ? 0.15 : 1, S = Math.min(W, H), cx = W / 2, cy = H / 2, R = S * 0.46, lite = opt.lite;
  SKY.time += dt * slow; SKY.rot += dt * 0.012 * slow * (1 + SKY.energy); if (dt) SKY.wheel = lerp(SKY.wheel, SKY.wheelTo, 0.03);
  const T = SKY.time, E = SKY.energy;
  // ---- deep space
  const bg = g.createRadialGradient(cx, cy, 0, cx, cy, S * 0.75);
  bg.addColorStop(0, '#151a3c'); bg.addColorStop(0.55, '#0a0d24'); bg.addColorStop(1, '#03040a');
  g.fillStyle = bg; g.fillRect(0, 0, W, H);
  if (!NEB) NEB = nebulaTex();
  g.save(); g.translate(cx, cy); g.globalCompositeOperation = 'lighter';
  g.globalAlpha = 0.32 + E * 0.22; g.rotate(SKY.rot); g.drawImage(NEB, -S * 0.8, -S * 0.8, S * 1.6, S * 1.6);
  if (!lite) { g.globalAlpha = 0.14 + SKY.high * 0.3; g.rotate(-SKY.rot * 2.6 + 1.3); g.drawImage(NEB, -S * 0.65, -S * 0.65, S * 1.3, S * 1.3); }
  g.restore(); g.globalAlpha = 1;
  // ---- stars
  const sr = SKY.rot * 0.6;
  SKY.stars.forEach((s, k) => {
    if (lite && k % 2) return;
    const a = s.a + sr, x = cx + Math.cos(a) * s.r * S, y = cy + Math.sin(a) * s.r * S, tw = 0.35 + 0.65 * (0.5 + 0.5 * Math.sin(T * s.w + s.p));
    g.fillStyle = `rgba(${s.hue},${tw})`; const z = s.s * (S / 600) * (1 + SKY.high * 0.5);
    g.fillRect(x - z / 2, y - z / 2, z, z);
    if (s.s > 1.3) { g.strokeStyle = `rgba(${s.hue},${tw * 0.35})`; g.lineWidth = Math.max(0.5, S / 900); g.beginPath(); g.moveTo(x - z * 3, y); g.lineTo(x + z * 3, y); g.moveTo(x, y - z * 3); g.lineTo(x, y + z * 3); g.stroke(); }
  });
  // ---- comets
  SKY.comets = SKY.comets.filter(c => c.life > 0);
  SKY.comets.forEach(c => { c.x += c.vx * dt * slow; c.y += c.vy * dt * slow; c.life -= dt * 0.6;
    const x = cx + c.x * S, y = cy + c.y * S, gr = g.createLinearGradient(x, y, x - c.vx * S * 0.25, y - c.vy * S * 0.25);
    gr.addColorStop(0, `rgba(240,235,220,${c.life})`); gr.addColorStop(1, 'rgba(240,235,220,0)'); g.strokeStyle = gr; g.lineWidth = S / 400;
    g.beginPath(); g.moveTo(x, y); g.lineTo(x - c.vx * S * 0.25, y - c.vy * S * 0.25); g.stroke(); });
  // ---- the wheel (zodiac + houses)
  g.save(); g.translate(cx, cy);
  const wr = SKY.wheel + SKY.rot * 0.3, lw = Math.max(1, S / 520);
  g.strokeStyle = `rgba(233,228,212,${0.32 + E * 0.25})`; g.lineWidth = lw;
  [1, 0.9, 0.77].forEach(f => { g.beginPath(); g.arc(0, 0, R * f, 0, 6.283); g.stroke(); });
  g.strokeStyle = `rgba(233,228,212,0.18)`; g.beginPath(); g.arc(0, 0, R * 0.52, 0, 6.283); g.stroke();
  for (let d = 0; d < 360; d += 5) { const a = wr + d * Math.PI / 180, l = d % 30 === 0 ? 0.9 : d % 10 === 0 ? 0.955 : 0.975;
    g.beginPath(); g.moveTo(Math.cos(a) * R * l, Math.sin(a) * R * l); g.lineTo(Math.cos(a) * R, Math.sin(a) * R); g.stroke(); }
  const cur = TR.chordIdx || 0;
  for (let h = 0; h < 12; h++) {
    const a0 = wr + h * Math.PI / 6, am = a0 + Math.PI / 12;
    g.strokeStyle = 'rgba(233,228,212,0.22)'; g.beginPath(); g.moveTo(Math.cos(a0) * R * 0.52, Math.sin(a0) * R * 0.52); g.lineTo(Math.cos(a0) * R * 0.9, Math.sin(a0) * R * 0.9); g.stroke();
    const lit = TR.playing && h === ((PJ.cur * 4 + cur) % 12);
    if (lit) { g.fillStyle = `rgba(242,184,102,${0.1 + SKY.chordFlash * 0.25})`; g.beginPath(); g.arc(0, 0, R * 0.9, a0, a0 + Math.PI / 6); g.arc(0, 0, R * 0.77, a0 + Math.PI / 6, a0, true); g.fill(); }
    g.fillStyle = lit ? 'rgba(242,200,140,0.95)' : 'rgba(233,228,212,0.55)'; g.font = `${Math.round(R * 0.075)}px "Segoe UI Symbol","Noto Sans Symbols2","DejaVu Sans",serif`;
    g.textAlign = 'center'; g.textBaseline = 'middle'; g.fillText(ZOD[h], Math.cos(am) * R * 0.835, Math.sin(am) * R * 0.835);
  }
  if (dt) SKY.chordFlash *= 0.96;
  // ---- planets on their orbits (one turn = one loop of that track)
  const sc = SC(), soloOn = PJ.mix.some(m => m.solo);
  const pos = SKY.planets.map((p, i) => {
    const tr = sc.tracks[i], loop = tr.len * RATES[tr.rate].s * (60 / PJ.bpm / 4);
    p.a += dt * slow * (TR.playing ? (Math.PI * 2) / Math.max(0.5, loop) : 0.05 + i * 0.01);
    const alive = !sc.mutes[i] && !PJ.mix[i].mute && (!soloOn || PJ.mix[i].solo);
    return { x: Math.cos(p.a) * R * p.r, y: Math.sin(p.a) * R * p.r, alive, p };
  });
  // aspects (the lines inside a chart), stronger when the music is loud
  for (let a = 0; a < 8; a++) for (let b = a + 1; b < 8; b++) {
    if (!pos[a].alive || !pos[b].alive) continue;
    let d = Math.abs(((SKY.planets[a].a - SKY.planets[b].a) * 180 / Math.PI) % 360); if (d > 180) d = 360 - d;
    for (const as of ASPECTS) { const orb = Math.abs(d - as.a); if (orb < 7) { const k = (1 - orb / 7) * (0.25 + E * 0.9);
      g.strokeStyle = `rgba(${as.c},${k})`; g.lineWidth = lw * (1 + k); g.beginPath(); g.moveTo(pos[a].x, pos[a].y); g.lineTo(pos[b].x, pos[b].y); g.stroke(); } }
  }
  // constellations
  SKY.cons = SKY.cons.filter(s => (s.life -= dt * 0.12 * slow) > 0);
  SKY.cons.forEach(s => {
    const col = hexRgb(TRACKS[s.i].col);
    if (s.prev && s.prev.life > 0) { g.strokeStyle = `rgba(${col},${Math.min(s.life, s.prev.life) * 0.5})`; g.lineWidth = lw * 0.8; g.beginPath(); g.moveTo(s.x * R, s.y * R); g.lineTo(s.prev.x * R, s.prev.y * R); g.stroke(); }
    g.fillStyle = `rgba(${col},${s.life})`; g.beginPath(); g.arc(s.x * R, s.y * R, lw * (1 + s.life * 1.5), 0, 6.283); g.fill();
  });
  // ripples
  SKY.rings = SKY.rings.filter(r => (r.a -= dt * 0.9) > 0);
  SKY.rings.forEach(r => { r.r += dt * 0.25; const p = r.i >= 0 ? pos[r.i] : { x: 0, y: 0 }; g.strokeStyle = r.i >= 0 ? `rgba(${hexRgb(TRACKS[r.i].col)},${r.a})` : `rgba(200,220,255,${r.a * 0.5})`;
    g.lineWidth = lw; g.beginPath(); g.arc(p.x, p.y, r.r * R * (r.i >= 0 ? 0.6 : 2), 0, 6.283); g.stroke(); });
  // planets
  pos.forEach((q, i) => {
    const p = q.p, col = hexRgb(TRACKS[i].col), f = p.flare, rr = R * (0.022 + f * 0.022);
    p.flare *= Math.pow(0.08, dt);
    g.globalAlpha = q.alive ? 1 : 0.28;
    const gl = g.createRadialGradient(q.x, q.y, 0, q.x, q.y, rr * 4); gl.addColorStop(0, `rgba(${col},${0.5 + f * 0.3})`); gl.addColorStop(1, `rgba(${col},0)`);
    g.fillStyle = gl; g.beginPath(); g.arc(q.x, q.y, rr * 4, 0, 6.283); g.fill();
    g.fillStyle = `rgb(${col})`; g.beginPath(); g.arc(q.x, q.y, rr, 0, 6.283); g.fill();
    if (!lite || W > 200) { g.fillStyle = `rgba(${col},0.9)`; g.font = `${Math.round(R * 0.06)}px "Segoe UI Symbol","Noto Sans Symbols2","DejaVu Sans",serif`; g.fillText(PGLYPH[TRACKS[i].planet] + '︎', q.x + rr * 2.6, q.y - rr * 2.2); }
    if (f > 0.9 && i !== 6) { g.strokeStyle = `rgba(${col},${(f - 0.9) * 0.6})`; g.lineWidth = lw; g.beginPath(); g.moveTo(q.x, q.y); g.lineTo(0, 0); g.stroke(); }
    g.globalAlpha = 1;
  });
  // the core (sun), pulses with the low end; eclipse covers it
  SKY.sun *= Math.pow(0.02, dt);
  const sz = R * (0.07 + SKY.low * 0.06 + SKY.sun * 0.04);
  const cg = g.createRadialGradient(0, 0, 0, 0, 0, sz * 3.2); cg.addColorStop(0, 'rgba(255,236,200,0.95)'); cg.addColorStop(0.3, 'rgba(242,184,102,0.45)'); cg.addColorStop(1, 'rgba(242,184,102,0)');
  g.fillStyle = cg; g.beginPath(); g.arc(0, 0, sz * 3.2, 0, 6.283); g.fill();
  g.strokeStyle = 'rgba(255,230,190,0.35)'; g.lineWidth = lw;
  for (let k = 0; k < 12; k++) { const a = k * Math.PI / 6 + T * 0.1; g.beginPath(); g.moveTo(Math.cos(a) * sz * 1.4, Math.sin(a) * sz * 1.4); g.lineTo(Math.cos(a) * sz * (2.1 + SKY.low), Math.sin(a) * sz * (2.1 + SKY.low)); g.stroke(); }
  if (LIVE.eclipse) { g.fillStyle = '#05060e'; g.beginPath(); g.arc(sz * 0.15, 0, sz * 1.05, 0, 6.283); g.fill(); g.strokeStyle = 'rgba(255,240,220,0.8)'; g.lineWidth = lw * 1.5; g.beginPath(); g.arc(sz * 0.15, 0, sz * 1.08, 0, 6.283); g.stroke(); }
  // supernova shockwaves
  SKY.shock = SKY.shock.filter(s => (s.a -= dt * 0.5) > 0);
  SKY.shock.forEach(s => { s.r += dt * 1.3; g.strokeStyle = `rgba(255,240,210,${s.a})`; g.lineWidth = lw * 6 * s.a; g.beginPath(); g.arc(0, 0, s.r * R, 0, 6.283); g.stroke(); });
  g.restore();
  if (SKY.flash > 0.01) { g.fillStyle = `rgba(255,245,225,${SKY.flash * 0.5})`; g.fillRect(0, 0, W, H); if (dt) SKY.flash *= 0.9; }
  if (LIVE.freeze) { g.fillStyle = 'rgba(140,200,255,0.07)'; g.fillRect(0, 0, W, H); }
  // vignette
  const vg = g.createRadialGradient(cx, cy, S * 0.4, cx, cy, S * 0.78); vg.addColorStop(0, 'rgba(0,0,0,0)'); vg.addColorStop(1, 'rgba(0,0,0,0.6)'); g.fillStyle = vg; g.fillRect(0, 0, W, H);
}
function hexRgb(h) { const n = parseInt(h.slice(1), 16); return `${(n >> 16) & 255},${(n >> 8) & 255},${n & 255}`; }

/* ------------------------------------------------------- the tape loop --- */
const TAPE = { off: 0, lvl: [0, 0, 0], vu: 0, spring: 0 };
function tapeLevels() {
  const md = ECHO_MODES[pstep(P.P_ST_MODE)], on = PARAMS[P.P_ST_ON] > .5;
  for (let h = 0; h < 3; h++) TAPE.lvl[h] = lerp(TAPE.lvl[h], (md.h >> h) & 1 && on ? Math.min(1, TELE[32 + h] * 3) : 0, 0.3);
  TAPE.vu = lerp(TAPE.vu, Math.min(1, TELE[35] * 1.6), 0.25); TAPE.spring = lerp(TAPE.spring, md.r && on ? Math.min(1, TELE[44] * 4 + TAPE.vu * 0.3) : 0, 0.2);
}
function drawTape(g, W, H, dt, mini) {
  const e = { mode: pstep(P.P_ST_MODE), on: PARAMS[P.P_ST_ON] > .5, wow: PARAMS[P.P_ST_WOW] * 127 }, on = e.on, md = ECHO_MODES[e.mode], T1 = TELE[15] || 0.3, frozen = LIVE.freeze;
  const speed = on ? (frozen ? 0.25 : 1) * (0.12 / T1) : 0;
  TAPE.off -= speed * dt * 160 * (W / 560);
  g.fillStyle = '#1a1712'; g.fillRect(0, 0, W, H);
  const pg = g.createLinearGradient(0, 0, 0, H); pg.addColorStop(0, '#2b261e'); pg.addColorStop(1, '#14110d'); g.fillStyle = pg;
  rr(g, W * 0.015, H * 0.04, W * 0.97, H * 0.92, H * 0.06); g.fill();
  const lw = Math.max(1, W / 280), top = H * 0.26, bot = H * 0.68, rad = (bot - top) / 2, x0 = W * 0.04 + rad, x1 = (mini ? W * 0.96 : W * 0.69) - rad;
  // tape chamber: the loop with slack (moves with wow)
  const wob = 1 ? Math.sin(performance.now() / 1000 * (0.55 + u(e.wow) * 2) * 6.28) * u(e.wow) * H * 0.03 * (on ? 1 : 0) : 0;
  g.save(); g.beginPath();
  g.moveTo(x0, bot); g.lineTo(x1, bot);
  g.arc(x1, (top + bot) / 2, (bot - top) / 2, Math.PI / 2, -Math.PI / 2, true);
  for (let i = 0; i <= 30; i++) { const t = i / 30, x = lerp(x1, x0, t), y = top + Math.sin(t * Math.PI * 3) * (H * 0.035 + wob) * Math.sin(t * Math.PI); g.lineTo(x, y); }
  g.arc(x0, (top + bot) / 2, (bot - top) / 2, -Math.PI / 2, Math.PI / 2, true);
  g.strokeStyle = '#4a321c'; g.lineWidth = lw * 5; g.stroke();
  g.strokeStyle = 'rgba(210,160,100,0.55)'; g.lineWidth = lw * 1.4; g.setLineDash([lw * 6, lw * 9]); g.lineDashOffset = TAPE.off; g.stroke(); g.setLineDash([]);
  g.restore();
  // capstan + pinch roller
  const cx = x1 - (mini ? W * 0.05 : W * 0.025), rot = TAPE.off / 20;
  [[cx, bot + H * 0.11, H * 0.06, '#3a3632'], [cx, bot - H * 0.01, H * 0.035, '#c9c0b0']].forEach(([x, y, r, c]) => { g.fillStyle = c; g.beginPath(); g.arc(x, y, r, 0, 6.283); g.fill(); g.strokeStyle = 'rgba(0,0,0,.5)'; g.lineWidth = lw; g.beginPath(); g.moveTo(x, y); g.lineTo(x + Math.cos(rot) * r, y + Math.sin(rot) * r); g.stroke(); });
  // heads: ERASE REC PB1 PB2 PB3
  const names = ['ERASE', 'REC', 'PB 1', 'PB 2', 'PB 3'], hx = i => lerp(x0 + W * 0.06, cx - W * (mini ? 0.1 : 0.08), i / 4);
  names.forEach((n, i) => {
    const x = hx(i), w = W * (mini ? 0.07 : 0.05), h = H * 0.13, pb = i - 2, act = pb >= 0 && on && (md.h >> pb) & 1;
    const lv = pb >= 0 ? TAPE.lvl[pb] : i === 1 ? TAPE.vu : 0;
    if (act || (i === 1 && on)) { const gl = g.createRadialGradient(x, bot + h * 0.4, 0, x, bot + h * 0.4, w * 2.4); gl.addColorStop(0, `rgba(127,211,224,${0.25 + lv * 0.7})`); gl.addColorStop(1, 'rgba(127,211,224,0)'); g.fillStyle = gl; g.beginPath(); g.arc(x, bot + h * 0.4, w * 2.4, 0, 6.283); g.fill(); }
    const hg = g.createLinearGradient(x - w / 2, 0, x + w / 2, 0); hg.addColorStop(0, '#6c665c'); hg.addColorStop(0.5, act ? '#d8f3f7' : '#b7b0a3'); hg.addColorStop(1, '#5a554d');
    g.fillStyle = hg; rr(g, x - w / 2, bot - h * 0.15, w, h, w * 0.25); g.fill();
    if (!mini || i >= 2) { g.fillStyle = act ? '#bfe9f0' : 'rgba(233,228,212,0.45)'; g.font = `600 ${Math.round(H * 0.048)}px Inter Tight, system-ui, sans-serif`; g.textAlign = 'center'; g.fillText(mini ? 'H' + (pb + 1) : n, x, bot + h * 1.45); }
  });
  if (!mini) {
    // spring tank
    const sx = W * 0.74, sy = H * 0.14, sw = W * 0.22, sh = H * 0.36;
    g.fillStyle = '#0d0c0a'; rr(g, sx, sy, sw, sh, H * 0.03); g.fill(); g.strokeStyle = 'rgba(233,228,212,0.2)'; g.lineWidth = lw; g.stroke();
    for (let s = 0; s < 2; s++) { g.beginPath(); for (let i = 0; i <= 80; i++) { const t = i / 80, x = sx + sw * 0.08 + t * sw * 0.84, y = sy + sh * (0.33 + s * 0.34) + Math.sin(t * 60 + (s ? 1 : 0)) * sh * 0.07 + Math.sin(performance.now() / 70 + t * 9 + s) * sh * 0.08 * TAPE.spring; i ? g.lineTo(x, y) : g.moveTo(x, y); } g.strokeStyle = md.r && on ? `rgba(242,200,140,${0.5 + TAPE.spring * 0.5})` : 'rgba(160,150,135,0.4)'; g.stroke(); }
    g.fillStyle = 'rgba(233,228,212,0.5)'; g.font = `600 ${Math.round(H * 0.06)}px Inter Tight, system-ui, sans-serif`; g.textAlign = 'center'; g.fillText('SPRING', sx + sw / 2, sy + sh + H * 0.08);
    // VU meter
    const vx = W * 0.85, vy = H * 0.86, vr = H * 0.22; g.fillStyle = '#e8dcc0'; g.beginPath(); g.arc(vx, vy, vr, Math.PI, 0); g.fill();
    g.strokeStyle = '#3a2f22'; g.lineWidth = lw; for (let i = 0; i <= 10; i++) { const a = Math.PI + (i / 10) * Math.PI * 0.8 + Math.PI * 0.1; g.beginPath(); g.moveTo(vx + Math.cos(a) * vr * 0.75, vy + Math.sin(a) * vr * 0.75); g.lineTo(vx + Math.cos(a) * vr * 0.92, vy + Math.sin(a) * vr * 0.92); g.stroke(); }
    const na = Math.PI + Math.PI * 0.1 + Math.min(1, TAPE.vu) * Math.PI * 0.8; g.strokeStyle = '#b8432e'; g.lineWidth = lw * 1.3; g.beginPath(); g.moveTo(vx, vy); g.lineTo(vx + Math.cos(na) * vr * 0.95, vy + Math.sin(na) * vr * 0.95); g.stroke();
    // labels
    g.textAlign = 'left'; g.fillStyle = '#f2b866'; g.font = `700 ${Math.round(H * 0.085)}px Inter Tight, system-ui, sans-serif`; g.fillText('SPACE TAPE', W * 0.045, H * 0.16);
    g.fillStyle = 'rgba(233,228,212,0.6)'; g.font = `500 ${Math.round(H * 0.06)}px Inter Tight, system-ui, sans-serif`;
    g.textAlign = 'right'; g.fillText(`MODE ${e.mode + 1} · ${Math.round(T1 * 1000)} ms${frozen ? ' · FROZEN' : ''}${on ? '' : ' · OFF'}`, W * 0.69, H * 0.16);
  }
}
function rr(g, x, y, w, h, r) { g.beginPath(); g.moveTo(x + r, y); g.arcTo(x + w, y, x + w, y + h, r); g.arcTo(x + w, y + h, x, y + h, r); g.arcTo(x, y + h, x, y, r); g.arcTo(x, y, x + w, y, r); g.closePath(); }
