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
| 24/11/2026 | `V26.11.1202-vn` | arm64ec (đo lại) | như trên | 1+0/73 | Malwarebytes `Malware.Heuristic.2108` |
| 24/11/2026 | `v26.11.2401` | `tsutramon_26.11.2401_windows-arm64ec.zip` (+ bản sao tên cũ cùng byte) | `44f203c8e0a4c6161cb10aea6bcc65109473ec9e338de7304c5783c3728044aa` | 1+0/73 | Malwarebytes `Malware.Heuristic.2108` |
| 24/11/2026 | `v26.11.2401` | `tsutramon_26.11.2401_windows-x64.zip` | `31c5d70c69a0d3e587f72212ed380beb66ab502523f93fb7a96d08785beec780` | 0+0/73 | - |
| 24/11/2026 | `v26.11.2401` | `tsutramon_26.11.2401_windows-x86.zip` | `372bc87bffa924175e62f343e94a795bc1ec211bab704c8e4f711904af8b5aa8` | 0+0/73 | - |
| 25/11/2026 | `v26.11.2401` | arm64ec (đo lại) | như trên | 1+0/73 | Malwarebytes `Malware.Heuristic.2108` |
| 25/11/2026 | `v26.11.2501` | `tsutramon_26.11.2501_windows-arm64ec.zip` (+ bản sao tên cũ cùng byte) | `27e3af98256374b142e37b27a695963755afe4dcefb00a3c0e9072e353a8d19c` | 1+0/74 | Malwarebytes `Malware.Heuristic.2108` |
| 25/11/2026 | `v26.11.2501` | `tsutramon_26.11.2501_windows-x64.zip` | `3151df0dc8fd942e6f0321401154a7fdf22bcebf2a29498d87ba46da373af878` | 0+0/74 | - |
| 25/11/2026 | `v26.11.2501` | `tsutramon_26.11.2501_windows-x86.zip` | `8aa3237c497c0906ed456020481880e2d90024b3679b3c102f123622fe365b24` | 0+0/74 | - |

Malwarebytes `Malware.Heuristic.2108` là nhận diện heuristic chung (không phải chữ ký mẫu mã độc cụ thể), chỉ trên
bản ARM64EC; bản x64/x86 build từ cùng mã nguồn sạch 0/74.

## 2. Hồ sơ nộp

| Hãng | Kênh | Ngày gửi | Mã hồ sơ / link | Trạng thái |
| --- | --- | --- | --- | --- |
| Malwarebytes | Ticket hỗ trợ qua chatbot https://help.malwarebytes.com/hc/en-us (diễn đàn báo nhầm trả 403 "Request not allowed from this Internet Service Provider" với nhà mạng của máy) | 24/11/2026 | Ticket **8706403** (thư xác nhận về hộp thư liên hệ của dự án) | Chờ phản hồi - cả 2 SHA-256 arm64ec (1202, 2401) |

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

## 4. Quyết định công bố khi có cờ (DISTRIBUTION_TRUST mục 3.1)

- 24/11/2026: job VirusTotal đưa Release `v26.11.2401` về nháp vì cờ Malwarebytes trên arm64ec (cùng nhận diện heuristic
  với bản 1202 đang công khai; x64/x86 sạch). Chủ dự án **chấp nhận** và cho công bố trong phiên; hồ sơ báo nhầm nộp cùng
  ngày (ticket 8706403, trong hạn 48 giờ). Nội dung ticket bị cắt ở 500 ký tự (giới hạn chatbot) nhưng đủ tên nhận diện,
  2 SHA-256, link VirusTotal và Releases; nếu hãng hỏi thêm, trả lời thư xác nhận bằng thư đầy đủ ở mục 2 (đã sửa 2 câu).
