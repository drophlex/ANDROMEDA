import { chromium } from '/opt/npm-tools/node_modules/playwright/index.mjs';
const b = await chromium.launch({ executablePath: '/usr/bin/google-chrome', args: ['--allow-file-access-from-files'] });
for (const [src, out] of [['manual_en_es.html', 'ANDROMEDA-Manual-EN-ES.pdf'], ['manual_es.html', 'ANDROMEDA-Manual-ES.pdf'], ['manual_en.html', 'ANDROMEDA-Manual-EN.pdf']]) {
  const p = await b.newPage();
  await p.goto('file:///home/claude/man/' + src, { waitUntil: 'load' });
  await p.evaluate(() => document.fonts.ready); await p.waitForTimeout(800);
  const over = await p.evaluate(() => [...document.querySelectorAll('.page')].map((pg, i) => { const r = pg.getBoundingClientRect(); let m = 0; pg.querySelectorAll('*').forEach(e => { if (e.closest('.art') || e.tagName === 'FOOTER' || e.closest('footer')) return; const q = e.getBoundingClientRect(); if (q.height) m = Math.max(m, q.bottom - r.top); }); return [i + 1, Math.round(m)]; }).filter(x => x[1] > 985));
  console.log(out, 'overflowing pages (px bottom > 990 of 1056):', JSON.stringify(over));
  await p.pdf({ path: out, preferCSSPageSize: true, printBackground: true });
}
await b.close();
