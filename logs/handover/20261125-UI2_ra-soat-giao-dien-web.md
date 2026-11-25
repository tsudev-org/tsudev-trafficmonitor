# PHIẾU - UI2: rà quét sâu giao diện web + hiện đại hoá (tsudev-trafficmonitor)

- **Mã phiếu**: 20261125-UI2 (phiếu con của `~/projects/logs/handover/20261125-UI2_ra-soat-giao-dien-web-toan-he.md`)
- **Từ**: chủ dự án (25/11/2026), phát bởi phiên `tsudev-3e` - **Đến**: agent của repo `tsudev-trafficmonitor`
- **Trạng thái**: ĐÓNG 25/11/2026 (A + B xong, nghiệm thu production)
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
- 25/11/2026 - tsudev-trafficmonitor-61: **A + B XONG**, deploy + nghiệm thu production.
  - Mục 0: (1) không có fade-in/opacity 0 ở màn đầu - ĐẠT sẵn; (3) không có scroll-reveal - ĐẠT sẵn;
    (2) **LỖI**: font không preload + tiêu đề JetBrains Mono rơi về font KHÔNG CHÂN ⇒ khối hero đổi bề rộng khi font tới
    (quay khung hình 1366 Tối, 150 ms/4 Mbps: CLS 0,0197). Sửa (#95): preload 4 woff2 màn đầu + `fonts-fallback.css`
    (Be Vietnam Pro theo số đo tsudev; JetBrains Mono/Chivo Mono: một họ dự phòng cho MỖI font đơn cách hệ thống -
    Consolas, Menlo/DejaVu Sans Mono, Courier New/Liberation Mono - số đo canvas). Production: CLS 0,00007, khung đầu đúng font.
  - A.2: quét 13 trang + 404 × 360/768/1366 × Sáng/Tối (78 tổ hợp, `quet-giao-dien.cjs` sửa khổ + khoá `tsudev-theme`):
    0 cuộn ngang/tràn/ảnh vỡ/trùng id/lỗi console/HTTP. Soi mắt: **khổ 360 đầu trang dính 3 hàng (~145 px)** ⇒ ≤ 440 px
    bỏ nút "Mã nguồn" ở đầu trang (còn ở chân trang), còn 2 hàng (#95).
  - A.4: không có em-dash; ngày DD/MM/YYYY; dấu tiếng Việt đúng font (subset vietnamese có sẵn). A.5: Tab qua 40 điểm
    dừng × 3 trang đều có vòng focus. A.3: không lỗi console/CSP.
  - B (#96): sync tsudev-standards **v6.0.0** (nền Sáng giấy `#EEF1F5`), theme-color Sáng `#eef1f5`; thẻ: lưới chấm pha
    sắc nhấn mờ dần về phía chữ, thẻ lớn giấy kẻ ô + nền phẳng 4% thay quầng loang, viền ngả màu sản phẩm khi rê;
    chân trang lưới chấm ≤ 15%. Không đổi hex nào trong repo. B.4 font: Be Vietnam Pro tự host có subset vietnamese - ĐẠT sẵn.
  - Phụ: build website đỏ ngẫu nhiên vì API winget-pkgs 403 (không xác thực) ⇒ truyền `GITHUB_TOKEN` (#96).
  - AKB: LES-20261125-1004-cf58 (font dự phòng đơn cách), -4c00 (ghi đè token products.css), lesson GITHUB_TOKEN.
