# PHIẾU YÊU CẦU QR1 + UI1 + DK2 - tsudev-trafficmonitor

- **Mã phiếu**: 20261124-QR1 (bản sao cho repo) - **Từ**: điều phối `projects-71`, 24/11/2026 - **Trạng thái**: MỞ
- **Phiếu gốc** (đọc phần liên quan): `~/projects/logs/handover/20261124-QR1_dang-nhap-qr-giao-dien-thong-nhat-toan-he.md`,
  `20261124-UI1_bieu-tuong-kho-thay-toan-he.md`, `20261124-DK2_dang-ky-bct-tin-nhiem-mang-chong-lua-dao.md`.
- **Chuẩn**: tsudev-standards **v5.1.0** - `CROSS_DEVICE_LOGIN.md`, `AUTH_AND_ACCOUNT.md` 3.0/13.2/13.4,
  `DESIGN_SYSTEM.md` 1 (biểu tượng trên viên chỉ báo), `LANGUAGE_SELECTION.md` 5.1, `PROJECT_STRUCTURE.md` 3.1,
  `BRAND_ASSETS.md` 12.3.
- **Hạng / loại**: C - Desktop C++

## Việc của repo này

1. Màn Ủng hộ theo 13.2; **UI1** (biểu tượng khay/thanh tác vụ ở chế độ Tối)
2. **STD-SYNC** trong PR kế tiếp (không PR riêng): sync v5.1.0, bật cổng Clean Codebase (mã chết/phụ thuộc thừa) và
   ngân sách tốc độ; liên kết về tsudev.com kèm dấu hiệu TsuDEV + favicon đủ bộ.
3. Rà **UI1** cả Sáng/Tối: biểu tượng trên nền thật ở mọi trạng thái ≥ 3:1, thêm test chặn tái phát (nếu có giao diện).

Repo có phiên riêng ⇒ phiên đó làm; ghi kết quả một dòng dưới đây + dòng tương ứng ở phiếu gốc; `akb` theo chuẩn.

## Kết quả xử lý

- 24/11/2026 - projects-71: mở phiếu.
- 24/11/2026 - phiên tsudev-trafficmonitor: STD-SYNC v5.1.0 + cổng Clean Codebase (knip website, ruff F401/F841 + vulture cho `tools/`) trong PR `chore/standards-v5.1.0-phieu-ton`. Còn: màn Ủng hộ 13.2 (app hiện chỉ mở trang /ung-ho/), UI1 icon khay ≥ 3:1 + test, ngân sách tốc độ - xếp phase ở `logs/STATE.md`.
- 24/11/2026 - phiên tsudev-trafficmonitor (P8): **UI1 xong** - đo icon khay trên nền thanh tác vụ thật (Win11/Win10, Sáng/Tối, 16-64 px): icon mặc định taskbar Tối `notifyicon.ico` đường bao 0% đạt 3:1 (ô xanh đậm chìm vào nền) ⇒ thêm viền màu nhấn `#3FB6F5` ⇒ 91-99%; `notifyicon3.ico` viền lùi vào trong ⇒ dải tối lọt ra mép (27-62%) ⇒ viền sát mép ⇒ 97-100%; icon đơn sắc 100%. Test chặn tái phát `tools/check_tray_icons.py` (job Kiểm tra Việt hóa). Màn Ủng hộ 13.2 + đăng nhập QR: phase riêng cùng đăng nhập OIDC (ADR-0002 mục 2.5) - app chưa có tài khoản.
