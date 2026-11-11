# STATE.md - Trạng thái project

> Agent đọc file này ở đầu mỗi phiên và cập nhật ở cuối mỗi phiên.
> Quy trình: `.standards/docs/AGENT_PROTOCOL.md`.

## Hàng đợi task (làm từ trên xuống)

- [ ] Khi nộp winget bản mới (sau khi PR winget-pkgs #477193 của 26.11.1001 được duyệt): mã gói giữ `tsudev.TrafficMonitor`, `RelativeFilePath` đổi thành `tsudev-trafficmonitor\tsudev-trafficmonitor.exe`.
- 12/11/2026 - Đã deploy website (logo, tên mới, /ung-ho/; QR production khớp SHA-256). Deploy: `XDG_CONFIG_HOME=$HOME/.config/wrangler-tsudev npm run deploy`.
- (trống) - việc tùy chọn (chỉ khi người dùng yêu cầu): xem phiếu `logs/handover/20261110-01_ket-phien-1001-va-xoa-repo-cu.md` mục 2

## Đang thực hiện

| Task | Agent | Bắt đầu |
| --- | --- | --- |

## Đã hoàn thành (mới nhất trên cùng)

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
