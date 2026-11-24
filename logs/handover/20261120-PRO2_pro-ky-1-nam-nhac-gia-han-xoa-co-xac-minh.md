# 20261120-PRO2 - tsudev-trafficmonitor: TsuDEV Pro kỳ 1 năm, nhắc gia hạn 7 ngày, xoá có xác minh

- **Người lập**: phiên điều phối `projects-a6`, 20/11/2026 - phiếu mẹ `~/projects/logs/handover/20261120-PRO2_pro-ky-1-nam-xoa-co-xac-minh-toan-he.md`
- **Hợp đồng**: `tsudev-org/tsudev-standards` **v4.0.0** - `docs/PRODUCT_POLICY.md` v1.2.0 mục 3.3, 7.2, 7.4, 7.5,
  **7.6**, 8. Lệch giữa phiếu và PRODUCT_POLICY ⇒ PRODUCT_POLICY thắng.
- **Thay phần tương ứng** của phiếu `20261119-PRO1` trong repo này (giá tháng/năm, mã `thang`, "tự gia hạn").
- **Người làm**: phiên tsudev-trafficmonitor

## 1. Bối cảnh (chủ dự án 20/11/2026)

- Pro áp dụng **1 năm**, **không tự gia hạn**; hết kỳ mà không gia hạn ⇒ về **trạng thái miễn phí ("khách mời")**:
  dùng đủ tính năng cục bộ, KHÔNG đồng bộ/sao lưu lên đám mây; dữ liệu trên thiết bị giữ nguyên; bản sao đám mây kéo/xuất
  được 90 ngày rồi bị xoá **có xác minh**.
- 7 ngày cuối trước khi hết hạn: **nhắc gia hạn mỗi lần mở app**; hết kỳ: báo **một lần**.
- Lỗi cần chặn: người **gia hạn trên web mà không mở app suốt 90 ngày** bị máy chủ xoá nhầm bản sao đám mây (máy chủ chỉ
  biết `pro_until` cũ; kênh RP `GET /api/rp/entitlements/{sub}` trả 404 khi grant OIDC quá 30 ngày).

## 2. Việc phải làm

### 2.1. Nhắc gia hạn + báo hết kỳ (PRODUCT_POLICY 7.6 - nội dung nguyên văn ở đó)

- Nhắc khi 0 < `pro_until` - bây giờ ≤ 7 ngày: **mỗi lần mở app** (khởi động mới, hoặc quay lại sau ≥ 30 phút ở nền),
  tối đa một lần mỗi lần mở; đóng được, không chặn, không hiện giữa lúc nhập liệu. Nút "Gia hạn" theo kênh (Play/App Store
  ⇒ IAP gói `nam`; ngoài cửa hàng ⇒ `tsudev.com/pro?app=<client_id>`; bản Play `MUST NOT` dẫn ra web).
- Báo hết kỳ **một lần** cho mỗi `pro_until` (khoá cục bộ `tsu.pro_expired.notified_until`), kèm ngày xoá đám mây
  (`pro_until` + 90 ngày; bỏ câu này nếu sản phẩm không có đồng bộ đám mây).
- Nguồn `pro_until` = JWS quyền lợi còn hạn; không có JWS còn hạn ⇒ không hiện gì, không lỗi.
- Desktop C++/MFC: hộp thoại gốc (chuỗi vi/en trong tài nguyên, không viết cứng), đúng nội dung + điều kiện 7.6; nút Gia hạn mở `https://tsudev.com/pro?app=<client_id>`.
- **6 test bắt buộc**: còn 8 ngày ⇒ không; còn 7 ngày ⇒ có; mở 2 lần ⇒ 2 lần, quay lại < 30 phút ⇒ không; hết hạn ⇒ báo
  1 lần, mở lại ⇒ không; gia hạn (JWS mới) ⇒ thôi nhắc, kỳ sau báo lại; ngoại tuyến JWS hết hạn ⇒ không, không lỗi.

### 2.2. Đồng bộ qua TsuDEV Sync (khi có thư viện máy khách: C++ gọi HTTP trực tiếp theo hợp đồng tsusync ADR-0001/0002 (hoặc thư viện khi có))

- Không tự viết logic xoá phía máy chủ - TsuDEV Sync (`tsudev-org/tsusync` v0.2.0) đã xoá có xác minh.
- Máy khách: gửi JWS mới (`iat` ≤ 24 giờ) - gặp `401 reason: "stale"` ⇒ lấy JWS mới từ `/api/me/entitlements` rồi thử
  lại **một lần**; gặp `402 pro_required` ⇒ hiện màn Pro, không thử lại; `507`/`429`/`5xx` ⇒ "Chưa đồng bộ", app vẫn
  chạy cục bộ. Khi thư viện hỗ trợ DPoP thì bật (tsusync ADR-0002 mục 3.2).
- Hết kỳ ⇒ tắt đẩy lên, vẫn cho "Tải bản sao đám mây về" trong 90 ngày (dùng `read_until` của `GET /v1/account`).

## 3. Điều kiện xong

- [ ] Các mục ở phần 2 xong, test xanh, CI xanh; sync bộ quy ước **v4.0.0** trong cùng PR (thay v3.9.1 của PRO1).
- [ ] Checklist PRODUCT_POLICY mục 8 (các dòng Pro) đạt; số liệu/ảnh chụp nhắc gia hạn vi/en Sáng/Tối trong PR.
- [ ] Việc cần chủ dự án (Play Console, App Store Connect, deploy production, merge bị bộ phân loại chặn): ghi rõ lệnh
  `!` cho chủ dự án, không lách.

## 4. Không làm

- Không thêm tự gia hạn, gói tháng, dùng thử miễn phí, popup nhắc Ủng hộ (chỉ nhắc Pro đúng điều kiện 7.6).
- Không xoá dữ liệu trên thiết bị trong bất kỳ trường hợp nào.

## 5. Kết quả xử lý (mỗi phiên ghi một dòng)

