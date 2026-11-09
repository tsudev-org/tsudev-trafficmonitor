#!/usr/bin/env python3
"""Sinh icon chương trình và ảnh đầu hộp thoại Giới thiệu từ dấu hiệu thu gọn (cú) của tsudev.

Nguồn: website/public/brand/logo-mark.png (512x626) — bản sinh từ dây chuyền packages/brand/build-assets.js
của repo tsudev (BRAND_ASSETS.md mục 1, 7). Script kiểm SHA-256 trước khi dùng; không sửa tay sản phẩm sinh ra.

Sinh ra:
  TrafficMonitor/res/TrafficMonitor.ico     icon exe, cửa sổ, thanh tác vụ, kiểu khay thứ 4 (IDR_MAINFRAME);
                                            nhiều lớp 16/20/24/28/32/40/48/64/128/256
  TrafficMonitor/res/about_background_hd.bmp ảnh đầu hộp thoại Giới thiệu (479x92, 24-bit)

  python3 tools/brand/make_app_icons.py --font C:/Windows/Fonts/segoeui.ttf --font-bold C:/Windows/Fonts/segoeuib.ttf
Cần Pillow.
"""
from __future__ import annotations

import argparse
import hashlib
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parent.parent.parent
SRC = ROOT / "website/public/brand/logo-mark.png"
SRC_SHA256 = "4cb423dc6a3e8d978f80ec7805109c2c4bdaf0a24578dbd6fb2d4a05d0817198"
RES = ROOT / "TrafficMonitor/res"
ICO_SIZES = [16, 20, 24, 28, 32, 40, 48, 64, 128, 256]

# Màu: nền bg-surface (token light), chữ text-primary; "dev" dùng cam #C2410C (BRAND_ASSETS mục 5, đạt 4.68:1 trên nền sáng).
# Nền ảnh trùng màu tô quanh ảnh trong CAboutDlg::OnPaint (ABOUT_BG).
ABOUT_BG = (0xFF, 0xFF, 0xFF)
TEXT_PRIMARY = (0x1C, 0x2B, 0x3A)
TSU = (0x11, 0x35, 0x5A)
DEV = (0xC2, 0x41, 0x0C)


def load_mark() -> Image.Image:
    data = SRC.read_bytes()
    got = hashlib.sha256(data).hexdigest()
    if got != SRC_SHA256:
        raise SystemExit(f"{SRC} không khớp SHA-256 bản chính thức ({got})")
    im = Image.open(SRC).convert("RGBA")
    return im.crop(im.getbbox())


def squarify(im: Image.Image, pad_ratio: float = 0.04) -> Image.Image:
    side = int(max(im.size) * (1 + pad_ratio * 2))
    canvas = Image.new("RGBA", (side, side), (0, 0, 0, 0))
    canvas.paste(im, ((side - im.width) // 2, (side - im.height) // 2), im)
    return canvas


def make_ico(mark: Image.Image) -> None:
    sq = squarify(mark)
    # Pillow ghi .ico từ ảnh lớn nhất, tự thu nhỏ từng lớp bằng LANCZOS
    big = sq.resize((256, 256), Image.LANCZOS)
    out = RES / "TrafficMonitor.ico"
    big.save(out, format="ICO", sizes=[(s, s) for s in ICO_SIZES])
    print("sinh", out.relative_to(ROOT))


def make_about(mark: Image.Image, font_path: str, bold_path: str) -> None:
    w, h = 479, 92
    im = Image.new("RGB", (w, h), ABOUT_BG)
    mh = 76
    m = mark.resize((round(mark.width * mh / mark.height), mh), Image.LANCZOS)
    x0 = 24
    im.paste(m, (x0, (h - mh) // 2), m)
    d = ImageDraw.Draw(im)
    tx = x0 + m.width + 22
    title = ImageFont.truetype(bold_path, 30)
    small = ImageFont.truetype(font_path, 17)
    d.text((tx, 40), "TrafficMonitor-VN", font=title, fill=TEXT_PRIMARY, anchor="ls")
    by = "một sản phẩm của "
    d.text((tx, 70), by, font=small, fill=TEXT_PRIMARY, anchor="ls")
    bx = tx + d.textlength(by, font=small)
    brand = ImageFont.truetype(bold_path, 17)
    d.text((bx, 70), "tsu", font=brand, fill=TSU, anchor="ls")
    d.text((bx + d.textlength("tsu", font=brand), 70), "dev", font=brand, fill=DEV, anchor="ls")
    out = RES / "about_background_hd.bmp"
    im.save(out, format="BMP")
    print("sinh", out.relative_to(ROOT))


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("--font", required=True, help="segoeui.ttf")
    ap.add_argument("--font-bold", required=True, help="segoeuib.ttf")
    a = ap.parse_args()
    mark = load_mark()
    make_ico(mark)
    make_about(mark, a.font, a.font_bold)


if __name__ == "__main__":
    main()
