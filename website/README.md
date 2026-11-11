# Website trafficmonitor.tsudev.com

Trang sản phẩm của TrafficMonitor-VN: [Astro](https://astro.build) dựng trang tĩnh; JavaScript phía trình duyệt chỉ có
`public/theme.js` (bộ chọn giao diện Sáng/Tối/Hệ thống ở chân trang, mặc định Tối, khóa `tsudev-theme` như tsudev.com;
nạp chặn trong `<head>`, CSP `script-src 'self'`, không script nội tuyến),
host trên Cloudflare Pages (dự án `tsudev-trafficmonitor`).

* Nội dung tài liệu lấy trực tiếp từ các tệp `.md` ở gốc repo (`Help.md`, `KhoGiaoDien.md`, `SECURITY.md`...), ảnh từ `Screenshots/`
  - sửa tài liệu ở đó, không sửa trong `website/`.
* Thông tin bản phát hành (phiên bản, liên kết, SHA-256) lấy từ GitHub Releases lúc build.
* Tài sản thương hiệu (`public/favicon*`, `apple-touch-icon.png`, `android-chrome-*`, `og-image.png`, `brand/`) sinh bằng
  `tools/brand/build_brand.py` từ ảnh gốc `assets/brand/source/`; không sửa tay. Trang dùng `<AnhThuongHieu>` (AVIF -> WebP -> PNG).
* Font Be Vietnam Pro tự host (SIL OFL 1.1), token màu sao nguyên văn từ `tsudev-standards/tokens/tokens.css`.
* Header bảo mật (HSTS, CSP chặt, `X-Frame-Options: DENY`...) ở `public/_headers`; `security.txt` ở `public/.well-known/`.
  `scripts/check-dist.mjs` chạy sau mỗi lần build để kiểm tra title, `og:site_name`, canonical, độ dài mô tả và chặn CSS/JS nội tuyến.

```bash
cd website
npm ci
npm run dev       # xem thử ở http://localhost:4321
npm run build     # dựng vào dist/ và kiểm tra
npm run check:a11y   # axe-core WCAG 2.2 AA mọi trang, Sáng + Tối (lần đầu: npx playwright install chromium)
npm run check:theme  # bộ chọn giao diện trong Chromium, với CSP của production
npm run deploy    # build rồi đưa lên Cloudflare Pages (cần `npx wrangler login` với tài khoản tsudev)
```

Sau mỗi lần phát hành bản mới, chạy lại `npm run deploy` để trang cập nhật phiên bản và mã SHA-256.
