"""
Bake stage1 background into 3 layered PNGs for parallax scrolling.
  stage1_bg_far.png  (layer 2 – far wall,  opaque background)
  stage1_bg_mid.png  (layer 1 – mid deco,  transparent background)
  stage1_bg_near.png (layer 0 – ground,    transparent background)
All 384 × 2250, covering world y = +50 → -2200.
"""
import re, os
from PIL import Image

STDSPEC    = r"C:\Users\tommy\Desktop\Projects\Others\truth-0.5.2-x86_64-pc-windows-msvc\truth-0.5.2\out.stdspec"
SPRITE_DIR = r"C:\Users\tommy\Desktop\Projects\MyProjects\touhou-koumakyou-clone\resources\th06c\th06c_ST_output\stg1bg"
OUT_FAR    = r"C:\Users\tommy\Desktop\Projects\MyProjects\touhou-koumakyou-clone\resources\stage1_bg_far.png"
OUT_MID    = r"C:\Users\tommy\Desktop\Projects\MyProjects\touhou-koumakyou-clone\resources\stage1_bg_mid.png"
OUT_NEAR   = r"C:\Users\tommy\Desktop\Projects\MyProjects\touhou-koumakyou-clone\resources\stage1_bg_near.png"

CANVAS_W    = 384
WORLD_Y_TOP = 50
WORLD_Y_BOT = -2200
CANVAS_H    = WORLD_Y_TOP - WORLD_Y_BOT   # 2250
BG_COLOR    = (16, 0, 32)

# ── Load sprites ─────────────────────────────────────────────────────────────
raw = {}
for i in range(28):
    p = os.path.join(SPRITE_DIR, f"sprite_{i}.png")
    if os.path.exists(p):
        raw[i] = Image.open(p).convert("RGBA")

def _alpha(img, factor):
    r, g, b, a = img.split()
    img.putalpha(a.point(lambda x: int(x * factor)))
    return img

def build_img(sid):
    if 0 <= sid <= 15: return raw[sid].resize((32, 32), Image.NEAREST)
    if sid == 16: return raw[16].copy()
    if sid == 17: return raw[16].transpose(Image.FLIP_LEFT_RIGHT)
    if sid == 18: return raw[17].copy()
    if sid == 19: return raw[17].transpose(Image.FLIP_LEFT_RIGHT)
    if 20 <= sid <= 22: return _alpha(raw[18 + (sid - 20)].copy(), 160 / 255)
    if 23 <= sid <= 25: return _alpha(raw[21 + (sid - 23)].resize((28, 28), Image.NEAREST), 128 / 255)
    if 26 <= sid <= 29: return raw[24 + (sid - 26)].copy()
    if 30 <= sid <= 33: return raw[24 + (sid - 30)].transpose(Image.ROTATE_180)
    return None

script_imgs = {}
for sid in range(34):
    try:
        img = build_img(sid)
        if img: script_imgs[sid] = img
    except Exception:
        pass

# ── Parse stdspec ─────────────────────────────────────────────────────────────
with open(STDSPEC, encoding="utf-8") as f:
    content = f.read()

objs_match = re.search(r'objects:\s*\{(.*?)\},\s*instances:', content, re.DOTALL)
assert objs_match
objs_txt = objs_match.group(1)

quad_re = re.compile(
    r'anm_script:\s*(\d+).*?pos:\s*\[([^\]]+)\].*?size:\s*\[([^\]]+)\]', re.DOTALL)

objects = {}
parts = re.split(r'(object\d+):', objs_txt)
i = 1
while i < len(parts) - 1:
    name  = parts[i].strip()
    block = parts[i + 1]
    oid   = int(re.search(r'\d+', name).group())
    lm    = re.search(r'layer:\s*(\d+)', block)
    layer = int(lm.group(1)) if lm else 0
    quads = []
    for qm in quad_re.finditer(block):
        sid  = int(qm.group(1))
        pos  = [float(v) for v in qm.group(2).split(',')]
        size = [float(v) for v in qm.group(3).split(',')]
        quads.append((sid, pos[0], pos[1], size[0], size[1]))
    objects[oid] = (layer, quads)
    i += 2

inst_start    = content.index('instances:')
bracket_open  = content.index('[', inst_start)
depth = 0; bracket_close = bracket_open
for k, ch in enumerate(content[bracket_open:], bracket_open):
    if ch == '[':   depth += 1
    elif ch == ']':
        depth -= 1
        if depth == 0: bracket_close = k; break
inst_body = content[bracket_open + 1 : bracket_close]

instances = []
for m in re.finditer(r'object(\d+)\s*\{pos:\s*\[([^\]]+)\]', inst_body):
    oid = int(m.group(1))
    pos = [float(v) for v in m.group(2).split(',')]
    instances.append((oid, pos[0], pos[1]))

print(f"Parsed {len(objects)} objects, {len(instances)} instances")

# ── Render ────────────────────────────────────────────────────────────────────
canvases = {
    2: Image.new("RGBA", (CANVAS_W, CANVAS_H), (*BG_COLOR, 255)),  # far  – opaque bg
    1: Image.new("RGBA", (CANVAS_W, CANVAS_H), (0, 0, 0, 0)),      # mid  – transparent
    0: Image.new("RGBA", (CANVAS_W, CANVAS_H), (0, 0, 0, 0)),      # near – transparent
}

def world_to_canvas(wx, wy):
    return int(wx), int(WORLD_Y_TOP - wy)

for oid, ix, iy in instances:
    if oid not in objects: continue
    layer, quads = objects[oid]
    if layer not in canvases: continue
    canvas = canvases[layer]
    for sid, qx, qy, sw, sh in quads:
        img = script_imgs.get(sid)
        if img is None: continue
        w = int(sw) if sw > 0 else img.width
        h = int(sh) if sh > 0 else img.height
        if w != img.width or h != img.height:
            img = img.resize((w, h), Image.NEAREST)
        cx, cy = world_to_canvas(ix + qx, iy + qy)
        canvas.paste(img, (cx, cy), img)

canvases[2].convert("RGB").save(OUT_FAR)
canvases[1].save(OUT_MID)
canvases[0].save(OUT_NEAR)
print(f"Saved far/mid/near  ({CANVAS_W}×{CANVAS_H})")
