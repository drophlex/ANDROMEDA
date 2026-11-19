# ANDROMEDA user manual — English and Spanish, rendered to PDF by headless Chrome
import os, sys, art
HERE = os.path.dirname(os.path.abspath(__file__))
WEB = 'https://drophlex.github.io/ANDROMEDA/'
ZIP = 'https://github.com/drophlex/ANDROMEDA/releases/latest/download/ANDROMEDA-mac.zip'
DON = 'https://www.paypal.com/donate/?business=drophlex%40gmail.com&amp;no_recurring=0&amp;item_name=ANDROMEDA&amp;currency_code=USD'
IG = 'https://instagram.com/polaris.sound.lab'; YT = 'https://youtube.com/@PolarisSoundLab'
INSTALL = 'curl -fsSL https://drophlex.github.io/ANDROMEDA/install.sh | bash'
L = 'en'
def T(en, es): return en if L == 'en' else es
def img(name): return f'file://{HERE}/shots/{L}_{name}.jpg'
def k(s, c=''): return f'<span class="k {c}">{s}</span>'
def kb(s): return f'<span class="kb">{s}</span>'
def fig(name, cap, pins=None, style=''):
    p = ''.join(f'<div class="pin" style="left:{x}%;top:{y}%">{i+1}</div>' for i, (x, y) in enumerate(pins or []))
    c = f'<div class="cap">{cap}</div>' if cap else ''
    return f'<div class="fig" style="{style}"><img src="{img(name)}">{p}</div>{c}'
def table(head, rows, cls=''):
    h = ''.join(f'<th>{x}</th>' for x in head)
    b = ''.join('<tr>' + ''.join(f'<td>{c}</td>' for c in r) + '</tr>' for r in rows)
    return f'<table class="{cls}"><tr>{h}</tr>{b}</table>'
def kick(s): return f'<div class="kick">{s}</div>'

PAGES = []
def page(body, title='', cls=''):
    PAGES.append((cls, body, title))

