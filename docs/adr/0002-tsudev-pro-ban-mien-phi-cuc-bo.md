# ADR-0002: TsuDEV Pro cho Tsu Tramon - bản miễn phí chạy cục bộ trọn vẹn, Pro chỉ thêm đồng bộ đám mây

- **Trạng thái**: ĐÃ CHẤP NHẬN (khung chung do chủ dự án chốt 19-20/11/2026 ở PRODUCT_POLICY mục 3, 7; phần riêng của
  sản phẩm do agent chốt theo uỷ quyền phiếu `20261124-UQ1`)
- **Ngày**: 24/11/2026
- **Người quyết định**: chủ dự án (gói, giá, kênh thanh toán - hợp đồng chung); phiên tsutramon (phạm vi Pro của sản phẩm)
- **Liên quan**: phiếu `logs/handover/20261119-PRO1_luu-cuc-bo-pro-dong-bo.md`, `20261120-PRO2_pro-ky-1-nam-nhac-gia-han-xoa-co-xac-minh.md`;
  `.standards/docs/PRODUCT_POLICY.md` mục 3, 7.1-7.6; `.standards/docs/AUTH_AND_ACCOUNT.md`; ADR-0001

## 1. Bối cảnh

PRODUCT_POLICY mục 7 (v5.1.0): miễn phí = không gửi dữ liệu người dùng lên mạng (N2); Pro = đồng bộ + sao lưu đám mây
(N3), kiểm quyền ở máy chủ đồng bộ (N4); một gói TsuDEV Pro chung kỳ 1 năm, không tự gia hạn (7.2); nhắc gia hạn 7 ngày
cuối và báo hết kỳ một lần (7.6). Bước 1 của phiếu PRO1 là kiểm kê dữ liệu rời thiết bị.

### 1.1. Kiểm kê (đọc mã 24/11/2026, bản 26.11.2401)

| Kết nối | Khi nào | Gửi gì đi | Nhận gì | Ghi chú |
| --- | --- | --- | --- | --- |
| `github.com/.../releases/latest/download/version_signed.xml` + `.sig`, `raw.githubusercontent.com/.../version_utf8.info` | Khởi động (tuỳ chọn "Kiểm tra cập nhật khi khởi động", mặc định bật) hoặc bấm "Kiểm tra cập nhật" | Không có dữ liệu người dùng (yêu cầu GET, User-Agent của WinINet) | Manifest phiên bản đã ký | `ForkConfig.h`; chữ ký ECDSA, SHA-256 |
| Link tải gói `github.com/.../releases/download/...` | Người dùng đồng ý cập nhật | Không | Gói zip (kiểm SHA-256 với manifest) | `UpdateDownload.h` |
| `api.ipify.org`, `api6.ipify.org` | CHỈ khi người dùng mở hộp thoại "Thông tin kết nối mạng" | Không (dịch vụ thấy IP công cộng - đó chính là thứ được hỏi) | Địa chỉ IP công cộng | Không gửi vị trí, không lưu |
| `tramon.tsudev.com`, GitHub (Trợ giúp, Nhật ký, Ủng hộ, Kho giao diện) | Người dùng bấm liên kết | Mở trình duyệt mặc định | - | Ứng dụng không tự gọi |
| `tsudev.com/oidc/token`, `/oidc/jwks`, `/oidc/token/revocation`, `/api/me/entitlements` (từ P9, `ProAccount.cpp`) | CHỈ khi đã đăng nhập tài khoản tsudev: lượt đăng nhập, 15 giây sau khi khởi động rồi mỗi 12 giờ, bấm "Làm mới trạng thái", đăng xuất | Mã đăng nhập + PKCE, refresh token, access token, mã cài đặt (`X-Tsu-Installation`, 128 bit ngẫu nhiên) | Token, JWKS, JWS quyền lợi | Chưa đăng nhập hoặc bật **Chế độ ngoại tuyến hoàn toàn** ⇒ không gọi (có test `tests/ProAccountTests.cpp`); công tắc này chặn cả mọi dòng trên |

Dữ liệu người dùng (cấu hình `config.ini`, lịch sử lưu lượng `history_traffic.dat`, skin, plugin) chỉ nằm ở
`%AppData%\tsutramon` (hoặc thư mục chương trình ở chế độ di động) - mức D1, **không** có đường gửi lên mạng nào. Ứng dụng
không có đăng nhập, không mã cài đặt, không đo đếm (telemetry), không quảng cáo.

