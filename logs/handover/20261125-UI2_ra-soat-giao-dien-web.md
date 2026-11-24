# PHIẾU - UI2: rà quét sâu giao diện web + hiện đại hoá (tsudev-trafficmonitor)

- **Mã phiếu**: 20261125-UI2 (phiếu con của `~/projects/logs/handover/20261125-UI2_ra-soat-giao-dien-web-toan-he.md`)
- **Từ**: chủ dự án (25/11/2026), phát bởi phiên `tsudev-3e` - **Đến**: agent của repo `tsudev-trafficmonitor`
- **Trạng thái**: MỞ
- **Phạm vi**: A + B (thuộc hệ sinh thái TsuDEV)
- **Ủy quyền**: chủ dự án cho agent TỰ MỞ TRÌNH DUYỆT (Playwright/Chromium headless) để rà website của repo này.

## Việc cần làm

1. Đọc phiếu toàn hệ (đường dẫn ở trên) - mục 0 là ba nguyên nhân "trang nhúng khi Ctrl+F5" đã gặp ở tsudev.com; kiểm
   cả ba ở repo này TRƯỚC (fade-in ở màn đầu, font không preload/dự phòng lệch số đo, scroll-reveal ẩn trước khi đo).
2. Làm phần A (rà quét sâu bằng trình duyệt: quay khung hình tải lạnh, ảnh 3 khổ × Sáng/Tối, console/mạng, tiếng
   Việt, bàn phím, LCP/CLS/INP).
3. Làm phần B (hiện đại hoá) - thay đổi bảng màu/font CHUNG chờ bản phát hành tsudev-standards, không tự đổi hex.
4. Ghi từng lỗi + cách sửa vào mục Kết quả; sửa theo PR nhỏ; `akb new issue` cho lỗi tốn > 10 phút.

## Kết quả xử lý

- 25/11/2026 - tsudev-3e: mở phiếu.
