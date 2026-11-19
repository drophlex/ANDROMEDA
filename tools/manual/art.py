# mathematical art for the manual (same families as the instrument's visions)
import math, random

def harmonograph(size=600, seed=3, f=(2, 3, 3.003, 2.002), decay=0.0016, n=9000, stroke='#e8dfd2', op=.55, w=.8, dot='#c98a55'):
    r = random.Random(seed); ph = [0, 1.57 + r.uniform(-.3, .3), .8 + r.uniform(-.3, .3), r.uniform(-.2, .2)]
    pts = []
    for i in range(n):
        t = i * 0.03; e = math.exp(-decay * t * 6)
        x = size / 2 + size * .22 * (math.sin(f[0] * t + ph[0]) + math.sin(f[2] * t + ph[1])) * e
        y = size / 2 + size * .22 * (math.sin(f[1] * t + ph[2]) + math.sin(f[3] * t + ph[3])) * e
        pts.append(f'{x:.1f},{y:.1f}')
    d = 'M' + ' L'.join(pts)
    return (f'<svg viewBox="0 0 {size} {size}" xmlns="http://www.w3.org/2000/svg"><path d="{d}" fill="none" stroke="{stroke}" '
            f'stroke-opacity="{op}" stroke-width="{w}"/><circle cx="{size/2}" cy="{size/2}" r="4" fill="{dot}"/></svg>')

def chladni(size=400, m=3, n=5, seed=1, color='#e8dfd2', count=2600, op=.7):
    r = random.Random(seed); dots = []
    tries = 0
    while len(dots) < count and tries < 400000:
        tries += 1
        x, y = r.random(), r.random()
        v = math.cos(n * math.pi * x) * math.cos(m * math.pi * y) - math.cos(m * math.pi * x) * math.cos(n * math.pi * y)
        if abs(v) < 0.035:
            dots.append(f'<circle cx="{x*size:.1f}" cy="{y*size:.1f}" r="{r.uniform(.5,1.2):.2f}"/>')
    return f'<svg viewBox="0 0 {size} {size}" xmlns="http://www.w3.org/2000/svg"><g fill="{color}" fill-opacity="{op}">{"".join(dots)}</g></svg>'

def attractor(size=400, a=-1.4, b=1.6, c=1.0, d=0.7, n=14000, color='#93b3c2', op=.5):
    x = y = 0.1; pts = []
    for i in range(n):
        x, y = math.sin(a * y) + c * math.cos(a * x), math.sin(b * x) + d * math.cos(b * y)
        if i > 20: pts.append(f'<circle cx="{size/2 + x*size*.21:.1f}" cy="{size/2 + y*size*.21:.1f}" r=".55"/>')
    return f'<svg viewBox="0 0 {size} {size}" xmlns="http://www.w3.org/2000/svg"><g fill="{color}" fill-opacity="{op}">{"".join(pts)}</g></svg>'

def logo(size=40, color='#e8dfd2'):
    # the instrument's mark: two crossed orbits around a point
    return (f'<svg viewBox="0 0 40 40" width="{size}" height="{size}" xmlns="http://www.w3.org/2000/svg"><g fill="none" stroke="{color}" stroke-width="1.3">'
            '<ellipse cx="20" cy="20" rx="16" ry="7" transform="rotate(30 20 20)"/><ellipse cx="20" cy="20" rx="16" ry="7" transform="rotate(-30 20 20)"/>'
            '<ellipse cx="20" cy="20" rx="16" ry="7" transform="rotate(90 20 20)" stroke-opacity=".5"/></g><circle cx="20" cy="20" r="2.4" fill="#c98a55"/></svg>')
