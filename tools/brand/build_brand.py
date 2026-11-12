#!/usr/bin/env python3
"""Dây chuyền thương hiệu của tsudev-trafficmonitor: MỘT ảnh gốc -> MỌI logo, icon, favicon, ảnh chia sẻ.

Nguồn duy nhất: assets/brand/source/logo-tsudev-trafficmonitor.png (cấu hình: assets/brand/brand.json).
Không sửa tay bất kỳ sản phẩm sinh ra nào; muốn đổi logo thì thay ảnh gốc (và ô cắt trong brand.json) rồi chạy lại:

  python3 -m venv .venv && .venv/bin/pip install "pillow>=11.3" numpy scipy
  .venv/bin/python tools/brand/build_brand.py           # sinh lại toàn bộ
  .venv/bin/python tools/brand/build_brand.py --check   # chỉ kiểm: sản phẩm trên đĩa khớp ảnh gốc hiện tại

Sinh ra (xem assets/brand/README.md):
  assets/brand/logo-tsudev-trafficmonitor.{png,webp,avif}  logo đầy đủ, nền trong suốt, 1024x1024
  assets/brand/mark-tsudev-trafficmonitor.{png,webp,avif}  dấu hiệu thu gọn (mặt cú, không chữ), 512x512
  tsudev-trafficmonitor/res/tsudev-trafficmonitor.ico                    icon exe/cửa sổ/khay kiểu 4: 16..256 (16-40 dùng dấu hiệu thu gọn)
  tsudev-trafficmonitor/res/about_background_hd.bmp               ảnh đầu hộp thoại Giới thiệu 479x92, 24-bit
  tsudev-trafficmonitor/res/notifyicon{,2,3,4,5}.ico              icon vùng thông báo (khay) 16..64, các kiểu chọn trong
                                                                  "Đổi biểu tượng vùng thông báo" (xem make_notify_icons)
  website/public/favicon.ico, favicon-{16,32,96}.png, apple-touch-icon.png, android-chrome[-maskable]-{192,512}.png
  website/public/og-image.png                              ảnh chia sẻ 1200x630
  website/public/brand/{logo,mark}-tsudev-trafficmonitor.{png,webp,avif}

Tách nền: ảnh gốc có nền đen đặc. Vùng tối nối với mép ảnh là nền (lan vùng + phép mở hình thái để cắt khe hẹp
lọt vào trong hình); dải sát mép hình lấy alpha theo độ sáng và gỡ nhân trước với nền đen, nên quầng sáng giữ độ
mềm trên mọi màu nền. Phần tối BÊN TRONG hình (xanh đậm) không bị đụng tới.
"""
from __future__ import annotations

import argparse
import hashlib
import io
import json
import sys
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont
from scipy import ndimage

ROOT = Path(__file__).resolve().parent.parent.parent
BRAND = ROOT / "assets/brand"
FONTS = BRAND / "fonts"
RES = ROOT / "tsudev-trafficmonitor/res"
WEB = ROOT / "website/public"
ICO_SIZES = [16, 20, 24, 28, 32, 40, 48, 64, 128, 256]
FAVICON_SIZES = [16, 24, 32, 48, 64, 128, 256]
SMALL_MAX = 40          # cỡ <= 40 px dùng dấu hiệu thu gọn (chữ "tsudev" của logo không đọc được ở cỡ nhỏ)
TINY_MAX = 20           # cỡ <= 20 px dùng ô cắt chặt hơn (mắt to hơn)
NOTIFY_SIZES = [16, 20, 24, 28, 32, 48, 64]   # icon khay: Windows nạp cỡ 16 x DPI (100%-400%)


def hex_rgb(h: str) -> tuple[int, int, int]:
    h = h.lstrip("#")
    return (int(h[0:2], 16), int(h[2:4], 16), int(h[4:6], 16))


