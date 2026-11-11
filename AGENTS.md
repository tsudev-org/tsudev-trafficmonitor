# AGENTS.md - tsudev-trafficmonitor

> **ĐỌC FILE NÀY ĐẦU TIÊN trong mọi phiên làm việc mới.**

## Phần A - Quy ước chung của hệ sinh thái (KHÔNG SỬA Ở ĐÂY)

Toàn bộ quy ước chung nằm trong bản sao chỉ-đọc tại
[`.standards/AGENTS.md`](.standards/AGENTS.md). `MUST` đọc file đó trước.

Bản quy ước repo này đang dùng: xem [`.standards-version`](.standards-version).

| Cần gì | Đọc file nào |
| --- | --- |
| Điểm vào, nguyên tắc chung | `.standards/AGENTS.md` |
| Quy trình phiên, khóa file, bàn giao | `.standards/docs/AGENT_PROTOCOL.md` |
| Bảo mật bắt buộc | `.standards/docs/SECURITY_BASELINE.md` |
| Quy tắc `.gitignore` | `.standards/docs/GITIGNORE_POLICY.md` |
| Nhánh, commit, PR, phát hành | `.standards/docs/GIT_WORKFLOW.md` |
| Phiên bản, CHANGELOG | `.standards/docs/VERSIONING.md` |
| Giao diện và token (website) | `.standards/docs/DESIGN_SYSTEM.md` |
| Logo, favicon, icon ứng dụng | `.standards/docs/BRAND_ASSETS.md` |
| tsudev.com, ảnh đại diện, trang hồ sơ | `.standards/docs/ECOSYSTEM_IDENTITY.md` |
| Cấu trúc thư mục | `.standards/docs/PROJECT_STRUCTURE.md` |
| Chọn ngôn ngữ, framework | `.standards/docs/LANGUAGE_SELECTION.md` |
| Kiểm thử và chất lượng mã | `.standards/docs/TESTING_QUALITY.md` |
| Khả năng truy cập | `.standards/docs/ACCESSIBILITY.md` |

`MUST NOT` sửa bất kỳ file nào trong `.standards/`. Cập nhật bằng `scripts/sync-standards.sh --ref <nhãn>`.

## Phần B - Riêng của repo này

### B.1. Repo này là gì

