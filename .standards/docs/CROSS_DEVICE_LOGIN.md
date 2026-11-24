# ĐĂNG NHẬP CHÉO THIẾT BỊ BẰNG MÃ QR - mọi sản phẩm tsudev hạng A, B (v1.0.0)

> Chuẩn bắt buộc cho tính năng "Quét để đăng nhập": dùng một thiết bị **đã đăng nhập** để đăng nhập thiết bị **mới**,
> bằng mã QR cộng một **mã khớp** gõ tay. Áp dụng cho web, desktop và app di động có tài khoản tsudev
> ([`AUTH_AND_ACCOUNT.md`](AUTH_AND_ACCOUNT.md) mục 1, hạng A và B; hạng C `MAY`).
>
> Căn cứ thiết kế: RFC 8628 (Device Authorization Grant), RFC 9449 (DPoP), bản nháp IETF
> *Cross-Device Flows: Security Best Current Practice* (`draft-ietf-oauth-cross-device-security`), các mẫu tấn công
> QRLJacking của OWASP. Chủ dự án chốt yêu cầu ngày 24/11/2026 (phiếu `20261124-QR1`).

## Mục lục

1. [Hai luồng và hai vai](#1-hai-luồng-và-hai-vai)
2. [Kiến trúc: chỉ IdP làm](#2-kiến-trúc-chỉ-idp-làm)
3. [Mô hình đe doạ](#3-mô-hình-đe-doạ)
4. [Giao thức](#4-giao-thức)
5. [Mật mã và giới hạn](#5-mật-mã-và-giới-hạn)
6. [Giao diện bắt buộc](#6-giao-diện-bắt-buộc)
7. [API của IdP](#7-api-của-idp)
8. [Kiểm thử bắt buộc](#8-kiểm-thử-bắt-buộc)
9. [Checklist nghiệm thu](#9-checklist-nghiệm-thu)

---

## 1. Hai luồng và hai vai

| Vai | Là ai | Việc |
| --- | --- | --- |
| **Thiết bị xác nhận** (approver) | Thiết bị **đã đăng nhập** tài khoản tsudev | Bấm Cho phép, **hiển thị mã khớp** |
| **Thiết bị mới** (requester) | Thiết bị muốn đăng nhập | **Gõ mã khớp**, nhận phiên |

| Luồng | Ai hiện QR | Ai quét | Ví dụ |
| --- | --- | --- | --- |
| **Đ1 - Đẩy** | Thiết bị xác nhận (PC đã đăng nhập, mục "Đăng nhập trên điện thoại") | Thiết bị mới (app điện thoại, nút "Quét để đăng nhập" ở màn đăng nhập) | Đang dùng tsudev.com trên PC, muốn vào app Quly Thu Chi trên điện thoại |
| **Đ2 - Kéo** | Thiết bị mới (màn đăng nhập web/desktop, khung "Mã QR") | Thiết bị xác nhận (app điện thoại đã đăng nhập, "Quét QR") | Điện thoại đã đăng nhập, muốn vào TsuFiles trên PC |

Quy tắc chung của cả hai luồng, `MUST`:

- Mã khớp **luôn hiện trên thiết bị xác nhận** và **luôn gõ trên thiết bị mới**. Người cầm thiết bị mới phải
  nhìn thấy màn hình của thiết bị đã đăng nhập - đây là bằng chứng "cùng một người, ngồi cùng một chỗ".
- Thiết bị xác nhận **phải bấm Cho phép** sau khi xem thông tin thiết bị mới (mục 6.3), TRƯỚC khi mã khớp hiện ra.
- Kết quả của luồng là một **mã uỷ quyền OIDC** cấp cho đúng `client_id` + `code_challenge` của thiết bị mới. Thiết bị
  mới đổi mã ở token endpoint như mọi luồng OIDC khác ([`AUTH_AND_ACCOUNT.md`](AUTH_AND_ACCOUNT.md) mục 4). QR
  **không bao giờ** chở token, mật khẩu, khoá E2EE hay dữ liệu tài khoản.

## 2. Kiến trúc: chỉ IdP làm

```
 Thiết bị mới (RP: app/desktop/web)          tsudev.com (IdP)                 Thiết bị xác nhận
 ---------------------------------          ----------------                 -----------------
 PKCE + khoá DPoP tạm  ----- tạo/nhận ----> bảng xd_sessions (chỉ băm) <---- phiên tsudev + DPoP
                       <---- thăm dò ------  trạng thái, mã khớp (băm)  ---- quét / Cho phép
 gõ mã khớp ---------- verify -----------> cấp authorization code  ----> /token như OIDC thường
```

- `MUST` chỉ một nơi cài đặt: IdP `tsudev.com`. RP `MUST NOT` tự dựng phiên QR, bảng phiên, hay mã khớp riêng; RP chỉ
  gọi API mục 7 qua gói dùng chung (`tsudev_ui` cho Flutter, `@tsudev/auth` cho web/desktop JS, crate Rust nếu có).
- Màn xác nhận trên PC (Đ1) `MUST` là trang của IdP (`https://tsudev.com/tai-khoan/thiet-bi/qr`), mở bằng trình duyệt
  hệ thống, vì phiên đăng nhập PC đáng tin nằm ở IdP.
- Màn đăng nhập web/desktop của RP hiện QR (Đ2) `MUST` là trang IdP (`/dang-nhap`, khung "Mã QR") trong chính luồng
  `/authorize` - RP không phải sửa gì ngoài việc bật tham số `prompt=login` như cũ.
- Lưu trạng thái: một bảng Postgres (Neon) `xd_sessions` + cron dọn mỗi 10 phút. Thăm dò bằng HTTP (`interval` như
  RFC 8628); `MUST NOT` dùng dịch vụ đẩy trả phí - 0 đồng theo [`FREE_TIER_STACK.md`](FREE_TIER_STACK.md).

## 3. Mô hình đe doạ

| # | Tấn công | Ví dụ | Chặn bằng (`MUST`) |
| --- | --- | --- | --- |
| T1 | **QRLJacking** (lừa nạn nhân quét QR của kẻ tấn công) | Trang giả "quét để nhận quà" nhúng QR Đ2 thật lấy từ máy kẻ tấn công | (a) Màn Cho phép hiện app, thiết bị, trình duyệt, **thành phố/quốc gia theo IP** của thiết bị mới + cảnh báo cố định mục 6.3; (b) khác quốc gia với thiết bị xác nhận ⇒ từ chối; (c) mã khớp phải gõ trên thiết bị mới - nạn nhân không ngồi trước máy kẻ tấn công nên không có chỗ gõ; trang giả xin "nhập mã vào đây" bị cảnh báo mục 6.3 chặn ("tsudev KHÔNG BAO GIỜ hỏi mã này qua trang/tin nhắn"); (d) thông báo + email "Đăng nhập mới bằng QR" kèm nút "Không phải tôi - đăng xuất ngay" |
| T2 | Nhìn trộm/chụp QR trên màn hình | Người đứng sau chụp QR Đ1 trên PC | QR chỉ là lời mời; vẫn cần Cho phép trên PC + mã khớp hiện trên PC sau khi quét. QR đổi mỗi 30 giây, sống tối đa 120 giây |
| T3 | **Nhầm tài khoản** (login CSRF) | Kẻ tấn công cho nạn nhân quét QR Đ1 từ PC của kẻ tấn công ⇒ app nạn nhân vào tài khoản kẻ tấn công, nhập dữ liệu thật vào đó | Thiết bị mới hiện email che (`ng***@gmail.com`) + ảnh đại diện của tài khoản sắp vào, nút "Đúng tài khoản của tôi" TRƯỚC khi nhận phiên |
| T4 | Đoán mã khớp | Script thử mã | 40 bit, sai **3 lần** ⇒ huỷ phiên; giới hạn theo IP + tài khoản (mục 5) |
| T5 | Chặn/ăn cắp kết quả | Bên thứ ba thăm dò thay thiết bị mới | `poll_token` 256 bit chỉ thiết bị mới có; mọi lệnh của thiết bị mới ký **DPoP** bằng khoá tạm sinh lúc tạo phiên; mã uỷ quyền gắn PKCE của chính nó |
| T6 | Phát lại | Dùng lại QR/mã cũ | Mỗi bí mật dùng một lần; trạng thái chỉ đi một chiều (mục 4.3); `jti` DPoP không lặp lại trong 5 phút |
| T7 | Rò cơ sở dữ liệu | Lộ bảng `xd_sessions` | Chỉ lưu **SHA-256** của bí mật QR, `poll_token`, mã khớp (bí mật ≥ 128 bit nên băm nhanh là đủ; mã khớp 40 bit băm **HMAC-SHA-256 với khoá máy chủ** để chặn dò ngược); bản ghi xoá sau 10 phút |
| T8 | Thiết bị xác nhận bị chiếm | Điện thoại mất | Cho phép QR đòi **mở khoá cục bộ** (sinh trắc/PIN của app) và phiên IdP còn mới (≤ 30 ngày, không bị hạn chế mục 9 AUTH); trang Phiên đăng nhập cho đăng xuất từ xa |
| T9 | QR độc giả mạo tên miền | QR trỏ `tsudev.com.evil.io` | Bộ quét trong app `MUST` so khớp **tuyệt đối** tiền tố `https://tsudev.com/x/`; sai ⇒ "Mã QR này không phải của tsudev", không mở liên kết |

Rủi ro còn lại, chấp nhận có ghi nhận: người dùng cố tình đọc mã khớp cho kẻ lừa đảo qua điện thoại. Chặn bằng câu cảnh
báo trên màn hiện mã (mục 6.3) và email sau đăng nhập; giống chuẩn OTP ngân hàng.

## 4. Giao thức

### 4.1. Đ2 - Kéo (thiết bị mới hiện QR)

1. Thiết bị mới (đang ở `/authorize` của IdP) sinh khoá DPoP tạm, gọi `POST /api/xd/sessions` `{flow:"pull", client_id,
   redirect_uri, code_challenge, device}` ⇒ nhận `{id, qr, poll_token, interval, expires_in}`.
2. Hiện QR chứa `https://tsudev.com/x/<id>#<k>` (`k` = bí mật QR; nằm sau `#` để không vào log máy chủ/proxy khi lỡ mở
   bằng camera thường).
3. Thiết bị xác nhận quét, gọi `POST /api/xd/sessions/<id>/scan` `{k}` (có access token + DPoP) ⇒ nhận thông tin thiết
   bị mới (mục 6.3).
4. Người dùng bấm **Cho phép** (sau mở khoá cục bộ) ⇒ `POST .../approve` ⇒ IdP sinh mã khớp, trả **chỉ cho** thiết bị
   xác nhận; thiết bị xác nhận hiện mã.
5. Thiết bị mới thăm dò `GET .../status` thấy `awaiting_code` ⇒ hiện ô "Nhập mã hiện trên điện thoại".
6. Người dùng gõ mã ⇒ `POST .../verify` `{code}` (DPoP) ⇒ IdP trả `authorization_code` + `state` như redirect OIDC.

### 4.2. Đ1 - Đẩy (thiết bị đã đăng nhập hiện QR)

1. Thiết bị xác nhận (trang IdP trên PC) gọi `POST /api/xd/sessions` `{flow:"push"}` ⇒ hiện QR.
2. Thiết bị mới (app điện thoại, màn đăng nhập ⇒ "Quét để đăng nhập") sinh PKCE + khoá DPoP tạm, quét, gọi
   `POST .../claim` `{k, client_id, redirect_uri, code_challenge, device}` ⇒ nhận `poll_token`.
3. PC hiện thông tin thiết bị mới (mục 6.3), người dùng bấm **Cho phép** ⇒ PC hiện mã khớp.
4. Thiết bị mới hiện tài khoản sắp vào (che email, T3) + ô nhập mã ⇒ `POST .../verify` ⇒ nhận `authorization_code`.

### 4.3. Máy trạng thái (một chiều)

```
created --scan/claim--> scanned --approve--> awaiting_code --verify ok--> completed
   \__________________\_____________\__ deny / hết hạn / sai 3 lần / huỷ __--> dead
```

Mọi chuyển trạng thái `MUST` là một câu `UPDATE ... WHERE state = <trạng thái cũ>` (so-và-đổi nguyên tử); hai thiết bị
cùng quét một QR ⇒ chỉ thiết bị đầu thắng, thiết bị sau nhận "Mã QR đã được dùng".

## 5. Mật mã và giới hạn

| Thông số | Giá trị `MUST` |
| --- | --- |
| `id` phiên | 128 bit ngẫu nhiên (CSPRNG), base64url |
| Bí mật QR `k`, `poll_token` | 256 bit ngẫu nhiên; lưu SHA-256; so bằng phép so thời gian hằng |
| Mã khớp | 8 ký tự Crockford Base32 bỏ `0 O 1 I L U` (≈ 40 bit), hiện `K7QF-M2XD`; không phân biệt hoa thường; lưu HMAC-SHA-256 (khoá `XD_CODE_PEPPER`, secret theo [`SECURITY_BASELINE.md`](SECURITY_BASELINE.md) mục 3) |
| Sống | QR: xoay mỗi 30 giây, phiên tối đa **120 giây** tới `awaiting_code`; mã khớp **60 giây**; bản ghi xoá sau 10 phút |
| Sai mã | tối đa **3 lần** ⇒ `dead` |
| Giới hạn tốc độ | tạo phiên: 10/phút/IP; quét/claim: 20/phút/tài khoản; verify: 5/phút/IP; thăm dò không nhanh hơn `interval` (mặc định 2 giây, vượt ⇒ `slow_down`) |
| DPoP | ES256, khoá tạm sinh trên thiết bị mới, `htm`/`htu`/`iat` ±60 giây, `jti` không lặp lại 5 phút; mã uỷ quyền và refresh token gắn `jkt` |
| `amr` trong `id_token` | `["qr", "mfa"]` - QR + mã khớp + mở khoá cục bộ trên thiết bị đã xác thực tính là hai yếu tố |
| Nhật ký | ghi `xd.created/scanned/approved/denied/verified/failed` vào nhật ký bảo mật của tài khoản (không ghi bí mật) |

## 6. Giao diện bắt buộc

### 6.1. Thiết bị mới - app điện thoại (Đ1)

- Màn đăng nhập ([`AUTH_AND_ACCOUNT.md`](AUTH_AND_ACCOUNT.md) mục 13.2) có liên kết **"Quét để đăng nhập"** ở cuối.
- Mở camera kèm dòng hướng dẫn: *"Trên máy tính đã đăng nhập, mở tsudev.com ⇒ Tài khoản ⇒ Đăng nhập trên điện thoại."*
- Xin quyền camera **lúc bấm**, không lúc mở app; từ chối quyền ⇒ hướng dẫn mở Cài đặt, không chặn lối vào khác.

### 6.2. Thiết bị mới - web/desktop (Đ2)

- Trang `/dang-nhap` của IdP: màn ≥ 960px hiện QR ở cột phải; màn hẹp hiện thẻ "Mã QR" cạnh "Email".
- Dưới QR: *"Mở app tsudev bất kỳ trên điện thoại đã đăng nhập ⇒ Quét QR."* + đồng hồ đếm ngược + nút làm mới.
- Sau khi được Cho phép: thay QR bằng ô nhập 8 ký tự (tự nhảy ô, dán được, `autocomplete="one-time-code"`).

### 6.3. Thiết bị xác nhận - màn Cho phép và màn hiện mã

Màn Cho phép `MUST` có, theo thứ tự: biểu tượng + tên sản phẩm (từ `client_id` đã đăng ký, không lấy chữ do thiết bị
mới gửi), loại thiết bị + hệ điều hành + trình duyệt, **thành phố, quốc gia** theo IP, thời điểm; câu cảnh báo cố
định:

> **Chỉ cho phép nếu bạn đang cầm cả hai thiết bị.** Nếu ai đó gửi mã QR này cho bạn qua tin nhắn, trang web hay điện
> thoại, hãy bấm Từ chối.

Hai nút cùng cỡ: **Từ chối** (mặc định focus) và **Cho phép**. Màn hiện mã khớp `MUST` có dòng:
*"Nhập mã này trên thiết bị mới. tsudev không bao giờ hỏi mã này qua điện thoại, tin nhắn hay trang web khác."*

### 6.4. Thông điệp chuẩn (dùng nguyên văn)

| Tình huống | Nội dung |
| --- | --- |
| QR hết hạn | `Mã QR đã hết hạn. Đang tạo mã mới...` |
| QR không phải của tsudev | `Mã QR này không phải của tsudev.` |
| QR đã dùng | `Mã QR này đã được dùng. Tạo mã mới trên thiết bị kia.` |
| Bị từ chối | `Yêu cầu đăng nhập đã bị từ chối trên thiết bị kia.` |
| Sai mã khớp | `Mã chưa đúng. Còn {n} lần thử.` |
| Khác quốc gia | `Không thể xác nhận: hai thiết bị đang ở hai quốc gia khác nhau.` |
| Thành công (email/thông báo) | `Tài khoản của bạn vừa đăng nhập trên {thiết bị} tại {thành phố} bằng mã QR. Không phải bạn? Đăng xuất ngay.` |

## 7. API của IdP

Tiền tố `/api/xd`, JSON, lỗi theo [`AUTH_AND_ACCOUNT.md`](AUTH_AND_ACCOUNT.md) mục 15. Công bố trong
`/.well-known/openid-configuration` khoá `tsudev_cross_device_endpoint`; RP `MUST` đọc từ đó.

| Phương thức | Đường dẫn | Ai gọi | Xác thực |
| --- | --- | --- | --- |
| `POST` | `/sessions` | Đ2: thiết bị mới; Đ1: thiết bị xác nhận | Đ2: DPoP tạm; Đ1: phiên IdP |
| `POST` | `/sessions/{id}/scan` | Đ2: thiết bị xác nhận | access token + DPoP |
| `POST` | `/sessions/{id}/claim` | Đ1: thiết bị mới | DPoP tạm |
| `GET` | `/sessions/{id}/status` | cả hai | `poll_token` (thiết bị mới) hoặc phiên (thiết bị xác nhận) |
| `POST` | `/sessions/{id}/approve` \| `/deny` | thiết bị xác nhận | access token/phiên + mở khoá cục bộ vừa xong (`auth_time` ≤ 60 giây với app) |
| `POST` | `/sessions/{id}/verify` | thiết bị mới | `poll_token` + DPoP |

## 8. Kiểm thử bắt buộc

- Đơn vị: máy trạng thái (mọi cặp chuyển sai bị từ chối), hết hạn, sai 3 lần, so thời gian hằng, xoay QR.
- Tranh chấp: 2 thiết bị cùng `scan`/`claim`, 2 lệnh `verify` song song ⇒ đúng một thành công.
- Bảo mật: thiếu/sai DPoP, `poll_token` sai, mã của phiên khác, QR khác tên miền, khác quốc gia, phát lại `jti`.
- E2E (Playwright + trình giả lập/adb): Đ1 và Đ2 trọn vẹn, cả Sáng và Tối, tiếng Việt và tiếng Anh.
- Đo: thời gian QR ⇒ đăng nhập xong ≤ 15 giây ở người dùng thật; thăm dò không vượt hạn mức miễn phí (ước tính trong PR).

## 9. Checklist nghiệm thu

- [ ] IdP có bảng `xd_sessions` chỉ lưu băm, cron dọn, API mục 7, công bố trong discovery.
- [ ] Mã khớp hiện trên thiết bị đã đăng nhập, gõ trên thiết bị mới - ở cả Đ1 và Đ2.
- [ ] Màn Cho phép có đủ thông tin + câu cảnh báo mục 6.3; Từ chối là nút mặc định.
- [ ] Thiết bị mới thấy tài khoản sắp vào (email che) trước khi nhận phiên (T3).
- [ ] DPoP + PKCE; `amr` có `qr`; email/thông báo sau đăng nhập; trang Phiên đăng nhập ghi "bằng mã QR".
- [ ] Mọi giới hạn mục 5 có test; mục 8 xanh trong CI.
- [ ] RP chỉ dùng gói dùng chung, không có bảng/mã QR riêng.
