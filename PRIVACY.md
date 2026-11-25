**Tiếng Việt | [English](./PRIVACY_en-us.md)**

# Quyền riêng tư và chế độ ngoại tuyến - Tsu Tramon

Tsu Tramon chạy đủ mọi tính năng **mà không cần tài khoản và không cần mạng**. Trang này liệt kê đầy đủ mọi kết nối
mạng mà ứng dụng có thể mở, khi nào mở và gửi đi những gì - để bạn tự kiểm chứng. Chính sách quyền riêng tư chung của
hệ sinh thái tsudev: [tsudev.com/privacy](https://tsudev.com/privacy).

## Tóm tắt

* **Chưa đăng nhập tài khoản tsudev** (mặc định): ứng dụng **không gửi dữ liệu nào của bạn** lên mạng. Cấu hình và lịch
  sử lưu lượng chỉ nằm trên máy (`%AppData%\tsutramon` hoặc thư mục chương trình). Không đo đếm (telemetry), không
  quảng cáo, không mã định danh gửi đi.
* **Chế độ ngoại tuyến hoàn toàn** (Cài đặt → Tài khoản tsudev): ứng dụng **không tự kết nối ra ngoài** - không kiểm
  tra bản cập nhật, không lấy IP công cộng, không đăng nhập hay làm mới TsuDEV Pro. Plugin bên thứ ba tự quản lý kết
  nối của chúng.
* **Đã đăng nhập tài khoản tsudev** (tuỳ chọn, chỉ dùng cho TsuDEV Pro): ứng dụng chỉ nói chuyện với `tsudev.com` để
  xác minh tài khoản và quyền lợi Pro. Lưu lượng mạng, lịch sử, cấu hình của bạn **không** được gửi đi.

## Mọi kết nối của ứng dụng

| Kết nối | Khi nào | Gửi đi | Nhận về |
| --- | --- | --- | --- |
| GitHub: thông tin phiên bản có chữ ký (`version_signed.xml` + `.sig`) | Khi khởi động (tuỳ chọn "Tự động kiểm tra bản cập nhật mới") hoặc bấm "Kiểm tra phiên bản mới" | Không có dữ liệu của bạn (yêu cầu GET thông thường) | Số phiên bản, liên kết tải, SHA-256 - chỉ tin khi chữ ký hợp lệ |
| GitHub: gói cập nhật `.zip` | Chỉ khi bạn đồng ý cập nhật | Không | Gói zip, chỉ lưu khi SHA-256 khớp |
| `api.ipify.org`, `api6.ipify.org` | Chỉ khi bạn mở hộp thoại "Chi tiết mạng" | Không (dịch vụ thấy IP công cộng - đó chính là thứ được hỏi) | Địa chỉ IP công cộng, không lưu |
| `tsudev.com` (đăng nhập OIDC, quyền lợi Pro) | **Chỉ khi đã đăng nhập**: lúc đăng nhập, 15 giây sau khi khởi động rồi mỗi 12 giờ, khi bấm "Làm mới trạng thái", khi đăng xuất | Mã đăng nhập dùng một lần + PKCE, token phiên, **mã cài đặt** (128 bit ngẫu nhiên, đổi mới khi đăng xuất) | Token phiên, trạng thái TsuDEV Pro có chữ ký số |
| Trang web (Trợ giúp, Nhật ký, Ủng hộ, Kho giao diện) | Chỉ khi bạn bấm liên kết | Mở trình duyệt mặc định của bạn | - |

**Chế độ ngoại tuyến hoàn toàn chặn mọi dòng trên** (trừ việc bạn tự bấm liên kết mở trình duyệt).

## Dữ liệu tài khoản lưu trên máy

Khi đăng nhập, ứng dụng lưu trên máy bạn:

* Phiên đăng nhập (refresh token) trong **Windows Credential Manager** (mục `tsudev/tsutramon/refresh_token`), được
  Windows mã hoá theo tài khoản người dùng.
* Trạng thái TsuDEV Pro đã ký số (`tsudev_entitlement.jws`) và khoá công khai của tsudev.com (`tsudev_jwks.json`)
  trong thư mục cấu hình, để ứng dụng biết trạng thái Pro cả khi mất mạng.
* Mã cài đặt trong `config.ini` (mục `[tsudev_account]`).

**Cài đặt → Tài khoản tsudev → Đăng xuất và xóa dữ liệu cục bộ** thu hồi phiên trên tsudev.com rồi xoá cả ba thứ
trên và đổi mã cài đặt mới. Cấu hình và lịch sử lưu lượng của Tsu Tramon không bị ảnh hưởng.

## Kiểm chứng

Mã nguồn mở tại [tsudev-org/tsutramon](https://github.com/tsudev-org/tsutramon): danh sách kết nối ở trên được kiểm kê
trong [ADR-0002](./docs/adr/0002-tsudev-pro-ban-mien-phi-cuc-bo.md); việc chưa đăng nhập hoặc bật Chế độ ngoại tuyến
hoàn toàn thì không gọi `tsudev.com` có kiểm thử tự động (`tests/ProAccountTests.cpp`). Cách kiểm bản tải về là bản
thật: [Bảo mật](./SECURITY.md).
