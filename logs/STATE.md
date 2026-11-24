# STATE.md - Trạng thái project

> **Phiên kế tiếp:** làm phase đầu tiên chưa đánh dấu ở "Hàng đợi task". Phiếu toàn hệ 20261117-P2: đọc `~/projects/logs/handover/20261117-P2_ke-hoach-phan-phoi-tin-cay-da-nen-tang.md` hàng 7 (`tsudev-trafficmonitor`) ở mục 3
> + `.standards/docs/DISTRIBUTION_TRUST.md` (chuẩn v3.7.0) + `.standards/docs/VERSIONING.md` mục 2. Khuôn thẻ/tên tệp mới chỉ áp
> cho bản phát hành KẾ TIẾP, đọc mục 2.4 (updater đã cài) trước khi đổi.

> Agent đọc file này ở đầu mỗi phiên và cập nhật ở cuối mỗi phiên.
> Quy trình: `.standards/docs/AGENT_PROTOCOL.md`.

## Hàng đợi task (làm từ trên xuống)

> **Kế hoạch phase 24/11/2026** (mỗi phase = một PR, một phiên agent nếu ngữ cảnh đầy). Phiếu nguồn ở `logs/handover/`.
> Gom cổng kiểm/CI một lần mỗi PR; website deploy sau khi gộp P2 và P3.

- [x] **P1** - đồng bộ quy ước v5.1.0, cổng Clean Codebase (knip/ruff/vulture), SEC1 ĐÓNG, AV1 sổ `docs/store/antivirus.md`, đối soát phiếu tồn (14-01 mục 2).
- [x] **P2** - website bản sắc màu + hình (STD50 PR 1: `data-product="trafficmonitor"`, `tokens.css` + `products.css`, gỡ `palette.css`, JetBrains Mono tự host, `--radius-control`) + rà quét giao diện 13-07 (`~/projects/logs/tools/quet-giao-dien.cjs`, 390/1280 × Sáng/Tối, có 404). Deploy + quét production.
- [x] **P3** - website bố cục `trinh-dien` + hoạ tiết `duong-bieu-do` tĩnh ở hero + sinh lại `og-image.png` (STD50 PR 2). Deploy.
- [x] **P4** - TC1 bước 1: ADR `docs/adr/0001-ten-san-pham-tsu-tramon.md` (updater đã cài, tên tệp cũ song song, winget id, Plugin API) + V1: test chứng minh updater bản đã cài (26.11.0901-1202) coi `v26.11.DDNN`/tên tệp mới là MỚI HƠN.
- [ ] **P5** - TC1 bước 2 (khảo sát sẵn 24/11/2026 - đọc trước khi làm):
  - Định danh tên cũ đang rải ở `stdafx.h` (`APP_NAME`, `APP_CLASS_NAME`, `TASKBAR_WINDOW_NAME`, `LEGACY_*`), mutex trong `TsudevTrafficMonitor.cpp` (~dòng 1150), `MigrateLegacyAutoRun` + `IsLegacyAutoRunTarget` (~dòng 830-900, chỉ nhận MỘT thế hệ tên cũ), `CCommon::GetAppDataConfigDir` (`Common.cpp` ~699, chép một thư mục cũ), `auto_start_helper.h` (`LEGACY_AUTO_START_TASK_FOLDER`).
  - Cách làm: header logic thuần mới (vd `LegacyNames.h`: danh sách thế hệ `{TrafficMonitor, tsudev-trafficmonitor}` gồm tên Run, thư mục %AppData%, thư mục tác vụ, mutex, lớp cửa sổ, ProductName; hàm chọn nguồn di trú) + test `tests/LegacyNamesTests.cpp` thêm vào job "Kiểm thử C++"; rồi vòng lặp qua danh sách ở 4 chỗ trên.
  - Đổi: exe `TsuTramon.exe` (TargetName trong `.vcxproj`, `release.yml`, `build.yml`, `tools/make_zip.py`, `tools/make_sbom.py`), ProductName/FileDescription trong `.rc` (UTF-16 LE BOM), chuỗi "tsudev-trafficmonitor" trong 20 tệp `language/*.ini`, `ReadMe.txt`, mẫu issue. GIỮ: thư mục mã, tên lớp C++, `ITsudevTrafficMonitor` (ADR-0001 mục 3.5), `ForkConfig.h` tiền tố tải cũ.
  - Việc gốc: đổi tên trong app (Tsu Tramon, `TsuTramon.exe`, `%AppData%\tsutramon`, Run key, di trú + test C++); thử thật Windows bằng build CI (memory `thu-giao-dien-tren-windows`).
