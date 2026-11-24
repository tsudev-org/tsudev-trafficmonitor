# PHIẾU YÊU CẦU - Hồ sơ trên các nền tảng xác minh (Best Practices, Scorecard, phát hành, kênh gói) cho `tsudev-trafficmonitor`

- **Mã phiếu**: 20261115-01 (phiếu con)
- **Từ**: phiên agent-files (repo `tsufiles`), chủ dự án yêu cầu 15/11/2026 - **Đến**: phiên kế tiếp của repo `tsudev-trafficmonitor`
- **Trạng thái**: MỞ
- **Phiếu mẹ (ĐỌC TRƯỚC, mục 0 nguyên tắc + mục 3 quy trình)**: `~/projects/logs/handover/20261115-01_yeu-cau-ho-so-xac-minh-toan-he.md`
- **Công cụ + mẫu đã đạt passing 100%**: `~/projects/logs/tools/bestpractices/` (`README.md`, `bp.cjs`, `mau-tsufiles.json`);
  mẫu hồ sơ dạng đọc: `~/projects/tsufiles/docs/best-practices.md`.

## 1. Hiện trạng đo 15/11/2026

5 Release, SHA256SUMS + chữ ký đã có. GitHub nhận giấy phép là **NOASSERTION** (Anti-996) - Best Practices hỏi `floss_license_osi`: Anti-996 KHÔNG được OSI duyệt.

## 2. Việc của repo này (theo thứ tự)

- [ ] 2.1. Giấy phép (P1.6): giữ giấy phép gốc theo nghĩa vụ upstream, nhưng ghi TRUNG THỰC trong hồ sơ: `floss_license` Met chỉ khi giấy phép là FLOSS; `floss_license_osi` (SUGGESTED) ghi Unmet + giải thích. Thêm tiêu đề rõ vào `LICENSE` để GitHub hiện tên.
- [ ] 2.2. Best Practices passing: kiểm từng câu, đặc biệt `release_notes`, `vulnerability_report_process`.
- [ ] 2.3. Thêm `scorecard.yml`; theo dõi winget-pkgs #477193 - manifest phải khớp SHA-256 của Release.
- [ ] 2.4. Hồ sơ Best Practices (nếu áp dụng): soạn `docs/best-practices.json` + `.md` (mỗi `Met` có URL bằng chứng trên
  `main`), gộp PR, rồi `bp.cjs new` -> `fill` chạy thử -> xem ảnh -> `fill --submit` -> `inspect` ra passing 100% -> huy hiệu README.
- [ ] 2.5. Ghi kết quả có số đo dưới đây + đánh dấu dòng `tsudev-trafficmonitor` ở mục 4 phiếu mẹ + một dòng mục 7 kế hoạch `20261113-02`.

**Nhắc lại**: không ghi `Met` cho điều repo chưa làm. Chưa đạt thì sửa repo trước, hoặc ghi `Unmet`/`N/A` trung thực.
Tự chứng nhận sai bị phát hiện sẽ làm hỏng hồ sơ SignPath của cả hệ.

## Kết quả xử lý (phiên nhận điền)

-
- 24/11/2026 (P8): rà trung thực TRƯỚC khi tạo hồ sơ Best Practices: tiêu chí `floss_license` là **MUST** (không chỉ `floss_license_osi`). Anti-996 có điều kiện tuân thủ luật lao động - không được OSI duyệt, không trong danh sách FSF ⇒ ghi Met là tự chứng nhận sai; ghi Unmet ⇒ hồ sơ không thể đạt passing. Mà `LICENSE` không được đổi (nghĩa vụ upstream, AGENTS.md B.3). ⇒ KHÔNG tạo hồ sơ (tránh hồ sơ treo dưới 100% gắn vào hệ); 2.1/2.2/2.4 đóng là 'không áp dụng được', 2.3 xong (Scorecard #75; winget #477193 còn OPEN). Chủ dự án muốn vẫn tạo hồ sơ 'in progress' trung thực thì báo phiên sau.
