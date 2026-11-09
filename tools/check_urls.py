#!/usr/bin/env python3
"""Kiểm tra URL trong repo tsudev-trafficmonitor.

Chế độ mặc định (chạy trong CI, chặn PR):
  * Lỗi nếu gặp `http://` (trừ namespace XML của MSBuild và mã bên thứ ba được loại trừ).
  * Lỗi nếu URL không thuộc danh sách cho phép (ALLOWED_PREFIXES).
  * Lỗi nếu chuỗi URL trong mã C++ (string literal) nằm ngoài TrafficMonitor/ForkConfig.h -
    mọi URL ứng dụng dùng phải khai báo một chỗ duy nhất. Chú thích C++ (tài liệu tham khảo) không bị kiểm tra.

Chế độ --live (chạy theo lịch hằng tuần, không chặn PR):
  * Gửi yêu cầu tới từng URL cố định (không chứa biến/mẫu) và báo các URL không trả HTTP 2xx/3xx.

Thêm URL mới: bổ sung tiền tố vào ALLOWED_PREFIXES kèm lý do trong chú thích.
"""
from __future__ import annotations

import argparse
import re
import subprocess
import sys
import urllib.error
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Tiền tố URL được phép (chỉ HTTPS). So khớp theo tiền tố chuỗi, phân biệt hoa thường ở phần đường dẫn.
ALLOWED_PREFIXES = [
    # Sản phẩm
    "https://github.com/tsudev-tsudev/tsudev-trafficmonitor",
    "https://github.com/tsudev-org/tsudev-trafficmonitor",   # repo sau khi chuyển sang Organization tsudev-org
    "https://raw.githubusercontent.com/tsudev-tsudev/tsudev-trafficmonitor/",
    "https://trafficmonitor.tsudev.com",
    "https://tsudev.com",
    # Repo cũ (lưu trữ): chỉ để dẫn tới lịch sử commit
    "https://github.com/tsudev-tsudev/TrafficMonitor-VN",
    # Ghi công thư viện bên thứ ba
    "https://github.com/LibreHardwareMonitor/LibreHardwareMonitor",
    "https://github.com/leethomason/tinyxml2",
    # Tra cứu IP công cộng
    "https://api.ipify.org/",
    "https://api6.ipify.org/",
    "https://www.ipify.org/",
    # Tài liệu tham khảo, huy hiệu README
    "https://learn.microsoft.com/",
    "https://docs.microsoft.com/",
    "https://img.shields.io/github/",
    "https://img.shields.io/badge/",
    "https://github.com/github/gitignore",
    "https://github.com/microsoft/Windows-classic-samples",
    "https://cli.github.com/",
    "https://astro.build",
    "https://api.github.com/repos/tsudev-tsudev/tsudev-trafficmonitor/",
]

# http:// duy nhất được chấp nhận: namespace XML (không phải liên kết tải)
ALLOWED_HTTP = ["http://schemas.microsoft.com/developer/msbuild/2003", "http://www.sitemaps.org/schemas/sitemap/0.9",
                "http://localhost:"]

# Tệp/thư mục không kiểm tra: mã và tài liệu bên thứ ba giữ nguyên văn
EXCLUDE = [
    re.compile(r"^LICENSE"),
    re.compile(r"^\.standards/"),          # bản sao chỉ-đọc của bộ quy ước tsudev (không sửa)
    re.compile(r"^TrafficMonitor/tinyxml2/"),
    re.compile(r"^OpenHardwareMonitorApi/LibreHardwareMonitorLib\.xml$"),
    re.compile(r"^tools/check_urls\.py$"),
    re.compile(r"^tests/"),                # kiểm thử cố ý chứa URL giả mạo/không tin cậy để thử bộ lọc
    re.compile(r"^website/package-lock\.json$"),
    re.compile(r"^website/public/fonts/"),
]

TEXT_EXT = {".cpp", ".h", ".hpp", ".c", ".rc", ".rc2", ".ini", ".md", ".info", ".yml", ".yaml",
            ".xml", ".py", ".ps1", ".props", ".vcxproj", ".filters", ".user", ".txt", ".json",
            ".gitignore", ".editorconfig", ".gitattributes", ".astro", ".ts", ".mjs", ".css", ".webmanifest"}
CPP_EXT = {".cpp", ".h", ".hpp", ".c"}
FORK_CONFIG = "TrafficMonitor/ForkConfig.h"

URL_RE = re.compile(r"\bhttps?://[^\s\"'<>()\[\]`|\\]+")
# Chuỗi C/C++ (kể cả tiền tố L/u8/_T(...)) - đủ dùng để tìm URL trong literal
CPP_STRING_RE = re.compile(r'(?:L|u8|u|U)?"((?:[^"\\\n]|\\.)*)"')
TEMPLATE_RE = re.compile(r"[$<>{}]|%[0-9A-Za-z]")


