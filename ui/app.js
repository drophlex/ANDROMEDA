/* app.js — ANDROMEDA interface (web and plugin share it). The engine owns the music; this page draws it and sends your moves. */
const AU = { ready: true };
const LS = { get(k) { try { return localStorage.getItem(k); } catch (e) { return null; } }, set(k, v) { try { localStorage.setItem(k, v); } catch (e) {} } };
const ctx = { get currentTime() { return performance.now() / 1000; } };
let TELE = new Float32Array(48); const SCOPE = { L: new Float32Array(512), R: new Float32Array(512) };
const TR = { playing: false, chord: 0, chordIdx: 0, barInScene: 0, queued: -1, pos: new Array(8).fill(0), tick: 0 };
const LIVE = { freeze: false, eclipse: false };
const VIS = [];
const UI = { vision: 'AUTO', sel: 2, tab: 'play', step: -1, lane: -1, lang: 'en', oct: 0, page: 0, cat: '', learn: false, fxHold: false, keep: 0, dragging: false };
const KNOBS = {};
const pstep = id => { const n = PARAM_META[id].steps; return n > 1 ? Math.round(PARAMS[id] * (n - 1)) : 0; };
const setP = (id, v) => { v = clamp(v, 0, 1); PARAMS[id] = v; Bridge.param(id, v); };
const setPStep = (id, s) => setP(id, s / Math.max(1, PARAM_META[id].steps - 1));
const MOOD_ES = { drift: 'sin batería, nubes flotando', tidal: 'pulso lento, ritmo cálido', haze: 'arpegios brillantes y nostálgicos, breakbeat', glass: 'arpegios de caja musical', pulse: 'sueños de pulse-width, bajo de goma', rust: 'drones oscuros, metales', breath: 'coros y suspiros largos', lullaby: 'melodía nostálgica, breaks lo-fi', orbit: 'polirritmos, braindance ligero', abyss: 'profundo y muy lento', chrome: 'metal frío, arco y gris', dawn: 'brillante, amanecer', static: 'fantasmas de radio' };
const ES = { 'DONATE': 'DONAR', 'free & open source': 'gratis y de código abierto', 'ENTER': 'ENTRAR', '◎ OBSERVATORY': '◎ OBSERVATORIO', 'SCENE': 'ESCENA', 'BAR': 'COMPÁS', 'Switch:': 'Cambiar:', 'SONG MODE': 'MODO CANCIÓN', '✦ PLAY': '✦ TOCAR', 'SOUND': 'SONIDO', 'FX & FILTERS': 'EFECTOS Y FILTROS', 'MIX': 'MEZCLA', 'SONG & FILES': 'CANCIÓN Y ARCHIVOS', 'INPUT · MIDI': 'ENTRADA · MIDI', 'HELP': 'AYUDA', 'Created at': 'Creado en',
  'an ambient instrument · cymatics · space tape': 'un instrumento ambient · cimática · space tape', 'MOOD': 'ESTADO', 'ALIGN SCENE': 'ALINEAR ESCENA', 'ALIGN SONG': 'ALINEAR CANCIÓN', 'AUTO': 'AUTO', 'AMOUNT': 'CANTIDAD', 'ENERGY': 'ENERGÍA', 'BACK TO MY PATTERN': 'VOLVER A MI PATRÓN', 'KEYS': 'TECLAS', 'RAW': 'LIBRE', 'SNAP': 'AJUSTAR', 'SHIFT': 'MODULAR',
  'RATE': 'VELOCIDAD', 'LENGTH': 'LARGO', 'NEW': 'NUEVO', 'VARY': 'VARIAR', 'CLEAR': 'BORRAR', 'FOLLOW CHORDS': 'SEGUIR ACORDES', 'CHORDS': 'ACORDES', 'GATE': 'DURACIÓN', 'VELOCITY': 'FUERZA', 'CHANCE': 'PROBABILIDAD', 'RATCHET': 'REPIQUE', 'OCTAVE': 'OCTAVA',
  'PRESETS': 'PRESETS', 'LISTEN': 'ESCUCHAR', 'MORE': 'MÁS', 'LESS': 'MENOS', 'DRUM KIT': 'KIT DE BATERÍA', 'TEXTURE': 'TEXTURA', 'ECHO': 'ECO', 'COSMOS REVERB': 'REVERB COSMOS', 'FREEZE': 'CONGELAR', 'TAPE STOP': 'TAPE STOP', 'SPIN': 'GIRAR', 'FILL': 'REDOBLE', 'SUPERNOVA': 'SUPERNOVA', 'ECLIPSE': 'ECLIPSE', 'SPRING KICK': 'GOLPE AL RESORTE',
  'KEEP SOUNDS': 'MANTENER SONIDOS', 'KEEP TEMPO': 'MANTENER TEMPO', 'KEEP KEY': 'MANTENER TONO', 'DRIFT': 'DERIVA', 'HAZE': 'BRUMA', 'BLOOM': 'FLORECER', 'GRAVITY': 'GRAVEDAD' };
const tr_ = s => (UI.lang === 'es' && ES[s]) || s;
const es = () => UI.lang === 'es';
let toastT; function toast(m) { const t = $('#toast'); t.textContent = m; t.classList.add('on'); clearTimeout(toastT); toastT = setTimeout(() => t.classList.remove('on'), 2200); }
const h = (tag, attrs = {}, ...kids) => { const e = document.createElement(tag); for (const k in attrs) { if (k === 'on') for (const ev in attrs.on) e.addEventListener(ev, attrs.on[ev]); else if (k === 'class') e.className = attrs[k]; else if (k === 'html') e.innerHTML = attrs[k]; else e.setAttribute(k, attrs[k]); } kids.flat().forEach(c => c != null && c !== '' && e.append(c.nodeType ? c : document.createTextNode(c))); return e; };
const btn = (label, fn, cls = '', on = false, title) => { const b = h('button', { class: cls + (on ? ' on' : '') }, tr_(label)); b.dataset.i = label; b.onclick = fn; if (title) b.title = title; return b; };
const h3 = s => { const e = h('h3', {}, tr_(s)); e.dataset.i = s; return e; };
const hint = s => h('p', { class: 'hint' }, s);
const grp = (title, ...k) => h('div', { class: 'grp' }, h('em', {}, tr_(title)), ...k);
const sel = (opts, val, fn, title) => { const s = h('select', title ? { title } : {}); opts.forEach((o, i) => s.append(h('option', { value: o.v ?? i }, o.n ?? o))); s.value = val; s.onchange = () => fn(s.value); return s; };
const pct = v => Math.round(v * 100) + '%';

/* ---------------------------------------------------------------- knobs (0..1 internally, any display) */
function arcD(a0, a1, r = 15) { const p = a => [20 + Math.cos(a) * r, 20 + Math.sin(a) * r], [x0, y0] = p(a0), [x1, y1] = p(a1); return `M${x0} ${y0}A${r} ${r} 0 ${a1 - a0 > Math.PI ? 1 : 0} 1 ${x1} ${y1}`; }
function knob(id, label, get, setv, o = {}) {
  const def = o.def ?? get(), steps = o.steps || 0;
  const el = h('div', { class: 'knob ' + (o.cls || ''), title: o.title || label });
  el.innerHTML = `<svg viewBox="0 0 40 40"><circle cx="20" cy="20" r="12" class="kb"/><path class="ka" d="${arcD(0.75 * Math.PI, 2.25 * Math.PI)}"/><path class="kv"/><line class="kp" x1="20" y1="20" x2="20" y2="11"/></svg><b></b><i></i>`;
  el.querySelector('i').textContent = tr_(label);
  const kv = el.querySelector('.kv'), kp = el.querySelector('.kp'), val = el.querySelector('b');
  const draw = () => { const f = clamp(get(), 0, 1), a = 0.75 * Math.PI + f * 1.5 * Math.PI; kv.setAttribute('d', f > 0.003 ? arcD(0.75 * Math.PI, a) : ''); kp.setAttribute('x2', 20 + Math.cos(a) * 9); kp.setAttribute('y2', 20 + Math.sin(a) * 9); val.textContent = o.fmt ? o.fmt(f) : Math.round(f * 100); };
  const setV = v => { v = clamp(v, 0, 1); if (steps > 1) v = Math.round(v * (steps - 1)) / (steps - 1); if (Math.abs(v - get()) > 1e-4) { setv(v); draw(); } };
  let y0, v0;
  el.addEventListener('pointerdown', e => { if (UI.learn && o.pid !== undefined) { Bridge.cmd(CMD.LEARN, o.pid); UI.learn = false; toast(es() ? 'Mueve una perilla de tu controlador' : 'Move a control on your controller'); uiLearn(); return; } el.setPointerCapture(e.pointerId); y0 = e.clientY; v0 = get(); UI.dragging = true; e.preventDefault(); });
  el.addEventListener('pointermove', e => { if (y0 === undefined) return; setV(v0 + (y0 - e.clientY) * (e.shiftKey ? 0.15 : 1) / 160); });
  el.addEventListener('pointerup', () => { y0 = undefined; UI.dragging = false; });
  el.addEventListener('dblclick', () => setV(def));
  el.addEventListener('wheel', e => { e.preventDefault(); setV(get() - Math.sign(e.deltaY) * (steps > 1 ? 1 / (steps - 1) : 1 / 64)); }, { passive: false });
  KNOBS[id] = { el, draw, pid: o.pid }; draw(); return el;
}
const pknob = (pid, label, o = {}) => knob('p' + pid + (o.k || ''), label, () => PARAMS[pid], v => setP(pid, v), Object.assign({ pid, steps: PARAM_META[pid].steps }, o));
const drawKnobs = () => Object.values(KNOBS).forEach(k => k.el.isConnected && k.draw());
// patch knob: value 0..127 in the project
function patchKnob(tr, field, label, o = {}) {
  const pf = PF[field]; return knob('pf' + tr + field, label, () => PJ.patches[tr].v[pf] / 127, v => { const x = Math.round(v * 127); PJ.patches[tr].v[pf] = x; Bridge.cmd(CMD.PATCH_VAL, tr, pf, x); }, o);
}
const tSec = v => 0.002 * Math.pow(10, u(v) * 3.9);
const msF = f => { const s = tSec(f * 127); return s < 1 ? Math.round(s * 1000) + 'ms' : s.toFixed(1) + 's'; };

/* ---------------------------------------------------------------- project sync */
let projVersion = -1, refreshT = 0;
async function refreshProject() {
  const r = await Bridge.getProject(); PJ = decodeProject(r.bytes); PARAMS = r.params; uiAll();
}
function maybeRefresh(v) { if (v === projVersion) return; projVersion = v; if (UI.dragging) return; clearTimeout(refreshT); refreshT = setTimeout(refreshProject, 120); }

