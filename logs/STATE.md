# STATE.md - Trạng thái project

> **Phiên kế tiếp:** đọc phiếu `logs/handover/20261125-03_ban-giao-sau-ui2-p9-nen.md` TRƯỚC (UI2 xong; P9 phần 1-2 = `ProAuth.h` + `ProNet.h` đã gộp, CHƯA nối app; việc kế: P9 phần 3 nối vào app + thử thật Windows). Repo `tsudev-org/tsutramon`. Việc khó đảo ngược (phát hành, xoá, đổi khoá) hỏi chủ dự án.

> Agent đọc file này ở đầu mỗi phiên và cập nhật ở cuối mỗi phiên.
> Quy trình: `.standards/docs/AGENT_PROTOCOL.md`.

## Hàng đợi task (làm từ trên xuống)

> **Kế hoạch phase 24/11/2026** (mỗi phase = một PR, một phiên agent nếu ngữ cảnh đầy). Phiếu nguồn ở `logs/handover/`.
> Gom cổng kiểm/CI một lần mỗi PR; website deploy sau khi gộp P2 và P3.

- [x] **P1** - đồng bộ quy ước v5.1.0, cổng Clean Codebase (knip/ruff/vulture), SEC1 ĐÓNG, AV1 sổ `docs/store/antivirus.md`, đối soát phiếu tồn (14-01 mục 2).
- [x] **P2** - website bản sắc màu + hình (STD50 PR 1: `data-product="trafficmonitor"`, `tokens.css` + `products.css`, gỡ `palette.css`, JetBrains Mono tự host, `--radius-control`) + rà quét giao diện 13-07 (`~/projects/logs/tools/quet-giao-dien.cjs`, 390/1280 × Sáng/Tối, có 404). Deploy + quét production.
- [x] **P3** - website bố cục `trinh-dien` + hoạ tiết `duong-bieu-do` tĩnh ở hero + sinh lại `og-image.png` (STD50 PR 2). Deploy.
- [x] **P4** - TC1 bước 1: ADR `docs/adr/0001-ten-san-pham-tsu-tramon.md` (updater đã cài, tên tệp cũ song song, winget id, Plugin API) + V1: test chứng minh updater bản đã cài (26.11.0901-1202) coi `v26.11.DDNN`/tên tệp mới là MỚI HƠN.
- [x] **P5** - TC1 bước 2 (#84): đổi tên trong app thành Tsu Tramon - `LegacyNames.h` + `tests/LegacyNamesTests.cpp` (61 phép kiểm), exe `TsuTramon.exe`, `%AppData%\tsutramon`, Run `TsuTramon`, tác vụ `\TsuTramon`, lớp `TsuTramon_r7XZaS4p`; di trú/chặn song song MỌI thế hệ tên cũ. Thử thật Windows (build CI): ĐẠT.
- [x] **P6** (#85): phát hành Tsu Tramon `v26.11.2401` (tệp V1 + bản sao tên cũ), website `tramon.tsudev.com` + 301; SHA256SUMS/attestation/chữ ký ĐẠT; VT arm64ec 1/73 Malwarebytes - chủ dự án chấp nhận, đã công bố.
- [x] **P7** (24/11): repo → `tsudev-org/tsutramon` (4 đường chuyển hướng ĐẠT); phiếu TM1 ở `tsudev` (đã nhắn, việc khẩn: thêm tên cũ vào `~/.claude/CLAUDE.md`) và `scoop-bucket` (gói `tsutramon` + gói chuyển tiếp - checkver cũ `V*-vn` không khớp thẻ mới nên đứng ở 1202, KHÔNG gãy). Còn winget: đổi `PackageName` sau khi #477193 gộp.
- [x] **P8** (24/11, phần làm được khi app chưa có tài khoản): ADR-0002 Pro + kiểm kê (bản miễn phí không gửi dữ liệu); `ProRenewal.h` + 6 ca test 7.6; UI1 icon khay ≥ 3:1 + `tools/check_tray_icons.py`; good first issue #87-#89; Best Practices: KHÔNG tạo hồ sơ (`floss_license` MUST mà Anti-996 không phải FLOSS - phiếu 15-01).
- [x] **SEC2** (25/11, phiếu `20261125-SEC2`): trang `/security/` bằng chứng tự kiểm chứng được (SSL Labs A+, Observatory A+, Scorecard 7.5, VirusTotal, chứng thực build) từ `docs/security/trust.json`; sửa allowlist Actions cho Scorecard.
- [ ] **P9** - phần 1-2 XONG (#97: `ProAuth.h` logic + `ProNet.h` WinHTTP/loopback/Credential Manager, test + fuzz); còn phần 3 nối app (phiếu 20261125-03 mục 2.2). Gốc: đăng nhập OIDC (client trên tsudev.com, PKCE, mã cài đặt 128 bit) + JWS quyền lợi + hộp thoại 7.6 vi/en nối `ProRenewal.h` + màn Pro/Ủng hộ 13.2 + đăng nhập QR (QR1). Cần client OIDC từ phiên `tsudev` trước.
- [ ] **P10** - đồng bộ qua TsuDEV Sync (cấu hình + lịch sử nhiều máy, E2E) + xuất lịch sử CSV (miễn phí) + thông báo nhiều thiết bị 7.3 (5 test).
- [x] **Phát hành 26.11.2501** (25/11, #93, thẻ ký `v26.11.2501`): URL trong app trỏ `tsutramon` (giữ 3 tiền tố tải tin cậy), icon khay P8. VT arm64ec 1/74 Malwarebytes ⇒ chủ dự án chấp nhận, đã công bố Latest; SHA256SUMS/attestation/chữ ký manifest ĐẠT; website deploy.
- [x] **UI2** (25/11, #95 #96, deploy): font màn đầu preload + dự phòng có số đo (CLS 0,0197 → 0,00007), đầu trang 2 hàng ở 360 px, quy ước v6.0.0 nền Sáng giấy, hoạ tiết thẻ/chân trang; quét 78 tổ hợp production 0 lỗi.
- [ ] Phát hành sau: bỏ tệp tên cũ `*_Lite.zip` khi scoop-bucket chuyển sang gói `tsutramon` (phiếu TM1); thử thật Windows bản 2401 tự lên 2501.
- [ ] Theo dõi PR winget-pkgs #477193 (`tsudev.TrafficMonitor` 26.11.1001, chờ Microsoft duyệt): phiếu `logs/handover/20261112-03_nhac-theo-doi-winget.md` - kiểm mỗi phiên.
- 👤 Chủ dự án: thay khoá VirusTotal đã lộ (phiếu SEC1 mục 4).
- [x] Gửi báo nhầm Malwarebytes: ticket **8706403** (24/11, qua chatbot help.malwarebytes.com; diễn đàn chặn ISP 403). [ ] Đo lại VT 27/11-01/12 (`docs/store/antivirus.md` mục 3); hết cờ ⇒ ĐÓNG AV1.
- Deploy website: `XDG_CONFIG_HOME=$HOME/.config/wrangler-tsudev npm run deploy` (24/11/2026 đã deploy #80; nếu báo "Cannot find native binding" thì chạy lại `npm ci`).

## Đang thực hiện

| Task | Agent | Bắt đầu |
| --- | --- | --- |

## Đã hoàn thành (mới nhất trên cùng)

- 25/11/2026 - **UI2 + P9 phần 1-2**: #95 (font/đầu trang), #96 (v6.0.0 + hoạ tiết + `GITHUB_TOKEN` cho build website), deploy + nghiệm thu production; #97 `ProAuth.h` (142 phép kiểm g++ ASan/UBSan + MSVC, ES256 thật bằng BCrypt, fuzz 120 giây) + `ProNet.h` (test loopback/Credential Manager). Client OIDC `tsutramon` có từ 25/11 (tsudev DK3).
- 25/11/2026 - **SEC2 + 26.11.2501**: trang `/security/` (#92; SSL Labs A+ 4/4, Observatory A+ 140, Scorecard 7.5 - workflow trước đó startup_failure vì allowlist Actions của repo chặn `ossf/scorecard-action`, chủ dự án đã thêm); phát hành 26.11.2501 (#93). P9: phiếu DK3 ở tsudev (#382), INSERT client là việc chủ dự án.
- 24/11/2026 - **P8**: ADR-0002 (kiểm kê dữ liệu rời máy: chỉ manifest cập nhật GitHub + ipify khi mở hộp thoại Thông tin mạng; Pro = đồng bộ), `ProRenewal.h` + `tests/ProRenewalTests.cpp` (27 phép kiểm), UI1: viền màu nhấn cho icon khay taskbar Tối + viền sát mép kiểu Win7, `tools/check_tray_icons.py` trong CI; issue #87-#89.
- 24/11/2026 - **P6 + P7**: công bố Release `v26.11.2401` (chủ dự án chấp nhận cờ heuristic Malwarebytes trên arm64ec; x64/x86 0/73); kiểm SHA256SUMS + `gh attestation verify` 6 gói + chữ ký ECDSA manifest ĐẠT; link tải tiền tố `tsudev-tsudev/...` tải đúng SHA trong manifest; website tramon.tsudev.com (Tải về 2401) + 301 từ tên miền cũ giữ đường dẫn/truy vấn. Đổi tên repo `tsudev-org/tsutramon`, kiểm manifest/link tải/raw/API theo tên cũ ĐẠT; tài liệu + website trỏ tên mới (chứng thực bản ≤ 2401 kiểm bằng tên repo cũ). Malwarebytes ticket 8706403. Chưa thử tự cập nhật 1202 → 2401 bằng giao diện trên Windows (đường tải + SHA đã kiểm bằng curl).
- 24/11/2026 - **P5** (TC1 bước 2, #84): đổi tên trong app thành Tsu Tramon + di trú mọi thế hệ tên cũ (`LegacyNames.h`, 61 phép kiểm có ASan); sửa lỗi cũ `GetAutoRun` (gán thay vì so sánh, cắt ký tự cuối đường dẫn). Thử thật trên máy chủ dự án bằng build CI `5f1e748`: 1202 đang chạy ⇒ TsuTramon báo "đang chạy sẵn" (không song song); nâng cấp ⇒ Run `tsudev-trafficmonitor` → `TsuTramon` trỏ exe mới, `%AppData%` chép 4/4 tệp (SHA lịch sử khớp, tổng 2.39 GB giữ nguyên), lớp `TsuTramon_r7XZaS4p`; đã khôi phục 1202 + Run key + xoá `%AppData%\tsutramon` (sao lưu `%TEMP%\tsudev-tm-thu\p5\saoluu`).
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
