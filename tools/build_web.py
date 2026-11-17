# builds dist/index.html (single self-contained page) and plugin/ui/index.html from engine + ui
import subprocess, base64, pathlib, sys
root = pathlib.Path(__file__).resolve().parents[1]
eng = root / 'engine'
subprocess.run([sys.executable, str(root / 'tools/gen_data.py')], check=True)
flags = ['clang++', '--target=wasm32', '-std=c++20', '-O3', '-ffast-math', '-nostdlib', '-fno-exceptions', '-fno-rtti', '-fno-threadsafe-statics',
         '-fvisibility=hidden', '-mbulk-memory', '-Wl,--no-entry', '-Wl,--export-dynamic', '-Wl,--export=__wasm_call_ctors',
         '-Wl,--initial-memory=67108864', '-Wl,--max-memory=67108864', '-Wl,--allow-undefined', '-I.', 'wasm.cpp', '-o', 'andromeda.wasm']
if '--no-wasm' not in sys.argv:
    subprocess.run(flags, cwd=eng, check=True)
wasm = (eng / 'andromeda.wasm').read_bytes()
ui = root / 'ui'
js = 'const WASM_B64 = "' + base64.b64encode(wasm).decode() + '";\n'
for f in ['gen_data.js', 'model.js', 'bridge.js', 'astral.js', 'vision.js', 'app.js']:
    js += f'\n/* ---- {f} ---- */\n' + (ui / f).read_text()
def esc_js(t): return ''.join(c if ord(c) < 128 else ('\\u%04x' % ord(c) if ord(c) < 0x10000 else ''.join('\\u%04x' % u for u in divmod(ord(c) - 0x10000, 0x400) and (0xD800 + (ord(c) - 0x10000) // 0x400, 0xDC00 + (ord(c) - 0x10000) % 0x400))) for c in t)
def esc_html(t): return ''.join(c if ord(c) < 128 else '&#x%x;' % ord(c) for c in t)
tpl = (ui / 'template.html').read_text()
a, b = tpl.split('/*JS*/')
html = esc_html(a) + esc_js(js.replace('</script', '<\\/script')) + esc_html(b)   # pure ASCII: no charset surprises in any web view
for out in [root / 'dist/index.html', root / 'index.html', root / 'plugin/ui/index.html']:
    out.parent.mkdir(parents=True, exist_ok=True); out.write_text(html)
print('ok', len(html) // 1024, 'KB, wasm', len(wasm) // 1024, 'KB')