/* ---------------------------------------------------------------- header */
function uiHeader() {
  $('#bpmV').textContent = Math.round(PJ.bpm);
  const ks = $('#keySel'); if (!ks.options.length) KEYS.forEach((k, i) => ks.append(h('option', { value: i }, k))); ks.value = PJ.key;
  const ss = $('#scaleSel'); if (!ss.options.length) SCALES.forEach((s, i) => ss.append(h('option', { value: i }, s.n))); ss.value = PJ.scale;
  $('#autoBtn').classList.toggle('on', PARAMS[P.P_AUTO] > .5);
  uiTransport();
}
function uiTransport() { const b = $('#playBtn'); b.textContent = TR.playing ? '■ STOP' : '▶ PLAY'; b.classList.toggle('prim', !TR.playing); $('#recBtn').classList.toggle('on', REC.on); }
function setBpm(v) { PJ.bpm = clamp(Math.round(v), 40, 180); $('#bpmV').textContent = PJ.bpm; Bridge.cmd(CMD.BPM10, PJ.bpm * 10); }
let taps = []; function tap() { const n = performance.now(); taps = taps.filter(t => n - t < 2500); taps.push(n); if (taps.length >= 3) setBpm(60000 / ((taps[taps.length - 1] - taps[0]) / (taps.length - 1))); }

/* ---------------------------------------------------------------- scenes + tracks */
function uiScene() {
  const box = $('#scenes'); box.innerHTML = '';
  'ABCDEFGH'.split('').forEach((L, n) => { const sc = PJ.scenes[n]; const b = h('button', { class: (n === PJ.cur ? 'cur ' : '') + (TR.queued === n ? 'q ' : '') + (sc.used ? '' : 'empty') }, L, h('i')); b.onclick = () => { Bridge.cmd(TR.playing ? CMD.QUEUE_SCENE : CMD.SET_SCENE_NOW, n); if (!TR.playing) { PJ.cur = n; uiAll(); } else TR.queued = n; uiScene(); }; box.append(b); });
  const vs = $('#visSel'); vs.innerHTML = ''; vs.append(h('span', { class: 'serif' }, es() ? 'visión' : 'vision'));
  VISIONS.forEach(v => vs.append(btn(v, () => { UI.vision = v; LS.set('andromeda.vision', v); uiScene(); }, 'cy', UI.vision === v)));
  const q = $('#quant'); q.innerHTML = ''; [1, 2, 4].forEach(n => q.append(btn(n + (es() ? (n > 1 ? ' compases' : ' compás') : (n > 1 ? ' bars' : ' bar')), () => { UI.quant = n; Bridge.cmd(CMD.QUANT, n); uiScene(); }, '', (UI.quant || 1) === n)));
  $('#songBtn').classList.toggle('on', !!PJ.songOn);
  uiTracks();
}
function patchName(i) { const t = TRACKS[i]; if (t.kind === 'drum') return KITS[PJ.kit].n + ' KIT'; if (t.kind === 'tex') return TEXTURES[PJ.texType].n; if (t.kind === 'input') return es() ? 'MIC · ARCHIVO' : 'MIC · FILE'; return PJ.patches[i].n; }
function uiTracks() {
  const box = $('#tracks'); box.innerHTML = '';
  TRACKS.forEach((t, i) => {
    const muted = SC().mutes[i];
    const mb = btn('M', e => { e.stopPropagation(); SC().mutes[i] = muted ? 0 : 1; Bridge.cmd(CMD.MUTE, PJ.cur, i, SC().mutes[i]); uiTracks(); }, 'rose', muted, es() ? 'silenciar (con fundido)' : 'mute (with a fade)');
    const el = h('div', { class: 'trk' + (UI.sel === i ? ' sel' : '') + (muted ? ' muted' : '') }, h('div', { class: 'nm' }, h('span', { class: 'dot', style: `background:${t.col}` }), (i + 1) + ' ' + t.n), h('div', { class: 'gl' }, patchName(i)), h('div', { class: 'ms' }, mb));
    el.onclick = () => selTrack(i); box.append(el);
  });
}
function selTrack(i) { UI.sel = i; UI.step = -1; UI.page = 0; setPStep(P.P_TRACK, i); uiTracks(); if (['play', 'snd'].includes(UI.tab)) uiTab(); }

/* ---------------------------------------------------------------- tabs */
function setTab(t) { UI.tab = t; $$('#tabs button').forEach(b => b.classList.toggle('on', b.dataset.t === t)); $$('.tab').forEach(x => x.classList.toggle('on', x.id === 't-' + t)); uiTab(); }
function uiTab() { if (!PJ) return; ({ play: uiPlay, snd: uiSnd, tape: uiTape, fx: uiFx, mix: uiMix, song: uiSong, setup: uiSetup, help: uiHelp })[UI.tab](); }

