# PHIẾU BÀN GIAO - Sau P9 phần 3 (tài khoản tsudev đã nối vào app)

- **Mã phiếu**: 20261125-04
- **Từ**: phiên tsudev-trafficmonitor-ca - **Đến**: phiên kế tiếp của repo này
- **Thời điểm**: 25/11/2026 - **Trạng thái**: ĐÓNG (25/11/2026)
- **Nhánh git**: không có nhánh dang dở. Không file nào khoá. Máy chủ dự án đã khôi phục bản 1202 (Run key, %AppData%, không còn credential thử).

## 1. Trạng thái hiện tại

- Bản Latest vẫn **Tsu Tramon `v26.11.2501`**. `main` có thêm (CHƯA phát hành):
  - #99 `tsudev-trafficmonitor/ProAccount.h/.cpp` - `CProAccount`: luồng nền một hàng lệnh (đăng nhập / làm mới / thu hồi),
    phụ thuộc tiêm được (`ProAccountDeps`), không MFC, biên dịch KHÔNG PCH. Test `tests/ProAccountTests.cpp` (68 phép kiểm).
  - #100 thẻ Tùy chọn **Tài khoản tsudev** (`AccountSettingsDlg`), **Chế độ ngoại tuyến hoàn toàn** (`general/offline_mode`,
    chặn `CCommon::GetURL`, `CheckUpdate`, ipify, `CProAccount`), mã cài đặt `[tsudev_account] installation_id`
    (đổi mới khi đăng xuất), khoá `tsu.pro_expired.notified_until`, hộp thoại 7.6 vi/en (TaskDialog) trong
    `CTsudevTrafficMonitorDlg::CheckProRenewal` (chỉ khi không hộp thoại nào mở; ngủ ≥ 30 phút rồi thức = lần mở mới).
  - Tệp đệm trong thư mục cấu hình: `tsudev_entitlement.jws`, `tsudev_jwks.json`. Refresh token: Credential Manager
    `tsudev/tsutramon/refresh_token`.
- Thử thật Windows (bình luận PR #100): đăng nhập thật, làm mới, khởi động lại, đăng xuất, ngoại tuyến - ĐẠT. Hộp thoại 7.6
  CHƯA thấy bằng dữ liệu thật (tài khoản không có Pro).

## 2. Việc còn lại (từ trên xuống)

1. **Theo dõi đầu phiên**: VT arm64ec 2401 + 2501 từ **27/11** (`docs/store/antivirus.md` mục 3; khoá VT là khoá trần,
   đọc `head -c 200 … | tr -d '[:space:]'`); winget #477193; phiếu TM1 scoop-bucket (chưa có `tsutramon.json`).
2. **Phát hành bản kế tiếp có tài khoản tsudev** - HỎI chủ dự án trước (việc khó đảo ngược). Trước khi phát hành: CHANGELOG +
   UpdateLog vi/en, FAQ `Help.md`/`Help_en-us.md` (mục Tài khoản tsudev + Chế độ ngoại tuyến), website (trang tính năng/
   quyền riêng tư nói rõ: chưa đăng nhập không gửi dữ liệu, ngoại tuyến hoàn toàn), giữ `*_Lite.zip` tên cũ tới khi TM1 xong.
   Sau phát hành: thử thật Windows 2501 → bản mới (tự cập nhật).
3. Thử hộp thoại 7.6 bằng dữ liệu thật: cần tài khoản có `pro_until` trong 7 ngày tới (nhờ phiên `tsudev` cấp Pro thử) - hoặc
   chấp nhận test ProRenewal hiện có.
4. **QR1** (đăng nhập QR, `CROSS_DEVICE_LOGIN.md` v1.1.0) + màn Pro/Ủng hộ 13.2 (ADR-0002 việc phát sinh).
5. **P10** (TsuDEV Sync, xuất CSV miễn phí, thông báo nhiều thiết bị 7.3).

## 3. Lưu ý / bẫy

- .vcxproj KHÔNG có `/utf-8`: tệp có tiếng Việt vào build app phải là UTF-8 CÓ BOM (đã thêm cho Pro*.h). AKB LES-20261125-1157-3376.
- Tệp không PCH build riêng trong test cần tự `#pragma comment(lib, "user32.lib")` (lỗi LNK2019 `__imp_PostMessageW`).
- PowerShell 5.1 trên máy chủ dự án hỏng assembly `Microsoft.PowerShell.Commands.Utility` (Add-Type/Select-Object lỗi) ⇒ dùng
  `pwsh.exe` (PowerShell 7, WindowsApps). `taskkill` không `/F` không đóng được TsuTramon.
- Kịch bản thử: `%TEMP%\tsudev-tm-thu\thu_p9.ps1` (saoluu/chay/mo-tab/bam/doc/dong/cred/khoi-phuc), sao lưu ở `p9\saoluu`.

## 4. File liên quan / đang khóa

| Đường dẫn | Lý do | Còn khóa? |
| --- | --- | --- |
| (không) | | Không |

## 5. Kết quả xử lý (mỗi phiên ghi một dòng)

- 25/11/2026 - phiên tsudev-trafficmonitor-62: mục 1 theo dõi (winget OPEN, TM1 chưa có gói, VT chưa tới ngày); mục 2 XONG - phát hành `v26.11.2502` (#104 #105, chủ dự án đồng ý + chấp nhận cờ Malwarebytes arm64ec), website deploy, thử 2501 → 2502 ĐẠT; mục 4 XONG (#103, thử thật ĐẠT); kèm chuẩn hoá tên #102. Mục 3 (7.6 bằng dữ liệu thật) và 5 (P10) chuyển phiếu 20261125-05.
