#!/usr/bin/env python3
"""Kiểm tra hạn `Expires` của security.txt (RFC 9116) để không để quá hạn.

Mặc định đọc `website/public/.well-known/security.txt` trong repo và báo lỗi khi:
  * thiếu trường bắt buộc `Contact` hoặc `Expires`, hoặc `Expires` sai định dạng ISO 8601;
  * còn ít hơn --min-days ngày (mặc định 60) tới hạn, hoặc đã quá hạn;
  * hạn xa hơn 1 năm + 31 ngày (RFC 9116 khuyến nghị không quá 1 năm).

--live: tải thêm bản đang triển khai trên website và báo lỗi nếu hạn ở đó cũng sắp hết hoặc khác bản trong repo
(nghĩa là đã gia hạn trong repo nhưng chưa deploy website).

Chạy theo lịch hằng tuần trong `.github/workflows/link-check.yml`.
Gia hạn: sửa dòng `Expires:` trong tệp trên (thường thêm 1 năm), PR, rồi deploy website.
"""
from __future__ import annotations

import argparse
import datetime as dt
import re
import sys
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LOCAL_FILE = ROOT / "website" / "public" / ".well-known" / "security.txt"
LIVE_URL = "https://trafficmonitor.tsudev.com/.well-known/security.txt"


def parse_expires(text: str, source: str, errors: list[str]) -> dt.datetime | None:
    fields: dict[str, list[str]] = {}
    for line in text.splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        m = re.match(r"^([A-Za-z-]+):\s*(.+)$", line)
        if m:
            fields.setdefault(m.group(1).lower(), []).append(m.group(2).strip())
    if not fields.get("contact"):
        errors.append(f"{source}: thiếu trường Contact")
    expires = fields.get("expires", [])
    if len(expires) != 1:
        errors.append(f"{source}: phải có đúng một trường Expires (đang có {len(expires)})")
        return None
    value = expires[0]
    try:
        when = dt.datetime.fromisoformat(value.replace("Z", "+00:00"))
    except ValueError:
        errors.append(f"{source}: Expires sai định dạng ISO 8601: {value}")
        return None
    if when.tzinfo is None:
        errors.append(f"{source}: Expires phải có múi giờ (ví dụ hậu tố Z): {value}")
        return None
    return when


def check(when: dt.datetime, source: str, now: dt.datetime, min_days: int, errors: list[str]) -> None:
    days = (when - now).total_seconds() / 86400
    print(f"[security.txt] {source}: Expires {when.isoformat()} (còn {days:.0f} ngày)")
    if days < 0:
        errors.append(f"{source}: security.txt ĐÃ QUÁ HẠN từ {when.date()}")
    elif days < min_days:
        errors.append(f"{source}: security.txt còn {days:.0f} ngày (< {min_days}) - gia hạn Expires rồi deploy website")
    if days > 366 + 31:
        errors.append(f"{source}: Expires xa hơn 1 năm (RFC 9116 khuyến nghị tối đa khoảng 1 năm)")


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--min-days", type=int, default=60)
    ap.add_argument("--live", action="store_true", help="kiểm cả bản đang triển khai trên website")
    ap.add_argument("--file", type=Path, default=LOCAL_FILE)
    ap.add_argument("--now", help="giả lập thời điểm hiện tại (ISO 8601), dùng để thử")
    args = ap.parse_args()

    now = dt.datetime.fromisoformat(args.now.replace("Z", "+00:00")) if args.now else dt.datetime.now(dt.timezone.utc)
    if now.tzinfo is None:
        now = now.replace(tzinfo=dt.timezone.utc)
    errors: list[str] = []

    local = parse_expires(args.file.read_text(encoding="utf-8"), "repo", errors)
    if local:
        check(local, "repo", now, args.min_days, errors)

    if args.live:
        req = urllib.request.Request(LIVE_URL, headers={"User-Agent": "tsudev-trafficmonitor-security-txt-check"})
        try:
            with urllib.request.urlopen(req, timeout=20) as resp:
                live_text = resp.read(64 * 1024).decode("utf-8", errors="replace")
        except Exception as exc:  # noqa: BLE001 - báo mọi lỗi mạng như nhau
            errors.append(f"website: không tải được {LIVE_URL}: {exc}")
        else:
            live = parse_expires(live_text, "website", errors)
            if live:
                check(live, "website", now, args.min_days, errors)
                if local and live != local:
                    errors.append(f"website: Expires ({live.date()}) khác bản trong repo ({local.date()}) - cần deploy website")

    for e in errors:
        print(f"[security.txt] LỖI: {e}", file=sys.stderr)
    print("[security.txt] " + ("ĐẠT" if not errors else "KHÔNG ĐẠT"))
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