# ============================================================== pages
def build():
    PAGES.clear()
    # ---------------------------------------------------------- 1 cover
    page(f'''<div class="cover">
<div class="ctop">{T('USER MANUAL', 'MANUAL DE USUARIO')} · VERSION 1.0</div>
<div class="cart">{art.harmonograph(600, seed=7)}</div>
<div class="ctitle">ANDROMEDA</div>
<div class="csub">{T('an ambient instrument', 'un instrumento ambient')}</div>
<div class="cfor"><span>WEB</span><span>{T('MAC APP', 'APP PARA MAC')}</span><span>{T('ABLETON PLUG-IN (VST3 · AU)', 'PLUG-IN PARA ABLETON (VST3 · AU)')}</span></div>
{'<div class="lang"><span>ENGLISH · PAGE 2</span><span>ESPAÑOL · PÁGINA ' + str(NPL + 1) + '</span></div>' if L == 'en' else '<div class="lang"><span>VERSIÓN EN ESPAÑOL</span></div>'}
<div class="cfoot"><b>DARK FUNK DURCAL DESIGN · POLARIS SOUND LAB</b><br>
<a href="{IG}">Instagram @polaris.sound.lab</a> &nbsp;·&nbsp; <a href="{YT}">YouTube @PolarisSoundLab</a></div>
</div>''', cls='coverp')

    # ---------------------------------------------------------- 2 welcome
    page(f'''{kick(T('Welcome', 'Bienvenida'))}
<h1>{T('Music that <i>breathes</i>,<br>made in minutes.', 'Música que <i>respira</i>,<br>hecha en minutos.')}</h1>
<p class="lead">{T('ANDROMEDA makes warm, floating, nostalgic ambient music: pads that breathe like old tape, choirs, music boxes, bells, soft drum machines, rain and vinyl dust. You don’t need to know music theory: <i>every note is always in key</i>, and if you want, the music keeps writing itself.',
 'ANDROMEDA hace música ambient cálida, flotante y nostálgica: pads que respiran como cinta vieja, coros, cajas de música, campanas, cajas de ritmo suaves, lluvia y polvo de vinilo. No necesitas saber teoría musical: <i>todas las notas siempre están en tono</i> y, si quieres, la música se sigue escribiendo sola.')}</p>
<h3>{T('Three ways to use it — same instrument, same sound', 'Tres formas de usarlo: el mismo instrumento y el mismo sonido')}</h3>
<div class="grid3">
<div class="card"><div class="big">Web</div><p class="small">{T('Open a link in Chrome and play. Nothing to install. Works on computers, tablets and phones.', 'Abres un link en Chrome y tocas. No se instala nada. Funciona en computadora, tablet y celular.')}</p><p class="small"><a href="{WEB}">drophlex.github.io/ANDROMEDA</a></p></div>
<div class="card"><div class="big">{T('Mac app', 'App Mac')}</div><p class="small">{T('ANDROMEDA as a normal program on your Mac (it appears as ANDROMEDA in your Applications). Good for playing live with a keyboard.', 'ANDROMEDA como programa normal en tu Mac (aparece como ANDROMEDA en tus Aplicaciones). Ideal para tocar en vivo con un teclado.')}</p></div>
<div class="card"><div class="big">Ableton</div><p class="small">{T('A plug-in (VST3 or AU) that lives inside Ableton Live or any music program: it follows the tempo, you can record and automate it.', 'Un plug-in (VST3 o AU) que vive dentro de Ableton Live o de cualquier programa de música: sigue el tempo, lo puedes grabar y automatizar.')}</p></div>
</div>
<h3>{T('In this manual', 'En este manual')}</h3>
<div class="toc">{TOC}</div>
<div class="note"><b>English · Español.</b> {T('ANDROMEDA opens in English. The', 'ANDROMEDA abre en inglés. El botón')} <span class="langsw">EN · ES</span> {T('button (top right) switches it to Spanish in the web, the Mac app and the plug-in; it remembers your choice. On the FM-1: VISION page, knob K4 (LANG).', '(arriba a la derecha) lo cambia a español en la web, la app de Mac y el plug-in; recuerda tu elección. En el FM-1: página VISION, perilla K4 (LANG).')}<br><b>{T('Free and open source.', 'Gratis y de código abierto.')}</b> {T('If ANDROMEDA makes you happy, you can support it:', 'Si ANDROMEDA te hace feliz, puedes apoyarlo:')} <a class="paypal" href="{DON}">♥ {T('Donate with PayPal', 'Donar con PayPal')}</a></div>
<div class="note"><b>{T('Tip.', 'Consejo.')}</b> {T('If you are in a hurry, read “Your first song in 5 minutes” and come back to the rest later. Words in boxes like', 'Si tienes prisa, lee “Tu primera canción en 5 minutos” y regresa al resto después. Las palabras en cajitas como')} {k(T('ALIGN SONG', 'ALINEAR CANCIÓN'), 'a')} {T('are buttons you will see on the screen.', 'son botones que verás en la pantalla.')}</div>
''', T('Welcome', 'Bienvenida'))

    # ---------------------------------------------------------- 3 glossary
    gl = [
        (T('Track', 'Pista'), T('One instrument inside ANDROMEDA. There are 8: drums, bass, pad, voice, bells, lead, texture and input.', 'Un instrumento dentro de ANDROMEDA. Hay 8: batería, bajo, pad, voz, campanas, lead, textura y entrada.')),
        (T('Pattern', 'Patrón'), T('The little loop of notes a track plays over and over. You draw it in a grid.', 'El pequeño ciclo de notas que una pista repite una y otra vez. Lo dibujas en una cuadrícula.')),
        (T('Step', 'Paso'), T('One square of the grid. 16 steps usually make one bar (“compás”).', 'Un cuadrito de la cuadrícula. 16 pasos normalmente forman un compás.')),
        (T('Bar', 'Compás'), T('A unit of musical time (4 beats). ANDROMEDA changes things at the start of a bar so it never sounds abrupt.', 'Una unidad de tiempo musical (4 tiempos). ANDROMEDA hace los cambios al empezar un compás para que nunca suene brusco.')),
        ('BPM', T('Beats per minute: the speed of the music.', 'Tiempos por minuto: la velocidad de la música.')),
        (T('Key and scale', 'Tono y escala'), T('The set of notes the music uses. ANDROMEDA chooses them for you so nothing sounds “wrong”.', 'El grupo de notas que usa la música. ANDROMEDA las elige por ti para que nada suene “mal”.')),
        (T('Mood', 'Estado'), T('A ready-made style (DRIFT, GLASS, LULLABY…) that chooses sounds, tempo, scale and rhythm.', 'Un estilo listo (DRIFT, GLASS, LULLABY…) que elige sonidos, tempo, escala y ritmo.')),
        (T('Scene', 'Escena'), T('A complete version of the song (A to H). Changing scene is like moving to the next part of the song.', 'Una versión completa de la canción (de la A a la H). Cambiar de escena es como pasar a la siguiente parte de la canción.')),
        ('Preset', T('A saved sound. Choosing one changes how a track sounds.', 'Un sonido guardado. Al elegirlo cambia cómo suena una pista.')),
        (T('Echo / reverb', 'Eco / reverb'), T('Echo repeats the sound; reverb makes it sound like a big space. ANDROMEDA has a tape echo (Space Tape) and a huge reverb (Cosmos).', 'El eco repite el sonido; la reverb lo hace sonar como en un espacio grande. ANDROMEDA tiene un eco de cinta (Space Tape) y una reverb enorme (Cosmos).')),
        (T('Knob', 'Perilla'), T('Round control. Drag it up or down with the mouse (or the mouse wheel). Double-click to return to its normal value.', 'Control redondo. Arrástrala hacia arriba o hacia abajo con el mouse (o con la rueda del mouse). Doble clic para regresarla a su valor normal.')),
        ('MIDI', T('The language music keyboards and controllers speak. A USB keyboard sends MIDI.', 'El idioma en el que hablan los teclados y controladores musicales. Un teclado USB envía MIDI.')),
        ('DAW', T('A program to make music on a computer, like Ableton Live.', 'Un programa para hacer música en computadora, como Ableton Live.')),
        ('Plug-in · VST3 · AU', T('An instrument that opens inside a DAW. VST3 and AU are the two formats Macs use; ANDROMEDA comes in both.', 'Un instrumento que se abre dentro de un DAW. VST3 y AU son los dos formatos que usa la Mac; ANDROMEDA viene en los dos.')),
    ]
    page(f'''{kick(T('Before you start', 'Antes de empezar'))}
<h1>{T('A few <i>words</i> you will see', 'Algunas <i>palabras</i> que vas a ver')}</h1>
<p class="lead">{T('You don’t need any of this to make music with ANDROMEDA, but knowing these words makes the rest of the manual easy.', 'No necesitas nada de esto para hacer música con ANDROMEDA, pero conocer estas palabras hace fácil el resto del manual.')}</p>
{table([T('Word', 'Palabra'), T('What it means', 'Qué significa')], gl)}
<div class="art" style="right:-.6in;bottom:-.5in;width:3.4in;height:3.4in;opacity:.18">{art.chladni(400, 3, 5, seed=4)}</div>
''', T('Words you will see', 'Palabras que vas a ver'))

    # ---------------------------------------------------------- 4 web
    page(f'''{kick(T('Install · 1 of 3', 'Instalar · 1 de 3'))}
<h1>{T('The <i>web</i> version', 'La versión <i>web</i>')}</h1>
<p class="lead">{T('The fastest way to start. Nothing to install, it is always the newest version, and it saves your work automatically.', 'La forma más rápida de empezar. No se instala nada, siempre es la versión más nueva y guarda tu trabajo solo.')}</p>
<div class="two"><div>
<ol class="steps">
<li>{T('Open <b>Google Chrome</b> (or Edge). Safari works too, but without MIDI keyboards.', 'Abre <b>Google Chrome</b> (o Edge). Safari también funciona, pero sin teclados MIDI.')}</li>
<li>{T('Go to', 'Entra a')} <a href="{WEB}">drophlex.github.io/ANDROMEDA</a> {T('— write ANDROMEDA in capital letters.', '— escribe ANDROMEDA con mayúsculas.')}</li>
<li>{T('Press', 'Presiona')} {k(T('ENTER', 'ENTRAR'), 'a')}. {T('The music starts playing.', 'La música empieza a sonar.')}</li>
<li>{T('Use headphones or good speakers: much of the beauty is in the low notes and the echoes.', 'Usa audífonos o buenas bocinas: mucha de la belleza está en las notas graves y en los ecos.')}</li>
</ol>
<div class="tip"><b>{T('On a phone or tablet', 'En celular o tablet')}</b> {T('ANDROMEDA turns on the', 'ANDROMEDA activa solo el')} {k(T('LIGHT ENGINE', 'MOTOR LIGERO'))} {T('by itself so the sound never breaks up. On a computer you can turn it on if you hear crackles.', 'para que el sonido nunca se corte. En computadora puedes activarlo si escuchas chasquidos.')}</div>
<div class="note"><b>{T('Your work is saved', 'Tu trabajo se guarda')}</b> {T('in the browser every few seconds. To keep a copy or move it to the plug-in, use', 'en el navegador cada pocos segundos. Para tener una copia o pasarla al plug-in, usa')} {k(T('SONG & FILES', 'CANCIÓN Y ARCHIVOS'))} → {k(T('SAVE', 'GUARDAR'))}.</div>
</div><div>{fig('splash', T('<b>fig. 1</b> — the welcome screen. Press ENTER to start the sound (browsers only allow sound after a click).', '<b>fig. 1</b> — la pantalla de bienvenida. Presiona ENTRAR para iniciar el sonido (los navegadores solo permiten sonido después de un clic).'))}
<div class="warn"><b>{T('Speakers via Bluetooth?', '¿Bocinas por Bluetooth?')}</b> {T('They add delay. For playing live with a keyboard, cable headphones feel much better.', 'Agregan retraso. Para tocar en vivo con teclado, los audífonos con cable se sienten mucho mejor.')}</div>
</div></div>
''', T('Web version', 'Versión web'))

    # ---------------------------------------------------------- 5 mac install
    page(f'''{kick(T('Install · 2 of 3', 'Instalar · 2 de 3'))}
<h1>{T('The <i>Mac</i> app and plug-in', 'La <i>app</i> y el plug-in para Mac')}</h1>
<p class="lead">{T('One step installs everything: the plug-in for Ableton (VST3 and AU) and the ANDROMEDA app. It works on any Mac from 2017 on (Apple Silicon or Intel, macOS 11 or newer).', 'Un solo paso instala todo: el plug-in para Ableton (VST3 y AU) y la app ANDROMEDA. Funciona en cualquier Mac desde 2017 (Apple Silicon o Intel, macOS 11 o más nuevo).')}</p>
<h3>{T('Why not just double-click?', '¿Por qué no basta con doble clic?')}</h3>
<p>{T('macOS only opens programs with a paid Apple certificate without asking. ANDROMEDA is free and doesn’t have one yet, so a double-click shows a warning like this one and refuses to open it. It is not a virus: it is simply not registered with Apple. The method below avoids the warning completely.',
 'macOS solo abre sin preguntar los programas que tienen un certificado de pago de Apple. ANDROMEDA es gratis y todavía no lo tiene, así que con doble clic aparece un aviso como este y no deja abrirlo. No es un virus: simplemente no está registrado con Apple. El método de abajo evita el aviso por completo.')}</p>
<div class="dlg"><div class="ic">⚠︎</div><b>{T('“Install ANDROMEDA.command” Not Opened', 'No se abrió “Install ANDROMEDA.command”')}</b>{T('Apple could not verify it is free of malware…', 'Apple no pudo verificar que esté libre de malware…')}<div class="bt"><span>{T('Move to Trash', 'Trasladar a la papelera')}</span><span class="x">{T('Done', 'OK')}</span></div></div>
<h3>{T('Install with one line (recommended)', 'Instalar con una línea (recomendado)')}</h3>
<ol class="steps">
<li>{T('Open <b>Terminal</b>: press', 'Abre la <b>Terminal</b>: presiona')} {kb('⌘ Cmd')} + {kb(T('Space', 'Espacio'))}, {T('type', 'escribe')} <b>Terminal</b> {T('and press', 'y presiona')} {kb('Enter')}. {T('A window with text opens; this is normal.', 'Se abre una ventana con texto; es normal.')}</li>
<li>{T('Copy this line exactly (select it, then', 'Copia esta línea exactamente (selecciónala y luego')} {kb('⌘ Cmd')} + {kb('C')}):<div class="cmd">{INSTALL}</div></li>
<li>{T('Click inside the Terminal window, paste with', 'Haz clic dentro de la ventana de Terminal, pega con')} {kb('⌘ Cmd')} + {kb('V')} {T('and press', 'y presiona')} {kb('Enter')}. {T('Wait about 20 seconds while it downloads.', 'Espera unos 20 segundos mientras descarga.')}</li>
<li>{T('When you read <b>✓ ANDROMEDA installed</b>, you can close Terminal. Done.', 'Cuando leas <b>✓ ANDROMEDA instalado</b>, puedes cerrar la Terminal. Listo.')}</li>
</ol>
<div class="term"><div class="bar"><i class="dot-r"></i><i class="dot-y"></i><i class="dot-g"></i><span class="t">Terminal</span></div><pre><span class="p">~ %</span> {INSTALL}
<span class="o">  A N D R O M E D A  ·  installing / instalando…</span>
<span class="o">  ✓ VST3 → ~/Library/Audio/Plug-Ins/VST3</span>
<span class="o">  ✓ AU   → ~/Library/Audio/Plug-Ins/Components</span>
<span class="o">  ✓ App  → ~/Applications</span>
<span class="ok">  ✓ ANDROMEDA instalado / installed.</span></pre></div>
<p class="small">{T('It asks for no password and only touches ANDROMEDA’s own files. To update ANDROMEDA in the future, run the same line again.', 'No pide contraseña y solo toca los archivos de ANDROMEDA. Para actualizar ANDROMEDA en el futuro, vuelve a correr la misma línea.')}</p>
''', T('Mac install', 'Instalar en Mac'))

    # ---------------------------------------------------------- 6 mac install alt + app
    page(f'''{kick(T('Install · 2 of 3 (continued)', 'Instalar · 2 de 3 (continuación)'))}
<h2>{T('If you already downloaded the zip', 'Si ya descargaste el zip')}</h2>
<p>{T('If you downloaded', 'Si descargaste')} <a href="{ZIP}">ANDROMEDA-mac.zip</a> {T('from the web page, it is in your <b>Downloads</b> folder. Paste this in Terminal instead (one single line) and press Enter:', 'desde la página, está en tu carpeta <b>Descargas</b>. Pega esto en la Terminal (es una sola línea) y presiona Enter:')}</p>
<div class="cmd" style="font-size:8.2pt">cd ~/Downloads &amp;&amp; {{ [ -d ANDROMEDA ] || ditto -x -k ANDROMEDA-mac.zip . ; }} &amp;&amp; xattr -dr com.apple.quarantine ANDROMEDA &amp;&amp; mkdir -p ~/Library/Audio/Plug-Ins/VST3 ~/Library/Audio/Plug-Ins/Components ~/Applications &amp;&amp; cp -R ANDROMEDA/ANDROMEDA.vst3 ~/Library/Audio/Plug-Ins/VST3/ &amp;&amp; cp -R ANDROMEDA/ANDROMEDA.component ~/Library/Audio/Plug-Ins/Components/ &amp;&amp; cp -R ANDROMEDA/ANDROMEDA.app ~/Applications/ &amp;&amp; echo "✓ ANDROMEDA"</div>
<p class="small">{T('What it does, in plain words: unzips the file, removes macOS’s “downloaded from the internet” label, and copies the three parts to the folders where music programs look for them.', 'Qué hace, en palabras simples: descomprime el archivo, quita la etiqueta de macOS de “descargado de internet” y copia las tres partes a las carpetas donde los programas de música las buscan.')}</p>
<h2>{T('Or without Terminal', 'O sin Terminal')}</h2>
<ol class="steps">
<li>{T('Double-click the zip to unzip it. Open the ANDROMEDA folder.', 'Doble clic en el zip para descomprimirlo. Abre la carpeta ANDROMEDA.')}</li>
<li>{T('<b>Right-click</b> (or Control-click) “Install ANDROMEDA.command” → <b>Open</b> → <b>Open</b>.', '<b>Clic derecho</b> (o Control + clic) en “Install ANDROMEDA.command” → <b>Abrir</b> → <b>Abrir</b>.')}</li>
<li>{T('On newer macOS this may still be blocked. Then open <b>System Settings → Privacy &amp; Security</b>, scroll down to the message about “Install ANDROMEDA.command” and click <b>Open Anyway</b>. Confirm with your password.', 'En macOS más nuevos puede seguir bloqueado. Entonces abre <b>Ajustes del Sistema → Privacidad y seguridad</b>, baja hasta el mensaje sobre “Install ANDROMEDA.command” y haz clic en <b>Abrir de todos modos</b>. Confirma con tu contraseña.')}</li>
<li>{T('A Terminal window shows the installation and asks you to press a key at the end.', 'Una ventana de Terminal muestra la instalación y al final te pide presionar una tecla.')}</li>
</ol>
<h2>{T('Where everything goes', 'Dónde queda todo')}</h2>
{table([T('Part', 'Parte'), T('Folder', 'Carpeta'), T('Used by', 'Lo usa')], [
 ['ANDROMEDA.vst3', '~/Library/Audio/Plug-Ins/VST3', T('Ableton Live, Bitwig, Reaper, Cubase, FL Studio…', 'Ableton Live, Bitwig, Reaper, Cubase, FL Studio…')],
 ['ANDROMEDA.component', '~/Library/Audio/Plug-Ins/Components', T('Logic Pro, GarageBand, Ableton (Audio Units)', 'Logic Pro, GarageBand, Ableton (Audio Units)')],
 ['ANDROMEDA.app', T('~/Applications (your Applications)', '~/Applications (tus Aplicaciones)'), T('You, as a normal app. Find it with ⌘ + Space → “ANDROMEDA”.', 'Tú, como app normal. Encuéntrala con ⌘ + Espacio → “ANDROMEDA”.')]])}
<div class="tip"><b>{T('The app', 'La app')}</b> {T('works exactly like the web version. The first time, click', 'funciona igual que la versión web. La primera vez, haz clic en')} <b>Options</b> {T('(top-left) → Audio/MIDI Settings to choose your sound card and your MIDI keyboard.', '(arriba a la izquierda) → Audio/MIDI Settings para elegir tu tarjeta de sonido y tu teclado MIDI.')}</div>
<p class="small">{T('To uninstall, delete those three items. Your songs (.andromeda files) are not touched.', 'Para desinstalar, borra esos tres elementos. Tus canciones (archivos .andromeda) no se tocan.')}</p>
''', T('Mac install', 'Instalar en Mac'))

    # ---------------------------------------------------------- 7 ableton
    page(f'''{kick(T('Install · 3 of 3', 'Instalar · 3 de 3'))}
<h1>{T('Open it in <i>Ableton Live</i>', 'Ábrelo en <i>Ableton Live</i>')}</h1>
<p class="lead">{T('You only do this once. After this, ANDROMEDA is in Ableton’s browser like any other instrument.', 'Esto se hace una sola vez. Después, ANDROMEDA está en el navegador de Ableton como cualquier otro instrumento.')}</p>
<ol class="steps">
<li>{T('Quit Ableton completely if it was open (⌘ + Q) and open it again, so it notices the new plug-in.', 'Cierra Ableton por completo si estaba abierto (⌘ + Q) y vuélvelo a abrir, para que note el plug-in nuevo.')}</li>
<li>{T('Open the settings: menu <b>Live → Settings…</b> (or', 'Abre los ajustes: menú <b>Live → Settings…</b> (o')} {kb('⌘ Cmd')} + {kb(',')}). {T('Click the <b>Plug-Ins</b> tab.', 'Haz clic en la pestaña <b>Plug-Ins</b>.')}</li>
<li>{T('Turn on <b>Use VST3 Plug-In System Folders</b> (and <b>Use Audio Units v2</b> if you like). Then press <b>Rescan</b>.', 'Activa <b>Use VST3 Plug-In System Folders</b> (y <b>Use Audio Units v2</b> si quieres). Luego presiona <b>Rescan</b>.')}</li>
<li>{T('Close the settings. In the browser on the left click <b>Plug-Ins</b> → <b>VST3</b> → <b>Polaris Sound Lab</b> → <b>ANDROMEDA</b>.', 'Cierra los ajustes. En el navegador de la izquierda haz clic en <b>Plug-Ins</b> → <b>VST3</b> → <b>Polaris Sound Lab</b> → <b>ANDROMEDA</b>.')}</li>
<li>{T('Drag ANDROMEDA onto an empty area of the session: Live creates a MIDI track with it. Its window opens; if not, click the small wrench icon on the device.', 'Arrastra ANDROMEDA a un área vacía de la sesión: Live crea una pista MIDI con él. Su ventana se abre; si no, haz clic en el pequeño ícono de llave del dispositivo.')}</li>
<li>{T('Press Play in Live (space bar). ANDROMEDA plays in Live’s tempo.', 'Presiona Play en Live (barra espaciadora). ANDROMEDA suena al tempo de Live.')}</li>
</ol>
<div class="win"><div class="wbar"><i class="dot-r"></i><i class="dot-y"></i><i class="dot-g"></i><span>Settings · Plug-Ins</span></div>
<div class="live"><div class="side"><div>Look/Feel</div><div>Display &amp; Input</div><div>Audio</div><div>Link, Tempo &amp; MIDI</div><div>File &amp; Folder</div><div>Library</div><div class="sel">Plug-Ins</div><div>Record, Warp &amp; Launch</div><div>Licenses &amp; Maintenance</div></div>
<div class="body"><b>Plug-In Sources</b>
<div class="row"><span>Use Audio Units v2</span><span class="tg on">On</span></div>
<div class="row"><span>Use VST2 Plug-In System Folders</span><span class="tg">Off</span></div>
<div class="row"><span>Use VST3 Plug-In System Folders</span><span class="tg on">On</span></div>
<div class="row"><span>Rescan Plug-Ins</span><span class="btn">Rescan</span></div>
<div class="hl">{T('← turn these on, then press Rescan', '← activa estos y presiona Rescan')}</div></div></div></div>
<div class="cap">{T('<b>fig. 2</b> — simplified drawing of Ableton Live 12’s settings. Ableton shows these options in English even if your Mac is in Spanish (unless you changed Live’s language).', '<b>fig. 2</b> — dibujo simplificado de los ajustes de Ableton Live 12. Si tu Ableton está en español, busca <b>Preferencias → Plug-Ins</b>, activa las carpetas de sistema VST3 y presiona “Volver a escanear”.')}</div>
<div class="tip"><b>{T('Other programs', 'Otros programas')}</b> {T('Logic Pro and GarageBand use the AU version: it appears under AU Instruments → Polaris Sound Lab → ANDROMEDA. Most other DAWs find the VST3 after a rescan.', 'Logic Pro y GarageBand usan la versión AU: aparece en Instrumentos AU → Polaris Sound Lab → ANDROMEDA. Casi todos los demás DAW encuentran el VST3 después de volver a escanear.')}</div>
''', 'Ableton')

    # ---------------------------------------------------------- 8 first song
    page(f'''{kick(T('Quick start', 'Inicio rápido'))}
<h1>{T('Your first song in <i>5 minutes</i>', 'Tu primera canción en <i>5 minutos</i>')}</h1>
<p class="lead">{T('These steps are the same in the web, the app and Ableton. Don’t worry about breaking anything: every button can be undone or tried again.', 'Estos pasos son iguales en la web, la app y Ableton. No te preocupes por romper algo: todo se puede deshacer o volver a intentar.')}</p>
<ol class="steps">
<li><b>{T('Choose a mood.', 'Elige un estado.')}</b> {T('In the', 'En la pestaña')} {k(T('✦ PLAY', '✦ TOCAR'), 'a')} {T('tab, click a name like', ', haz clic en un nombre como')} {k('DRIFT')} {T('(floating, no drums) or', '(flotante, sin batería) o')} {k('LULLABY')} {T('(nostalgic melody with soft beats). Under the names you can read what each one is.', '(melodía nostálgica con ritmos suaves). Debajo de los nombres puedes leer qué es cada uno.')}</li>
<li><b>{T('Write the song.', 'Escribe la canción.')}</b> {T('Press', 'Presiona')} {k(T('ALIGN SONG', 'ALINEAR CANCIÓN'))}. {T('ANDROMEDA writes six parts (scenes A–F): intro, build, peak, a break and an ending, with matching sounds.', 'ANDROMEDA escribe seis partes (escenas A–F): intro, subida, clímax, una pausa y un final, con sonidos que combinan.')}</li>
<li><b>{T('Let it play.', 'Déjala sonar.')}</b> {T('Press', 'Presiona')} {k('▶ PLAY')} {T('(in Ableton, press Live’s Play). Turn on', '(en Ableton, el Play de Live). Activa')} {k(T('SONG MODE', 'MODO CANCIÓN'), 'c')} {T('and it moves through the parts by itself.', 'y pasará por las partes sola.')}</li>
<li><b>{T('Make it yours.', 'Hazla tuya.')}</b> {T('Click a track at the bottom (for example', 'Haz clic en una pista abajo (por ejemplo')} {k('5 BELLS')}) {T('and click squares in the grid to add or remove notes. Every row is in key, so any square sounds good.', 'y haz clic en los cuadros de la cuadrícula para poner o quitar notas. Cada fila está en tono, así que cualquier cuadro suena bien.')}</li>
<li><b>{T('Turn on AUTO.', 'Activa AUTO.')}</b> {k('✦ AUTO', 'a')} {T('keeps developing your pattern: small changes every few bars, parts entering and leaving. Press', 'sigue desarrollando tu patrón: cambios pequeños cada pocos compases, partes que entran y salen. Presiona')} {k(T('BACK TO MY PATTERN', 'VOLVER A MI PATRÓN'))} {T('whenever you want your original back.', 'cuando quieras regresar a tu original.')}</li>
<li><b>{T('Play with the sound.', 'Juega con el sonido.')}</b> {T('Turn the four big knobs at the bottom:', 'Gira las cuatro perillas grandes de abajo:')} {k(T('DRIFT', 'DERIVA'))} {k(T('HAZE', 'BRUMA'))} {k(T('BLOOM', 'FLORECER'))} {k(T('GRAVITY', 'GRAVEDAD'))}. {T('Then try', 'Luego prueba')} {k(T('FREEZE', 'CONGELAR'))} {T('and', 'y')} {k('SUPERNOVA')}.</li>
<li><b>{T('Keep it.', 'Guárdala.')}</b> {k(T('SONG & FILES', 'CANCIÓN Y ARCHIVOS'))} → {k(T('SAVE', 'GUARDAR'))} {T('saves a .andromeda file. In Ableton, it is saved with your Live Set automatically.', 'guarda un archivo .andromeda. En Ableton se guarda solo dentro de tu proyecto de Live.')}</li>
</ol>
<div class="note"><b>{T('Everything is smooth.', 'Todo es suave.')}</b> {T('When you change a scene, a mood or an effect, ANDROMEDA waits for the right moment in the rhythm (the next beat or bar) and fades the change in. You can press buttons freely while the music plays.', 'Cuando cambias una escena, un estado o un efecto, ANDROMEDA espera el momento correcto del ritmo (el siguiente tiempo o compás) y hace el cambio con un fundido. Puedes presionar botones libremente mientras la música suena.')}</div>
''', T('First song', 'Primera canción'))

    # ---------------------------------------------------------- 9 screen map
    pins = [(42, 3.3), (63.7, 3.3), (88, 3.3), (20, 40), (20, 74), (20, 79.5), (55, 11), (70, 21), (70, 42), (70, 70), (20, 95), (49, 95), (63, 95), (84, 95)]
    lg = [T('Transport: play/stop, tempo (drag the number), TAP, key and scale.', 'Transporte: play/stop, tempo (arrastra el número), TAP, tono y escala.'),
          T('AUTO on/off (the music keeps writing itself).', 'AUTO encendido/apagado (la música se sigue escribiendo).'),
          T('Light engine, the full-screen Observatory and language EN/ES.', 'Motor ligero, el Observatorio a pantalla completa e idioma EN/ES.'),
          T('The vision: mathematical art that moves with the music. Shows scene, mood, bar and chord.', 'La visión: arte matemático que se mueve con la música. Muestra escena, estado, compás y acorde.'),
          T('Scenes A–H. Click one to go there at the next bar.', 'Escenas A–H. Haz clic en una para ir ahí en el siguiente compás.'),
          T('Choose the vision, when scenes switch (1, 2 or 4 bars) and SONG MODE.', 'Elige la visión, cuándo cambian las escenas (1, 2 o 4 compases) y el MODO CANCIÓN.'),
          T('The tabs: PLAY, SOUND, SPACE TAPE, FX & FILTERS, MIX, SONG & FILES, INPUT · MIDI, HELP.', 'Las pestañas: TOCAR, SONIDO, SPACE TAPE, EFECTOS Y FILTROS, MEZCLA, CANCIÓN Y ARCHIVOS, ENTRADA · MIDI, AYUDA.'),
          T('Moods and the buttons that write music for you.', 'Los estados y los botones que escriben música por ti.'),
          T('The AUTO card and how your keyboard is followed.', 'La tarjeta AUTO y cómo se sigue a tu teclado.'),
          T('The pattern of the selected track.', 'El patrón de la pista seleccionada.'),
          T('The 8 tracks: click to select, M to mute (with a fade).', 'Las 8 pistas: clic para seleccionar, M para silenciar (con fundido).'),
          T('The four macros: DRIFT, HAZE, BLOOM, GRAVITY.', 'Las cuatro macros: DERIVA, BRUMA, FLORECER, GRAVEDAD.'),
          T('The XY pad: drag to play the tape echo (or the active effect).', 'El pad XY: arrástralo para tocar el eco de cinta (o el efecto activo).'),
          T('Performance buttons for playing live.', 'Botones de performance para tocar en vivo.')]
    page(f'''{kick(T('The screen', 'La pantalla'))}
<h1>{T('Everything at a <i>glance</i>', 'Todo de un <i>vistazo</i>')}</h1>
{fig('main', T('<b>fig. 3</b> — the main screen (web, app and plug-in look the same).', '<b>fig. 3</b> — la pantalla principal (la web, la app y el plug-in se ven igual).'), pins)}
<div class="legend">{''.join(f'<div><span>{i+1}</span>{t}</div>' for i, t in enumerate(lg))}</div>
''', T('The screen', 'La pantalla'))

    # ---------------------------------------------------------- 10 moods
    MD = [('DRIFT', T('beatless, floating clouds', 'sin batería, nubes flotando'), '66–82', T('none', 'ninguna')),
          ('TIDAL', T('slow pulse, warm soft beat', 'pulso lento, ritmo cálido y suave'), '84–98', T('soft', 'suave')),
          ('HAZE', T('bright nostalgic arpeggios, breakbeat', 'arpegios brillantes y nostálgicos, breakbeat'), '100–118', 'break'),
          ('GLASS', T('music box arpeggios and frost', 'arpegios de caja musical y escarcha'), '90–112', T('ticks', 'tics')),
          ('PULSE', T('pulse-width dreams, rubber bass', 'sueños de pulse-width, bajo de goma'), '104–124', T('steady 4/4', '4/4 constante')),
          ('RUST', T('dark drones, clanks, machines', 'drones oscuros, golpes metálicos, máquinas'), '70–92', T('metallic', 'metálica')),
          ('BREATH', T('choirs and long sighs', 'coros y suspiros largos'), '60–76', T('none', 'ninguna')),
          ('LULLABY', T('nostalgic melody, lo-fi breaks', 'melodía nostálgica, breaks lo-fi'), '96–116', 'break'),
          ('ORBIT', T('polyrhythms, light braindance', 'polirritmos, braindance ligero'), '112–132', T('broken', 'quebrada')),
          ('ABYSS', T('deep, dark, very slow', 'profundo, oscuro y muy lento'), '54–70', T('slow pulse', 'pulso lento')),
          ('CHROME', T('cold metal, bowed and grey', 'metal frío, con arco y gris'), '64–84', T('metallic', 'metálica')),
          ('DAWN', T('bright, hopeful, sunrise', 'brillante, esperanzador, amanecer'), '88–104', T('soft', 'suave')),
          ('STATIC', T('radio ghosts and textures', 'fantasmas de radio y texturas'), '72–90', T('ticks', 'tics'))]
    page(f'''{kick(T('Moods', 'Estados'))}
<h1>{T('Thirteen <i>moods</i>', 'Trece <i>estados</i>')}</h1>
<p class="lead">{T('A mood is a starting point. It chooses the tempo, a scale that fits, the sounds of every track, the drum kit, a texture and the style of the rhythm. Nothing is locked: you can change any of it afterwards.', 'Un estado es un punto de partida. Elige el tempo, una escala que combine, los sonidos de cada pista, el kit de batería, una textura y el estilo del ritmo. Nada queda fijo: puedes cambiar cualquier cosa después.')}</p>
{table([T('Mood', 'Estado'), T('What it sounds like', 'Cómo suena'), 'BPM', T('Drums', 'Batería')], MD, 'tight')}
<h3>{T('The buttons next to the moods', 'Los botones junto a los estados')}</h3>
{table([T('Button', 'Botón'), T('What it does', 'Qué hace')], [
 [k(T('ALIGN SCENE', 'ALINEAR ESCENA'), 'a'), T('Writes every part of the current scene in the chosen mood.', 'Escribe todas las partes de la escena actual en el estado elegido.')],
 [k(T('ALIGN SONG', 'ALINEAR CANCIÓN')), T('Writes a whole song in scenes A–F (intro, build, peak, break, ending) and its song order.', 'Escribe una canción completa en las escenas A–F (intro, subida, clímax, pausa, final) y su orden.')],
 ['CHAOS', T('How adventurous the writing is. Low = calm and repetitive; high = more surprises.', 'Qué tan aventurera es la escritura. Bajo = tranquila y repetitiva; alto = más sorpresas.')],
 [k(T('KEEP SOUNDS', 'MANTENER SONIDOS'), 'c') + ' ' + k(T('KEEP TEMPO', 'MANTENER TEMPO'), 'c') + ' ' + k(T('KEEP KEY', 'MANTENER TONO'), 'c'), T('Turn these on before ALIGN to keep your sounds, tempo or key while the notes are rewritten.', 'Actívalos antes de ALINEAR para conservar tus sonidos, tempo o tono mientras se reescriben las notas.')]])}
<div class="tip"><b>{T('Shortcut.', 'Atajo.')}</b> {T('Double-click a mood name to align the scene in that mood straight away.', 'Doble clic en el nombre de un estado para alinear la escena en ese estado de inmediato.')}</div>
''', T('Moods', 'Estados'))

    # ---------------------------------------------------------- 11 tracks
    TR = [('1 DRUMS', T('Drum machine with 11 voices (kick, snare, hats, clap, rim, tom, clank, shaker, blip, cowbell). Each row of its grid is one voice.', 'Caja de ritmos con 11 voces (bombo, tarola, hats, clap, aro, tom, golpe metálico, shaker, blip, cencerro). Cada fila de su cuadrícula es una voz.')),
          ('2 BASS', T('Low notes that hold everything together.', 'Notas graves que sostienen todo.')),
          ('3 PAD', T('Long, breathing chords. The heart of ambient.', 'Acordes largos que respiran. El corazón del ambient.')),
          ('4 VOICE', T('Choirs, vowels and sighs.', 'Coros, vocales y suspiros.')),
          ('5 BELLS', T('Music boxes, bells, pianos, marimbas.', 'Cajas de música, campanas, pianos, marimbas.')),
          ('6 LEAD', T('A melody that sings above.', 'Una melodía que canta arriba.')),
          ('7 TEXTURE', T('Wind, rain, vinyl, tape hiss, ocean, radio or a deep hum. Its grid adds swells.', 'Viento, lluvia, vinilo, siseo de cinta, océano, radio o un zumbido grave. Su cuadrícula agrega oleadas.')),
          ('8 INPUT', T('Your own audio (microphone, a file or, in Ableton, another track) passing through ANDROMEDA. Its grid cuts it in rhythm.', 'Tu propio audio (micrófono, un archivo o, en Ableton, otra pista) pasando por ANDROMEDA. Su cuadrícula lo corta en ritmo.'))]
    page(f'''{kick(T('Tracks', 'Pistas'))}
<h1>{T('Eight <i>tracks</i>', 'Ocho <i>pistas</i>')}</h1>
<p class="lead">{T('The tracks are the boxes at the bottom of the screen. Click one to select it: the grid, the SOUND tab and your keyboard all work on the selected track.', 'Las pistas son las cajitas de abajo de la pantalla. Haz clic en una para seleccionarla: la cuadrícula, la pestaña SONIDO y tu teclado trabajan sobre la pista seleccionada.')}</p>
{fig('footer', T('<b>fig. 4</b> — the bottom bar: tracks, macros, XY pad and performance buttons.', '<b>fig. 4</b> — la barra de abajo: pistas, macros, pad XY y botones de performance.'))}
{table([T('Track', 'Pista'), T('What it plays', 'Qué toca')], TR)}
<div class="grid2"><div class="card"><h3>{T('Mute', 'Silenciar')}</h3><p class="small">{T('The', 'El botón')} {k('M', 'rose')} {T('button silences a track with a half-beat fade, so it never clicks. Press again to bring it back.', 'silencia una pista con un fundido de medio tiempo, así nunca truena. Presiónalo otra vez para regresarla.')}</p></div>
<div class="card"><h3>{T('The light on each track', 'La luz de cada pista')}</h3><p class="small">{T('Glows every time the track plays a note: a quick way to see who is playing.', 'Brilla cada vez que la pista toca una nota: una forma rápida de ver quién está sonando.')}</p></div></div>
''', T('Tracks', 'Pistas'))

    # ---------------------------------------------------------- 12 play grid
    page(f'''{kick(T('✦ Play · the pattern', '✦ Tocar · el patrón'))}
<h1>{T('Draw your <i>pattern</i>', 'Dibuja tu <i>patrón</i>')}</h1>
<div class="two" style="font-size:9.6pt"><div>
<p>{T('The grid shows time from left to right (each column is a step) and pitch from bottom to top. The rows only contain notes of the current scale, labelled with their names, so <b>every square is in key</b>.', 'La cuadrícula muestra el tiempo de izquierda a derecha (cada columna es un paso) y la altura de abajo hacia arriba. Las filas solo tienen notas de la escala actual, con su nombre, así que <b>cada cuadro está en tono</b>.')}</p>
<ul>
<li><b>{T('Click', 'Clic')}</b> {T('an empty square: adds a note (you hear it).', 'en un cuadro vacío: agrega una nota (la escuchas).')}</li>
<li><b>{T('Click it again', 'Clic otra vez')}</b>: {T('removes it.', 'la quita.')}</li>
<li><b>{kb('Shift')} + {T('click', 'clic')}</b>: {T('makes the note longer each time.', 'hace la nota más larga cada vez.')}</li>
<li>{T('The white column is the playhead.', 'La columna blanca es donde va sonando.')}</li>
<li>{T('Dim squares are beyond the pattern length.', 'Los cuadros oscuros están fuera del largo del patrón.')}</li>
</ul>
<h3>{T('Above the grid', 'Arriba de la cuadrícula')}</h3>
{table([T('Control', 'Control'), T('What it does', 'Qué hace')], [
 [T('RATE', 'VELOCIDAD'), T('How long each step lasts: 1/16 is normal, 1/8 is half as fast, 1/1 is one bar per step (for slow pads).', 'Cuánto dura cada paso: 1/16 es lo normal, 1/8 la mitad de rápido, 1/1 un compás por paso (para pads lentos).')],
 [T('LENGTH', 'LARGO'), T('How many steps before it repeats (1–32). Different lengths on different tracks create slowly shifting patterns.', 'Cuántos pasos antes de repetir (1–32). Largos distintos en cada pista crean patrones que cambian poco a poco.')],
 ['FWD · REV · PING', T('Play forwards, backwards or back and forth.', 'Toca hacia adelante, hacia atrás o de ida y vuelta.')],
 ['OCT', T('Moves the whole track up or down an octave.', 'Sube o baja toda la pista una octava.')],
 [T('FOLLOW CHORDS', 'SEGUIR ACORDES'), T('The notes move with the song’s chords (recommended).', 'Las notas se mueven con los acordes de la canción (recomendado).')],
 [T('CHORDS', 'ACORDES'), T('Each note becomes a full three-note chord. Rows then show chord names.', 'Cada nota se vuelve un acorde completo de tres notas. Las filas muestran nombres de acordes.')],
 [k(T('✦ NEW', '✦ NUEVO'), 'a') + ' ' + k(T('≈ VARY', '≈ VARIAR')) + ' ' + k(T('CLEAR', 'BORRAR')), T('Write a new pattern for this track · change it a little · empty it.', 'Escribe un patrón nuevo para esta pista · cámbialo un poco · vacíalo.')]], 'tight')}
</div><div>{fig('play', T('<b>fig. 5</b> — the PLAY tab with the BELLS pattern.', '<b>fig. 5</b> — la pestaña TOCAR con el patrón de BELLS.'))}
<h3>{T('The selected note', 'La nota seleccionada')}</h3>
<p class="small">{T('After you click a note, a small editor appears under the grid:', 'Al hacer clic en una nota aparece un pequeño editor bajo la cuadrícula:')}</p>
{table(['', ''], [[T('GATE', 'DURACIÓN'), T('note length', 'largo de la nota')], [T('VELOCITY', 'FUERZA'), T('how strong it sounds', 'qué tan fuerte suena')], [T('CHANCE', 'PROBABILIDAD'), T('how often it plays: 50% = half of the times. Great for patterns that never repeat exactly.', 'qué tan seguido suena: 50% = la mitad de las veces. Ideal para patrones que nunca se repiten igual.')], [T('OCTAVE', 'OCTAVA'), T('this note only, up or down', 'solo esta nota, arriba o abajo')], [T('RATCHET', 'REPIQUE'), T('(drums) repeats the hit 2–4 times inside the step', '(batería) repite el golpe 2–4 veces dentro del paso')]], 'tight')}
<p class="small">{T('<b>Drums:</b> each row is a voice; the menu on the left of each row changes the voice. Shift + click a hit to edit it. <b>Texture and Input</b> have a single row: on = swell / sound passes.', '<b>Batería:</b> cada fila es una voz; el menú a la izquierda de cada fila cambia la voz. Shift + clic en un golpe para editarlo. <b>Textura y Entrada</b> tienen una sola fila: encendido = oleada / pasa el sonido.')}</p>
</div></div>
''', T('The pattern', 'El patrón'))

    # ---------------------------------------------------------- 13 auto + keys
    page(f'''{kick(T('✦ Play · AUTO and your keyboard', '✦ Tocar · AUTO y tu teclado'))}
<h1>{T('Music that <i>keeps going</i>', 'Música que <i>sigue sola</i>')}</h1>
{fig('auto', '')}
<div class="grid2"><div>
<h2>✦ AUTO</h2>
<p>{T('With AUTO on, ANDROMEDA uses <b>your pattern as a seed</b>. Every 2 to 4 bars it changes one part a little, using your own intervals and rhythms, so it still sounds like you. Notes on strong beats stay on the chord, so it is always in key and in time. Parts enter and leave with fades.', 'Con AUTO encendido, ANDROMEDA usa <b>tu patrón como semilla</b>. Cada 2 a 4 compases cambia un poco una parte, usando tus propios intervalos y ritmos, así que sigue sonando a ti. Las notas en los tiempos fuertes se quedan en el acorde: siempre en tono y en tiempo. Las partes entran y salen con fundidos.')}</p>
{table(['', ''], [[T('AMOUNT', 'CANTIDAD'), T('how much it changes. 20% = gentle, 80% = it travels far from your pattern.', 'cuánto cambia. 20% = suave, 80% = se aleja mucho de tu patrón.')], [T('ENERGY', 'ENERGÍA'), T('how many parts play at once. Low = sparse and calm, high = full.', 'cuántas partes suenan a la vez. Baja = escaso y tranquilo, alta = lleno.')], [T('BACK TO MY PATTERN', 'VOLVER A MI PATRÓN'), T('returns all tracks of this scene to what you wrote.', 'regresa todas las pistas de esta escena a lo que escribiste.')]], 'tight')}
<p class="small">{T('Leave AUTO on for an hour and it never plays the same thing twice: perfect for installations, studying or sleeping.', 'Déjalo encendido una hora y nunca toca lo mismo dos veces: perfecto para instalaciones, estudiar o dormir.')}</p>
</div><div>
<h2>{T('KEYS · how your keyboard is followed', 'TECLAS · cómo se sigue a tu teclado')}</h2>
<p>{T('Play the selected track with a MIDI keyboard or with your computer keys. Choose how ANDROMEDA treats the notes you play:', 'Toca la pista seleccionada con un teclado MIDI o con las teclas de tu computadora. Elige cómo trata ANDROMEDA las notas que tocas:')}</p>
{table(['', ''], [[T('RAW', 'LIBRE'), T('your notes exactly as you play them.', 'tus notas tal como las tocas.')], [T('SNAP', 'AJUSTAR'), T('every note moves to the nearest note of the scale. You can’t play a wrong note.', 'cada nota se mueve a la nota más cercana de la escala. No puedes tocar una nota equivocada.')], [T('SHIFT', 'MODULAR'), T('<b>the magic one.</b> If you play a note outside the scale, ANDROMEDA finds the closest scale that contains it and moves the whole song there on the next beat. The pads and bass glide to the new chord. You hear your intervention, and the music keeps developing.', '<b>la mágica.</b> Si tocas una nota fuera de la escala, ANDROMEDA busca la escala más cercana que la contiene y lleva toda la canción ahí en el siguiente tiempo. Los pads y el bajo se deslizan al acorde nuevo. Se nota tu intervención y la música sigue desarrollándose.')]], 'tight')}
<div class="tip"><b>{T('Call and response.', 'Pregunta y respuesta.')}</b> {T('With AUTO on, the melody you play becomes a motif: a few bars later the bells or the lead answer it.', 'Con AUTO encendido, la melodía que tocas se vuelve un motivo: unos compases después las campanas o el lead la contestan.')}</div>
<p class="small">{T('Example: the song is in C major and you play an F#. ANDROMEDA moves to G major (only one note changes), shows “shifts to G MAJOR” and the sky flashes.', 'Ejemplo: la canción está en Do mayor y tocas un Fa#. ANDROMEDA pasa a Sol mayor (solo cambia una nota), muestra “modula a G MAJOR” y el cielo destella.')}</p>
</div></div>
''', 'AUTO')

    # ---------------------------------------------------------- 14 sound
    page(f'''{kick(T('Sound', 'Sonido'))}
<h1>{T('Choose and shape the <i>sound</i>', 'Elige y moldea el <i>sonido</i>')}</h1>
<div class="two"><div>
<p>{T('The SOUND tab changes the instrument of the selected track. Pick a category, then a preset; it plays a short preview. Press', 'La pestaña SONIDO cambia el instrumento de la pista seleccionada. Elige una categoría y luego un preset; suena una pequeña muestra. Presiona')} {k(T('▶ LISTEN', '▶ ESCUCHAR'), 'a')} {T('anytime to hear it again.', 'cuando quieras para escucharlo otra vez.')}</p>
{table([T('Category', 'Categoría'), T('53 presets', '53 presets')], [
 ['PAD', 'Polar Dawn, Gossamer Saw, Tidal Glass, Velvet Orbit, Pulse Width Dream, Analog Strings, Cold Plateau, Amber Fog, Low Sun, Saturn Hum, Weightless, Paper Moon, Static Bloom, Distant Horn'],
 ['CHOIR', 'Breath of Io, Choir of Tides, Hollow Choir, Ghost Vowel, Cathedral Air, Cathedral Drone, Whisper Field'],
 ['BELL', 'Glass Bell, Music Box, Heliotrope, Satellite, Ice Marimba, Tin Gamelan, Tape Piano, Electric Haze, Prepared Piano, Clockwork, Singing Bowl, Comet Pluck, Iron Bell'],
 ['METAL', 'Rusted Bowl, Hollow Metal, Grey Water, Glass Harmonica'],
 ['BASS', 'Deep Field, Rubber Ball, Rubber Moon, Sub Gravity, Acid Dust, Warm Pulse'],
 ['LEAD', 'Lonely Probe, Ice Bubble, Heliosphere Arp, Pulsewidth, Wobble Lead, Sine Flute, Thin Signal, Dream Saw, Square Kid']], 'tight')}
<h3>{T('The eight essential knobs', 'Las ocho perillas esenciales')}</h3>
{table(['', ''], [['CUTOFF', T('brightness: low = dark and soft, high = bright', 'brillo: bajo = oscuro y suave, alto = brillante')], ['RESO', T('a singing peak at the cutoff', 'un pico que “canta” en el brillo')], ['ATTACK', T('how slowly each note fades in', 'qué tan lento entra cada nota')], ['RELEASE', T('how long it rings after the note ends', 'cuánto sigue sonando cuando termina la nota')], ['WARBLE', T('old-tape wobble in the pitch', 'ondulación de cinta vieja en la afinación')], ['ENSEMBLE', T('wide, shimmering chorus', 'chorus ancho y brillante')], ['DRIVE', T('warm saturation', 'saturación cálida')], ['LEVEL', T('volume of this sound', 'volumen de este sonido')]], 'tight')}
</div><div>{fig('snd', T('<b>fig. 6</b> — SOUND, with MORE opened.', '<b>fig. 6</b> — SONIDO, con MÁS abierto.'))}
<p class="small">{k(T('MORE', 'MÁS'))} {T('opens the deeper controls: oscillator waves, detune and sub (pads and leads), FM ratio, brightness and shimmer (bells), bow and metal (bowed sounds), vowels (choirs), the filter envelope, decay/sustain, vibrato, glide and stereo width.', 'abre los controles profundos: ondas del oscilador, desafinación y sub (pads y leads), ratio FM, brillo y destello (campanas), arco y metal (sonidos con arco), vocales (coros), la envolvente del filtro, decay/sustain, vibrato, glide y ancho estéreo.')}</p>
<h3>{T('Drums and textures', 'Batería y texturas')}</h3>
<p class="small">{T('With track 1 selected, SOUND shows the 7 drum kits: <b>DUST</b> lo-fi and crunchy · <b>VELVET</b> round and gentle · <b>RUST</b> dark and metallic · <b>TIN</b> tiny vintage box · <b>STEEL</b> tight, for broken beats · <b>BOOM</b> long analog boom · <b>GRIT</b> punchy and dirty. With track 7: the 7 textures with LEVEL, TONE and MOTION.', 'Con la pista 1 seleccionada, SONIDO muestra los 7 kits: <b>DUST</b> lo-fi y crujiente · <b>VELVET</b> redondo y suave · <b>RUST</b> oscuro y metálico · <b>TIN</b> cajita vintage · <b>STEEL</b> cerrado, para ritmos quebrados · <b>BOOM</b> bombo análogo largo · <b>GRIT</b> golpeado y sucio. Con la pista 7: las 7 texturas con LEVEL, TONE y MOTION.')}</p>
</div></div>
''', T('Sound', 'Sonido'))

    # ---------------------------------------------------------- 15 space tape 1
    MO = [('1', T('head 1', 'cabeza 1'), T('short slapback', 'eco corto')), ('2', T('head 2', 'cabeza 2'), T('medium echo', 'eco medio')), ('3', T('head 3', 'cabeza 3'), T('long echo', 'eco largo')), ('4', '2 + 3', T('two echoes, a rhythm', 'dos ecos, un ritmo')),
          ('5', '1 + ' + T('spring', 'resorte'), T('short echo in a room', 'eco corto en un cuarto')), ('6', '2 + ' + T('spring', 'resorte'), ''), ('7', '3 + ' + T('spring', 'resorte'), ''), ('8', '1 + 2 + ' + T('spring', 'resorte'), T('galloping', 'galope')),
          ('9', '2 + 3 + ' + T('spring', 'resorte'), T('the classic dub setting', 'el clásico para dub')), ('10', '1 + 3 + ' + T('spring', 'resorte'), ''), ('11', '1 + 2 + 3 + ' + T('spring', 'resorte'), T('the biggest, a cloud', 'el más grande, una nube')), ('12', T('spring only', 'solo resorte'), T('reverb, no echo', 'reverb, sin eco'))]
    page(f'''{kick('Space Tape')}
<h1>{T('A tape echo you can <i>see</i>', 'Un eco de cinta que puedes <i>ver</i>')}</h1>
<p class="lead">{T('Space Tape recreates the classic three-head tape echo of the 70s: a loop of tape passes a recording head and three playback heads. Each head reads the sound a little later, so you hear echoes. The tape is old and warm: it wobbles, darkens and gets saturated, and it has a spring reverb inside.', 'Space Tape recrea el clásico eco de cinta de tres cabezas de los 70: un ciclo de cinta pasa por una cabeza de grabación y tres de reproducción. Cada cabeza lee el sonido un poco después, así escuchas ecos. La cinta es vieja y cálida: ondula, se oscurece y se satura, y adentro tiene una reverb de resorte.')}</p>
{fig('tape', T('<b>fig. 7</b> — the SPACE TAPE tab. The lit heads (PB 1–3) are the ones playing; the spring and the VU meter move with the sound.', '<b>fig. 7</b> — la pestaña SPACE TAPE. Las cabezas iluminadas (PB 1–3) son las que suenan; el resorte y el medidor VU se mueven con el sonido.'), style='width:58%;margin-left:auto;margin-right:auto')}
<h3>{T('The 12-position mode selector', 'El selector de 12 modos')}</h3>
<div class="grid2" style="gap:.18in">{table([T('Mode', 'Modo'), T('Heads', 'Cabezas'), T('Feels like', 'Se siente como')], MO[:6], 'tight')}{table([T('Mode', 'Modo'), T('Heads', 'Cabezas'), T('Feels like', 'Se siente como')], MO[6:], 'tight')}</div>
''', 'Space Tape')

    # ---------------------------------------------------------- 16 space tape 2
    page(f'''{kick(T('Space Tape · controls', 'Space Tape · controles'))}
<h1>{T('Every <i>knob</i>, explained', 'Cada <i>perilla</i>, explicada')}</h1>
{table([T('Control', 'Control'), T('What it does', 'Qué hace')], [
 ['INPUT VOLUME', T('How hard the sound hits the tape. The little light next to it turns red when the tape saturates: a bit of red is the warm sound you want.', 'Qué tan fuerte llega el sonido a la cinta. La lucecita junto a ella se pone roja cuando la cinta se satura: un poco de rojo es el sonido cálido que buscas.')],
 ['REPEAT RATE', T('Tape speed = time between echoes. Turn it while the music plays and you hear the motor speed up and slow down, bending the echoes.', 'Velocidad de la cinta = tiempo entre ecos. Gírala mientras suena y escucharás el motor acelerar y frenar, doblando los ecos.')],
 ['INTENSITY', T('How many times the echoes repeat. Past 3 o’clock the tape starts to sing by itself (self-oscillation); it never gets out of control.', 'Cuántas veces se repiten los ecos. Más allá de las 3 la cinta empieza a cantar sola (auto-oscilación); nunca se sale de control.')],
 ['ECHO VOLUME', T('Volume of the echoes.', 'Volumen de los ecos.')],
 ['REVERB VOLUME', T('Volume of the spring reverb.', 'Volumen de la reverb de resorte.')],
 ['BASS · TREBLE', T('Tone of the echoes (±12 dB). Lower the treble for darker, older echoes.', 'Tono de los ecos (±12 dB). Baja los agudos para ecos más oscuros y viejos.')],
 ['WOW · FLUTTER', T('Slow and fast wobble of the tape.', 'Ondulación lenta y rápida de la cinta.')],
 ['TAPE AGE', T('How worn the tape is: duller, little dropouts and hiss.', 'Qué tan gastada está la cinta: más opaca, pequeñas caídas y siseo.')],
 [k('ECHO ON', 'c'), T('Turns the whole echo on or off (the echoes already on the tape fade out).', 'Enciende o apaga todo el eco (los ecos que ya están en la cinta se desvanecen).')],
 [k('SYNC', 'c'), T('Locks the echo time to the tempo (1/32 to 1/4 dotted). Choose the division in the menu that appears.', 'Amarra el tiempo del eco al tempo (de 1/32 a 1/4 con punto). Elige la división en el menú que aparece.')],
 [k(T('❄ FREEZE', '❄ CONGELAR'), 'c'), T('The tape stops recording and loops what it has forever. Play on top of it. Press again to release.', 'La cinta deja de grabar y repite para siempre lo que tiene. Toca encima. Presiona otra vez para soltar.')],
 [k(T('↻ SPIN', '↻ GIRAR')) + ' ' + k('↺'), T('Spins the tape fast forwards or backwards for a moment: a classic swoosh.', 'Gira la cinta rápido hacia adelante o hacia atrás un momento: el clásico zumbido.')],
 [k(T('⚡ SPRING KICK', '⚡ GOLPE AL RESORTE'), 'rose'), T('Kicks the reverb tank: the famous metallic crash.', 'Golpea el tanque de reverb: el famoso estruendo metálico.')]])}
<h2>{T('Cosmos reverb', 'Reverb Cosmos')}</h2>
<p>{T('After the tape comes Cosmos, a huge, soft reverb for the long ambient tails. <b>SIZE</b> (1–13 seconds), <b>DAMP</b> (darker tails), <b>PRE-DELAY</b> (a small gap before the reverb, for clarity) and <b>VOLUME</b>. How much of each track goes to the tape and to Cosmos is set in MIX.', 'Después de la cinta viene Cosmos, una reverb enorme y suave para las colas largas del ambient. <b>SIZE</b> (1–13 segundos), <b>DAMP</b> (colas más oscuras), <b>PRE-DELAY</b> (un pequeño espacio antes de la reverb, para claridad) y <b>VOLUME</b>. Cuánto de cada pista va a la cinta y a Cosmos se ajusta en MEZCLA.')}</p>
<div class="tip"><b>{T('Play the tape with your finger.', 'Toca la cinta con el dedo.')}</b> {T('When no effect is on, the XY pad at the bottom plays the tape: left–right is the speed, up–down is the intensity. Let go and it returns to where it was.', 'Cuando no hay un efecto encendido, el pad XY de abajo toca la cinta: izquierda–derecha es la velocidad, arriba–abajo la intensidad. Suéltalo y regresa a donde estaba.')}</div>
''', 'Space Tape')

    # ---------------------------------------------------------- 17 fx
    FX = [('LADDER', T('the classic warm 24 dB low-pass filter', 'el clásico filtro pasabajos cálido de 24 dB'), T('cutoff', 'corte'), T('resonance', 'resonancia')),
          ('ACID', T('resonant, squelchy filter that reacts to the sound', 'filtro resonante y chillón que reacciona al sonido'), T('cutoff', 'corte'), T('squelch', 'chillido')),
          ('MS', T('aggressive, raspy filter with drive', 'filtro agresivo y rasposo con saturación'), T('cutoff', 'corte'), T('resonance', 'resonancia')),
          ('HIGH SWEEP', T('DJ high-pass: removes the bass for a build-up', 'pasaaltos de DJ: quita los graves para una subida'), T('cutoff', 'corte'), T('resonance', 'resonancia')),
          ('BAND', T('band-pass: only a slice of the sound, like a radio', 'pasabanda: solo una franja del sonido, como un radio'), T('frequency', 'frecuencia'), T('narrowness', 'estrechez')),
          ('VOWEL', T('makes the music “talk”: A-E-I-O-U', 'hace “hablar” a la música: A-E-I-O-U'), T('vowel', 'vocal'), T('resonance', 'resonancia')),
          ('ENVELOPE', T('auto-wah that opens with every hit', 'auto-wah que se abre con cada golpe'), T('sensitivity', 'sensibilidad'), T('resonance', 'resonancia')),
          ('NOTCH', T('a hole that sweeps through the sound', 'un hueco que barre el sonido'), T('position', 'posición'), T('sweep speed', 'velocidad')),
          ('PHASER', T('swirling, liquid phase', 'fase giratoria y líquida'), T('speed', 'velocidad'), T('depth', 'profundidad')),
          ('JET', T('jet-plane flanger', 'flanger de avión'), T('speed', 'velocidad'), T('intensity', 'intensidad')),
          ('ENSEMBLE', T('wide string-machine chorus', 'chorus ancho de máquina de cuerdas'), T('speed', 'velocidad'), T('depth', 'profundidad')),
          ('REPEAT', T('grabs a slice of the beat and repeats it (stutter)', 'toma un trozo del ritmo y lo repite (tartamudeo)'), T('slice length', 'largo del trozo'), T('fade of repeats', 'desvanecer')),
          ('DUST', T('bit-crusher: old sampler grit', 'reductor de bits: aspereza de sampler viejo'), T('bits', 'bits'), T('sample rate', 'muestreo')),
          ('HALF-SPEED', T('the music slows down like a tape running out', 'la música se frena como una cinta que se acaba'), T('how slow', 'qué tan lento'), '—'),
          ('GATE', T('rhythmic chopping on 1/16 notes', 'corte rítmico en 1/16'), T('pattern', 'patrón'), T('depth', 'profundidad')),
          ('METAL', T('ring modulation: bells and robots', 'modulación en anillo: campanas y robots'), T('frequency', 'frecuencia'), T('amount', 'cantidad'))]
    page(f'''{kick(T('FX & Filters', 'Efectos y filtros'))}
<h1>{T('Sixteen <i>live</i> effects', 'Dieciséis efectos <i>en vivo</i>')}</h1>
<div class="two"><div>
<p>{T('These effects change everything that is playing, in real time. Click an effect to turn it on; click it again to turn it off. Click another one to switch.', 'Estos efectos cambian todo lo que está sonando, en tiempo real. Haz clic en un efecto para encenderlo; otra vez para apagarlo. Clic en otro para cambiar.')}</p>
<p>{T('<b>They never cut the music:</b> each one enters on the next 1/16 note (REPEAT on the next 1/8) and leaves with a short fade.', '<b>Nunca cortan la música:</b> cada uno entra en el siguiente 1/16 (REPEAT en el siguiente 1/8) y sale con un fundido corto.')}</p>
<ul><li>{T('Drag inside the square to play <b>X</b> (left–right) and <b>Y</b> (down–up).', 'Arrastra dentro del cuadro para tocar <b>X</b> (izquierda–derecha) y <b>Y</b> (abajo–arriba).')}</li>
<li>{k('FX MIX')}: {T('how much of the effect you hear.', 'cuánto efecto escuchas.')}</li>
<li>{k('LATCH')} / {k('HOLD', 'c')}: {T('LATCH stays on until you click again; HOLD only sounds while you press.', 'LATCH se queda encendido hasta que vuelvas a hacer clic; HOLD solo suena mientras presionas.')}</li></ul>
</div><div>{fig('fx', T('<b>fig. 8</b> — ACID on, with its XY square.', '<b>fig. 8</b> — ACID encendido, con su cuadro XY.'))}</div></div>
{table([T('Effect', 'Efecto'), T('What it does', 'Qué hace'), 'X', 'Y'], FX, 'tight')}
''', T('FX & Filters', 'Efectos y filtros'))

    # ---------------------------------------------------------- 18 performance
    PF = [(T('FREEZE', 'CONGELAR'), 'O', T('Loops what is on the tape forever (toggle).', 'Repite para siempre lo que hay en la cinta (encender/apagar).')),
          ('TAPE STOP', 'P', T('Hold it: the whole song slows down to a stop like a turntable. Let go and it speeds back up.', 'Mantenlo: toda la canción se frena hasta detenerse como un tocadiscos. Suéltalo y vuelve a acelerar.')),
          (T('SPIN', 'GIRAR'), '[', T('A fast tape spin.', 'Un giro rápido de la cinta.')),
          (T('FILL', 'REDOBLE'), ']', T('A drum fill until the end of the bar.', 'Un redoble de batería hasta el final del compás.')),
          ('SUPERNOVA', '\\', T('A two-bar rising sweep that explodes in a deep boom on the next downbeat. Use it before changing scene.', 'Una subida de dos compases que explota en un golpe grave en el siguiente primer tiempo. Úsalo antes de cambiar de escena.')),
          ('ECLIPSE', "'", T('Hold it: drums, bass and lead disappear into the echoes and the reverb. Let go and the beat returns.', 'Mantenlo: batería, bajo y lead desaparecen en los ecos y la reverb. Suéltalo y regresa el ritmo.')),
          (T('SPRING KICK', 'GOLPE AL RESORTE'), 'K', T('Kicks the spring reverb.', 'Golpea la reverb de resorte.')),
          ('AUTO', 'A', T('AUTO on/off.', 'AUTO encendido/apagado.'))]
    MC = [(T('DRIFT', 'DERIVA'), T('How much the tape wobbles and the pitch floats. Up = seasick, nostalgic.', 'Cuánto ondula la cinta y flota la afinación. Arriba = mareado, nostálgico.')),
          (T('HAZE', 'BRUMA'), T('A warm fog over everything: softer highs and saturation; past half it gets crunchy like an old sampler.', 'Una niebla cálida sobre todo: agudos más suaves y saturación; pasando la mitad se vuelve crujiente como un sampler viejo.')),
          (T('BLOOM', 'FLORECER'), T('How much of everything goes into the echo and reverb. Up = huge space.', 'Cuánto de todo va al eco y la reverb. Arriba = espacio enorme.')),
          (T('GRAVITY', 'GRAVEDAD'), T('One knob, two filters: left removes the highs (underwater), right removes the lows (thin, far away). Center = off.', 'Una perilla, dos filtros: a la izquierda quita agudos (bajo el agua), a la derecha quita graves (delgado, lejano). Al centro = apagado.'))]
    page(f'''{kick(T('Performance', 'Performance'))}
<h1>{T('Playing it <i>live</i>', 'Tocarlo <i>en vivo</i>')}</h1>
<h3>{T('The four macros', 'Las cuatro macros')}</h3>
{table(['', ''], MC)}
<h3>{T('The performance buttons (and their computer keys)', 'Los botones de performance (y su tecla en la computadora)')}</h3>
{table([T('Button', 'Botón'), T('Key', 'Tecla'), T('What it does', 'Qué hace')], [[a, kb(b), c] for a, b, c in PF], 'tight')}
<div class="grid2"><div class="card"><h3>{T('XY pad', 'Pad XY')}</h3><p class="small">{T('Bottom bar. With an effect on, it plays the effect’s X and Y. With no effect, it plays the tape (speed and intensity) and springs back when you let go.', 'Barra de abajo. Con un efecto encendido, toca la X y la Y del efecto. Sin efecto, toca la cinta (velocidad e intensidad) y regresa al soltar.')}</p></div>
<div class="card"><h3>{T('Observatory', 'Observatorio')}</h3><p class="small">{T('The ◎ OBSERVATORY button (or the', 'El botón ◎ OBSERVATORIO (o la tecla')} {kb('`')} {T('key) shows the vision full-screen, for projections and live sets. Click ✕ or press the key again to come back.', ') muestra la visión a pantalla completa, para proyecciones y presentaciones. Haz clic en ✕ o presiona la tecla otra vez para regresar.')}</p></div></div>
''', T('Performance', 'Performance'))

    # ---------------------------------------------------------- 19 visions
    page(f'''{kick(T('Visions', 'Visiones'))}
<h1>{T('Mathematics that <i>listens</i>', 'Matemáticas que <i>escuchan</i>')}</h1>
<p class="lead">{T('The large square on the left is not decoration: every vision is drawn from the music in real time. Choose one under the scenes, or leave AUTO and the mood chooses.', 'El cuadro grande de la izquierda no es decoración: cada visión se dibuja con la música en tiempo real. Elige una debajo de las escenas, o deja AUTO y el estado elige.')}</p>
<div class="grid2" style="width:86%;margin:0 auto;gap:.18in">
<div>{fig('vis_CYMATICS', T('<b>CYMATICS</b> — sand on a vibrating plate (Chladni figures). Each chord draws a different shape.', '<b>CYMATICS</b> — arena sobre una placa que vibra (figuras de Chladni). Cada acorde dibuja una forma distinta.'))}</div>
<div>{fig('vis_HARMONOGRAPH', T('<b>HARMONOGRAPH</b> — pendulums drawing the ratios between the notes of the chord.', '<b>HARMONOGRAPH</b> — péndulos que dibujan las proporciones entre las notas del acorde.'))}</div>
<div>{fig('vis_ATTRACTOR', T('<b>ATTRACTOR</b> — a chaotic map that slowly changes shape with the sound.', '<b>ATTRACTOR</b> — un mapa caótico que cambia de forma lentamente con el sonido.'))}</div>
<div>{fig('vis_SCOPE', T('<b>SCOPE</b> — the real sound, left against right. <b>ORRERY</b> (not shown) — a sky chart where each track is a planet.', '<b>SCOPE</b> — el sonido real, izquierdo contra derecho. <b>ORRERY</b> (no se muestra) — una carta del cielo donde cada pista es un planeta.'))}</div>
</div>
''', T('Visions', 'Visiones'))

    # ---------------------------------------------------------- 20 scenes & song
    page(f'''{kick(T('Scenes and song', 'Escenas y canción'))}
<h1>{T('From a loop to a <i>song</i>', 'De un loop a una <i>canción</i>')}</h1>
<div class="two"><div>
<h2>{T('Scenes A–H', 'Escenas A–H')}</h2>
<p>{T('Each scene is a full set of patterns for all 8 tracks. Click a letter: ANDROMEDA waits for the end of the bar and moves there smoothly. The letter blinks while it waits.', 'Cada escena es un juego completo de patrones para las 8 pistas. Haz clic en una letra: ANDROMEDA espera al final del compás y se mueve ahí con suavidad. La letra parpadea mientras espera.')}</p>
<p>{T('<b>Switch: 1 / 2 / 4 bars</b> — how long it waits. 4 bars is the most musical for slow songs.', '<b>Cambiar: 1 / 2 / 4 compases</b> — cuánto espera. 4 compases es lo más musical en canciones lentas.')}</p>
<p>{T('Empty scenes look dim. In', 'Las escenas vacías se ven apagadas. En')} {k(T('SONG & FILES', 'CANCIÓN Y ARCHIVOS'))} {T('you can <b>copy</b> a scene to another letter (to make a variation) or <b>clear</b> it.', 'puedes <b>copiar</b> una escena a otra letra (para hacer una variación) o <b>borrarla</b>.')}</p>
<h2>{T('Song mode', 'Modo canción')}</h2>
<p>{T('The song is a list: “A for 8 bars, then B for 8, then C for 16…”. Edit it in SONG & FILES (change the letter and the bars, ✕ to remove a row, + to add one) and turn on', 'La canción es una lista: “A por 8 compases, luego B por 8, luego C por 16…”. Edítala en CANCIÓN Y ARCHIVOS (cambia la letra y los compases, ✕ para quitar una fila, + para agregar) y activa')} {k(T('SONG MODE', 'MODO CANCIÓN'), 'c')}. {T('ALIGN SONG fills it for you.', 'ALINEAR CANCIÓN la llena por ti.')}</p>
</div><div>{fig('song', T('<b>fig. 9</b> — SONG & FILES.', '<b>fig. 9</b> — CANCIÓN Y ARCHIVOS.'))}</div></div>
<h2>{T('Saving and sharing', 'Guardar y compartir')}</h2>
{table([T('Button', 'Botón'), T('What you get', 'Qué obtienes')], [
 [k(T('⤓ SAVE', '⤓ GUARDAR')), T('a <b>.andromeda</b> file with everything (patterns, sounds, knobs). It opens the same in the web, the app and the plug-in.', 'un archivo <b>.andromeda</b> con todo (patrones, sonidos, perillas). Se abre igual en la web, la app y el plug-in.')],
 [k(T('⤒ OPEN', '⤒ ABRIR')), T('loads a .andromeda file.', 'carga un archivo .andromeda.')],
 [k(T('NEW', 'NUEVO')), T('starts from zero (press twice to confirm).', 'empieza de cero (presiona dos veces para confirmar).')],
 [k(T('⤓ MIDI SCENE / SONG', '⤓ MIDI ESCENA / CANCIÓN')), T('a MIDI file of the notes, one track per instrument. Drag it into Ableton to play the notes with other instruments.', 'un archivo MIDI con las notas, una pista por instrumento. Arrástralo a Ableton para tocar las notas con otros instrumentos.')],
 [k('● REC', 'rose'), T('(web) records what you hear to a WAV file until you press it again. In Ableton, record with Live as usual.', '(web) graba lo que escuchas en un archivo WAV hasta que lo vuelvas a presionar. En Ableton, graba con Live como siempre.')]], 'tight')}
''', T('Scenes and song', 'Escenas y canción'))

    # ---------------------------------------------------------- 21 input
    page(f'''{kick(T('Input · your own sound', 'Entrada · tu propio sonido'))}
<h1>{T('Run any audio through <i>ANDROMEDA</i>', 'Pasa cualquier audio por <i>ANDROMEDA</i>')}</h1>
<p class="lead">{T('Track 8 (INPUT) lets ANDROMEDA work as an effect: your voice, a guitar, a synth or a whole song can pass through its filter, the Space Tape, the Cosmos reverb and the 16 effects.', 'La pista 8 (INPUT) permite usar ANDROMEDA como efecto: tu voz, una guitarra, un sinte o una canción completa pueden pasar por su filtro, el Space Tape, la reverb Cosmos y los 16 efectos.')}</p>
<div class="two"><div>
<h3>{T('Web and app', 'Web y app')}</h3>
<ol class="steps"><li>{T('Open', 'Abre')} {k(T('INPUT · MIDI', 'ENTRADA · MIDI'))}.</li>
<li>{T('<b>Microphone:</b> press 🎙 MIC and allow the microphone when the browser asks.', '<b>Micrófono:</b> presiona 🎙 MIC y permite el micrófono cuando el navegador lo pida.')}</li>
<li>{T('<b>A file:</b> press', '<b>Un archivo:</b> presiona')} {k(T('⤒ LOAD AUDIO', '⤒ SUBIR AUDIO'))} {T('and choose a WAV, MP3, AIFF or FLAC. It loops; ▶/■ starts and stops it.', 'y elige un WAV, MP3, AIFF o FLAC. Se repite en loop; ▶/■ lo inicia y lo detiene.')}</li>
<li>{T('Shape it with the INPUT knobs, and play with FX & FILTERS and the macros.', 'Moldéalo con las perillas de INPUT y juega con EFECTOS Y FILTROS y las macros.')}</li></ol>
{table(['', ''], [['FILTER · RESO', T('a filter just for the input', 'un filtro solo para la entrada')], ['DRIVE', T('warmth and saturation', 'calidez y saturación')], ['MOTION', T('the filter moves by itself', 'el filtro se mueve solo')], ['LEVEL', T('volume', 'volumen')], ['TO TAPE · TO COSMOS', T('how much goes to the echo and the reverb', 'cuánto va al eco y a la reverb')]], 'tight')}
</div><div>{fig('setup', T('<b>fig. 10</b> — INPUT · MIDI.', '<b>fig. 10</b> — ENTRADA · MIDI.'))}
<h3>{T('In Ableton (sidechain)', 'En Ableton (sidechain)')}</h3>
<p class="small">{T('To pass another Live track through ANDROMEDA: open the ANDROMEDA device, show its side-chain section (the small triangle ▸ in the device title bar) and in the <b>Audio From</b> menu choose the track you want. Turn the INPUT track’s LEVEL up. You can also use ⤒ LOAD AUDIO, which opens a file window.', 'Para pasar otra pista de Live por ANDROMEDA: abre el dispositivo ANDROMEDA, muestra su sección de side-chain (el pequeño triángulo ▸ en la barra de título del dispositivo) y en el menú <b>Audio From</b> elige la pista que quieras. Sube el LEVEL de la pista INPUT. También puedes usar ⤒ SUBIR AUDIO, que abre una ventana para elegir archivo.')}</p>
<div class="tip"><b>{T('Rhythm from silence.', 'Ritmo desde el silencio.')}</b> {T('The INPUT track’s grid opens and closes the sound in rhythm. Draw a pattern and your voice becomes a pulse.', 'La cuadrícula de la pista INPUT abre y cierra el sonido en ritmo. Dibuja un patrón y tu voz se vuelve un pulso.')}</div>
</div></div>
''', T('Audio input', 'Entrada de audio'))

    # ---------------------------------------------------------- 22 ableton deeper
    page(f'''{kick(T('In Ableton Live', 'En Ableton Live'))}
<h1>{T('ANDROMEDA as a <i>plug-in</i>', 'ANDROMEDA como <i>plug-in</i>')}</h1>
<div class="grid2"><div>
<h3>{T('Tempo and play', 'Tempo y play')}</h3>
<p>{T('Like XO: ANDROMEDA’s own ▶ PLAY / STOP are independent of Live, and it always runs at Live’s tempo. Live’s play lines it up on Live’s bar; Live’s stop stops it only if Live started it.', 'Como XO: ▶ PLAY / STOP de ANDROMEDA son independientes de Live, y siempre va al tempo de Live. El play de Live lo alinea en su compás; el stop de Live lo detiene sólo si Live lo arrancó.')}</p>
<h3>{T('Playing it from a clip', 'Tocarlo desde un clip')}</h3>
<p>{T('Notes in a MIDI clip on the ANDROMEDA track play the selected track, exactly like a keyboard (and with KEYS = SHIFT they can change the key of the song).', 'Las notas de un clip MIDI en la pista de ANDROMEDA tocan la pista seleccionada, igual que un teclado (y con TECLAS = MODULAR pueden cambiar el tono de la canción).')}</p>
<h3>{T('Recording knob moves (automation)', 'Grabar movimientos de perillas (automatización)')}</h3>
<p>{T('ANDROMEDA has 74 parameters Live can record. Turn on Live’s Automation Arm, press record, move knobs in ANDROMEDA or on your controller, and Live draws the curves. You can also draw them in the clip’s envelopes.', 'ANDROMEDA tiene 74 parámetros que Live puede grabar. Activa el Automation Arm de Live, graba, mueve perillas en ANDROMEDA o en tu controlador y Live dibuja las curvas. También puedes dibujarlas en las envolventes del clip.')}</p>
</div><div>
<h3>{T('Saving', 'Guardar')}</h3>
<p>{T('Everything ANDROMEDA is doing is saved inside your Live Set. No extra files needed. You can still save a .andromeda file to use it on the web.', 'Todo lo que ANDROMEDA está haciendo se guarda dentro de tu proyecto de Live. No necesitas archivos extra. Aun así puedes guardar un .andromeda para usarlo en la web.')}</p>
<h3>{T('Drag MIDI and audio', 'Arrastrar MIDI y audio')}</h3>
<p>{T('In SONG · EXPORT: drag <b>⇱ MIDI SCENE</b> or <b>⇱ MIDI SONG</b> onto a MIDI track (one track, channel 1), or <b>⇱ WAV</b> onto an audio track (the scene, 4 bars). Drop a .mid on ANDROMEDA and it takes the clip’s key and scale.', 'En CANCIÓN · EXPORTAR: arrastra <b>⇱ MIDI ESCENA</b> o <b>⇱ MIDI CANCIÓN</b> a una pista MIDI (una pista, canal 1), o <b>⇱ WAV</b> a una pista de audio (la escena, 4 compases). Suelta un .mid sobre ANDROMEDA y toma la tonalidad y escala del clip.')}</p>
<h3>{T('More than one ANDROMEDA', 'Más de un ANDROMEDA')}</h3>
<p>{T('You can open several copies on different tracks: one for pads, one for drums… each one is independent.', 'Puedes abrir varias copias en pistas distintas: una para pads, otra para batería… cada una es independiente.')}</p>
<div class="note"><b>{T('Parameter banks.', 'Bancos de parámetros.')}</b> {T('Live groups the 74 parameters in banks of 8 (see the next page). Any controller that shows Live’s device banks (KeyLab, Push, Launchkey, Komplete Kontrol…) shows them with their names.', 'Live agrupa los 74 parámetros en bancos de 8 (ver la siguiente página). Cualquier controlador que muestre los bancos de Live (KeyLab, Push, Launchkey, Komplete Kontrol…) los muestra con su nombre.')}</div>
</div></div>
''', 'Ableton')

    # ---------------------------------------------------------- 23 keylab
    BK = [('1 PERFORM', 'Drift · Haze · Bloom · Gravity · Auto · Auto Amount · Energy · FX Mix'),
          ('2 SPACE TAPE', 'Tape Mode · Repeat Rate · Intensity · Echo Volume · Reverb Volume · Tape Bass · Tape Treble · Input Volume'),
          ('3 TAPE · COSMOS', 'Wow · Flutter · Tape Age · Echo On · Cosmos Size · Cosmos Damp · Cosmos Pre · Cosmos Volume'),
          ('4 ' + T('VOLUMES', 'VOLÚMENES'), 'Drums · Bass · Pad · Voice · Bells · Lead · Texture · Input'),
          ('5 ' + T('ECHO SENDS', 'ENVÍOS ECO'), T('the same 8 tracks → Space Tape', 'las mismas 8 pistas → Space Tape')),
          ('6 ' + T('COSMOS SENDS', 'ENVÍOS COSMOS'), T('the same 8 tracks → Cosmos', 'las mismas 8 pistas → Cosmos')),
          ('7 ' + T('SOUND', 'SONIDO'), T('Cutoff · Resonance · Attack · Release · Warble · Drive · Level · Filter Env (of the selected track)', 'Cutoff · Resonance · Attack · Release · Warble · Drive · Level · Filter Env (de la pista seleccionada)')),
          ('8 FX', 'FX Type · FX X · FX Y · FX On · Scene · Mood · Chaos · Swing'),
          ('9 EXTRAS', 'Master · Tape Sync · Tape Division · Tape Freeze · Input Filter · Input Reso · Input Drive · Input Motion'),
          ('10', T('Track (which track bank 7 edits) · Key Follow (RAW / SNAP / SHIFT)', 'Track (qué pista edita el banco 7) · Key Follow (LIBRE / AJUSTAR / MODULAR)'))]
    pads = [('A', 's'), ('B', 's'), ('C', 's'), ('D', 's'), ('E', 's'), ('F', 's'), ('G', 's'), ('H', 's'),
            (T('FREEZE', 'CONGELAR'), 'p'), ('TAPE STOP', 'p'), (T('SPIN', 'GIRAR'), 'p'), (T('FILL', 'REDOBLE'), 'p'), ('SUPER<br>NOVA', 'p'), ('ECLIPSE', 'p'), (T('SPRING', 'RESORTE'), 'p'), ('FX<br>ON/OFF', 'p')]
    page(f'''{kick(T('Controllers', 'Controladores'))}
<h1>Arturia <i>KeyLab Essential 49</i></h1>
<div class="grid2"><div>
<h3>{T('Setup in Live (once)', 'Preparación en Live (una vez)')}</h3>
<ol class="steps"><li>{T('Connect the KeyLab by USB. Live → Settings → <b>Link, Tempo &amp; MIDI</b>.', 'Conecta el KeyLab por USB. Live → Settings → <b>Link, Tempo &amp; MIDI</b>.')}</li>
<li>{T('Control Surface: <b>KeyLab Essential mk3</b> (or “KeyLab Essential” for older models). Input and Output: the KeyLab’s <b>DAW</b> port.', 'Control Surface: <b>KeyLab Essential mk3</b> (o “KeyLab Essential” en modelos anteriores). Input y Output: el puerto <b>DAW</b> del KeyLab.')}</li>
<li>{T('In the MIDI ports list, turn on <b>Track</b> and <b>Remote</b> for the KeyLab’s <b>MIDI</b> port.', 'En la lista de puertos MIDI, activa <b>Track</b> y <b>Remote</b> para el puerto <b>MIDI</b> del KeyLab.')}</li>
<li>{T('Select the ANDROMEDA track. The 8 knobs and the screen now show ANDROMEDA’s parameters.', 'Selecciona la pista de ANDROMEDA. Las 8 perillas y la pantalla ahora muestran los parámetros de ANDROMEDA.')}</li></ol>
<h3>{T('Keys, wheel and pedal', 'Teclas, rueda y pedal')}</h3>
<p class="small">{T('<b>Keys</b> play the selected track (with RAW / SNAP / SHIFT). <b>Mod wheel</b> = FX X. <b>Sustain pedal</b> = FREEZE. <b>Faders:</b> in DAW mode Live uses them for its own mixer; to use them in ANDROMEDA, press LEARN in INPUT · MIDI, touch a knob on screen and move a fader.', '<b>Teclas</b> tocan la pista seleccionada (con LIBRE / AJUSTAR / MODULAR). <b>Rueda de modulación</b> = FX X. <b>Pedal sustain</b> = CONGELAR. <b>Faders:</b> en modo DAW Live los usa para su mezclador; para usarlos en ANDROMEDA presiona LEARN en ENTRADA · MIDI, toca una perilla en pantalla y mueve un fader.')}</p>
</div><div>
<h3>{T('The 10 knob banks', 'Los 10 bancos de perillas')}</h3>
<p class="small">{T('Move between banks with the KeyLab’s bank/page buttons.', 'Cambia de banco con los botones de banco/página del KeyLab.')}</p>
{table([T('Bank', 'Banco'), T('Knobs 1–8', 'Perillas 1–8')], BK, 'tight')}
</div></div>
<h3>{T('The pads (MIDI channel 10, notes 36–51)', 'Los pads (canal MIDI 10, notas 36–51)')}</h3>
<div class="pads">{''.join(f'<div class="{c}">{n}<small>{i+1}</small></div>' for i, (n, c) in enumerate(pads))}</div>
<p class="small">{T('Pads 1–8 (bank A) change scenes A–H on the next bar; pads 9–16 (bank B) are the performance buttons. If your pads send something else, set them to channel 10 in Arturia’s MIDI Control Center, or change the pad channel in INPUT · MIDI. <b>Any other controller</b> works the same way: keys, pads on channel 10 and LEARN for everything else.', 'Los pads 1–8 (banco A) cambian las escenas A–H en el siguiente compás; los pads 9–16 (banco B) son los botones de performance. Si tus pads envían otra cosa, ponlos en canal 10 con MIDI Control Center de Arturia, o cambia el canal de pads en ENTRADA · MIDI. <b>Cualquier otro controlador</b> funciona igual: teclas, pads en canal 10 y LEARN para todo lo demás.')}</p>
''', 'KeyLab')

    # ---------------------------------------------------------- 24 keys + midi
    page(f'''{kick(T('Shortcuts and MIDI', 'Atajos y MIDI'))}
<h1>{T('Your computer is an <i>instrument</i>', 'Tu computadora es un <i>instrumento</i>')}</h1>
<div class="grid2"><div>
{table([T('Key', 'Tecla'), T('Action', 'Acción')], [
 [kb(T('Space', 'Espacio')), 'play / stop'], [kb('Z S X D C V G B H N J M , L . ; /'), T('play the selected track, always in key (Z is the lowest)', 'tocar la pista seleccionada, siempre en tono (Z es la más grave)')],
 [kb('-') + ' ' + kb('='), T('octave down / up', 'octava abajo / arriba')], [kb('1') + ' … ' + kb('8'), T('select track 1–8', 'seleccionar pista 1–8')], [kb('Q W E R T Y U I'), T('scenes A–H', 'escenas A–H')],
 [kb('A'), 'AUTO'], [kb('O'), T('FREEZE', 'CONGELAR')], [kb('P'), T('TAPE STOP (hold)', 'TAPE STOP (mantener)')], [kb('['), T('SPIN', 'GIRAR')], [kb(']'), T('FILL', 'REDOBLE')], [kb('\\'), 'SUPERNOVA'], [kb("'"), T('ECLIPSE (hold)', 'ECLIPSE (mantener)')], [kb('K'), T('SPRING KICK', 'GOLPE AL RESORTE')], [kb('F'), 'FX on/off'], [kb('`'), T('Observatory', 'Observatorio')]], 'tight')}
<p class="small">{T('On a Spanish keyboard some symbols are in other places; the letters always work.', 'En un teclado en español algunos símbolos están en otro lugar; las letras siempre funcionan.')}</p>
</div><div>
<h3>{T('Knobs with the mouse', 'Perillas con el mouse')}</h3>
<p class="small">{T('Drag up/down · mouse wheel · hold Shift for fine moves · double-click to reset.', 'Arrastra arriba/abajo · rueda del mouse · mantén Shift para movimientos finos · doble clic para restablecer.')}</p>
<h3>{T('MIDI keyboards on the web', 'Teclados MIDI en la web')}</h3>
<ol class="steps"><li>{T('Connect the keyboard before opening the page (Chrome or Edge).', 'Conecta el teclado antes de abrir la página (Chrome o Edge).')}</li>
<li>{T('INPUT · MIDI →', 'ENTRADA · MIDI →')} {k('ENABLE MIDI', 'a')} {T('and allow it when Chrome asks.', 'y permítelo cuando Chrome pregunte.')}</li></ol>
<p class="small">{T('In the app, choose the keyboard in Options → Audio/MIDI Settings. In Ableton, MIDI arrives from Live’s track.', 'En la app, elige el teclado en Options → Audio/MIDI Settings. En Ableton, el MIDI llega desde la pista de Live.')}</p>
<h3>MIDI LEARN</h3>
<ol class="steps"><li>{T('Press', 'Presiona')} {k('LEARN', 'c')}: {T('the learnable knobs glow.', 'las perillas que se pueden asignar se iluminan.')}</li><li>{T('Click a knob on screen.', 'Haz clic en una perilla en pantalla.')}</li><li>{T('Move a knob or fader on your controller. Done: it is linked.', 'Mueve una perilla o fader de tu controlador. Listo: quedó enlazado.')}</li></ol>
</div></div>
''', T('Shortcuts', 'Atajos'))

    # ---------------------------------------------------------- 25 recipes
    R = [(T('Rain over a frozen cathedral', 'Lluvia sobre una catedral congelada'), T('BREATH · ALIGN SONG · track 7: RAIN, LEVEL up · Space Tape mode 12 · BLOOM 80% · after 8 bars press FREEZE and play slow notes on top with KEYS = SNAP.', 'BREATH · ALINEAR CANCIÓN · pista 7: RAIN, LEVEL arriba · Space Tape modo 12 · FLORECER 80% · después de 8 compases presiona CONGELAR y toca notas lentas encima con TECLAS = AJUSTAR.')),
         (T('Childhood cassette', 'Casete de la infancia'), T('LULLABY · bells: Music Box · DRIFT 60% · HAZE 55% · TAPE AGE 80% · Space Tape mode 9 with SYNC 1/8. dotted.', 'LULLABY · campanas: Music Box · DERIVA 60% · BRUMA 55% · TAPE AGE 80% · Space Tape modo 9 con SYNC 1/8. con punto.')),
         (T('Dub in space', 'Dub en el espacio'), T('PULSE · Space Tape mode 9, INTENSITY at 3 o’clock · drag the XY pad left and right during the breaks · SPRING KICK on the last beat.', 'PULSE · Space Tape modo 9, INTENSITY a las 3 · arrastra el pad XY a izquierda y derecha en las pausas · GOLPE AL RESORTE en el último tiempo.')),
         (T('The drop that never comes', 'La caída que nunca llega'), T('ORBIT · HIGH SWEEP, slowly drag X to the right for 4 bars · SUPERNOVA · switch to the next scene on the boom · ECLIPSE for the break.', 'ORBIT · HIGH SWEEP, arrastra X a la derecha despacio por 4 compases · SUPERNOVA · cambia a la siguiente escena en el golpe · ECLIPSE para la pausa.')),
         (T('Talking to the machine', 'Hablando con la máquina'), T('Any mood · AUTO on, ENERGY 40% · KEYS = SHIFT · play a short melody with a “wrong” note: listen how the song changes key and the bells answer you.', 'Cualquier estado · AUTO encendido, ENERGÍA 40% · TECLAS = MODULAR · toca una melodía corta con una nota “equivocada”: escucha cómo la canción cambia de tono y las campanas te contestan.')),
         (T('Your voice as a choir', 'Tu voz como coro'), T('INPUT · MIDI → MIC · INPUT FILTER 60%, MOTION 50% · TO COSMOS 80% · FX: VOWEL · sing one long note and press FREEZE.', 'ENTRADA · MIDI → MIC · INPUT FILTER 60%, MOTION 50% · TO COSMOS 80% · FX: VOWEL · canta una nota larga y presiona CONGELAR.'))]
    page(f'''{kick(T('Recipes', 'Recetas'))}
<h1>{T('Six ideas to <i>start</i>', 'Seis ideas para <i>empezar</i>')}</h1>
<p class="lead">{T('Follow them step by step the first time; then change one thing and make them yours.', 'Síguelas paso a paso la primera vez; luego cambia una cosa y hazlas tuyas.')}</p>
<div class="grid2">{''.join(f'<div class="recipe"><h2>{a}</h2><p class="small">{b}</p></div>' for a, b in R)}</div>
<div class="art" style="right:-.4in;bottom:-.3in;width:3.2in;height:3.2in;opacity:.22">{art.attractor(400)}</div>
''', T('Recipes', 'Recetas'))

    # ---------------------------------------------------------- 26 troubleshooting
    FAQ = [(T('I hear nothing (web)', 'No escucho nada (web)'), T('Press ENTER/ENTRAR on the welcome screen; check the volume of your computer; try Chrome. If the page was open for a long time, reload it.', 'Presiona ENTRAR en la pantalla de bienvenida; revisa el volumen de tu computadora; prueba con Chrome. Si la página estuvo abierta mucho tiempo, recárgala.')),
           (T('Crackles or cuts', 'Chasquidos o cortes'), T('Turn on LIGHT ENGINE (top). Close other tabs. In Ableton, raise the buffer size in Settings → Audio (512 is safe).', 'Activa MOTOR LIGERO (arriba). Cierra otras pestañas. En Ableton, sube el buffer en Settings → Audio (512 es seguro).')),
           (T('macOS doesn’t let me open the installer', 'macOS no me deja abrir el instalador'), T('Use the Terminal line on page', 'Usa la línea de Terminal de la página') + f' {PG_MAC}.'),
           (T('ANDROMEDA doesn’t appear in Ableton', 'ANDROMEDA no aparece en Ableton'), T('Quit Live (⌘Q) and open it again; check that “Use VST3 Plug-In System Folders” is on and press Rescan. Run the install line again if needed.', 'Cierra Live (⌘Q) y vuelve a abrirlo; revisa que “Use VST3 Plug-In System Folders” esté activado y presiona Rescan. Si hace falta, vuelve a correr la línea de instalación.')),
           (T('The plug-in window is empty or black', 'La ventana del plug-in está vacía o negra'), T('Close and open the window again (wrench icon). ANDROMEDA uses the Mac’s web engine to draw its screen; updating macOS helps.', 'Cierra y abre otra vez la ventana (ícono de llave). ANDROMEDA usa el motor web de la Mac para dibujar su pantalla; actualizar macOS ayuda.')),
           (T('My MIDI keyboard does nothing (web)', 'Mi teclado MIDI no hace nada (web)'), T('Use Chrome or Edge (Safari has no Web MIDI). Connect it before opening the page and press ENABLE MIDI.', 'Usa Chrome o Edge (Safari no tiene Web MIDI). Conéctalo antes de abrir la página y presiona ENABLE MIDI.')),
           (T('The KeyLab pads don’t change scenes', 'Los pads del KeyLab no cambian escenas'), T('They must send notes 36–51 on channel 10. Check in Arturia MIDI Control Center or change the pad channel in INPUT · MIDI.', 'Deben enviar las notas 36–51 en el canal 10. Revísalo en MIDI Control Center de Arturia o cambia el canal de pads en ENTRADA · MIDI.')),
           (T('The song changed key by itself', 'La canción cambió de tono sola'), T('That is KEYS = SHIFT reacting to a note outside the scale. Choose SNAP or RAW if you don’t want it, or set the key again at the top.', 'Es TECLAS = MODULAR reaccionando a una nota fuera de la escala. Elige AJUSTAR o LIBRE si no lo quieres, o vuelve a elegir el tono arriba.')),
           (T('It sounds too busy', 'Suena demasiado lleno'), T('Lower ENERGY, mute a track (M), or lower CHAOS before ALIGN.', 'Baja la ENERGÍA, silencia una pista (M) o baja CHAOS antes de ALINEAR.'))]
    page(f'''{kick(T('Help', 'Ayuda'))}
<h1>{T('If something <i>doesn’t work</i>', 'Si algo <i>no funciona</i>')}</h1>
{table([T('Problem', 'Problema'), T('Solution', 'Solución')], FAQ)}
<div class="note"><b>{T('Still stuck?', '¿Sigues atorado?')}</b> {T('Write to Polaris Sound Lab on Instagram', 'Escríbele a Polaris Sound Lab en Instagram')} <a href="{IG}">@polaris.sound.lab</a>{T(' with a photo or a screen recording of what you see.', ' con una foto o una grabación de pantalla de lo que ves.')}</div>
''', T('Help', 'Ayuda'))

    # ---------------------------------------------------------- 27 back
    page(f'''<div class="back">
<div style="width:4.6in;height:4.6in;opacity:.85">{art.chladni(400, 2, 5, seed=11, count=3200, op=.55)}</div>
<div style="font:300 30pt/1 Cormorant;letter-spacing:.42em;margin:.1in -.42em 0 0">ANDROMEDA</div>
<div class="csub" style="font:italic 400 14pt Cormorant;color:var(--star2);margin-top:.08in">{T('an ambient instrument', 'un instrumento ambient')}</div>
<div style="margin-top:.35in;font:600 7.6pt/2 'Inter Tight';letter-spacing:.3em;color:var(--star2)">DARK FUNK DURCAL DESIGN<br>{T('CREATED AT', 'CREADO EN')} <b style="color:var(--amber2)">POLARIS SOUND LAB</b> · MÉRIDA, YUCATÁN, MÉXICO</div>
<div style="margin-top:.2in;font-size:9.4pt;line-height:1.9"><a href="{IG}">instagram.com/polaris.sound.lab</a> &nbsp;·&nbsp; <a href="{YT}">youtube.com/@PolarisSoundLab</a><br><a href="{WEB}">drophlex.github.io/ANDROMEDA</a> &nbsp;·&nbsp; <a href="https://github.com/drophlex/ANDROMEDA">github.com/drophlex/ANDROMEDA</a></div>
<div style="margin-top:.26in"><a class="paypal" href="{DON}">♥ {T('Support ANDROMEDA · Donate with PayPal', 'Apoya ANDROMEDA · Dona con PayPal')}</a><div class="small" style="color:var(--star2);margin-top:.08in;font-size:8pt">{T('Free and open source: the code is yours to read, change and share (AGPL-3.0).', 'Gratis y de código abierto: el código es tuyo para leerlo, cambiarlo y compartirlo (AGPL-3.0).')}</div></div>
<div class="small" style="margin-top:.35in;max-width:5.2in;color:var(--dim);font-size:7.6pt">{T('ANDROMEDA 1.0. Plug-in built with JUCE. VST is a trademark of Steinberg Media Technologies GmbH; Audio Units, Mac and macOS are trademarks of Apple Inc.; Ableton and Live are trademarks of Ableton AG; Arturia and KeyLab are trademarks of Arturia. ANDROMEDA is not affiliated with these companies.', 'ANDROMEDA 1.0. Plug-in hecho con JUCE. VST es marca de Steinberg Media Technologies GmbH; Audio Units, Mac y macOS son marcas de Apple Inc.; Ableton y Live son marcas de Ableton AG; Arturia y KeyLab son marcas de Arturia. ANDROMEDA no está afiliado a esas empresas.')}</div>
</div>''', '', cls='coverp')