def tracked_files() -> list[str]:
    out = subprocess.run(["git", "ls-files"], cwd=ROOT, capture_output=True, text=True, check=True)
    return [f for f in out.stdout.splitlines() if f]


def read_text(path: Path) -> str | None:
    data = path.read_bytes()
    if data.startswith(b"\xff\xfe") or data.startswith(b"\xfe\xff"):
        return data.decode("utf-16")
    if b"\0" in data[:4096]:
        return None
    return data.decode("utf-8-sig", errors="replace")


def strip_cpp_comments(text: str) -> str:
    """Bỏ chú thích C++ nhưng giữ chuỗi; thay chú thích bằng khoảng trắng để giữ số dòng."""
    out, i, n = [], 0, len(text)
    while i < n:
        c = text[i]
        if c == '"' or c == "'":
            j = i + 1
            while j < n and text[j] != c and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            out.append(text[i:j + 1]); i = j + 1
        elif text.startswith("//", i):
            j = text.find("\n", i); j = n if j < 0 else j
            out.append(" " * (j - i)); i = j
        elif text.startswith("/*", i):
            j = text.find("*/", i + 2); j = n if j < 0 else j + 2
            out.append("".join(ch if ch == "\n" else " " for ch in text[i:j])); i = j
        else:
            out.append(c); i += 1
    return "".join(out)


def clean(url: str) -> str:
    return url.rstrip(".,;:!?*_'")


def is_allowed(url: str) -> bool:
    return any(url.startswith(p) for p in ALLOWED_PREFIXES)


def scan() -> tuple[list[str], set[str]]:
    errors: list[str] = []
    urls: set[str] = set()
    for rel in tracked_files():
        if any(rx.search(rel) for rx in EXCLUDE):
            continue
        path = ROOT / rel
        if path.suffix.lower() not in TEXT_EXT and path.name not in TEXT_EXT:
            continue
        text = read_text(path)
        if text is None:
            continue
        is_cpp = path.suffix.lower() in CPP_EXT
        body = strip_cpp_comments(text) if is_cpp else text
        for lineno, line in enumerate(body.splitlines(), 1):
            for m in URL_RE.finditer(line):
                url = clean(m.group(0))
                if line[m.end():m.end() + 1] in ("<", "{", "$"):
                    continue  # URL mẫu, ví dụ https://github.com/<tài-khoản>/<repo>
                where = f"{rel}:{lineno}"
                if url.startswith("http://"):
                    if not any(url.startswith(a) for a in ALLOWED_HTTP):
                        errors.append(f"{where}: dùng http:// (phải là https://): {url}")
                    continue
                if TEMPLATE_RE.search(url):
                    continue  # URL mẫu/biến, ví dụ https://github.com/$GITHUB_REPOSITORY
                if not is_allowed(url):
                    errors.append(f"{where}: URL ngoài danh sách cho phép: {url}")
                else:
                    urls.add(url)
            # Chỉ áp cho ứng dụng chính; PluginDemo là plugin mẫu độc lập, không dùng ForkConfig.h
            if is_cpp and rel.startswith("TrafficMonitor/") and rel != FORK_CONFIG:
                for s in CPP_STRING_RE.finditer(line):
                    if URL_RE.search(s.group(1)):
                        errors.append(f"{rel}:{lineno}: URL trong mã C++ phải khai báo ở {FORK_CONFIG}: {s.group(0)}")
    return errors, urls


def check_live(urls: set[str]) -> list[str]:
    bad = []
    for url in sorted(urls):
        if "img.shields.io" in url or url.startswith("https://api") or url.endswith("/releases/download/"):
            continue  # huy hiệu, API trả nội dung động, tiền tố tải (không phải trang)
        target = url.split("#", 1)[0]
        req = urllib.request.Request(target, method="GET", headers={"User-Agent": "tsudev-trafficmonitor-link-check"})
        try:
            with urllib.request.urlopen(req, timeout=20) as resp:
                if resp.status >= 400:
                    bad.append(f"{resp.status} {url}")
        except urllib.error.HTTPError as e:
            bad.append(f"{e.code} {url}")
        except Exception as e:  # noqa: BLE001 - báo mọi lỗi mạng
            bad.append(f"ERR {url} ({e.__class__.__name__})")
    return bad


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--live", action="store_true", help="kiểm tra liên kết còn sống (HTTP)")
    args = ap.parse_args()

    errors, urls = scan()
    for e in errors:
        print(f"[url] LỖI {e}")
    print(f"[url] {len(urls)} URL hợp lệ, {len(errors)} lỗi")
    if args.live:
        bad = check_live(urls)
        for b in bad:
            print(f"[url] HỎNG {b}")
        print(f"[url] liên kết hỏng: {len(bad)}")
        return 1 if bad or errors else 0
    print("[url] ĐẠT" if not errors else "[url] KHÔNG ĐẠT")
    return 1 if errors else 0


if __name__ == "__main__":
    sys.exit(main())