## 2. Quyết định

1. **Bản miễn phí giữ MỌI tính năng hiện có** (giám sát, cửa sổ nổi, thanh tác vụ, lịch sử, skin, plugin, thu vào mép,
   cập nhật). Không tính năng nào đang có chuyển sang Pro ⇒ không cần ân hạn cho người dùng cũ.
2. **Phần Pro duy nhất của sản phẩm** (khi làm): đồng bộ cấu hình + xem lịch sử lưu lượng của nhiều máy cùng tài khoản qua
   TsuDEV Sync (`tsudev-org/tsusync`), mã hoá đầu cuối, máy chủ kiểm JWS quyền lợi (N3, N4). Miễn phí vẫn có sao lưu/khôi
   phục **ra tệp** (thư mục cấu hình mở được từ Cài đặt; xuất lịch sử CSV khi làm phần đồng bộ - N2).
3. **Kênh thanh toán**: bản tải trực tiếp/winget/Scoop ⇒ nút "Gia hạn"/"Nâng cấp" mở `https://tsudev.com/pro?app=<client_id>`
   (không nhúng cổng thanh toán); Microsoft Store (nếu lên) dùng cùng trang web (PRODUCT_POLICY 3.3). Ủng hộ: trang
   `tramon.tsudev.com/ung-ho/` (VietQR) như hiện tại.
4. **Nhắc gia hạn / báo hết kỳ (7.6)**: logic thuần `tsutramon/ProRenewal.h` + 6 ca bắt buộc ở
   `tests/ProRenewalTests.cpp` (job "Kiểm thử C++", có ASan). Chưa có nguồn JWS nên `Evaluate()` luôn trả `None` ở bản
   hiện tại; hộp thoại vi/en (chuỗi trong tài nguyên, nội dung nguyên văn 7.6, nút Gia hạn mở trang ở mục 3) nối cùng lúc
   với đăng nhập - KHÔNG nối hộp thoại khi chưa thể hiện được nó (tránh mã chết trong giao diện).
5. **Đăng nhập + JWS + đồng bộ** làm ở một phase riêng khi có: client OIDC đăng ký cho sản phẩm trên tsudev.com
   (`AUTH_AND_ACCOUNT.md`, mã cài đặt 128 bit), thư viện/hợp đồng tsusync cho C++ (HTTP trực tiếp theo ADR-0001/0002 của
   tsusync), và đo phản hồi < 100 ms của thao tác chính.

## 3. Hệ quả

**Tích cực:** đúng N2 ngay từ bây giờ (không có gì phải tắt); người dùng hiện tại không mất gì; điều kiện nhắc gia hạn đã
khoá bằng test trước khi có giao diện.

**Tiêu cực:** chưa bán được Pro trong sản phẩm này tới khi có đăng nhập; huy hiệu Pro/Ủng hộ (PRODUCT_POLICY mục 4) chưa
hiển thị được vì cần tài khoản.

**Việc phát sinh:**
- [x] Kiểm kê (mục 1.1), ADR này, `ProRenewal.h` + test (P8, 24/11/2026)
- [x] Phase đăng nhập OIDC + JWS quyền lợi + hộp thoại 7.6 (P9, 25/11/2026: `ProAuth.h`, `ProNet.h`, `ProAccount.cpp`, thẻ Cài đặt "Tài khoản tsudev", Chế độ ngoại tuyến hoàn toàn)
- [x] Màn Pro/Ủng hộ 13.2, đăng nhập QR (QR1) (25/11/2026, `SignInDlg`): màn đăng nhập bản địa theo khung 13.2 (Google/GitHub
  qua `idp_hint`, email, khoá truy cập, mã QR - QR và khoá truy cập do trang tsudev.com làm theo CROSS_DEVICE_LOGIN mục 2,
  app không tự dựng QR), ô Điều khoản không tích sẵn + bảng đồng ý 13.4, "Dùng ngay, không cần tài khoản"; màn TsuDEV
  Pro/Ủng hộ cùng khung (mua/gia hạn trên web, ủng hộ không cần tài khoản). Lệch: không có ô chọn ngôn ngữ trên màn
  (ngôn ngữ theo Cài đặt của app), nền theo hộp thoại Windows (app chưa có chế độ Tối cho hộp thoại)
- [ ] Phase đồng bộ qua TsuDEV Sync + xuất lịch sử CSV + thông báo nhiều thiết bị (7.3, 5 test bắt buộc)