/* ================================================================= PLAY: pattern + AUTO */
function uiPlay() {
  const T = $('#t-play'); T.innerHTML = '';
  // moods (one row) + align
  const moods = h('div', { class: 'chips', style: 'margin-bottom:8px' }, ...MOODS.map((m, i) => { const b = btn(m.n, () => { PJ.mood = i; Bridge.cmd(CMD.MOOD, i); uiPlay(); }, '', PJ.mood === i, es() ? MOOD_ES[m.id] : m.d); b.ondblclick = () => Bridge.cmd(CMD.ALIGN, i, UI.keep, 0); b.textContent = m.n; delete b.dataset.i; return b; }));
  T.append(h('div', { class: 'prow' }, h('span', { class: 'lab' }, tr_('MOOD')), h('span', { class: 'hint', style: 'margin:0' }, es() ? MOOD_ES[MOODS[PJ.mood].id] : MOODS[PJ.mood].d)), moods);
  T.append(h('div', { class: 'row', style: 'margin-bottom:6px' },
    btn('ALIGN SCENE', () => { Bridge.cmd(CMD.ALIGN, PJ.mood, UI.keep, 0); toast(MOODS[PJ.mood].n); }, 'prim', false, es() ? 'escribe todas las partes de esta escena' : 'writes every part of this scene'),
    btn('ALIGN SONG', () => { Bridge.cmd(CMD.ALIGN, PJ.mood, UI.keep, 1); toast('SONG · ' + MOODS[PJ.mood].n); }, 'cy', false, es() ? 'una canción completa en A–F' : 'a whole song in A–F'),
    knob('chaos', 'CHAOS', () => PJ.chaos / 127, v => { PJ.chaos = Math.round(v * 127); Bridge.cmd(CMD.CHAOS, PJ.chaos); }, { cls: 'ro', fmt: pct, pid: P.P_CHAOS }),
    h('div', { class: 'chips' }, ...[['KEEP SOUNDS', 1], ['KEEP TEMPO', 2], ['KEEP KEY', 4]].map(([l, b]) => btn(l, () => { UI.keep ^= b; uiPlay(); }, 'cy', UI.keep & b)))));
  // AUTO card
  const auto = PARAMS[P.P_AUTO] > .5;
  T.append(h('div', { class: 'autoc' },
    btn(auto ? '✦ AUTO · ON' : '✦ AUTO', () => { setP(P.P_AUTO, auto ? 0 : 1); uiPlay(); uiHeader(); }, 'big ' + (auto ? 'prim' : 'cy'), false, es() ? 'la música se escribe sola a partir de tu patrón' : 'the music writes itself from your pattern'),
    pknob(P.P_AUTO_AMT, 'AMOUNT', { cls: 'cy', fmt: pct }), pknob(P.P_ENERGY, 'ENERGY', { fmt: pct }),
    h('div', {}, h('div', { class: 'lab', style: 'font-size:10px;color:var(--dim);letter-spacing:.14em;margin-bottom:4px' }, tr_('KEYS')),
      h('div', { class: 'chips kmode' }, ...['RAW', 'SNAP', 'SHIFT'].map((l, k) => btn(l, () => { setPStep(P.P_KEYMODE, k); uiPlay(); }, 'cy', pstep(P.P_KEYMODE) === k, [es() ? 'tus notas tal cual' : 'your notes as played', es() ? 'cada nota cae en la escala' : 'every note lands in the scale', es() ? 'si tocas fuera de la escala, ANDROMEDA modula suave a la escala más cercana, en el siguiente tiempo' : 'play outside the scale and ANDROMEDA glides to the nearest scale, on the next beat'][k])))),
    btn('BACK TO MY PATTERN', () => { Bridge.cmd(CMD.SEED_RETURN, PJ.cur); setTimeout(refreshProject, 80); }, '', false)));
  T.append(hint(es() ? 'AUTO toma tu patrón como semilla: cada 2–4 compases varía una parte con tus propios intervalos y ritmos, siempre en tono y en tiempo, y las partes entran y salen con fundido según la ENERGÍA. Lo que tocas en el teclado se vuelve un motivo que la música contesta.' : 'AUTO uses your pattern as a seed: every 2–4 bars it varies one part with your own intervals and rhythms, always in key and in time, and parts fade in and out with ENERGY. What you play on the keyboard becomes a motif the music answers.'));
  // track editor
  const i = UI.sel, t = TRACKS[i], tr = SC().tracks[i];
  const head = h('div', { class: 'prow' }, h('b', { style: `color:${t.col};letter-spacing:.2em;font-size:13px;margin-right:6px` }, (i + 1) + ' ' + t.n));
  const sendTrack = () => Bridge.cmd(CMD.TRACK, PJ.cur, i, tr.len, tr.rate, tr.dir, tr.follow, tr.chord, tr.oct);
  head.append(h('span', { class: 'lab' }, tr_('RATE')), sel(RATES.map((r, k) => ({ v: k, n: r.n })), tr.rate, v => { tr.rate = +v; sendTrack(); }));
  const ln = h('input', { type: 'number', min: 1, max: 32, value: tr.len }); ln.onchange = () => { tr.len = clamp(+ln.value || 16, 1, 32); sendTrack(); uiPlay(); };
  head.append(h('span', { class: 'lab' }, tr_('LENGTH')), ln);
  if (t.kind === 'synth') {
    head.append(sel(DIRS.map((d, k) => ({ v: k, n: d })), tr.dir, v => { tr.dir = +v; sendTrack(); }), sel([-2, -1, 0, 1, 2].map(o => ({ v: o, n: 'OCT ' + (o > 0 ? '+' : '') + o })), tr.oct, v => { tr.oct = +v; sendTrack(); uiPlay(); }));
    head.append(btn('FOLLOW CHORDS', () => { tr.follow = tr.follow ? 0 : 1; sendTrack(); uiPlay(); }, 'cy', tr.follow), btn('CHORDS', () => { tr.chord = tr.chord ? 0 : 1; sendTrack(); uiPlay(); }, 'cy', tr.chord));
  }
  T.append(head);
  T.append(h('div', { class: 'row' }, btn('✦ ' + tr_('NEW'), () => { Bridge.cmd(CMD.GEN_TRACK, i); setTimeout(refreshProject, 60); }, 'prim'), btn('≈ ' + tr_('VARY'), () => { Bridge.cmd(CMD.MUTATE, i, 70); setTimeout(refreshProject, 60); }),
    btn('CLEAR', () => { Bridge.cmd(CMD.CLEAR_TRACK, PJ.cur, i); setTimeout(refreshProject, 60); }), tr.len > 16 ? btn(UI.page ? '◀ 1–16' : '17–32 ▶', () => { UI.page ^= 1; uiPlay(); }, 'pagebtn') : ''));
  T.append(t.kind === 'synth' ? rollEl(i, tr) : t.kind === 'drum' ? drumEl(tr) : rowEl(i, tr));
  if (UI.step >= 0) T.append(stepEditor(i, tr));
  T.append(hint(es() ? 'Toca una celda para poner una nota (siempre en tono); tócala otra vez para quitarla. Mantén Shift al tocar para alargarla. Teclado de la compu: Z S X D C V G B H N J M , L . ; /' : 'Tap a cell to place a note (always in key); tap it again to remove it. Hold Shift while tapping to make it longer. Computer keys: Z S X D C V G B H N J M , L . ; /'));
}
function stepCols(tr) { const L = tr.len, from = UI.page ? 16 : 0, to = Math.min(L, from + 16); return { from, to, n: Math.max(8, to - from) }; }
function rollEl(i, tr) {   // piano roll in scale degrees: every row is in key
  const { from, to, n } = stepCols(tr), rows = tr.chord ? [6, 5, 4, 3, 2, 1, 0, -1, -2, -3] : [11, 9, 7, 6, 5, 4, 3, 2, 1, 0, -1, -2, -3];
  const g = h('div', { class: 'pr', style: `grid-template-columns:62px repeat(${n},1fr)` }); const col = TRACKS[i].col;
  rows.forEach(d => {
    const lab = tr.chord ? chordName(d).r + ' · ' + chordName(d).root : noteName(degToMidi(i, d, tr.oct)).replace(/(\d)/, ' $1');
    g.append(h('div', { class: 'rl' }, lab));
    for (let q = from; q < from + n; q++) {
      const s = tr.steps[q], inLen = q < tr.len, isOn = s && s.on && s.deg === d;
      let tail = false; for (let b = 1; b < 8 && !isOn; b++) { const p2 = tr.steps[q - b]; if (q - b >= 0 && p2 && p2.on && p2.deg === d && p2.gate4 > b * 4) { tail = true; break; } }
      const c = h('div', { class: 'c' + (q % 4 === 0 ? ' beat' : '') + (isOn ? ' on' : '') + (isOn && s.prob < 127 ? ' lp' : '') + (tail ? ' tail' : '') + (d % curScale().length === 0 ? ' root' : '') + (inLen ? '' : ' off') + (UI.step === q && isOn ? ' sel' : ''), style: `--c:${col}` });
      c.dataset.s = q;
      c.onclick = e => {
        if (!inLen) return; const st = tr.steps[q];
        if (st.on && st.deg === d && !e.shiftKey) { st.on = 0; UI.step = -1; }
        else { const was = st.on && st.deg === d; Object.assign(st, { on: 1, deg: d, vel: st.vel || 100, prob: st.prob || 127, gate4: e.shiftKey ? Math.min(128, (was ? st.gate4 : 4) + 4) : (was ? st.gate4 : 4), oct: 0, rat: 1 }); UI.step = q; previewNote(i, d); }
        sendStep(i, q); uiPlay();
      };
      g.append(c);
    }
  });
  return g;
}
function drumEl(tr) {
  const { from, n } = stepCols(tr), g = h('div', { class: 'pr', style: `grid-template-columns:96px repeat(${n},1fr)` });
  tr.lanes.forEach((ln, l) => {
    g.append(h('div', { class: 'rl' }, sel(DRUM_VOICES.map((v, k) => ({ v: k, n: v })), ln.v, v => { ln.v = +v; Bridge.cmd(CMD.LANE_VOICE, PJ.cur, l, ln.v); })));
    for (let q = from; q < from + n; q++) { const s = ln.steps[q], inLen = q < tr.len;
      const c = h('div', { class: 'c' + (q % 4 === 0 ? ' beat' : '') + (s.on ? ' on' : '') + (s.on && s.prob < 127 ? ' lp' : '') + (inLen ? '' : ' off') + (UI.step === q && UI.lane === l ? ' sel' : ''), style: '--c:' + TRACKS[0].col }); c.dataset.s = q;
      c.onclick = e => { if (!inLen) return; if (e.shiftKey && s.on) { UI.step = q; UI.lane = l; uiPlay(); return; } s.on = s.on ? 0 : 1; if (s.on) { s.vel = s.vel || 100; s.prob = s.prob || 127; UI.step = q; UI.lane = l; } else UI.step = -1; Bridge.cmd(CMD.DSTEP, PJ.cur, l, q, s.on, s.vel, s.prob, s.rat || 1); uiPlay(); };
      g.append(c); }
  });
  return g;
}
function rowEl(i, tr) {
  const { from, n } = stepCols(tr), g = h('div', { class: 'pr', style: `grid-template-columns:96px repeat(${n},1fr)` });
  g.append(h('div', { class: 'rl' }, TRACKS[i].kind === 'input' ? (es() ? 'compuerta' : 'gate') : (es() ? 'oleadas' : 'swells')));
  for (let q = from; q < from + n; q++) { const s = tr.steps[q], inLen = q < tr.len; const c = h('div', { class: 'c' + (q % 4 === 0 ? ' beat' : '') + (s.on ? ' on' : '') + (inLen ? '' : ' off'), style: '--c:' + TRACKS[i].col }); c.dataset.s = q;
    c.onclick = () => { if (!inLen) return; s.on = s.on ? 0 : 1; s.vel = s.vel || 100; s.prob = s.prob || 127; s.gate4 = s.gate4 || 4; sendStep(i, q); uiPlay(); }; g.append(c); }
  return g;
}
const sendStep = (i, q) => { const s = SC().tracks[i].steps[q]; Bridge.cmd(CMD.STEP, PJ.cur, i, q, s.on, s.deg, s.vel || 100, s.prob ?? 127, s.gate4 || 4, s.oct || 0, s.rat || 1); };
function stepEditor(i, tr) {
  const box = h('div', { class: 'stepedit' }, h('b', { style: 'align-self:center;color:var(--cyan);letter-spacing:.15em;font-size:11px' }, 'STEP ' + (UI.step + 1)));
  if (TRACKS[i].kind === 'drum') { if (UI.lane < 0) return ''; const s = tr.lanes[UI.lane].steps[UI.step]; const send = () => Bridge.cmd(CMD.DSTEP, PJ.cur, UI.lane, UI.step, s.on, s.vel, s.prob, s.rat);
    box.append(knob('dv', 'VELOCITY', () => s.vel / 127, v => { s.vel = Math.max(1, Math.round(v * 127)); send(); }), knob('dp', 'CHANCE', () => s.prob / 127, v => { s.prob = Math.round(v * 127); send(); }, { cls: 'ro', fmt: pct }), knob('dr', 'RATCHET', () => (s.rat - 1) / 3, v => { s.rat = 1 + Math.round(v * 3); send(); }, { steps: 4, cls: 'cy', fmt: f => '×' + (1 + Math.round(f * 3)) }));
    return box; }
  const s = tr.steps[UI.step]; if (!s || !s.on) return ''; const send = () => sendStep(i, UI.step);
  box.append(knob('sg', 'GATE', () => (s.gate4 - 1) / 127, v => { s.gate4 = 1 + Math.round(v * 127); send(); }, { fmt: f => ((1 + Math.round(f * 127)) / 4) + '×' }),
    knob('sv', 'VELOCITY', () => s.vel / 127, v => { s.vel = Math.max(1, Math.round(v * 127)); send(); }), knob('sp', 'CHANCE', () => s.prob / 127, v => { s.prob = Math.round(v * 127); send(); }, { cls: 'ro', fmt: pct }),
    knob('so', 'OCTAVE', () => (s.oct + 2) / 4, v => { s.oct = Math.round(v * 4) - 2; send(); }, { steps: 5, fmt: f => (Math.round(f * 4) - 2 > 0 ? '+' : '') + (Math.round(f * 4) - 2) }));
  return box;
}
function previewNote(i, d) { const m = degToMidi(i, d + (SC().tracks[i].follow && TR.playing ? TR.chord : 0), SC().tracks[i].oct); Bridge.cmd(CMD.NOTE, i, m, 96); setTimeout(() => Bridge.cmd(CMD.NOTE, i, m, 0), 260); }

