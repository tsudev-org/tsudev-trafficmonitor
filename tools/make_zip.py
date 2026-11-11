#!/usr/bin/env python3
"""Đóng gói zip TẤT ĐỊNH (reproducible) cho bản phát hành - thay Compress-Archive (không tất định).

Cùng nội dung thư mục => cùng từng byte của tệp zip, bất kể máy, thư mục tạm hay thời điểm chạy:
  * thứ tự mục cố định (sắp xếp theo đường dẫn, phân biệt hoa thường, dấu '/');
  * thời gian mọi mục = --epoch (mặc định SOURCE_DATE_EPOCH, thường là thời điểm commit của tag), theo UTC;
  * quyền/thuộc tính cố định (tệp 0644, thư mục 0755), hệ tạo "Unix", không trường mở rộng;
  * nén DEFLATE mức 9 (zlib của Python).

Cách dùng:
  python -I tools/make_zip.py --root <thư mục chứa gốc> --entry tsudev-trafficmonitor --out goi.zip [--epoch 1762700000]
Mục trong zip có dạng "tsudev-trafficmonitor/..." (giống bố cục cũ của Compress-Archive).
"""
from __future__ import annotations

import argparse
import os
import sys
import time
import zipfile
from pathlib import Path

MIN_EPOCH = 315532800  # 1980-01-01: giới hạn dưới của định dạng zip


def zip_time(epoch: int) -> tuple[int, int, int, int, int, int]:
    t = time.gmtime(max(epoch, MIN_EPOCH))
    # Định dạng DOS lưu giây chẵn
    return (t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec - t.tm_sec % 2)


def collect(root: Path, entry: str) -> list[tuple[str, Path]]:
    base = root / entry
    if not base.is_dir():
        raise SystemExit(f"[make_zip] không thấy thư mục {base}")
    items: list[tuple[str, Path]] = []
    for dirpath, dirnames, filenames in os.walk(base):
        dirnames.sort()
        d = Path(dirpath)
        rel_dir = d.relative_to(root).as_posix()
        items.append((rel_dir + "/", d))
        for name in sorted(filenames):
            p = d / name
            if p.is_symlink():
                raise SystemExit(f"[make_zip] không đóng gói liên kết tượng trưng: {p}")
            items.append(((d / name).relative_to(root).as_posix(), p))
    items.sort(key=lambda x: x[0])
    return items


def make_zip(root: Path, entry: str, out: Path, epoch: int) -> None:
    dt = zip_time(epoch)
    items = collect(root, entry)
    tmp = out.with_name(out.name + ".tmp")
    with zipfile.ZipFile(tmp, "w") as zf:
        for arcname, path in items:
            info = zipfile.ZipInfo(arcname, date_time=dt)
            info.create_system = 3  # Unix: thuộc tính cố định, không phụ thuộc hệ điều hành chạy script
            if arcname.endswith("/"):
                info.external_attr = (0o40755 << 16) | 0x10
                info.compress_type = zipfile.ZIP_STORED
                zf.writestr(info, b"")
            else:
                info.external_attr = 0o100644 << 16
                info.compress_type = zipfile.ZIP_DEFLATED
                zf.writestr(info, path.read_bytes(), compress_type=zipfile.ZIP_DEFLATED, compresslevel=9)
    os.replace(tmp, out)


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--root", type=Path, required=True)
    ap.add_argument("--entry", required=True)
    ap.add_argument("--out", type=Path, required=True)
    ap.add_argument("--epoch", type=int, default=None, help="mặc định: biến môi trường SOURCE_DATE_EPOCH")
    args = ap.parse_args()
    epoch = args.epoch
    if epoch is None:
        env = os.environ.get("SOURCE_DATE_EPOCH", "")
        if not env.isdigit():
            print("[make_zip] cần --epoch hoặc SOURCE_DATE_EPOCH (giây Unix)", file=sys.stderr)
            return 2
        epoch = int(env)
    make_zip(args.root, args.entry, args.out, epoch)
    print(f"[make_zip] {args.out} ({args.out.stat().st_size} byte, thời gian mục {time.strftime('%Y-%m-%d %H:%M:%S', time.gmtime(max(epoch, MIN_EPOCH)))} UTC)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
