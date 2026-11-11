# Thương hiệu tsudev-trafficmonitor

Một ảnh gốc, một script, mọi logo/icon/favicon sinh ra từ đó. **Không sửa tay** sản phẩm sinh ra.

| Tệp | Là gì |
| --- | --- |
| `source/logo-tsudev-trafficmonitor.png` | **Ảnh gốc** (1254x1254, nền đen). Nguồn duy nhất. |
| `brand.json` | Cấu hình: tham số tách nền, ô cắt dấu hiệu thu gọn, màu nền đặc |
| `fonts/BeVietnamPro-*.ttf` | Phông vẽ chữ lên og-image và ảnh hộp thoại Giới thiệu (SIL OFL 1.1, `fonts/OFL.txt`) |
| `logo-tsudev-trafficmonitor.{png,webp,avif}` | Logo đầy đủ, **nền trong suốt**, 1024x1024 (sinh ra) |
| `mark-tsudev-trafficmonitor.{png,webp,avif}` | Dấu hiệu thu gọn: mặt cú cắt từ logo, không chữ, ô bo góc 512x512 (sinh ra) |

## Sinh lại

```bash
python3 -m venv .venv && .venv/bin/pip install "pillow>=11.3" numpy scipy
.venv/bin/python tools/brand/build_brand.py            # ~2 phút (nén PNG/AVIF mức cao)
.venv/bin/python tools/brand/build_brand.py --check    # chỉ kiểm, không ghi
# Skin mang logo và dấu hiệu nhỏ trên skin đồ thị (cần phông Segoe UI cho ảnh xem trước):
cd tools/skin_assets && ../../.venv/bin/python make_skins.py --font /mnt/c/Windows/Fonts/segoeui.ttf --only tsudev_Logo \
  && ../../.venv/bin/python make_skins_dothi.py --font /mnt/c/Windows/Fonts/segoeui.ttf
```

Sản phẩm: icon ứng dụng `TrafficMonitor/res/TrafficMonitor.ico` (16-256; <= 40 px dùng dấu hiệu thu gọn, <= 20 px ô cắt
chặt hơn cho mắt to), ảnh đầu hộp thoại Giới thiệu `about_background_hd.bmp`, và trong `website/public/`: `favicon.ico`
(16-256), `favicon-{16,32,96}x*.png`, `apple-touch-icon.png` (180, nền đặc), `android-chrome[-maskable]-{192,512}x*.png`
(maskable chừa vùng an toàn 80%), `og-image.png` (1200x630), `brand/{logo,mark}-tsudev-trafficmonitor.{png,webp,avif}`.

## Đổi logo

1. Thay `source/logo-tsudev-trafficmonitor.png` (vuông, cạnh >= 1024 px, nền đen đặc hoặc trong suốt).
2. Mở ảnh, đo lại `mark_box` / `mark_small_box` trong `brand.json` (ô vuông quanh mặt cú, không dính chữ).
3. Chạy các lệnh ở trên, xem lại icon ở 16/32/48 px và og-image, rồi commit cả ảnh gốc lẫn sản phẩm.

## Vì sao không có SVG

Logo là ảnh minh họa có quầng sáng và chuyển màu; dò nét ra SVG làm mất chi tiết rõ rệt. Thay vào đó website dùng
`<picture>` AVIF -> WebP -> PNG (component `website/src/components/AnhThuongHieu.astro`), mọi cỡ icon thu nhỏ bằng LANCZOS
từ bản gốc kèm làm nét nhẹ ở cỡ <= 48 px, nên hiển thị sắc nét trên màn hình mật độ cao.

## Quy ước

Lệch BRAND_ASSETS (logo sản phẩm riêng thay dấu hiệu cú chung) - ghi ở `AGENTS.md` mục B.4.
