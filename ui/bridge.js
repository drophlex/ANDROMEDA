/* bridge.js — one UI, two engines: the browser runs the C++ engine as WebAssembly in an AudioWorklet,
   the VST3/AU plugin runs it natively and talks to this page through JUCE. Same commands either way. */
const WORKLET_CODE = `
class AndroProc extends AudioWorkletProcessor {
  constructor(o) {
    super({});
    const mod = new WebAssembly.Module(o.processorOptions.wasm);
    this.X = new WebAssembly.Instance(mod, {}).exports; this.X.__wasm_call_ctors(); this.X.andro_init(sampleRate);
    this.n = 0; this.uiR = 0; this.rec = false; this.recL = []; this.recR = []; this.recN = 0; this.hasIn = false;
    this.port.onmessage = e => this.msg(e.data);
  }
  mem() { return this.X.memory.buffer; }
  msg(d) {
    const X = this.X;
    if (d.t === 'cmd') { const a = new Int32Array(this.mem(), X.andro_args(), 16); for (let i = 0; i < 16; i++) a[i] = d.a[i] | 0; X.andro_cmd(d.op); }
    else if (d.t === 'param') X.andro_param(d.id, d.v);
    else if (d.t === 'getProj') { const n = X.andro_proj_write(); const b = new Uint8Array(this.mem(), X.andro_proj(), n).slice(); const p = new Float32Array(d.np); for (let i = 0; i < d.np; i++) p[i] = X.andro_param_get(i); this.port.postMessage({ t: 'proj', id: d.id, bytes: b, params: p }, [b.buffer, p.buffer]); }
    else if (d.t === 'setProj') { new Uint8Array(this.mem(), X.andro_proj(), d.bytes.length).set(d.bytes); X.andro_proj_read(d.bytes.length); }
    else if (d.t === 'presets') { const out = []; const s = p => { const u = new Uint8Array(this.mem(), p, 24); let r = ''; for (let k = 0; k < 24 && u[k]; k++) r += String.fromCharCode(u[k]); return r; };
      for (let i = 0; i < X.andro_preset_count(); i++) out.push({ n: s(X.andro_preset_name(i)), cat: s(X.andro_preset_cat(i)) }); this.port.postMessage({ t: 'presets', list: out }); }
    else if (d.t === 'file') { const max = X.andro_file_max(), n = Math.min(max, d.L.length); new Float32Array(this.mem(), X.andro_file_l(), n).set(d.L.subarray(0, n)); if (d.R) new Float32Array(this.mem(), X.andro_file_r(), n).set(d.R.subarray(0, n)); X.andro_file_set(n, d.R ? 1 : 0); }
    else if (d.t === 'rec') { this.rec = d.on; if (!d.on && this.recN) { this.port.postMessage({ t: 'recdata', L: this.recL, R: this.recR }); this.recL = []; this.recR = []; this.recN = 0; } }
    else if (d.t === 'in') this.hasIn = d.on;
  }
  process(inputs, outputs) {
    const X = this.X, o = outputs[0], i0 = inputs[0], n = o[0].length;
    const hasIn = this.hasIn && i0 && i0.length > 0;
    if (hasIn) { new Float32Array(this.mem(), X.andro_in_l(), 128).set(i0[0]); new Float32Array(this.mem(), X.andro_in_r(), 128).set(i0[1] || i0[0]); }
    X.andro_process(n, hasIn ? 1 : 0);
    const L = new Float32Array(this.mem(), X.andro_out_l(), n), R = new Float32Array(this.mem(), X.andro_out_r(), n);
    o[0].set(L); if (o[1]) o[1].set(R);
    if (this.rec) { this.recL.push(L.slice()); this.recR.push(R.slice()); this.recN += n; if (this.recN > 48000 * 4) { this.port.postMessage({ t: 'recdata', L: this.recL, R: this.recR }); this.recL = []; this.recR = []; this.recN = 0; } }
    if (++this.n % 8 === 0) {
      const tele = new Float32Array(this.mem(), X.andro_tele(), 48).slice();
      const sw = X.andro_scope_w(), sc = new Float32Array(this.mem(), X.andro_scope(), 2048), scope = new Float32Array(1024);
      for (let k = 0; k < 512; k++) { const j = ((sw - 512 + k) & 1023) * 2; scope[k * 2] = sc[j]; scope[k * 2 + 1] = sc[j + 1]; }
      const uw = X.andro_ui_w(), ui = new Int32Array(this.mem(), X.andro_ui(), 512 * 4), ev = [];
      for (let k = Math.max(this.uiR, uw - 500); k < uw; k++) { const j = (k % 512) * 4; ev.push(ui[j], ui[j + 1], ui[j + 2], ui[j + 3]); } this.uiR = uw;
      this.port.postMessage({ t: 'tele', tele, scope, ev: new Int32Array(ev) }, [tele.buffer, scope.buffer]);
    }
    return true;
  }
}
registerProcessor('andromeda', AndroProc);`;