NPL = 27; TOC = ''; PG_MAC = 5
def toc_html():
    want = [(T('Words you will see', 'Palabras que vas a ver'), 3), (T('Install: web', 'Instalar: web'), 4), (T('Install: Mac app and plug-in', 'Instalar: app y plug-in para Mac'), 5), (T('Install: Ableton Live', 'Instalar: Ableton Live'), 7),
            (T('Your first song in 5 minutes', 'Tu primera canción en 5 minutos'), 8), (T('The screen at a glance', 'La pantalla de un vistazo'), 9), (T('Moods', 'Estados'), 10), (T('Tracks', 'Pistas'), 11),
            (T('Drawing patterns', 'Dibujar patrones'), 12), (T('AUTO and your keyboard', 'AUTO y tu teclado'), 13), (T('Sound and presets', 'Sonido y presets'), 14), ('Space Tape', 15),
            (T('FX & Filters', 'Efectos y filtros'), 17), (T('Playing live', 'Tocar en vivo'), 18), (T('Visions', 'Visiones'), 19), (T('Scenes, song and files', 'Escenas, canción y archivos'), 20),
            (T('Audio input', 'Entrada de audio'), 21), (T('In Ableton Live', 'En Ableton Live'), 22), ('KeyLab Essential 49', 23), (T('Shortcuts and MIDI', 'Atajos y MIDI'), 24),
            (T('Recipes', 'Recetas'), 25), (T('If something doesn’t work', 'Si algo no funciona'), 26)]
    off = 0 if L == 'en' else NPL
    return ''.join(f'<div><span>{a}</span><span>{b + off}</span></div>' for a, b in want)

