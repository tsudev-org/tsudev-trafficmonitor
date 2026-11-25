# PHIẾU BÀN GIAO - Sau phát hành 26.11.2502 (tài khoản tsudev, màn 13.2, chuẩn hoá tên)

- **Mã phiếu**: 20261125-05
- **Từ**: phiên tsudev-trafficmonitor-62 - **Đến**: phiên kế tiếp của repo này
- **Thời điểm**: 25/11/2026 - **Trạng thái**: MỞ
- **Nhánh git**: không có nhánh dang dở. Không file nào khoá. Máy chủ dự án đã khôi phục bản 1202 (Run key, %AppData%, không còn credential thử).

## 1. Trạng thái hiện tại

- Bản Latest: **Tsu Tramon `v26.11.2502`** (25/11/2026) - tài khoản tsudev tuỳ chọn, Chế độ ngoại tuyến hoàn toàn, màn đăng
  nhập/TsuDEV Pro khung 13.2 (`SignInDlg`), nhắc gia hạn 7.6, bí danh plugin `ITsuTramon`. Vẫn kèm tệp `*_Lite.zip` tên cũ.
- **Thư mục cục bộ đổi thành `~/projects/tsutramon`** (cuối phiên 25/11). AKB GIỮ slug `tsudev-trafficmonitor`
  (`akb link-project tsudev-trafficmonitor ~/projects/tsutramon`) - mọi lệnh akb truyền `--project tsudev-trafficmonitor`.
- Trong repo: thư mục mã `tsutramon/`, `tsutramon.sln`, `tsutramon_Lite.sln`; chuỗi tương thích giữ nguyên theo ADR-0001 mục 5.
- Website tramon.tsudev.com đã deploy bản 2502: trang `/quyen-rieng-tu/` (từ `PRIVACY.md`), `_redirects` 301 cho tên ảnh cũ.

## 2. Việc còn lại (từ trên xuống)

1. **Theo dõi đầu phiên**: VT arm64ec 2401/2501/2502 từ **27/11** (`docs/store/antivirus.md` mục 3, khoá ở
   `~/.config/tsudev/virustotal.env`); winget #477193 (đổi `PackageName` khi gộp); phiếu TM1 scoop-bucket (chưa có `tsutramon.json`
   ⇒ còn giữ `*_Lite.zip`).
2. **BQ1** (`20261125-BQ1`): chủ dự án chốt 25/11 làm ĐÚNG như phiếu - trang Bản quyền: tác giả TsuDEV, đồng tác giả dự án gốc
   TrafficMonitor, contributors (repo gốc + tsutramon, lọc bot), danh sách phiên bản; JSON sinh bằng script + Action hằng tuần;
   giữ NOTICE/LICENSE. Ghi kết quả vào phiếu và báo phiên `tsudev`.
3. **PRO3** (`20261125-PRO3`, quy ước v6.1.0 PRODUCT_POLICY 3.7): Pro là gói tự nguyện ủng hộ tác giả - sync quy ước, sửa câu chữ
   màn Pro (`IDS_PRO_SCREEN_*`), README, không chữ "phi lợi nhuận".
4. Thử hộp thoại 7.6 bằng dữ liệu thật (cần tài khoản có `pro_until` trong 7 ngày - nhờ phiên `tsudev`).
5. **P10** (TsuDEV Sync, xuất CSV miễn phí, thông báo nhiều thiết bị 7.3).
6. Phiếu `tsudev-standards/logs/handover/20261125-TM1b_*` (đường dẫn logo trong bản sắc) - chờ repo đó.

## 3. Lưu ý / bẫy

- Ruleset bắt chữ ký commit: KHÔNG dùng `gh pr update-branch --rebase` (commit GitHub viết lại không ký ⇒ BLOCKED); rebase cục bộ
  rồi `push --force-with-lease` (AKB LES-20261125-1534-64cd).
- Artifact CI đặt tên theo SHA merge của PR (`tsutramon_x64_Lite_<sha merge>`), không theo SHA đầu nhánh.
- Thử Windows: `%TEMP%\tsudev-tm-thu\thu_p9.ps1` (saoluu/chay/mo-tab/bam/doc/khoi-phuc) + `thu_qr1.ps1` (đọc/bấm theo tiêu đề cửa sổ),
  `ds.ps1` (liệt kê cửa sổ của TsuTramon), `yes.ps1 <tên nút>`. Chạy bằng `pwsh.exe`.
- Quét giao diện production: `NODE_PATH=<repo>/website/node_modules node ~/projects/logs/tools/quet-giao-dien.cjs https://tramon.tsudev.com /...`.

## 4. File liên quan / đang khóa

| Đường dẫn | Lý do | Còn khóa? |
| --- | --- | --- |
| (không) | | Không |

## 5. Kết quả xử lý (mỗi phiên ghi một dòng)

-