const Bridge = {
  kind: 'web', ready: false, onTele: null, onProject: null, presets: [], _id: 1, _wait: {}, node: null, ctx: null, recCb: null, sampleRate: 48000,
  async start() {
    if (window.__JUCE__ && window.__JUCE__.backend) return this.startPlugin();
    const AC = window.AudioContext || window.webkitAudioContext;
    const light = ENGINE_LIGHT();
    try { this.ctx = new AC({ latencyHint: 'playback', sampleRate: light ? 44100 : 48000 });   // the engine runs at 44.1/48 kHz even if the interface is at 96 kHz } catch (e) { this.ctx = new AC({ latencyHint: 'playback' }); }
    const url = URL.createObjectURL(new Blob([WORKLET_CODE], { type: 'application/javascript' }));
    try { await this.ctx.audioWorklet.addModule(url); } catch (e) { await this.ctx.audioWorklet.addModule('data:application/javascript,' + encodeURIComponent(WORKLET_CODE)); }
    const wasm = Uint8Array.from(atob(WASM_B64), c => c.charCodeAt(0));
    this.node = new AudioWorkletNode(this.ctx, 'andromeda', { numberOfInputs: 1, numberOfOutputs: 1, outputChannelCount: [2], processorOptions: { wasm } });
    this.node.connect(this.ctx.destination);
    this.node.port.onmessage = e => this._msg(e.data);
    this.sampleRate = this.ctx.sampleRate; this.ready = true;
    this.node.port.postMessage({ t: 'presets' });
    if (this.ctx.state !== 'running') await this.ctx.resume();
  },
  _msg(d) {
    if (d.t === 'tele') { this.onTele && this.onTele(d.tele, d.scope, d.ev); }
    else if (d.t === 'proj') { const f = this._wait[d.id]; delete this._wait[d.id]; f && f({ bytes: d.bytes, params: d.params }); }
    else if (d.t === 'presets') { this.presets = d.list; this.onPresets && this.onPresets(); }
    else if (d.t === 'recdata') this.recCb && this.recCb(d.L, d.R);
  },
  cmd(op, ...a) { const args = new Array(16).fill(0); a.forEach((v, i) => (args[i] = v | 0)); if (this.kind === 'plugin') return this._call('cmd', [op, ...args]); this.node && this.node.port.postMessage({ t: 'cmd', op, a: args }); },
  param(id, v) { if (this.kind === 'plugin') return this._call('param', [id, v]); this.node && this.node.port.postMessage({ t: 'param', id, v }); },
  getProject() { if (this.kind === 'plugin') return this._call('getProject', []).then(r => ({ bytes: Uint8Array.from(atob(r.bytes), c => c.charCodeAt(0)), params: Float32Array.from(r.params) }));
    return new Promise(res => { const id = this._id++; this._wait[id] = res; this.node.port.postMessage({ t: 'getProj', id, np: PARAM_META.length }); }); },
  setProject(bytes) { if (this.kind === 'plugin') return this._call('setProject', [btoa(String.fromCharCode(...bytes))]); this.node.port.postMessage({ t: 'setProj', bytes }); },
  loadFile(L, R) { this.node.port.postMessage({ t: 'file', L, R }, R ? [L.buffer, R.buffer] : [L.buffer]); },
  record(on, cb) { this.recCb = cb; this.node.port.postMessage({ t: 'rec', on }); },
  input(on) { this.node.port.postMessage({ t: 'in', on }); },
  // ---- plugin side (JUCE 8 WebBrowserComponent native functions + events)
  async startPlugin() {
    this.kind = 'plugin'; const B = window.__JUCE__.backend;
    B.addEventListener('tele', d => { const tele = Float32Array.from(d.tele), scope = Float32Array.from(d.scope), ev = Int32Array.from(d.ev); this.onTele && this.onTele(tele, scope, ev); });
    this.presets = await this._call('presets', []); this.ready = true; this.sampleRate = 48000;
  },
  _call(name, args) {
    const B = window.__JUCE__.backend, id = this._id++;
    return new Promise(res => {
      const h = e => { if (e.promiseId === id) { B.removeEventListener('__juce__complete', tok); res(e.result); } };
      const tok = B.addEventListener('__juce__complete', h);
      B.emitEvent('__juce__invoke', { name, params: args, resultId: id });
    });
  },
  pluginFileDialog() { return this._call('openAudioFile', []); },
};
function ENGINE_LIGHT() { try { const v = localStorage.getItem('andromeda.eco'); if (v !== null) return v === '1'; } catch (e) {} return matchMedia('(pointer:coarse)').matches || /Android|iPhone|iPad/i.test(navigator.userAgent); }
