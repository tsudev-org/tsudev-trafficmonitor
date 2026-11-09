#!/usr/bin/env python3
"""Sinh hai skin "đồ thị mạng" của tsudev cho cửa sổ nổi: tsudev_DoThi_Sang (nền sáng) và tsudev_DoThi_Toi (nền tối).

Nền: thẻ bo góc, lưới mờ, hai đường đồ thị tốc độ (tải xuống/tải lên) dạng vùng tô trong suốt ở nửa dưới,
dấu hiệu cú tsudev nhỏ ở góc phải trên. PNG trong suốt ngoài thẻ.
Cấu hình skin.xml: cửa sổ lớn 3 hàng (↑/↓, CPU/RAM, GPU/HDD), cửa sổ nhỏ 1 hàng (↑/↓).

Ảnh vẽ ở tỉ lệ 1,5 so với kích thước trong skin.xml (như các skin có sẵn). Đồ thị sinh từ hàm cố định (không ngẫu nhiên)
nên chạy lại cho cùng kết quả. Màu lấy từ token tsudev (light/dark). Cần Pillow.

  python3 tools/skin_assets/make_skins_dothi.py --font C:/Windows/Fonts/segoeui.ttf
"""
from __future__ import annotations

import argparse
import hashlib
import math
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

from make_skins import HERE, SHOTS, SKINS, SCALE, card, colorref, hex_rgb

ROOT = HERE.parent.parent
MARK = ROOT / "website/public/brand/logo-mark.png"
MARK_SHA256 = "4cb423dc6a3e8d978f80ec7805109c2c4bdaf0a24578dbd6fb2d4a05d0817198"

W_L, H_L = 232, 80      # cửa sổ lớn (đơn vị điểm ảnh cửa sổ)
W_S, H_S = 232, 40      # cửa sổ nhỏ
COL1, COL2, COL_W = 10, 116, 100
ROWS_L = (4, 29, 54)

XML = """\ufeff<?xml version="1.0" encoding="utf-8"?>
<root>
\t<skin>
\t\t<text_color>{text_color}</text_color>
\t\t<specify_each_item_color>0</specify_each_item_color>
\t\t<skin_author>tsudev</skin_author>
\t\t<font name="Segoe UI" size="9" style="0"/>
\t\t<display_text>
\t\t\t<up>↑: </up>
\t\t\t<down>↓: </down>
\t\t\t<cpu>CPU: </cpu>
\t\t\t<memory>RAM: </memory>
\t\t\t<gpu>GPU: </gpu>
\t\t\t<hdd>HDD: </hdd>
\t\t</display_text>
\t</skin>
\t<layout text_height="20" no_label="0">
\t\t<layout_l width="{wl}" height="{hl}">
\t\t\t<up x="{c1}" y="{r1}" width="{cw}" align="0" show="1"/>
\t\t\t<down x="{c2}" y="{r1}" width="{cw}" align="0" show="1"/>
\t\t\t<cpu x="{c1}" y="{r2}" width="{cw}" align="0" show="1"/>
\t\t\t<memory x="{c2}" y="{r2}" width="{cw}" align="0" show="1"/>
\t\t\t<gpu x="{c1}" y="{r3}" width="{cw}" align="0" show="1"/>
\t\t\t<hdd x="{c2}" y="{r3}" width="{cw}" align="0" show="1"/>
\t\t</layout_l>
\t\t<layout_s width="{ws}" height="{hs}">
\t\t\t<up x="{c1}" y="10" width="{cw}" align="0" show="1"/>
\t\t\t<down x="{c2}" y="10" width="{cw}" align="0" show="1"/>
\t\t</layout_s>
\t</layout>
\t<preview width="250" height="{ph}">
\t\t<l x="0" y="0"/>
\t\t<s x="0" y="{py}"/>
\t</preview>
</root>
"""


def mark() -> Image.Image:
    if hashlib.sha256(MARK.read_bytes()).hexdigest() != MARK_SHA256:
        raise SystemExit(f"{MARK} không khớp SHA-256 bản chính thức")
    im = Image.open(MARK).convert("RGBA")
    return im.crop(im.getbbox())


def wave(x: float, phase: float) -> float:
    """Đường tốc độ cố định trong [0, 1]."""
    v = 0.55 + 0.22 * math.sin(x * 9.0 + phase) + 0.14 * math.sin(x * 23.0 + phase * 2.1) + 0.07 * math.sin(x * 51.0 + phase * 0.7)
    return min(max(v, 0.0), 1.0)