- [ ] **P6** - TC1 bước 3-4: website tên mới + `tramon.tsudev.com` (301 từ tên miền cũ), phát hành bản đầu theo khuôn V1 (`v26.11.DDNN`), kiểm SHA256SUMS/attestation/chữ ký/VirusTotal.
- [ ] **P7** - TC1 bước 5-6: đổi tên repo → `tsutramon` (sau phát hành), báo điều phối ghi tên cũ vào danh sách bất khả xâm phạm; phiếu scoop-bucket/linux-packages/tsudev.com; winget (sau khi #477193 gộp).
- [ ] **P8** - PRO1/PRO2: kiểm kê + ADR Pro (bản miễn phí không gửi dữ liệu); nhắc gia hạn 7 ngày (logic thuần + 6 test bắt buộc, hộp thoại vi/en) khi có JWS quyền lợi; QR1: UI1 icon khay ≥ 3:1 + test, màn Ủng hộ 13.2; 13-03/15-01: good first issue + hồ sơ Best Practices trung thực (`floss_license_osi` Unmet).
- [ ] Theo dõi PR winget-pkgs #477193 (`tsudev.TrafficMonitor` 26.11.1001, chờ Microsoft duyệt): phiếu `logs/handover/20261112-03_nhac-theo-doi-winget.md` - kiểm mỗi phiên.
- 👤 Chủ dự án: thay khoá VirusTotal đã lộ (phiếu SEC1 mục 4); gửi báo nhầm Malwarebytes (phiếu AV1, sổ `docs/store/antivirus.md`).
- Deploy website: `XDG_CONFIG_HOME=$HOME/.config/wrangler-tsudev npm run deploy` (24/11/2026 đã deploy #80; nếu báo "Cannot find native binding" thì chạy lại `npm ci`).

## Đang thực hiện

| Task | Agent | Bắt đầu |
| --- | --- | --- |

## Đã hoàn thành (mới nhất trên cùng)

- 24/11/2026 - **P4** (TC1 bước 1 + V1): ADR-0001 đổi tên Tsu Tramon (khảo sát updater bản đã cài: so `<version>`, link giữ tiền tố repo cũ mãi mãi, tệp tên cũ song song một bản); test `TestKhuonV1`.
- 24/11/2026 - **P2 + P3** (STD50 + 13-07): website bản sắc vàng chanh/JetBrains Mono/bo góc mem từ token chuẩn, bố cục trinh-dien + hoạ tiết duong-bieu-do, og-image mới; sửa trùng id `/nhat-ky/`, ngày DD/MM/YYYY.
- 24/11/2026 - **P1**: đồng bộ quy ước v4.2.0 → v5.1.0; cổng Clean Codebase (knip trong website.yml, job "Clean Codebase (Python)" ruff F401/F841 + vulture, phụ thuộc ghim băm `tools/requirements-lint.txt`); bỏ 7 export/2 kiểu thừa ở website, 1 import thừa; `check_i18n.py` bỏ qua `.venv`. Đưa 11 phiếu 13-24/11 vào repo, ĐÓNG 20261111-01, SEC1; sổ antivirus.
- 12/11/2026 - **Phát hành 26.11.1202** (#64, #65; tag ký V26.11.1202-vn, tự duyệt environment bằng `gh api ... pending_deployments`): **thu cửa sổ nổi vào mép màn hình** (`EdgeDock.h` logic thuần + `tests/EdgeDockTests.cpp` 99 phép kiểm có ASan; tay nắm mũi tên `EdgeHandleWnd` GDI+ layered; hoạt ảnh trượt/mờ theo QPC; kéo tay nắm dọc mép; nhớ `edge_dock`/`edge_auto_hide`; bật/tắt trong Cài đặt > Cửa sổ chính và menu Chức năng khác) và **icon khay theo logo mới** (`notifyicon*.ico` sinh bằng `tools/brand/build_brand.py`; 1201 vẫn còn icon sóng cũ). SHA256SUMS, attestation 3 gói, chữ ký manifest ĐẠT; legacy URL chuyển hướng đúng. Thử Windows bằng build CI: mép trái/phải/trên, gợi ý khi kéo, hover + chú thích, bấm trượt lại, kéo dọc mép, khởi động lại nhớ trạng thái ĐẠT (kịch bản `%TEMP%\tsudev-tm-thu\thu_edge.ps1`, `top.ps1`). Máy chủ dự án đã nâng lên 1202 (Run key trỏ thư mục 1202; sao lưu `%TEMP%\tsudev-tm-thu\truoc-nang-cap-1202-*`). Website + Scoop (excavator) lên 1202.
- 12/11/2026 - **Phát hành 26.11.1201** (tag ký V26.11.1201-vn, chủ dự án đẩy tag và duyệt environment): SHA256SUMS, `gh attestation verify` 3 gói, chữ ký ECDSA `version_signed.xml` đều ĐẠT; liên kết owner cũ chuyển hướng đúng. Thử thật trên máy chủ dự án: bản 1001 tự thấy 1201, tự tải, SHA khớp, có MotW; nâng cấp máy lên 1201 - chuyển %AppData%, Run key, lịch sử 5 ngày ĐẠT; hộp Giới thiệu đúng logo/tên/"Ủng hộ tsudev". Scoop: scoop-bucket#1 (extract_dir/shortcut tên mới, CI cài thử thật đạt). Website đã deploy 1201.
- 12/11/2026 - Chuẩn bị 26.11.1201: logo sản phẩm (#57), đổi tên toàn bộ thành tsudev-trafficmonitor + chuyển dữ liệu/tự khởi động bản cũ (#58), gỡ Gitee + ủng hộ cũ (#59), trang `/ung-ho/` riêng (bản tham chiếu tsudev-files, kiểm VietQR + SHA-256) và **sửa lỗi mất bản ghi ngày hôm trước sau nửa đêm** (#61). Thử Windows bằng build CI: chặn chạy song song với bản cũ, chép %AppData%, chuyển Run key đều ĐẠT; lỗi lịch sử tái hiện trên 1001 và hết trên 1201; khôi phục ngày 11/11 bị mất trong dữ liệu thật của chủ máy (sao lưu ở %TEMP%\tsudev-tm-thu\).
- 10/11/2026 - Phiếu `20261110-05`: website đen tuyền + tím kiểu Supabase, bộ chọn Sáng/Tối/Hệ thống ở chân trang (`public/theme.js`, CSP `script-src 'self'`), CI thêm `check:theme`. Chi tiết: mục Kết quả của phiếu.
- 10/11/2026 - Phát hành 26.11.1001 (#45-#48): đọc skin.xml an toàn (`XmlParsing.h`: 1 MB, 4096 phần tử, sâu 16, kẹp giá trị) + test + fuzz; mở đúng thư mục Downloads sau khi tải; **build tái lập** (`/Brepro`, ngày biên dịch = thời điểm commit, `tools/make_zip.py`): exe trong gói phát hành trùng từng byte với build độc lập của `build.yml`. Đã kiểm sha256/provenance/SBOM/chữ ký; thử Windows 0905 -> 1001 (tự tải, SHA khớp), 0904 -> 1001, 1001 báo mới nhất, 12 skin giống ảnh chuẩn
- 09/11/2026 - Phát hành 26.11.0905 (#42, #44): ứng dụng tự tải gói cập nhật, kiểm SHA-256 với manifest đã ký, lưu vào Downloads (không ghi đè, Mark of the Web); test tích hợp tải gói thật trong CI. Thử Windows: build PR #42 tải 0905 thật, SHA khớp; 0904/0901 thấy 0905. Tự duyệt environment `release` bằng `gh api ... pending_deployments` CHẠY ĐƯỢC
- 09/11/2026 - Cảnh báo hạn security.txt hằng tuần (`tools/check_security_txt.py`, #43); gỡ liên kết repo cũ `TrafficMonitor-VN` (#41) trước khi xóa repo cũ (sao lưu cục bộ `~/backups/github/20261109/`)
- 09/11/2026 - Sửa 5 cảnh báo CodeQL JS/Python (#38, #39); website deploy 26.11.0904; ruleset "Bảo vệ main" bắt buộc thêm Kiểm thử C++, Quét secret (gitleaks), Cổng kiểm quy ước (nguồn GitHub Actions) - 8 check
- 09/11/2026 - Phát hành 26.11.0904 (0903 lỗi bước SBOM, không có Release): URL sang tsudev-org (#31), tinyxml2 11.0.0 vá CVE-2024-50614/50615 (#33), ASan + fuzzing + CodeQL đa ngôn ngữ + SBOM SPDX có chứng thực (#32, #35); đã kiểm sha256/provenance/SBOM/chữ ký, thử cập nhật 0901 -> 0904 và 12 skin trên Windows; gitleaks cài cục bộ, hook chặn thật
- 09/11/2026 - Chuyển repo sang Organization `tsudev-org` (phiếu `logs/handover/20261109-01`); updater giữ tương thích bản đã cài (PR #28)
- 09/11/2026 - Kiểm tra khả năng truy cập website bằng axe-core trong CI (WCAG 2.2 AA, sáng + tối); sửa tương phản menu và khối mã cuộn (PR #26)
- 09/11/2026 - Kiểm thử tự động C++ cho kênh cập nhật: `TrafficMonitor/UpdateSecurity.h` + `tests/UpdateSecurityTests.cpp` (73 phép kiểm, job "Kiểm thử C++") (PR #25)
- 09/11/2026 - CI "Chuẩn tsudev": chạy gitleaks bằng bản phát hành kiểm SHA-256 (gitleaks-action bị chính sách Actions chặn) (PR #24)
- 09/11/2026 - Phát hành 26.11.0902 (PR #20-#22): icon cú tsudev, skin đồ thị mạng sáng/tối, gỡ tên dự án gốc; đã kiểm sha256/attestation/chữ ký, thử cập nhật 26.11.0901 -> 26.11.0902 trên Windows, icon khay sáng/tối; website đã deploy
- 09/11/2026 - Tài liệu tiếng Anh, CHANGELOG.md (PR #18); gỡ tên/liên kết dự án gốc, bỏ kiểm tra phiên bản plugin bên thứ ba (PR #19); icon cú tsudev, skin đồ thị mạng (PR #20); áp dụng bộ quy ước v3.1.1
- 09/11/2026 - Phát hành 26.11.0901 (khuôn phiên bản mới)

## Quyết định quan trọng

> - 24/11/2026 - [ADR-0001](../docs/adr/0001-ten-san-pham-tsu-tramon.md): đổi tên Tsu Tramon; link trong manifest giữ tiền tố `tsudev-tsudev/tsudev-trafficmonitor` mãi mãi.
>
> Quyết định kiến trúc lớn thì viết ADR riêng theo `docs/templates/ADR.md` và chỉ
> ghi một dòng tham chiếu ở đây.

- 12/11/2026 - Tên sản phẩm duy nhất **tsudev-trafficmonitor** (thư mục mã, .sln/.vcxproj, exe, %AppData%). Tên cũ chỉ còn ở LICENSE*, câu ghi công dự án gốc (Anti 996), lịch sử CHANGELOG/UpdateLog, hằng LEGACY_* (chuyển dữ liệu bản cũ), mã winget `tsudev.TrafficMonitor`. Logo riêng của sản phẩm (lệch BRAND_ASSETS, AGENTS.md B.4). Ủng hộ: chỉ mở trang https://trafficmonitor.tsudev.com/ung-ho/ (phiếu 20261112-02)
- 10/11/2026 - Dự án giữ **chi phí 0 đồng**: BỎ hẳn kế hoạch ký số Authenticode (mọi chứng thư tin cậy đều tốn phí; SignPath Foundation miễn phí nhưng đòi giấy phép OSI, Anti 996 không thuộc). Không đề xuất lại dịch vụ/công cụ trả phí. Toàn vẹn dựa vào SHA-256, chứng thực build + SBOM, manifest ký ECDSA, build tái lập (SECURITY.md)
- 10/11/2026 - Build tái lập là yêu cầu: bước build/đóng gói mới phải giữ exe và zip tất định (CI build 2 lần so SHA-256); không dùng __DATE__/__TIME__ hay giờ build trong tài nguyên
- 10/11/2026 - ĐÃ XÓA repo `tsudev-tsudev/TrafficMonitor-VN` (sao lưu `~/backups/github/20261109/`; bản V1.86-vn mất kênh thông báo cập nhật) và fork `tsudev-tsudev/winget-pkgs` (fork thừa, tạo lại khi cần nộp manifest)
- 09/11/2026 - Bản phát hành chỉ build từ tag đẩy lên (environment `release` chỉ nhận tag V*); workflow_dispatch không qua được environment. Bước nào của release.yml đổi thì phải có bước thử tương đương trong build.yml (bài học 26.11.0903)
- 09/11/2026 - Repo ở `tsudev-org`. version_signed.xml luôn ghi liên kết theo owner cũ `tsudev-tsudev` (bản đã cài chỉ tin tiền tố đó). KHÔNG BAO GIỜ tạo lại repo tên `tsudev-trafficmonitor` dưới `tsudev-tsudev` (GitHub bỏ chuyển hướng => bản đã cài mất kênh cập nhật)
- 09/11/2026 - Gỡ tên tác giả dự án gốc khỏi repo ở mức tối đa giấy phép Anti 996 cho phép: chỉ giữ trong LICENSE, LICENSE_CN, LICENSE_VI.md, NOTICE.md
- 09/11/2026 - Không kiểm tra phiên bản plugin qua mạng (bỏ nguồn bên thứ ba); giữ API plugin v8
- 09/11/2026 - Người dịch gói vi/en ghi tsudev; gói ngôn ngữ khác để trống người dịch

## Sự cố bảo mật

> Ghi theo `.standards/docs/SECURITY_BASELINE.md` mục 9. Để trống nếu chưa có.

- (chưa có)