- **Loại**: phần mềm desktop Windows (C++/MFC) + website sản phẩm tĩnh (Astro, `website/`).
- **Hình trạng**: A (ứng dụng đơn), giữ bố cục thư mục của mã kế thừa - xem B.4.
- **Stack**: C++17/20, MFC, Win32, MSBuild (VS 2022 v143); website Astro + Cloudflare Pages. Chi tiết: [`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md).
- **Mức phân loại dữ liệu cao nhất**: D1 (cấu hình và lịch sử lưu lượng cục bộ trên máy người dùng; không gửi đi đâu).
- **Người liên hệ khi có sự cố**: tsudev (repo thuộc Organization `tsudev-org`, quản trị: `tsudev-tsudev`).

### B.2. Chạy trong 5 phút

```bash
# Kiểm tra nhanh (Linux/WSL): URL, Việt hóa, cổng kiểm quy ước
python3 tools/check_urls.py && python3 tools/check_i18n.py && bash scripts/check-standards.sh
# Build ứng dụng: chỉ qua GitHub Actions (build.yml) hoặc Windows + VS 2022:
#   msbuild tsudev-trafficmonitor_Lite.sln -p:Configuration="Release (lite)" -p:Platform=x64
# Website
cd website && npm ci && npm run build
```

### B.3. Ranh giới sở hữu file

| Vùng | Chủ sở hữu / quy tắc |
| --- | --- |
| `LICENSE`, `LICENSE_CN` | Giữ nguyên văn (giấy phép Anti 996 điều 1), `MUST NOT` sửa |
| `tsudev-trafficmonitor/tinyxml2/`, `OpenHardwareMonitorApi/LibreHardwareMonitorLib.*` | Mã bên thứ ba, giữ nguyên thông báo bản quyền |
| `tsudev-trafficmonitor/ForkConfig.h` | Nơi duy nhất khai URL của ứng dụng (`tools/check_urls.py` canh) |
| `assets/brand/source/` | Ảnh gốc logo (nguồn duy nhất của mọi logo/icon/favicon); đổi logo: xem `assets/brand/README.md` |
| `assets/brand/*.{png,webp,avif}`, `tsudev-trafficmonitor/res/tsudev-trafficmonitor.ico`, `about_background_hd.bmp`, `website/public/` (favicon, icon PWA, og-image, `brand/`) | Sản phẩm sinh từ `tools/brand/build_brand.py`, không sửa tay |
| `tsudev-trafficmonitor/skins/0_tsudev_Cu`, `tsudev_*` | Sinh từ `tools/skin_assets/*.py`, không sửa tay |
| `tools/keys/update_signing_pub.pem` | Khóa công khai; khóa bí mật chỉ ở secret `UPDATE_SIGNING_KEY` (environment `release`) |

### B.4. Điểm lệch chuẩn đã biết

| Lệch ở đâu | Vì sao | Kế hoạch |
| --- | --- | --- |
| PROJECT_STRUCTURE: không có `src/`, `assets/`; mã ở `tsudev-trafficmonitor/`, `include/`, script ở `tools/` | Bản phái sinh của mã C++/MFC có sẵn; dời thư mục làm hỏng `.vcxproj`/`.sln` và khó đối chiếu với mã gốc | Giữ nguyên; script mới đặt trong `tools/` |
| TESTING_QUALITY: test tự động C++ phủ logic thuần của kênh cập nhật (`tests/UpdateSecurityTests.cpp`), tự tải gói + kiểm SHA-256 (`tests/UpdateDownloadTests.cpp`, tải gói phát hành thật) và phân tích XML (`tests/XmlParsingTests.cpp`, mọi skin.xml thật) - kèm ASan và 2 harness fuzz (`tests/fuzz/`); chưa phủ giao diện MFC | Mã giao diện MFC kế thừa khó tách để test; CI còn build `/WX` có chọn lọc + CodeQL + kiểm Việt hóa/URL/chữ bị cắt + build tái lập (build 2 lần, so SHA-256) | Logic thuần mới tách vào header không phụ thuộc MFC (như `UpdateSecurity.h`, `UpdateDownload.h`, `XmlParsing.h`) và thêm test vào `tests/` (job "Kiểm thử C++" trong build.yml) |
| VERSIONING: gói phát hành là `.zip` chạy ngay (không `-setup.exe`), kiến trúc `arm64ec` | Ứng dụng di động (portable), không có bộ cài; build là ARM64EC | Ghi ở README mục Tải về |
| LANGUAGE_SELECTION: C++/MFC | Kế thừa | Không đổi |
| BRAND_ASSETS mục 1, 7: logo, icon, favicon KHÔNG lấy từ dấu hiệu cú chung của repo `tsudev` mà từ logo sản phẩm riêng `assets/brand/source/logo-tsudev-trafficmonitor.png` (có chữ "tsudev"); icon <= 40 px dùng mặt cú cắt từ chính logo này (không chữ) | Chủ dự án chọn logo riêng cho sản phẩm (12/11/2026) | Đã gửi phiếu đề xuất cho `tsudev-standards` cho phép logo riêng từng sản phẩm; dây chuyền: `tools/brand/build_brand.py` |

### B.5. Việc riêng của repo cần biết trước khi sửa

- Giấy phép Anti 996: mọi bản phân phối kèm `LICENSE`, `LICENSE_CN`, `NOTICE.md` (release.yml đóng gói sẵn).
- `tsudev-trafficmonitor/tsudev-trafficmonitor.rc`, `OpenHardwareMonitorApi/app.rc` là **UTF-16 LE có BOM**; script `.ps1` có tiếng Việt lưu UTF-8 có BOM.
- Không push thẳng `main`: nhánh → PR → CI → squash merge. Phát hành: README mục "Phát hành (người bảo trì)".
- Bật hook chặn secret: `git config core.hooksPath .githooks` (cần `gitleaks`).
