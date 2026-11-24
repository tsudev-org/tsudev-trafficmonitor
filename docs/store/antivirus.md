# Sổ antivirus / báo nhầm - tsudev-trafficmonitor

Theo `.standards/docs/DISTRIBUTION_TRUST.md` mục 3, 3.1 và mẫu `.standards/docs/templates/TRUST_TICKET.md` mục D.
Phiếu: `logs/handover/20261122-AV1_bao-nham-antivirus.md`. Thư soạn sẵn cả hệ:
`~/projects/logs/handover/20261122-AV1_bao-nham-antivirus-toan-he.md` mục 3.8 (Malwarebytes).

## 1. Kết quả VirusTotal

| Ngày đo | Bản | Tệp | SHA-256 | Kết quả | Engine gắn cờ |
| --- | --- | --- | --- | --- | --- |
| 22/11/2026 | `V26.11.1202-vn` | `tsudev-trafficmonitor_26.11.1202_arm64ec_Lite.zip` | `11ecdaa0bdd646cd89b6295ba2660722efa0f3e8921b5ab6eb81f2581d429009` | 1+0/74 | Malwarebytes `Malware.Heuristic.2108` |
| 22/11/2026 | `V26.11.1202-vn` | `tsudev-trafficmonitor_26.11.1202_x64_Lite.zip` | `acaf926b3029bb25c3711b3c618ef8cf8e67e597b55ca6bfd815535f32b729ad` | 0+0/74 | - |
| 22/11/2026 | `V26.11.1202-vn` | `tsudev-trafficmonitor_26.11.1202_x86_Lite.zip` | `fe9f0110d5d471844e2ee312ea205cd6d7563aae88c42a208f54122a3c286199` | 0+0/74 | - |
| 24/11/2026 | `V26.11.1202-vn` | arm64ec (đã yêu cầu VT phân tích lại) | như trên | xem mục 3 | |

Malwarebytes `Malware.Heuristic.2108` là nhận diện heuristic chung (không phải chữ ký mẫu mã độc cụ thể), chỉ trên
bản ARM64EC; bản x64/x86 build từ cùng mã nguồn sạch 0/74.

## 2. Hồ sơ nộp

| Hãng | Kênh | Ngày gửi | Mã hồ sơ / link | Trạng thái |
| --- | --- | --- | --- | --- |
| Malwarebytes | Diễn đàn https://forums.malwarebytes.com/forum/122-false-positives/ (👤 cần tài khoản) | | | Chờ chủ dự án gửi |

Kiểm thư soạn sẵn (mục 3.8 phiếu điều phối) ngày 24/11/2026:

- Tên tệp, SHA-256, tên nhận diện, link Release, mô tả hành vi ("network/CPU/memory monitor in the taskbar") ĐÚNG.
- Câu "not yet Authenticode-signed (SignPath Foundation application pending)" KHÔNG đúng với repo này: dự án đã bỏ ký
  số Authenticode (chi phí 0 đồng; SignPath Foundation đòi giấy phép OSI, Anti 996 không thuộc - `logs/STATE.md` mục
  Quyết định 10/11/2026). Khi gửi riêng cho sản phẩm này, thay bằng: "They are not Authenticode-signed; integrity is
  provided by SHA256SUMS, SLSA build provenance and reproducible builds (the exe can be rebuilt byte-for-byte)."
- Câu "Each repository has PRIVACY and SECURITY policies": repo có `SECURITY.md`, chưa có `PRIVACY.md` (ứng dụng không
  gửi dữ liệu đi đâu - D1 cục bộ). Bỏ chữ "PRIVACY" hoặc chỉ ghi "SECURITY policy".

## 3. Theo dõi

- Đo lại 3-7 ngày sau khi gửi: `curl -H "x-apikey: <khoá>" https://www.virustotal.com/api/v3/files/<sha256>` (khoá
  đọc thẳng từ `~/.config/tsudev/virustotal.env`, không in ra). Hết cờ ⇒ ghi ngày gỡ vào bảng mục 1.
- Bản phát hành sau: `release.yml` đã có job VirusTotal dùng chung (#76). Có cờ mới ⇒ quét riêng exe/zip và bản
  trước làm đối chứng trước khi đổi cách đóng gói.
