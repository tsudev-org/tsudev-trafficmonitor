# PHIẾU YÊU CẦU AV1 - Antivirus / báo nhầm (tsudev-trafficmonitor)

- **Mã phiếu**: 20261122-AV1
- **Thời điểm**: 22/11/2026
- **Người gửi**: agent-tsuswico (phiên tsuswico-6e) theo uỷ quyền của chủ dự án
- **Phiếu điều phối**: `~/projects/logs/handover/20261122-AV1_bao-nham-antivirus-toan-he.md` (bảng kết quả cả hệ + thư soạn sẵn từng hãng)
- **Chuẩn**: `.standards/docs/DISTRIBUTION_TRUST.md` mục 3, 3.1
- **Trạng thái**: MỞ

## 1. Kết quả VirusTotal 22/11/2026 - bản Latest `V26.11.1202-vn`

| Tệp | Kết quả | Engine |
| --- | --- | --- |
| `tsudev-trafficmonitor_26.11.1202_arm64ec_Lite.zip` | 1+0/74 | Malwarebytes `Malware.Heuristic.2108` |
| `tsudev-trafficmonitor_26.11.1202_x64_Lite.zip` | 0+0/74 | - |
| `tsudev-trafficmonitor_26.11.1202_x86_Lite.zip` | 0+0/74 | - |

## 2. Việc cần làm

1. Tạo/cập nhật `docs/store/antivirus.md` (mẫu `.standards/docs/templates/TRUST_TICKET.md` mục D): tệp, SHA-256, engine, ngày.
2. Kiểm phần của repo này trong thư soạn sẵn (phiếu điều phối mục 3): mô tả sản phẩm đúng hành vi thật, link Release đúng.
3. 👤 Chủ dự án gửi thư/biểu mẫu (agent không có hộp thư/tài khoản). Ghi ngày gửi + mã hồ sơ vào sổ.
4. Đo lại VT 3-7 ngày sau (`~/.config/tsudev/virustotal.env`, API `/api/v3/files/<sha256>`); hết cờ ⇒ ghi ngày gỡ.
5. Bản phát hành sau: Release nháp → job VirusTotal → sạch mới công bố; cờ ⇒ quét riêng exe/zip + bản trước làm đối chứng trước khi đổi đóng gói (bài học tsuswico ISS-20261122-2153-055f).
- Malwarebytes `Malware.Heuristic.2108` với bản arm64ec: nộp qua diễn đàn Malwarebytes (👤 tài khoản). Repo CHƯA có job VirusTotal trong workflow phát hành ⇒ thêm (`tsudev-org/.github/.github/workflows/virustotal.yml`).

## 3. Khép phiếu

Đổi Trạng thái → ĐÓNG khi mọi cờ đã gỡ (hoặc 👤 chấp nhận) và sổ đã ghi.

## 4. Kết quả xử lý

- 24/11/2026 - phiên tsudev-trafficmonitor: tạo sổ `docs/store/antivirus.md` (tệp, SHA-256, engine, ngày). Kiểm thư mục
  3.8: tên tệp/SHA/link/mô tả đúng; 2 câu cần sửa khi gửi cho sản phẩm này (SignPath - repo đã bỏ Authenticode; PRIVACY -
  repo chưa có `PRIVACY.md`), ghi ở sổ mục 2. Đo VT 24/11: kết quả lưu vẫn là 22/11 (arm64ec 1/74 Malwarebytes, x64/x86
  0/74); đã yêu cầu VT phân tích lại. Job VirusTotal trong `release.yml` đã có (#76). Còn 👤: gửi Malwarebytes.