/* ================================================================= SOUND */
const WNAME = ['SAW', 'SQR', 'TRI', 'SINE', 'PWM'];
function uiSnd() {
  const T = $('#t-snd'); T.innerHTML = ''; const i = UI.sel, t = TRACKS[i];
  T.append(h('div', { class: 'prow' }, h('b', { style: `color:${t.col};letter-spacing:.2em;font-size:13px` }, (i + 1) + ' ' + t.n), h('span', { class: 'badge' }, patchName(i))));
  if (t.kind === 'drum') {
    T.append(h3('DRUM KIT')); const g = h('div', { class: 'moods' });
    KITS.forEach((k, n) => { const b = h('button', { class: 'mood' + (PJ.kit === n ? ' on' : '') }, h('b', {}, k.n), h('span', {}, k.d)); b.onclick = () => { PJ.kit = n; Bridge.cmd(CMD.KIT, n); uiSnd(); uiTracks(); [0, 2, 1, 2].forEach((v, x) => setTimeout(() => Bridge.cmd(CMD.NOTE, 0, [36, 42, 38, 42][x], 110), x * 180)); }; g.append(b); });
    T.append(g, h('div', { class: 'chips' }, ...DRUM_VOICES.map((v, n) => btn(v, () => Bridge.cmd(CMD.NOTE, 0, 36 + [0, 2, 5, 9, 3, 1, 6, 12, 14, 15, 15][n], 110)))));
    return;
  }
  if (t.kind === 'tex') {
    T.append(h3('TEXTURE')); const g = h('div', { class: 'moods' });
    TEXTURES.forEach((x, n) => { const b = h('button', { class: 'mood' + (PJ.texType === n ? ' on' : '') }, h('b', {}, x.n)); b.onclick = () => { PJ.texType = n; Bridge.cmd(CMD.TEX_TYPE, n); uiSnd(); uiTracks(); }; g.append(b); });
    T.append(g, h('div', { class: 'row' }, knob('tl', 'LEVEL', () => PJ.texLvl / 127, v => { PJ.texLvl = Math.round(v * 127); Bridge.cmd(CMD.TEX_LVL, PJ.texLvl); }, { fmt: pct }), knob('tt', 'TONE', () => PJ.texTone / 127, v => { PJ.texTone = Math.round(v * 127); Bridge.cmd(CMD.TEX_TONE, PJ.texTone); }), knob('tm', 'MOTION', () => PJ.texMotion / 127, v => { PJ.texMotion = Math.round(v * 127); Bridge.cmd(CMD.TEX_MOTION, PJ.texMotion); })));
    return;
  }
  if (t.kind === 'input') { T.append(hint(es() ? 'La entrada (micrófono, línea o un archivo de audio) se configura en ENTRADA · MIDI.' : 'The input (mic, line or an audio file) is set up in INPUT · MIDI.'), btn('INPUT · MIDI', () => setTab('setup'), 'prim')); return; }
  const Pt = PJ.patches[i], cats = [...new Set(Bridge.presets.map(p => p.cat))];
  if (!UI.cat || !cats.includes(UI.cat)) UI.cat = Pt.cat || cats[0];
  T.append(h3('PRESETS'), h('div', { class: 'chips' }, ...cats.map(c => btn(c, () => { UI.cat = c; uiSnd(); }, 'cy', UI.cat === c))));
  const pl = h('div', { class: 'presets' }); Bridge.presets.forEach((p, k) => { if (p.cat !== UI.cat) return; pl.append(btn(p.n, () => { Bridge.cmd(CMD.PRESET, i, k); setTimeout(async () => { await refreshProject(); listen(i); }, 60); }, '', Pt.n === p.n)); }); T.append(pl);
  T.append(h('div', { class: 'row', style: 'margin-bottom:8px' }, btn('▶ ' + tr_('LISTEN'), () => listen(i), 'prim'), h('span', { class: 'badge' }, ENGINES[Pt.v[PF.eng]] || '')));
  const K = (f, l, o = {}) => patchKnob(i, f, l, o);
  const main = h('div', { class: 'grps' }, grp(es() ? 'LO ESENCIAL' : 'ESSENTIALS', K('cut', 'CUTOFF', { cls: 'cy' }), K('res', 'RESO', { cls: 'cy' }), K('a', 'ATTACK', { fmt: msF }), K('r', 'RELEASE', { fmt: msF }), K('warble', 'WARBLE', { cls: 'ro' }), K('chorus', 'ENSEMBLE'), K('drive', 'DRIVE', { cls: 'ro' }), K('lvl', 'LEVEL', { fmt: pct })));
  T.append(main);
  const more = h('div', { class: 'grps', style: UI.more ? '' : 'display:none' });
  const eng = Pt.v[PF.eng];
  if (eng === 2) more.append(grp('FM', K('ratio', 'RATIO'), K('index', 'BRIGHT'), K('idec', 'SHIMMER', { fmt: msF })));
  else if (eng === 5) more.append(grp('BOW', K('ratio', 'METAL'), K('noise', 'BOW'), K('res', 'RING')));
  else more.append(grp('OSC', K('w1', 'WAVE 1', { steps: 5, fmt: f => WNAME[Math.round(f * 4)] }), K('w2', 'WAVE 2', { steps: 5, fmt: f => WNAME[Math.round(f * 4)] }), K('pwm', 'PWM'), K('det', 'DETUNE'), K('mix', 'MIX 2', { fmt: pct }), K('sub', 'SUB', { fmt: pct }), K('noise', 'BREATH', { fmt: pct })));
  if (eng === 1) more.append(grp('VOWEL', K('vowel', 'A·E·I·O·U', { fmt: f => 'AEIOU'[Math.round(f * 4)] })));
  more.append(grp('FILTER ENV', K('env', 'ENV'), K('fa', 'F.ATTACK', { fmt: msF }), K('fd', 'F.DECAY', { fmt: msF })), grp('AMP', K('d', 'DECAY', { fmt: msF }), K('s', 'SUSTAIN', { fmt: pct })), grp('MOTION', K('vib', 'VIBRATO'), K('glide', 'GLIDE'), K('spread', 'WIDTH')));
  T.append(btn(UI.more ? 'LESS' : 'MORE', () => { UI.more = !UI.more; uiSnd(); }, 'cy'), more);
}
function listen(i) { const ch = i === 2 ? [0, 2, 4] : i === 1 ? [0] : [0, 2, 4, 7]; ch.forEach((d, k) => setTimeout(() => { const m = degToMidi(i, d, 0); Bridge.cmd(CMD.NOTE, i, m, 100); setTimeout(() => Bridge.cmd(CMD.NOTE, i, m, 0), i === 2 || i === 3 ? 1800 : 300); }, i === 2 ? 0 : k * 220)); }

/* ================================================================= SPACE TAPE (a full three-head echo panel) */
const MODE_LBL = ECHO_MODES.map(m => (m.h ? [1, 2, 4].map((b, k) => (m.h & b ? k + 1 : '')).filter(Boolean).join('+') : '') + (m.r ? (m.h ? '+R' : 'R') : ''));
function uiTape() {
  const T = $('#tapeCtl'); T.innerHTML = '';
  const mode = pstep(P.P_ST_MODE);
  const ms = h('div', { class: 'modesel' }); ECHO_MODES.forEach((m, k) => { const b = h('button', { class: mode === k ? 'on' : '' }, String(k + 1), h('small', {}, MODE_LBL[k])); b.onclick = () => { setPStep(P.P_ST_MODE, k); uiTape(); }; ms.append(b); });
  T.append(h('div', { class: 'row', style: 'margin-top:10px;align-items:center' }, h('span', { class: 'lab', style: 'font-size:10px;letter-spacing:.2em;color:var(--dim)' }, 'MODE SELECTOR'), btn(PARAMS[P.P_ST_ON] > .5 ? 'ECHO ON' : 'ECHO OFF', () => { setP(P.P_ST_ON, PARAMS[P.P_ST_ON] > .5 ? 0 : 1); uiTape(); }, 'cy', PARAMS[P.P_ST_ON] > .5),
    btn('SYNC', () => { setP(P.P_ST_SYNC, PARAMS[P.P_ST_SYNC] > .5 ? 0 : 1); uiTape(); }, 'cy', PARAMS[P.P_ST_SYNC] > .5), PARAMS[P.P_ST_SYNC] > .5 ? sel(['1/32', '1/16', '1/16.', '1/8T', '1/8', '1/8.', '1/4', '1/4.'].map((n, k) => ({ v: k, n })), pstep(P.P_ST_DIV), v => setPStep(P.P_ST_DIV, +v)) : ''), ms);
  T.append(h('div', { class: 'tapepanel' },
    pknob(P.P_ST_INPUT, 'INPUT VOLUME', { k: 't', cls: 'lg' }), h('span', { class: 'led', id: 'peakLed', title: 'PEAK' }),
    pknob(P.P_ST_RATE, 'REPEAT RATE', { k: 't', cls: 'lg', fmt: () => Math.round((TELE[15] || 0.3) * 1000) + 'ms' }), pknob(P.P_ST_INT, 'INTENSITY', { k: 't', cls: 'lg ro', fmt: pct }),
    pknob(P.P_ST_ECHO, 'ECHO VOLUME', { k: 't', cls: 'lg', fmt: pct }), pknob(P.P_ST_REVERB, 'REVERB VOLUME', { k: 't', cls: 'lg', fmt: pct }),
    pknob(P.P_ST_BASS, 'BASS', { k: 't', fmt: f => (f > .5 ? '+' : '') + Math.round((f - .5) * 24) + 'dB', def: .5 }), pknob(P.P_ST_TREBLE, 'TREBLE', { k: 't', fmt: f => (f > .5 ? '+' : '') + Math.round((f - .5) * 24) + 'dB', def: .5 })));
  T.append(h('div', { class: 'tapepanel' }, pknob(P.P_ST_WOW, 'WOW', { k: 't', cls: 'cy' }), pknob(P.P_ST_FLUTTER, 'FLUTTER', { k: 't', cls: 'cy' }), pknob(P.P_ST_AGE, 'TAPE AGE', { k: 't', cls: 'ro' }),
    btn('❄ ' + tr_('FREEZE'), () => { Bridge.cmd(CMD.PERF, PERF.FREEZE, 1); setTimeout(uiTape, 50); }, 'cy big', PARAMS[P.P_ST_FREEZE] > .5), btn('↻ ' + tr_('SPIN'), () => Bridge.cmd(CMD.PERF, PERF.SPIN, 1), 'big'), btn('↺', () => Bridge.cmd(CMD.PERF, PERF.SPIN_BACK, 1), 'big'),
    btn('⚡ ' + tr_('SPRING KICK'), () => Bridge.cmd(CMD.PERF, PERF.KICK, 1), 'rose big')));
  T.append(h3('COSMOS REVERB'), h('div', { class: 'grps' }, grp('COSMOS', pknob(P.P_COS_SIZE, 'SIZE', { fmt: f => (1.2 + f * 12).toFixed(1) + 's' }), pknob(P.P_COS_DAMP, 'DAMP'), pknob(P.P_COS_PRE, 'PRE-DELAY', { fmt: f => Math.round(f * 250) + 'ms' }), pknob(P.P_COS_VOL, 'VOLUME', { fmt: pct }))));
  T.append(hint(es() ? 'Tres cabezas leen la misma cinta. Los modos 1–4 son solo eco, del 5 al 11 se suma el resorte y el 12 es solo resorte. Sube INTENSITY más allá de las 3 y la cinta canta sola; cambia REPEAT RATE mientras suena y oirás el motor acelerar y frenar. Golpea el resorte con SPRING KICK.' : 'Three heads read the same tape. Modes 1–4 are echo only, 5–11 add the spring, 12 is spring only. Turn INTENSITY past 3 o\'clock and the tape sings by itself; move REPEAT RATE while it plays and you hear the motor speed up and slow down. Kick the spring with SPRING KICK.'));
}

