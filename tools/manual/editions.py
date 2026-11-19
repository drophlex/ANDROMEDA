# ANDROMEDA manuals per edition (web, plug-in) cut from the full manual, with the edition stamped on the cover
import subprocess, json
from pypdf import PdfReader, PdfWriter
ED = {'Web': {'skip': [5, 6, 7, 22, 23], 'en': 'WEB EDITION', 'es': 'EDICIÓN WEB'},
      'VST': {'skip': [4], 'en': 'PLUG-IN EDITION · VST3 · AU · MAC APP', 'es': 'EDICIÓN PLUG-IN · VST3 · AU · APP MAC'}}
jobs = []
for ed, d in ED.items():
    for lang in ('en', 'es'):
        h = f'/tmp/andro_stamp_{ed}_{lang}.html'
        open(h, 'w').write(f'<html><style>@page{{size:8.5in 11in;margin:0}}body{{margin:0}}div{{position:absolute;top:.42in;left:0;right:0;text-align:center;font:600 11px "IBM Plex Mono",monospace;letter-spacing:4px;color:#c07a52}}</style><body><div>{d[lang]}</div></body></html>')
        jobs.append([h, h.replace('.html', '.pdf')])
open('/tmp/andro_jobs.json', 'w').write(json.dumps(jobs))
subprocess.run(['node', '-e', '''
const pw=require('/opt/npm-tools/node_modules/playwright/index.js');const fs=require('fs');(async()=>{const b=await pw.chromium.launch({executablePath:'/usr/bin/google-chrome'});const p=await b.newPage();
for(const [s,o] of JSON.parse(fs.readFileSync('/tmp/andro_jobs.json'))){await p.goto('file://'+s);await p.pdf({path:o,width:'8.5in',height:'11in',printBackground:false});}await b.close();})();'''], check=True)
for ed, d in ED.items():
    per = {}
    for lang in ('en', 'es'):
        r = PdfReader(f'ANDROMEDA-Manual-{lang.upper()}.pdf'); w = PdfWriter()
        for i, pg in enumerate(r.pages, 1):
            if i in d['skip']: continue
            w.add_page(pg)
        stamp = PdfReader(f'/tmp/andro_stamp_{ed}_{lang}.pdf').pages[0]
        w.pages[0].merge_page(stamp)
        out = f'ANDROMEDA-{ed}-Manual-{lang.upper()}.pdf'
        with open(out, 'wb') as f: w.write(f)
        per[lang] = out; print(out, len(w.pages))
    w = PdfWriter()
    for lang in ('en', 'es'):
        for pg in PdfReader(per[lang]).pages: w.add_page(pg)
    with open(f'ANDROMEDA-{ed}-Manual-EN-ES.pdf', 'wb') as f: w.write(f)
