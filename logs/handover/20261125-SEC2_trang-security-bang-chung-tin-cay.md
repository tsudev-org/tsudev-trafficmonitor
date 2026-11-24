# PHIẾU YÊU CẦU SEC2 - Trang `/security` với bằng chứng tin cậy TỰ KIỂM CHỨNG ĐƯỢC

- **Mã phiếu**: 20261125-SEC2 (bản sao cho repo) - **Từ**: phiên `tsudev-8d`, 25/11/2026, theo yêu cầu chủ dự án - **Trạng thái**: ĐÓNG 25/11/2026
- **Phiếu gốc (ĐỌC mục 0-2 TRƯỚC)**: `~/projects/logs/handover/20261125-SEC2_trang-security-bang-chung-tin-cay-toan-he.md`
- **Website của repo**: tramon.tsudev.com

## Việc của repo này

E4 security.txt (Contact security@tsudev.com, test Expires) · E5 SSL Labs · E6 Observatory · trang /security thật (mục 2 phiếu gốc) · E8 SignPath nếu giấy phép được chấp nhận · E1 KHÔNG làm được (Anti-996) ⇒ ghi lý do trên /security.

## Ràng buộc (tóm tắt phiếu gốc mục 0)

- TUYỆT ĐỐI không nhãn trang trí: mỗi mục có liên kết tới trang kết quả của bên thứ ba + ngày đo; không có ⇒ không hiện.
- Chỉ khai sự thật; điểm thấp hiện đúng số; không áp dụng ⇒ ghi lý do.
- Số liệu lấy từ tệp trong repo lúc build (không gọi API ngoài khi người xem mở trang).
- Biểu mẫu (Best Practices, SignPath, SSL Labs, Observatory, HSTS preload): mở Edge hồ sơ `tsudev-bp` theo phiếu gốc mục 0.4;
  đăng nhập/OTP/giấy tờ ⇒ 👤 tự nhập. Mỗi lúc một phiên dùng hồ sơ; đóng Edge khi xong.
- Làm trong PR kế tiếp của repo nếu nhỏ; xong ghi một dòng dưới đây + cập nhật hàng của repo ở bảng mục 3 phiếu gốc; `akb` theo chuẩn.

## Kết quả xử lý

- 25/11/2026 - tsudev-8d: mở phiếu.
- 25/11/2026 - tsudev-trafficmonitor (SES-20261125-0022-ee38): ĐO, chưa làm trang. Observatory A+ 140, 12/12 (quét 24/11 17:23Z);
  SSL Labs (publish=off) A+ trên 3/4 điểm cuối, điểm cuối IPv6 thứ 4 chưa xong; E10: PVR bật; HSTS header có preload,
  tsudev.com `unknown` (chưa nộp). E2 Scorecard KHÔNG CÓ ĐIỂM: `scorecard.yml` startup_failure từ 20/11 vì allowlist Actions
  của repo chỉ có `microsoft/setup-msbuild@*` ⇒ 👤 thêm `ossf/scorecard-action@*` (agent bị chặn đổi chính sách).
  Việc còn: `security.txt` thêm `Contact: mailto:security@tsudev.com` + `Policy` → `/security/`; `LIVE_URL` của
  `tools/check_security_txt.py` sang tramon.tsudev.com; `docs/security/trust.json` + trang `website/src/pages/security.astro`
  (đúng mục 2 phiếu gốc, giữ `/bao-mat/`), liên kết chân trang; E1/E8 ghi lý do Anti-996 không phải OSI.
- 25/11/2026 - tsudev-trafficmonitor: 👤 thêm `ossf/scorecard-action@*` vào allowlist ⇒ Scorecard chạy, công khai **7.5/10**;
  SSL Labs xong **A+ 4/4**. Trang `/security/` (dữ liệu `docs/security/trust.json`, kiểm lúc build `website/src/lib/tin-cay.ts`:
  liên kết https ngoài site, ngày đo ≤ 120 ngày), `security.txt` thêm `Contact: mailto:security@tsudev.com` + `Policy` → `/security/`,
  chân trang + sitemap, `check-dist.mjs` kiểm nội dung trang. E1/E8 ghi lý do (Anti-996 không phải OSI). Phiếu ĐÓNG sau khi gộp + deploy.
- 25/11/2026 - tsudev-trafficmonitor: #92 gộp + deploy, trang live (kiểm nội dung). `trust.json` cập nhật sang bản 26.11.2501. ĐÓNG.
