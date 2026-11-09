#!/usr/bin/env python3
"""Sinh hai skin của tsudev cho cửa sổ nổi từ ảnh nguồn trong thư mục này.

  tsudev-cu.png        chú cú mascot tsudev (nền trong suốt)  -> skin mặc định  0_tsudev_Cu   (nền sáng)
  tsudev-vong-mang.png avatar "tsudev_" với vòng mạng          -> skin           tsudev_VongMang (nền tối)

Ảnh nền vẽ ở tỉ lệ 1,5 so với kích thước cửa sổ trong skin.ini (giống các skin có sẵn): cửa sổ lớn 232x55 -> 348x82,
cửa sổ nhỏ 232x40 -> 348x59. Ảnh xem trước cho KhoGiaoDien.md lưu ở Screenshots/skins/.
Màu lấy từ token tsudev (tsudev-standards/tokens). Cần Pillow.

  python3 tools/skin_assets/make_skins.py --font C:/Windows/Fonts/segoeui.ttf
"""
from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image, ImageDraw, ImageFilter, ImageFont

HERE = Path(__file__).resolve().parent
ROOT = HERE.parent.parent
SKINS = ROOT / "TrafficMonitor/skins"
SHOTS = ROOT / "Screenshots/skins"
SCALE = 1.5

# Bố cục (đơn vị: điểm ảnh cửa sổ). Chữ nằm bên trái, hình ở bên phải trong vùng x >= 190.
LAYOUT = """[skin]
text_color={text_color}
specify_each_item_color=0
skin_author="tsudev"
[layout]
text_height=20
no_text=0
preview_width=250
preview_height=128
width_l=232
height_l=55
up_x_l=10
up_y_l=8
up_width_l=88
up_align_l=0
down_x_l=100
down_y_l=8
down_width_l=88
down_align_l=0
cpu_x_l=10
cpu_y_l=28
cpu_width_l=88
cpu_align_l=0
memory_x_l=100
memory_y_l=28
memory_width_l=88
memory_align_l=0
show_up_l=1
show_down_l=1
show_cpu_l=1
show_memory_l=1
preview_x_l=0
preview_y_l=59
width_s=232
height_s=40
up_x_s=10
up_y_s=10
up_width_s=88
up_align_s=0
down_x_s=100
down_y_s=10
down_width_s=88
down_align_s=0
cpu_x_s=0
cpu_y_s=0
cpu_width_s=0
cpu_align_s=0
memory_x_s=0
memory_y_s=0
memory_width_s=0
memory_align_s=0
show_up_s=1
show_down_s=1
show_cpu_s=0
show_memory_s=0
preview_x_s=0
preview_y_s=0
"""


def hex_rgb(h: str) -> tuple[int, int, int]:
    h = h.lstrip("#")
    return tuple(int(h[i:i + 2], 16) for i in (0, 2, 4))  # type: ignore[return-value]


def colorref(h: str) -> int:
    """COLORREF của Windows: 0x00BBGGRR."""
    r, g, b = hex_rgb(h)
    return (b << 16) | (g << 8) | r


def card(w: int, h: int, top: str, bottom: str, border: str, radius: int) -> Image.Image:
    """Thẻ bo góc, nền chuyển màu dọc, viền 2px, ngoài thẻ trong suốt."""
    grad = Image.new("RGBA", (w, h))
    t, b = hex_rgb(top), hex_rgb(bottom)
    for y in range(h):
        k = y / max(h - 1, 1)
        c = tuple(round(t[i] + (b[i] - t[i]) * k) for i in range(3)) + (255,)
        ImageDraw.Draw(grad).line([(0, y), (w, y)], fill=c)
    mask = Image.new("L", (w * 4, h * 4), 0)
    ImageDraw.Draw(mask).rounded_rectangle((0, 0, w * 4 - 1, h * 4 - 1), radius=radius * 4, fill=255)
    mask = mask.resize((w, h), Image.LANCZOS)
    out = Image.new("RGBA", (w, h), (0, 0, 0, 0))
    out.paste(grad, (0, 0), mask)
    ring = Image.new("L", (w * 4, h * 4), 0)
    ImageDraw.Draw(ring).rounded_rectangle((0, 0, w * 4 - 1, h * 4 - 1), radius=radius * 4, outline=255, width=8)
    ring = ring.resize((w, h), Image.LANCZOS)
    out.paste(Image.new("RGBA", (w, h), hex_rgb(border) + (255,)), (0, 0), ring)
    return out


