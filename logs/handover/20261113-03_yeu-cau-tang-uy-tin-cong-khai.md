# PHIẾU YÊU CẦU - Tăng uy tín công khai (tsudev-trafficmonitor)

- **Mã phiếu**: 20261113-03
- **Từ**: phiên `tsudev-06` (repo `tsudev`), chủ dự án yêu cầu 13/11/2026 - **Đến**: phiên kế tiếp của repo `tsudev-trafficmonitor`
- **Trạng thái**: MỞ

Kế hoạch đầy đủ (nguyên tắc, giai đoạn, ngưỡng, công cụ đo): `~/projects/logs/handover/20261113-02_ke-hoach-uy-tin-he-sinh-thai.md`.
ĐỌC mục 0 (nguyên tắc - chỉ tín hiệu thật, miễn phí trước) trước khi làm.

## Phần của repo này (mục 4 của kế hoạch)

Ưu tiên 2: P1.3 Scorecard; P1.5 good first issue; P1.6 tệp LICENSE (Anti-996) có tiêu đề rõ để GitHub nhận diện; theo dõi winget-pkgs #477193.

Đo trước/sau: `cd ~/projects/tsudev && npm run he-sinh-thai:tin-hieu`. Xong mục nào thì đánh dấu ở mục 4 + ghi một dòng
vào mục 7 của kế hoạch, và ghi kết quả dưới đây.

## Kết quả xử lý (phiên nhận điền)

- P1.3 Scorecard: XONG - `.github/workflows/scorecard.yml` (#75, 20/11/2026).
- P1.6 tiêu đề `LICENSE`: KHÔNG làm - `AGENTS.md` B.3 cấm sửa `LICENSE`/`LICENSE_CN` (giữ nguyên văn Anti 996 điều 1).
  GitHub nhận NOASSERTION là đúng sự thật (Anti 996 không có mã SPDX/không được OSI duyệt); ghi trung thực ở hồ sơ
  Best Practices (phiếu 20261115-01).
- P1.5 good first issue, Best Practices: xếp phase ở `logs/STATE.md`. winget #477193: còn OPEN (24/11/2026).
- 24/11/2026 (P8): **P1.5 XONG** - 3 issue `good first issue` + `help wanted` có thật: #87 (German, 19 chuỗi), #88 (Japanese, 11), #89 (French, 15) - dịch chuỗi còn thiếu, không cần công cụ build.