/* ================================================================= FX & FILTERS */
const FX_ES = ['pasabajos clásico 24 dB', 'diodo resonante, ácido', 'agresivo, rasposo', 'pasaaltos de DJ', 'pasabanda', 'formante que habla', 'auto-wah por envolvente', 'muesca que barre', 'fase giratoria', 'flanger tipo jet', 'chorus de ensamble', 'repetición de beat', 'bits y polvo', 'cinta a media velocidad', 'compuerta rítmica', 'modulación en anillo'];
const FX_EN = ['classic 24 dB low-pass', 'resonant diode, acid', 'aggressive, raspy', 'DJ high-pass', 'band-pass', 'talking formant', 'envelope auto-wah', 'sweeping notch', 'swirling phaser', 'jet flanger', 'ensemble chorus', 'beat repeat', 'bits and dust', 'half-speed tape', 'rhythmic gate', 'ring modulation'];
function uiFx() {
  const T = $('#t-fx'); T.innerHTML = '';
  const cur = pstep(P.P_FX_TYPE), on = PARAMS[P.P_FX_ON] > .5;
  const grid = h('div', { class: 'fxgrid' });
  FX_NAMES.forEach((n, k) => {
    const b = h('button', { class: (k === cur ? 'sel ' : '') + (k === cur && on ? 'live' : '') }, n, h('small', {}, (es() ? FX_ES : FX_EN)[k]));
    let downT = 0;
    b.addEventListener('pointerdown', e => { e.preventDefault(); downT = performance.now(); if (pstep(P.P_FX_TYPE) !== k) { setPStep(P.P_FX_TYPE, k); setP(P.P_FX_ON, 0); setTimeout(() => setP(P.P_FX_ON, 1), 90); } else setP(P.P_FX_ON, PARAMS[P.P_FX_ON] > .5 ? 0 : 1); uiFx(); });
    b.addEventListener('pointerup', () => { if (UI.fxHold && performance.now() - downT > 350) { setP(P.P_FX_ON, 0); uiFx(); } });
    grid.append(b);
  });
  T.append(h('div', { class: 'row', style: 'gap:18px;align-items:flex-start' }, grid, h('div', {}, h('canvas', { id: 'fxpad', width: 600, height: 600 }),
    h('div', { class: 'row' }, pknob(P.P_FX_MIX, 'FX MIX', { fmt: pct }), pknob(P.P_FX_X, 'X', { cls: 'cy' }), pknob(P.P_FX_Y, 'Y', { cls: 'ro' }), btn(UI.fxHold ? 'HOLD' : 'LATCH', () => { UI.fxHold = !UI.fxHold; uiFx(); }, 'cy', UI.fxHold, es() ? 'HOLD: suena mientras mantienes; LATCH: se queda hasta que lo apagues' : 'HOLD: on while held; LATCH: stays on until you tap again')))));
  T.append(hint(es() ? 'Cada efecto entra en el siguiente 1/16 (REPEAT en el siguiente 1/8) y sale con fundido: nunca corta la música. Arrastra en el cuadro para mover X y Y en vivo. En el KeyLab: rueda de modulación = X.' : 'Every effect engages on the next 1/16 (REPEAT on the next 1/8) and leaves with a fade: it never cuts the music. Drag in the square to play X and Y live. On a KeyLab: mod wheel = X.'));
  fxPadInit();
}
function fxPadInit() {
  const c = $('#fxpad'); if (!c) return;
  const pos = e => { const r = c.getBoundingClientRect(); return [clamp((e.clientX - r.left) / r.width, 0, 1), clamp(1 - (e.clientY - r.top) / r.height, 0, 1)]; };
  c.addEventListener('pointerdown', e => { c.setPointerCapture(e.pointerId); UI.fxPad = true; if (PARAMS[P.P_FX_ON] < .5) setP(P.P_FX_ON, 1); const [x, y] = pos(e); setP(P.P_FX_X, x); setP(P.P_FX_Y, y); });
  c.addEventListener('pointermove', e => { if (!UI.fxPad) return; const [x, y] = pos(e); setP(P.P_FX_X, x); setP(P.P_FX_Y, y); });
  const up = () => { if (UI.fxPad && UI.fxHold) setP(P.P_FX_ON, 0); UI.fxPad = false; drawKnobs(); };
  c.addEventListener('pointerup', up); c.addEventListener('pointercancel', up);
}
function drawFxPad() {
  const c = $('#fxpad'); if (!c) return; const g = c.getContext('2d'), W = c.width, H = c.height; g.clearRect(0, 0, W, H);
  g.strokeStyle = 'rgba(232,223,210,.08)'; for (let i = 1; i < 8; i++) { g.beginPath(); g.moveTo(i * W / 8, 0); g.lineTo(i * W / 8, H); g.moveTo(0, i * H / 8); g.lineTo(W, i * H / 8); g.stroke(); }
  const x = PARAMS[P.P_FX_X] * W, y = (1 - PARAMS[P.P_FX_Y]) * H, on = PARAMS[P.P_FX_ON] > .5, r = on ? 26 + TELE[11] * 60 : 18;
  const gr = g.createRadialGradient(x, y, 0, x, y, r * 3); gr.addColorStop(0, on ? 'rgba(201,138,85,.9)' : 'rgba(147,179,194,.6)'); gr.addColorStop(1, 'rgba(0,0,0,0)'); g.fillStyle = gr; g.beginPath(); g.arc(x, y, r * 3, 0, 6.283); g.fill();
  g.fillStyle = 'rgba(232,223,210,.55)'; g.font = 'italic 26px Georgia, serif'; g.fillText(FX_NAMES[pstep(P.P_FX_TYPE)].toLowerCase(), 22, 44);
}

/* ================================================================= MIX */
function uiMix() {
  const T = $('#t-mix'); T.innerHTML = ''; const box = h('div', { class: 'mix' });
  TRACKS.forEach((t, i) => box.append(h('div', { class: 'strip' }, h('div', { class: 'nm', style: `color:${t.col}` }, t.n), pknob(P.P_VOL1 + i, 'VOL', { fmt: pct }), pknob(P.P_ECH1 + i, 'ECHO', { cls: 'cy', fmt: pct }), pknob(P.P_REV1 + i, 'COSMOS', { fmt: pct }),
    btn('M', () => { SC().mutes[i] = SC().mutes[i] ? 0 : 1; Bridge.cmd(CMD.MUTE, PJ.cur, i, SC().mutes[i]); uiMix(); uiTracks(); }, 'rose', SC().mutes[i]))));
  T.append(box, h('div', { class: 'row', style: 'margin-top:12px' }, pknob(P.P_MASTER, 'MASTER', { fmt: pct, cls: 'lg' }), knob('swing', 'SWING', () => PJ.swing / 127, v => { PJ.swing = Math.round(v * 127); Bridge.cmd(CMD.SWING, PJ.swing); }, { fmt: pct, pid: P.P_SWING })));
}

/* ================================================================= SONG & FILES */
function uiSong() {
  const T = $('#t-song'); T.innerHTML = '';
  T.append(h3(es() ? 'ESCENAS' : 'SCENES'));
  const a = sel('ABCDEFGH'.split(''), PJ.cur, () => {}), b = sel('ABCDEFGH'.split(''), (PJ.cur + 1) % 8, () => {});
  T.append(h('div', { class: 'row' }, es() ? 'copiar' : 'copy', a, es() ? 'a' : 'to', b, btn('COPY', () => { Bridge.cmd(CMD.COPY_SCENE, +a.value, +b.value); setTimeout(refreshProject, 60); }), btn('CLEAR', () => { Bridge.cmd(CMD.CLEAR_SCENE, PJ.cur); setTimeout(refreshProject, 60); })));
  T.append(h3(es() ? 'CANCIÓN' : 'SONG')); const list = h('div', { class: 'songl' });
  const sendSong = () => { Bridge.cmd(CMD.SONG_LEN, PJ.songLen); for (let k = 0; k < PJ.songLen; k++) Bridge.cmd(CMD.SONG_ENTRY, k, PJ.song[k].s, PJ.song[k].b); };
  for (let k = 0; k < PJ.songLen; k++) { const e = PJ.song[k]; const bars = h('input', { type: 'number', min: 1, max: 128, value: e.b }); bars.onchange = () => { e.b = clamp(+bars.value || 4, 1, 128); sendSong(); };
    list.append(h('div', { class: 'it' + (PJ.songOn && TR.playing && TELE[10] === k ? ' now' : '') }, h('b', {}, k + 1), sel('ABCDEFGH'.split(''), e.s, v => { e.s = +v; sendSong(); }), bars, es() ? 'compases' : 'bars', btn('✕', () => { PJ.song.splice(k, 1); PJ.song.push({ s: 0, b: 8 }); PJ.songLen--; sendSong(); uiSong(); }))); }
  T.append(list, h('div', { class: 'row', style: 'margin-top:8px' }, btn('+', () => { if (PJ.songLen < 32) { PJ.song[PJ.songLen] = { s: PJ.cur, b: 8 }; PJ.songLen++; sendSong(); uiSong(); } }), btn('SONG MODE', () => $('#songBtn').click(), 'cy', PJ.songOn)));
  T.append(h3(es() ? 'EXPORTAR' : 'EXPORT'), h('div', { class: 'row' }, btn('⤓ MIDI ' + (es() ? 'ESCENA' : 'SCENE'), () => exportMidi(false)), btn('⤓ MIDI ' + (es() ? 'CANCIÓN' : 'SONG'), () => exportMidi(true)), Bridge.kind === 'web' ? btn('● REC WAV', () => $('#recBtn').click(), 'rose', REC.on) : ''));
  T.append(h3(es() ? 'PROYECTO' : 'PROJECT'));
  const fi = h('input', { type: 'file', accept: '.andromeda,application/octet-stream', style: 'display:none' });
  fi.onchange = async () => { const f = fi.files[0]; if (!f) return; const bytes = new Uint8Array(await f.arrayBuffer()); if (bytes[0] !== 65 || bytes[1] !== 78) { toast('?'); return; } Bridge.setProject(bytes); setTimeout(refreshProject, 100); toast(f.name); };
  T.append(h('div', { class: 'row' }, btn('⤓ ' + (es() ? 'GUARDAR' : 'SAVE'), async () => { const r = await Bridge.getProject(); download(new Blob([r.bytes]), 'my-song.andromeda'); }), btn('⤒ ' + (es() ? 'ABRIR' : 'OPEN'), () => fi.click()), fi, btn(es() ? 'NUEVO' : 'NEW', () => { if (confirmNew()) { Bridge.cmd(CMD.NEW_PROJECT); setTimeout(refreshProject, 80); } })));
  T.append(hint(es() ? 'Los archivos .andromeda se abren igual en la web y en el plugin.' : '.andromeda files open the same in the web version and in the plugin.'));
}
function confirmNew() { if (UI.newArm && performance.now() - UI.newArm < 2500) { UI.newArm = 0; return true; } UI.newArm = performance.now(); toast(es() ? 'Toca NUEVO otra vez' : 'Press NEW again'); return false; }
function download(blob, name) { const a = document.createElement('a'); a.href = URL.createObjectURL(blob); a.download = name; document.body.appendChild(a); a.click(); setTimeout(() => { URL.revokeObjectURL(a.href); a.remove(); }, 2000); }

