import fs from 'fs';
const bytes = fs.readFileSync(new URL('../andromeda.wasm', import.meta.url));
const mod = await WebAssembly.compile(bytes);
console.log('imports', WebAssembly.Module.imports(mod).map(i => i.module + '.' + i.name));
const instance = await WebAssembly.instantiate(mod, {});
const X = instance.exports; X.__wasm_call_ctors(); X.andro_init(48000);
const mem = () => X.memory.buffer;
const args = new Int32Array(mem(), X.andro_args(), 16);
args[0] = 2; args[1] = 0; args[2] = 1; X.andro_cmd(60); // align HAZE song
args[0] = 2; X.andro_cmd(8); X.andro_cmd(1); // scene C, play
const N = 48000 * 20 / 128; let pk = 0, s2 = 0; const t0 = performance.now();
const oL = new Float32Array(mem(), X.andro_out_l(), 128);
for (let i = 0; i < N; i++) { X.andro_process(128, 0); for (let k = 0; k < 128; k++) { const v = oL[k]; if (Math.abs(v) > pk) pk = Math.abs(v); s2 += v * v; } }
const ms = performance.now() - t0;
console.log('load %', (ms / 20000 * 100).toFixed(1), 'peak', pk.toFixed(2), 'rms', Math.sqrt(s2 / (N * 128)).toFixed(3));
const tele = new Float32Array(mem(), X.andro_tele(), 48); console.log('tele bpm', tele[1], 'bar', tele[2], 'scene', tele[4]);
const n = X.andro_proj_write(); console.log('project bytes', n, 'presets', X.andro_preset_count());
const dec = new TextDecoder(); const nameAt = p => { const u = new Uint8Array(mem(), p, 24); return dec.decode(u.subarray(0, u.indexOf(0))); };
console.log('preset 0', nameAt(X.andro_preset_name(0)), nameAt(X.andro_preset_cat(0)));