# ---------------------------------------------------------------- tách nền
def _flood_from_border(mask: np.ndarray) -> np.ndarray:
    """Phần của mask (True) nối với mép ảnh (liên thông 4 hướng)."""
    lab, _ = ndimage.label(mask)
    edge = np.unique(np.concatenate([lab[0], lab[-1], lab[:, 0], lab[:, -1]]))
    edge = edge[edge != 0]
    return np.isin(lab, edge)


def remove_background(src: Image.Image, cfg: dict) -> Image.Image:
    a = np.asarray(src.convert("RGB")).astype(np.float32)
    mx = a.max(axis=2)
    bg = _flood_from_border(mx < cfg["seed_max"])
    k = cfg["open_px"]
    opened = Image.fromarray((bg * 255).astype(np.uint8)).filter(ImageFilter.MinFilter(k)).filter(ImageFilter.MaxFilter(k))
    bg = _flood_from_border(np.asarray(opened) > 127)
    band = np.asarray(Image.fromarray((bg * 255).astype(np.uint8)).filter(ImageFilter.MaxFilter(cfg["soft_band_px"]))) > 127
    lo, hi = cfg["soft_lo"], cfg["soft_hi"]
    ramp = np.clip((mx - lo) / (hi - lo), 0, 1)
    alpha = np.ones_like(mx)
    soft = band & (mx < hi)
    alpha[soft] = ramp[soft]
    alpha[bg] = np.minimum(alpha[bg], ramp[bg])
    fg = a.copy()
    vis = alpha > 1 / 255
    fg[vis] = np.clip(a[vis] / alpha[vis, None], 0, 255)   # gỡ nhân trước: pixel = alpha*màu + (1-alpha)*đen
    fg[~vis] = 0
    rgba = np.dstack([fg, alpha * 255]).round().astype(np.uint8)
    return Image.fromarray(rgba, "RGBA")


