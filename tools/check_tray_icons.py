"""Kiểm tương phản icon khay (UI1, DESIGN_SYSTEM mục 1: biểu tượng trên nền thật >= 3:1).

Mỗi icon khay được ghép lên màu nền thanh tác vụ mà nó dành cho (Sáng/Tối, Windows 10/11); ở MỌI cỡ (16-64 px):
  - đường bao (điểm đục giáp điểm trong suốt hoặc mép ảnh) có >= NGUONG_BAO điểm đạt >= 3:1 với nền - hình khối của
    icon không chìm vào thanh tác vụ (lỗi cũ: ô xanh đậm trên thanh tác vụ Tối chỉ 0% đường bao đạt);
  - toàn icon có >= NGUONG_NOI_DUNG điểm đục đạt >= 3:1 - chi tiết mặt cú đọc được.
Chỉ dùng thư viện chuẩn (tự giải mã PNG trong .ico) để chạy được trong CI không cài thêm gói.

Chạy: python3 tools/check_tray_icons.py   (mã thoát 1 nếu có icon dưới ngưỡng)
"""
from __future__ import annotations

import struct
import sys
import zlib
from pathlib import Path

GOC = Path(__file__).resolve().parent.parent
RES = GOC / "tsudev-trafficmonitor" / "res"

# Nền thanh tác vụ thật (sRGB): Windows 11 Tối/Sáng (Mica tắt), Windows 10 Tối/Sáng.
NEN_TOI = {"win11-toi": (0x20, 0x20, 0x20), "win10-toi": (0x10, 0x10, 0x10)}
NEN_SANG = {"win11-sang": (0xF3, 0xF3, 0xF3), "win10-sang": (0xEE, 0xEE, 0xEE)}

# Icon nào dùng trên nền nào - khớp bảng kiểu icon trong tools/brand/build_brand.py
# (0/1 taskbar tối, 4/5 taskbar sáng; 1 <-> 5 khi "tự thích ứng"; 2 = viền xanh lơ cho Windows 7/8, xét trên nền tối).
ICON_NEN = {
    "notifyicon.ico": NEN_TOI,
    "notifyicon2.ico": NEN_TOI,
    "notifyicon3.ico": NEN_TOI,
    "notifyicon4.ico": NEN_SANG,
    "notifyicon5.ico": NEN_SANG,
}
NGUONG_TUONG_PHAN = 3.0
NGUONG_BAO = 0.6
NGUONG_NOI_DUNG = 0.2


def doc_ico(du_lieu: bytes) -> list[tuple[int, bytes]]:
    """Trả về [(cỡ, dữ liệu PNG)] của mọi ảnh trong tệp .ico."""
    _, kieu, so = struct.unpack("<HHH", du_lieu[:6])
    if kieu != 1:
        raise ValueError("không phải tệp .ico")
    anh = []
    for i in range(so):
        rong, _, _, _, _, _, co, vt = struct.unpack("<BBBBHHII", du_lieu[6 + 16 * i:22 + 16 * i])
        khoi = du_lieu[vt:vt + co]
        if khoi[:8] != b"\x89PNG\r\n\x1a\n":
            raise ValueError(f"ảnh {rong or 256}px không phải PNG (build_brand.py luôn ghi PNG)")
        anh.append((rong or 256, khoi))
    return anh


