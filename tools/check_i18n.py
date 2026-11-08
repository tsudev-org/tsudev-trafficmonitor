#!/usr/bin/env python3
"""Kiểm tra chất lượng bản Việt hóa của TrafficMonitor-VN.

Chạy:  python3 tools/check_i18n.py   (từ thư mục gốc repo)

Các kiểm tra:
  1. Vietnamese.ini có đủ mọi key của English.ini (và không thừa key).
  2. Các tham số định dạng (<%1%>, %s, %d...) trong bản dịch khớp với bản tiếng Anh,
     tránh lỗi hiển thị hoặc lỗi định dạng chuỗi khi chạy.
  3. Phần [general] của Vietnamese.ini khớp với DEFAULT_LANGUAGE_CONFIG trong ForkConfig.h.
  4. Mã nguồn C/C++/C# và tệp dự án không còn ký tự tiếng Trung.

Trả về mã thoát 0 nếu đạt, 1 nếu có lỗi.
"""
import os
import re
import sys

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
LANG_DIR = os.path.join(ROOT, "TrafficMonitor", "language")
CJK = re.compile("[一-鿿]")
KEY_RE = re.compile(r"^\s*([A-Za-z0-9_]+)\s*=\s*(.*?)\s*$")
FMT_RE = re.compile(r"<%\d+%>|%(?:\d+\$)?[-+ #0]*\d*(?:\.\d+)?(?:l|ll|h|I64)?[sdifuxXcS]")

errors = []


def load_ini(path):
    data = {}
    section = None
    with open(path, encoding="utf-8-sig") as f:
        for line in f:
            line = line.rstrip("\r\n")
            if line.startswith("["):
                section = line.strip("[] ")
                continue
            if not line or line.lstrip().startswith(";"):
                continue
            m = KEY_RE.match(line)
            if m:
                data[(section, m.group(1))] = m.group(2)
    return data


def check_language_pack():
    en = load_ini(os.path.join(LANG_DIR, "English.ini"))
    vi = load_ini(os.path.join(LANG_DIR, "Vietnamese.ini"))
    en_keys = {k for k in en if k[0] != "general"}
    vi_keys = {k for k in vi if k[0] != "general"}
    for k in sorted(en_keys - vi_keys):
        errors.append(f"Vietnamese.ini thiếu key [{k[0]}] {k[1]}")
    for k in sorted(vi_keys - en_keys):
        errors.append(f"Vietnamese.ini thừa key [{k[0]}] {k[1]} (không có trong English.ini)")
    for k in sorted(en_keys & vi_keys):
        a = sorted(FMT_RE.findall(en[k]))
        b = sorted(FMT_RE.findall(vi[k]))
        if a != b:
            errors.append(f"Tham số định dạng không khớp ở {k[1]}: EN={a} VI={b}")
    print(f"[i18n] English: {len(en_keys)} key, Vietnamese: {len(vi_keys)} key")
    return vi


def check_default_language(vi):
    cfg_path = os.path.join(ROOT, "TrafficMonitor", "ForkConfig.h")
    with open(cfg_path, encoding="utf-8-sig") as f:
        m = re.search(r'#define\s+DEFAULT_LANGUAGE_CONFIG\s+L"([^"]*)"', f.read())
    if not m:
        errors.append("Không tìm thấy DEFAULT_LANGUAGE_CONFIG trong ForkConfig.h")
        return
    unq = lambda s: s.strip().strip('"')
    expected = "|".join(unq(vi.get(("general", k), "")) for k in ("BCP_47", "DISPLAY_NAME", "TRANSLATOR"))
    if m.group(1) != expected:
        errors.append(f"DEFAULT_LANGUAGE_CONFIG='{m.group(1)}' không khớp Vietnamese.ini '{expected}'")


# Tệp được phép chứa chữ Hán: giấy phép gốc (bắt buộc giữ nguyên văn), gói tiếng Nhật (kanji), và chính tệp này.
CJK_ALLOWED = {
    os.path.join("TrafficMonitor", "language", "Japanese.ini"),
    os.path.join("tools", "check_i18n.py"),
}


def read_text(path):
    """Đọc tệp văn bản UTF-8 hoặc UTF-16 (tệp .rc của Visual Studio); trả về None nếu là tệp nhị phân."""
    with open(path, "rb") as f:
        raw = f.read()
    if raw[:2] in (b"\xff\xfe", b"\xfe\xff"):
        return raw.decode("utf-16")
    if path.endswith((".rc", ".rc2")) and raw[1:2] == b"\x00":
        return raw.decode("utf-16-le", errors="replace")
    try:
        return raw.decode("utf-8-sig")
    except UnicodeDecodeError:
        return None


def check_no_chinese_in_code():
    exts = (".cpp", ".h", ".hpp", ".c", ".cs", ".vcxproj", ".filters", ".props", ".yml", ".yaml",
            ".rc", ".rc2", ".md", ".ini", ".xml", ".info", ".txt", ".py", ".sln", ".gitignore", ".gitattributes")
    count = 0
    for dirpath, dirnames, filenames in os.walk(ROOT):
        dirnames[:] = [d for d in dirnames if d not in (".git", "Bin", "Obj", ".vs", ".handoff")]
        for fn in filenames:
            if not fn.endswith(exts):
                continue
            path = os.path.join(dirpath, fn)
            if os.path.relpath(path, ROOT) in CJK_ALLOWED:
                continue
            text = read_text(path)
            if text is None:
                errors.append(f"Tệp không phải UTF-8/UTF-16: {os.path.relpath(path, ROOT)}")
                continue
            for no, line in enumerate(text.splitlines(), 1):
                if CJK.search(line):
                    count += 1
                    if count <= 20:
                        errors.append(f"Còn tiếng Trung: {os.path.relpath(path, ROOT)}:{no}")
    print(f"[i18n] Số dòng mã nguồn và tài liệu còn tiếng Trung: {count}")


def main():
    vi = check_language_pack()
    check_default_language(vi)
    check_no_chinese_in_code()
    if errors:
        print("\n".join("LỖI: " + e for e in errors))
        print(f"[i18n] THẤT BẠI: {len(errors)} lỗi")
        return 1
    print("[i18n] ĐẠT")
    return 0


if __name__ == "__main__":
    sys.exit(main())