def render(langs, out):
    global L, TOC, PG_MAC
    allp = []
    for lang in langs:
        L = lang; off = 0 if (lang == 'en' or len(langs) == 1) else NPL
        TOC = toc_html() if len(langs) > 1 else toc_html().replace('', '')
        if len(langs) == 1:   # single-language book: pages numbered from 1
            TOC = ''.join(f'<div><span>{a}</span><span>{b}</span></div>' for a, b in [])
            L = lang; tmp = toc_html()
            import re
            TOC = re.sub(r'<span>(\d+)</span></div>', lambda m: f'<span>{int(m.group(1)) - (NPL if lang == "es" else 0)}</span></div>', tmp)
        PG_MAC = 5 + off
        build()
        assert len(PAGES) == NPL, len(PAGES)
        for i, (cls, body, title) in enumerate(PAGES):
            n = i + 1 + off
            foot = '' if cls == 'coverp' else f'<footer><span>ANDROMEDA · {"MANUAL" if lang == "en" else "MANUAL"} · {lang.upper()}</span><span>{title}</span><span class="pn">{n:02d}</span></footer>'
            allp.append(f'<section class="page {cls}">{body}{foot}</section>')
    html = f'<!DOCTYPE html><html><head><meta charset="utf-8"><title>ANDROMEDA · Manual</title><style>{open(os.path.join(HERE, "manual.css")).read()}</style></head><body>{"".join(allp)}</body></html>'
    open(out, 'w').write(html)

if __name__ == '__main__':
    render(['en', 'es'], os.path.join(HERE, 'manual_en_es.html'))
    render(['es'], os.path.join(HERE, 'manual_es.html'))
    render(['en'], os.path.join(HERE, 'manual_en.html'))
    print('ok')