def trim_square(im: Image.Image, size: int, pad: float = 0.02) -> Image.Image:
    im = im.crop(im.getbbox())
    side = round(max(im.size) * (1 + 2 * pad))
    c = Image.new("RGBA", (side, side), (0, 0, 0, 0))
    c.alpha_composite(im, ((side - im.width) // 2, (side - im.height) // 2))
    return c.resize((size, size), Image.LANCZOS)


def mark_tile(src: Image.Image, box: list[int], radius: float, size: int) -> Image.Image:
    """Dấu hiệu thu gọn: ô vuông bo góc cắt từ ảnh gốc (nền xanh đậm của logo, không chữ)."""
    t = src.convert("RGBA").crop(tuple(box)).resize((size, size), Image.LANCZOS)
    ss = 4
    m = Image.new("L", (size * ss, size * ss), 0)
    ImageDraw.Draw(m).rounded_rectangle((0, 0, size * ss - 1, size * ss - 1), radius=round(size * ss * radius), fill=255)
    t.putalpha(m.resize((size, size), Image.LANCZOS))
    return t


def downscale(im: Image.Image, s: int) -> Image.Image:
    out = im.resize((s, s), Image.LANCZOS)
    if s <= 48:   # bù độ nét bị mất khi thu nhỏ nhiều lần
        out = out.filter(ImageFilter.UnsharpMask(radius=0.6, percent=60, threshold=1))
    return out


def on_solid(im: Image.Image, size: int, color: str, scale: float) -> Image.Image:
    c = Image.new("RGBA", (size, size), hex_rgb(color) + (255,))
    s = round(size * scale)
    c.alpha_composite(downscale(im, s), ((size - s) // 2, (size - s) // 2))
    return c.convert("RGB")


# ---------------------------------------------------------------- ghi tệp tất định
class Out:
    def __init__(self, check: bool) -> None:
        self.check = check
        self.bad: list[str] = []

    def put(self, path: Path, data: bytes) -> None:
        rel = path.relative_to(ROOT).as_posix()
        if self.check:
            if not path.exists() or path.read_bytes() != data:
                self.bad.append(rel)
            return
        path.parent.mkdir(parents=True, exist_ok=True)
        if not path.exists() or path.read_bytes() != data:
            path.write_bytes(data)
            print("sinh", rel, f"({len(data):,} byte)")

    def image(self, path: Path, im: Image.Image, **kw) -> None:
        buf = io.BytesIO()
        fmt = {".png": "PNG", ".webp": "WEBP", ".avif": "AVIF", ".ico": "ICO", ".bmp": "BMP"}[path.suffix]
        if fmt == "PNG":
            kw = {"optimize": True, **kw}
        elif fmt == "WEBP":
            kw = {"quality": 90, "alpha_quality": 100, "method": 6, **kw}   # PNG là bản gốc không mất dữ liệu
        elif fmt == "AVIF":
            kw = {"quality": 90, "speed": 4, **kw}
        im.save(buf, format=fmt, **kw)
        self.put(path, buf.getvalue())

    def triple(self, stem: Path, im: Image.Image) -> None:
        for ext in (".png", ".webp", ".avif"):
            self.image(stem.with_suffix(ext), im)


def font(weight: str, size: int) -> ImageFont.FreeTypeFont:
    return ImageFont.truetype(str(FONTS / f"BeVietnamPro-{weight}.ttf"), size)


# ---------------------------------------------------------------- sản phẩm
def make_ico(out: Out, path: Path, logo: Image.Image, mark: Image.Image, tiny: Image.Image, sizes: list[int]) -> None:
    layers = [downscale(tiny if s <= TINY_MAX else mark if s <= SMALL_MAX else logo, s) for s in sizes]
    big = layers[-1]
    out.image(path, big, sizes=[(s, s) for s in sizes], append_images=layers[:-1])


def _shape_mask(size: int, shape: str, radius: float) -> np.ndarray:
    ss = 4
    m = Image.new("L", (size * ss, size * ss), 0)
    d = ImageDraw.Draw(m)
    if shape == "circle":
        d.ellipse((0, 0, size * ss - 1, size * ss - 1), fill=255)
    else:
        d.rounded_rectangle((0, 0, size * ss - 1, size * ss - 1), radius=round(size * ss * radius), fill=255)
    return np.asarray(m.resize((size, size), Image.LANCZOS)).astype(np.float32) / 255


def owl_circle(face: Image.Image, ring: str) -> Image.Image:
    """Mặt cú cắt tròn, viền màu nhấn của logo."""
    s = face.width
    im = face.convert("RGBA").copy()
    im.putalpha(Image.fromarray((_shape_mask(s, "circle", 0) * 255).round().astype(np.uint8)))
    ss = 4
    r = Image.new("RGBA", (s * ss, s * ss), (0, 0, 0, 0))
    w = round(s * ss * 0.05)
    ImageDraw.Draw(r).ellipse((w // 2, w // 2, s * ss - 1 - w // 2, s * ss - 1 - w // 2), outline=hex_rgb(ring) + (255,), width=w)
    im.alpha_composite(r.resize((s, s), Image.LANCZOS))
    return im


def owl_knockout(face: Image.Image, rgb: tuple[int, int, int]) -> Image.Image:
    """Icon đơn sắc: hình tròn đặc một màu, các nét sáng của mặt cú (mắt, mày, mỏ) khoét trong suốt.

    Giống kiểu icon khay đơn sắc của Windows: nền taskbar lộ qua chỗ khoét nên đọc được ở 16 px trên cả nền sáng lẫn tối.
    """
    a = np.asarray(face.convert("RGB")).astype(np.float32)
    lum = (0.299 * a[..., 0] + 0.587 * a[..., 1] + 0.114 * a[..., 2]) / 255
    feat = np.clip((lum - 0.30) / 0.35, 0, 1)
    alpha = _shape_mask(face.width, "circle", 0) * (1 - feat)
    out = np.zeros((*alpha.shape, 4), np.uint8)
    out[..., :3] = rgb
    out[..., 3] = (alpha * 255).round().astype(np.uint8)
    return Image.fromarray(out, "RGBA")


def make_notify_icons(out: Out, src: Image.Image, cfg: dict, mark: Image.Image, tiny: Image.Image) -> None:
    """Icon vùng thông báo (khay hệ thống). Chỉ số = m_notify_icon_selected (TsudevTrafficMonitorDlg.cpp):

    0 notifyicon.ico   mặc định taskbar tối: mặt cú màu (giống icon ứng dụng ở cỡ nhỏ)
    1 notifyicon2.ico  đơn sắc trắng (taskbar tối)       <-> 5 khi "tự thích ứng" sáng/tối
    2 notifyicon3.ico  mặt cú tròn viền xanh lơ (mặc định Windows 7/8)
    3 (tsudev-trafficmonitor.ico của ứng dụng)
    4 notifyicon4.ico  mặc định taskbar sáng: mặt cú màu  <-> 0
    5 notifyicon5.ico  đơn sắc đen (taskbar sáng)        <-> 1
    """
    def face(box_key: str) -> Image.Image:
        return src.convert("RGB").crop(tuple(cfg[box_key])).resize((512, 512), Image.LANCZOS)

    big, small = face("mark_box"), face("mark_small_box")
    ring = cfg["colors"]["logo_accent"]
    styles = {
        "notifyicon.ico": (mark, tiny),
        "notifyicon4.ico": (mark, tiny),
        "notifyicon2.ico": (owl_knockout(big, (255, 255, 255)), owl_knockout(small, (255, 255, 255))),
        "notifyicon5.ico": (owl_knockout(big, (0, 0, 0)), owl_knockout(small, (0, 0, 0))),
        "notifyicon3.ico": (owl_circle(big, ring), owl_circle(small, ring)),
    }
    for name, (normal, tight) in styles.items():
        layers = [downscale(tight if s <= TINY_MAX else normal, s) for s in NOTIFY_SIZES]
        out.image(RES / name, layers[-1], sizes=[(s, s) for s in NOTIFY_SIZES], append_images=layers[:-1])


def make_about(out: Out, logo: Image.Image, c: dict) -> None:
    w, h = 479, 92
    im = Image.new("RGB", (w, h), hex_rgb(c["about_bg"]))
    mh = 80
    m = downscale(logo, mh)
    x0 = 18
    im.paste(m, (x0, (h - mh) // 2), m)
    d = ImageDraw.Draw(im)
    tx = x0 + mh + 16
    d.text((tx, 42), "tsudev-trafficmonitor", font=font("Bold", 27), fill=hex_rgb(c["about_text"]), anchor="ls")
    small = font("Regular", 15)
    by = "một sản phẩm của hệ sinh thái "
    d.text((tx, 70), by, font=small, fill=hex_rgb(c["about_text"]), anchor="ls")
    bx = tx + d.textlength(by, font=small)
    b = font("Bold", 15)
    d.text((bx, 70), "tsu", font=b, fill=hex_rgb(c["about_text"]), anchor="ls")
    d.text((bx + d.textlength("tsu", font=b), 70), "dev", font=b, fill=hex_rgb(c["about_accent"]), anchor="ls")
    out.image(RES / "about_background_hd.bmp", im)


def make_og(out: Out, logo: Image.Image, c: dict) -> None:
    W, H = 1200, 630
    og = Image.new("RGBA", (W, H), hex_rgb(c["bg"]) + (255,))
    # quầng sáng xanh lơ sau logo (màu của chính logo)
    glow = Image.new("RGBA", (W, H), (0, 0, 0, 0))
    ImageDraw.Draw(glow).ellipse((70, 95, 510, 535), fill=hex_rgb(c["logo_accent"]) + (70,))
    og.alpha_composite(glow.filter(ImageFilter.GaussianBlur(70)))
    ls = 420
    og.alpha_composite(downscale(logo, ls), (80, (H - ls) // 2))
    d = ImageDraw.Draw(og)
    x = 560
    d.text((x, 205), "tsudev-trafficmonitor", font=font("Bold", 50), fill=hex_rgb(c["text"]), anchor="ls")
    reg = font("Regular", 28)
    d.text((x, 262), "Theo dõi tốc độ mạng, CPU, RAM", font=reg, fill=hex_rgb(c["text2"]), anchor="ls")
    d.text((x, 302), "trên thanh tác vụ Windows - tiếng Việt,", font=reg, fill=hex_rgb(c["text2"]), anchor="ls")
    d.text((x, 342), "gia cố bảo mật, miễn phí.", font=reg, fill=hex_rgb(c["text2"]), anchor="ls")
    url = "trafficmonitor.tsudev.com"
    uf = font("SemiBold", 24)
    pw = round(d.textlength(url, font=uf)) + 56
    d.rounded_rectangle((x, 392, x + pw, 452), radius=30, fill=hex_rgb(c["primary"]))
    d.text((x + pw / 2, 422), url, font=uf, fill=hex_rgb(c["on_primary"]), anchor="mm")
    d.text((W - 64, H - 56), "tsudev.com", font=font("SemiBold", 24), fill=hex_rgb(c["text2"]), anchor="rs")
    out.image(WEB / "og-image.png", og.convert("RGB"))


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--check", action="store_true", help="không ghi; báo lỗi nếu sản phẩm trên đĩa khác bản sinh lại")
    a = ap.parse_args()
    cfg = json.loads((BRAND / "brand.json").read_text(encoding="utf-8"))
    src_path = BRAND / cfg["source"]
    src = Image.open(src_path)
    src.load()
    print("nguồn", src_path.relative_to(ROOT).as_posix(), src.size, hashlib.sha256(src_path.read_bytes()).hexdigest()[:16])
    c = cfg["colors"]
    out = Out(a.check)

    cut = remove_background(src, cfg["background"])
    logo = trim_square(cut, 1024)
    mark = mark_tile(src, cfg["mark_box"], cfg["mark_radius"], 512)
    tiny = mark_tile(src, cfg["mark_small_box"], cfg["mark_radius"], 512)

    # bản gốc đã xử lý (dùng cho mọi nơi khác: skin, tài liệu, ảnh đại diện)
    out.triple(BRAND / "logo-tsudev-trafficmonitor.png", logo)
    out.triple(BRAND / "mark-tsudev-trafficmonitor.png", mark)

    # ứng dụng Windows
    make_ico(out, RES / "tsudev-trafficmonitor.ico", logo, mark, tiny, ICO_SIZES)
    make_notify_icons(out, src, cfg, mark, tiny)
    make_about(out, logo, c)

    # website
    make_ico(out, WEB / "favicon.ico", logo, mark, tiny, FAVICON_SIZES)
    for s in (16, 32):
        out.image(WEB / f"favicon-{s}x{s}.png", downscale(tiny if s <= TINY_MAX else mark, s))
    out.image(WEB / "favicon-96x96.png", downscale(logo, 96))
    out.image(WEB / "apple-touch-icon.png", on_solid(logo, 180, c["bg"], 0.88))       # iOS: nền đặc, tự bo góc
    for s in (192, 512):
        out.image(WEB / f"android-chrome-{s}x{s}.png", downscale(logo, s))
        out.image(WEB / f"android-chrome-maskable-{s}x{s}.png", on_solid(logo, s, c["bg"], 0.78))  # vùng an toàn 80%
    out.triple(WEB / "brand/logo-tsudev-trafficmonitor.png", downscale(logo, 512))
    out.triple(WEB / "brand/mark-tsudev-trafficmonitor.png", downscale(mark, 128))
    make_og(out, logo, c)

    if a.check:
        if out.bad:
            print("Sản phẩm thương hiệu KHÔNG khớp ảnh gốc (chạy tools/brand/build_brand.py rồi commit):", file=sys.stderr)
            for b in out.bad:
                print("  -", b, file=sys.stderr)
            return 1
        print("Sản phẩm thương hiệu khớp ảnh gốc.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
