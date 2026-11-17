# generates ui/gen_data.js from engine/data.h so the UI and the engine always agree
import re, json, pathlib
root = pathlib.Path(__file__).resolve().parents[1]
d = (root / 'engine/data.h').read_text()
enum = d[d.index('enum Param {'):d.index('P_COUNT')]
params = re.findall(r'\b(P_[A-Z0-9_]+)\b', enum)
meta = re.findall(r'\{"([^"]+)", ([0-9./fN_A-Z\-() ]+?), ([0-9A-Z_]+)\}', d[d.index('static const ParamMeta PARAMS'):d.index('} // namespace')])
moods = re.findall(r'\{"(\w+)", "([A-Z]+)", "([^"]+)", (\d+), (\d+), \{[^}]*\}, "(\w+)", (\d+), (\d+), \{([^}]*)\}', d)
kits = re.findall(r'\{"([A-Z]+)", "([^"]+)", \d+, \d+', d[d.index('static const Kit KITS'):])
pf = re.findall(r'"(\w+)"', d[d.index('PF_NAMES[PF_COUNT]'):d.index('static const uint8_t PF_DEF')])
fx = re.findall(r'"([A-Z\- ]+)"', d[d.index('FX_NAMES[16]'):d.index('constexpr int NFX')])
tex = re.findall(r'"([A-Z ]+)"', d[d.index('TEX_NAMES[7]'):d.index('// ---- moods')])
drums = re.findall(r'"([A-Z ]+)"', d[d.index('DRUM_NAMES[11]'):d.index('struct Kit')])
scales = re.findall(r'\{"(\w+)", "([A-Z ]+)", (\d), \{([0-9, ]+)\}\}', d)
steps = {}
for name, (pn, dv, st) in zip(params, meta):
    steps[name] = int(st) if st.isdigit() else (16 if st == 'NFX' else len(moods) if st == 'NMOODS' else 0)
out = 'const P = ' + json.dumps({n: i for i, n in enumerate(params)}) + ';\n'
out += 'const PARAM_META = ' + json.dumps([{'id': n, 'name': m[0], 'steps': steps[n]} for n, m in zip(params, meta)]) + ';\n'
out += 'const MOODS = ' + json.dumps([{'id': m[0], 'n': m[1], 'd': m[2], 'drums': m[5]} for m in moods]) + ';\n'
out += 'const KITS = ' + json.dumps([{'n': k[0], 'd': k[1]} for k in kits]) + ';\n'
out += 'const PF = ' + json.dumps({n: i for i, n in enumerate(pf)}) + ';\n'
out += 'const FX_NAMES = ' + json.dumps(fx) + ';\nconst TEXTURES = ' + json.dumps([{'n': t} for t in tex]) + ';\nconst DRUM_VOICES = ' + json.dumps(drums) + ';\n'
out += 'const SCALES = ' + json.dumps([{'id': s[0], 'n': s[1], 'i': [int(x) for x in s[3].split(',')][:int(s[2])]} for s in scales]) + ';\n'
(root / 'ui').mkdir(exist_ok=True)
(root / 'ui/gen_data.js').write_text('/* generated from engine/data.h — do not edit */\n' + out)
print(len(params), 'params', len(moods), 'moods', len(kits), 'kits', len(fx), 'fx', len(scales), 'scales', len(pf), 'patch fields')