def giai_ma_png(png: bytes) -> tuple[int, int, list[tuple[int, int, int, int]]]:
    """Giải mã PNG RGBA/RGB 8 bit, không xen kẽ (đúng loại build_brand.py sinh ra)."""
    vt, idat = 8, b""
    rong = cao = mau = 0
    while vt < len(png):
        dai, loai = struct.unpack(">I4s", png[vt:vt + 8])
        than = png[vt + 8:vt + 8 + dai]
        if loai == b"IHDR":
            rong, cao, sau, mau, _, _, xen = struct.unpack(">IIBBBBB", than)
            if sau != 8 or mau not in (2, 6) or xen:
                raise ValueError(f"PNG chưa hỗ trợ: sâu {sau}, kiểu màu {mau}, xen kẽ {xen}")
        elif loai == b"IDAT":
            idat += than
        vt += 12 + dai
    bpp = 4 if mau == 6 else 3
    tho = zlib.decompress(idat)
    dong = rong * bpp
    truoc = bytearray(dong)
    diem: list[tuple[int, int, int, int]] = []
    for y in range(cao):
        loc = tho[y * (dong + 1)]
        hang = bytearray(tho[y * (dong + 1) + 1:(y + 1) * (dong + 1)])
        for x in range(dong):
            a = hang[x - bpp] if x >= bpp else 0
            b = truoc[x]
            c = truoc[x - bpp] if x >= bpp else 0
            if loc == 1:
                hang[x] = (hang[x] + a) & 0xFF
            elif loc == 2:
                hang[x] = (hang[x] + b) & 0xFF
            elif loc == 3:
                hang[x] = (hang[x] + (a + b) // 2) & 0xFF
            elif loc == 4:
                p = a + b - c
                pa, pb, pc = abs(p - a), abs(p - b), abs(p - c)
                hang[x] = (hang[x] + (a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 0xFF
            elif loc != 0:
                raise ValueError(f"bộ lọc PNG lạ {loc}")
        for x in range(rong):
            px = hang[x * bpp:(x + 1) * bpp]
            diem.append((px[0], px[1], px[2], px[3] if bpp == 4 else 255))
        truoc = hang
    return rong, cao, diem


def do_sang(rgb: tuple[float, float, float]) -> float:
    def kenh(v: float) -> float:
        v /= 255
        return v / 12.92 if v <= 0.04045 else ((v + 0.055) / 1.055) ** 2.4
    r, g, b = (kenh(v) for v in rgb)
    return 0.2126 * r + 0.7152 * g + 0.0722 * b


def tuong_phan(a: tuple[float, float, float], b: tuple[float, float, float]) -> float:
    la, lb = sorted((do_sang(a), do_sang(b)), reverse=True)
    return (la + 0.05) / (lb + 0.05)


def ti_le_dat(rong: int, cao: int, diem: list[tuple[int, int, int, int]], nen: tuple[int, int, int]) -> tuple[float, float]:
    """(tỉ lệ điểm đường bao đạt >= 3:1, tỉ lệ mọi điểm đục đạt >= 3:1) sau khi ghép lên nền."""
    def duc(x: int, y: int) -> bool:
        return 0 <= x < rong and 0 <= y < cao and diem[y * rong + x][3] >= 128

    bao = bao_dat = tong = tong_dat = 0
    for y in range(cao):
        for x in range(rong):
            if not duc(x, y):
                continue
            r, g, b, a = diem[y * rong + x]
            t = a / 255
            ghep = tuple(c * t + n * (1 - t) for c, n in zip((r, g, b), nen))
            dat = tuong_phan(ghep, nen) >= NGUONG_TUONG_PHAN
            tong += 1
            tong_dat += dat
            if not all(duc(x + dx, y + dy) for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1))):
                bao += 1
                bao_dat += dat
    return (bao_dat / bao if bao else 0.0), (tong_dat / tong if tong else 0.0)


def kiem() -> list[str]:
    loi, dong = [], []
    for ten, cac_nen in ICON_NEN.items():
        for co, png in doc_ico((RES / ten).read_bytes()):
            rong, cao, diem = giai_ma_png(png)
            for ten_nen, nen in cac_nen.items():
                tb, tn = ti_le_dat(rong, cao, diem, nen)
                dong.append(f"{ten:16} {co:3}px {ten_nen:11} bao {tb:6.1%}  noi dung {tn:6.1%}")
                if tb < NGUONG_BAO:
                    loi.append(f"{ten} {co}px trên {ten_nen}: đường bao chỉ {tb:.1%} đạt >= 3:1 (cần >= {NGUONG_BAO:.0%})")
                if tn < NGUONG_NOI_DUNG:
                    loi.append(f"{ten} {co}px trên {ten_nen}: nội dung chỉ {tn:.1%} đạt >= 3:1 (cần >= {NGUONG_NOI_DUNG:.0%})")
    print("\n".join(dong))
    return loi


def main() -> int:
    loi = kiem()
    if loi:
        print("\n[tray] LỖI tương phản icon khay:\n  " + "\n  ".join(loi))
        return 1
    print("[tray] ĐẠT - mọi icon khay >= 3:1 trên nền thanh tác vụ thật")
    return 0


if __name__ == "__main__":
    sys.exit(main())