/* ================================================================= INPUT · MIDI */
const INP = { stream: null, src: null, file: null };
function uiSetup() {
  const T = $('#t-setup'); T.innerHTML = '';
  T.append(h3(es() ? 'ENTRADA DE AUDIO' : 'AUDIO INPUT'), hint(es() ? 'Micrófono, guitarra o un archivo: pasan por su filtro, por el Space Tape, la reverb Cosmos y todos los efectos. Secuencia la pista INPUT para cortarla en ritmo.' : 'Mic, guitar or a file: it goes through its own filter, the Space Tape, the Cosmos reverb and every effect. Sequence the INPUT track to gate it in rhythm.'));
  if (Bridge.kind === 'web') {
    T.append(h('div', { class: 'row' }, btn(INP.stream ? '■ MIC' : '🎙 MIC', async () => { if (INP.stream) { INP.stream.getTracks().forEach(t => t.stop()); INP.src.disconnect(); INP.stream = null; Bridge.input(false); uiSetup(); return; }
        try { INP.stream = await navigator.mediaDevices.getUserMedia({ audio: { echoCancellation: false, noiseSuppression: false, autoGainControl: false } }); INP.src = Bridge.ctx.createMediaStreamSource(INP.stream); INP.src.connect(Bridge.node); Bridge.input(true); } catch (e) { toast(e.message); } uiSetup(); }, INP.stream ? 'rose on' : 'prim'),
      btn('⤒ ' + (es() ? 'SUBIR AUDIO' : 'LOAD AUDIO'), () => fileIn.click(), 'cy'), INP.file ? h('span', { class: 'badge' }, INP.file) : '', INP.file ? btn(INP.playing ? '■' : '▶', () => { INP.playing = !INP.playing; Bridge.cmd(CMD.FILE_PLAY, INP.playing ? 1 : 0, 100); uiSetup(); }, 'cy', INP.playing) : ''));
  } else T.append(h('div', { class: 'row' }, btn('⤒ ' + (es() ? 'SUBIR AUDIO' : 'LOAD AUDIO'), async () => { const r = await Bridge.pluginFileDialog(); if (r) { INP.file = r; INP.playing = true; uiSetup(); } }, 'cy'), INP.file ? h('span', { class: 'badge' }, INP.file) : '', INP.file ? btn(INP.playing ? '■' : '▶', () => { INP.playing = !INP.playing; Bridge.cmd(CMD.FILE_PLAY, INP.playing ? 1 : 0, 100); uiSetup(); }, 'cy', INP.playing) : '',
    h('span', { class: 'hint', style: 'margin:0' }, es() ? 'o manda audio a ANDROMEDA por sidechain' : 'or send audio to ANDROMEDA via sidechain')));
  T.append(h('div', { class: 'grps', style: 'margin-top:8px' }, grp('INPUT', pknob(P.P_IN_FILTER, 'FILTER', { cls: 'cy' }), pknob(P.P_IN_RES, 'RESO', { cls: 'cy' }), pknob(P.P_IN_DRIVE, 'DRIVE', { cls: 'ro' }), pknob(P.P_IN_MOTION, 'MOTION'), pknob(P.P_VOL8, 'LEVEL', { k: 'i', fmt: pct }), pknob(P.P_ECH8, 'TO TAPE', { k: 'i', cls: 'cy', fmt: pct }), pknob(P.P_REV8, 'TO COSMOS', { k: 'i', fmt: pct }))));
  T.append(h3('MIDI'));
  if (Bridge.kind === 'web') T.append(h('div', { class: 'row' }, btn(MIDI.on ? 'MIDI ✓' : 'ENABLE MIDI', midiInit, MIDI.on ? 'cy on' : 'prim')));
  T.append(h('div', { class: 'row', style: 'margin-top:6px' }, btn('LEARN', () => { UI.learn = !UI.learn; uiLearn(); toast(es() ? 'Toca una perilla en pantalla, luego mueve tu controlador' : 'Touch a knob on screen, then move your controller'); }, 'cy', UI.learn),
    h('span', { class: 'hint', style: 'margin:0' }, es() ? 'Canal de pads:' : 'Pad channel:'), sel([...Array(16)].map((_, k) => ({ v: k, n: 'ch ' + (k + 1) })), UI.padCh ?? 9, v => { UI.padCh = +v; Bridge.cmd(CMD.PAD_CHANNEL, +v); })));
  T.append(h3('KEYLAB ESSENTIAL 49'), h('table', { class: 'map' }, ...(es() ? [
    ['Teclas', 'tocan la pista seleccionada (TECLAS: LIBRE / AJUSTAR / MODULAR)'], ['Perillas + pantalla', 'en Ableton, modo DAW: controlan los 8 parámetros del banco que ves en la pantalla del KeyLab (PERFORM, SPACE TAPE, COSMOS, MIX, SONIDO, FX). Cambia de banco con los botones de banco/página'],
    ['Pads 1–8 (canal 10)', 'escenas A–H, en tiempo'], ['Pads 9–16', 'FREEZE · TAPE STOP · SPIN · FILL · SUPERNOVA · ECLIPSE · SPRING KICK · FX on/off'], ['Faders', 'con LEARN asígnalos a volúmenes o a lo que quieras'], ['Rueda de modulación', 'FX X'], ['Pedal sustain', 'FREEZE']]
    : [['Keys', 'play the selected track (KEYS: RAW / SNAP / SHIFT)'], ['Knobs + screen', 'in Ableton DAW mode: they control the 8 parameters of the bank shown on the KeyLab screen (PERFORM, SPACE TAPE, COSMOS, MIX, SOUND, FX). Switch banks with the bank/page buttons'],
    ['Pads 1–8 (channel 10)', 'scenes A–H, in time'], ['Pads 9–16', 'FREEZE · TAPE STOP · SPIN · FILL · SUPERNOVA · ECLIPSE · SPRING KICK · FX on/off'], ['Faders', 'use LEARN to assign them to volumes or anything'], ['Mod wheel', 'FX X'], ['Sustain pedal', 'FREEZE']]).map(r => h('tr', {}, h('td', {}, r[0]), h('td', {}, r[1])))));
}
const fileIn = h('input', { type: 'file', accept: 'audio/*', style: 'display:none' });
fileIn.onchange = async () => { const f = fileIn.files[0]; if (!f) return; try { const buf = await Bridge.ctx.decodeAudioData(await f.arrayBuffer()); const L = buf.getChannelData(0).slice(), R = buf.numberOfChannels > 1 ? buf.getChannelData(1).slice() : null; Bridge.loadFile(L, R); INP.file = f.name; INP.playing = true; Bridge.cmd(CMD.FILE_PLAY, 1, 100); toast(f.name); uiSetup(); } catch (e) { toast(e.message); } };
function uiLearn() { Object.values(KNOBS).forEach(k => k.el.classList.toggle('learn', UI.learn && k.pid !== undefined)); }
const MIDI = { on: false };
async function midiInit() {
  if (!navigator.requestMIDIAccess) { toast('Web MIDI: Chrome / Edge'); return; }
  try { const acc = await navigator.requestMIDIAccess(); const bind = () => acc.inputs.forEach(i => (i.onmidimessage = e => { const d = e.data; if (d.length >= 2 && (d[0] & 0xF0) !== 0xF0) Bridge.cmd(CMD.MIDI, d[0], d[1], d[2] || 0); })); bind(); acc.onstatechange = bind; MIDI.on = true; toast('MIDI ✓'); uiSetup(); } catch (e) { toast('MIDI ✕'); }
}

/* ================================================================= HELP */
const DONATE_URL = 'https://www.paypal.com/donate/?business=drophlex%40gmail.com&no_recurring=0&item_name=ANDROMEDA&currency_code=USD';
function openLink(url) { if (Bridge.kind === 'plugin') Bridge._call('openUrl', [url]); else window.open(url, '_blank', 'noopener'); }
function uiHelp() {
  const T = $('#t-help'); T.innerHTML = '';
  T.append(h('p', { class: 'hint', style: 'margin:0 0 10px' }, es() ? 'ANDROMEDA es gratis y de código abierto. Si te gusta, puedes apoyarlo con una donación: ' : 'ANDROMEDA is free and open source. If you enjoy it, you can support it with a donation: ', (() => { const b = h('a', { class: 'don', href: DONATE_URL }, '♥ PayPal'); b.onclick = e => { e.preventDefault(); openLink(DONATE_URL); }; return b; })(), ' · ', (() => { const u = 'https://github.com/drophlex/ANDROMEDA'; const b = h('a', { href: u, style: 'color:inherit' }, es() ? 'código fuente (AGPL-3.0)' : 'source code (AGPL-3.0)'); b.onclick = e => { e.preventDefault(); openLink(u); }; return b; })(), ' · ', (() => { const u = 'https://drophlex.github.io/ANDROMEDA/guide/'; const b = h('a', { href: u, style: 'color:inherit' }, es() ? 'guía rápida y manuales' : 'quick guide & manuals'); b.onclick = e => { e.preventDefault(); openLink(u); }; return b; })()));
  T.append(h3(es() ? 'EMPEZAR' : 'START'), h('ol', { class: 'hint', style: 'line-height:1.8' }, ...(es() ? ['Elige un ESTADO en ✦ TOCAR y presiona ALINEAR CANCIÓN.', 'Dibuja o cambia notas en la cuadrícula (todo está en tono).', 'Enciende ✦ AUTO: la música sigue sola a partir de tu patrón.', 'Toca encima con el teclado: con MODULAR, una nota fuera de escala cambia la escala con suavidad en el siguiente tiempo.', 'Juega con SPACE TAPE, los 16 EFECTOS Y FILTROS y las macros DERIVA · BRUMA · FLORECER · GRAVEDAD.']
    : ['Pick a MOOD in ✦ PLAY and press ALIGN SONG.', 'Draw or change notes in the grid (everything is in key).', 'Turn on ✦ AUTO: the music keeps going from your pattern.', 'Play over it on a keyboard: with SHIFT, a note outside the scale glides the scale on the next beat.', 'Play the SPACE TAPE, the 16 FX & FILTERS and the DRIFT · HAZE · BLOOM · GRAVITY macros.']).map(x => h('li', {}, x))));
  T.append(h3(es() ? 'TECLADO DE LA COMPU' : 'COMPUTER KEYS'), h('table', { class: 'map' }, ...[['SPACE', 'play / stop'], ['Z S X D C V G B H N J M , L . ; /', es() ? 'tocar en tono' : 'play in key'], ['1 … 8', es() ? 'elegir pista' : 'select track'], ['Q W E R T Y U I', es() ? 'escenas A–H' : 'scenes A–H'], ['A', 'AUTO'], ['O', 'FREEZE'], ['P', 'TAPE STOP'], ['[', 'SPIN'], [']', 'FILL'], ['\\', 'SUPERNOVA'], ["'", 'ECLIPSE'], ['K', 'SPRING KICK'], ['F', 'FX on/off'], ['`', es() ? 'observatorio' : 'observatory']].map(r => h('tr', {}, h('td', {}, r[0]), h('td', {}, r[1])))));
  T.append(h3(es() ? 'VISIONES' : 'VISIONS'), hint(es() ? 'CYMATICS: arena en una placa que vibra con cada acorde · HARMONOGRAPH: péndulos que dibujan las proporciones del acorde · ATTRACTOR: un mapa caótico · SCOPE: el sonido real · ORRERY: la carta astral.' : 'CYMATICS: sand on a plate that vibrates with each chord · HARMONOGRAPH: pendulums drawing the chord ratios · ATTRACTOR: a chaotic map · SCOPE: the real sound · ORRERY: the sky chart.'));
}

