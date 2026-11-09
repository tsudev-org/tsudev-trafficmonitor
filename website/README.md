# Website trafficmonitor.tsudev.com

Trang sản phẩm của TrafficMonitor-VN: [Astro](https://astro.build) dựng trang tĩnh, **không có JavaScript phía trình duyệt**,
host trên Cloudflare Pages (dự án `tsudev-trafficmonitor`).

* Nội dung tài liệu lấy trực tiếp từ các tệp `.md` ở gốc repo (`Help.md`, `KhoGiaoDien.md`, `SECURITY.md`...), ảnh từ `Screenshots/`
  - sửa tài liệu ở đó, không sửa trong `website/`.
* Thông tin bản phát hành (phiên bản, liên kết, SHA-256) lấy từ GitHub Releases lúc build.
* Tài sản thương hiệu (`public/favicon*`, `og-image.png`, `brand/`) sinh bằng `scripts/brand_assets.py` từ dây chuyền thương hiệu
  của repo `tsudev-tsudev/tsudev`; không sửa tay.
* Font Be Vietnam Pro tự host (SIL OFL 1.1), token màu sao nguyên văn từ `tsudev-standards/tokens/tokens.css`.
* Header bảo mật (HSTS, CSP chặt, `X-Frame-Options: DENY`...) ở `public/_headers`; `security.txt` ở `public/.well-known/`.
  `scripts/check-dist.mjs` chạy sau mỗi lần build để kiểm tra title, `og:site_name`, canonical, độ dài mô tả và chặn CSS/JS nội tuyến.

```bash
cd website
npm ci
npm run dev       # xem thử ở http://localhost:4321
npm run build     # dựng vào dist/ và kiểm tra
npm run check:a11y   # axe-core WCAG 2.2 AA mọi trang, sáng + tối (lần đầu: npx playwright install chromium)
npm run deploy    # build rồi đưa lên Cloudflare Pages (cần `npx wrangler login` với tài khoản tsudev)
```

Sau mỗi lần phát hành bản mới, chạy lại `npm run deploy` để trang cập nhật phiên bản và mã SHA-256.
