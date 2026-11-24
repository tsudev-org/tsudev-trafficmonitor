# PHIẾU YÊU CẦU - Bản sắc riêng của TsuDEV TrafficMonitor (tsudev-standards v5.0.0)

- **Mã phiếu**: 20261121-STD50
- **Từ**: phiên tsudev-standards, 21/11/2026 - **Đến**: phiên kế tiếp của `tsudev-trafficmonitor`
- **Trạng thái**: MỞ
- **Nguồn**: https://github.com/tsudev-org/tsudev-standards/releases/tag/v5.0.0 - `docs/DESIGN_SYSTEM.md` mục 1.2,
  CHANGELOG 5.0.0 "Hướng dẫn nâng cấp"
- **Bối cảnh**: chủ dự án muốn mỗi sản phẩm có đặc trưng nổi bật riêng (màu, giao diện, bố cục). Trước v5.0.0, 9/11
  sản phẩm dùng cùng một màu xanh. Từ v5.0.0: nền/chữ/trạng thái/tiếp cận là chung; màu nhấn, bố cục, bo góc, hoạ
  tiết, font tiêu đề là riêng, khai ở `.standards/tokens/products/trafficmonitor.json` - KHÔNG tự khai lại tại repo này.

## Bản sắc đã chốt cho `tsudev-trafficmonitor`

| Trường | Giá trị |
| --- | --- |
| `data-product` | `trafficmonitor` |
| Màu chính Tối / Sáng | `#B3A90F` / `#736C01` (hue OKLCH 105; lý do: logo dùng chung con cú TsuDEV - chọn vàng chanh của đèn LED báo lưu lượng) |
| Bố cục `layout` | `trinh-dien` - hero hai cột (chữ trái, ảnh chụp app lớn tràn phải) + khối tính năng dạng bento |
| Bo góc `shape` | `mem` - 6/10/14px, nút 10px |
| Hoạ tiết `motif` | `duong-bieu-do` (tĩnh, ở hero + `og-image`, phủ ≤ 15%) |
| Font tiêu đề | JetBrains Mono (tự host woff2 bộ `vietnamese`, tối đa 2 độ đậm) |

Muốn đổi bất kỳ giá trị nào: PR vào tsudev-standards (sửa `hue`/`layout`... + chạy `gen-product-palette.mjs`), cổng
kiểm chống trùng phải xanh. KHÔNG ghi đè tại repo này.

## Việc cần làm - chia hai PR, mỗi PR một phiên

**PR 1 - màu + hình** (`feat(ui): bản sắc trafficmonitor theo tsudev-standards v5.0.0`):
1. Sync `.standards/` lên v5.0.0 (bước `curl` sync-standards.sh trước, theo `docs/SYNC.md`).
2. Web: nạp `.standards/tokens/tokens.css` rồi `.standards/tokens/products.css`; `<html data-product="trafficmonitor">` (cạnh
   `data-theme`). Desktop/di động: gộp đè `products/trafficmonitor.json` lên `color.light`/`color.dark` lúc build.
3. Xoá bảng màu tự khai (`palette.css`, `extensions.*.json`, dải `--color-*` riêng) và mã màu cứng còn sót.
4. Giá trị đã lưu `warm` đọc thành `light` (khối `warm` đã gỡ khỏi token).
5. Nút dùng `var(--radius-control)`; h1-h3 và số liệu lớn dùng `var(--font-display)`.
6. Ảnh chụp trước/sau, Sáng + Tối, đính vào PR. Cổng kiểm + test xanh.

**PR 2 - bố cục** (`feat(ui): bố cục trinh-dien + hoạ tiết duong-bieu-do`):
1. Dựng phần THÂN trang chủ website / màn hình đầu app theo `trinh-dien`; header, khu tài khoản, chân trang, bộ chọn
   giao diện giữ theo `ECOSYSTEM_IDENTITY.md` mục 4.
2. Hoạ tiết `duong-bieu-do` tĩnh ở hero, vẽ bằng `accent`/`glow`; sinh lại `og-image.png` cùng hoạ tiết.
3. Ảnh chụp trước/sau, Sáng + Tối.

## Việc riêng của repo này

- Logo dùng chung con cú TsuDEV nên màu nhấn là vàng chanh đèn LED lưu lượng (hue 105). Icon khay `MAY` thêm huy hiệu góc tô `primary` (`BRAND_ASSETS.md` mục 12.3).

## Kết quả xử lý

- 24/11/2026 - phiên tsudev-trafficmonitor: PR 1 + PR 2 gộp một PR (`feat/bo-cuc-trinh-dien`) để chạy CI một lần.
  Web: token nạp thẳng từ `.standards/tokens/{tokens,products}.css`, `data-product="trafficmonitor"`, gỡ `palette.css` +
  `tokens.css` v2.0.0 tự chép; nút `--radius-control`, bo góc `mem`; h1-h3 JetBrains Mono 600 tự host (latin,
  latin-ext, vietnamese). Lệch có lý do: chữ số 0 trong `--font-display` lấy từ Chivo Mono (chủ dự án yêu cầu số 0
  trơn 16/11/2026; JetBrains Mono có chấm giữa và bản subset không có tính năng `zero`), `--font-mono` giữ Chivo Mono.
  Bố cục `trinh-dien` (hero hai cột + bento), hoạ tiết `duong-bieu-do` tĩnh ở hero, og-image sinh lại từ
  `products/trafficmonitor.json`. Kiểm: a11y 13 trang × 2 chế độ 0 lỗi, theme 0 lỗi, quét giao diện OK.
  Còn lại (app desktop): huy hiệu góc tô `primary` cho icon khay - gộp với UI1 (phase P8).