/* ================================================================= live bar */
function uiMacros() { const box = $('#macros'); box.innerHTML = ''; [[P.P_DRIFT, 'DRIFT', 'cy'], [P.P_HAZE, 'HAZE', 'ro'], [P.P_BLOOM, 'BLOOM', ''], [P.P_GRAVITY, 'GRAVITY', 'cy']].forEach(([id, l, c]) => box.append(pknob(id, l, { k: 'm', cls: c, def: id === P.P_GRAVITY ? .5 : undefined, fmt: id === P.P_GRAVITY ? f => (f < .47 ? 'LP' : f > .53 ? 'HP' : '—') : pct }))); }
const PERFB = [['FREEZE', PERF.FREEZE, 'O', 'cy'], ['TAPE STOP', PERF.STOP, 'P', 'rose', 1], ['SPIN', PERF.SPIN, '['], ['FILL', PERF.FILL, ']'], ['SUPERNOVA', PERF.NOVA, '\\'], ['ECLIPSE', PERF.ECLIPSE, "'", 'rose', 1], ['SPRING KICK', PERF.KICK, 'K'], ['AUTO', PERF.AUTO, 'A', 'cy']];
function uiPerf() {
  const box = $('#perf'); if (!box.children.length) PERFB.forEach(([l, id, k, cls, hold]) => { const b = h('button', { class: cls || '', title: 'key ' + k }, tr_(l)); b.dataset.p = id;
    b.addEventListener('pointerdown', e => { e.preventDefault(); Bridge.cmd(CMD.PERF, id, 1); if (id === PERF.AUTO) setTimeout(refreshProject, 80); if (hold) b.classList.add('on'); });
    if (hold) { const up = () => { if (b.classList.contains('on')) { Bridge.cmd(CMD.PERF, id, 0); b.classList.remove('on'); } }; b.addEventListener('pointerup', up); b.addEventListener('pointerleave', up); }
    box.append(b); });
  $$('#perf button').forEach(b => { const id = +b.dataset.p; if (id === PERF.FREEZE) b.classList.toggle('on', TELE[37] > .5); if (id === PERF.AUTO) b.classList.toggle('on', TELE[39] > .5); });
}
function xyInit() {   // when an FX is on, the pad plays it; otherwise it plays the tape
  const c = $('#xy'); const pos = e => { const r = c.getBoundingClientRect(); return [clamp((e.clientX - r.left) / r.width, 0, 1), clamp(1 - (e.clientY - r.top) / r.height, 0, 1)]; };
  c.addEventListener('pointerdown', e => { c.setPointerCapture(e.pointerId); const fx = PARAMS[P.P_FX_ON] > .5; UI.xy = { fx, save: [PARAMS[P.P_ST_RATE], PARAMS[P.P_ST_INT]], trail: [] }; move(e); });
  const move = e => { if (!UI.xy) return; const [x, y] = (UI.xy.p = pos(e)); if (UI.xy.fx) { setP(P.P_FX_X, x); setP(P.P_FX_Y, y); } else { setP(P.P_ST_RATE, x); setP(P.P_ST_INT, 0.25 + y * 0.75); } UI.xy.trail.push([x, y]); if (UI.xy.trail.length > 30) UI.xy.trail.shift(); };
  c.addEventListener('pointermove', move);
  const up = () => { if (!UI.xy) return; if (!UI.xy.fx) { setP(P.P_ST_RATE, UI.xy.save[0]); setP(P.P_ST_INT, UI.xy.save[1]); } UI.xy = null; drawKnobs(); };
  c.addEventListener('pointerup', up); c.addEventListener('pointercancel', up);
}
function drawXY() {
  const c = $('#xy'), g = c.getContext('2d'), W = c.width, H = c.height; g.clearRect(0, 0, W, H);
  g.strokeStyle = 'rgba(147,179,194,.14)'; for (let i = 1; i < 6; i++) { g.beginPath(); g.moveTo(i * W / 6, 0); g.lineTo(i * W / 6, H); g.stroke(); }
  const fx = PARAMS[P.P_FX_ON] > .5; g.fillStyle = 'rgba(232,223,210,.5)'; g.font = 'italic 18px Georgia, serif'; g.fillText(fx ? FX_NAMES[pstep(P.P_FX_TYPE)].toLowerCase() : 'space tape', 12, 26);
  const p = UI.xy && UI.xy.p ? UI.xy.p : fx ? [PARAMS[P.P_FX_X], PARAMS[P.P_FX_Y]] : [PARAMS[P.P_ST_RATE], (PARAMS[P.P_ST_INT] - .25) / .75];
  if (UI.xy) UI.xy.trail.forEach(([x, y], k) => { g.fillStyle = `rgba(147,179,194,${k / 40})`; g.beginPath(); g.arc(x * W, (1 - y) * H, 5, 0, 6.28); g.fill(); });
  g.fillStyle = fx ? '#c98a55' : UI.xy ? '#93b3c2' : 'rgba(201,138,85,.85)'; g.beginPath(); g.arc(p[0] * W, (1 - clamp(p[1], 0, 1)) * H, UI.xy ? 11 : 7, 0, 6.28); g.fill();
}

/* ================================================================= telemetry → screens */
const REC = { on: false, L: [], R: [] };
let lastMod = -1;
function onTele(tele, scope, ev) {
  if (!(tele[1] > 0)) return;   // engine has not run a block yet (no audio device / host not processing)
  TELE = tele; for (let k = 0; k < 512; k++) { SCOPE.L[k] = scope[k * 2]; SCOPE.R[k] = scope[k * 2 + 1]; }
  const wasPlaying = TR.playing; TR.playing = tele[0] > .5; TR.barInScene = tele[2]; TR.chordIdx = tele[6]; TR.chord = tele[7]; TR.queued = tele[5]; for (let i = 0; i < 8; i++) TR.pos[i] = tele[16 + i];
  LIVE.freeze = tele[37] > .5; LIVE.eclipse = tele[38] > .5;
  if (PJ) { if (PJ.cur !== tele[4]) { PJ.cur = tele[4]; uiScene(); if (UI.tab === 'play') uiPlay(); } if (PJ.key !== tele[8] || PJ.scale !== tele[9]) { PJ.key = tele[8]; PJ.scale = tele[9]; uiHeader(); $('.skybox').classList.remove('modflash'); void $('.skybox').offsetWidth; $('.skybox').classList.add('modflash'); if (UI.tab === 'play') uiPlay(); } PJ.bpm = tele[1]; }
  if (wasPlaying !== TR.playing) uiTransport();
  const now = performance.now() / 1000;
  for (let k = 0; k < ev.length; k += 4) { const t = ev[k], a = ev[k + 1], b = ev[k + 2], c = ev[k + 3];
    if (t === 1) VIS.push({ t: now, i: a, midi: a === 0 ? undefined : b, l: a === 0 ? b : undefined, vel: c });
    else if (t === 2) VIS.push({ t: now, bar: true, chord: a, barN: b });
    else if (t === 5) VIS.push({ t: now, nova: true });
    else if (t === 6) VIS.push({ t: now, evolve: true });
    else if (t === 4) toast((es() ? 'modula a ' : 'shifts to ') + KEYS[a] + ' ' + SCALES[b].n); }
  maybeRefresh(tele[40]);
}

/* ================================================================= frame */
function sizeCanvas(c) { const d = Math.min(2, window.devicePixelRatio || 1), w = Math.round(c.clientWidth * d), hh = Math.round(c.clientHeight * d); if (c.width !== w || c.height !== hh) { c.width = w; c.height = hh; } }
let lastFrame = performance.now(), skip = 0;
function frame(manual) {
  if (manual !== true) requestAnimationFrame(frame); if (!PJ) return;
  if (ENGINE_LIGHT() && (skip ^= 1)) return;
  const now = performance.now(), dt = Math.min(0.05, (now - lastFrame) / 1000); lastFrame = now;
  const obs = $('#obs').classList.contains('on'), c = obs ? $('#obsSky') : $('#sky'); sizeCanvas(c); drawVision(c.getContext('2d'), c.width, c.height);
  if (obs) { $('#oL').textContent = `${MOODS[PJ.mood].n} · ${'ABCDEFGH'[PJ.cur]}`; $('#oR').textContent = `${KEYS[PJ.key]} ${SCALES[PJ.scale].n} · ${chordName(TR.chord).r}`; }
  if (UI.tab === 'tape') { tapeLevels(); const tc = $('#tape'); drawTape(tc.getContext('2d'), tc.width, tc.height, dt); const led = $('#peakLed'); if (led) led.classList.toggle('on', TELE[35] > 0.7); }
  if (UI.tab === 'fx') drawFxPad();
  $('#hScene').textContent = 'ABCDEFGH'[PJ.cur] + (TR.queued >= 0 ? ' → ' + 'ABCDEFGH'[TR.queued] : ''); $('#hMood').textContent = MOODS[PJ.mood].n; $('#hBar').textContent = TR.playing ? TR.barInScene + 1 : '—';
  const cn = chordName(TR.playing ? TR.chord : SC().prog[0]); $('#hChord').textContent = `${cn.r} · ${cn.root}`; $('#hKey').textContent = `${KEYS[PJ.key]} ${SCALES[PJ.scale].n}${TELE[39] > .5 ? ' · AUTO' : ''}`; $('#hSong').textContent = PJ.songOn ? `SONG ${TELE[10] + 1}/${PJ.songLen}` : '';
  if (UI.tab === 'play' && TR.playing) { const p = TR.pos[UI.sel]; $$('#t-play .c.now').forEach(x => { if (+x.dataset.s !== p) x.classList.remove('now'); }); $$(`#t-play .c[data-s="${p}"]`).forEach(x => x.classList.add('now')); }
  $$('#tracks .trk').forEach((el, i) => { const f = TELE[24 + i] || 0; el.querySelector('.dot').style.boxShadow = `0 0 ${4 + f * 14}px ${f * 4}px ${TRACKS[i].col}`; });
  drawXY(); uiPerf();
}
function uiAll() { uiHeader(); uiScene(); uiMacros(); uiPerf(); uiTab(); applyLang(); drawKnobs(); }
function applyLang() { $$('[data-i]').forEach(el => (el.textContent = tr_(el.dataset.i))); $('#langBtn').innerHTML = UI.lang === 'es' ? 'EN · <b>ES</b>' : '<b>EN</b> · ES'; $('#ecoBtn').textContent = (es() ? 'MOTOR LIGERO' : 'LIGHT ENGINE') + (ENGINE_LIGHT() ? ' ✓' : ''); }

/* ================================================================= keys */
const KEYMAP = 'zsxdcvgbhnjm,l.;/'.split(''); const held = {};
document.addEventListener('keydown', e => {
  if (e.target.matches('input,select,textarea') || e.metaKey || e.ctrlKey || e.altKey || !PJ) return; const k = e.key.toLowerCase(); if (e.repeat) { if (KEYMAP.includes(k)) e.preventDefault(); return; }
  if (k === ' ') { e.preventDefault(); Bridge.cmd(TR.playing ? CMD.STOP : CMD.PLAY); return; }
  if (e.code.startsWith('Digit')) { const n = +e.code.slice(5) - 1; if (n >= 0 && n < 8) selTrack(n); return; }
  const si = 'qwertyui'.indexOf(k); if (si >= 0) { Bridge.cmd(TR.playing ? CMD.QUEUE_SCENE : CMD.SET_SCENE_NOW, si); return; }
  const pm = { o: PERF.FREEZE, p: PERF.STOP, '[': PERF.SPIN, ']': PERF.FILL, '\\': PERF.NOVA, "'": PERF.ECLIPSE, k: PERF.KICK, a: PERF.AUTO }; if (k in pm) { Bridge.cmd(CMD.PERF, pm[k], 1); return; }
  if (k === 'f') { setP(P.P_FX_ON, PARAMS[P.P_FX_ON] > .5 ? 0 : 1); if (UI.tab === 'fx') uiFx(); return; }
  if (k === '`') { toggleObs(); return; }
  if (k === '-') { UI.oct = clamp(UI.oct - 1, -2, 2); return; } if (k === '=') { UI.oct = clamp(UI.oct + 1, -2, 2); return; }
  const ix = KEYMAP.indexOf(k); if (ix >= 0) { e.preventDefault(); const tr = SC().tracks[UI.sel]; const m = UI.sel === 0 ? 36 + ix : degToMidi(UI.sel, ix + UI.oct * 7, tr.oct); held[k] = m; Bridge.cmd(CMD.NOTE, UI.sel, m, 100); }
});
document.addEventListener('keyup', e => { const k = e.key.toLowerCase(); if (k === 'p') Bridge.cmd(CMD.PERF, PERF.STOP, 0); if (k === "'") Bridge.cmd(CMD.PERF, PERF.ECLIPSE, 0); if (held[k] !== undefined) { Bridge.cmd(CMD.NOTE, UI.sel, held[k], 0); delete held[k]; } });
function toggleObs() { const o = $('#obs'); o.classList.toggle('on'); if (o.classList.contains('on')) { o.requestFullscreen && o.requestFullscreen().catch(() => {}); } else if (document.fullscreenElement) document.exitFullscreen(); }

