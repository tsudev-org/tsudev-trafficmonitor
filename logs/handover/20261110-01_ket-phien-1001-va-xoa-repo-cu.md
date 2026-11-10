# PHIẾU BÀN GIAO - Kết phiên: 26.11.0905, 26.11.1001, build tái lập, xóa repo cũ

- **Mã phiếu**: 20261110-01
- **Từ**: phiên Claude 09-10/11/2026 - **Đến**: phiên sau (bất kỳ agent nào)
- **Thời điểm**: 09:30 10/11/2026
- **Trạng thái**: HOÀN THÀNH (không có việc dang dở bắt buộc)
- **Nhánh git**: `main` (sạch, 0 PR mở). Bản mới nhất: **26.11.1001** (tag `V26.11.1001-vn`)

## 1. Việc đã làm xong

Toàn bộ kế hoạch 7 mục (`.handoff/HANDOFF-2026-11-11.md`), chi tiết trong `logs/STATE.md`:

- #41 gỡ liên kết repo cũ; #42 + #46 ứng dụng tự tải gói cập nhật và kiểm SHA-256 (`TrafficMonitor/UpdateDownload.h`, test tích hợp `tests/UpdateDownloadTests.cpp` tải gói thật); #43 cảnh báo hạn security.txt (`tools/check_security_txt.py`, link-check.yml hằng tuần); #45 phân tích XML an toàn (`TrafficMonitor/XmlParsing.h`, `tests/XmlParsingTests.cpp`, `tests/fuzz/SkinXmlFuzz.cpp`); #47 build tái lập (`/Brepro`, `print_compile_time.bat` = thời điểm commit, `tools/make_zip.py`, `tools/make_package.ps1`); #44 phát hành 26.11.0905; #48 phát hành 26.11.1001; #49, #50 STATE/AGENTS.
- 26.11.1001: đã kiểm sha256, provenance, SBOM, chữ ký `version_signed.xml`, SHA trong manifest = SHA256SUMS, zip đủ giấy phép + 12 skin; exe 3 kiến trúc trùng từng byte với build độc lập của build.yml.
- Thử Windows: 0905 -> 1001 tự tải, SHA khớp; 0904/0901 thấy bản mới; 1001 báo mới nhất; 12 skin giống ảnh chuẩn. Máy người dùng đã khôi phục (đang chạy 0904, config md5 `caf3c0f5…`).
- Website https://trafficmonitor.tsudev.com đã deploy 26.11.1001 (người dùng chạy).
- Đã XÓA `tsudev-tsudev/TrafficMonitor-VN` và `tsudev-tsudev/winget-pkgs` (sao lưu `~/backups/github/20261109/`). Người dùng đã thu hồi lại quyền `delete_repo` của gh.
- Mục 3 (ký số Authenticode): HOÃN - không có phương án miễn phí (SignPath Foundation đòi giấy phép OSI; Anti 996 không thuộc). Ghi ở SECURITY.md mục "Chữ ký số Authenticode".

## 2. Việc dang dở + bước tiếp theo CỤ THỂ

Không có việc bắt buộc. Việc tùy chọn, chỉ làm khi người dùng yêu cầu:

- [ ] **Ký số Authenticode** - chỉ khi người dùng đồng ý trả phí (Azure Artifact Signing ~10 USD/tháng). Bước: người dùng tạo tài khoản + profile, cấu hình federated credential (OIDC) vào environment `release`, tự thêm action ký của Microsoft vào chính sách Actions; agent thêm bước ký exe trong `release.yml` TRƯỚC `tools/make_package.ps1`, thêm bước thử tương ứng trong `build.yml`, kiểm `signtool verify /pa /v`, cập nhật SECURITY.md. Lưu ý: chữ ký làm exe khác byte so với build.yml - phải sửa bước so SHA (so exe trước khi ký).
- [ ] **Máy người dùng còn chạy 0904**: gói 1001 đã có sẵn ở `C:\Users\devng\Downloads\tsudev-trafficmonitor_26.11.1001_x64_Lite.zip` (SHA `a7adf581…`). Chỉ cập nhật nếu người dùng muốn.
- [ ] **Dọn tàn dư (hỏi người dùng trước khi xóa)**: thư mục thử `%TEMP%\tmvn-pr42`, `tmvn-0901`, `tmvn-0905`, `tmvn-1001`, `tmvn-main`, `tmvn-shots5`, script `%TEMP%\tmvn-*.ps1`; bản clone cục bộ `~/projects/TrafficMonitor-VN` (repo GitHub đã xóa; còn nhánh `docs/da-chuyen-repo` và `.wrangler/` chưa theo dõi).

## 3. File liên quan / đang khóa

| Đường dẫn | Lý do | Còn khóa? |
| --- | --- | --- |
| (không có) | `logs/LOCKS.md` trống | Không |

## 4. Yêu cầu gửi agent đang giữ khóa (nếu có)

- Không có.

## 5. Cảnh báo và quyết định quan trọng

- KHÔNG BAO GIỜ tạo lại repo tên `tsudev-trafficmonitor` dưới `tsudev-tsudev` (mất chuyển hướng -> bản đã cài mất cập nhật). Đã kiểm 10/11: `tsudev-tsudev/tsudev-trafficmonitor/releases/download/...` vẫn 301 sang `tsudev-org`. `version_signed.xml`, `version_utf8.info` vẫn ghi liên kết owner cũ.
- Số phiên bản theo **ngày phát hành thực tế** `{YY}.{M}.{DD}{NN}` (phiên này qua nửa đêm nên 0906 -> 1001). Bản kế tiếp ngày 10/11 là 26.11.1002.
- Tự duyệt environment `release` CHẠY ĐƯỢC: `gh api -X POST repos/tsudev-org/tsudev-trafficmonitor/actions/runs/<id>/pending_deployments -F 'environment_ids[]=26636079552' -f state=approved` (gọi khi job "Tạo GitHub Release" ở trạng thái waiting).
- Build tái lập là yêu cầu: không đưa giờ build/`__DATE__`/`__TIME__` vào exe; bước đóng gói chỉ qua `tools/make_package.ps1`. CI build 2 lần và so SHA-256 - khác là lỗi.
- Hằng số trong header thuần đặt tiền tố riêng (vd `SKIN_MAX_*`): `stdafx.h` có macro `MAX_FONT_SIZE` từng làm hỏng build.
- Script Python in tiếng Việt chạy trên runner Windows phải dùng `python -I -X utf8`.
- Không có MSVC cục bộ: kiểm cú pháp header bằng g++ + header giả lập Windows, còn build thật qua CI. Hàng đợi runner GitHub có lúc chậm 10-20 phút - chờ bằng `gh pr checks <n> --watch`.
- Không thêm `Co-Authored-By: Claude` hay chân trang "Generated with Claude Code" (yêu cầu người dùng).

## 6. Trạng thái cổng kiểm

- [x] `./scripts/check-standards.sh` đạt
- [x] Lint và test đạt (CI main xanh; 0 cảnh báo CodeQL/Dependabot/secret scanning; link-check 0 hỏng; security.txt còn 356 ngày)
- [x] `git status` sạch ngoài phạm vi bàn giao

## 7. Kết quả xử lý (agent nhận điền sau khi thực hiện)

-