def overlay(w: int, h: int, grid: str, down: str, up: str, logo: Image.Image, logo_alpha: int) -> Image.Image:
    ss = 4                                   # vẽ to rồi thu nhỏ cho mịn
    W, H = w * ss, h * ss
    lay = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    d = ImageDraw.Draw(lay)
    # Lưới mờ
    step = 18 * ss
    for x in range(step, W, step):
        d.line([(x, 6 * ss), (x, H - 6 * ss)], fill=hex_rgb(grid) + (40,), width=ss)
    for y in range(step // 2, H, step // 2 * 2):
        d.line([(6 * ss, y), (W - 6 * ss, y)], fill=hex_rgb(grid) + (40,), width=ss)
    # Hai vùng đồ thị ở nửa dưới (tải xuống cao hơn, tải lên thấp hơn)
    base = H - 5 * ss
    amp = H * 0.34
    for color, phase, scale, alpha in ((down, 0.0, 1.0, 30), (up, 1.7, 0.55, 40)):
        pts = [(x, base - amp * scale * wave(x / W, phase)) for x in range(6 * ss, W - 6 * ss + 1, ss * 2)]
        d.polygon(pts + [(pts[-1][0], base), (pts[0][0], base)], fill=hex_rgb(color) + (alpha,))
        d.line(pts, fill=hex_rgb(color) + (alpha * 3,), width=int(1.2 * ss), joint="curve")
    out = lay.resize((w, h), Image.LANCZOS)
    # Dấu hiệu cú (giữ nguyên hình, không đổi màu; chỉ giảm độ đậm để chữ dễ đọc)
    lh = round(h * 0.42)
    lg = logo.resize((round(logo.width * lh / logo.height), lh), Image.LANCZOS)
    a = lg.split()[3].point(lambda v: v * logo_alpha // 255)
    lg.putalpha(a)
    out.alpha_composite(lg, (w - lg.width - 8, 6))
    return out


def preview(bg: Image.Image, font: ImageFont.FreeTypeFont, color: str, large: bool) -> Image.Image:
    im = bg.copy()
    d = ImageDraw.Draw(im)
    if large:
        rows = [("↑: 1.2 MB/s", COL1, ROWS_L[0]), ("↓: 18.6 MB/s", COL2, ROWS_L[0]),
                ("CPU: 12 %", COL1, ROWS_L[1]), ("RAM: 48 %", COL2, ROWS_L[1]),
                ("GPU: 7 %", COL1, ROWS_L[2]), ("HDD: 3 %", COL2, ROWS_L[2])]
    else:
        rows = [("↑: 1.2 MB/s", COL1, 10), ("↓: 18.6 MB/s", COL2, 10)]
    for text, x, y in rows:
        d.text((x * SCALE, (y + 10) * SCALE), text, font=font, fill=hex_rgb(color) + (255,), anchor="lm")
    return im


def build(name: str, top: str, bottom: str, border: str, text: str, grid: str, down: str, up: str,
          logo_alpha: int, logo: Image.Image, font_path: str) -> None:
    out = SKINS / name
    out.mkdir(parents=True, exist_ok=True)
    font = ImageFont.truetype(font_path, round(12 * SCALE))
    for fname, (w, h), large in (("background_l.png", (W_L, H_L), True), ("background.png", (W_S, H_S), False)):
        pw, ph = round(w * SCALE), round(h * SCALE)
        bg = card(pw, ph, top, bottom, border, radius=12)
        bg.alpha_composite(overlay(pw, ph, grid, down, up, logo, logo_alpha))
        bg.save(out / fname, optimize=True)
        if large:
            SHOTS.mkdir(parents=True, exist_ok=True)
            bg.save(SHOTS / f"{name}.png", optimize=True)
        preview(bg, font, text, large).save(HERE / f"xem-truoc-{name}{'' if large else '-nho'}.png", optimize=True)
    xml = XML.format(text_color=colorref(text), wl=W_L, hl=H_L, ws=W_S, hs=H_S, c1=COL1, c2=COL2, cw=COL_W,
                     r1=ROWS_L[0], r2=ROWS_L[1], r3=ROWS_L[2], ph=H_L + H_S + 8, py=H_L + 6)
    (out / "skin.xml").write_text(xml, encoding="utf-8")
    print("sinh skin", name)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--font", required=True, help="segoeui.ttf (chỉ dùng cho ảnh xem trước)")
    a = ap.parse_args()
    logo = mark()
    # Sáng (token light): bg-surface -> bg-base, viền border-strong, chữ text-primary, đồ thị primary/info
    build("tsudev_DoThi_Sang", "#FFFFFF", "#E3EDF8", "#9FB8D4", "#1C2B3A",
          grid="#9FB8D4", down="#1F6FD1", up="#C2410C", logo_alpha=150, logo=logo, font_path=a.font)
    # Tối (token dark): bg-surface -> bg-base, viền border, chữ text-primary, đồ thị primary/cam
    build("tsudev_DoThi_Toi", "#16263C", "#0F1B2D", "#3F5B80", "#E6EDF5",
          grid="#3F5B80", down="#66A3F2", up="#FE7B2E", logo_alpha=170, logo=logo, font_path=a.font)


if __name__ == "__main__":
    main()
