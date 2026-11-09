#!/usr/bin/env python3
"""Tạo thông tin phiên bản có chữ ký (version_signed.xml) cho bản phát hành.

  build   - tạo version_signed.xml từ tag, các gói zip trong thư mục dist và nội dung cập nhật trong version_utf8.info
  der2raw - đổi chữ ký ECDSA dạng DER (openssl dgst -sign) sang dạng thô r||s 64 byte, mã hóa base64
            (định dạng mà Windows CNG BCryptVerifySignature dùng)

Quy trình trong release.yml:
  python3 tools/version_manifest.py build --tag V1.88-vn --dist dist --repo owner/repo > dist/version_signed.xml
  openssl dgst -sha256 -sign key.pem -out sig.der dist/version_signed.xml
  python3 tools/version_manifest.py der2raw sig.der > dist/version_signed.xml.sig
  openssl dgst -sha256 -verify tools/keys/update_signing_pub.pem -signature sig.der dist/version_signed.xml
"""
from __future__ import annotations

import argparse
import base64
import hashlib
import re
import sys
from pathlib import Path
from xml.sax.saxutils import escape

ARCHES = {"x86": "", "x64": "_x64", "arm64ec": "_arm64ec"}


def read_contents(info_path: Path) -> tuple[str, str]:
    text = info_path.read_text(encoding="utf-8-sig")
    def node(name: str) -> str:
        m = re.search(rf"<{name}>(.*?)</{name}>", text, re.S)
        return m.group(1).strip() if m else ""
    return node("contents_vi"), node("contents_en")


def build(args: argparse.Namespace) -> int:
    m = re.fullmatch(r"V(\d+(?:\.\d+){0,3})-vn", args.tag)
    if not m:
        print(f"Tag không hợp lệ: {args.tag}", file=sys.stderr)
        return 1
    version = m.group(1)
    dist = Path(args.dist)
    prefix = f"https://github.com/{args.repo}/releases/download/{args.tag}/"
    links, hashes = {}, {}
    for arch, suffix in ARCHES.items():
        name = f"tsudev-trafficmonitor_{version}_{arch}_Lite.zip"  # VERSIONING.md: {ten-app}_{phiên bản}_{arch}
        path = dist / name
        if not path.is_file():
            print(f"Thiếu gói: {path}", file=sys.stderr)
            return 1
        links[suffix] = prefix + name
        hashes[suffix] = hashlib.sha256(path.read_bytes()).hexdigest()
    contents_vi, contents_en = read_contents(Path(args.info))

    lines = ["<root>", f"\t<version>{version}</version>", "\t<GitHub>"]
    for suffix in ARCHES.values():
        lines.append(f"\t\t<link{suffix}>{escape(links[suffix])}</link{suffix}>")
    # Bản phát hành chỉ có bản Lite (không đo nhiệt độ): dùng cùng liên kết cho nhóm *_without_temperature
    for suffix in ARCHES.values():
        lines.append(f"\t\t<link_without_temperature{suffix}>{escape(links[suffix])}</link_without_temperature{suffix}>")
    for suffix in ARCHES.values():
        lines.append(f"\t\t<sha256{suffix}>{hashes[suffix]}</sha256{suffix}>")
    lines += ["\t</GitHub>", "\t<update_contents>",
              f"\t\t<contents_vi>{contents_vi}</contents_vi>",
              f"\t\t<contents_en>{contents_en}</contents_en>",
              "\t</update_contents>", "</root>", ""]
    sys.stdout.buffer.write("\n".join(lines).encode("utf-8"))
    return 0


def der2raw(args: argparse.Namespace) -> int:
    der = Path(args.sig).read_bytes()
    # SEQUENCE { INTEGER r, INTEGER s } - độ dài ngắn (< 128) với P-256
    if len(der) < 8 or der[0] != 0x30 or der[1] != len(der) - 2:
        print("Chữ ký DER không hợp lệ", file=sys.stderr)
        return 1
    pos, parts = 2, []
    for _ in range(2):
        if der[pos] != 0x02:
            print("Chữ ký DER không hợp lệ", file=sys.stderr)
            return 1
        n = der[pos + 1]
        value = der[pos + 2: pos + 2 + n].lstrip(b"\0")
        if len(value) > 32:
            print("Số nguyên quá dài", file=sys.stderr)
            return 1
        parts.append(value.rjust(32, b"\0"))
        pos += 2 + n
    if pos != len(der):
        print("Chữ ký DER thừa dữ liệu", file=sys.stderr)
        return 1
    print(base64.b64encode(parts[0] + parts[1]).decode("ascii"))
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    b = sub.add_parser("build")
    b.add_argument("--tag", required=True)
    b.add_argument("--dist", required=True)
    b.add_argument("--repo", required=True)
    b.add_argument("--info", default="version_utf8.info")
    b.set_defaults(func=build)
    d = sub.add_parser("der2raw")
    d.add_argument("sig")
    d.set_defaults(func=der2raw)
    args = ap.parse_args()
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())