/* ================================================================= MIDI file export (from the patterns) */
function exportMidi(song) {
  const PPQ = 96, perStep = PPQ / 4, tracks = TRACKS.map(() => []), GM = [36, 38, 42, 46, 39, 37, 45, 56, 70, 76, 56];
  const list = song ? PJ.song.slice(0, PJ.songLen) : [{ s: PJ.cur, b: 8 }]; let k0 = 0;
  const stepIdx = (t, c) => { const L = t.len; if (t.dir === 1) return L - 1 - (c % L); if (t.dir === 2) { const Pp = Math.max(1, 2 * L - 2), x = c % Pp; return x < L ? x : Pp - x; } return c % L; };
  list.forEach(e => { const sc = PJ.scenes[e.s], total = e.b * 16;
    for (let kS = 0; kS < total; kS++) { const bar = Math.floor(kS / 16), cd = sc.prog[Math.floor(bar / Math.max(1, sc.cb)) % 4];
      for (let i = 0; i < 6; i++) { if (sc.mutes[i]) continue; const t = sc.tracks[i], rs = RATES[t.rate].s; if (kS % rs) continue; const st = stepIdx(t, kS / rs), t0 = (k0 + kS) * perStep;
        if (i === 0) { t.lanes.forEach(ln => { const s = ln.steps[st]; if (s.on && s.prob >= 64) tracks[0].push([t0, 0x99, GM[ln.v], s.vel], [t0 + perStep / 2, 0x89, GM[ln.v], 0]); }); continue; }
        const s = t.steps[st]; if (!s.on || s.prob < 64) continue; const d = s.deg + (t.follow ? cd : 0); const notes = t.chord ? [d, d + 2, d + 4].map(x => degToMidi(i, x, t.oct + s.oct)) : [degToMidi(i, d, t.oct + s.oct)];
        notes.forEach(m => tracks[i].push([t0, 0x90 | (i - 1), m, s.vel], [t0 + Math.max(1, Math.round(s.gate4 / 4 * rs * perStep * 0.97)), 0x80 | (i - 1), m, 0])); } }
    k0 += total; });
  const vlq = n => { const b = [n & 0x7f]; while ((n >>= 7)) b.unshift((n & 0x7f) | 0x80); return b; }, str = s => [...s].map(c => c.charCodeAt(0) & 0x7f), chunks = [], tempo = Math.round(60000000 / PJ.bpm);
  chunks.push([0, 0xFF, 0x51, 3, (tempo >> 16) & 255, (tempo >> 8) & 255, tempo & 255, 0, 0xFF, 0x2F, 0]);
  tracks.forEach((ev, i) => { if (!ev.length) return; ev.sort((a, b) => a[0] - b[0] || (a[1] & 0xF0) - (b[1] & 0xF0)); const d = [0, 0xFF, 0x03, ...vlq(TRACKS[i].n.length), ...str(TRACKS[i].n)]; let last = 0; ev.forEach(e => { d.push(...vlq(e[0] - last), e[1], e[2] & 127, e[3] & 127); last = e[0]; }); d.push(0, 0xFF, 0x2F, 0); chunks.push(d); });
  const body = []; chunks.forEach(c => { const L = c.length; body.push(...str('MTrk'), (L >>> 24) & 255, (L >> 16) & 255, (L >> 8) & 255, L & 255, ...c); });
  download(new Blob([new Uint8Array([...str('MThd'), 0, 0, 0, 6, 0, 1, 0, chunks.length, 0, PPQ, ...body])]), song ? 'andromeda-song.mid' : 'andromeda-scene.mid');
}
function saveWav(chL, chR) {
  const n = chL.reduce((a, c) => a + c.length, 0), sr = Bridge.sampleRate, buf = new ArrayBuffer(44 + n * 4), dv = new DataView(buf); const ws = (o, s) => { for (let i = 0; i < s.length; i++) dv.setUint8(o + i, s.charCodeAt(i)); };
  ws(0, 'RIFF'); dv.setUint32(4, 36 + n * 4, true); ws(8, 'WAVE'); ws(12, 'fmt '); dv.setUint32(16, 16, true); dv.setUint16(20, 1, true); dv.setUint16(22, 2, true); dv.setUint32(24, sr, true); dv.setUint32(28, sr * 4, true); dv.setUint16(32, 4, true); dv.setUint16(34, 16, true); ws(36, 'data'); dv.setUint32(40, n * 4, true);
  let o = 44; chL.forEach((c, k) => { const r = chR[k]; for (let i = 0; i < c.length; i++) { dv.setInt16(o, clamp(c[i], -1, 1) * 32767, true); dv.setInt16(o + 2, clamp(r[i], -1, 1) * 32767, true); o += 4; } });
  download(new Blob([buf], { type: 'audio/wav' }), 'ANDROMEDA-' + new Date().toISOString().slice(0, 16).replace(/[:T]/g, '') + '.wav');
}

/* ================================================================= boot */
function splashArt() { const c = $('#splashArt'); if (!c) return; const g = c.getContext('2d'), Z = 640, d = 0.0016, ph = [0, 1.57 + rnd(-.3, .3), 0.8 + rnd(-.3, .3), rnd(-.2, .2)]; g.strokeStyle = 'rgba(30,34,40,.38)'; g.lineWidth = 0.9; g.beginPath();
  for (let i = 0; i < 30000; i++) { const t = i * 0.03, e = Math.exp(-d * t * 6); const x = Z / 2 + Z * 0.22 * (Math.sin(2 * t + ph[0]) + Math.sin(3.003 * t + ph[1])) * e, y = Z / 2 + Z * 0.22 * (Math.sin(3 * t + ph[2]) + Math.sin(2.002 * t + ph[3])) * e; i ? g.lineTo(x, y) : g.moveTo(x, y); }
  g.stroke(); g.fillStyle = '#964e2c'; g.beginPath(); g.arc(Z / 2, Z / 2, 5, 0, 6.283); g.fill(); }
async function boot() {
  UI.vision = LS.get('andromeda.vision') || 'AUTO';
  UI.lang = LS.get('andromeda.lang') === 'es' ? 'es' : 'en';   // English unless ES was chosen applyLang(); splashArt();
  $$('#tabs button').forEach(b => (b.onclick = () => setTab(b.dataset.t)));
  $('#playBtn').onclick = () => Bridge.cmd(TR.playing ? CMD.STOP : CMD.PLAY);
  $('#bpmDn').onclick = () => setBpm(PJ.bpm - 1); $('#bpmUp').onclick = () => setBpm(PJ.bpm + 1); $('#tapBtn').onclick = tap;
  let by; const bv = $('#bpmV'); bv.addEventListener('pointerdown', e => { by = [e.clientY, PJ.bpm]; bv.setPointerCapture(e.pointerId); }); bv.addEventListener('pointermove', e => { if (by) setBpm(by[1] + (by[0] - e.clientY) / 4); }); bv.addEventListener('pointerup', () => (by = null));
  $('#keySel').onchange = e => { PJ.key = +e.target.value; Bridge.cmd(CMD.KEY, PJ.key); uiTab(); };
  $('#scaleSel').onchange = e => { PJ.scale = +e.target.value; Bridge.cmd(CMD.SCALE, PJ.scale); uiTab(); };
  $('#autoBtn').onclick = () => { setP(P.P_AUTO, PARAMS[P.P_AUTO] > .5 ? 0 : 1); uiHeader(); if (UI.tab === 'play') uiPlay(); };
  $('#songBtn').onclick = () => { PJ.songOn = PJ.songOn ? 0 : 1; Bridge.cmd(CMD.SONG_ON, PJ.songOn); uiScene(); };
  $('#recBtn').onclick = () => { if (Bridge.kind !== 'web') return; REC.on = !REC.on; if (REC.on) { REC.L = []; REC.R = []; Bridge.record(true, (L, R) => { REC.L.push(...L); REC.R.push(...R); if (!REC.on) saveWav(REC.L, REC.R); }); toast('● REC'); } else { Bridge.record(false, (L, R) => { REC.L.push(...L); REC.R.push(...R); saveWav(REC.L, REC.R); }); } uiTransport(); };
  $('#obsBtn').onclick = toggleObs; $('#obsX').onclick = toggleObs;
  ['#donBtn', '#donBtn2'].forEach(s => { const d = $(s); if (d) d.onclick = e => { e.preventDefault(); e.stopPropagation(); openLink(DONATE_URL); }; });
  $('#langBtn').onclick = () => { UI.lang = UI.lang === 'es' ? 'en' : 'es'; LS.set('andromeda.lang', UI.lang); uiAll(); };
  $('#ecoBtn').onclick = () => { if (Bridge.kind !== 'web') return; LS.set('andromeda.eco', ENGINE_LIGHT() ? '0' : '1'); saveLocal().then(() => location.reload()); };
  xyInit();
  const go = async () => {
    $('#splash').remove(); Bridge.onTele = onTele; await Bridge.start();
    const saved = LS.get('andromeda.project.v2'); if (saved) { try { Bridge.setProject(Uint8Array.from(atob(saved), c => c.charCodeAt(0))); } catch (e) {} }
    if (Bridge.kind === 'web') { Bridge.onPresets = () => uiTab(); }
    await refreshProject(); setTab('play'); if (Bridge.kind === 'web') Bridge.cmd(CMD.PLAY);
    setInterval(saveLocal, 5000);
  };
  if (window.__JUCE__ && window.__JUCE__.backend) go(); else { $('#startBtn').onclick = e => { e.stopPropagation(); go(); }; $('#splash').onclick = () => $('#startBtn').click(); }
  requestAnimationFrame(frame);
  setInterval(() => { if (performance.now() - lastFrame > 300) frame(true); }, 100);   // some plugin web views throttle animation frames
}
async function saveLocal() { if (Bridge.kind !== 'web' || !Bridge.ready) return; const r = await Bridge.getProject(); let s = ''; for (let i = 0; i < r.bytes.length; i++) s += String.fromCharCode(r.bytes[i]); try { LS.set('andromeda.project.v2', btoa(s)); } catch (e) {} }
boot();