def place_right(bg: Image.Image, art: Image.Image, height: int, margin: int, glow: str | None = None) -> None:
    w = round(art.width * height / art.height)
    a = art.resize((w, height), Image.LANCZOS)
    x, y = bg.width - w - margin, (bg.height - height) // 2
    if glow:
        halo = Image.new("RGBA", bg.size, (0, 0, 0, 0))
        alpha = a.split()[3].filter(ImageFilter.GaussianBlur(4))
        halo.paste(Image.new("RGBA", a.size, hex_rgb(glow) + (110,)), (x, y), alpha)
        bg.alpha_composite(halo)
    bg.alpha_composite(a, (x, y))


def circle_crop(img: Image.Image) -> Image.Image:
    s = min(img.size)
    img = img.crop(((img.width - s) // 2, (img.height - s) // 2, (img.width + s) // 2, (img.height + s) // 2)).convert("RGBA")
    m = Image.new("L", (s, s), 0)
    ImageDraw.Draw(m).ellipse((0, 0, s - 1, s - 1), fill=255)
    img.putalpha(m)
    return img


def preview(bg: Image.Image, font: ImageFont.FreeTypeFont, color: str, large: bool) -> Image.Image:
    """Ảnh xem trước: nền + chữ mẫu đặt đúng vị trí trong skin.ini (tỉ lệ 1,5)."""
    im = bg.copy()
    d = ImageDraw.Draw(im)
    rows = [("↑: 88.8 KB/s", 10, 8 if large else 10), ("↓: 88.9 KB/s", 100, 8 if large else 10)]
    if large:
        rows += [("CPU: 50 %", 10, 28), ("RAM: 51 %", 100, 28)]
    for text, x, y in rows:
        d.text((x * SCALE, y * SCALE + 20 * SCALE / 2), text, font=font, fill=hex_rgb(color) + (255,), anchor="lm")
    return im


def build(name: str, art: Image.Image, top: str, bottom: str, border: str, text: str, glow: str | None, font_path: str,
          art_scale: float = 1.0) -> None:
    out = SKINS / name
    out.mkdir(parents=True, exist_ok=True)
    font = ImageFont.truetype(font_path, round(12 * SCALE))
    for fname, (w, h), large in (("background_l.png", (348, 82), True), ("background.png", (348, 59), False)):
        bg = card(w, h, top, bottom, border, radius=12)
        place_right(bg, art, height=round((h - 10) * art_scale), margin=8, glow=glow)
        bg.save(out / fname, optimize=True)
        if large:
            SHOTS.mkdir(parents=True, exist_ok=True)
            bg.save(SHOTS / f"{name}.png", optimize=True)   # như các skin khác: chỉ ảnh nền (KhoGiaoDien.md)
            preview(bg, font, text, True).save(HERE / f"xem-truoc-{name}.png", optimize=True)
    (out / "skin.ini").write_text(LAYOUT.format(text_color=colorref(text)), encoding="utf-8")
    print("sinh skin", name)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--font", required=True, help="Segoe UI (segoeui.ttf, phông mặc định của ứng dụng; chỉ dùng cho ảnh xem trước)")
    a = ap.parse_args()
    cu = Image.open(HERE / "tsudev-cu.png").convert("RGBA")
    cu = cu.crop(cu.getbbox())
    vong = circle_crop(Image.open(HERE / "tsudev-vong-mang.png"))
    # Nền sáng (token light): bg-surface -> bg-base, viền border-strong, chữ text-primary
    build("0_tsudev_Cu", cu, "#FFFFFF", "#E3EDF8", "#9FB8D4", "#1C2B3A", None, a.font)
    # Nền tối (token dark): bg-surface -> bg-base, viền border, chữ text-primary, quầng sáng primary
    build("tsudev_VongMang", vong, "#16263C", "#0F1B2D", "#3F5B80", "#E6EDF5", "#66A3F2", a.font, art_scale=0.8)


if __name__ == "__main__":
    main()
